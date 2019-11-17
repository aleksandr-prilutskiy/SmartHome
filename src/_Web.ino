//  Filename:     _Web.ino
//  Description:  Система "Умный дом". Блок Smart LPG Sensor. Функции подготовки и обработки web-страниц
//  Author:       Aleksandr Prilutskiy
//  Date:         08.11.2019

// #FUNCTION# ===================================================================================================
// Name...........: webGetIndex
// Description....: Обработка запроса /
// Syntax.........: webGetIndex()
// ==============================================================================================================
void webGetIndex() {
 digitalWrite(ledWiFi, WiFi.status() == WL_CONNECTED ? LOW : HIGH);
 Serial.println("HTTP GET /index");
 WebServer.send(200, "text/html", webPageIndex());
} // webGetIndex

// #FUNCTION# ===================================================================================================
// Name...........: webGetSetup
// Description....: Обработка запроса /setup
// Syntax.........: webGetSetup()
// ==============================================================================================================
void webGetSetup() {
 digitalWrite(ledWiFi, WiFi.status() == WL_CONNECTED ? LOW : HIGH);
 Serial.println("HTTP GET /setup");
 WebServer.send(200, "text/html", webPageSetup());
} // webGetSetup()

// #FUNCTION# ===================================================================================================
// Name...........: webGetLog
// Description....: Обработка запроса /setup
// Syntax.........: webGetLog()
// ==============================================================================================================
void webGetLog() {
 digitalWrite(ledWiFi, WiFi.status() == WL_CONNECTED ? LOW : HIGH);
 Serial.println("HTTP GET /setup");
 WebServer.send(200, "text/html", webPageLog());
} // webGetLog()

