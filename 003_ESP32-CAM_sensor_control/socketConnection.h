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
    case WStype_CONNECTED:    // Если клиент потключился, выполнить следующий блок кода.
      Serial.println("Client " + String(num) + " connected");
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
        const char* param_type = doc_rx["type"];
        // Получим значение параметра из JSON строки.
        const int param_value = doc_rx["value"];
        Serial.println("Type: " + String(param_type) + " Value: " + String(param_value));

        // Исходя из типа принятой JSON строки выполним соответствующий блок кода.
        if(String(param_type) == "capture") {
          // Присвоим bool capture значение true чтобы в void loop() {} 
          // выполнить блок кода отвечающий за захват изображения.
          capture = doc_rx["value"];
        } 
        // Выбор фильтров: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // WB MODES (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
        else if(String(param_type) == "wb_mode") {
          s->set_wb_mode(s, param_value);
          sendJson(jsonString, doc_tx, String(param_type), param_value);
        } 
        // SPECIAL EFFECTS (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
        else if(String(param_type) == "special_effects") {
          s->set_special_effect(s, param_value);
          sendJson(jsonString, doc_tx, String(param_type), param_value);
        } 
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Задать значение параметра через ползунок. ++++++++++++++++++++++++++++++++++++++++++++++++++++
        // BRIGHTNESS (-2 to 2)
        else if(String(param_type) == "brightness") {
          s->set_brightness(s, param_value);
          sendJson(jsonString, doc_tx, String(param_type), param_value);
        } 
        // CONTRAST (-2 to 2)
        else if(String(param_type) == "contrast") {
          s->set_contrast(s, param_value);
          sendJson(jsonString, doc_tx, String(param_type), param_value);
        } 
        // SATURATION (-2 to 2)
        else if(String(param_type) == "saturation") {
          s->set_saturation(s, param_value);
          sendJson(jsonString, doc_tx, String(param_type), param_value);
        } 
        // AE LEVELS (-2 to 2)
        else if(String(param_type) == "ae_level") {
          s->set_ae_level(s, param_value);
          sendJson(jsonString, doc_tx, String(param_type), param_value);
        } 
        // GAIN CEILING (0 to 6)
        else if(String(param_type) == "gainceiling") {
          s->set_gainceiling(s, (gainceiling_t)param_value);
          sendJson(jsonString, doc_tx, String(param_type), param_value);
        } 
        // AGC GAIN (0 to 30)
        else if(String(param_type) == "agc_gain") {
          s->set_agc_gain(s, param_value);
          sendJson(jsonString, doc_tx, String(param_type), param_value);
        } 
        // AEC VALUES (0 to 1200)
        else if(String(param_type) == "aec_value") {
          s->set_aec_value(s, param_value);
          sendJson(jsonString, doc_tx, String(param_type), param_value);
        } 
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Кнопки подключения/отключения параметров. ''''''''''''''''''''''''''''''''''''''''''''''''''''
        // WHITE BALANCE (0 = Disable , 1 = Enable)
        else if(String(param_type) == "whitebal") {
          if(param_value == 0){
            s->set_whitebal(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_whitebal(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // AWB GAIN (0 = Disable , 1 = Enable)
        else if(String(param_type) == "awb_gain") {
          if(param_value == 0){
            s->set_awb_gain(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_awb_gain(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // EXPOSURE CONTROLS (0 = Disable , 1 = Enable)
        else if(String(param_type) == "exposure_ctrl") {
          if(param_value == 0){
            s->set_exposure_ctrl(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_exposure_ctrl(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // GAIN CONTROLS (0 = Disable , 1 = Enable)
        else if(String(param_type) == "gain_ctrl") {
          if(param_value == 0){
            s->set_gain_ctrl(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_gain_ctrl(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // WPC (0 = Disable , 1 = Enable)
        else if(String(param_type) == "wpc") {
          if(param_value == 0){
            s->set_wpc(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_wpc(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // RAW GMA (0 = Disable , 1 = Enable)
        else if(String(param_type) == "raw_gma") {
          if(param_value == 0){
            s->set_raw_gma(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_raw_gma(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // LENC (0 = Disable , 1 = Enable)
        else if(String(param_type) == "ae_lenc") {
          if(param_value == 0){
            s->set_lenc(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_lenc(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // DCW (0 = Disable , 1 = Enable)
        else if(String(param_type) == "dcw") {
          if(param_value == 0){
            s->set_dcw(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_dcw(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // COLOR BAR PATTERN (0 = Disable , 1 = Enable)
        else if(String(param_type) == "colorbar") {
          if(param_value == 0){
            s->set_colorbar(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_colorbar(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // HORIZ MIRROR (0 = Disable , 1 = Enable)
        else if(String(param_type) == "hmirror") {
          if(param_value == 0){
            s->set_hmirror(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_hmirror(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // VERT FLIP (0 = Disable , 1 = Enable)
        else if(String(param_type) == "vflip") {
          if(param_value == 0){
            s->set_vflip(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_vflip(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // BPC (0 = Disable , 1 = Enable)
        else if(String(param_type) == "bpc") {
          if(param_value == 0){
            s->set_bpc(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_bpc(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // AEC2 (0 = Disable , 1 = Enable)
        else if(String(param_type) == "aec2") {
          if(param_value == 0){
            s->set_aec2(s, 1);
            sendJson(jsonString, doc_tx, String(param_type), 1);
          } else {
            s->set_aec2(s, 0);
            sendJson(jsonString, doc_tx, String(param_type), 0);
          }
        } 
        // ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
      Serial.println("");
      break;
  }
}
}