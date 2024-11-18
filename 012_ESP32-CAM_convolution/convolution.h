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
  





/** Функция сворачивает входное изображение при помощи ядра, тем самым на выходе получаем преобразованное изображение.
      bool convert2jpeg - Если true, то возвращает изображение в формате JPEG. Если false, то возвращает матрицу.
      uint8_t * buf_gray - Буфер хранящий чёрно-белое изображение.
      size_t len_gray - Размер чёрно-белого изображения.
      uint8_t * &buf_gray_out - Буфер для записи в него обработоного изображения.
      size_t &len_gray_out - Размер обработоного изображения.
      int width - Ширина исходного изображения.
      int height - Высота исходного изображения.
      double kernel[3][3] -  Ядро для свёртки.**/
void apply2DKernel_3x3(bool convert2jpeg, uint8_t * buf_gray, size_t len_gray, uint8_t * &buf_conv_out, size_t &len_conv_out, int width_img, int height_img, double kernel[3][3]) {

  // Размер буффера для свёртки.
  size_t len_conv = (width_img - 2) * (height_img - 2);
  // Инициализировать буфер под хранение свёртки.
  uint8_t *buf_conv = (uint8_t *)ps_malloc(len_conv);

  // Переменная для итерации по буферу в который записывается преобразованное после свёртки изображение.
  int iter_conv = 0;
  // Пройти через все пиксели чёрно-белого изображения по вертикали.
  for(int y = 1; y < height_img - 1; y++){
    // Пройти через все пиксели чёрно-белого изображения по горизонтали.
    for(int x = 1; x < width_img - 1; x++){
      // Проходим через все пиксели чёрно-белого изображения.
      // Останавливаясь на каждом пикселе, находим все окружающие его пиксели чтобы получить квадрат 3х3,
      // используя ядро совершаем операцию свёртки над текущем квадратом 3х3 изображения,
      // полученое значение записываем как пиксель нового преобразованного изображения.
      buf_conv[iter_conv] = (byte) (kernel[0][0] * buf_gray[x + (width_img * (y - 1)) - 1]  + \
                                    kernel[0][1] * buf_gray[x + (width_img * (y - 1))]      + \
                                    kernel[0][2] * buf_gray[x + (width_img * (y - 1)) + 1]  + \
                                    kernel[1][0] * buf_gray[x + (width_img * y) - 1]        + \
                                    kernel[1][1] * buf_gray[x + (width_img * y)]            + \
                                    kernel[1][2] * buf_gray[x + (width_img * y) + 1]        + \
                                    kernel[2][0] * buf_gray[x + (width_img * (y + 1)) - 1]  + \
                                    kernel[2][1] * buf_gray[x + (width_img * (y + 1))]      + \
                                    kernel[2][2] * buf_gray[x + (width_img * (y + 1)) + 1]);
      // Сделать шаг итерации по буферу.
      iter_conv +=1;
    }
  }

  // Если требуется конвертировать матрицу хранящую преобразованное после свёртки изображение в формат JPEG.
  if(convert2jpeg){
    // Конвертируем преобразованное после свёртки изображение в сжатое изображение в формате JPEG.
    bool ok2 = fmt2jpg(buf_conv, len_conv, width_img - 2, height_img - 2, PIXFORMAT_GRAYSCALE, 80, &buf_conv_out, &len_conv_out);
    // Освобождаем буфер хранящий преобразованное после свёртки изображение.
    if (buf_conv) free(buf_conv);
  } else{
    // Возвращаем буфер хранящий матрицу содержащую преобразованное после свёртки изображение.
    buf_conv_out = buf_conv;
    // Возвращаем размер буфера хранящего матрицу содержащую преобразованное после свёртки изображение.
    len_conv_out = len_conv;
  }

}



/** Функция сворачивает входное изображение при помощи двух ядер, тем самым на выходе получаем преобразованное изображение.
      bool convert2jpeg - Если true, то возвращает изображение в формате JPEG. Если false, то возвращает матрицу.
      uint8_t * buf_gray - Буфер хранящий чёрно-белое изображение.
      size_t len_gray - Размер чёрно-белого изображения.
      uint8_t * &buf_gray_out - Буфер для записи в него обработоного изображения.
      size_t &len_gray_out - Размер обработоного изображения.
      int width - Ширина исходного изображения.
      int height - Высота исходного изображения.
      double kernel[3][3] -  Ядро для свёртки.**/
