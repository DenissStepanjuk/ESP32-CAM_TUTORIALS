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
"      <div>Input to TFL model:<br/>"
"      <img id='imgGRAY'>"
      /** Кнопки:
        - сделать фото
        - перезагрузить страничку **/
"      <p><button type='button' id='BTN_classify'> CLASSIFY </button>"
"      <button type='button' id='BTN_dataset'> DATASET </button>"
"      <button onclick='location.reload();'>REFRESH PAGE</button></p>";

  // Вывод списка классов, которые может определить модель.
  for(int i = 0; i < kCategoryCount; i++){
        
    html += "<p>"+ String(i + 1) + ") " + String(kCategoryLabels[i]) + ": <span id='id_show_probability_for_" +  String(kCategoryLabels[i]) + "'> - </span> </p>";

  }

  html += ""
"    </body>"
      
    /** script **//////////////////////////////////////////////////////////////////////////////////////////////
"    <script>"
      // Создание экземпляр вебсокета.
"      var Socket;"
      /** При каждом клике по кнопке 'BTN_classify' вызывать функцию button_classify.**/
"      document.getElementById('BTN_classify').addEventListener('click', button_classify);"
"      document.getElementById('BTN_dataset').addEventListener('click', button_dataset);"
      /** Функция инициализации вебсокета. -----------------------**/
"      function init() {"
        // Экземпляр вебсокета ссылается на 81 порт.
"        Socket = new WebSocket('ws://' + window.location.hostname + ':81/');"
        // При приёме сообщения сокетом вызываем функцию ответного вызова.
"        Socket.onmessage = function(event) {"
"          processCommand(event);"
"        };"
"      }"
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
"         document.getElementById('imgGRAY').src = URL.createObjectURL(event.data);"
"       } else if(img_type == 2){"
"         document.getElementById('imgBORDERS').src = URL.createObjectURL(event.data);"
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
"           }";


// Вывод вероятности для каждого класса из тех, которые может определить модель.

  for(int i = 0; i < kCategoryCount; i++){

  html += ""

"					else if(type.localeCompare(\"obj_" + String(i + 1) + "\") == 0){"

"         document.getElementById('id_show_probability_for_" + String(kCategoryLabels[i]) + "').innerHTML = parseInt(obj.value); }";

  }


  html += ""
"				  } catch (e) { console.error(\"Received data is neither Blob nor valid JSON:\", event.data); }"
"			  }"
"		  }"
      /** Функция отправляет JSON строку через соеденение сокетов при клике по кнопке 'CLASSIFY'.**/
"      function button_classify() {"
        // JSON строка даст знать контролеру что надо сделать фото и определить что изображено.
"        var btn_cpt = {type: 'classify', value: true};"
"        Socket.send(JSON.stringify(btn_cpt));"
"      }"
        /** Функция отправляет JSON строку через соеденение сокетов при клике по кнопке 'DATASET'.**/
"      function button_dataset() {"
        // JSON строка даст знать контролеру что надо сделать фото и сохранить на карте памяти.
"        var btn_cpt = {type: 'dataset', value: true};"
"        Socket.send(JSON.stringify(btn_cpt));"
"      }"
      /** Первым делом при подключении клиента к серверу должен быть инициализирован вебсокет. ---------------------**/
"      window.onload = function(event) {"
"      init();}"

"    </script>"
"  </html>";

return html;
}