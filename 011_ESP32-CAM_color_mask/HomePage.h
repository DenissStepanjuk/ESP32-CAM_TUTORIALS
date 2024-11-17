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
      /** Изображения. **/
"      <div>ORIGINAL: </div>"
"      <img id='imgORIGINAL'>"
"      <div>MASK: </div>"
"      <img id='imgHSV'>"
      /** Кнопки:
        - сделать фото
        - перезагрузить страничку **/
"      <p><button type='button' id='BTN_CAPTURE'> CAPTURE </button>"
"      <button onclick='location.reload();'>REFRESH PAGE</button></p>"

		  /** Задать значение HSV параметра через ползунок. ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/
"		  <p>HUE minimum: <span id='id_show_hue_minimum'>-</span>"
"		  <input type='range' min='0' max='255' value='0' class='slider' id='id_slider_hue_minimum'></p>"
		
"		  <p>HUE maximum: <span id='id_show_hue_maximum'>-</span>"
"		  <input type='range' min='0' max='255' value='255' class='slider' id='id_slider_hue_maximum'></p>"
		
"		  <p>SATURATION minimum: <span id='id_show_saturation_minimum'>-</span>"
"		  <input type='range' min='0' max='255' value='0' class='slider' id='id_slider_saturation_minimum'></p>"
		
"		  <p>SATURATION maximum: <span id='id_show_saturation_maximum'>-</span>"
"		  <input type='range' min='0' max='255' value='255' class='slider' id='id_slider_saturation_maximum'></p>"
		
"		  <p>VALUE minimum: <span id='id_show_value_minimum'>-</span>"
"		  <input type='range' min='0' max='255' value='0' class='slider' id='id_slider_value_minimum'></p>"
		
"		  <p>VALUE maximum: <span id='id_show_value_maximum'>-</span>"
"		  <input type='range' min='0' max='255' value='255' class='slider' id='id_slider_value_maximum'></p>"



"      <p>RED: 236 - 255 ____________ Sat: 54 - 255 ______ Val: 28 - 232</p>"
"      <p>Purple: 212 - 240 ___________ Sat: 54 - 255 ______ Val: 28 - 232</p>"
"      <p>Dark purple: 181 - 214 ______ Sat: 54 - 255 ______ Val: 28 - 232</p>"
"      <p>Dark blue: 142 - 181 ________ Sat: 54 - 255 ______ Val: 28 - 232</p>"
"      <p>Blue: 119 - 144 ____________ Sat: 54 - 255 ______ Val: 28 - 232</p>"
"      <p>Green: 85 - 119 ____________ Sat: 54 - 255 ______ Val: 28 - 232</p>"
"      <p>Yellow: 25 - 82 ____________ Sat: 20 - 255 ______ Val: 28 - 255</p>"
"      <p>Orange: 0 - 27 _____________ Sat: 20 - 255 ______ Val: 28 - 255</p>"
		  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

		/** Задать значение параметра через ползунок. +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/

		// Значение ползунка регулирующего параметр brightness.
"		var slider_hue_minimum = document.getElementById('id_slider_hue_minimum');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_hue_minimum.addEventListener('change', function () {"
"			var msg = {type: 'hue_minimum', value: slider_hue_minimum.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_hue_minimum = document.getElementById('id_show_hue_minimum');"

		// Значение ползунка регулирующего параметр brightness.
"		var slider_hue_maximum = document.getElementById('id_slider_hue_maximum');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_hue_maximum.addEventListener('change', function () {"
"			var msg = {type: 'hue_maximum', value: slider_hue_maximum.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_hue_maximum = document.getElementById('id_show_hue_maximum');"

		// Значение ползунка регулирующего параметр brightness.
"		var slider_saturation_minimum = document.getElementById('id_slider_saturation_minimum');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_saturation_minimum.addEventListener('change', function () {"
"			var msg = {type: 'saturation_minimum', value: slider_saturation_minimum.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_saturation_minimum = document.getElementById('id_show_saturation_minimum');"

		// Значение ползунка регулирующего параметр brightness.
"		var slider_saturation_maximum = document.getElementById('id_slider_saturation_maximum');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_saturation_maximum.addEventListener('change', function () {"
"			var msg = {type: 'saturation_maximum', value: slider_saturation_maximum.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_saturation_maximum = document.getElementById('id_show_saturation_maximum');"

		// Значение ползунка регулирующего параметр brightness.
"		var slider_value_minimum = document.getElementById('id_slider_value_minimum');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_value_minimum.addEventListener('change', function () {"
"			var msg = {type: 'value_minimum', value: slider_value_minimum.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_value_minimum = document.getElementById('id_show_value_minimum');"

		// Значение ползунка регулирующего параметр brightness.
"		var slider_value_maximum = document.getElementById('id_slider_value_maximum');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_value_maximum.addEventListener('change', function () {"
"			var msg = {type: 'value_maximum', value: slider_value_maximum.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_value_maximum = document.getElementById('id_show_value_maximum');"



    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Переменная должна укзывать на то, какое изображение принимаем  (imgFULL или imgCROP).
"   var img_type = 0;"
		/** Функция обрабатывает данные полученные через соеденение вебсокетов с контролера.**/
