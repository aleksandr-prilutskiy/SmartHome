//  Filename:     _Web.ino
//  Description:  Система "Умный дом". Функции подготовки и обработки web-страниц
//  Author:       Aleksandr Prilutskiy
//  Date:         10.04.2019

// #FUNCTION# ===================================================================================================
// Name...........: webGetIndex
// Description....: Обработка запроса /
// Syntax.........: webGetIndex()
// ==============================================================================================================
void webGetIndex() {
 digitalWrite(ledWiFi, HIGH);
 Serial.println("HTTP GET /index");
 WebServer.send(200, "text/html", webPageIndex());
 digitalWrite(ledWiFi, LOW);
} // webGetIndex

// #FUNCTION# ===================================================================================================
// Name...........: webGetSetup
// Description....: Обработка запроса /setup
// Syntax.........: webGetSetup()
// ==============================================================================================================
void webGetSetup() {
 digitalWrite(ledWiFi, HIGH);
 Serial.println("HTTP GET /setup");
 WebServer.send(200, "text/html", webPageSetup());
 digitalWrite(ledWiFi, LOW);
} // webGetSetup()

// #FUNCTION# ===================================================================================================
// Name...........: webGetUpdate
// Description....: Обработка запроса /update
// Syntax.........: webGetUpdate()
// ==============================================================================================================
void webGetUpdate() {
 digitalWrite(ledWiFi, HIGH);
 Serial.println("HTTP POST /update");
 errorStr = "";
 EEPROM.begin(sizeEEPROM);
 for (int i = 0; i < WebServer.args(); i++) {
  if (WebServer.argName(i) == "ssid")
   EEPROMSaveString(WebServer.arg(i), WiFiSSID, 31, "WiFi SSID", epprom_WiFiSSID);
  else if (WebServer.argName(i) == "pass")
   EEPROMSaveString(WebServer.arg(i), WiFiPassword, 31, "WiFi Password", epprom_WiFiPass);
  else if (WebServer.argName(i) == "mqtt_server")
   EEPROMSaveString(WebServer.arg(i), MQTT_Server, 61, "MQTT Server", epprom_MQTTAddr);
  else if (WebServer.argName(i) == "mqtt_port")
   EEPROMSaveInt(WebServer.arg(i).toInt(), MQTT_Port, "MQTT Port", epprom_MQTTPort);
  else if (WebServer.argName(i) == "mqtt_id")
   EEPROMSaveString(WebServer.arg(i), MQTT_ID, 31, "MQTT Instance ID", epprom_MQTTID);
  else if (WebServer.argName(i) == "mqtt_user")
   EEPROMSaveString(WebServer.arg(i), MQTT_Login, 15, "MQTT Login", epprom_MQTTLogin);
  else if (WebServer.argName(i) == "mqtt_pass")
   EEPROMSaveString(WebServer.arg(i), MQTT_Password, 15, "MQTT Password", epprom_MQTTPass);
  else if (WebServer.argName(i) == "mqtt_tmp")
   EEPROMSaveString(WebServer.arg(i), MQTT_Temperature, 15, "Topic Temperature Sensor", epprom_Temperat);
  else if (WebServer.argName(i) == "mqtt_hum")
   EEPROMSaveString(WebServer.arg(i), MQTT_Humidity, 15, "Topic Humidity Sensor", epprom_Humidity);
  else if (WebServer.argName(i) == "mqtt_lpg")
   EEPROMSaveString(WebServer.arg(i), MQTT_LPG, 15, "Topic LPG Sensor", epprom_LPG);
  else if (WebServer.argName(i) == "lpg")
   EEPROMSaveInt(WebServer.arg(i).toInt(), alarmLPG, "LPG Alarm Level", epprom_alarmLPG);
 }
 EEPROM.end();
 if (errorStr.length() > 0) {
  WebServer.send(200, "text/html", webPageError(errorStr));
  digitalWrite(ledWiFi, LOW);
  return;
 }
 WebServer.send(200, "text/html", webPageUpdate());
 digitalWrite(ledWiFi, LOW);
 for (int i = 0; i < 5; i++) {
  delay(500);
  digitalWrite(ledPower, LOW);
  delay(500);
  digitalWrite(ledPower, HIGH);
 }
 Reboot();
} // webGetUpdate

// #FUNCTION# ===================================================================================================
// Name...........: webGetReset
// Description....: Обработка запроса /reset
// Syntax.........: webGetReset()
// ==============================================================================================================
void webGetReset() {
 digitalWrite(ledWiFi, HIGH);
 Serial.println("HTTP GET /reset");
 Reboot();
} // webGetReset

