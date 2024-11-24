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
// В документе реализлваны функции для работы с памятью. 
#include "memory.h"
#include "soc/soc.h" // Используется при нестабильном питании и перезапуске
#include "soc/rtc_cntl_reg.h" //используется при нестабильном питании и перезапуске
#include <Arduino.h>
// Библиотека предоставляет функции для преобразования изображений между различными форматами.
#include "img_converters.h"


// TensorFlowLite_ESP32-----------------------------------------------------


// Библиотека позволяет развернуть модели машиного обучения на семействе микроконтроллеров ESP32.
//#include <TensorFlowLite_ESP32.h>
#include <MicroTFLite.h>

// Реализация ряда функций обработки данных моделью необходимых интерпретатору для запуска модели.
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
// Ряд функций предоставляющих отчёт об ошибках и отладочную информацию.
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
//#include "tensorflow/lite/micro/micro_error_reporter.h"
// Интерпретатор содержит код для загрузки и запуска модели без предвапительной компиляции.
// (выполняет программу построчно, читая каждую инструкцию и преобразуя её в исполняемый код)
#include "tensorflow/lite/micro/micro_interpreter.h"
// Cодержит схему преобразования модели в поток байтов для передачи в память на базе библиотеки FlatBuffers.
// (FlatBuffers — эффективная кроссплатформенная библиотека сериализации для C++, C#, C)
#include "tensorflow/lite/schema/schema_generated.h"
// Этот файл определяет общие типы данных и API для реализации операций, делегатов и других конструкций в TensorFlow Lite.
#include "tensorflow/lite/c/common.h"

// Модель машиного обучения.
#include "TensorFlowLiteModel.h"
// Параметры изображения передаваемого модели, а так же кол-во категорий для классификации.
#include "TensorFlowLiteModelConfig.h"
// Документ хранящий код HTML странички.
#include "HomePage.h"
// -------------------------------------------------------------------------

// Инициализировать необходимые структуры данных для работы с библиотекой Tensor Flow Lite.
// Обьект ErrorReporter предоставляет отчёт об ошибках и отладочную информацию.
tflite::ErrorReporter* error_reporter = nullptr;
// Обьект для хранения модели машиного обучения осуществляющей классификацию изображения.
const tflite::Model* model = nullptr;
// Обьект для хранения интерпретатора осуществляющего загрузку и запуск модели.
tflite::MicroInterpreter* interpreter = nullptr;
// Обьект (тензор) для хранения изображения передаваемого на вход моделя для последующей классификации.
TfLiteTensor* input = nullptr;

// Обьём памяти, который необходимо выделить для хранения массивов модели.
// Для входного, выходного и промежуточных массивов модели.
//constexpr int kTensorArenaSize = 81 * 1024;
constexpr int kTensorArenaSize = 162 * 1024;
// Массив для хранения входных, выходных и промежуточных массивов модели.
static uint8_t *tensor_arena;//[kTensorArenaSize]; // Maybe we should move this to external

// Данные WiFi сети.
const char* ssid = "WiFi_Name_Vremenoi";
const char* password = "WiFi_Par0l_Vosp";

// Экземпляр вебсервера ссылается на 80 порт.
AsyncWebServer server(80);
// Экземпляр WebSocket-сервера ссылается на 81 порт.
WebSocketsServer webSocket = WebSocketsServer(81);

// Переменная отображает состояние кнопки отвечающей за классификацию фото.
bool classify = false;
// Переменная отображает состояние кнопки отвечающей за добавления фото на карту памяти.
bool dataset = false;

/** В документе реализованы функции позволяющие предобработать изображение
перед подачей на вход модели машиного обучения для дальнейшие классификации. **/
#include "imagePreprocessing.h"
/** В документе реализлвана функция webSocketEvent для обработки данных 
полученых через соеденение сокетов. **/
#include "socketConnection.h"

