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
      /** Изображение SPIFFS. **/
"      <div>SPIFFS: </div>"
"      <img id='img_SPIFFS'>"

      /** Изображение SD cart. **/
"      <div>SD Cart: </div>"
"      <img id='img_SDcart'>"
      /** Выпадающий список (select). **/
"      <div id='container'></div>"

      /** Кнопки:
        - сделать фото
        - перезагрузить страничку **/
"      <p><button type='button' id='BTN_CAPTURE'> CAPTURE </button>"
"      <button onclick='location.reload();'>REFRESH PAGE</button></p>"

"    </body>"

    /** script **///////////////////////////////////
"    <script>"
      // Создание экземпляр вебсокета.
"      var Socket;"
      // Создание элемента select.
"      let select = document.createElement('select');"
      // Переменная должна укзывать на то, какое изображение принимаем  (img_SPIFFS или img_SDcart).
"      var img_type = 0;"
      /** При каждом клике по кнопке 'BTN_CAPTURE' вызывать функцию button_capture.**/
"      document.getElementById('BTN_CAPTURE').addEventListener('click', button_capture);"

      /** Функция инициализации вебсокета. -----------------------**/
"      function init() {"
        // Экземпляр вебсокета ссылается на 81 порт.
"        Socket = new WebSocket('ws://' + window.location.hostname + ':81/');"
        // При приёме сообщения сокетом вызываем функцию обработки полученых данных.
"        Socket.onmessage = function(event) {"
"          processCommand(event);"
"        };"
"      }"
      
      /** Функция отправляет JSON строку через соеденение сокетов при клике по кнопке 'BTN_CAPTURE'.**/
"      function button_capture() {"
      // JSON строка даст знать контролеру что надо сделать фото.
"        var btn_cpt = {type: 'capture', value: true};"
"        Socket.send(JSON.stringify(btn_cpt));"
"      }"
      
      /** Функция обрабатывает данные полученные через соеденение вебсокетов.**/
"      function processCommand(event){"
        // Если данные — объект Blob (например, изображение)
"        if (event.data instanceof Blob) {"
          // Разместить изображение в соответствующем окне.
"          if(img_type == 0){"
"            document.getElementById('img_SPIFFS').src = URL.createObjectURL(event.data);"
"          } else if(img_type == 1){"
"            document.getElementById('img_SDcart').src = URL.createObjectURL(event.data);"
"          }"
"        } else {"
"          try {"
            // Принимаем json обьект и достаём из него данные.
"            var obj = JSON.parse(event.data);"
            // Определяем тип JSON строки.
"            var type = obj.type;"
            
            // Указываем какое изображение должно придти в следующем сообщении.
"            if(type.localeCompare(\"change_img_type\") == 0){"
"              img_type = parseInt(obj.value);"
            // Записываем в select имена всех фото что есть на карте памяти.
"            }else if(type.localeCompare(\"img_list\") == 0){"
              // Очистка select
"              select.innerHTML = '';"
              // Заполнение select опциями на основе значений JSON-объекта
"              for (let key in obj) {"
                  // Игнорируем 'type' и 'img_count'
"                if (key !== 'type' && key !== 'img_count') {"
"                  let option = document.createElement('option');"
"                  option.value = obj[key];"
"                  option.text = obj[key];"
"                  select.appendChild(option);"
"                }"
"              }"
              // Добавление select в контейнер.
"              document.getElementById('container').appendChild(select);"
"            }"
"          } catch (e) {"
"          console.error(\"Received data is neither Blob nor valid JSON:\", event.data);"
"          }"
"        }"
"      }"
      
      // Обработка события выбора опции в select
"      select.addEventListener('change', function () {"
"        let selectedValue = select.value;"
"        console.log('Selected value:', selectedValue);"

        // Отправка выбранного значения через WebSocket
"        var msg = {type: 'img_SDcart_send', value: selectedValue};"
"        Socket.send(JSON.stringify(msg));"
"      });"
      
"      window.onload = function(event) {"
"        init();"
"      }"
"    </script>"
"  </html>";



  return html;
}