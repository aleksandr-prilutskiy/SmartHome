//  Filename:     _Probes.ino
//  Description:  Система "Умный дом". Функции для работы с датчиками
//  Author:       Aleksandr Prilutskiy
//  Date:         10.04.2019

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
 delay(250);
 if (isnan(t) || isnan(h)) {
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
 lastTemperature = round(10 * t) / 10;
 lastHumidity    = round(10 * h) / 10;
 sumTemperature  = sumTemperature +  t;
 sumHumidity     = sumHumidity + h;
 countDHT11++;
 Serial.println((String) "Temperature = " + lastTemperature + "C");
 Serial.println((String) "Humidity = " + lastHumidity + "%");
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
 float g = 3.3 * analogRead(pinMQ6) / 5;
 delay(250);
 if (g < 0) {
  digitalWrite(ledPower, HIGH);
  Serial.println("Error: Sensor LPG Read Error");
  for (int i = 0; i < 3; i++) {
   digitalWrite(ledError, HIGH);
   delay(250);
   digitalWrite(ledError, LOW);
   delay(250);
  }
  return;
 }
 lastLPG = round(10 * g) / 10;
 sumLPG  = sumLPG + g;
 countLPG++;
 Serial.println((String) "LPG = " + lastLPG + " PPM");
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
 digitalWrite(ledWiFi, HIGH);
 if (!client.connected()) MQTTReconnect();
 if (!client.connected()) return;
 if (countDHT11 > 0) {
  if (MQTT_Temperature.length() > 0) {
   snprintf(msg, 50, "%f", floor(10 * sumTemperature / countDHT11) / 10);
   client.publish(MQTT_Temperature.c_str(), msg);
  }
  if (MQTT_Humidity.length() > 0) {
   snprintf(msg, 50, "%f", floor(10 * sumHumidity / countDHT11) / 10);
   client.publish(MQTT_Humidity.c_str(), msg);
  }
 }
 if ((countLPG > 0) && (MQTT_LPG.length() > 0)) {
  snprintf(msg, 50, "%f", floor(10 * sumLPG / countLPG) / 10);
  client.publish(MQTT_LPG.c_str(), msg);
 }
 digitalWrite(ledWiFi, LOW);
 sumTemperature   = 0;
 sumHumidity      = 0;
 sumLPG           = 0;
 countDHT11       = 0;
 countLPG         = 0;
} // ProbesSendData

