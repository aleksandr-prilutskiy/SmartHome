//  Filename:     _Web.ino
//  Description:  Система "Умный дом". Блок Smart LPG Sensor. Функции работы с временем (через NTP)
//  Author:       Aleksandr Prilutskiy
//  Date:         11.10.2019

#include <NTPClient.h>
#include <WiFiUdp.h>

const uint32_t      timeoutRefreshNTP    = 3600000;          // Задержка повторной сихронизации времени через NTP
const uint32_t      timeoutReconnectNTP  =   10000;          // Задержка повторного подключения к серверу NTP
      uint32_t      lastTimeFromNTP      = 0;                // Значение времени при последней сихронизации
      uint32_t      timerRefreshNTP      = 0;                // Таймер сихронизации времени через NTP
      uint32_t      timerReconnectNTP    = 0;                // Таймер
      uint32_t      millisFix            = 0;                // Поправка счетчика времени работы устройства
      uint32_t      millisLast           = 0;                // Значение последней проверки счатчика миллисекунд

WiFiUDP             ntpUDP;
NTPClient           timeClient(ntpUDP, "ntp1.stratum2.ru");

#define LEAP_YEAR(Year) (!(Year%4) && ((Year%100) || !(Year%400)))

// #FUNCTION# ===================================================================================================
// Name...........: timeInit
// Description....: Инициализация работы с временем
// Syntax.........: timeInit()
// ==============================================================================================================
void timeInit() {
 timeClient.begin();
 timeUpdate();
} // timeInit

// #FUNCTION# ===================================================================================================
// Name...........: timeUpdate
// Description....: Обновление времени с NTP-сервера
// Syntax.........: timeUpdate()
// ==============================================================================================================
void timeUpdate() {
 uint32_t millisNow = millis();
 if (millisNow < millisLast) millisFix += 4294967;
 millisLast = millisNow;
 if ((WiFi.getMode() != WIFI_STA) || (WiFi.status() != WL_CONNECTED)) return;
 if (lastTimeFromNTP == 0) {
  if (abs(millisNow - timerReconnectNTP) < timeoutReconnectNTP) return;
 } else {
  if (abs(millisNow - timerReconnectNTP) < timeoutRefreshNTP) return;
 }
 timerReconnectNTP = millisNow;
 if (timeClient.update()) {
  lastTimeFromNTP = timeClient.getEpochTime();
  timerRefreshNTP = millisFix + (millisNow / 1000);
  Serial.print("NTP time: ");
  Serial.println(lastTimeFromNTP);
 } else {
  Serial.println("NTP Update Error!");
 }
} // timeUpdate

// #FUNCTION# ===================================================================================================
// Name...........: timeNow
// Description....: Получение текущего времени в формате POSIX (Unix time)
// Syntax.........: timeNow()
// ==============================================================================================================
uint32_t timeNow() {
 return timeGet(millisFix + (millis() / 1000));
} // timeNow

// #FUNCTION# ===================================================================================================
// Name...........: timeGet
// Description....: Получение времени в формате POSIX (Unix time) по заданной метке
// Syntax.........: timeGet(timestamp)
// ==============================================================================================================
uint32_t timeGet(uint32_t timestamp) {
 if (lastTimeFromNTP == 0) return timestamp;
 return lastTimeFromNTP + timestamp - timerRefreshNTP + 3600 * (UTC - 24);
} // timeGet

// #FUNCTION# ===================================================================================================
// Name...........: timeToStr
// Description....: Получение строки времени в формате часы:минуты:секунды из формата POSIX (Unix time)
// Syntax.........: timeToStr(timestamp)
// ==============================================================================================================
String timeToStr(uint32_t timestamp) {
 String result = "";
 int hours  = (timestamp % 86400L) / 3600;
 if (hours < 10) result += "0";
 result += String(hours) + ":";
 int minutes = (timestamp % 3600) / 60;
 if (minutes < 10) result += "0";
 result += String(minutes) + ":";
 int seconds = timestamp % 60;
 if (seconds < 10) result += "0";
 result += String(seconds);
 return result;
} // timeToStr

// #FUNCTION# ===================================================================================================
// Name...........: dateToStr
// Description....: Получение строки даты в формате день.месяц.год из формата POSIX (Unix time)
// Syntax.........: dateToStr(timestamp)
// ==============================================================================================================
String dateToStr(uint32_t timestamp) {
 uint32_t days = timestamp / 86400;
 uint16_t nYear = 1970;  
 uint8_t  nMonth = 1;
 uint32_t count = 0;
 uint16_t n;
 while (true) {
  n = LEAP_YEAR(nYear) ? 366 : 365;
  if (count + n > days) break;
  nYear ++;
  count = count + n;
 }
 while (true) {
  n = 31;
  if (nMonth == 2) n = LEAP_YEAR(nYear) ? 29 : 28;
  if ((nMonth == 4) || (nMonth == 6) || (nMonth == 9) || (nMonth == 11)) n = 30;
  if (count + n > days) break;
  nMonth ++;
  count = count + n;
 }
 days = days - count + 1;
 String result = "";
 if (days < 10) result += "0";
 result += String(days) + ".";
 if (nMonth < 10) result += "0";
 result += String(nMonth) + "." + String(nYear);
 return result;
} // dateToStr

