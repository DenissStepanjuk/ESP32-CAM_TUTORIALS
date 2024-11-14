// Функция подготавливает и возвращает HTML страничку.

String getHTML() {
  String html = ""
"  <!DOCTYPE html>"
"  <html>"
    /** head **//////////////////////////////////////////////////////////////////////////////////////////////////////
"    <head>"
"      <title>ESP32-CAM</title>"
"    </head>"
      
    /** body **//////////////////////////////////////////////////////////////////////////////////////////////
"    <body style='background-color: #EEEEEE;'>"
      /** Изображение. **/
"      <img id='img'>"
      /** Кнопки:
        - сделать фото
        - перезагрузить страничку **/
"      <p><button type='button' id='BTN_CAPTURE'> CAPTURE </button>"
"      <button onclick='location.reload();'>REFRESH PAGE</button></p>"
"    </body>"
      
    /** script **//////////////////////////////////////////////////////////////////////////////////////////////
"    <script>"
      // Создание экземпляр вебсокета.
"      var Socket;"
      /** При каждом клике по кнопке 'BTN_CAPTURE' вызывать функцию button_capture.**/
"      document.getElementById('BTN_CAPTURE').addEventListener('click', button_capture);"
      /** Функция инициализации вебсокета. -----------------------**/
"      function init() {"
        // Экземпляр вебсокета ссылается на 81 порт.
"        Socket = new WebSocket('ws://' + window.location.hostname + ':81/');"
        // При приёме сообщения сокетом вызываем функцию ответного вызова.
"        Socket.onmessage = function(event) {"
"          document.getElementById('img').src = URL.createObjectURL(event.data);"
"        };"
"      }"
      /** Функция отправляет JSON строку через соеденение сокетов при клике по кнопке 'BTN_CAPTURE'.**/
"      function button_capture() {"
        // JSON строка даст знать контролеру что надо сделать фото.
"        var btn_cpt = {type: 'capture', value: true};"
"        Socket.send(JSON.stringify(btn_cpt));"
"      }"
      /** Первым делом при подключении клиента к серверу должен быть инициализирован вебсокет. ---------------------**/
"      window.onload = function(event) {"
"      init();}"
"    </script>"
"  </html>";

return html;
}