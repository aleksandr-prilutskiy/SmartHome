# SmartHome-SensorLPG
Блок датчика бытового газа для "Умного дома"<br>
<br>
Функции устройства:<br>
1. Чтение показаний датчика углеводородных газов и отправка их брокеру MQTT<br>
2. Чтение показаний датчика температуры и влажности и отправка их брокеру MQTT<br>
3. Сигнализация о превышении критического уровня углеводородных газов<br>
4. Контроль и настройка устройства через web-интерфейс<br>
5. Сброс настроек устройства при удержании специальной кнопк<br>
<br>
Аппаратные средства:<br>
- Контроллер <strong><a href="https://wiki.wemos.cc/products:d1:d1_mini">WeMos Di mini</a></strong><br>
- Датчик углеводородных газов <strong>MQ-6</strong><br>
- Датчик температуры и влажности <strong>DHT11</strong><br>
<br>
Подключение <strong>WeMos Di mini</strong>:<br>
 pin D0 -> Reset Button<br>
 pin D1 -> Error LED (Red)<br>
 pin D2 -> Power LED (Green)<br>
 pin D3 -> WiFi LED (Blue)<br>
 pin D4 -> DHT11 (BuiltIn LED)<br>
 pin D5 -> Piezo buzzer<br>
 pin D6 -> NC<br>
 pin D7 -> NC<br>
 pin D8 -> NC<br>
 pin A0 -> MQ6<br>
<br>
В папке <strong>Case</strong> - файлы под печать для копруса устройства.<br>
<img src="Scheme_bb.jpg"><br>
Резисторы:<br>
- DHT11 и кнопrf = 10KOm<br>
- Светодиоды = 220Om..10KOm
