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
                "      <div>ORIGINAL: </div>"
                "      <img id='imgORIGINAL'>"
                "      <div>GRAY = 0.114 * BLUE + 0.587 * GREEN + 0.299 * RED: </div>"
                "      <img id='imgGRAY'>"
                "      <div>Identical Transformation:<br/>"
                "         | 0  0  0 |<br/>"
                "         | 0  1  0 |<br/>"
                "         | 0  0  0 |</div>"
                "      <img id='imgIDENTICAL'>"
                "      <div>Blur:<br/>"
                "         | 0.0625 0.125 0.0625 |<br/>"
                "         | 0.1250 0.250 0.1250 |<br/>"
                "         | 0.0625 0.125 0.0625 |</div>"
                "      <img id='imgBLUR'>"
                "      <div>Increasing Sharpness:<br/>"
                "         | 0  -1 0 |<br/>"
                "         |-1  5 -1 |<br/>"
                "         | 0  -1 0 |</div>"
                "      <img id='imgSHARPNESS'>"
                "      <div>Dilation:<br/>"
                "         | 0  1  0 |<br/>"
                "         | 1  1  1 |<br/>"
                "         | 0  1  0 |</div>"
                "      <img id='imgDilation'>"
                "      <div>Vertical borders:<br/>"
                "         | 0  0  0 |<br/>"
                "         | 1 -2  1 |<br/>"
                "         | 0  0  0 |</div>"
                "      <img id='imgVERTICAL'>"
                "      <div>Horizontal borders:<br/>"
                "         | 0  1  0 |<br/>"
                "         | 0 -2  0 |<br/>"
                "         | 0  1  0 |</div>"
                "      <img id='imgHORIZONTAL'>"
                "      <div>Borders (Laplacian Operator):<br/>"
                "         | 0  1  0 |<br/>"
                "         | 1 -4  1 |<br/>"
                "         | 0  1  0 |</div>"
                "      <img id='imgBORDERS'>"
                "      <div>Roberts cross operator:<br/>"
                "         | 1  0 |  | 0  1 |<br/>"
                "         | 0 -1 |  |-1  0 |</div>"
                "      <img id='imgROBERTS'>"
                "      <div>Sobel operator,:<br/>"
                "         | 1  0  -1 | | 1  2  1 |<br/>"
                "         | 2  0  -2 | | 0  0  0 |<br/>"
                "         | 1  0  -1 | |-1 -2 -1 |</div>"
                "      <img id='imgSOBEL'>"
                "      <div>Prewitt*s operator:<br/>"
                "         | 1  0  -1 | | 1  1  1 |<br/>"
                "         | 1  0  -1 | | 0  0  0 |<br/>"
                "         | 1  0  -1 | |-1 -1 -1 |</div>"
                "      <img id='imgPREWITT'>"
                /** Кнопки:
        - сделать фото
        - перезагрузить страничку **/
                "      <p><button type='button' id='BTN_CAPTURE'> CAPTURE </button>"
                "      <button onclick='location.reload();'>REFRESH PAGE</button></p>"
                "    </body>"

                /** script **/  /////////////////////////////////////////////////////////////////////////////////////////////
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
                "         document.getElementById('imgIDENTICAL').src = URL.createObjectURL(event.data);"
                "       } else if(img_type == 3){"
                "         document.getElementById('imgBLUR').src = URL.createObjectURL(event.data);"
                "       } else if(img_type == 4){"
                "         document.getElementById('imgSHARPNESS').src = URL.createObjectURL(event.data);"
                "       } else if(img_type == 5){"
                "         document.getElementById('imgDilation').src = URL.createObjectURL(event.data);"
                "       } else if(img_type == 6){"
                "         document.getElementById('imgVERTICAL').src = URL.createObjectURL(event.data);"
                "       } else if(img_type == 7){"
                "         document.getElementById('imgHORIZONTAL').src = URL.createObjectURL(event.data);"
                "       } else if(img_type == 8){"
                "         document.getElementById('imgBORDERS').src = URL.createObjectURL(event.data);"
                "       } else if(img_type == 9){"
                "         document.getElementById('imgROBERTS').src = URL.createObjectURL(event.data);"
                "       } else if(img_type == 10){"
                "         document.getElementById('imgSOBEL').src = URL.createObjectURL(event.data);"
                "       } else if(img_type == 11){"
                "         document.getElementById('imgPREWITT').src = URL.createObjectURL(event.data);"
                "       }"
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