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
// Библиотека предоставляет функции для преобразования изображений между различными форматами.
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

/** В документе реализованы функции позволяющие применить фильтр к изображению. **/
#include "lineBordersDetection.h"
/** В документе реализлвана функция webSocketEvent для обработки данных 
полученых через соеденение сокетов. **/
#include "socketConnection.h"

void setup() {
  // Последовательный порт (serial port).
  Serial.begin(115200);

  // Инициализация модуля камеры OV2640 (CAMERA_MODEL_AI_THINKER).
  //cam_init(FRAMESIZE_VGA, PIXFORMAT_JPEG, 10);
  cam_init(FRAMESIZE_QVGA, PIXFORMAT_JPEG, 25);

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

    // Инициализировать переменную для хранение размера буфера содержащего чёрно-белое изображение.
    size_t len_img_gray;
    // Инициализировать буфер под хранение чёрно-белого изображения.
    uint8_t *buf_img_gray;

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
      
      // Ширина изображения:
      int img_width = frame->width;
      // Высота изображения:
      int img_height = frame->height;

      // Обозначить на вебстраничке что следующим будет принято исходное изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 0);
      // Отправить исходное изображение на вебстраничку.
      webSocket.broadcastBIN(frame->buf, frame->len);

      // Преобразуем исходное изображение в 2D матрицу содержащую чёрно-белое изображение.
      jpeg2gray(false, frame->buf, frame->len, buf_img_gray, len_img_gray, img_width, img_height);
      // Преобразуем исходное изображение в чёрно-белое JPEG изображение.
      bool ok_gray = fmt2jpg(buf_img_gray, len_img_gray, img_width, img_height, PIXFORMAT_GRAYSCALE, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будет принято чёрно-белое изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 1);
      // Отправить чёрно-белое изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);

      //  Горизонталь - координата (Y) по которой будет расчитыватся разница между пикселями.
      int verticalCoordinate_1 = img_height / 6;
      int verticalCoordinate_2 = img_height / 2;
      int verticalCoordinate_3 = img_height / 6 * 5;

      //  Переменная в которую будет записана координата (X) левой границы линии.
      int leftBorder_1;
      int leftBorder_2;
      int leftBorder_3;

      //  Переменная в которую будет записана координата (X) центра линии.
      int center_1;
      int center_2;
      int center_3;

      //  Переменная в которую будет записана координата (X) правой границы линии.
      int rightBorder_1;
      int rightBorder_2;
      int rightBorder_3;

      //  Массив в который будут записаны все значения разницы между парами пикселей.
      int diffValuesArray_1[320 - 1];
      int diffValuesArray_2[320 - 1];
      int diffValuesArray_3[320 - 1];

      // Найти границы для линии в трёх местах.
      findBorders(buf_img_gray, len_img_gray, img_width, img_height, verticalCoordinate_1, leftBorder_1, center_1, rightBorder_1, diffValuesArray_1);
      findBorders(buf_img_gray, len_img_gray, img_width, img_height, verticalCoordinate_2, leftBorder_2, center_2, rightBorder_2, diffValuesArray_2);
      findBorders(buf_img_gray, len_img_gray, img_width, img_height, verticalCoordinate_3, leftBorder_3, center_3, rightBorder_3, diffValuesArray_3);



      // Вывести все найденые параметры в последовательный порт.
      Serial.println("Parameters for Y = " + String(verticalCoordinate_1) + ":");
      Serial.println("Left Border: " + String(leftBorder_1) + "  Center:" + String(center_1) + "  Right border:" + String(rightBorder_1));
      for(int i = 0; i < 320 - 1; i += 1){
        Serial.print("X(" + String(i) +"): " + String(diffValuesArray_1[i]) + " || ");
      }
      Serial.println("");

      Serial.println("Parameters for Y = " + String(verticalCoordinate_2) + ":");
      Serial.println("Left Border: " + String(leftBorder_2) + "  Center:" + String(center_2) + "  Right border:" + String(rightBorder_2));
      for(int i = 0; i < 320 - 1; i += 1){
        Serial.print("X(" + String(i) + "): " + String(diffValuesArray_2[i]) + " || ");
      }
      Serial.println("");

      Serial.println("Parameters for Y = " + String(verticalCoordinate_3) + ":");
      Serial.println("Left Border: " + String(leftBorder_3) + "  Center:" + String(center_3) + "  Right border:" + String(rightBorder_3));
      for(int i = 0; i < 320 - 1; i += 1){
        Serial.print("X(" + String(i) + "): " + String(diffValuesArray_3[i]) + " || ");
      }
      Serial.println("");


      // Освобождаем буфер для чёрно-белого изображения.
      if (buf_img_gray) free(buf_img_gray);


      // Переменная для хранения размера изображения в формате RGB888.
      size_t rgb_len = img_width * img_height * 3;
      // Инициализировать буфер под хранение изображения в формате RGB888.
      uint8_t *rgb_buf = (uint8_t *)ps_malloc(rgb_len);
      // Конвертируем сжатое в формате JPEG изображение в изображение в формате RGB888.
      bool okey = fmt2rgb888(frame->buf, frame->len, frame->format, rgb_buf);
      // Освободить зарезервированную память под исходное изображение.
      esp_camera_fb_return(frame);


      // Чтобы рисовать при помощи библиотеки fb_gfx требуется
      // инициализировать структуру данных для хранения подробной информации об изображении.
      fb_data_t frame_data;
      // Ширина изображения.
      frame_data.width = img_width;
      // Высота изображения
      frame_data.height = img_height;
      // Кол-во байт на один пиксель.
      frame_data.bytes_per_pixel = 3;
      // Формат изображения.
      frame_data.format = FB_RGB888;
      // Буфер хранящий изобржение.
      frame_data.data = rgb_buf;

      // Палитра цветов, которыми можно рисовать: красный, зелёный, чёрный, тёмный жёлтый, коричневый, жёлтый.
      uint32_t fb_gfx_color_red = 0b0000011111100000;
      uint32_t fb_gfx_color_green = 0b1111100000011111; 
      uint32_t fb_gfx_color_black = 0b0000000000000000;
      uint32_t fb_gfx_color_dark_yellow = 0b1010101010101010;
      uint32_t fb_gfx_color_brown = 0b0000010101010000;
      uint32_t fb_gfx_color_yellow = 0b1111111111100000;

      /**  Начертить вертикальные линии по найденым параметрам чтобы обозначить найденые границы на изображении (fb_gfx_drawFastVLine).
        - camera_fb_t *fb - изображение на котором будем рисовать.
        - int32_t x - координата начала линии.
        - int32_t y - координата начала линии.
        - int32_t h - высота линии.
        - uint32_t color - цвет которым будем рисовать.**/

      // 1 ==================================================================================================
      fb_gfx_drawFastVLine(&frame_data, leftBorder_1, verticalCoordinate_1 - 20, 40, fb_gfx_color_yellow);
      fb_gfx_drawFastVLine(&frame_data, leftBorder_1+1, verticalCoordinate_1 - 20, 40, fb_gfx_color_yellow);

      fb_gfx_drawFastVLine(&frame_data, rightBorder_1, verticalCoordinate_1 - 20, 40, fb_gfx_color_green);
      fb_gfx_drawFastVLine(&frame_data, rightBorder_1+1, verticalCoordinate_1 - 20, 40, fb_gfx_color_green);

      fb_gfx_drawFastVLine(&frame_data, center_1, verticalCoordinate_1 - 10, 20, fb_gfx_color_red);


      // 2 ==================================================================================================
      fb_gfx_drawFastVLine(&frame_data, leftBorder_2, verticalCoordinate_2 - 20, 40, fb_gfx_color_yellow);
      fb_gfx_drawFastVLine(&frame_data, leftBorder_2+1, verticalCoordinate_2 - 20, 40, fb_gfx_color_yellow);

      fb_gfx_drawFastVLine(&frame_data, rightBorder_2, verticalCoordinate_2 - 20, 40, fb_gfx_color_green);
      fb_gfx_drawFastVLine(&frame_data, rightBorder_2+1, verticalCoordinate_2 - 20, 40, fb_gfx_color_green);

      fb_gfx_drawFastVLine(&frame_data, center_2, verticalCoordinate_2 - 10, 20, fb_gfx_color_red);


      // 3 ==================================================================================================
      fb_gfx_drawFastVLine(&frame_data, leftBorder_3, verticalCoordinate_3 - 20, 40, fb_gfx_color_yellow);
      fb_gfx_drawFastVLine(&frame_data, leftBorder_3+1, verticalCoordinate_3 - 20, 40, fb_gfx_color_yellow);

      fb_gfx_drawFastVLine(&frame_data, rightBorder_3, verticalCoordinate_3 - 20, 40, fb_gfx_color_green);
      fb_gfx_drawFastVLine(&frame_data, rightBorder_3+1, verticalCoordinate_3 - 20, 40, fb_gfx_color_green);

      fb_gfx_drawFastVLine(&frame_data, center_3, verticalCoordinate_3 - 10, 20, fb_gfx_color_red);

      // Конвертируем изображение, на котором отмечены все найденые границы, из формата RGB888 в JPEG формат.
      bool ok = fmt2jpg(rgb_buf, rgb_len, img_width, img_height, PIXFORMAT_RGB888, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будет принято тождественно преобразованое изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 2);
      // Отправить тождественно преобразованое изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);
      // Освобождаем буфер.
      if (rgb_buf) free(rgb_buf);
    }
    // Фото было сделано, обработано и отправлено клиенту.
    capture = false;
    }
    delay(30);
  }

