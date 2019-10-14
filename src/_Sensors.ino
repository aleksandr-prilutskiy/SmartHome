//  Filename:     _Probes.ino
//  Description:  Система "Умный дом". Блок Smart LPG Sensor. Функции для работы с датчиками
//  Author:       Aleksandr Prilutskiy
//  Date:         06.09.2019

      float         lastTemperature  =      0;               // Текущее значение температуры
      float         sumTemperature   =      0;               // Накопление значений температуры
      float         lastHumidity     =      0;               // Текущее значение влажности
      float         sumHumidity      =      0;               // Накопление значений влажности
      uint16_t      countDHT11       =      0;               // Счетчик значений датчика температуры и влажности
const uint32_t      timeoutDHT11     =   5000;               // Периодичность проверки температуры и влажности
      uint32_t      timerDHT11       =      0;               // Таймер проверки датчика температуры и влажности

      float         lastLPG          =      0;               // Текущее значение углеводородных газов
      float         sumLPG           =      0;               // Накопление значений датчика углеводородных газов
      uint16_t      countLPG         =      0;               // Счетчик значений датчика углеводородных газов
const uint32_t      timeoutLPG       =  15000;               // Периодичность проверки углеводородных газов
      uint32_t      timerLPG         =      0;               // Таймер проверки датчика углеводородных газов
      bool          flagAlarmLPG     =  false;               // Признак превышения уровня углеводородных газов

const uint32_t      timeoutAlarmBeep =   3000;               // Период повторения сигнала углеводородных газов
      uint32_t      timerAlarmBeep   =      0;               // Таймер сигнализации уровня углеводородных газов
      uint32_t      timerSendData    =      0;               // Таймер отправки результатов измерений
const uint32_t      timeoutSendData  =  30000;               // Периодичность отправки результатов измерений

      char          msg[50];

// #FUNCTION# ===================================================================================================
// Name...........: sensorsReadDHT11
// Description....: Получение и накопление значений температуры и влажности от датчика DHT-11
// Syntax.........: sensorsReadDHT11()
// ==============================================================================================================
void sensorsReadDHT11() {
 if (((timerDHT11 == 0) && (millis() < 60000)) || (abs(millis() - timerDHT11) < timeoutDHT11)) return;
 digitalWrite(ledPower, LOW);
 timerDHT11 = millis();
 float temperature = dht11.readTemperature();
 float humidity = dht11.readHumidity();
 delay(500);
 if (isnan(temperature) || isnan(humidity)) {
  digitalWrite(ledPower, HIGH);
  Serial.println("Error: Sensor DHT11 Read Error");
  for (int i = 0; i < 3; i++) {
   digitalWrite(ledError, HIGH);
   delay(250);
   digitalWrite(ledError, LOW);
   delay(250);
  }
  return;
 }
 lastTemperature = round(10 * temperature) / 10;
 lastHumidity    = round(10 * humidity) / 10;
 sumTemperature  = sumTemperature +  temperature;
 sumHumidity     = sumHumidity + humidity;
 countDHT11++;
 Serial.println((String) "Temperature = " + lastTemperature + "C");
 Serial.println((String) "Humidity = " + lastHumidity + "%");
 digitalWrite(ledPower, HIGH);
} // sensorsReadDHT11

// #FUNCTION# ===================================================================================================
// Name...........: sensorsReadLPG
// Description....: Получение и накопление значений датчика углеводородных газов от датчика MQ6
// Syntax.........: sensorsReadLPG()
// ==============================================================================================================
void sensorsReadLPG() {
 if (abs(millis() - timerLPG) < timeoutLPG) return;
 digitalWrite(ledPower, LOW);
 timerLPG = millis();
 float gaz = 3.3 * analogRead(pinMQ6) / 5;
 delay(250);
 digitalWrite(ledPower, HIGH);
 delay(250);
 digitalWrite(ledPower, LOW);
 delay(250);
 if (gaz == 0) {
  digitalWrite(ledPower, HIGH);
  Serial.println("Error: Sensor LPG Read Error");
  for (int i = 0; i < 5; i++) {
   digitalWrite(ledError, HIGH);
   delay(250);
   digitalWrite(ledError, LOW);
   delay(250);
  }
  return;
 }
 lastLPG = round(10 * gaz) / 10;
 sumLPG  = sumLPG + gaz;
 countLPG++;
 Serial.println((String) "LPG = " + lastLPG + " PPM");
 digitalWrite(ledPower, HIGH);
} // sensorsReadLPG

// #FUNCTION# ===================================================================================================
// Name...........: sensorsCheckLPG
// Description....: Проверка значений датчика углеводородных газов. Отправка данных на сервер "Умного дома" и
//                  звуковой сигнал в случае превышения допустимого уровня
// Syntax.........: sensorsCheckLPG()
// ==============================================================================================================
void sensorsCheckLPG() {
 digitalWrite(ledError, LOW);
 if ((alarmLPG == 0) || (lastLPG < alarmLPG)) {
  flagAlarmLPG = false;
  return;
 }
 digitalWrite(ledError, HIGH);
 if (!flagAlarmLPG) logSave(logAlarmLPG, (int)lastLPG);
 flagAlarmLPG = true;
 if (abs(millis() - timerAlarmBeep) < timeoutAlarmBeep) return;
 timerAlarmBeep = millis();
 tone(pinBuzzer, 2500);
 Serial.print("WARNING! LPG = ");
 Serial.print(lastLPG);
 Serial.println(" PPM");
 if (!client.connected()) MQTTReconnect();
 if (!client.connected()) return;
 snprintf(msg, 50, "%f", lastLPG);
 client.publish(MQTT_LPG.c_str(), msg);
 digitalWrite(ledWiFi, LOW);
 while (abs(millis() - timerAlarmBeep) < 1000) delay(10); 
 noTone(pinBuzzer);
 timerAlarmBeep = millis();
} // sensorsCheckLPG

// #FUNCTION# ===================================================================================================
// Name...........: sensorsSendData
// Description....: Отправка значений, полученных от датчиков на сервер "Умного дома"
// Syntax.........: sensorsSendData()
// ==============================================================================================================
void sensorsSendData() {
 if (abs(millis() - timerSendData) < timeoutSendData) return;
 timerSendData = millis();
 if ((countDHT11 == 0) && (countLPG == 0)) return;
 digitalWrite(ledWiFi, HIGH);
 if (!client.connected()) MQTTReconnect();
 if (!client.connected()) return;
 if (countDHT11 > 0) {
  if (MQTT_Temperature.length() > 0) {
   snprintf(msg, 50, "%f", floor(10 * sumTemperature / countDHT11) / 10);
   //sprintf(msg, "%1.2f", sumTemperature / countDHT11);
   client.publish(MQTT_Temperature.c_str(), msg);
  }
  if (MQTT_Humidity.length() > 0) {
   snprintf(msg, 50, "%f", floor(10 * sumHumidity / countDHT11) / 10);
   //sprintf(msg, "%1.2f", sumHumidity / countDHT11);
   client.publish(MQTT_Humidity.c_str(), msg);
  }
 }
 if ((countLPG > 0) && (MQTT_LPG.length() > 0)) {
  snprintf(msg, 50, "%f", floor(10 * sumLPG / countLPG) / 10);
  //sprintf(msg, "%1.2f", sumLPG / countLPG);
  client.publish(MQTT_LPG.c_str(), msg);
 }
 digitalWrite(ledWiFi, LOW);
 sumTemperature   = 0;
 sumHumidity      = 0;
 sumLPG           = 0;
 countDHT11       = 0;
 countLPG         = 0;
} // sensorsSendData

