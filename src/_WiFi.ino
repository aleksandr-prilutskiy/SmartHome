//  Filename:     _WiFi.ino
//  Description:  Система "Умный дом". Функции для работы с сетью WiFi и протоколом MQTT
//  Author:       Aleksandr Prilutskiy
//  Date:         26.05.2019

const uint32_t      timeoutWiFiConnect   = 10000;            // Время ожидания подключения к WiFi
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
 digitalWrite(ledWiFi, LOW);
 timerWiFi = millis();
 if (WiFiSSID.length() == 0) {
  Serial.print("Setting Soft-AP ... ");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  if (WiFi.softAP("ESP8266WiFi", "")) {
   Serial.println("Ready");
   WebServer.begin();
   Serial.print("HTTP server started on: ");
   Serial.println(WiFi.softAPIP());
  }
  else Serial.println("Failed!");
  return;
 }
 Serial.print("Connecting to " + WiFiSSID + " .");
 uint32_t timer = millis();
 WiFi.mode(WIFI_STA);
 WiFi.setAutoConnect(false);
 WiFi.begin(WiFiSSID.c_str(), WiFiPassword.c_str());
 delay(250);
 while (WiFi.status() != WL_CONNECTED) {
  delay(250);
  digitalWrite(ledWiFi, HIGH);
  delay(250);
  digitalWrite(ledWiFi, LOW);
  if (abs(millis() - timer) > timeoutWiFiConnect) {
    WiFiError(3);
    timerWiFi = millis();
    return;
  }
  Serial.print(".");
 }
 Serial.println(". Ready");
 WiFi.setAutoConnect(true);
 WebServer.begin();
 Serial.print("HTTP server started on: ");
 Serial.println(WiFi.localIP());
 if (MQTT_Server.length() > 0) {
  timerMQTT = millis();
  Serial.print("Connection to MQTT broker .. ");
  client.setServer(MQTT_Server.c_str(), MQTT_Port);
  client.connect(MQTT_ID.c_str(), MQTT_Login.c_str(), MQTT_Password.c_str());
  if (client.connected()) {
   connectMQTT = true;
   Serial.println("Ready");
  }
  else {
   connectMQTT = false;
   WiFiError(2);
  }
 }
} // WiFiSetup

// #FUNCTION# ===================================================================================================
// Name...........: WiFiReconnect
// Description....: Восстановление подключения WiFi
// Syntax.........: WiFiReconnect()
// ==============================================================================================================
void WiFiReconnect() {
 if ((WiFiSSID.length() == 0) || (abs(millis() - timerWiFi) < timeoutWiFiReconnect)) return;
 digitalWrite(ledWiFi, HIGH);
 timerWiFi = millis();
 Serial.print("Reconnecting to " + WiFiSSID + " .");
 WiFi.disconnect();
 WiFi.mode(WIFI_OFF);
 WiFi.persistent(true);
 delay(1000);
 uint32_t timer = millis();
 WiFi.mode(WIFI_STA);
 WiFi.begin(WiFiSSID.c_str(), WiFiPassword.c_str());
 delay(250);
 while (WiFi.status() != WL_CONNECTED) {
  delay(250);
  digitalWrite(ledWiFi, LOW);
  if (abs(millis() - timer) > timeoutWiFiConnect) {
    WiFiError(3);
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.persistent(true);
    return;
  }
  delay(250);
  digitalWrite(ledWiFi, HIGH);
  Serial.print(".");
 }
 Serial.println(". Ready");
 Serial.print("IP: ");
 Serial.println(WiFi.localIP());
} // WiFiReconnect

// #FUNCTION# ===================================================================================================
// Name...........: MQTTReconnect
// Description....: Восстановление подключения к брокеру MQTT
// Syntax.........: MQTTReconnect()
// ==============================================================================================================
void MQTTReconnect() {
 if ((WiFiSSID.length() == 0) || (MQTT_Server.length() == 0) || (WiFi.status() != WL_CONNECTED)) return;
 if (abs(millis() - timerMQTT) < timeoutMQTTReconnect) return;
 digitalWrite(ledWiFi, HIGH);
 timerMQTT = millis();
 Serial.print("Attempting MQTT connection ... ");
 client.setServer(MQTT_Server.c_str(), MQTT_Port);
 if (client.connect(MQTT_ID.c_str(), MQTT_Login.c_str(), MQTT_Password.c_str())) {
  connectMQTT = true;
  Serial.println("connected");
 }
 else {
  connectMQTT = false;
  WiFiError(2);
 }
} // MQTTReconnect

// #FUNCTION# ===================================================================================================
// Name...........: WiFiError
// Description....: Сигнализация об ошибке
// Syntax.........: WiFiError(n)
// ==============================================================================================================
void WiFiError(int n) {
 Serial.println("Failed!");
 for (unsigned int i = 0; i < n; i++) {
  digitalWrite(ledWiFi, LOW);
  digitalWrite(ledError, HIGH);
  delay(250);
  digitalWrite(ledWiFi, HIGH);
  digitalWrite(ledError, LOW);
  delay(250);
 }
 digitalWrite(ledWiFi, LOW);
} // WiFiError
