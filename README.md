# SmartHome-SensorLPG
Блок датчика газа для "Умного дома"

Функции устройства:
1. Чтение показаний датчика углеводородных газов и отправка их брокеру MQTT
2. Чтение показаний датчика температуры и влажности и отправка их брокеру MQTT
3. Сигнализация о превышении критического уровня углеводородных газов
4. Контроль и настройка устройства через web-интерфейс
5. Сброс настроек устройства при удержании специальной кнопк

Аппаратные средства:
- Контроллер <strong><a href="https://wiki.wemos.cc/products:d1:d1_mini">WeMos Di mini</a></strong>
- Датчик углеводородных газов <strong>MQ-6</strong>
- Датчик температуры и влажности <strong>DHT11</strong>

Подключение <strong>WeMos Di mini</strong>:
 pin D0 -> Reset Button
 pin D1 -> Error LED (Red)
 pin D2 -> Power LED (Green)
 pin D3 -> WiFi LED (Blue)
 pin D4 -> DHT11 (BuiltIn LED)
 pin D5 -> Piezo buzzer
 pin D6 -> NC
 pin D7 -> NC
 pin D8 -> NC
 pin A0 -> MQ6

В папке <strong>Case</strong> - файлы под печать для копруса устройства.
