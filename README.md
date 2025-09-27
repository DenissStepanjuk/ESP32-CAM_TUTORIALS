# ESP32-CAM_TUTORIALS

Не скрываю, что я любитель, так что если у вас есть замечание, рекомендации, правки и идеи, то прошу поделится ими в телеграм-чате:

https://t.me/KBISDB


## BOARDS MANAGER:

esp32 by Espressif Systems: https://github.com/espressif/arduino-esp32


## LIBRARY MANAGER:

ArduinoBLE by Arduino: https://docs.arduino.cc/libraries/arduinoble/

ESP Async WebServer by Me-No-Dev: https://github.com/mathieucarbou/ESPAsyncWebServer

AsyncTCP by dvarrel: https://github.com/dvarrel/AsyncTCP

WebSockets by Markus Sattler: https://github.com/Links2004/arduinoWebSockets

ArduinoJson by Benoit Blanchon: https://arduinojson.org/?utm_source=meta&utm_medium=library.properties

MicroTFLite by johnosbb and TensorFlow Authors: https://github.com/johnosbb/MicroTFLite


### ESP32-CAM 01: Захват кадра

В этом видео начинается серия уроков по работе с модулем **ESP32-CAM** — микроконтроллером ESP32 со встроенной камерой. Автор показывает базовый пример захвата изображения и его отображения на компьютере через веб-интерфейс.

**Основные моменты ролика:**

* Обзор модуля **ESP32-CAM** и его возможностей для проектов компьютерного зрения.
* Подключение платы к ПК через программатор с microUSB.
* Подготовка Arduino IDE: установка плат ESP32 и необходимых библиотек (WiFi, ESP WebServer, WebSockets, ArduinoJson, ESP32 Camera).
* Настройка камеры через файл `camera_config.h` (пины, разрешение кадра, формат JPEG, качество изображения).
* Подключение ESP32-CAM к локальной Wi-Fi сети и получение IP-адреса.
* Создание простого **веб-сервера** с HTML-страницей и кнопкой *Capture* для захвата кадра.
* Использование **WebSockets** для обмена данными между микроконтроллером и браузером.
* Реализация логики: при нажатии кнопки кадр фиксируется и отображается в браузере.
* Демонстрация работы: однократный захват кадра и модификация кода для стриминга видео.


## License

This project is released under the [CC0 1.0 Universal (Public Domain Dedication)](https://creativecommons.org/publicdomain/zero/1.0/) license.  
You are free to use, modify, and distribute it for any purpose, without asking for permission.
