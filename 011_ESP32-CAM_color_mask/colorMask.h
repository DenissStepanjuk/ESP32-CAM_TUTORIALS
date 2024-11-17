/** Функция берёт цветное изображение в формате JPEG  и преобразует его в HSV изображение.
      bool convert2jpeg - Если true, то возвращает изображение в формате JPEG. Если false, то возвращает матрицу.
      uint8_t * buf_jpeg - Буфер хранящий исходное изображение в формате JPEG.
      size_t len_jpeg - Размер исходного изображения.
      uint8_t * &buf_hsv_out - Буфер для записи в него hsv изображения.
      size_t &len_hsv_out - Размер hsv изображения.
      int width - Ширина исходного изображения.
      int height - Высота исходного изображения.**/
void jpeg2HSV(bool convert2jpeg, uint8_t * buf_jpeg, size_t len_jpeg, uint8_t * &buf_hsv_out, size_t &len_hsv_out, int width, int height) {
  
  // Размер буффера для хранения исходного изображения преобразованого в цветовую матрицу RGB.
  size_t  len_rgb = width * height * 3;
  // Инициализировать буфер под хранение исходного изображения преобразованого в цветовую матрицу RGB.
  uint8_t *buf_rgb = (uint8_t *)ps_malloc(len_rgb);

  // Конвертируем исходное изображение в цветовую матрицу RGB.
  bool okey = fmt2rgb888(buf_jpeg, len_jpeg, PIXFORMAT_JPEG, buf_rgb);

  // Если удалось представить изображение в виде цветовой матрицы, чтоб каждому пикселю соответствовало 3 значения: R, G, B.
  if (okey) {
    // Размер буффера для матрицы содержащей HSV представление изображения.
    size_t len_hsv = width * height * 3;
    // Инициализировать буфер под хранение матрицы содержащей HSV представление изображения.
    uint8_t *buf_hsv = (uint8_t *)ps_malloc(len_hsv);


    // Проходим через все RGB пиксели цветовой матрицы содержащей исходное изображение чтобы 
    // получить матрицу содержащую HSV представление изображения.
    for(int i = 0; i < len_rgb; i += 3){

      // Синяя компонента от RGB пикселя.
      double blue = buf_rgb[i] / 255.0;
      // Зелёная компонента от RGB пикселя.
      double green = buf_rgb[i+1] / 255.0;
      // Красная компонента от RGB пикселя.
      double red = buf_rgb[i+2] / 255.0;

      // Находим минимальное значение среди r, g и b.
      double min = red < green ? red : green;
      min = min < blue ? min : blue;

      // Находим максимальное значение среди r, g и b.
      double max = red > green ? red : green;
      max = max > blue ? max : blue;

      // Разница между максимальным и минимальным значением цветов.
      double delta = max - min;

      // Яркость (V) равна максимальному значению r, g или b.
      double value = max * 255.0;
      // Оттенок (H).
      double hue;
      // Насыщенность (S).
      double saturation;

      // Если разница очень маленькая (почти 0), то цвет нейтральный (оттенок и насыщенность не определяются).
      if (delta < 0.00001) {
        saturation = 0;  // насыщенность равна 0
        hue = 0;  // оттенок не определен (условно ставим 0)
        continue; // перейти к следующему пикселю.
      }


      // Если максимальное значение больше 0, то вычисляем насыщенность (S).
      if (max > 0.0) {
          saturation = (delta / max) * 255.0;  // насыщенность: разница делится на максимальное значение
      } else {
          // Если максимальное значение равно 0, то цвет чёрный (r = g = b = 0), 
          // и насыщенность также равна 0, оттенок не определён.
          saturation = 0.0;
          hue = NAN;  // оттенок неопределён
          continue; // перейти к следующему пикселю.
      }

      // Определяем оттенок (H) в зависимости от того, какой цвет (r, g или b) является максимальным.
      if (red >= max)
          hue = (green - blue) / delta;  // оттенок между жёлтым и пурпурным
      else if (green >= max)
          hue = 2.0 + (blue - red) / delta;  // оттенок между цианом и жёлтым
      else
          hue = 4.0 + (red - green) / delta;  // оттенок между пурпурным и цианом

      // Преобразуем оттенок из диапазона [0, 6] в градусы [0, 360].
      hue *= 60.0;  // перевод в градусы

      // Если оттенок оказался отрицательным, добавляем 360, чтобы получить положительное значение.
      if (hue < 0.0)
          hue += 360.0;

      // Записать представление пикселя в HSV формате в буфер.
      buf_hsv[i] = (int)((hue / 360.0) * 255);
      buf_hsv[i+1] = (int)saturation;
      buf_hsv[i+2] = (int)value;

    }

    
    // Освобождаем буфер хранящий исходное изображение преобразованое в цветовую матрицу RGB.
    if (buf_rgb) free(buf_rgb);
    // Если требуется конвертировать матрицу хранящую чёрно-белое изображение в формат JPEG.
    if(convert2jpeg){
      // Конвертируем HSV матрицу в RAW-формат, это формат для хранения необработанных данных.
      bool ok2 = fmt2jpg(buf_hsv, len_hsv, width, height, PIXFORMAT_JPEG, 80, &buf_hsv_out, &len_hsv_out);
      //bool ok2 = fmt2jpg(buf_hsv, len_hsv, width, height, PIXFORMAT_RAW, 80, &buf_hsv_out, &len_hsv_out);
      // Освобождаем буфер хранящий чёрно-белую матрицу.
      if (buf_hsv) free(buf_hsv);
    } else{
      // Возвращаем буфер хранящий чёрно-белую матрицу.
      buf_hsv_out = buf_hsv;
      // Возвращаем размер буфера хранящего чёрно-белую матрицу.
      len_hsv_out = len_hsv;
    }
  }
}

  


