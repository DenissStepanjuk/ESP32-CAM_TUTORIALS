/** Функция берёт цветное изображение в формате JPEG  и преобразует его в чёрно-белое изображение.
      bool convert2jpeg - Если true, то возвращает изображение в формате JPEG. Если false, то возвращает матрицу.
      uint8_t * buf_jpeg - Буфер хранящий исходное изображение в формате JPEG.
      size_t len_jpeg - Размер исходного изображения.
      uint8_t * &buf_gray_out - Буфер для записи в него обработоного изображения.
      size_t &len_gray_out - Размер обработоного изображения.
      int width - Ширина исходного изображения.
      int height - Высота исходного изображения.**/
void jpeg2gray(bool convert2jpeg, uint8_t * buf_jpeg, size_t len_jpeg, uint8_t * &buf_gray_out, size_t &len_gray_out, int width, int height) {
  
  // Размер буффера для хранения исходного изображения преобразованого в цветовую матрицу RGB.
  size_t  len_rgb = width * height * 3;
  // Инициализировать буфер под хранение исходного изображения преобразованого в цветовую матрицу RGB.
  uint8_t *buf_rgb = (uint8_t *)ps_malloc(len_rgb);

  // Конвертируем исходное изображение в цветовую матрицу RGB.
  bool okey = fmt2rgb888(buf_jpeg, len_jpeg, PIXFORMAT_JPEG, buf_rgb);

  // Если удалось представить изображение в виде цветовой матрицы, чтоб каждому пикселю соответствовало 3 значения: R, G, B.
  if (okey) {
    // Размер буффера для матрицы содержащей чёрно-белое изображение.
    size_t len_gray = width * height;
    // Инициализировать буфер под хранение матрицы содержащей чёрно-белое изображение.
    uint8_t *buf_gray = (uint8_t *)ps_malloc(len_gray);

    // Переменная для итерации по буферу в который записывается чёрно-белое изображение.
    int iter_gray = 0;
    // Проходим через все пиксели цветовой матрицы содержащей исходное изображение чтобы 
    // получить матрицу содержащую чёрно-белое изображение.
    for(int i = 0; i < len_rgb; i += 3){
      // Пиксель чёрно-белого изображния состоит из одной компоненты, а пиксель цветного изображения состоит из 3 компонент RGB.
      // Чтобы получить чёрно-белый пиксель требуется сложить 3 компоненты цветного пикселя по следующей формуле:
      // чёрно-белый пиксель = 0.114*синяя компонента + 0.587*зелёная компонента + 0.299*красная компонента.
      buf_gray[iter_gray] = (byte)(0.114*buf_rgb[i] + 0.587*buf_rgb[i+1] + 0.299*buf_rgb[i+2]);
      // Сделать шаг итерации по буферу.
      iter_gray +=1;
    }
    // Освобождаем буфер хранящий исходное изображение преобразованое в цветовую матрицу RGB.
    if (buf_rgb) free(buf_rgb);
    // Если требуется конвертировать матрицу хранящую чёрно-белое изображение в формат JPEG.
    if(convert2jpeg){
      // Конвертируем чёрно-белое изображения в сжатое изображение в формате JPEG.
      bool ok2 = fmt2jpg(buf_gray, len_gray, width, height, PIXFORMAT_GRAYSCALE, 80, &buf_gray_out, &len_gray_out);
      // Освобождаем буфер хранящий чёрно-белую матрицу.
      if (buf_gray) free(buf_gray);
    } else{
      // Возвращаем буфер хранящий чёрно-белую матрицу.
      buf_gray_out = buf_gray;
      // Возвращаем размер буфера хранящего чёрно-белую матрицу.
      len_gray_out = len_gray;
    }
    
  }
}
  



