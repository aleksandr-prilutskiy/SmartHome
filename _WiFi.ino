//  Filename:     _WiFi.ino
//  Description:  Система "Умный дом". Функции для работы с сетью WiFi и протоколом MQTT
//  Author:       Aleksandr Prilutskiy
//  Date:         05.04.2019

const uint32_t      timeoutWiFiConnect   =  5000;            // Время ожидания подключения к WiFi
const uint32_t      timeoutWiFiReconnect = 30000;            // Время ожидания для переподключения к WiFi
const uint32_t      timeoutMQTTConnect   =  5000;            // Время ожидания подключения к MQTT
const uint32_t      timeoutMQTTReconnect = 30000;            // Время ожидания для переподключения к MQTT
      uint32_t      timerWiFi            =     0;            // Таймер повторной попытки подключения к WiFi
      uint32_t      timerMQTT            =     0;            // Таймер повторной попытки подключения к MQTT
IPAddress           local_IP(192,168,0,1);                   // IP-адрес в режиме точки доступа WiFi
IPAddress           subnet(255,255,255,0);                   // Маска подсети в режиме точки доступа WiFi
IPAddress           gateway(192,168,0,1);                    // Шлюз по умолчанию в режиме точки доступа WiFi

// #FUNCTION# ===================================================================================================
// Name...........: WiFiSetup
// Description....: Настройка работы WiFi
// Syntax.........: WiFiSetup()
// ==============================================================================================================
void WiFiSetup() {
 digitalWrite(ledWiFi, HIGH);
 timerWiFi = millis();
 if (WiFiSSID.length() == 0) {
  Serial.print("Setting Soft-AP ... ");
  WiFi.softAPConfig(local_IP, gateway, subnet);
  if (WiFi.softAP("ESP8266WiFi", "")) {
   Serial.println("Ready");
   WebServer.begin();
   Serial.print("HTTP server started on: ");
   Serial.println(WiFi.softAPIP());
  }
  else Serial.println("Failed!");
  digitalWrite(ledWiFi, LOW);
  return;
 }
 Serial.print("Connecting to " + WiFiSSID + " ");
 uint32_t timer = millis();
 WiFi.begin(WiFiSSID.c_str(), WiFiPassword.c_str());
 delay(250);
 while (WiFi.status() != WL_CONNECTED) {
  delay(250);
  digitalWrite(ledWiFi, LOW);
  delay(250);
  digitalWrite(ledWiFi, HIGH);
  if (abs(millis() - timer) > timeoutWiFiConnect) {
    WiFiError();
    timerWiFi = millis();
    return;
  }
  Serial.print(".");
 }
 Serial.println(". Ready");
 WebServer.begin();
 Serial.print("HTTP server started on: ");
 Serial.println(WiFi.localIP());
 if (MQTT_Server.length() > 0) {
  timerMQTT = millis();
  Serial.print("Connection to MQTT broker .. ");
  client.setServer(MQTT_Server.c_str(), MQTT_Port);
  client.connect(MQTT_ID.c_str(), MQTT_Login.c_str(), MQTT_Password.c_str());
  if (client.connected()) Serial.println("Ready");
  else WiFiError();
 }
 digitalWrite(ledWiFi, LOW);
} // WiFiSetup

// #FUNCTION# ===================================================================================================
// Name...........: WiFiReconnect
// Description....: Восстановление подключения WiFi
// Syntax.........: WiFiReconnect()
// ==============================================================================================================
void WiFiReconnect() {
 if (abs(millis() - timerWiFi) < timeoutWiFiReconnect) return;
 digitalWrite(ledWiFi, HIGH);
 timerWiFi = millis();
 Serial.print("Reconnecting to " + WiFiSSID + " ");
 WiFi.begin(WiFiSSID.c_str(), WiFiPassword.c_str());
 delay(250);
 while (WiFi.status() != WL_CONNECTED) {
  delay(250);
  digitalWrite(ledWiFi, LOW);
  if (millis() > timerWiFi) {
    WiFiError();
    return;
  }
  delay(250);
  digitalWrite(ledWiFi, HIGH);
  Serial.print(".");
 }
 Serial.println(". Ready");
 Serial.print("IP: ");
 Serial.println(WiFi.localIP());
 digitalWrite(ledWiFi, LOW);
} // WiFiReconnect

// #FUNCTION# ===================================================================================================
// Name...........: MQTTReconnect
// Description....: Восстановление подключения к брокеру MQTT
// Syntax.........: MQTTReconnect()
// ==============================================================================================================
void MQTTReconnect() {
 if (MQTT_Server.length() == 0) return;
 if (abs(millis() - timerMQTT) < timeoutMQTTReconnect) return;
 timerMQTT = millis();
 Serial.print("Attempting MQTT connection ... ");
 if (client.connect(MQTT_ID.c_str(), MQTT_Login.c_str(), MQTT_Password.c_str())) Serial.println("connected");
 else Serial.println("failed!");
} // MQTTReconnect

// #FUNCTION# ===================================================================================================
// Name...........: WiFiError
// Description....: Сигнализация об ошибке работы с WiFi
// Syntax.........: WiFiError()
// ==============================================================================================================
void WiFiError() {
 Serial.println("Failed!");
 digitalWrite(ledError, HIGH);
 digitalWrite(ledWiFi, HIGH);
 delay(500);
 digitalWrite(ledError, LOW);
 digitalWrite(ledWiFi, LOW);
} // WiFiError

