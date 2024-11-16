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
// Библиотека для изменения форматов изображений.
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

/** В документе реализлвана функция . **/
#include "draw.h"
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

    // Произвести захват изображения с матрицы камеры.
    camera_fb_t *frame = esp_camera_fb_get();
    esp_camera_fb_return(frame);
    frame = esp_camera_fb_get();

    // Если захват кадра был произведён успешно.
    if (frame) {

      // Обозначить на вебстраничке что следующим будет принято исходное изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 0);
      // Если удалось представить изображение в сжатом виде, тогда отправляем его на вебстраничку.
      webSocket.broadcastBIN(frame->buf, frame->len);


      // Инициализировать переменную для хранение размера цветовой матрицы RGB.
      size_t rgb_buf_len = 640 * 480 * 3;
      // Инициализировать буфер под хранение цветовой матрицы RGB.
      uint8_t *rgb_buf = (uint8_t *)ps_malloc(rgb_buf_len);

      // Конвертируем сжатое в формате JPEG изображение в цветовую матрицу RGB.
      bool okey = fmt2rgb888(frame->buf, frame->len, frame->format, rgb_buf);
      // Если удалось представить изображение в виде цветовой матрицы, чтоб каждому пикселю соответствовало 3 значения: R, G, B.
      if (okey) {

        // Закрасить пиксели в ручном режиме:
        // Пиксель номер 1:
        rgb_buf[0] = 0; // синяя компонента.
        rgb_buf[1] = 255; // зелёная компонента.
        rgb_buf[2] = 0; // красная компонента.
        // Пиксель номер 2:
        rgb_buf[3] = 0; // синяя компонента.
        rgb_buf[4] = 255; // зелёная компонента.
        rgb_buf[5] = 0; // красная компонента.
        // Пиксель номер 3:
        rgb_buf[6] = 0; // синяя компонента.
        rgb_buf[7] = 255; // зелёная компонента.
        rgb_buf[8] = 0; // красная компонента.

        // Пиксель номер 641:
        rgb_buf[1920] = 0; // синяя компонента.
        rgb_buf[1921] = 255; // зелёная компонента.
        rgb_buf[1922] = 0; // красная компонента.
        // Пиксель номер 642:
        rgb_buf[1923] = 0; // синяя компонента.
        rgb_buf[1924] = 255; // зелёная компонента.
        rgb_buf[1925] = 0; // красная компонента.
        // Пиксель номер 643:
        rgb_buf[1926] = 0; // синяя компонента.
        rgb_buf[1927] = 255; // зелёная компонента.
        rgb_buf[1928] = 0; // красная компонента.

        // Чтобы рисовать при помощи библиотеки fb_gfx требуется
        // инициализировать структуру данных для хранения подробной информации об изображении.
        fb_data_t frame_data;
        // Ширина изображения.
        frame_data.width = 640;
        // Высота изображения
        frame_data.height = 480;
        // Кол-во байт на один пиксель.
        frame_data.bytes_per_pixel = 3;
        // Формат изображения.
        frame_data.format = FB_RGB888;
        // Буфер хранящий изобржение.
        frame_data.data = rgb_buf;

        /** Нарисовать закрашеный прямоугольник (fb_gfx_fillRect).
              - camera_fb_t *fb - изображение на котором будем рисовать.
              - int32_t x - координата левого верхнего угла примоугольника.
              - int32_t y - координата левого верхнего угла примоугольника.
              - int32_t w - ширина прямоугольника.
              - int32_t h - высота прямоугольника.
              - uint32_t color - цвет которым будем рисовать.**/
        uint32_t fb_gfx_color_red = 0b0000011111100000;
        fb_gfx_fillRect(&frame_data, 100, 400, 70, 30, fb_gfx_color_red);

        /**  Нарисовать горизонтальную линию (fb_gfx_drawFastHLine).
              - camera_fb_t *fb - изображение на котором будем рисовать.
              - int32_t x - координата начала линии.
              - int32_t y - координата начала линии.
              - int32_t w - ширина линии.
              - uint32_t color - цвет которым будем рисовать.**/
        uint32_t fb_gfx_color_brown = 0b0000010101010000;
        fb_gfx_drawFastHLine(&frame_data, 30, 30, 600, fb_gfx_color_brown);

        /**  Нарисовать вертикальную линию (fb_gfx_drawFastVLine).
          - camera_fb_t *fb - изображение на котором будем рисовать.
          - int32_t x - координата начала линии.
          - int32_t y - координата начала линии.
          - int32_t h - высота линии.
          - uint32_t color - цвет которым будем рисовать.**/
        uint32_t fb_gfx_color_yellow = 0b1111111111100000;
        fb_gfx_drawFastVLine(&frame_data, 30, 30, 440, fb_gfx_color_yellow);
        
        /**  Вставить букву (fb_gfx_putc).
          - camera_fb_t *fb - изображение на котором будем рисовать.
          - int32_t x - координата куда вставить.
          - int32_t y - координата куда вставить.
          - uint32_t color - цвет которым будем печатать.
          - unsigned char c - буква.**/
        uint32_t fb_gfx_color_green = 0b1111100000011111;
        fb_gfx_putc(&frame_data, 100, 100, fb_gfx_color_green, 'H');

        /**  Вставить текст (fb_gfx_print).
          - camera_fb_t *fb - изображение на котором будем рисовать.
          - int32_t x - координата начала линии.
          - int32_t y - координата начала линии.
          - uint32_t color - цвет которым будем печатать.
          - const char * str - текст.**/
        uint32_t fb_gfx_color_black = 0b0000000000000000;
        fb_gfx_print(&frame_data, 100, 200, fb_gfx_color_black, "ESP32-CAM");

        /**  Вставить форматированый текст (fb_gfx_printf).
          - camera_fb_t *fb - изображение на котором будем рисовать.
          - int32_t x - координата начала линии.
          - int32_t y - координата начала линии.
          - uint32_t color - цвет которым будем печатать.
          - const char * str - форматированый текст.
          - параметр для иньекции. **/
        uint32_t fb_gfx_color_dark_yellow = 0b1010101010101010;
        fb_gfx_printf(&frame_data, 100, 300, fb_gfx_color_dark_yellow, "Computer vision");


        // Нарисовать круг.
        drawCircle(rgb_buf, 100, 400, 50, 640, 480);
        // Нарисовать что-то.
        risovalka(rgb_buf, 420, 200, 640, 480, 120, 120);
        
        
        // Инициализировать буфер под хранение сжатого изображения в формате JPEG.
        size_t jpg_buf_len = 0;
        // Инициализировать переменную для хранение размера буфера.
        uint8_t *jpg_buf = nullptr;

        // Конвертируем цветовую матрицу RGB в сжатое в формате JPEG изображение.
        bool ok = fmt2jpg(rgb_buf, rgb_buf_len, frame->width, frame->height, PIXFORMAT_RGB888, 80, &jpg_buf, &jpg_buf_len);
        // Обозначить на вебстраничке что следующим будет принято изображение с рисунком.
        sendJson(jsonString, doc_tx, "change_img_type", 1);
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

