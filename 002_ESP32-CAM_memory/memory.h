//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ESP32-CAM SPIFFS

/** SPIFFS — это файловая система, предназначенная для флэш-устройств SPI NOR на встроенных объектах. 
Она поддерживает выравнивание износа, проверку целостности файловой системы и многое другое. **/
#include <SPIFFS.h>
#include <FS.h> // file system wrapper

/** Функция инициализации для файловой системы SPIFFS. Инициализация должна быть проведена в void setup() {}.**/
void SPIFFS_init() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }
}

// Путь для сохранения изображения в памяти SPIFFS
#define path_SPIFFS "/photoSPIFFS.jpg"

// Фунция проверяет успешно ли записался файл.
bool checkPhoto( fs::FS &fs ) {
  File f_pic = fs.open(path_SPIFFS);
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}

// Функция для записи изображения в память SPIFFS.
void saveToSpiffs(uint8_t *jpg_buf, size_t jpg_buf_len) {
  // Переменая для индикации успешной записи файла.
  bool ok = 0;
  do {
    /** Чтобы записать файл в файловую систему SPIFFS ESP32 требуется открыть его в режиме записи. 
    Для этого вызовем метод SPIFFS.open(), передадим в качестве первого аргумента путь к файлу, 
    а в качестве второго — режим открытия. **/
    File img_SPIFFS = SPIFFS.open(path_SPIFFS, FILE_WRITE);

    // Запишем данные в память.
    if (!img_SPIFFS) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      // Метод file.write сохраняет фото в FILE_PHOTO (image/jpg).
      img_SPIFFS.write(jpg_buf, jpg_buf_len); // payload (image), payload length
      // Выводим сообщение об успешой записи избражения в память.
      Serial.println("The picture has been saved in " + String(path_SPIFFS));
      // Выводим размер записаного файла.
      Serial.println(" - Size: " + String(img_SPIFFS.size()) + " bytes");
    }

    // Закрываем файл.
    img_SPIFFS.close();
    // Проверка успешно ли записался файл.
    ok = checkPhoto(SPIFFS);
  } while ( !ok );
}

// Функция для чтения изображения из памяти SPIFFS.
void readFromSpiffs(String path, uint8_t * &buf, unsigned int &pic_sz) {
  // Открыть 
  File f_pic = SPIFFS.open(path, FILE_READ);

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

// Функция возвращает список изображений на MicroSD карте памяти.
void ListFromMicroSD(StaticJsonDocument<200>* doc) {
  // Открываем корневую директорию
  File root = SD_MMC.open("/");
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }

  // Очищаем JSON документ.
  doc->clear();
  // Указываем тип данных которые будем передавать в JSON строке.
  (*doc)["type"] = "img_list";

  // Начинаем итерироваться по файлам внутри директории.
  File img = root.openNextFile();
  int photo_count = 1;
  // Проходим через все файлы в директории и выводим их имена.
  while (img) {
    // Открываем первый и последующие файлы.
    img = root.openNextFile();
    if(img){
      // Записываем имя файла в JSON документ.
      (*doc)[String(photo_count)] = String(img.name());
      // Обновляем счётчик имён изображений записаных в JSON строку.
      photo_count += 1;
    }
  }
  // Указываем кол-во имён изображений записаных в JSON строку.
  (*doc)["img_count"] = photo_count - 1;
  //img.close();
  root.close();
}