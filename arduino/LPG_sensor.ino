//  Title:        SmartHome - Smart LPG Sensor
//  Filename:     LPG_sensor.ino
//  Description:  Система "Умный дом". Скетч прошивки блока Smart LPG Sensor
//  Author:       Aleksandr Prilutskiy
//  Version:      0.1.0.0
//  Date:         10.04.2019
//  URL:          https://github.com/aleksandr-prilutskiy/SmartHome-SensorLPG
//
// Функции устройства:
// 1. Чтение показаний датчика углеводородных газов и отправка их брокеру MQTT
// 2. Чтение показаний датчика температуры и влажности и отправка их брокеру MQTT
// 3. Сигнализация о превышении критического уровня углеводородных газов
// 4. Контроль и настройка устройства через web-интерфейс
// 5. Сброс настроек устройства при удержании специальной кнопки
//
// Аппаратные средства:
//  WeMos Di mini (https://wiki.wemos.cc/products:d1:d1_mini)
//  Датчик углеводородных газов MQ-6
//  Датчик температуры и влажности DHT11
//
// Подключение:
//  pin D0 -> Reset Button
//  pin D1 -> Error LED (Red)
//  pin D2 -> Power LED (Green)
//  pin D3 -> WiFi LED (Blue)
//  pin D4 -> DHT11 (BuiltIn LED)
//  pin D5 -> Piezo buzzer
//  pin D6 -> NC
//  pin D7 -> NC
//  pin D8 -> NC
//  pin A0 -> MQ6

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <DHT.h>

// Константы настройки устройства:
const String        deviceName       = "Smart LPG Sensor";   // Название устройства
const String        deviceVersion    = "1.0.0";              // Версия прошивки устройства
const uint8_t       pinButtonReset   = D0;                   // Кнопка сброса настроек WiFi
const uint8_t       ledError         = D1;                   // Светодиод индикации ошибки
const uint8_t       ledPower         = D2;                   // Светодиод индикации работы устройства
const uint8_t       ledWiFi          = D3;                   // Светодиод активности WiFi
const uint8_t       pinDHT           = D4;                   // Датчик DHT-11
const uint8_t       pinBuzzer        = D5;                   // Пьезоизлучатель
const uint8_t       pinMQ6           = A0;                   // Датчик углеводородных газов (MQ6)
const uint16_t      sizeEEPROM       = 256;                  // Размер используемой EEPROM

// Переменные настройки устройства, хранящиеся в EEPROM:
      String        WiFiSSID         = "";                   // SSID точки доступа WiFi
      String        WiFiPassword     = "";                   // Пароль для подключения к точке доступа WiFi
      String        MQTT_Server      = "";                   // DNS-имя или IP-адрес брокера MQTT
      uint16_t      MQTT_Port        = 0;                    // Порт для подключения к брокеру MQTT
      String        MQTT_ID          = "";                   // ID образца (Instance) брокера MQTT
      String        MQTT_Login       = "";                   // Логин пользователя брокера MQTT
      String        MQTT_Password    = "";                   // Пароль пользователя брокера MQTT
      String        MQTT_Temperature = "";                   // Topic датчика температуры
      String        MQTT_Humidity    = "";                   // Topic датчика влажности
      String        MQTT_LPG         = "";                   // Topic датчика углеводородных газов
      uint16_t      alarmLPG         = 0;                    // Порог опасного значения углеводородных газов

// Прочие переменные:
      float         lastTemperature  = 0;                    // Текущее значение температуры
      float         lastHumidity     = 0;                    // Текущее значение влажности
      float         lastLPG          = 0;                    // Текущее значение углеводородных газов
      String        errorStr         = "";                   // Строка с сообщением об ошибке
      bool          connectMQTT      = false;                // Признак подключения к брокеру MQTT