// #FUNCTION# ===================================================================================================
// Name...........: webPageHeader
// Description....: Подготовка 'шапки' web-страницы
// Syntax.........: webPageHeader()
// ==============================================================================================================
String webPageHeader() {
 String web = "<html>"
        "<head>"
          "<title>SmartHome - " + deviceName + "</title>"
          "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />"
          "<style>"
           "*{padding:0;margin:0}"
           "body{background:#000;font-family:Verdana,Arial,Helvetica,sans-serif;font-size:11px;"
            "line-height:18px;color:#aaa}"
           "table{font-size:12px;line-height:18px;width:100%}td{width:50%}td:first-child{text-align:right}"
           "a{color:#97C4FF;text-decoration:none}a:hover{text-decoration:underline;color:#aaa}"
           "#wrap{margin:40px auto 0 auto;width: 800px}"
           "#header{border:5px solid #222;height:100px;background:#333}"
           "#header h1{font-size:26px;font-weight:100;padding:12px 0 5px 10px}"
           "#header h2{color:#ccc;font-size:15px;font-weight:100;padding:0 0 0 11px;"
            "letter-spacing:-1px;line-height:12px}"
           ".left{margin-top:10px;width:570px;float:left;text-align:justify;border:5px solid #222;"
            "padding:10px;background:#333}"
           ".left h2{color:#FF4800;font-size:24px;letter-spacing:-3px;font-weight:100;padding:10px 0 15px 0}"
           ".right{margin-top:10px;width:160px;float:right;border:5px solid #222;font-size:12px;"
            "padding:10px;background:#333}"
           ".right ul{list-style-type:square;padding:5px 10px 10px 20px;color:#59799F}"
           ".right h2{height:30px;font-size:14px;color:#666;line-height:30px}"
           ".right a{text-decoration:none}"
           "#footer{margin-top:10px;text-align:center;color:#eee;font-size:11px;border:5px solid #222;"
            "padding:10px;background:#333}"
           "textarea{color:#ccc;background:#333;resize: none}"
          "</style>"
         "</head>"
         "<body>"
          "<div id=\"wrap\">"
           "<div id=\"header\"><h1>MySmartHome</h1><br><h2>" + deviceName + " v." + deviceVersion + "</h2></div>";
 return (web);
} // webPageHeader

// #FUNCTION# ===================================================================================================
// Name...........: webPageIndex
// Description....: Подготовка главной web-страницы устройства
// Syntax.........: webPageIndex()
// ==============================================================================================================
String webPageIndex() {
 String web = webPageHeader();
 web +=    "<div class=\"left\">"
            "<h2>Состояние утройства</h2>"
            "<div class=\"info\">"
             "<table>" +
              (!isnan(lastTemperature) ? "<tr><td>Значения датчика температуры:</td><td>" +
               String(round(lastTemperature * 100) / 100, DEC) + " °С</td></tr>" : "") +
              (!isnan(lastHumidity) ? "<tr><td>Значения датчика влажности:</td><td>" +
               String(round(lastHumidity * 100) / 100, DEC) + " %</td></tr>" : "") +
              "<tr><td>Значения датчика углеводородных газов:</td><td>" +
               String(round(lastLPG * 100) / 100, DEC) + " PPM</td></tr>"
              "<tr><td>Соединение с брокером MQTT:</td><td>" +
               (connectMQTT ? "установлено" : "отсуствует") + "</td></tr>"
             "</table>"
            "</div>"
           "</div>";
 return (web + webPageFooter());
} // webPageIndex

