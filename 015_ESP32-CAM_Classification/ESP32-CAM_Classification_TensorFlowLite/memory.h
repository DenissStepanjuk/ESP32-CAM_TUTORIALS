//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ESP32-CAM MicroSD Card

// Библиотека предоставляет функционал для работы с SD-картами памяти.
#include "SD_MMC.h"
// Библиотека EEPROM дл работы с энергонезависимой памятью.
#include "EEPROM.h"

// Выделяем 1 байт памяти EEPROM куда будет записываться кол-во изображений сохранёных на MicroSD карте.
#define EEPROM_SIZE 1
// Счётчик кол-ва изображений.
unsigned int pictureCount = 0;

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
  }}

// Функция для записи изображения на MicroSD карту.
void saveToMicroSD(uint8_t *jpg_buf, size_t jpg_buf_len) {

  // Увеличим счётчик изображений на MicroSD карте на еденицу.
  pictureCount = EEPROM.read(0) + 1;
 
  // Сгенерируем путь по которому будет сохранено изображение на MicroSD карте.
  String path_microSD = "/image" + String(pictureCount) + ".jpg";
  Serial.printf("Picture file name path_microSD---: %s\n", path_microSD);

  // Создадим файл по сгенерированому пути, откроем его в режиме записи и запишем файл.
  File img_microSD = SD_MMC.open(path_microSD, FILE_WRITE);
  if (!img_microSD) {
    Serial.println("Failed to open file in write mode");
  }
  else {
    img_microSD.write(jpg_buf, jpg_buf_len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path_microSD);
  }
  // Закроем файл для записи.
  img_microSD.close();
 
  // Обновим кол-во изображений сохранёных на MicroSD карте в памяти EEPROM.
  EEPROM.write(0, pictureCount);
  EEPROM.commit();
}
