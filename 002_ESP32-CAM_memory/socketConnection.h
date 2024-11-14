// Выделить память для работы с Json(200 байт).
StaticJsonDocument<200> doc_tx; // для передачи данных.
StaticJsonDocument<200> doc_rx; // для приёма данных.
String jsonString = "";

// Функция отправляет данные на вебстраничку к клиенту.
void sendJson(String jsonString ,StaticJsonDocument<200> doc, String l_type, int l_value) {
    // Создать JSON обьект.
    JsonObject object = doc.to<JsonObject>();
    // Записать данные в JSON обьект.
    object["type"] = l_type;
    object["value"] = l_value;
    // Конвертировать JSON обьект в строку.
    serializeJson(doc, jsonString);
    // Отправить данные на вебстраничку к клиенту.
    webSocket.broadcastTXT(jsonString);
    // Очистить JSON документ.
    doc.clear();
}

/** Функция webSocketEvent обработывает данные полученные от клиента через соеденение вебсокетов.
    - byte num (номер клиента)
    - WStype_t type (тип данных принятых от клиента)
    - uint8_t * payload (данные принятые от клиента)
    - size_t length (длинна принятых данных)**/
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {
  // В зависимости от типа принятых данных выполнить соответствующий блок кода.
  switch (type) {
    // Обработка отключения клиента:
    case WStype_DISCONNECTED: // Если клиент отключился, выполнить следующий блок кода.
      Serial.println("Client " + String(num) + " disconnected");
      break;

    // Обработка подключения клиента:
    case WStype_CONNECTED:    // Если клиент подключился, выполнить следующий блок кода.
      Serial.println("Client " + String(num) + " connected");
      // Обозначить на вебстраничке что следующее фото будет принято из памяти SPIFFS.
      sendJson(jsonString, doc_tx, "change_img_type", 0);
      // Вызываем функцию для записи изображения из памяти SPIFFS в буфер.
      readFromSpiffs(String(path_SPIFFS), jpg_buf, jpg_buf_len );
      // Отправляем изображение из памяти SPIFFS пользователю.
      webSocket.broadcastBIN(jpg_buf, jpg_buf_len);
      // Освободить зарезервированную память под изображение.
      if(jpg_buf) free(jpg_buf);

      // Подготовить список изображений записаных на MicroSD карте памяти.
      ListFromMicroSD(&doc_tx);
      // Сериализуем и выводим содержимое JSON-документа
      serializeJson(doc_tx, jsonString);
      // Передать список изображений пользователю в select.
      webSocket.broadcastTXT(jsonString);
      // Очищаем JSON документ.
      doc_tx.clear();
      // Если фото с карты памяти было выбрано, то отправлять выбраное фото всем новым подключившемся клиентам.
      if(microSD_bool){
          // Обозначить на вебстраничке что следующее фото будет принято с карты памяти.
          sendJson(jsonString, doc_tx, "change_img_type", 1);
          // Вызываем функцию для записи изображения с карты памяти в буфер.
          readFromMicroSD(microSD_path, jpg_buf, jpg_buf_len);
          // Отправляем изображение из памяти SPIFFS пользователю.
          webSocket.broadcastBIN(jpg_buf, jpg_buf_len);
          // Освободить зарезервированную память под изображение.
          if(jpg_buf) free(jpg_buf);
      }
      break;

    // Обработка текстовых данных, отправленных клиентом:
    case WStype_TEXT:   // Если клиент отправил текстовые данные, обработать их.
      // Записать данные переданные от клиента "payload" в памяти контролера "doc_rx".
      DeserializationError error = deserializeJson(doc_rx, payload);
      // Если произошла ошибка при записи, вывести сообщение об ошибке.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      // При успешной записи JSON строки в память контролера обработать её.
      else {
        // Выведим пользователя от которого были приняты данные.
        Serial.println("Received from user: " + String(num));
        // Определим тип JSON строки обратившись к ней по ключу ["type"].
        const char* msg_type = doc_rx["type"];
        Serial.println("Type: " + String(msg_type));

        // Исходя из типа принятой JSON строки выполним соответствующий блок кода.
        if(String(msg_type) == "capture") {
          // Присвоим bool capture значение true чтобы в void loop() {} 
          // выполнить блок кода отвечающий за захват изображения.
          capture = doc_rx["value"];
        } else if(String(msg_type) == "img_SDcart_send"){
          // Клиент выбрал фото с карты памяти, которое хочет увидеть.
          microSD_bool = true;
          // Обозначить на вебстраничке что следующее фото будет принято с карты памяти.
          sendJson(jsonString, doc_tx, "change_img_type", 1);
          // Путь к файлу, который выбрал клиент.
          microSD_path = "/" + String(doc_rx["value"]);
          Serial.println("Path: " + microSD_path);
          // Вызываем функцию для записи изображения с карты памяти в буфер.
          readFromMicroSD(microSD_path, jpg_buf, jpg_buf_len);
          // Отправляем изображение с карты памяти пользователю.
          webSocket.broadcastBIN(jpg_buf, jpg_buf_len);
          // Освободить зарезервированную память под изображение.
          if(jpg_buf) free(jpg_buf);
        }
      }
      Serial.println("");
      break;
  }
}