// #FUNCTION# ===================================================================================================
// Name...........: webPageSetup
// Description....: Подготовка web-страницы настройки устройства
// Syntax.........: webPageSetup()
// ==============================================================================================================
String webPageSetup() {
 String password = "";
 for (int i = 0; i < WiFiPassword.length(); i++) password = password + " ";
 String web = webPageHeader();
 web +=    "<div class=\"left\">"
            "<h2>Настройка утройства</h2>"
            "<div class=\"info\">"
             "<form method=\"POST\" action=\"update\">"
              "<table>"
               "<tr><td><strong>Настройки беспроводной сети:</strong></td></tr>"
               "<tr>"
                "<td>SSID точки доступа WiFi:</td>"
                "<td><input name=\"ssid\" value=\"" + WiFiSSID + "\"></td>"
               "</tr>"
               "<tr>"
                "<td>Пароль точки доступа WiFi:</td>"
                "<td><input type=\"password\" name=\"pass\" value=\"" + password + "\"></td>"
               "</tr>"
               "<tr><td><br><strong>Настройки протокола MQTT:</strong></td></tr>"
               "<tr>"
                "<td>DNS-имя или IP-адрес брокера MQTT:</td>"
                "<td><input name=\"mqtt_server\" value=\"" + MQTT_Server + "\"></td>"
               "</tr>"
               "<tr>"
                "<td>Порт для подключения к брокеру MQTT:</td>"
                "<td><input name=\"mqtt_port\" value=\"" + MQTT_Port + "\"></td>"
               "</tr>"
               "<tr>"
                "<td>ID образца (Instance) брокера MQTT:</td>"
                "<td><input name=\"mqtt_id\" value=\"" + MQTT_ID + "\"></td>"
               "</tr>"
               "<tr>"
                "<td>Логин пользователя брокера MQTT:</td>"
                "<td><input name=\"mqtt_user\" value=\"" + MQTT_Login + "\"></td>"
               "</tr>"
               "<tr>"
                "<td>Пароль пользователя брокера MQTT:</td>"
                "<td><input name=\"mqtt_pass\" value=\"" + MQTT_Password + "\"></td>"
               "</tr>"
               "<tr><td><br><strong>Настройки датчиков:</strong></td></tr>"
               "<tr>"
                "<td>MQTT ID датчика температуры :</td>"
                "<td><input name=\"mqtt_tmp\" value=\"" + MQTT_Temperature + "\"></td>"
               "</tr>"
               "<tr>"
                "<td>MQTT ID датчика влажности:</td>"
                "<td><input name=\"mqtt_hum\" value=\"" + MQTT_Humidity + "\"></td>"
               "</tr>"
               "<tr>"
                "<td>MQTT ID датчика углеводородных газов:</td>"
                "<td><input name=\"mqtt_lpg\" value=\"" + MQTT_LPG + "\"></td>"
               "</tr>"
               "<tr>"
                "<td>Пороговое значение датчика LPG:</td>"
                "<td><input name=\"lpg\" value=\"" + alarmLPG + "\"></td>"
               "</tr>"
              "</table>"
              "<br><p align=\"center\"><input type=\"submit\" value=\"Сохранить\"></p>"
             "</form>"
            "</div>"
            "<h2>Управление утройством</h2>"
            "<div class=\"info\">"
             "<p align=\"center\">"
              "<button onclick=\"confirmReboot()\">Перезагрузить</button>"
             "</p>"
            "</div>"
           "</div>"
"<script>"
"function confirmReboot() {"
"if (confirm(\"Подтвертите действие для перезагрузки устройства\")==true) window.location.href='/reset';"
"}"
"</script>";
 return (web + webPageFooter());
} // webPageSetup

// #FUNCTION# ===================================================================================================
// Name...........: webPageUpdate
// Description....: 
// Syntax.........: webPageUpdate()
// ==============================================================================================================
String webPageUpdate() {
 String web = webPageHeader();
 web +=    "<div class=\"left\">"
            "<h2>Перезагрузка утройства</h2>"
            "<div class=\"info\">"
             "<p>Подождите. Сейчас устройство будет перезагружено...</p>"
            "</div>"
           "</div>"
 "<script>"
  "function redirect(){location=\"/\";}"
  "function timeout(){setTimeout(redirect, 5000);}"
  "window.onload = timeout;"
 "</script>";
 return (web + webPageFooter());
} // webPageUpdate

// #FUNCTION# ===================================================================================================
// Name...........: webPageError
// Description....: Подготовка web-страницы с сообщением об ошибке
// Syntax.........: webPageError(message)
// ==============================================================================================================
String webPageError(String message) {
 String web = webPageHeader();
 web +=    "<div class=\"left\">"
            "<h2>Ошибка!</h2>"
            "<div class=\"info\">" + message + "</div>"
           "</div>";
 return (web + webPageFooter());
} // webPageError

// #FUNCTION# ===================================================================================================
// Name...........: webNotFound
// Description....: Вывод ошибки "Страница не найдена"
// Syntax.........: webNotFound()
// ==============================================================================================================
void webNotFound() {
 digitalWrite(ledWiFi, HIGH);
 Serial.println("Error: HTTP Page Not Found");
 WebServer.send(404, "text/html", webPageError("Страница не найдена"));
 digitalWrite(ledWiFi, LOW);
} // webNotFound

// #FUNCTION# ===================================================================================================
// Name...........: webPageFooter
// Description....: Подготовка 'подвала' web-страницы
// Syntax.........: webPageFooter()
// ==============================================================================================================
String webPageFooter() {
 String web = "<div class=\"right\">"
            "<h2>Меню :</h2>"
            "<ul>"
             "<li><a href=\"/\">Главная</a></li>"
             "<li><a href=\"/setup\">Настройки</a></li>"
            "</ul>"
           "</div>"
           "<div style=\"clear:both\"> </div>"
           "<div id=\"footer\">(с) 2019 Aleksandr Prilutskiy</div>"
          "</div>"
         "</body>"
        "</html>";
 return (web);
} // webPageFooter

