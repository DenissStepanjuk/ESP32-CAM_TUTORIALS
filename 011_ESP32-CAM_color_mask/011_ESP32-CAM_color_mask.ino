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
const char* password = "WiFi_Par0l_Vospitavshii";

// Экземпляр вебсервера ссылается на 80 порт.
AsyncWebServer server(80);
// Экземпляр WebSocket-сервера ссылается на 81 порт.
WebSocketsServer webSocket = WebSocketsServer(81);

// Переменная отображает состояние кнопки отвечающей за фото.
bool capture = false;

int hue_min = 0;
int hue_max = 255;
int sat_min = 0;
int sat_max = 255;
int val_min = 0;
int val_max = 255;

/** В документе реализованы функции позволяющие преобразвать изображение в формат HSV, 
создать маску для заданого цвета, тем самым выделить его. **/
#include "colorMask.h"
/** В документе реализлвана функция webSocketEvent для обработки данных 
полученых через соеденение сокетов. **/
#include "socketConnection.h"

void setup() {
  // Последовательный порт (serial port).
  Serial.begin(115200);

  // Инициализация модуля камеры OV2640 (CAMERA_MODEL_AI_THINKER).
  cam_init(FRAMESIZE_VGA, PIXFORMAT_JPEG, 10);
  //cam_init(FRAMESIZE_QVGA, PIXFORMAT_JPEG, 10);

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
      // Отправить исходное изображение на вебстраничку.
      webSocket.broadcastBIN(frame->buf, frame->len);

      // Применить фильтр по цвету.
      applyMask(frame->buf, frame->len, frame->width, frame->height, hue_min, hue_max, sat_min, sat_max, val_min, val_max);

      // Обозначить на вебстраничке что следующим будет принято отфильтрованое по цвету изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 1);
      // Отправить отфильтрованое по цвету изображение на вебстраничку.
      webSocket.broadcastBIN(frame->buf, frame->len);

    }
    // Освободить зарезервированную память под изображение.
    esp_camera_fb_return(frame);
    // Фото было сделано, обработано и отправлено клиенту.
    capture = false;
    }
    delay(30);
  }