void setup() {

  // TensorFlowLite_ESP32-----------------------------------------------------

  // Для ведения журнала ошибок создадим переменную "error_reporter" на базе предоставляемых библиотекой Tensor Flow Lite структур данных.
  //static tflite::MicroErrorReporter micro_error_reporter;
  static tflite::MicroErrorReporter micro_error_reporter;
  // Переменную "error_reporter" необходимо передать в интерпретатор, который будет в свою очередь передавать в неё список ошибок.
  error_reporter = &micro_error_reporter;


  // Создадим экземпляр модели используя массив данных из документа "TensorFlowLiteModel.h" 
  model = tflite::GetModel(model_TFLite);
  // Проверка соответствия версии модели и версии библиотеки.
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }
  // Выделить обьём памяти для входного, выходного и промежуточных массивов модели,
  if (tensor_arena == NULL) {
    // Выделить более медляную память, но большую по обьёму.
    tensor_arena = (uint8_t*) ps_calloc(kTensorArenaSize, 1);
    // Выделить более быструю память, но меньшую по обьёму.
    //tensor_arena = (uint8_t *) heap_caps_malloc(kTensorArenaSize, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  }
  // Если не удалось выделить обьём памяти для входного, выходного и промежуточных массивов модели, то вывести сообщение об ошибке.
  if (tensor_arena == NULL) {
    printf("Couldn't allocate memory of %d bytes\n", kTensorArenaSize);
    return;
  }


  // Загрузить все методы, что содержит библиотека Tensor Flow Lite, для обработки данных моделью. (Занимает большой обьём памяти)
  // tflite::AllOpsResolver resolver;

  // Загрузить необходимые методы для обработки данных моделью из библиотеки Tensor Flow Lite.
  static tflite::MicroMutableOpResolver<9> micro_op_resolver;
  // AveragePool2D — операция, применяемая в свёрточных нейронных сетях (CNN), для уменьшения ширины и высоты входного тензора.
  micro_op_resolver.AddAveragePool2D();
  // MaxPool2D — операция в свёрточных нейронных сетях (CNN), которая выполняет подвыборку данных, уменьшая ширину и высоту входного тензора.
  micro_op_resolver.AddMaxPool2D();
  // Reshape — операция, используемая в машинном обучении и обработке данных, которая изменяет форму (размерность) тензора без изменения его данных
  micro_op_resolver.AddReshape();
  // FullyConnected (полносвязанный слой) — используется для выполнения нелинейных преобразований данных и играет важную роль в моделях глубокого обучения.
  micro_op_resolver.AddFullyConnected();
  // Conv2D (свёрточный слой) — выполняет операцию свёртки над входными данными, чтобы извлекать локальные признаки, использует их для построения более сложных представлений на следующих слоях.
  micro_op_resolver.AddConv2D();
  // DepthwiseConv2D — разновидность свёрточного слоя, которая применяется для увеличения вычислительной эффективности и уменьшения количества параметров модели.
  micro_op_resolver.AddDepthwiseConv2D();
  // Softmax — функция активации, которая используется в выходных слоях нейронных сетей для задач классификации.
  micro_op_resolver.AddSoftmax();
  // Quantize (квантование) — процесс преобразования данных или моделей глубокого обучения, чтобы снизить их размер и вычислительную сложность, сохраняя при этом приемлемую точность.
  micro_op_resolver.AddQuantize();
  // Dequantize (деквантование) — процесс обратного преобразования данных из квантованного формата обратно в формат с плавающей точкой или в более высокую точность. 
  micro_op_resolver.AddDequantize();


  // Создадим экземпляр интерпретатора передавав необходимые данные для запуска модели.
  static tflite::MicroInterpreter static_interpreter(
    model, micro_op_resolver, tensor_arena, kTensorArenaSize);
      //model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);

  interpreter = &static_interpreter;


  // Выделим память для внутрених тензоров модели из выделеной ранее памяти tensor_arena.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  // При неудачном выделении памяти сообщить об ошибке.
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Получить указатель на входной тензор модели.
  input = interpreter->input(0);
  // -------------------------------------------------------------------------

  // Последовательный порт (serial port).
  Serial.begin(115200);

  // Инициализация модуля камеры OV2640 (CAMERA_MODEL_AI_THINKER).
  cam_init(FRAMESIZE_240X240, PIXFORMAT_JPEG, 12);

  // Инициализация MicroSD Card.
  SDCard_init();

  // Инициализация EEPROM.
  EEPROM.begin(EEPROM_SIZE);

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




// Блок кода делает фото, обрабатывает его и записывает на карту памяти.
if(dataset){
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

      // Инициализировать переменную для хранение размера изображения в формате RGB888.
      size_t len_rgb = img_width * img_height * 3;
      // Инициализировать буфер под хранение изображения в формате RGB888.
      uint8_t *buf_rgb = (uint8_t *)ps_malloc(len_rgb);
      // Преобразуем исходное изображение из JPEG в формат RGB888.
      bool okey = fmt2rgb888(frame->buf, frame->len, PIXFORMAT_JPEG, buf_rgb);
      // Освободить зарезервированную память под исходное изображение.
      esp_camera_fb_return(frame);
      

      // Переменная для хранения размера буфера содержащего изображение передаваемое на вход модели.
      size_t len_input;
      // Буфер под хранение изображения передаваемого на вход модели.
      uint8_t *buf_input;
      // Ширина изображения передаваемого на вход модели.
      int input_width = 32;
      // Высота изображения передаваемого на вход модели.
      int input_height = 32;

      // Уменьшить изображение чтобы передать его моделе.
      downsampleRGBMatrix(buf_rgb, len_rgb, buf_input, len_input, img_width, img_height, input_width, input_height);
      // Освобождаем буфер.
      if (buf_rgb) free(buf_rgb);
      // Преобразуем уменьшеное изображение в JPEG.
      bool ok_downsample = fmt2jpg(buf_input, len_input, input_width, input_height, PIXFORMAT_RGB888, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будет принято уменьшеное изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 1);
      // Отправить уменьшеное изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Функция для записи изображения на MicroSD карту.
      saveToMicroSD(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);
      // Освобождаем буфер.
      if (buf_input) free(buf_input);
    }
    // Изображение было записано на карту памяти.
    dataset = false;
    }






  // Блок кода делает фото, обрабатывает его, передаёт фото модели и делает вывод что изображено на фото.
  if(classify){
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

      // Инициализировать переменную для хранение размера изображения в формате RGB888.
      size_t len_rgb = img_width * img_height * 3;
      // Инициализировать буфер под хранение изображения в формате RGB888.
      uint8_t *buf_rgb = (uint8_t *)ps_malloc(len_rgb);
      // Преобразуем исходное изображение из JPEG в формат RGB888.
      bool okey = fmt2rgb888(frame->buf, frame->len, PIXFORMAT_JPEG, buf_rgb);
      // Освободить зарезервированную память под исходное изображение.
      esp_camera_fb_return(frame);
      

      // Переменная для хранения размера буфера содержащего изображение передаваемое на вход модели.
      size_t len_input;
      // Буфер под хранение изображения передаваемого на вход модели.
      uint8_t *buf_input;
      // Ширина изображения передаваемого на вход модели.
      int input_width = 32;
      // Высота изображения передаваемого на вход модели.
      int input_height = 32;

      // Уменьшить изображение чтобы передать его моделе.
      downsampleRGBMatrix(buf_rgb, len_rgb, buf_input, len_input, img_width, img_height, input_width, input_height);
      // Освобождаем буфер.
      if (buf_rgb) free(buf_rgb);
      // Преобразуем уменьшеное изображение в JPEG.
      bool ok_downsample = fmt2jpg(buf_input, len_input, input_width, input_height, PIXFORMAT_RGB888, 80, &buf_img, &len_img);
      // Обозначить на вебстраничке что следующим будет принято уменьшеное изображение.
      sendJson(jsonString, doc_tx, "change_img_type", 1);
      // Отправить уменьшеное изображение на вебстраничку.
      webSocket.broadcastBIN(buf_img, len_img);
      // Освобождаем буфер.
      if (buf_img) free(buf_img);

      // Входной тензор модели (вход модели).
      int8_t * image_data = input->data.int8;
      // Передаём на вход модели уменьшеное цветное изображение.
      for (int i = 0; i < kNumRows; i++) {
        for (int j = 0; j < kNumCols; j++) {
          // Blue - Голубая компонента текущего пикселя.
          image_data[(i * kNumCols + j) * 3 + 2] = buf_input[(i * kNumCols + j) * 3 + 0] - 128;
          // Gree - Зелёная компонента текущего пикселя.
          image_data[(i * kNumCols + j) * 3 + 1] = buf_input[(i * kNumCols + j) * 3 + 1] - 128;
          // Red - Красная компонента текущего пикселя.
          image_data[(i * kNumCols + j) * 3 + 0] = buf_input[(i * kNumCols + j) * 3 + 2] - 128;
        }
      }

      // Вызвать модель (произвести преобразование входного изображения в вероятность принадлежности 
      // данного изображения к каждому из возможных классов).
      if (kTfLiteOk != interpreter->Invoke()) {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed.");
      }

      // Получить выход модели.
      TfLiteTensor* output = interpreter->output(0);

      // Пройти по каждому элементу выхода модели.
      for(int i = 0; i < kCategoryCount; i++){

        // Получить вероятность для i-го класса.
        int8_t cur_confidence = output->data.uint8[i];
        // Отправить вероятность принадлежности данного изображения к i-ому классу на вебстраничку.
        sendJson(jsonString, doc_tx, "obj_" + String(i + 1), cur_confidence);
      }

      // Освобождаем буфер.
      if (buf_input) free(buf_input);
    }
    // Изображение было классифициравано.
    classify = false;
    }
    delay(30);
  }

