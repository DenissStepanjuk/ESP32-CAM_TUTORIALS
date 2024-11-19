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
const char* password = "WiFi_Par0l_Vosp";

// Экземпляр вебсервера ссылается на 80 порт.
AsyncWebServer server(80);
// Экземпляр WebSocket-сервера ссылается на 81 порт.
WebSocketsServer webSocket = WebSocketsServer(81);

// Переменная отображает состояние кнопки отвечающей за фото.
bool capture = false;

/** В документе реализованы функции позволяющие применить фильтр к изображению. **/
#include "houghTransformCircle.h"
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
  cam_init(FRAMESIZE_240X240, PIXFORMAT_JPEG, 63);
  //cam_init(FRAMESIZE_QVGA, PIXFORMAT_JPEG, 25);

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

      // Ширина изображения:
      int img_width = frame->width;
      // Высота изображения:
      int img_height = frame->height;

      // Обозначить на вебстраничке что следующим будет принято исходное изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 0);
      // Отправить исходное изображение на вебстраничку.
      webSocket.broadcastBIN(frame->buf, frame->len);

      // Инициализировать переменную для хранение размера буфера содержащего чёрно-белое  изображение.
      size_t len_matrix_gray;
      // Инициализировать буфер под хранение чёрно-белого изображения.
      uint8_t *buf_matrix_gray;

      // Преобразуем исходное изображение в 2D матрицу содержащую чёрно-белое изображение.
      jpeg2gray(false, frame->buf, frame->len, buf_matrix_gray, len_matrix_gray, img_width, img_height);
      // Освободить зарезервированную память под исходное изображение.
      //esp_camera_fb_return(frame);
      // Преобразуем  2D матрицу содержащую чёрно-белое изображение в JPEG.
      bool ok_gray = fmt2jpg(buf_matrix_gray, len_matrix_gray, img_width, img_height, PIXFORMAT_GRAYSCALE, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будет принято чёрно-белое изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 1);
      // Отправить чёрно-белое изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);


      // Инициализировать переменную для хранение размера буфера содержащего контуры изображения.
      size_t len_matrix_borders;
      // Инициализировать буфер под хранение контуры изображения.
      uint8_t *buf_matrix_borders;

      // Ядро для свёртки, используется для выделения границ.
      double kernel_borders[3][3] = {
        {0, 1, 0},
        {1, -4, 1},
        {0, 1, 0}
      };
      // Применим к изображению ядро для выделения границ.
      apply2DKernel_3x3(false, buf_matrix_gray, len_matrix_gray, buf_matrix_borders, len_matrix_borders, img_width, img_height, kernel_borders);
      // После свёртки изображение уменьшилось.
      img_width -= 2;
      img_height -= 2;
      // Преобразуем 2D матрицу контуров изображение в JPEG.
      bool ok_borders = fmt2jpg(buf_matrix_borders, len_matrix_borders, img_width, img_height, PIXFORMAT_GRAYSCALE, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующими будут приняты контуры изображения.
      sendJson(jsonString, doc_tx, "change_img_type", 2);
      // Отправить чёрно-белое изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);
      // Освобождаем буфер.
      if (buf_matrix_gray) free(buf_matrix_gray);






      // Удалить шум.
      deleteNoise(buf_matrix_borders, len_matrix_borders, img_width, img_height);
      // Удалить шум.
      deleteNoise(buf_matrix_borders, len_matrix_borders, img_width, img_height);
      // Преобразуем 2D матрицу контуров изображения без шума в JPEG.
      bool ok_borders_denoise = fmt2jpg(buf_matrix_borders, len_matrix_borders, img_width, img_height, PIXFORMAT_GRAYSCALE, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будут приняты контуры изображения.
      sendJson(jsonString, doc_tx, "change_img_type", 3);
      // Отправить чёрно-белое изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);


      // Инициализировать переменную для хранение размера буфера содержащего контуры изображения.
      size_t len_hough;
      // Инициализировать буфер под хранение контуры изображения.
      uint8_t *buf_hough;
      // Преобразовать контуры исходной фотографии в пространство Хафа.
      computeHoughCircle(buf_matrix_borders, len_matrix_borders, buf_hough, len_hough, 32, img_width, img_height);
      // Преобразуем пространство Хафа в JPEG изображение.
      bool ok_hough = fmt2jpg(buf_hough, len_hough, img_width, img_height, PIXFORMAT_GRAYSCALE, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будет принято пространство Хафа.
      sendJson(jsonString, doc_tx, "change_img_type", 4);
      // Отправить пространство Хафа на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);
      if (buf_matrix_borders) free(buf_matrix_borders);


      
      // Массив для хранения 
      uint8_t arr_Y[50];
      // Массив для хранения 
      uint8_t arr_X[50];
      // Присвоить всем элементам массивов значение 0.
      for(int i = 0; i < 50; i+=1){
        arr_Y[i] = 0;
        arr_X[i] = 0;
      }
      // Переменная для подсчёта найденых линий.
      int circlesFound;

      // Получить параметры для построения окружностей лежащих по контурам на исходной фотографии.
      getDotsFromHoughSpace(buf_hough, len_hough, img_width, img_height, arr_X, arr_Y, circlesFound);
      // Преобразуем пространство Хафа в JPEG изображение.
      bool ok_hough_dot = fmt2jpg(buf_hough, len_hough, img_width, img_height, PIXFORMAT_GRAYSCALE, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будет принято пространство Хафа.
      sendJson(jsonString, doc_tx, "change_img_type", 5);
      // Отправить пространство Хафа на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);
      if (buf_hough) free(buf_hough);


      Serial.println("Circles found: " + String(circlesFound));





      // Размер буффера для хранения исходного изображения преобразованого в цветовую матрицу RGB.
      size_t  len_rgb = frame->width * frame->height * 3;
      // Инициализировать буфер под хранение исходного изображения преобразованого в цветовую матрицу RGB.
      uint8_t *buf_rgb = (uint8_t *)ps_malloc(len_rgb);
      

      // Конвертируем исходное изображение в цветовую матрицу RGB.
      bool okey = fmt2rgb888(frame->buf, frame->len, PIXFORMAT_JPEG, buf_rgb);

      // Построить все найденые окружности.
      for(int dot = 0; dot < circlesFound; dot += 1){
        drawRGBCircle(buf_rgb, arr_Y[dot], arr_X[dot], 32, frame->width, frame->height);
      }

      // Освобождаем буфер хранящий JPEG чтобы перезаписать в него обработаное изображение.
      if (frame->buf) free(frame->buf);

      bool ok_RGB = fmt2jpg(buf_rgb, len_rgb, frame->width, frame->height, PIXFORMAT_RGB888, 80, &frame->buf, &frame->len);
      if (buf_rgb) free(buf_rgb);
      // Обозначить на вебстраничке что следующим будет принято чёрно-белое изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 6);
      // Отправить чёрно-белое изображение на вебстраничку.
      webSocket.broadcastBIN(frame->buf, frame->len);
      // Освобождаем буфер.
      //if (buf_img) free(buf_img);

      // Освободить зарезервированную память под исходное изображение.
      esp_camera_fb_return(frame);
    }

    capture = false;
    }
    delay(30);
  }

