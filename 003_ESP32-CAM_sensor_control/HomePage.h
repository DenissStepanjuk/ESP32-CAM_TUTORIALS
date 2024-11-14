// Функция подготавливает и возвращает HTML страничку.

String getHTML() {
  String html = ""
"<!DOCTYPE html>"
"<html>"
	/** head **/
"	<head>"
"		<title>ESP32-CAM</title>"
"	</head>"

	/** body **//////////////////////////////////////////////////////////////////////////////////////////////
"	<body style='background-color: #EEEEEE;'>"
	
		/** Изображение. **/
"		<img id='img'>"
		
		/** Кнопки:
			- сделать фото
			- перезагрузить страничку **/
"		<p><button type='button' id='BTN_CAPTURE'> CAPTURE </button>"
"		<button onclick='location.reload();'>REFRESH PAGE</button></p>"
		
		/** Выбор фильтров: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		WB MODES (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
		Каждой кнопке присвоен свой ID и соответствует свой фильтр. **/
"		<form>"
"			<p>WB MODES:</p>"
"			<div>"
				// Кнопка 1 - режмим Auto.
"				<input type='radio' id='id_wb_mode_0' name='operation_mode'>"
"				<label for='id_wb_mode_0'>Auto</label>"
				// Кнопка 2 - режмим Sunny.
"				<input type='radio' id='id_wb_mode_1' name='operation_mode'>"
"				<label for='id_wb_mode_1'>Sunny</label>"
				// Кнопка 3 - режмим Cloudy.
"				<input type='radio' id='id_wb_mode_2' name='operation_mode'>"
"				<label for='id_wb_mode_2'>Cloudy</label>"
				// Кнопка 4 - режмим Office.
"				<input type='radio' id='id_wb_mode_3' name='operation_mode'>"
"				<label for='id_wb_mode_3'>Office</label>"
				// Кнопка 5 - режмим Home.
"				<input type='radio' id='id_wb_mode_4' name='operation_mode'>"
"				<label for='id_wb_mode_4'>Home</label>"
"			</div>"
"		</form>"
		
		/** SPECIAL EFFECTS (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
		Каждой кнопке присвоен свой ID и соответствует свой фильтр. **/
"		<form>"
"			<p>SPECIAL EFFECTS:</p>"
"			<div>"
				// Кнопка 1 - режмим No Effect.
"				<input type='radio' id='id_special_effects_0' name='operation_mode'>"
"				<label for='id_special_effects_0'>No Effect</label>"
				// Кнопка 2 - режмим Negative.
"				<input type='radio' id='id_special_effects_1' name='operation_mode'>"
"				<label for='id_special_effects_1'>Negative</label>"
				// Кнопка 3 - режмим Grayscale.
"				<input type='radio' id='id_special_effects_2' name='operation_mode'>"
"				<label for='id_special_effects_2'>Grayscale</label>"
				// Кнопка 4 - режмим Red Tint.
"				<input type='radio' id='id_special_effects_3' name='operation_mode'>"
"				<label for='id_special_effects_3'>Red Tint</label>"
				// Кнопка 5 - режмим Green Tint.
"				<input type='radio' id='id_special_effects_4' name='operation_mode'>"
"				<label for='id_special_effects_4'>Green Tint</label>"
				// Кнопка 6 - режмим Blue Tint.
"				<input type='radio' id='id_special_effects_5' name='operation_mode'>"
"				<label for='id_special_effects_5'>Blue Tint</label>"
				// Кнопка 7 - режмим Sepia.
"				<input type='radio' id='id_special_effects_6' name='operation_mode'>"
"				<label for='id_special_effects_6'>Sepia</label>"
"			</div>"
"		</form>"
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		
		/** Задать значение параметра через ползунок. ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/
		
"		<p>BRIGHTNESS (-2 to 2): <span id='id_show_slider_brightness'>-</span>"
"		<input type='range' min='-2' max='2' value='0' class='slider' id='id_slider_brightness'></p>"
		
"		<p>CONTRAST (-2 to 2): <span id='id_show_slider_contrast'>-</span>"
"		<input type='range' min='-2' max='2' value='0' class='slider' id='id_slider_contrast'></p>"
		
"		<p>SATURATION (-2 to 2): <span id='id_show_slider_saturation'>-</span>"
"		<input type='range' min='-2' max='2' value='0' class='slider' id='id_slider_saturation'></p>"
		
"		<p>AE LEVELS (-2 to 2): <span id='id_show_slider_ae_level'>-</span>"
"		<input type='range' min='-2' max='2' value='0' class='slider' id='id_slider_ae_level'></p>"
		
"		<p>GAIN CEILING (0 to 6): <span id='id_show_slider_gainceiling'>-</span>"
"		<input type='range' min='0' max='6' value='0' class='slider' id='id_slider_gainceiling'></p>"
		
"		<p>AGC GAIN (0 to 30): <span id='id_show_slider_agc_gain'>-</span>"
"		<input type='range' min='0' max='30' value='0' class='slider' id='id_slider_agc_gain'></p>"
		
"		<p>AEC VALUES (0 to 1200): <span id='id_show_slider_aec_value'>-</span>"
"		<input type='range' min='0' max='1200' value='300' class='slider' id='id_slider_aec_value'></p>"
		
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		
		/** Кнопки подключения/отключения параметров. ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''**/
		
"		<p>WHITE BALANCE: <button type='button' id='id_btn_whitebal'> <span id='id_label_whitebal'>ON</span> </button></p>"
		
"		<p>AWB GAIN: <button type='button' id='id_btn_awb_gain'> <span id='id_label_awb_gain'>ON</span> </button></p>"
		
"		<p>EXPOSURE CONTROLS: <button type='button' id='id_btn_exposure_ctrl'> <span id='id_label_exposure_ctrl'>ON</span> </button></p>"
		
"		<p>GAIN CONTROLS: <button type='button' id='id_btn_gain_ctrl'> <span id='id_label_gain_ctrl'>ON</span> </button></p>"

"		<p>WPC: <button type='button' id='id_btn_wpc'> <span id='id_label_wpc'>ON</span> </button></p>"
		
"		<p>RAW GMA : <button type='button' id='id_btn_raw_gma'> <span id='id_label_raw_gma'>ON</span> </button></p>"
		
"		<p>LENC: <button type='button' id='id_btn_ae_lenc'> <span id='id_label_ae_lenc'>ON</span> </button></p>"
		
"		<p>DCW: <button type='button' id='id_btn_dcw'> <span id='id_label_dcw'>ON</span> </button></p>"
		
"		<p>COLOR BAR PATTERN: <button type='button' id='id_btn_colorbar'> <span id='id_label_colorbar'>OFF</span> </button></p>"
		
"		<p>HORIZ MIRROR: <button type='button' id='id_btn_hmirror'> <span id='id_label_hmirror'>OFF</span> </button></p>"
		
"		<p>VERT FLIP: <button type='button' id='id_btn_vflip'> <span id='id_label_vflip'>OFF</span> </button></p>"
		
"		<p>BPC: <button type='button' id='id_btn_bpc'> <span id='id_label_bpc'>OFF</span> </button></p>"
		
"		<p>AEC2: <button type='button' id='id_btn_aec2'> <span id='id_label_aec2'>OFF</span> </button></p>"
		
		// '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

"	</body>" ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** script **************************************************************************************************************************/
"	<script>"
"		var Socket;"
		/** При каждом клике по кнопке 'BTN_CAPTURE' вызывать функцию button_capture.**/
"		document.getElementById('BTN_CAPTURE').addEventListener('click', button_capture);"
		
		/** Функция отправляет JSON строку через соеденение сокетов при клике по кнопке 'BTN_CAPTURE'.**/
"		function button_capture() {"
			// JSON строка даст знать контролеру что надо сделать фото.
"			var btn_cpt = {type: 'capture', value: true};"
"			Socket.send(JSON.stringify(btn_cpt));"
"		}"
		
		// Выбор фильтров: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// При выборе ' WB MODES ' вызвать функцию wb_mode_changed.
"		document.getElementById('id_wb_mode_0').checked = true;"
"		document.getElementById('id_wb_mode_0').addEventListener('click', wb_mode_changed);"
"		document.getElementById('id_wb_mode_1').addEventListener('click', wb_mode_changed);"
"		document.getElementById('id_wb_mode_2').addEventListener('click', wb_mode_changed);"
"		document.getElementById('id_wb_mode_3').addEventListener('click', wb_mode_changed);"
"		document.getElementById('id_wb_mode_4').addEventListener('click', wb_mode_changed);"

		/** Функция переключения между фильтрами ' WB MODES '. **/
"		function wb_mode_changed(){"
			// По умолчанию выбран режим Auto.
"			var wb_mode_selected = 0;"
			// Выбрать режим Sunny.
"			if(document.getElementById('id_wb_mode_1').checked == true){"
"				wb_mode_selected = 1;}"
			// Выбрать режим Cloudy.
"			else if(document.getElementById('id_wb_mode_2').checked == true){"
"				wb_mode_selected = 2;} "
			// Выбрать режим Office.
"			else if(document.getElementById('id_wb_mode_3').checked == true){"
"				wb_mode_selected = 3;}"
			// Выбрать режим Home.
"			else if(document.getElementById('id_wb_mode_4').checked == true){"
"				wb_mode_selected = 4;}"
			// Вывести в консоль браузера выбраный режим.
"			console.log(wb_mode_selected);"
			// Отправить номер выбранного режима на контроллер через соеденение сокетов используя JSON.
"			var msg = {type: 'wb_mode', value: wb_mode_selected};"
"			Socket.send(JSON.stringify(msg));"
"		}"
		
		// При выборе ' SPECIAL EFFECTS ' вызвать функцию special_effects_changed.
"		document.getElementById('id_special_effects_0').checked = true;"
"		document.getElementById('id_special_effects_0').addEventListener('click', special_effects_changed);"
"		document.getElementById('id_special_effects_1').addEventListener('click', special_effects_changed);"
"		document.getElementById('id_special_effects_2').addEventListener('click', special_effects_changed);"
"		document.getElementById('id_special_effects_3').addEventListener('click', special_effects_changed);"
"		document.getElementById('id_special_effects_4').addEventListener('click', special_effects_changed);"
"		document.getElementById('id_special_effects_5').addEventListener('click', special_effects_changed);"
"		document.getElementById('id_special_effects_6').addEventListener('click', special_effects_changed);"
		
		/** Функция переключения между фильтрами ' WB MODES '. **/
"		function special_effects_changed(){"
			// По умолчанию выбран режим No Effect.
"			var special_effects_selected = 0;"
			// Выбрать режим Negative.
"			if(document.getElementById('id_special_effects_1').checked == true){"
"				special_effects_selected = 1;}"
			// Выбрать режим Grayscale.
"			else if(document.getElementById('id_special_effects_2').checked == true){"
"				special_effects_selected = 2;} "
			// Выбрать режим Red Tint.
"			else if(document.getElementById('id_special_effects_3').checked == true){"
"				special_effects_selected = 3;}"
			// Выбрать режим Green Tint.
"			else if(document.getElementById('id_special_effects_4').checked == true){"
"				special_effects_selected = 4;}"
			// Выбрать режим Blue Tint.
"			else if(document.getElementById('id_special_effects_5').checked == true){"
"				special_effects_selected = 5;}"
			// Выбрать режим Sepia.
"			else if(document.getElementById('id_special_effects_6').checked == true){"
"				special_effects_selected = 6;}"
			// Вывести в консоль браузера выбраный режим.
"			console.log(special_effects_selected);"
			// Отправить номер выбранного режима на контроллер через соеденение сокетов используя JSON.
"			var msg = {type: 'special_effects', value: special_effects_selected};"
"			Socket.send(JSON.stringify(msg));"
"		}"
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		/** Задать значение параметра через ползунок. +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/
		// Значение ползунка регулирующего параметр brightness.
"		var slider_brightness = document.getElementById('id_slider_brightness');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_brightness.addEventListener('change', function () {"
"			var msg = {type: 'brightness', value: slider_brightness.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_slider_brightness = document.getElementById('id_show_slider_brightness');"
		
		// Значение ползунка регулирующего параметр contrast.
"		var slider_contrast = document.getElementById('id_slider_contrast');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_contrast.addEventListener('change', function () {"
"			var msg = {type: 'contrast', value: slider_contrast.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_slider_contrast = document.getElementById('id_show_slider_contrast');"
		
		// Значение ползунка регулирующего параметр saturation.
"		var slider_saturation = document.getElementById('id_slider_saturation');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_saturation.addEventListener('change', function () {"
"			var msg = {type: 'saturation', value: slider_saturation.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_slider_saturation = document.getElementById('id_show_slider_saturation');"
		
		// Значение ползунка регулирующего параметр ae_level.
"		var slider_ae_level = document.getElementById('id_slider_ae_level');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_ae_level.addEventListener('change', function () {"
"			var msg = {type: 'ae_level', value: slider_ae_level.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_slider_ae_level = document.getElementById('id_show_slider_ae_level');"
		
		// Значение ползунка регулирующего параметр gainceiling.
"		var slider_gainceiling = document.getElementById('id_slider_gainceiling');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_gainceiling.addEventListener('change', function () {"
"			var msg = {type: 'gainceiling', value: slider_gainceiling.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_slider_gainceiling = document.getElementById('id_show_slider_gainceiling');"
		
		// Значение ползунка регулирующего параметр agc_gain.
"		var slider_agc_gain = document.getElementById('id_slider_agc_gain');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_agc_gain.addEventListener('change', function () {"
"			var msg = {type: 'agc_gain', value: slider_agc_gain.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_slider_agc_gain = document.getElementById('id_show_slider_agc_gain');"
		
		// Значение ползунка регулирующего параметр aec_value.
"		var slider_aec_value = document.getElementById('id_slider_aec_value');"
		// При перемещении ползунка отправить новое значение на контролер.
"		slider_aec_value.addEventListener('change', function () {"
"			var msg = {type: 'aec_value', value: slider_aec_value.value};"
"			Socket.send(JSON.stringify(msg));});"
		// Переменная для отображения значения ползунка на вебстраничке для пользователя.
"		var show_slider_aec_value = document.getElementById('id_show_slider_aec_value');"
		
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


		// Обработка кнопок подключения/отключения параметров ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
		
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_whitebal = 1;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_whitebal = document.getElementById('id_label_whitebal');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_whitebal').addEventListener('click', function () {"
"			var msg = {type: 'whitebal', value: btn_whitebal};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_awb_gain = 1;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_awb_gain = document.getElementById('id_label_awb_gain');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_awb_gain').addEventListener('click', function () {"
"			var msg = {type: 'awb_gain', value: btn_awb_gain};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_exposure_ctrl = 1;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_exposure_ctrl = document.getElementById('id_label_exposure_ctrl');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_exposure_ctrl').addEventListener('click', function () {"
"			var msg = {type: 'exposure_ctrl', value: btn_exposure_ctrl};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_gain_ctrl = 1;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_gain_ctrl = document.getElementById('id_label_gain_ctrl');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_gain_ctrl').addEventListener('click', function () {"
"			var msg = {type: 'gain_ctrl', value: btn_gain_ctrl};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_wpc = 1;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_wpc = document.getElementById('id_label_wpc');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_wpc').addEventListener('click', function () {"
"			var msg = {type: 'wpc', value: btn_wpc};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_raw_gma = 1;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_raw_gma = document.getElementById('id_label_raw_gma');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_raw_gma').addEventListener('click', function () {"
"			var msg = {type: 'raw_gma', value: btn_raw_gma};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_ae_lenc = 1;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_ae_lenc = document.getElementById('id_label_ae_lenc');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_ae_lenc').addEventListener('click', function () {"
"			var msg = {type: 'ae_lenc', value: btn_ae_lenc};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_dcw = 1;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_dcw = document.getElementById('id_label_dcw');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_dcw').addEventListener('click', function () {"
"			var msg = {type: 'dcw', value: btn_dcw};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_colorbar = 0;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_colorbar = document.getElementById('id_label_colorbar');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_colorbar').addEventListener('click', function () {"
"			var msg = {type: 'colorbar', value: btn_colorbar};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_hmirror = 0;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_hmirror = document.getElementById('id_label_hmirror');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_hmirror').addEventListener('click', function () {"
"			var msg = {type: 'hmirror', value: btn_hmirror};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_vflip = 0;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_vflip = document.getElementById('id_label_vflip');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_vflip').addEventListener('click', function () {"
"			var msg = {type: 'vflip', value: btn_vflip};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_bpc = 0;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_bpc = document.getElementById('id_label_bpc');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_bpc').addEventListener('click', function () {"
"			var msg = {type: 'bpc', value: btn_bpc};"
"			Socket.send(JSON.stringify(msg));});"
			
		// Переменная хранит состояние параметра вкл/выкл.
"		var btn_aec2 = 0;"
		// Переменная позволяет изменить надпись на кнопке. 
"		var label_aec2 = document.getElementById('id_label_aec2');"
		// При нажатии на кнопку отправить её состояние на контроллер.
"		document.getElementById('id_btn_aec2').addEventListener('click', function () {"
"			var msg = {type: 'aec2', value: btn_aec2};"
"			Socket.send(JSON.stringify(msg));});"
			
		// ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

		/** Функция инициализации вебсокета. -----------------------**/
"		function init() {"
			// Экземпляр вебсокета ссылается на 81 порт.
"			Socket = new WebSocket('ws://' + window.location.hostname + ':81/');"
			// При приёме сообщения сокетом вызываем функцию ответного вызова.
"			Socket.onmessage = function(event) {"
"				processCommand(event);"
"			};"
"		}"

		/** Функция обрабатывает данные полученные через соеденение вебсокетов с контролера.**/
"		function processCommand(event){"
			// Если данные — объект Blob (например, изображение)
"			if (event.data instanceof Blob) {"
				// Разместить изображение в соответствующем окне.
"				document.getElementById('img').src = URL.createObjectURL(event.data);"
"			} else {"
"				try {"
					// Принимаем json обьект и достаём из него данные.
"					var obj = JSON.parse(event.data);"
					// Определяем тип JSON строки.
"					var type = obj.type;"

					// Обновляем параметры на вебстраничке на основе полученых данных.
					// Выбор фильтров: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"					if(type.localeCompare(\"wb_mode\") == 0){"
						// Принимаем значение с номером выбраного фтльтра.
"						var wb_mode_selected = parseInt(obj.value);"
"						console.log(wb_mode_selected);"
						// Ставим галочку на место выбраного фтльтра.
"						if(wb_mode_selected == 0){"
"							document.getElementById('id_wb_mode_0').checked = true;"
"						} else if(wb_mode_selected == 1){"
"							document.getElementById('id_wb_mode_1').checked = true;"
"						} else if(wb_mode_selected == 2){"
"							document.getElementById('id_wb_mode_2').checked = true;"
"						} else if(wb_mode_selected == 3){"
"							document.getElementById('id_wb_mode_3').checked = true;"
"						} else if(wb_mode_selected == 4){"
"							document.getElementById('id_wb_mode_4').checked = true;"
"						}"
"					} else if(type.localeCompare(\"special_effects\") == 0){"
						// Принимаем значение с номером выбраного фтльтра.
"						var special_effects_selected = parseInt(obj.value);"
"						console.log(special_effects_selected);"
						// Ставим галочку на место выбраного фтльтра.
"						if(special_effects_selected == 0){"
"							document.getElementById('id_special_effects_0').checked = true;"
"						} else if(special_effects_selected == 1){"
"							document.getElementById('id_special_effects_1').checked = true;"
"						} else if(special_effects_selected == 2){"
"							document.getElementById('id_special_effects_2').checked = true;"
"						} else if(special_effects_selected == 3){"
"							document.getElementById('id_special_effects_3').checked = true;"
"						} else if(special_effects_selected == 4){"
"							document.getElementById('id_special_effects_4').checked = true;"
"						} else if(special_effects_selected == 5){"
"							document.getElementById('id_special_effects_5').checked = true;"
"						} else if(special_effects_selected == 6){"
"							document.getElementById('id_special_effects_6').checked = true;"
"						}"
"					}"
					// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
					// Обновляем значение ползунка. +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
"					else if(type.localeCompare(\"brightness\") == 0){"
						// Принимаем значение ползунка с контролера.
"						var brightness_value = parseInt(obj.value);"
						// Обновляем ползунок.
"						slider_brightness.value = brightness_value;"
						// Обновляем отображение значения ползунка на вебстраничке.
"						show_slider_brightness.innerHTML = brightness_value;"
"					} else if(type.localeCompare(\"contrast\") == 0){"
						// Принимаем значение ползунка с контролера.
"						var contrast_value = parseInt(obj.value);"
						// Обновляем ползунок.
"						slider_contrast.value = contrast_value;"
						// Обновляем отображение значения ползунка на вебстраничке.
"						show_slider_contrast.innerHTML = contrast_value;"
"					} else if(type.localeCompare(\"saturation\") == 0){"
						// Принимаем значение ползунка с контролера.
"						var saturation_value = parseInt(obj.value);"
						// Обновляем ползунок.
"						slider_saturation.value = saturation_value;"
						// Обновляем отображение значения ползунка на вебстраничке.
"						show_slider_saturation.innerHTML = saturation_value;"
"					} else if(type.localeCompare(\"ae_level\") == 0){"
						// Принимаем значение ползунка с контролера.
"						var ae_level_value = parseInt(obj.value);"
						// Обновляем ползунок.
"						slider_ae_level.value = ae_level_value;"
						// Обновляем отображение значения ползунка на вебстраничке.
"						show_slider_ae_level.innerHTML = ae_level_value;"
"					} else if(type.localeCompare(\"gainceiling\") == 0){"
						// Принимаем значение ползунка с контролера.
"						var gainceiling_value = parseInt(obj.value);"
						// Обновляем ползунок.
"						slider_gainceiling.value = gainceiling_value;"
						// Обновляем отображение значения ползунка на вебстраничке.
"						show_slider_gainceiling.innerHTML = gainceiling_value;"
"					} else if(type.localeCompare(\"agc_gain\") == 0){"
						// Принимаем значение ползунка с контролера.
"						var agc_gain_value = parseInt(obj.value);"
						// Обновляем ползунок.
"						slider_agc_gain.value = agc_gain_value;"
						// Обновляем отображение значения ползунка на вебстраничке.
"						show_slider_agc_gain.innerHTML = agc_gain_value;"
"					} else if(type.localeCompare(\"aec_value\") == 0){"
						// Принимаем значение ползунка с контролера.
"						var aec_value_value = parseInt(obj.value);"
						// Обновляем ползунок.
"						slider_aec_value.value = aec_value_value;"
						// Обновляем отображение значения ползунка на вебстраничке.
"						show_slider_aec_value.innerHTML = aec_value_value;"
"					} "
					// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					// Обновляем кнопки подключения/отключения параметров ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
"					else if(type.localeCompare(\"whitebal\") == 0){"
"						btn_whitebal = parseInt(obj.value);"
"						if(btn_whitebal == 0){label_whitebal.innerHTML = 'OFF';} else {label_whitebal.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"awb_gain\") == 0){"
"						btn_awb_gain = parseInt(obj.value);"
"						if(btn_awb_gain == 0){label_awb_gain.innerHTML = 'OFF';} else {label_awb_gain.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"exposure_ctrl\") == 0){"
"						btn_exposure_ctrl = parseInt(obj.value);"
"						if(btn_exposure_ctrl == 0){label_exposure_ctrl.innerHTML = 'OFF';} else {label_exposure_ctrl.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"gain_ctrl\") == 0){"
"						btn_gain_ctrl = parseInt(obj.value);"
"						if(btn_gain_ctrl == 0){label_gain_ctrl.innerHTML = 'OFF';} else {label_gain_ctrl.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"wpc\") == 0){"
"						btn_wpc = parseInt(obj.value);"
"						if(btn_wpc == 0){label_wpc.innerHTML = 'OFF';} else {label_wpc.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"raw_gma\") == 0){"
"						btn_raw_gma = parseInt(obj.value);"
"						if(btn_raw_gma == 0){label_raw_gma.innerHTML = 'OFF';} else {label_raw_gma.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"ae_lenc\") == 0){"
"						btn_ae_lenc = parseInt(obj.value);"
"						if(btn_ae_lenc == 0){label_ae_lenc.innerHTML = 'OFF';} else {label_ae_lenc.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"dcw\") == 0){"
"						btn_dcw = parseInt(obj.value);"
"						if(btn_dcw == 0){label_dcw.innerHTML = 'OFF';} else {label_dcw.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"colorbar\") == 0){"
"						btn_colorbar = parseInt(obj.value);"
"						if(btn_colorbar == 0){label_colorbar.innerHTML = 'OFF';} else {label_colorbar.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"hmirror\") == 0){"
"						btn_hmirror = parseInt(obj.value);"
"						if(btn_hmirror == 0){label_hmirror.innerHTML = 'OFF';} else {label_hmirror.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"vflip\") == 0){"
"						btn_vflip = parseInt(obj.value);"
"						if(btn_vflip == 0){label_vflip.innerHTML = 'OFF';} else {label_vflip.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"bpc\") == 0){"
"						btn_bpc = parseInt(obj.value);"
"						if(btn_bpc == 0){label_bpc.innerHTML = 'OFF';} else {label_bpc.innerHTML = 'ON';}"
"					} else if(type.localeCompare(\"aec2\") == 0){"
"						btn_aec2 = parseInt(obj.value);"
"						if(btn_aec2 == 0){label_aec2.innerHTML = 'OFF';} else {label_aec2.innerHTML = 'ON';}"
"					}"
					// '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
"				} catch (e) { console.error(\"Received data is neither Blob nor valid JSON:\", event.data); }"
"			}"
"		}"

		/** Первым делом при подключении клиента к серверу должен быть инициализирован вебсокет. ---------------------**/
"		window.onload = function(event) { init(); }"
"	</script>" // ********************************************************************************************************************
"</html>";

return html;
}