// #FUNCTION# ===================================================================================================
// Name...........: webGetUpdate
// Description....: Обработка запроса /update
// Syntax.........: webGetUpdate()
// ==============================================================================================================
void webGetUpdate() {
 digitalWrite(ledWiFi, WiFi.status() == WL_CONNECTED ? LOW : HIGH);
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
  else if (WebServer.argName(i) == "utc")
   EEPROMSaveByte(WebServer.arg(i).toInt() + 24, UTC, "Time UTC", epprom_UTC);
 }
 EEPROM.end();
 if (errorStr.length() > 0) {
  WebServer.send(200, "text/html", webPageError(errorStr));
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
 digitalWrite(ledWiFi, WiFi.status() == WL_CONNECTED ? LOW : HIGH);
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
           "table{font-size:12px;line-height:18px;width:100%}"
           "a{color:#97C4FF;text-decoration:none}a:hover{text-decoration:underline;color:#aaa}"
           "#wrap{margin:40px auto 0 auto;width: 800px}"
           "#header{border:5px solid #222;height:100px;background:#333}"
           "#header h1{font-size:26px;font-weight:100;padding:12px 0 5px 10px}"
           "#header h2{color:#ccc;font-size:15px;font-weight:100;padding:0 0 0 11px;"
            "letter-spacing:-1px;line-height:12px}"
           ".left{margin-top:10px;width:570px;float:left;text-align:justify;border:5px solid #222;"
            "padding:10px;background:#333}"
           ".left h2{color:#FF4800;font-size:24px;letter-spacing:-1px;font-weight:100;padding:10px 0 15px 0}"
           ".right{margin-top:10px;width:160px;float:right;border:5px solid #222;font-size:12px;"
            "padding:10px;background:#333}"
           ".right ul{list-style-type:square;padding:5px 10px 10px 20px;color:#59799F}"
           ".right h2{height:30px;font-size:14px;color:#666;line-height:30px}"
           ".right a{text-decoration:none}"
           ".split td{width:50%}"
           ".split td:first-child{text-align:right}"
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
 char strTemperature[12];
 char strHumidity[12];
 char strLPG[12];
 if (!isnan(lastTemperature)) sprintf(strTemperature, "%1.2f", lastTemperature);  
 if (!isnan(lastTemperature)) sprintf(strHumidity, "%1.2f", lastHumidity);  
 sprintf(strLPG, "%1.2f", lastLPG);  
 String web = webPageHeader();
 web +=    "<div class=\"left\">"
            "<h2>Состояние устройства</h2>"
            "<div class=\"split\">"
             "<table>" +
              (!isnan(lastTemperature) ? "<tr><td>Значения датчика температуры:</td><td>" +
               String(strTemperature) + " °С</td></tr>" : "") +
              (!isnan(lastHumidity) ? "<tr><td>Значения датчика влажности:</td><td>" +
               String(strHumidity) + " %</td></tr>" : "") +
              "<tr><td>Значения датчика углеводородных газов:</td><td>" +
               String(strLPG) + " PPM</td></tr>"
              "<tr><td>Соединение с брокером MQTT:</td><td>" +
               (connectMQTT ? "установлено" : "отсутствует") + "</td></tr>";
 if (lastTimeFromNTP != 0) web += "<tr><td><br></td></tr>"
              "<tr><td>Текущее время:</td>"
               "<td><i id=\"tD\"></i> <i id=\"tH\"></i>:<i id=\"tM\"></i>:<i id=\"tS\"></i></td></tr>"
              "<tr><td>Время синхронизации часов:</td><td><i id=\"uT\"></i></td></tr>"
              "<tr><td>Время работы устройства:</td>"
               "<td><i id=\"rD\"></i> дней <i id=\"rH\"></i> часов <i id=\"rM\"></i> минут</td></tr>";
 web +=      "</table>"
            "</div>"
           "</div>";
 if (lastTimeFromNTP != 0) {
  uint32_t time_now = timeNow();
  uint32_t time_work = millisFix + (millis() / 1000);
  uint32_t time_update = lastTimeFromNTP + 3600 * (UTC - 24);
  web += "<script>"
   "function str(i){if(i<10)return '0'+i;return i;}"
   "function time(){"
    "if(++tS>59){tM++;rM++;tS=0;}if(tM>59){tH++;tM=0;}if(tH>23){window.location.reload();}"
    "if(rM>59){rH++;rM=0;}if(rH>23){rD++;rH=0;}"
    "document.getElementById(\"tH\").innerHTML=str(tH);"
    "document.getElementById(\"tM\").innerHTML=str(tM);"
    "document.getElementById(\"tS\").innerHTML=str(tS);"
    "document.getElementById(\"rD\").innerHTML=str(rD);"
    "document.getElementById(\"rH\").innerHTML=str(rH);"
    "document.getElementById(\"rM\").innerHTML=str(rM);"
    "setTimeout(time, 1000);"
   "}"
   "var tH=" + String((time_now % 86400L) / 3600, DEC) + ";"
   "var tM=" + String((time_now % 3600) / 60, DEC) + ";"
   "var tS=" + String(time_now % 60, DEC) + ";"
   "document.getElementById(\"tD\").innerHTML='" + dateToStr(time_now) + "';"
   "document.getElementById(\"uT\").innerHTML='" + dateToStr(time_update) + " " + timeToStr(time_update) + "';"
   "var rD=" + String(time_work / 86400L, DEC) + ";"
   "var rH=" + String((time_work % 86400L) / 3600, DEC) + ";"
   "var rM=" + String((time_work % 3600) / 60, DEC) + ";"
   "window.onload = time;"
  "</script>";
 }
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
            "<h2>Настройка устройства</h2>"
            "<div class=\"split\">"
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
               "<tr><td><strong>Настройки протокола MQTT:</strong></td></tr>"
               "<tr>"
                "<td>DNS-имя или IP-адрес брокера MQTT:</td>"
                "<td><input name=\"mqtt_server\" value=\"" + MQTT_Server + "\"></td>"
               "</tr>"
               "<tr>"
                "<td>Порт для подключения к брокеру MQTT:</td>"
                "<td><input name=\"mqtt_port\" value=\"" + String(MQTT_Port) + "\"></td>"
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
               "<tr><td><strong>Настройки датчиков:</strong></td></tr>"
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
                "<td><input name=\"lpg\" value=\"" + String(alarmLPG) + "\"></td>"
               "</tr>"
               "<tr><td><strong>Настройка времени:</strong></td></tr>"
               "<tr>"
                "<td>Часовой пояс (UTC +/- часов):</td>"
                "<td><input name=\"utc\" value=\"" + String(UTC - 24) + "\"></td>"
               "</tr>"
              "</table>"
              "<br><p align=\"center\"><input type=\"submit\" value=\"Сохранить\"></p>"
             "</form>"
            "</div>"
            "<h2>Управление устройством</h2>"
            "<div class=\"info\">"
             "<p align=\"center\">"
              "<button onclick=\"confirmReboot()\">Перезагрузить</button>"
             "</p>"
            "</div>"
           "</div>"
"<script>"
"function confirmReboot() {"
"if (confirm(\"Подтвердите действие для перезагрузки устройства\")==true) window.location.href='/reset';"
"}"
"</script>";
 return (web + webPageFooter());
} // webPageSetup

// #FUNCTION# ===================================================================================================
// Name...........: webPageLog
// Description....: Подготовка web-страницы журнала работы устройства
// Syntax.........: webPageLog()
// ==============================================================================================================
String webPageLog() {
 String web = webPageHeader();
 web +=    "<div class=\"left\">"
            "<h2>Журнал работы устройства</h2>"
            "<div>"
             "<table>";
 for (int i = logSize - 1; i >=0 ; i--) {
  String txt = logGetEvent(i);
  if (txt.length() > 0) web += "<tr><td colspan=\"2\">" + logGetDateTime(i) + ": " + txt + "</td></tr>";
 }
 web +=      "</table>"
            "</div>"
           "</div>";
 return (web + webPageFooter());
} // webPageLog

// #FUNCTION# ===================================================================================================
// Name...........: webPageUpdate
// Description....: Подготовка web-страницы ожидания перезагрузки устройства
// Syntax.........: webPageUpdate()
// ==============================================================================================================
String webPageUpdate() {
 String web = webPageHeader();
 web +=    "<div class=\"left\">"
            "<h2>Перезагрузка устройства</h2>"
            "<div>"
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
             "<li><a href=\"/log\">Журнал</a></li>"
            "</ul>"
           "</div>"
           "<div style=\"clear:both\"> </div>"
           "<div id=\"footer\">(с) 2019 Aleksandr Prilutskiy</div>"
          "</div>"
         "</body>"
        "</html>";
 return (web);
} // webPageFooter