/** Функция понижает разрешение чёрно-белого изображения.
      uint8_t * buf_gray - Буфер хранящий исходное чёрно-белое изображение в формате PIXFORMAT_GRAYSCALE.
      size_t len_gray - Размер исходного изображения.
      uint8_t * &buf_out - Буфер для записи в него обработоного изображения.
      size_t &len_out - Размер обработоного изображения.
      int width_in - Ширина исходного изображения.
      int height_in - Высота исходного изображения.
      int width_out - Ширина до которой надо понизить исходное изображение.
      int height_out - Высота до которой надо понизить исходное изображение.**/
void downsampleGrayMatrix(uint8_t * buf_gray, size_t len_gray, uint8_t * &buf_out, size_t &len_out, int width_in, int height_in, int width_out, int height_out) {
  // Коэффициент масштабирования по оси Y.
  float Y_scale = (float)height_in / height_out;
  // Коэффициент масштабирования по оси X.
  float X_scale = (float)width_in / width_out;
  // Размер буффера для чёрно-белого изображения с пониженым разрешением.
  len_out = width_out * height_out;
  // Выделить память под буфер для хранение чёрно-белого изображения с пониженым разрешением.
  buf_out = (uint8_t *)ps_malloc(len_out);

  // Пройти через буфер для изображения с пониженым разрешением по оси Y.
  for (int y = 0; y < height_out; y++) {
    // Пройти через буфер для изображения с пониженым разрешением по оси X.
    for (int x = 0; x < width_out; x++) {
      // Найти координаты писеля на исходном изображении соответствующие 
      // текущему пикселю на изображении с пониженым разрешением.
      int Y_in = y * Y_scale;
      int X_in = x * X_scale;
      //uint8_t pixel_in = buf_gray[Y_in * width_in + X_in];
      // Присвоить пикселю изображения с пониженым разрешением соответствующие значение пикселя с исходного изображения.
      buf_out[y * width_out + x] = buf_gray[Y_in * width_in + X_in];
    }
  }
}


/** Функция понижает разрешение цветного изображения.
      uint8_t * buf_in - Буфер хранящий исходное изображение в формате RGB888.
      size_t len_in - Размер исходного изображения.
      uint8_t * &buf_out - Буфер для записи в него обработоного изображения.
      size_t &len_out - Размер обработоного изображения.
      int width_in - Ширина исходного изображения.
      int height_in - Высота исходного изображения.
      int width_out - Ширина до которой надо понизить исходное изображение.
      int height_out - Высота до которой надо понизить исходное изображение.**/
void downsampleRGBMatrix(uint8_t * buf_in, size_t len_in, uint8_t * &buf_out, size_t &len_out, int width_in, int height_in, int width_out, int height_out) {
  // Коэффициент масштабирования по оси Y.
  float Y_scale = (float)height_in / height_out;
  // Коэффициент масштабирования по оси X.
  float X_scale = (float)width_in / width_out;
  // Размер буффера для чёрно-белого изображения с пониженым разрешением.
  len_out = width_out * height_out * 3;
  // Выделить память под буфер для хранение чёрно-белого изображения с пониженым разрешением.
  buf_out = (uint8_t *)ps_malloc(len_out);

  // Пройти через буфер для изображения с пониженым разрешением по оси Y.
  for (int y = 0; y < height_out; y++) {
    // Пройти через буфер для изображения с пониженым разрешением по оси X.
    for (int x = 0; x < width_out; x++) {
      // Найти координаты писеля на исходном изображении соответствующие 
      // текущему пикселю на изображении с пониженым разрешением.
      int Y_in = y * Y_scale;
      int X_in = x * X_scale;
      //uint8_t pixel_in = buf_gray[Y_in * width_in + X_in];
      // Присвоить пикселю изображения с пониженым разрешением соответствующие значение пикселя с исходного изображения.
      buf_out[((y * width_out + x) * 3)] = buf_in[((Y_in * width_in + X_in)*3)];
      buf_out[((y * width_out + x) * 3) + 1] = buf_in[((Y_in * width_in + X_in)*3) + 1];
      buf_out[((y * width_out + x) * 3) + 2] = buf_in[((Y_in * width_in + X_in)*3) + 2];
    }
  }
}