void applyTwo2DKernel_2x2(bool convert2jpeg, uint8_t * buf_gray, size_t len_gray, uint8_t * &buf_conv_out, size_t &len_conv_out, int width_img, int height_img, double kernelHorizontal[2][2], double kernelVertical[2][2]) {

  // Размер буффера для свёртки.
  size_t len_conv = (width_img - 2) * (height_img - 2);
  // Инициализировать буфер под хранение свёртки.
  uint8_t *buf_conv = (uint8_t *)ps_malloc(len_conv);

  // Переменная для итерации по буферу в который записывается преобразованное после свёртки изображение.
  int iter_conv = 0;
  // Пройти через все пиксели чёрно-белого изображения по вертикали.
  for(int y = 1; y < height_img - 1; y++){
    // Пройти через все пиксели чёрно-белого изображения по горизонтали.
    for(int x = 1; x < width_img - 1; x++){
      // Проходим через чёрно-белое изображения окном 2х2.
      // В кажом месте, где остановилось окно, рассчитываем две компоненты для каждого ядра.
      int horizontalComponent = (byte) (kernelHorizontal[0][0] * buf_gray[x + (width_img * (y - 1)) - 1]  + \
                                        kernelHorizontal[0][1] * buf_gray[x + (width_img * (y - 1))]      + \
                                        kernelHorizontal[1][0] * buf_gray[x + (width_img * y) - 1]        + \
                                        kernelHorizontal[1][1] * buf_gray[x + (width_img * y)]);

      int verticalComponent = (byte) (kernelVertical[0][0] * buf_gray[x + (width_img * (y - 1)) - 1]  + \
                                      kernelVertical[0][1] * buf_gray[x + (width_img * (y - 1))]      + \
                                      kernelVertical[1][0] * buf_gray[x + (width_img * y) - 1]        + \
                                      kernelVertical[1][1] * buf_gray[x + (width_img * y)]);
      // Чтобы получить значение для пикселя преобразованного изображения
      // требуется посчитать корень квадратный из суммы квадратов двух найденых выше компонент.
      buf_conv[iter_conv] = (byte) (sqrt(pow(horizontalComponent, 2) + pow(verticalComponent, 2)));
      // Сделать шаг итерации по буферу.
      iter_conv +=1;
    }
  }

  // Если требуется конвертировать матрицу хранящую преобразованное после свёртки изображение в формат JPEG.
  if(convert2jpeg){
    // Конвертируем преобразованное после свёртки изображение в сжатое изображение в формате JPEG.
    bool ok2 = fmt2jpg(buf_conv, len_conv, width_img - 2, height_img - 2, PIXFORMAT_GRAYSCALE, 80, &buf_conv_out, &len_conv_out);
    // Освобождаем буфер хранящий преобразованное после свёртки изображение.
    if (buf_conv) free(buf_conv);
  } else{
    // Возвращаем буфер хранящий матрицу содержащую преобразованное после свёртки изображение.
    buf_conv_out = buf_conv;
    // Возвращаем размер буфера хранящего матрицу содержащую преобразованное после свёртки изображение.
    len_conv_out = len_conv;
  }

}


/** Функция сворачивает входное изображение при помощи ядра, тем самым на выходе получаем преобразованное изображение.
      bool convert2jpeg - Если true, то возвращает изображение в формате JPEG. Если false, то возвращает матрицу.
      uint8_t * buf_gray - Буфер хранящий чёрно-белое изображение.
      size_t len_gray - Размер чёрно-белого изображения.
      uint8_t * &buf_gray_out - Буфер для записи в него обработоного изображения.
      size_t &len_gray_out - Размер обработоного изображения.
      int width - Ширина исходного изображения.
      int height - Высота исходного изображения.
      double kernel[3][3] -  Ядро для свёртки.**/