DHT                 dht11(pinDHT, DHT11);                    // Переменная для работы с датчиком DHT-11
ESP8266WebServer    WebServer(80);                           // Объект для работы с Web-сервером
WiFiClient          espClient;
PubSubClient        client(espClient);

// #FUNCTION# ===================================================================================================
// Name...........: setup
// Description....: Перврначальная настройка при запуске устройства
// Syntax.........: setup()
// ==============================================================================================================
void setup() {
 pinMode(ledError, OUTPUT);
 pinMode(ledPower, OUTPUT);
 pinMode(ledWiFi, OUTPUT);
 pinMode(pinBuzzer, OUTPUT);
 pinMode(pinButtonReset, INPUT);
 pinMode(pinMQ6, INPUT);
 digitalWrite(ledError, LOW);
 digitalWrite(ledPower, HIGH);
 digitalWrite(ledWiFi, LOW);
 digitalWrite(pinBuzzer, LOW);
 Serial.begin(115200);
 Serial.println();
 Serial.println("Start...");
 CheckReset();
 dht11.begin();
 EEPROMReadAll();
 WiFiSetup();
 WiFi.persistent(false);
 WebServer.on("/", webGetIndex);
 WebServer.on("/reset", webGetReset);
 WebServer.on("/setup", webGetSetup);
 WebServer.on("/update", webGetUpdate);
 WebServer.onNotFound(webNotFound);
 StartMessage();
} // setup

// #FUNCTION# ===================================================================================================
// Name...........: loop
// Description....: Основной цикл работы устройства
// Syntax.........: loop()
// ==============================================================================================================
void loop() {
 if (WiFi.status() != WL_CONNECTED) WiFiReconnect();
 if ((WiFiSSID.length() == 0) || (WiFi.status() == WL_CONNECTED)) WebServer.handleClient();
 ProbesReadDHT11();
 ProbesReadLPG();
 ProbesCheckLPG();
 ProbesSendData();
 delay(50);
} // loop

// #FUNCTION# ===================================================================================================
// Name...........: Reboot
// Description....: Перезагрузка устройства
// Syntax.........: Reboot()
// ==============================================================================================================
void(* Reboot) (void) = 0;

// #FUNCTION# ===================================================================================================
// Name...........: CheckReset
// Description....: Проверка нажатия кнопка сброса настроек и сброс настроек при ее удержании
// Syntax.........: CheckReset()
// ==============================================================================================================
void CheckReset() {
 if (digitalRead(pinButtonReset) == LOW) return;
 uint32_t timer = millis() + 3000;
 while (millis() < timer) {
  delay(200);
  digitalWrite(ledPower, LOW);
  delay(200);
  digitalWrite(ledPower, HIGH);
  if (digitalRead(pinButtonReset) == LOW) return;
 }
 Serial.println();
 Serial.println("Reset device:");
 EEPROM.begin(sizeEEPROM);
 for (int i = 0; i < sizeEEPROM; i++) EEPROM.write(i, 0);
 EEPROM.commit();
 Serial.println("Clear EPPROM ... OK");
 Serial.println("Reboot...");
 Serial.println();
 Reboot();
} // CheckReset

// #FUNCTION# ===================================================================================================
// Name...........: StartMessage
// Description....: Индикация и звуковой сигнал при запуске устройства
// Syntax.........: StartMessage()
// ==============================================================================================================
void StartMessage() {
 digitalWrite(ledPower, LOW);
 tone(pinBuzzer, 415, 250);
 delay(100);
 digitalWrite(ledPower, HIGH);
 tone(pinBuzzer, 466, 250);
 delay(100);
 digitalWrite(ledPower, LOW);
 tone(pinBuzzer, 370, 350);
 digitalWrite(ledPower, HIGH);
 tone(pinBuzzer, 370, 150);
 delay(200);
 noTone(pinBuzzer); 
 digitalWrite(ledPower, LOW);
 delay(350);
 digitalWrite(ledPower, HIGH);
} // StartMessage

