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
// Библиотека предоставляет функции для преобразования изображений между различными форматами.
#include "img_converters.h"

// Данные WiFi сети.
const char* ssid = "WiFi_Name_Vremenoi";
const char* password = "WiFi_Par0l";

// Экземпляр вебсервера ссылается на 80 порт.
AsyncWebServer server(80);
// Экземпляр WebSocket-сервера ссылается на 81 порт.
WebSocketsServer webSocket = WebSocketsServer(81);

// Переменная отображает состояние кнопки отвечающей за фото.
bool capture = false;

/** В документе реализованы функции позволяющие применить фильтр к изображению. **/
#include "convolution.h"
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

      // Преобразуем исходное изображение в чёрно-белое JPEG изображение.
      jpeg2gray(true, frame->buf, frame->len, buf_img, len_img, img_width, img_height);
      // Преобразуем исходное изображение в 2D матрицу содержащую чёрно-белое изображение.
      jpeg2gray(false, frame->buf, frame->len, buf_img_gray, len_img_gray, img_width, img_height);
      // Освободить зарезервированную память под исходное изображение.
      esp_camera_fb_return(frame);
      // Обозначить на вебстраничке что следующим будет принято серое изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 1);
      // Отправить чёрно-белое изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);
      
      // Ядро для свёртки, тождественное преобразование.
      double kernel_identical_transformation[3][3] = {
        {0, 0, 0},
        {0, 1, 0},
        {0, 0, 0}
      };
      // Применим к изображению ядро для тождественного преобразования.
      apply2DKernel_3x3(true, buf_img_gray, len_img_gray, buf_img, len_img, img_width, img_height, kernel_identical_transformation);
      // Обозначить на вебстраничке что следующим будет принято тождественно преобразованое изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 2);
      // Отправить тождественно преобразованое изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);

      // Ядро для свёртки, предаёт эффект размытия изображению.
      double kernel_blur[3][3] = {
        {0.0625, 0.125, 0.0625},
        {0.125, 0.25, 0.125},
        {0.0625, 0.125, 0.0625}
      };
      // Применим к изображению ядро для размытия.
      apply2DKernel_3x3(true, buf_img_gray, len_img_gray, buf_img, len_img, img_width, img_height, kernel_blur);
      // Обозначить на вебстраничке что следующим будет принято размытое изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 3);
      // Отправить размытое изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);

      // Ядро для свёртки, предаёт эффект увеличения резкости изображению.
      double kernel_increasing_sharpness[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
      };
      // Применим к изображению ядро для увеличения резкости.
      apply2DKernel_3x3(true, buf_img_gray, len_img_gray, buf_img, len_img, img_width, img_height, kernel_increasing_sharpness);
      // Обозначить на вебстраничке что следующим будет принято изображение с увеличеной резкостью.
      sendJson(jsonString, doc_tx, "change_img_type", 4);
      // Отправить изображение с увеличеной резкостью на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);

      // Ядро для свёртки, dilation - расширение.
      double kernel_dilation[3][3] = {
        {0, 1, 0},
        {1, 1, 1},
        {0, 1, 0}
      };

      // Применим к изображению ядро для расширения границ.
      apply2DKernel_3x3(true, buf_img_gray, len_img_gray, buf_img, len_img, img_width, img_height, kernel_dilation);
      // Обозначить на вебстраничке что следующим будет принято изображение с расширеными границами.
      sendJson(jsonString, doc_tx, "change_img_type", 5);
      // Отправить изображение с расширеными границами на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);

      // Ядро для свёртки, используется для выделения вертикальных границ.
      double kernel_vertical_borders[3][3] = {
        {0, 0, 0},
        {1, -2, 1},
        {0, 0, 0}
      };
      // Применим к изображению ядро для выделения вертикальных границ.
      apply2DKernel_3x3(true, buf_img_gray, len_img_gray, buf_img, len_img, img_width, img_height, kernel_vertical_borders);
      // Обозначить на вебстраничке что следующим будет принято изображение с выделеными вертикальными границами.
      sendJson(jsonString, doc_tx, "change_img_type", 6);
      // Отправить изображение с выделеными вертикальными границами на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);

      // Ядро для свёртки, используется для выделения горизонтальных границ.
      double kernel_horizontal_borders[3][3] = {
        {0, 1, 0},
        {0, -2, 0},
        {0, 1, 0}
      };
      // Применим к изображению ядро для выделения горизонтальных границ.
      apply2DKernel_3x3(true, buf_img_gray, len_img_gray, buf_img, len_img, img_width, img_height, kernel_horizontal_borders);
      // Обозначить на вебстраничке что следующим будет принято изображение с выделеными горизонтальными границами.
      sendJson(jsonString, doc_tx, "change_img_type", 7);
      // Отправить изображение с выделеными горизонтальными границами на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);

      // Ядро для свёртки, используется для выделения границ.
      double kernel_borders[3][3] = {
        {0, 1, 0},
        {1, -4, 1},
        {0, 1, 0}
      };
      // Применим к изображению ядро для выделения границ.
      apply2DKernel_3x3(true, buf_img_gray, len_img_gray, buf_img, len_img, img_width, img_height, kernel_borders);
      // Обозначить на вебстраничке что следующим будет принято изображение с выделеными границами.
      sendJson(jsonString, doc_tx, "change_img_type", 8);
      // Отправить изображение с выделеными границами на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);


      // Ядра для свёртки, перекрёстный оператор Робертса, используется для выделения границ.
      double kernel_roberts_1[2][2] = {
        {1, 0},
        {0, -1},
      };
      double kernel_roberts_2[2][2] = {
        {0, 1},
        {-1, 0},
      };
      // Применим к изображению два ядра для выделения границ.
      applyTwo2DKernel_2x2(true, buf_img_gray, len_img_gray, buf_img, len_img, img_width, img_height, kernel_roberts_1, kernel_roberts_2);
      // Обозначить на вебстраничке что следующим будет принято изображение с выделеными границами.
      sendJson(jsonString, doc_tx, "change_img_type", 9);
      // Отправить изображение с выделеными границами на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);


      // Ядро для свёртки, оператор Собеля, используется для выделения границ.
      double kernel_sobel_horizontal[3][3] = {
        {1, 0, -1},
        {2, 0, -2},
        {1, 0, -1}
      };
      // Ядро для свёртки, оператор Собеля, используется для выделения границ.
      double kernel_sobel_vertical[3][3] = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
      };
      applyTwo2DKernel_3x3(true, buf_img_gray, len_img_gray, buf_img, len_img, img_width, img_height, kernel_sobel_horizontal, kernel_sobel_vertical);
      // Обозначить на вебстраничке что следующим будет принято изображение с выделеными границами.
      sendJson(jsonString, doc_tx, "change_img_type", 10);
      // Отправить изображение с выделеными границами на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);


      // Ядро для свёртки, оператор Прюитта, используется для выделения границ.
      double kernel_prewitt_horizontal[3][3] = {
        {1, 0, -1},
        {1, 0, -1},
        {1, 0, -1}
      };
      // Ядро для свёртки, оператор Прюитта, используется для выделения границ.
      double kernel_prewitt_vertical[3][3] = {
        {1, 1, 1},
        {0, 0, 0},
        {-1, -1, -1}
      };

      applyTwo2DKernel_3x3(true, buf_img_gray, len_img_gray, buf_img, len_img, img_width, img_height, kernel_prewitt_horizontal, kernel_prewitt_vertical);
      // Обозначить на вебстраничке что следующим будет принято изображение с выделеными границами.
      sendJson(jsonString, doc_tx, "change_img_type", 11);
      // Отправить изображение с выделеными границами на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);

      // Освобождаем буфер чёрно-белого изображения.
      if (buf_img_gray) free(buf_img_gray);
    }
    // Фото было сделано, обработано и отправлено клиенту.
    capture = false;
    }
    delay(30);
  }