void applyTwo2DKernel_3x3(bool convert2jpeg, uint8_t * buf_gray, size_t len_gray, uint8_t * &buf_conv_out, size_t &len_conv_out, int width_img, int height_img, double kernelHorizontal[3][3], double kernelVertical[3][3]) {

  // Размер буффера для свёртки.
  size_t len_conv = (width_img - 2) * (height_img - 2);
  // Инициализировать буфер под хранение свёртки.
  uint8_t *buf_conv = (uint8_t *)ps_malloc(len_conv);

  // Переменная для итерации по буферу в который записывается преобразованное после свёртки изображение.
  int iter_conv = 0;
  // Пройти через все пиксели чёрно-белого изображения по вертикали.
  for(int y = 1; y < height_img - 1; y++){
    // Пройти через все пиксели чёрно-белого изображения по горизонтали.
    for(int x = 1; x < width_img - 1; x++){
      // Проходим через чёрно-белое изображения окном 3х3.
      // В кажом месте, где остановилось окно, рассчитываем две компоненты для каждого ядра.
      int horizontalComponent = (byte) (kernelHorizontal[0][0] * buf_gray[x + (width_img * (y - 1)) - 1]  + \
                                        kernelHorizontal[0][1] * buf_gray[x + (width_img * (y - 1))]      + \
                                        kernelHorizontal[0][2] * buf_gray[x + (width_img * (y - 1)) + 1]  + \
                                        kernelHorizontal[1][0] * buf_gray[x + (width_img * y) - 1]        + \
                                        kernelHorizontal[1][1] * buf_gray[x + (width_img * y)]            + \
                                        kernelHorizontal[1][2] * buf_gray[x + (width_img * y) + 1]        + \
                                        kernelHorizontal[2][0] * buf_gray[x + (width_img * (y + 1)) - 1]  + \
                                        kernelHorizontal[2][1] * buf_gray[x + (width_img * (y + 1))]      + \
                                        kernelHorizontal[2][2] * buf_gray[x + (width_img * (y + 1)) + 1]);

      int verticalComponent = (byte) (kernelVertical[0][0] * buf_gray[x + (width_img * (y - 1)) - 1]  + \
                                      kernelVertical[0][1] * buf_gray[x + (width_img * (y - 1))]      + \
                                      kernelVertical[0][2] * buf_gray[x + (width_img * (y - 1)) + 1]  + \
                                      kernelVertical[1][0] * buf_gray[x + (width_img * y) - 1]        + \
                                      kernelVertical[1][1] * buf_gray[x + (width_img * y)]            + \
                                      kernelVertical[1][2] * buf_gray[x + (width_img * y) + 1]        + \
                                      kernelVertical[2][0] * buf_gray[x + (width_img * (y + 1)) - 1]  + \
                                      kernelVertical[2][1] * buf_gray[x + (width_img * (y + 1))]      + \
                                      kernelVertical[2][2] * buf_gray[x + (width_img * (y + 1)) + 1]);
      // Чтобы получить значение для пикселя преобразованного изображения
      // требуется посчитать корень квадратный из суммы квадратов двух найденых выше компонент.
      buf_conv[iter_conv] = (byte) (sqrt(pow(horizontalComponent, 2) + pow(verticalComponent, 2)));
      // Сделать шаг итерации по буферу.
      iter_conv +=1;
    }
  }

  // Если требуется конвертировать матрицу хранящую преобразованное после свёртки изображение в формат JPEG.
  if(convert2jpeg){
    // Конвертируем преобразованное после свёртки изображение в сжатое изображение в формате JPEG.
    bool ok2 = fmt2jpg(buf_conv, len_conv, width_img - 2, height_img - 2, PIXFORMAT_GRAYSCALE, 80, &buf_conv_out, &len_conv_out);
    // Освобождаем буфер хранящий преобразованное после свёртки изображение.
    if (buf_conv) free(buf_conv);
  } else{
    // Возвращаем буфер хранящий матрицу содержащую преобразованное после свёртки изображение.
    buf_conv_out = buf_conv;
    // Возвращаем размер буфера хранящего матрицу содержащую преобразованное после свёртки изображение.
    len_conv_out = len_conv;
  }

}



