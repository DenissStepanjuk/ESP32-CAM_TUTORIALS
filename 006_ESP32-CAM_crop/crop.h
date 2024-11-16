/** Функция обрезает изображение.
uint8_t * &buf_full_jpeg - Буфер хранящий исходное изображение в формате JPEG. 
size_t &len_full_jpeg - Размер исходного буфера.
uint8_t * buf_crop_jpeg - Буфер в который будет производится запись обрезаного изображения.
size_t len_crop_jpeg - Размер буфера для обрезаного изображения.
int x - координата левого верхнего угла.
int y - координата левого верхнего угла.
int width_img - ширина исходного изображения.
int height_img - высота исходного изображения.
int width - ширина обрезаного изображения.
int height - высота обрезаного изображения.**/
void cropJPEG(uint8_t * buf_full_jpeg, size_t len_full_jpeg, uint8_t * &buf_crop_jpeg, size_t &len_crop_jpeg, int x, int y, int width_img, int height_img, int width, int height) {
  
  // Коректируем некоторые аргументы чтоб найти заданый участок изображения.
  int y_real = y - 1;
  int x_real = (x - 1) * 3;
  int width_real = width * 3;

  // Инициализировать переменную для хранение размера цветовой матрицы RGB полного изображения.
  size_t len_full_rgb = width_img * height_img * 3;
  // Инициализировать буфер под хранение цветовой матрицы RGB полного изображения.
  uint8_t *buf_full_rgb = (uint8_t *)ps_malloc(len_full_rgb);

  // Инициализировать переменную для хранение размера цветовой матрицы RGB обрезаного изображения.
  size_t len_crop_rgb = width * height * 3;;
  // Инициализировать буфер под хранение цветовой матрицы RGB обрезаного изображения.
  uint8_t *buf_crop_rgb = (uint8_t *)malloc(len_crop_rgb);

  // Конвертируем сжатое в формате JPEG полное изображение в цветовую матрицу RGB.
  bool okey = fmt2rgb888(buf_full_jpeg, len_full_jpeg, PIXFORMAT_JPEG, buf_full_rgb);
  // Если удалось представить изображение в виде цветовой матрицы, чтоб каждому пикселю соответствовало 3 значения: R, G, B.
  if (okey) {

    // Переменная для итерации по буферу в который записывается изображение.
    int iter_x = 0;
    // Если заданый участок не выходит за границы изображения.
    if (x + width < width_img + 1 && y + height < height_img + 1) {
      // Пройти через все пиксели на заданом участке изображения.
      for(int i = x_real; i < x_real + width_real; i += 3){
        // Переменная для итерации по буферу в который записывается изображение.
        int iter_y = 0;
        for(int j = y_real; j < y_real + height; j += 1){

          // Переносим пиксели из буфера содержащего полное изображение в буфер выделеный под обрезаное изображение.
          buf_crop_rgb[(iter_x) + width*3*(iter_y) + 2] = buf_full_rgb[i+width_img*3*j+2];
          buf_crop_rgb[(iter_x) + width*3*(iter_y) + 1] = buf_full_rgb[i+width_img*3*j+1];
          buf_crop_rgb[(iter_x) + width*3*(iter_y)] = buf_full_rgb[i+width_img*3*j];
          // Делаем шаг итерации по вертикали.
          iter_y += 1;
        }
        // Делаем шаг итерации по горизонтали.
        iter_x += 3;
      }
      // Освобождаем буфер.
      if (buf_full_rgb) free(buf_full_rgb);
      // Конвертируем цветовую матрицу RGB обрезаного изображения в сжатое изображение в формате JPEG.
      bool ok2 = fmt2jpg(buf_crop_rgb, len_crop_rgb, width, height, PIXFORMAT_RGB888, 80, &buf_crop_jpeg, &len_crop_jpeg);
      // Освобождаем буфер.
      if (buf_crop_rgb) free(buf_crop_rgb);

    } else {
      Serial.println("Wrong width or height!");
    }
  }
}