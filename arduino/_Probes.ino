//  Filename:     _Probes.ino
//  Description:  Система "Умный дом". Функции для работы с датчиками
//  Author:       Aleksandr Prilutskiy
//  Date:         15.04.2019

      float         sumTemperature   = 0;                    // Накапление значений температуры
      float         sumHumidity      = 0;                    // Накапление значений влажности
      uint16_t      countDHT11       = 0;                    // Счетчик значений датчика температуры и влажности
const uint32_t      timeoutDHT11     = 15000;                // Периодичность проверки температуры и влажности
      uint32_t      timerDHT11       = 0;                    // Таймер проверки датчика температуры и влажности

      float         sumLPG           = 0;                    // Накапление значений датчика углеводородных газов
      uint16_t      countLPG         = 0;                    // Счетчик значений датчика углеводородных газов
const uint32_t      timeoutLPG       = 15000;                // Периодичность проверки углеводородных газов
      uint32_t      timerLPG         = 0;                    // Таймер проверки датчика углеводородных газов

      uint32_t      timerAlarmBeep   = 0;                    // Таймер сигнализации уровня углеводородных газов
      uint32_t      timerSendData    = 0;                    // Таймер отправки результатов измерений
const uint32_t      timeoutSendData  = 30000;               // Периодичность отправки результатов измерений

      char          msg[50];

// #FUNCTION# ===================================================================================================
// Name...........: ProbesReadDHT11
// Description....: Получение и накопление значений температуры и влажности
// Syntax.........: ProbesReadDHT11()
// ==============================================================================================================
void ProbesReadDHT11() {
 if (((timerDHT11 == 0) && (millis() < 60000)) ||
     (abs(millis() - timerDHT11) < timeoutDHT11)) return;
 digitalWrite(ledPower, LOW);
 timerDHT11 = millis();
 float t = dht11.readTemperature();
 float h = dht11.readHumidity();
 if (isnan(t) || isnan(h)) {
  Serial.println("Error: Sensor DHT11 Read Error");
  for (int i = 0; i < 3; i++) {
   digitalWrite(ledError, HIGH);
   delay(100);
   digitalWrite(ledError, LOW);
   delay(100);
  }
  digitalWrite(ledPower, HIGH);
  return;
 }
 dtostrf(round(10 * t) / 10, 10, 2, msg);
 strTemperature = String(msg);
 strTemperature.trim();
 dtostrf(round(10 * h) / 10, 10, 2, msg);
 strHumidity = String(msg);
 strHumidity.trim();
 sumTemperature  = sumTemperature +  t;
 sumHumidity     = sumHumidity + h;
 countDHT11++;
 Serial.println((String) "Temperature = " + strTemperature + "C");
 Serial.println((String) "Humidity = " + strHumidity + "%");
 digitalWrite(ledPower, HIGH);
} // ProbesReadDHT11

// #FUNCTION# ===================================================================================================
// Name...........: ProbesReadLPG
// Description....: Получение и накопление значений датчика углеводородных газов
// Syntax.........: ProbesReadLPG()
// ==============================================================================================================
void ProbesReadLPG() {
 if (abs(millis() - timerLPG) < timeoutLPG) return;
 digitalWrite(ledPower, LOW);
 timerLPG = millis();
 float LPG = 3.3 * analogRead(pinMQ6) / 5;
 if (LPG < 0) {
  Serial.println("Error: Sensor LPG Read Error");
  for (int i = 0; i < 3; i++) {
   digitalWrite(ledError, HIGH);
   delay(100);
   digitalWrite(ledError, LOW);
   delay(100);
  }
  digitalWrite(ledPower, HIGH);
  return;
 }
 lastLPG = round(10 * LPG) / 10;
 dtostrf(lastLPG, 10, 2, msg);
 strLPG = String(msg);
 strLPG.trim();
 sumLPG  = sumLPG + LPG;
 countLPG++;
 Serial.println((String) "LPG = " + strLPG + " PPM");
 digitalWrite(ledPower, HIGH);
} // ProbesReadLPG

// #FUNCTION# ===================================================================================================
// Name...........: ProbesReadLPG
// Description....: Проверка значений датчика углеводородных газов. Отправка данных на сервер "Умного дома" и
//                  звуковой сигнал в случае превышения допустимого уровня
// Syntax.........: ProbesReadLPG()
// ==============================================================================================================
void ProbesCheckLPG() {
 digitalWrite(ledError, LOW);
 if ((alarmLPG == 0) || (lastLPG < alarmLPG)) return;
 digitalWrite(ledError, HIGH);
 if (abs(millis() - timerAlarmBeep) < 1000) return;
 timerAlarmBeep = millis();
 tone(pinBuzzer, 2500);
 Serial.print("WARNING! LPG = " + strLPG + " PPM");
 if (!client.connected()) MQTTReconnect();
 if (!client.connected()) return;
 digitalWrite(ledWiFi, LOW);
 client.publish(MQTT_LPG.c_str(), strLPG.c_str());
 Serial.println("MQTT publish: " + MQTT_LPG + "->" + strLPG);
 while (abs(millis() - timerAlarmBeep) < 1000) delay(10); 
 noTone(pinBuzzer);
 timerAlarmBeep = millis();
} // ProbesCheckLPG

// #FUNCTION# ===================================================================================================
// Name...........: ProbesSendData
// Description....: Отправка значений, полученных от датчиков на сервер "Умного дома"
// Syntax.........: ProbesSendData()
// ==============================================================================================================
void ProbesSendData() {
 if (abs(millis() - timerSendData) < timeoutSendData) return;
 timerSendData = millis();
 if ((countDHT11 == 0) && (countLPG == 0)) return;
 if (!client.connected()) MQTTReconnect();
 if (!client.connected()) return;
 digitalWrite(ledWiFi, LOW);
 if (countDHT11 > 0) {
  if (MQTT_Temperature.length() > 0) {
   dtostrf(round(10 * sumTemperature / countDHT11) / 10, 10, 2, msg);
   String Temperature = String(msg);
   Temperature.trim();
   client.publish(MQTT_Temperature.c_str(), Temperature.c_str());
   Serial.println("MQTT publish: " + MQTT_Temperature + "->" + Temperature);
  }
  if (MQTT_Humidity.length() > 0) {
   dtostrf(round(10 * sumHumidity / countDHT11) / 10, 10, 2, msg);
   String Humidity = String(msg);
   Humidity.trim();
   client.publish(MQTT_Humidity.c_str(), Humidity.c_str());
   Serial.println("MQTT publish: " + MQTT_Humidity + "->" + Humidity);
  }
 }
 if ((countLPG > 0) && (MQTT_LPG.length() > 0)) {
  dtostrf(round(10 * sumLPG / countLPG) / 10, 10, 2, msg);
  String LPG = String(msg);
  LPG.trim();
  client.publish(MQTT_LPG.c_str(), LPG.c_str());
  Serial.println("MQTT publish: " + MQTT_LPG + "->" + LPG);
 }
 sumTemperature   = 0;
 sumHumidity      = 0;
 sumLPG           = 0;
 countDHT11       = 0;
 countLPG         = 0;
} // ProbesSendData

