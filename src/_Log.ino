//  Filename:     _Web.ino
//  Description:  Система "Умный дом". Блок Smart LPG Sensor. Функции работы с журналом работы устройства
//  Author:       Aleksandr Prilutskiy
//  Date:         14.10.2019
//
//  Журнал работы устройства представляет собой два одномерных массива, связанных по одному индексу.
//  Первый массив (logTime) содержит время события, выраенное в секундах с момента запуска устройства.
//  Второй массив (logData) представляет собой массив 4-х байтных целых чисел, старший байт которых
//  содержит код события, а остальный 3 байта - дополнительную информацию, в засисимости от события.

const uint8_t       logSize              = 50;               // Количество записей в журнале
uint32_t            logTime[logSize];                        // Количество секунд с начала запуска устройства
uint32_t            logData[logSize];                        // Код события (старший байт) и данные

//  Коды событий:
const uint8_t       logStart             =   1;              // Запуск устройства
const uint8_t       logConnectWiFi       =   2;              // Успешное подключение к точке доступа WiFi
const uint8_t       logErrorWiFi         =   3;              // Ошибка подключения к точке доступа WiFi
const uint8_t       logAlarmLPG          = 100;              // Превышение заданного уровня показаний датчика LPG

// #FUNCTION# ===================================================================================================
// Name...........: logInit
// Description....: Инициализация журнала
// Syntax.........: logInit()
// ==============================================================================================================
void logInit() {
 for (int i = 0; i < logSize; i++) {
  logTime[i] = 0;
  logData[i] = 0;
 }
 logSave(logStart);
} // logInit

// #FUNCTION# ===================================================================================================
// Name...........: logSave
// Description....: Сохранение кода события в журнал (с дополнительными данными)
// Syntax.........: logSave(code, data)
// ==============================================================================================================
void logSave(uint8_t code, uint32_t data) {
 for (int i = logSize - 1; i > 0; i--) {
  logTime[i] = logTime[i - 1];
  logData[i] = logData[i - 1];
 }
 logTime[0] = timeNow();
 logData[0] = (code << 24) | (data & 0x00FFFFFF);
} // logSave

// #FUNCTION# ===================================================================================================
// Name...........: logSave
// Description....: Сохранение кода события в журнал
// Syntax.........: logSave(data)
// ==============================================================================================================
void logSave(uint8_t code) {
 logSave(code, 0);
} // logSave

// #FUNCTION# ===================================================================================================
// Name...........: logGetEvent
// Description....: Получение описания события в журнале по индексу
// Syntax.........: logGetEvent(index)
// ==============================================================================================================
String logGetEvent(uint8_t index) {
 if ((index < 0) || (index >= logSize)) return "";
 switch (logData[index] >> 24) {
  case logStart:
   return "Start";
  case logConnectWiFi:
   return "Connect to WiFi - OK";
  case logErrorWiFi:
   return "Connect to WiFi - Error";
  case logAlarmLPG:
   return "LPG Level Alarm! = " + String(logData[index] & 0x00FFFFFF) + "PPM";
  default:
   return "";
 }
} // logGetEvent

// #FUNCTION# ===================================================================================================
// Name...........: logGetDateTime
// Description....: Получение строки даты и времени события в журнале по индексу
// Syntax.........: logGetDateTime(index)
// ==============================================================================================================
String logGetDateTime(uint8_t index) {
 if ((index < 0) || (index >= logSize)) return "";
 if (logData[index] == 0) return "";
 uint32_t logtime = timeGet(logTime[index]);
 return String(logtime) + " = " + dateToStr(logtime) + " " + timeToStr(logtime);
} // logGetDateTime

