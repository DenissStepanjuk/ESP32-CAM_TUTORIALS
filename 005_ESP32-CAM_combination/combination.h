// Библиотека предоставляет функционал для работы с SD-картами памяти.
#include "SD_MMC.h"
#include <FS.h> // file system wrapper

/** Функция инициализации для MicroSD Card. Инициализация должна быть проведена в void setup() {}.**/
void SDCard_init() {
  Serial.println("Mounting MicroSD Card");
  if (!SD_MMC.begin()) {
    Serial.println("MicroSD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No MicroSD Card found");
    return;
  }
}

// Функция для чтения изображения с MicroSD карты.
void readFromMicroSD(String path, uint8_t * &buf, unsigned int &pic_sz) {
  //Открыть 
  File f_pic = SD_MMC.open(path, FILE_READ);
  if (!f_pic) {
    Serial.println("Failed to open file");
    return;
  }
  // Получаем размер файла
  pic_sz = f_pic.size();
  // Выделяем память для буфера
  buf = (uint8_t *)malloc(pic_sz);
  // Чтение данных из файла в буфер
  f_pic.read(buf, pic_sz);
  f_pic.close();
}


/** Функция для объеденения двух изображений.
uint8_t * &buf_full_jpeg - Буфер хранящий исходное изображение в формате JPEG. В исходное изображение будет осуществлятся вставка.
size_t &len_full_jpeg - Размер исходного изображения.
uint8_t * buf_logo_jpeg - Буфер хранящий (логотип) изображение в формате JPEG, которое будет вставляться в исходное изображение.
size_t len_logo_jpeg - Размер (логотипа) изображения для вставки в исходное изображение.
int x - координата левого верхнего угла для вставки.
int y - координата левого верхнего угла для вставки.
int width_img - ширина исходного изображения.
int height_img - высота исходного изображения.
int width - ширина (логотипа) изображения для вставки в исходное изображение.
int height - высота (логотипа) изображения для вставки в исходное изображение.**/
void insertLogo(uint8_t * &buf_full_jpeg, size_t &len_full_jpeg, uint8_t * buf_logo_jpeg, size_t len_logo_jpeg, int x, int y, int width_img, int height_img, int width, int height) {
  
  // Коректируем некоторые аргументы чтоб найти заданый участок изображения.
  int y_real = y - 1;
  int x_real = (x - 1) * 3;
  int width_real = width * 3;

  // Размер буффера для хранения исходного изображения преобразованого в цветовую матрицу RGB.
  size_t len_full_rgb = width_img * height_img * 3;
  // Инициализировать буфер под хранение исходного изображения преобразованого в цветовую матрицу RGB.
  uint8_t *buf_full_rgb = (uint8_t *)ps_malloc(len_full_rgb);

  // Размер буффера для хранения логотипа преобразованого в цветовую матрицу RGB.
  size_t len_logo_rgb = width * height * 3;;
  // Инициализировать буфер под хранение логотипа преобразованого в цветовую матрицу RGB.
  uint8_t *buf_logo_rgb = (uint8_t *)malloc(len_logo_rgb);

  // Конвертируем исходное изображение в формате JPEG в цветовую матрицу RGB.
  bool okey_full = fmt2rgb888(buf_full_jpeg, len_full_jpeg, PIXFORMAT_JPEG, buf_full_rgb);
  // Освобождаем буфер.
  if (buf_full_jpeg) free(buf_full_jpeg);
  // Конвертируем логотип в формате JPEG в цветовую матрицу RGB.
  bool okey_logo = fmt2rgb888(buf_logo_jpeg, len_logo_jpeg, PIXFORMAT_JPEG, buf_logo_rgb);
  // Если удалось представить изображение в виде цветовой матрицы, чтоб каждому пикселю соответствовало 3 значения: R, G, B.
  if (okey_full && okey_logo) {

    // Переменная для итерации по буферу в который записывается изображение.
    int iter_x = 0;
    // Если заданый участок не выходит за границы изображения.
    if (x + width < width_img + 1 && y + height < height_img + 1) {
      // Пройти через все пиксели на заданом участке изображения.
      for(int i = x_real; i < x_real + width_real; i += 3){
        // Переменная для итерации по буферу в который записывается изображение.
        int iter_y = 0;
        for(int j = y_real; j < y_real + height; j += 1){

          // Переносим пиксели из буфера содержащего логотип в буфер содержащий исходное изображение.
          // Тем замым доболяем логотип на фото.
          buf_full_rgb[i+width_img*3*j+2] = buf_logo_rgb[(iter_x) + width*3*(iter_y) + 2];
          buf_full_rgb[i+width_img*3*j+1] = buf_logo_rgb[(iter_x) + width*3*(iter_y) + 1];
          buf_full_rgb[i+width_img*3*j] = buf_logo_rgb[(iter_x) + width*3*(iter_y)];

          // Делаем шаг итерации по вертикали.
          iter_y += 1;
        }
        // Делаем шаг итерации по горизонтали.
        iter_x += 3;
      }
      // Освобождаем буфер.
      if (buf_logo_rgb) free(buf_logo_rgb);
      // Конвертируем цветовую матрицу RGB комбинированого изображения в сжатое изображение в формате JPEG.
      bool ok2 = fmt2jpg(buf_full_rgb, len_full_rgb, width_img, height_img, PIXFORMAT_RGB888, 80, &buf_full_jpeg, &len_full_jpeg);
      // Освобождаем буфер.
      if (buf_full_rgb) free(buf_full_rgb);

    } else {
      Serial.println("Wrong width or height!");
    }
  }
}