/** Функция проверяет каждый пиксель изображения, находится ли его представление в HSV формате в указаных 
на вебстраничке пределах для параметров Яркость (V), Оттенок (H), Насыщенность (S), если нет, то пиксель
делаем черным, если да, то оставляем исхоный цвет для пикселя **/
void applyMask(uint8_t * &buf_jpeg, size_t &len_jpeg, int width_img, int height_img, \
              int hue_min, int hue_max, int sat_min, int sat_max, int val_min, int val_max) {

  // Размер буффера для матрицы содержащей HSV представление изображения.
  size_t len_hsv;
  // Инициализировать буфер под хранение  HSV представление изображения.
  uint8_t *buf_hsv;

  // Размер буффера для исходного изображения в формате RGB888.
  size_t len_rgb = (width_img) * (height_img) * 3;
  // Инициализировать буфер под хранение  исходного изображения в формате RGB888..
  uint8_t *buf_rgb = (uint8_t *)ps_malloc(len_rgb);

  // Преобразуем исходное цветное изображение в HSV изображение.
  jpeg2HSV(false, buf_jpeg, len_jpeg, buf_hsv, len_hsv, width_img, height_img);

  // Конвертируем исходное изображение в цветовую матрицу RGB.
  bool okey = fmt2rgb888(buf_jpeg, len_jpeg, PIXFORMAT_JPEG, buf_rgb);

  // Пройти через все пиксели исходного изображения в формате RGB888.
  for(int i = 0; i < len_rgb; i += 3){
    /** Проверяем пиксель изображения, находится ли его представление в HSV формате в указаных 
        на вебстраничке пределах для параметров Яркость (V), Оттенок (H), Насыщенность (S)**/
    if(hue_min < buf_hsv[i] && hue_max > buf_hsv[i] && sat_min < buf_hsv[i+1] && sat_max > buf_hsv[i+1] && val_min < buf_hsv[i+2] && val_max > buf_hsv[i+2]) {
      //buf_rgb[i] = 0;
      //buf_rgb[i+1] = 0;
      //buf_rgb[i+2] = 0;
      //Serial.println("Blue: " + String(buf_rgb[i]) + " Green: " + String(buf_rgb[i+1]) + " Red: " + String(buf_rgb[i+2]));
      continue;
    } else {
      buf_rgb[i] = 255;
      buf_rgb[i+1] = 255;
      buf_rgb[i+2] = 255;
    }
  }
  // Освобождаем буфер хранящий HSV представление изображения.
  if (buf_hsv) free(buf_hsv);


  // Освобождаем буфер хранящий JPEG чтобы перезаписать в него обработаное изображение.
  if (buf_jpeg) free(buf_jpeg);



  // Конвертируем преобразованное изображение в сжатое изображение в формате JPEG.
  bool ok2 = fmt2jpg(buf_rgb, len_rgb, width_img, height_img, PIXFORMAT_RGB888, 80, &buf_jpeg, &len_jpeg);
  // Освобождаем буфер хранящий исходное изображение в формате RGB888.
  if (buf_rgb) free(buf_rgb);

}

