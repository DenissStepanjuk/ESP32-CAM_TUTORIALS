/**
    Подготовил: Степанюк Денис Борисович

    YouTube:
    https://www.youtube.com/@Stepanjuk

    VK:
    https://vk.com/SDB07

    GitHub:
    https://github.com/DenissStepanjuk

    LinkedIn:
    https://www.linkedin.com/in/stepanjuk/

    Telegram:
    https://t.me/KBISDB
    https://t.me/SDBproduction

**/

// Распиновка и функция инициализации для модуля камеры OV2640 (CAMERA_MODEL_AI_THINKER).
#include "cameraConfig.h"
// Библиотека для работы с wifi подключением (standard library).
#include <WiFi.h>  
// Библиотека для создания и запуска веб-сервера, обработки HTTP-запросов, формирования и отправки HTTP-ответов клиенту.
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
// Библиотека позволяет устанавливать постоянное соединение между сервером и клиентом, что делает возможным двустороннюю передачу данных в реальном времени.
#include <WebSocketsServer.h> 
// Библиотека используется для работы с JSON (JavaScript Object Notation) данными.
#include <ArduinoJson.h>
// Документ хранящий код HTML странички.
#include "HomePage.h"

#include "soc/soc.h" // Используется при нестабильном питании и перезапуске
#include "soc/rtc_cntl_reg.h" //используется при нестабильном питании и перезапуске
#include <Arduino.h>
#include "img_converters.h"

// Данные WiFi сети.
const char* ssid = "WiFi_Name_Vremenoi";
const char* password = "WiFi_Par0l_Rodivshii";

// Экземпляр вебсервера ссылается на 80 порт.
AsyncWebServer server(80);
// Экземпляр WebSocket-сервера ссылается на 81 порт.
WebSocketsServer webSocket = WebSocketsServer(81);

// Переменная отображает состояние кнопки отвечающей за фото.
bool capture = false;

/** В документе реализлвана функция для определения цвета. **/
#include "colorDetector.h"
/** В документе реализлвана функция webSocketEvent для обработки данных 
полученых через соеденение сокетов. **/
#include "socketConnection.h"

void setup() {
  // Последовательный порт (serial port).
  Serial.begin(115200);

  // Инициализация модуля камеры OV2640 (CAMERA_MODEL_AI_THINKER).
  cam_init(FRAMESIZE_VGA, PIXFORMAT_JPEG, 10);

  // Подключение к WiFi.
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  // Вывод IP адреса в последовательный порт.
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());


  /** Инициализация вебсервера (загружаем HTML страничку на WebServer, делаем её корневой "/"):
        + "/" - корневая папка, 
        + HTTP_GET - HTTP-метод GET для запроса данных с сервера
        + [](AsyncWebServerRequest *request) {} - лямбда-функция
        + AsyncWebServerRequest *request - указатель на объект, 
          который содержит всю информацию о запросе, поступившем на сервер.**/
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // отправить (200 - http код ответа, метод отправки по html, HTML страничка)
    request -> send(200, "text\html", getHTML());
  });

  // Запуск вебсокета.
  webSocket.begin();
  // При приёме данных от клиента контролером через соеденение вебсокетов передать данные функцие webSocketEvent для дальнейшей обработки.
  // Функция webSocketEvent реализована в документе "receiveData.h".
  webSocket.onEvent(webSocketEvent);

  // Запуск вебсервера.
  server.begin();

  // Настройка параметров фотографии для лучшей цветопередачи.
  sensor_t * s = esp_camera_sensor_get();
  s->set_saturation(s, 2);
  s->set_brightness(s, -1);
  s->set_contrast(s, -2);
  s->set_ae_level(s, 2);
}

void loop() {
  /** Метод webSocket.loop() обеспечивает:
        - Поддержание активного соединения с клиентами.
        - Обработку входящих данных от клиентов.
        - Обработку новых подключений и отключений клиентов.
        - Отправку данных клиентам, если это необходимо.**/
  webSocket.loop();

  if(capture){
    Serial.println("Take a photo");

    // Инициализировать буфер под хранение сжатого изображения в формате JPEG.
    size_t jpg_buf_len = 0;
    // Инициализировать переменную для хранение размера буфера.
    uint8_t *jpg_buf = nullptr;

    // Инициализировать переменную для хранение размера цветовой матрицы RGB.
    size_t rgb_buf_len = 640 * 480 * 3;
    // Инициализировать буфер под хранение цветовой матрицы RGB.
    uint8_t *rgb_buf = (uint8_t *)ps_malloc(rgb_buf_len);

    // Произвести захват изображения с матрицы камеры.
    camera_fb_t *frame = esp_camera_fb_get();
    esp_camera_fb_return(frame);
    frame = esp_camera_fb_get();

    // Если захват кадра был произведён успешно.
    if (frame) {
      // Конвертируем сжатое в формате JPEG изображение в цветовую матрицу RGB.
      bool okey = fmt2rgb888(frame->buf, frame->len, frame->format, rgb_buf);
      // Если удалось представить изображение в виде цветовой матрицы, чтоб каждому пикселю соответствовало 3 значения: R, G, B.
      if (okey) {
        // Определить какого цвета участок изображения.
        String area1 = detectColor(rgb_buf, 300, 200, 640, 480, 20, 20);
        // Отправить цвет который был определён клиенту на вебстраничку.
        sendJson(jsonString, doc_tx, "area_1", area1);
        Serial.println("First area: " + area1);
        // Конвертируем цветовую матрицу RGB в сжатое в формате JPEG изображение.
        bool ok = fmt2jpg(rgb_buf, rgb_buf_len, frame->width, frame->height, PIXFORMAT_RGB888, 80, &jpg_buf, &jpg_buf_len);
        // Если удалось представить изображение в сжатом виде, тогда отправляем его на вебстраничку.
        if (ok) webSocket.broadcastBIN(jpg_buf, jpg_buf_len);
        // Освобождаем буфер.
        if (jpg_buf) free(jpg_buf);
      }
      // Освобождаем буфер.
      free(rgb_buf);
      }
    // Освободить зарезервированную память под изображение.
    esp_camera_fb_return(frame);
    // Фото было сделано и отправлено клиенту.
    capture = false;
    }
    delay(30);
  }

