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
const char* password = "WiFi_Par0l_Beremen0i";

// Экземпляр вебсервера ссылается на 80 порт.
AsyncWebServer server(80);
// Экземпляр WebSocket-сервера ссылается на 81 порт.
WebSocketsServer webSocket = WebSocketsServer(81);

// Переменные отображают состояния кнопок отвечающих за захват фото.
bool capture_main = false;
bool capture_current = false;

/** В документе реализлвана функция webSocketEvent для обработки данных 
полученых через соеденение сокетов. **/
#include "socketConnection.h"



// Размера буфера содержащего основное изображение.
size_t len_main_rgb;
// Буфер содержащий основное изображение.
uint8_t *buf_main_rgb;



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

  // Если на вебстраничке была нажата кнопка для захвата основного изображения.
  if(capture_main){
    Serial.println("Take a main photo");

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


      // Обозначить на вебстраничке что следующим будет принято основное изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 0);
      // Отправить основное изображение на вебстраничку.
      webSocket.broadcastBIN(frame->buf, frame->len);


      // Освобождаем буфер.
      if (buf_main_rgb) free(buf_main_rgb);

      // Рассчитывем размер буфера для основного изображения в формате RGB888.
      len_main_rgb = img_width * img_height * 3;
      // Инициализировать буфер под хранение основного изображения в формате RGB888.
      buf_main_rgb = (uint8_t *)ps_malloc(len_main_rgb);
      // Конвертируем изображение захваченое с матрицы камеры из JPEG в формат RGB888.
      bool okey = fmt2rgb888(frame->buf, frame->len, frame->format, buf_main_rgb);
      // Освободить зарезервированную память под изображение с матрицы камеры.
      esp_camera_fb_return(frame);


    }
    // Фото было сделано, обработано и отправлено клиенту.
    capture_main = false;
    }



  // Если на вебстраничке была нажата кнопка для захвата текущего изображения.
  if(capture_current){
    Serial.println("Take a current photo");

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


      // Обозначить на вебстраничке что следующим будет принято текущее изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 1);
      // Отправить текущее изображение на вебстраничку.
      webSocket.broadcastBIN(frame->buf, frame->len);


      // Рассчитывем размер буфера для текущего изображения в формате RGB888.
      size_t len_current_rgb = img_width * img_height * 3;
      // Инициализировать буфер под хранение текущего изображения в формате RGB888.
      uint8_t *buf_current_rgb = (uint8_t *)ps_malloc(len_current_rgb);
      // Конвертируем изображение захваченое с матрицы камеры из JPEG в формат RGB888.
      bool okey = fmt2rgb888(frame->buf, frame->len, frame->format, buf_current_rgb);
      // Освободить зарезервированную память под изображение с матрицы камеры.
      esp_camera_fb_return(frame);


      // Сумма разниц между голубыми компонентами всех пикселей основного и текущего изображений.
      double blueDiffValue = 0;
      // Сумма разниц между зелёными компонентами всех пикселей основного и текущего изображений.
      double greenDiffValue = 0;
      // Сумма разниц между красными компонентами всех пикселей основного и текущего изображений.
      double redDiffValue = 0;
      // Сумма разниц между всеми компонентами всех пикселей основного и текущего изображений.
      double averageValue = 0;

      // Пройти по всем пикселям основного и текущего изображений.
      for(int i = 0; i < len_current_rgb - 1; i += 3){
        
        // Рассчитать разницу между голубыми компонентами пикселей основного и текущего изображений и добавить к основной сумме.
        blueDiffValue += (buf_main_rgb[i] - buf_current_rgb[i]) / 255.0;
        // Рассчитать разницу между зелёными компонентами пикселей основного и текущего изображений и добавить к основной сумме.
        greenDiffValue += (buf_main_rgb[i + 1] - buf_current_rgb[i + 1]) / 255.0;
        // Рассчитать разницу между красными компонентами пикселей основного и текущего изображений и добавить к основной сумме.
        redDiffValue += (buf_main_rgb[i + 2] - buf_current_rgb[i + 2]) / 255.0;
      }

      /**
      Вывести в последовательный порт и на вебстраничку рассчитаные параметры.
      **/
      Serial.println("Red difference: " + String(redDiffValue));
      sendJson(jsonString, doc_tx, "red", redDiffValue);

      Serial.println("Green difference: " + String(greenDiffValue));
      sendJson(jsonString, doc_tx, "green", greenDiffValue);

      Serial.println("Blue difference: " + String(blueDiffValue));
      sendJson(jsonString, doc_tx, "blue", blueDiffValue);
      
      // Рассчитать cумма разниц между всеми компонентами всех пикселей основного и текущего изображений.
      averageValue = redDiffValue + greenDiffValue + blueDiffValue / 3;

      Serial.println("AVERAGE: " + String(averageValue));
      sendJson(jsonString, doc_tx, "average", averageValue);

      // Освобождаем буфер.
      if (buf_current_rgb) free(buf_current_rgb);

    }
    // Фото было сделано, обработано и отправлено клиенту.
    capture_current = false;
    }



    delay(30);
  }

