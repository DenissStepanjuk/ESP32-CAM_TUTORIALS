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
#include "houghTransformLine.h"
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
      // Освобождаем буфер.
      if (buf_matrix_gray) free(buf_matrix_gray);
      // Преобразуем 2D матрицу контуров изображение в JPEG.
      bool ok_borders = fmt2jpg(buf_matrix_borders, len_matrix_borders, img_width, img_height, PIXFORMAT_GRAYSCALE, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующими будут приняты контуры изображения.
      sendJson(jsonString, doc_tx, "change_img_type", 2);
      // Отправить чёрно-белое изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);







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
      // Шириной пространства Хафа будет диагональ изображения содержащего контуры исходной фотографии помноженая на 2.
      int diagonal;
      // Высотой пространства Хафа будет диапозон угла teta от -90 до 90 градусов.
      int teta;


      // Преобразовать контуры исходной фотографии в пространство Хафа.
      computeHoughLine(buf_matrix_borders, len_matrix_borders, buf_hough, len_hough, img_width, img_height, diagonal, teta);
      // Освобождаем буфер.
      if (buf_matrix_borders) free(buf_matrix_borders);
      // Преобразуем пространство Хафа в JPEG изображение.
      bool ok_hough = fmt2jpg(buf_hough, len_hough, diagonal, teta, PIXFORMAT_GRAYSCALE, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будет принято пространство Хафа.
      sendJson(jsonString, doc_tx, "change_img_type", 4);
      // Отправить пространство Хафа на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);


      // Массив для хранения параметров R (расстояние от начала координат к прямой под углом 90 градусов)
      // для построяния прямых линий лежащих по контурам на исходной фотографии.
      uint8_t arr_R[50];
      // Массив для хранения углов theta (угол наклона прямой "R" относительно оси x)
      // для построяния прямых линий лежащих по контурам на исходной фотографии.
      uint8_t arr_theta[50];
      // Присвоить всем элементам массивов значение 0.
      for(int i = 0; i < 50; i+=1){
        arr_R[i] = 0;
        arr_theta[i] = 0;
      }
      // Переменная для подсчёта найденых линий.
      int linesFound;
      

      // Получить параметры для построения прямых линий лежащих по контурам на исходной фотографии.
      getDotsFromHoughSpace(buf_hough, len_hough, diagonal, teta, arr_R, arr_theta, linesFound);
      // Преобразуем пространство Хафа в JPEG изображение.
      bool ok_hough_dots = fmt2jpg(buf_hough, len_hough, diagonal, teta, PIXFORMAT_GRAYSCALE, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будет принято пространство Хафа.
      sendJson(jsonString, doc_tx, "change_img_type", 5);
      // Отправить пространство Хафа на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);






      if (buf_hough) free(buf_hough);




      // Инициализировать переменную для хранение размера буфера содержащего все линии полученые из пространства Хафа.
      size_t len_all_lines;
      // Инициализировать буфер содержащий все линии полученые из пространства Хафа.
      uint8_t *buf_all_lines;
      // Начертить все линии полученые из пространства Хафа.
      drawAllLines(buf_all_lines, len_all_lines, img_width, img_height, arr_R, arr_theta, linesFound);
      // Преобразовать буфер изображения содержащий все линии полученые из пространства Хафа в JPEG.
      bool ok_all_line = fmt2jpg(buf_all_lines, len_all_lines, img_width, img_height, PIXFORMAT_GRAYSCALE, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будет принято изображение содержащие все линии полученые из пространства Хафа.
      sendJson(jsonString, doc_tx, "change_img_type", 6);
      // Отправить изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);

      Serial.println("All lines: " + String(linesFound));
      if (buf_all_lines) free(buf_all_lines);


      // Инициализировать переменную для хранение размера буфера содержащего отфильтрованные линии полученые из пространства Хафа.
      size_t len_filtred_lines;
      // Инициализировать буфер содержащий отфильтрованные линии полученые из пространства Хафа.
      uint8_t *buf_filtred_lines;
      // Массив для хранения отфильтрованых линий.
      int arr_lines[6][238];
      // Переменная для подсчёта линий.
      int linesSorted = 0;

      // Начертить отфильтрованные линии полученые из пространства Хафа.
      drawFiltredLines(buf_filtred_lines, len_filtred_lines, img_width, img_height, arr_R, arr_theta, arr_lines, linesFound, linesSorted);
      // Преобразовать буфер изображения содержащий отфильтрованные линии полученые из пространства Хафа в JPEG.
      bool ok_filtred_line = fmt2jpg(buf_filtred_lines, len_filtred_lines, img_width, img_height, PIXFORMAT_GRAYSCALE, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будет принято изображение отфильтрованных линий полученых из пространства Хафа.
      sendJson(jsonString, doc_tx, "change_img_type", 7);
      // Отправить изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);

      Serial.println("Filtred lines: " + String(linesSorted));
      if (buf_filtred_lines) free(buf_all_lines);







      // Преобразуем исходное изображение в 2D матрицу содержащую чёрно-белое изображение.
      drawRGBLines(frame->buf, frame->len, frame->width, frame->height, arr_lines, linesSorted);

      // Обозначить на вебстраничке что следующим будет принято исходное изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 8);
      // Отправить исходное изображение на вебстраничку.
      webSocket.broadcastBIN(frame->buf, frame->len);
      // Освободить зарезервированную память под исходное изображение.
      esp_camera_fb_return(frame);

    }

    capture = false;
    }
    delay(30);
  }

