//  Filename:     _EEPROM.ino
//  Description:  Система "Умный дом". Функции для работы с EEPROM
//  Author:       Aleksandr Prilutskiy
//  Date:         24.05.2019

// Константы адресов переменных в EEPROM:
const uint16_t      epprom_WiFiSSID  =   0;                  // WiFiSSID (32 байта)
const uint16_t      epprom_WiFiPass  =  32;                  // WiFiPassword (32 байта)
const uint16_t      epprom_MQTTAddr  =  64;                  // MQTT_Server (62 байта)
const uint16_t      epprom_MQTTPort  = 126;                  // MQTT_Port (2 байта)
const uint16_t      epprom_MQTTID    = 128;                  // MQTT_ID (32 байта)
const uint16_t      epprom_MQTTLogin = 160;                  // MQTT_Login (16 байт)
const uint16_t      epprom_MQTTPass  = 176;                  // MQTT_Password (16 байт)
const uint16_t      epprom_Temperat  = 192;                  // MQTT_Temperature (16 байт)
const uint16_t      epprom_Humidity  = 208;                  // MQTT_Humidity (16 байт)
const uint16_t      epprom_LPG       = 224;                  // MQTT_LPG (16 байт)
const uint16_t      epprom_alarmLPG  = 240;                  // alarmLPG (2 байта)

// #FUNCTION# ===================================================================================================
// Name...........: EEPROMReadAll
// Description....: Чтение значений всех переменных в EEPROM
// Syntax.........: EEPROMReadAll()
// ==============================================================================================================
void EEPROMReadAll() {
 EEPROM.begin(sizeEEPROM);
 WiFiSSID         = EEPROMReadString(epprom_WiFiSSID, 32);
 WiFiPassword     = EEPROMReadString(epprom_WiFiPass, 32);
 MQTT_Server      = EEPROMReadString(epprom_MQTTAddr, 62);
 MQTT_Port        = EEPROM.read(epprom_MQTTPort) + 256 * EEPROM.read(epprom_MQTTPort + 1);
 MQTT_ID          = EEPROMReadString(epprom_MQTTID, 32);
 MQTT_Login       = EEPROMReadString(epprom_MQTTLogin, 16);
 MQTT_Password    = EEPROMReadString(epprom_MQTTPass, 16);
 MQTT_Temperature = EEPROMReadString(epprom_Temperat, 16);
 MQTT_Humidity    = EEPROMReadString(epprom_Humidity, 16);
 MQTT_LPG         = EEPROMReadString(epprom_LPG, 16);
 alarmLPG         = EEPROM.read(epprom_alarmLPG) + 256 * EEPROM.read(epprom_alarmLPG + 1);
 EEPROM.end();
 Serial.print(" WiFi SSID = ");
 Serial.println(WiFiSSID);
 Serial.print(" MQTT Server = ");
 Serial.print(MQTT_Server);
 Serial.print(":");
 Serial.println(MQTT_Port);
 Serial.print(" MQTT Instance = ");
 Serial.println(MQTT_ID);
 Serial.print(" MQTT User = ");
 Serial.println(MQTT_Login);
 Serial.print(" MQTT Topic Temperature = ");
 Serial.println(MQTT_Temperature);
 Serial.print(" MQTT Topic Humidity = ");
 Serial.println(MQTT_Humidity);
 Serial.print(" MQTT Topic LPG = ");
 Serial.println(MQTT_LPG);
 Serial.print(" LPG Alarm Level = ");
 Serial.println(alarmLPG);
 Serial.println();
} // EEPROMReadAll

// #FUNCTION# ===================================================================================================
// Name...........: EEPROMReadString
// Description....: Чтение строки из EEPROM
// Syntax.........: EEPROMReadString(startaddr, maxlength)
// ==============================================================================================================
String EEPROMReadString(unsigned int startaddr, unsigned int maxlength) {
 String newstr = "";
 byte value;
 for (unsigned int i = 0; i < maxlength; i++) {
  value = EEPROM.read(startaddr + i);
  if (value == 0) break;
  newstr = newstr + char(value);
 }
 return newstr;
} // EEPROMReadString

// #FUNCTION# ===================================================================================================
// Name...........: EEPROMSaveInt
// Description....: Запись в EEPROM двухбайтного целого числа
// Syntax.........: EEPROMSaveInt()
// ==============================================================================================================
void EEPROMSaveInt(uint16_t data, uint16_t& varSetup, String varName, int addr) {
 if (data == varSetup) return;
 varSetup = data;
 EEPROM.write(addr, lowByte(varSetup));
 EEPROM.write(addr + 1, highByte(varSetup));
 Serial.println(varName + " = " + varSetup);
} // EEPROMSaveInt

// #FUNCTION# ===================================================================================================
// Name...........: EEPROMSaveString
// Description....: Запись в EEPROM строки
// Syntax.........: EEPROMSaveString()
// ==============================================================================================================
void EEPROMSaveString(String data, String& varSetup, int maxLength, String varName, int addr) {
 data.replace(" ", "");
 if ((data.length() == 0)||(data == varSetup)) return;
 if (data.length() > maxLength) {
  if (varName.length() > 0)
   errorStr = errorStr + varName + " не должен быть больше " + maxLength + " символов<br>";
  return;
 }
 varSetup = data;
 for (byte i = 0; i < varSetup.length(); i++) EEPROM.write(addr + i, varSetup[i]);
 EEPROM.write(addr + varSetup.length(), 0x00);
 Serial.println(varName + " = " + varSetup);
} // EEPROMSaveString

