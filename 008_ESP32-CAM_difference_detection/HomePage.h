// Функция подготавливает и возвращает HTML страничку.

String getHTML() {
  String html = ""
                "  <!DOCTYPE html>"
                "  <html>"
                /** head **/  /////////////////////////////////////////////////////////////////////////////////////////////////////
                "    <head>"
                "      <title>ESP32-CAM</title>"
                "    </head>"

                /** body **/  /////////////////////////////////////////////////////////////////////////////////////////////
                "    <body style='background-color: #EEEEEE;'>"
                /** Изображения. **/
                "      <div>MAIN: </div>"
                "      <img id='imgMAIN'>"
                "      <div>CURRENT: </div>"
                "      <img id='imgCURRENT'>"
             
                /** Кнопки:
                - сделать фото
                - перезагрузить страничку **/
                "      <p><button type='button' id='BTN_CAPTURE_MAIN'> SET MAIN </button>"
                "         <button type='button' id='BTN_CAPTURE_CURRENT'> GET CURRENT </button>"
                "      <button onclick='location.reload();'>REFRESH PAGE</button></p>"
                // Вывод информации.
                "     <p>Red difference: <span id='id_label_red'>-</span></p>"
                "     <p>Green difference: <span id='id_label_green'>-</span></p>"
                "     <p>Blue difference: <span id='id_label_blue'>-</span></p>"
                "     <p>AVERAGE: <span id='id_label_average'>-</span></p>"
                "    </body>"

                /** script **/  /////////////////////////////////////////////////////////////////////////////////////////////
                "    <script>"
                // Создание экземпляр вебсокета.
                "      var Socket;"
                /** При каждом клике по кнопке 'BTN_CAPTURE_MAIN' вызывать функцию button_capture_main.**/
                "      document.getElementById('BTN_CAPTURE_MAIN').addEventListener('click', button_capture_main);"
                /** При каждом клике по кнопке 'BTN_CAPTURE_CURRENT' вызывать функцию button_capture_current.**/
                "      document.getElementById('BTN_CAPTURE_CURRENT').addEventListener('click', button_capture_current);"
                /** Функция инициализации вебсокета. -----------------------**/
                "      function init() {"
                // Экземпляр вебсокета ссылается на 81 порт.
                "        Socket = new WebSocket('ws://' + window.location.hostname + ':81/');"
                // При приёме сообщения сокетом вызываем функцию ответного вызова.
                "        Socket.onmessage = function(event) {"
                "          processCommand(event);"
                "        };"
                "      }"
                // Следующие переменные позволяют вывести параметры разницы между двумя изображениями.
                "		var label_red = document.getElementById('id_label_red');"
                "		var label_green = document.getElementById('id_label_green');"
                "		var label_blue = document.getElementById('id_label_blue');"
                "		var label_average = document.getElementById('id_label_average');"

                // Переменная должна укзывать на то, какое изображение принимаем  (imgFULL или imgCROP).
                "   var img_type = 0;"
                /** Функция обрабатывает данные полученные через соеденение вебсокетов с контролера.**/
                "		function processCommand(event){"
                // Если данные — объект Blob (например, изображение)
                "			if (event.data instanceof Blob) {"
                // Разместить изображение в соответствующем окне.
                "       if(img_type == 0){"
                "         document.getElementById('imgMAIN').src = URL.createObjectURL(event.data);"
                "       } else if(img_type == 1){"
                "         document.getElementById('imgCURRENT').src = URL.createObjectURL(event.data);"
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
                "           } else if(type.localeCompare(\"red\") == 0){"
                // Выводим параметр.
                "						  label_red.innerHTML = obj.value;"
                "           } else if(type.localeCompare(\"green\") == 0){"
                // Выводим параметр.
                "						  label_green.innerHTML = obj.value;"
                "           } else if(type.localeCompare(\"blue\") == 0){"
                // Выводим параметр.
                "						  label_blue.innerHTML = obj.value;"
                "           } else if(type.localeCompare(\"average\") == 0){"
                // Выводим параметр.
                "						  label_average.innerHTML = obj.value;"
                "           }"
                "				  } catch (e) { console.error(\"Received data is neither Blob nor valid JSON:\", event.data); }"
                "			  }"
                "		  }"
                /** Функция отправляет JSON строку через соеденение сокетов при клике по кнопке 'BTN_CAPTURE_MAIN'.**/
                "      function button_capture_main() {"
                // JSON строка даст знать контролеру что надо сделать фото.
                "        var btn_cpt = {type: 'capture_main', value: true};"
                "        Socket.send(JSON.stringify(btn_cpt));"
                "      }"
                /** Функция отправляет JSON строку через соеденение сокетов при клике по кнопке 'BTN_CAPTURE_CURRENT'.**/
                "      function button_capture_current() {"
                // JSON строка даст знать контролеру что надо сделать фото.
                "        var btn_cpt = {type: 'capture_current', value: true};"
                "        Socket.send(JSON.stringify(btn_cpt));"
                "      }"
                /** Первым делом при подключении клиента к серверу должен быть инициализирован вебсокет. ---------------------**/
                "      window.onload = function(event) {"
                "      init();}"
                "    </script>"
                "  </html>";

  return html;
}