"		function processCommand(event){"
			// Если данные — объект Blob (например, изображение)
"			if (event.data instanceof Blob) {"
				// Разместить изображение в соответствующем окне.
"       if(img_type == 0){"
"         document.getElementById('imgORIGINAL').src = URL.createObjectURL(event.data);"
"       } else if(img_type == 1){"
"         document.getElementById('imgHSV').src = URL.createObjectURL(event.data);"
"       } "
"			} else {"
"				  try {"
					  // Принимаем json обьект и достаём из него данные.
"					  var obj = JSON.parse(event.data);"
					  // Определяем тип JSON строки.
"					  var type = obj.type;"
					  // Обновляем параметры на вебстраничке на основе полученых данных.
"					  if(type.localeCompare(\"change_img_type\") == 0){"
"             img_type = obj.value;"
"           }"
            // Обновляем значение ползунка. +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
"					  else if(type.localeCompare(\"hue_minimum\") == 0){"
						  // Принимаем значение ползунка с контролера.
"						  var hue_minimum_value = parseInt(obj.value);"
						  // Обновляем ползунок.
"						  slider_hue_minimum.value = hue_minimum_value;"
						  // Обновляем отображение значения ползунка на вебстраничке.
"						  show_hue_minimum.innerHTML = hue_minimum_value;"
"					  } else if(type.localeCompare(\"hue_maximum\") == 0){"
						  // Принимаем значение ползунка с контролера.
"						  var hue_maximum_value = parseInt(obj.value);"
						  // Обновляем ползунок.
"						  slider_hue_maximum.value = hue_maximum_value;"
						  // Обновляем отображение значения ползунка на вебстраничке.
"						  show_hue_maximum.innerHTML = hue_maximum_value;"
"					  } else if(type.localeCompare(\"saturation_minimum\") == 0){"
						  // Принимаем значение ползунка с контролера.
"						  var saturation_minimum_value = parseInt(obj.value);"
						  // Обновляем ползунок.
"						  slider_saturation_minimum.value = saturation_minimum_value;"
						  // Обновляем отображение значения ползунка на вебстраничке.
"						  show_saturation_minimum.innerHTML = saturation_minimum_value;"
"					  } else if(type.localeCompare(\"saturation_maximum\") == 0){"
						  // Принимаем значение ползунка с контролера.
"						  var saturation_maximum_value = parseInt(obj.value);"
						  // Обновляем ползунок.
"						  slider_saturation_maximum.value = saturation_maximum_value;"
						  // Обновляем отображение значения ползунка на вебстраничке.
"						  show_saturation_maximum.innerHTML = saturation_maximum_value;"
"					  } else if(type.localeCompare(\"value_minimum\") == 0){"
						  // Принимаем значение ползунка с контролера.
"						  var value_minimum_value = parseInt(obj.value);"
						  // Обновляем ползунок.
"						  slider_value_minimum.value = value_minimum_value;"
						  // Обновляем отображение значения ползунка на вебстраничке.
"						  show_value_minimum.innerHTML = value_minimum_value;"
"					  } else if(type.localeCompare(\"value_maximum\") == 0){"
						  // Принимаем значение ползунка с контролера.
"						  var value_maximum_value = parseInt(obj.value);"
						  // Обновляем ползунок.
"						  slider_value_maximum.value = value_maximum_value;"
						  // Обновляем отображение значения ползунка на вебстраничке.
"						  show_value_maximum.innerHTML = value_maximum_value;"
"					  }"
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