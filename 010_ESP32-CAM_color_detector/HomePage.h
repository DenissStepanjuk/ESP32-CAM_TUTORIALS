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
      /** Вывод цвета. **/
"     <p>Color (area 1): <span id='id_label_area_1'>-</span></p>"
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
"          processCommand(event);"
"        };"
"      }"
		// Переменная позволяет вывести название цвета который был определён.
"		var label_area_1 = document.getElementById('id_label_area_1');"
		/** Функция обрабатывает данные полученные через соеденение вебсокетов с контролера.**/
"		function processCommand(event){"
			// Если данные — объект Blob (например, изображение)
"			if (event.data instanceof Blob) {"
				// Разместить изображение в соответствующем окне.
"				document.getElementById('img').src = URL.createObjectURL(event.data);"
"			} else {"
"				  try {"
					  // Принимаем json обьект и достаём из него данные.
"					  var obj = JSON.parse(event.data);"
					  // Определяем тип JSON строки.
"					  var type = obj.type;"
					  // Обновляем параметры на вебстраничке на основе полученых данных.
"					  if(type.localeCompare(\"area_1\") == 0){"
						  // Выводим название цвета который был определён.
"						  label_area_1.innerHTML = obj.value;"
"           }"
"				  } catch (e) { console.error(\"Received data is neither Blob nor valid JSON:\", event.data); }"
"			  }"
"		  }"
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