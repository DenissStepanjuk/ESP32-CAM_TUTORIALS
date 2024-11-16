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
// Интерфейс для рисования на различных экранах или буферах кадров.
// https://github.com/espressif/esp-who/blob/master/components/fb_gfx/include/fb_gfx.h
#include "fb_gfx.h"

#include "soc/soc.h" // Используется при нестабильном питании и перезапуске
#include "soc/rtc_cntl_reg.h" //используется при нестабильном питании и перезапуске
#include <Arduino.h>
#include "img_converters.h"

// Данные WiFi сети.
const char* ssid = "WiFi_Name_Vremenoi";
const char* password = "WiFi_Par0l_Beremen0i";

// Экземпляр вебсервера ссылается на 80 порт.
AsyncWebServer server(80);
// Экземпляр WebSocket-сервера ссылается на 81 порт.
WebSocketsServer webSocket = WebSocketsServer(81);

// Переменная отображает состояние кнопки отвечающей за фото.
bool capture = false;

/** В документе реализлваны функции для масштабирования изображений. **/
#include "resize.h"
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

    // Инициализировать переменную для хранение размера буфера содержащего изображение.
    size_t len_img;
    // Инициализировать буфер под хранение изображения.
    uint8_t *buf_img;

    // Произвести захват изображения с матрицы камеры.
    camera_fb_t *frame = esp_camera_fb_get();
    esp_camera_fb_return(frame);
    frame = esp_camera_fb_get();

    // Если захват кадра был произведён успешно.
    if (frame) {
      // Обозначить на вебстраничке что следующим будет принято исходное изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 0);
      // Отправить исходное изображение на вебстраничку.
      webSocket.broadcastBIN(frame->buf, frame->len);

      // Инициализировать переменную для хранение размера изображения в формате RGB888.
      size_t len_rgb = frame->width * frame->height * 3;
      // Инициализировать буфер под хранение изображения в формате RGB888.
      uint8_t *buf_rgb = (uint8_t *)ps_malloc(len_rgb);
      // Преобразуем исходное изображение из JPEG в формат RGB888.
      bool okey = fmt2rgb888(frame->buf, frame->len, PIXFORMAT_JPEG, buf_rgb);
      // Освободить зарезервированную память под исходное изображение.
      esp_camera_fb_return(frame);


      // Переменная для хранения размера буфера содержащего преобразованное изображение.
      size_t len_resized;
      // Буфер под хранение преобразованного изображения.
      uint8_t *buf_resized;
      // Ширина для нового изображения.
      int set_width = 1000;
      // Высота для нового изображения.
      int set_height = 1000;


      // Изменить размер изображения.
      resizeRGBMatrix(buf_rgb, len_rgb, buf_resized, len_resized, frame->width, frame->height, set_width, set_height);
      // Освобождаем буфер.
      //if (buf_rgb) free(buf_rgb);
      // Преобразуем увеличеное изображение в JPEG.
      bool ok_upsample = fmt2jpg(buf_resized, len_resized, set_width, set_height, PIXFORMAT_RGB888, 80, &buf_img, &len_img);
      // Освобождаем буфер.
      if (buf_resized) free(buf_resized);
      // Обозначить на вебстраничке что следующим будет принято увеличеное изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 1);
      // Отправить увеличеное изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);


      // Ширина для нового изображения.
      set_width = 199;
      // Высота для нового изображения.
      set_height = 102;
      // Уменьшаем изображение.
      resizeRGBMatrix(buf_rgb, len_rgb, buf_resized, len_resized, frame->width, frame->height, set_width, set_height);
      // Освобождаем буфер.
      if (buf_rgb) free(buf_rgb);
      // Преобразуем уменьшеное изображение в JPEG.
      bool ok_downsample = fmt2jpg(buf_resized, len_resized, set_width, set_height, PIXFORMAT_RGB888, 80, &buf_img, &len_img);
      // Освобождаем буфер.
      if (buf_resized) free(buf_resized);
      // Обозначить на вебстраничке что следующим будет принято уменьшеное изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 2);
      // Отправить уменьшеное изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);
      }
    // Фото было сделано и отправлено клиенту.
    capture = false;
    }
    delay(30);
  }

