//  Filename:     _WiFi.ino
//  Description:  SmartHome - Функции для работы с сетью WiFi и протоколом MQTT
//  Author:       Aleksandr Prilutskiy
//  Date:         23.07.2019

const uint32_t      timeoutWiFiConnect   =  5000;            // Время ожидания подключения к WiFi
const uint32_t      timeoutWiFiReconnect = 30000;            // Время ожидания для переподключения к WiFi
const uint32_t      timeoutMQTTConnect   = 10000;            // Время ожидания подключения к MQTT
const uint32_t      timeoutMQTTReconnect = 30000;            // Время ожидания для переподключения к MQTT
      uint32_t      timerWiFi            =     0;            // Таймер повторной попытки подключения к WiFi
      uint32_t      timerMQTT            =     0;            // Таймер повторной попытки подключения к MQTT
      uint8_t       reconnectWiFiCount   =     0;            // Счетчик попыток повторного подключения к WiFi
IPAddress           local_IP(192,168,0,1);                   // IP-адрес в режиме точки доступа WiFi
IPAddress           subnet(255,255,255,0);                   // Маска подсети в режиме точки доступа WiFi
IPAddress           gateway(192,168,0,1);                    // Шлюз по умолчанию в режиме точки доступа WiFi

// #FUNCTION# ===================================================================================================
// Name...........: WiFiCreateAP
// Description....: Настройка работы WiFi в режиме точки доступа
// Syntax.........: WiFiCreateAP()
// ==============================================================================================================
bool WiFiCreateAP() {
 WiFi.persistent(false);
 if (WiFiSSID.length() > 0) return false;
 digitalWrite(ledWiFi, HIGH);
 Serial.print("Setting Soft-AP ... ");
 WiFi.mode(WIFI_AP);
 WiFi.softAPConfig(local_IP, gateway, subnet);
 if (WiFi.softAP("ESP8266WiFi", "")) {
  Serial.println("Ready");
  WebServer.begin();
  Serial.print("HTTP server started on: ");
  Serial.println(WiFi.softAPIP());
  digitalWrite(ledWiFi, LOW);
  return true;
 }
 Serial.println("Failed!");
 while (true) {
  digitalWrite(ledError, HIGH);
  delay(1000);
  digitalWrite(ledError, LOW);
  delay(1000);
 }
} // WiFiCreateAP

// #FUNCTION# ===================================================================================================
// Name...........: WiFiSetup
// Description....: Настройка работы WiFi в режиме подключения к точке доступа
// Syntax.........: WiFiSetup()
// ==============================================================================================================
void WiFiSetup() {
 digitalWrite(ledWiFi, HIGH);
 if (WiFiSSID.length() == 0) return;
 Serial.print("Connecting to " + WiFiSSID + " ");
 timerWiFi = 0;
 uint32_t timer = millis();
 WiFi.mode(WIFI_STA);
 WiFi.setAutoConnect(false);
 WiFi.setAutoReconnect(false);
 WiFi.begin(WiFiSSID.c_str(), WiFiPassword.c_str());
 delay(250);
 while (WiFi.status() != WL_CONNECTED) {
  delay(250);
  digitalWrite(ledWiFi, LOW);
  delay(250);
  digitalWrite(ledWiFi, HIGH);
  if (abs(millis() - timer) > timeoutWiFiConnect) {
   Serial.println("Failed!");
   digitalWrite(ledError, HIGH);
   digitalWrite(ledWiFi, HIGH);
   delay(500);
   digitalWrite(ledError, LOW);
   digitalWrite(ledWiFi, LOW);
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
  if (client.connected()) {
   connectMQTT = true;
   Serial.println("Ready");
  }
  else {
   connectMQTT = false;
   Serial.println("Failed!");
   digitalWrite(ledError, HIGH);
   digitalWrite(ledWiFi, HIGH);
   delay(500);
   digitalWrite(ledError, LOW);
   digitalWrite(ledWiFi, LOW);
  }
 }
 digitalWrite(ledWiFi, LOW);
} // WiFiSetup

// #FUNCTION# ===================================================================================================
// Name...........: WiFiReconnect
// Description....: Восстановление подключения к WiFi
// Syntax.........: WiFiReconnect()
// ==============================================================================================================
void WiFiReconnect() {
 digitalWrite(ledWiFi, WiFi.status() == WL_CONNECTED ? HIGH : LOW);
 if ((WiFiSSID.length() == 0) || (WiFi.status() == WL_CONNECTED)) return;
 if ((timerWiFi != 0) && (abs(millis() - timerWiFi) < timeoutWiFiReconnect))  return;
 timerWiFi = millis();
 WebServer.stop();
 WiFi.disconnect();
 delay(500);
 WiFi.mode(WIFI_OFF);
 delay(1000);
 WiFiSetup();
 reconnectWiFiCount++;
 if (WiFi.status() == WL_CONNECTED) reconnectWiFiCount = 0;
 if (reconnectWiFiCount > 5) Reboot();
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
  Serial.println("Failed!");
  digitalWrite(ledError, HIGH);
  digitalWrite(ledWiFi, HIGH);
  delay(500);
  digitalWrite(ledError, LOW);
  digitalWrite(ledWiFi, LOW);
 }
} // MQTTReconnect
