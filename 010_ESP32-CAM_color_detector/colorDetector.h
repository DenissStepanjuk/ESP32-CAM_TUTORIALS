#include <cmath>

// Структура для хранения названий цветов и их RGB значений
struct Color {
  const char* name;
  int r, g, b;
};

// Массив известных цветов и их RGB значений.
Color knownColors[] = {
  {"Red", 255, 116, 132},
  {"Yellow", 219, 231, 141},
  {"Green", 123, 225, 150},
  {"Blue", 120, 193, 222},
  {"Black", 50, 50, 50},
  {"White", 222, 215, 218},
  {"Purple", 255, 150, 183},
  {"Orange", 255, 173, 145}
};

// Количество известных цветов.
const int numColors = sizeof(knownColors) / sizeof(knownColors[0]);

/** Функция для вычисления евклидова расстояния между двумя цветами (известным и тем который надо определить):
     - Для каждой компоненты цвета RGB расчитать разницу между известным цветом и тем который надо определить.
     - Разицу для каждой компоненты RGB возвести в квадрат.
     - Посчитать сумму квадратов всех компонент.
     - Получить корень квадратный от суммы квадратов. **/
float colorDifference(int r1, int g1, int b1, int r2, int g2, int b2) {
  return sqrt(pow(r1 - r2, 2) + pow(g1 - g2, 2) + pow(b1 - b2, 2));
}

// Функция для определения ближайшего цвета.
const char* getColorName(int r, int g, int b) {
  // Переменная будет хранить минимльное евклидово расстояние (инициализируем большим числом).
  float minDifference = 1000.0;
  // Переменная будет хранить ближайший цвет.
  const char* closestColor = "Unknown";

  // Проходим по всем известным цветам и находим минимальное расстояние.
  for (int i = 0; i < numColors; i++) {
    float diff = colorDifference(r, g, b, knownColors[i].r, knownColors[i].g, knownColors[i].b);
    // Если евклидово расстояние для текущего цвета наименьшее.
    if (diff < minDifference) {
      // Обновляем минимльное евклидово расстояние для дальнейшего сравнения.
      minDifference = diff;
      // Соханяем текущий цвет как ближайший.
      closestColor = knownColors[i].name;
    }
  }

  return closestColor;
}


// Функция для определеия цвета на заданом участке изображения.
String detectColor(uint8_t * &buf, int x, int y, int width_img, int height_img, int width_sensor, int height_sensor) {
  
  // Коректируем некоторые аргументы чтоб наайти заданый участок изображения.
  int y_real = y - 1;
  int x_real = (x - 1) * 3;
  int width_sensor_real = width_sensor * 3;

  // Инициализируем переменные для расчёта среднего значения для каждой компоненты RGB на заданом участке изображения.
  double red_avg = 0;
  double green_avg = 0;
  double blue_avg = 0;
  // Рассчитываем кол-во пикселей в изображении.
  int pixels_amount = width_sensor * height_sensor;
  // Переменная будет хранить ближайший цвет.
  const char* closest_сolor;

  // Если заданый участок не выходит за границы изображения.
  if (x + width_sensor < width_img + 1 && y + height_sensor < height_img + 1) {
    // Пройти через все пиксели на заданом участке изображения.
    for(int i = x_real; i < x_real + width_sensor_real; i += 3){
      for(int j = y_real; j < y_real + height_sensor; j += 1){
        // Посчитать сумму всех компонент пикселя для каждого цвета отдельно.
        red_avg += buf[i+1920*j+2];
        green_avg += buf[i+1920*j+1];
        blue_avg += buf[i+1920*j];
        // Закрасить заданый участок изображения чтоб человеку было его видно.
        if(i%2 == 0){
          buf[i+1920*j+2] /= 2;
        } else {
          buf[i+1920*j+1] = 255;
        }
      }     
    }
    // Посчитать среднее арифметическое для всех компонент пикселя.
    red_avg /= pixels_amount;
    green_avg /= pixels_amount;
    blue_avg /= pixels_amount;

    // Определяем ближайший цвет
    closest_сolor = getColorName((int) red_avg, (int) green_avg, (int) blue_avg);

    Serial.println("red_avg: " +  String(red_avg));
    Serial.println("green_avg: " +  String(green_avg));
    Serial.println("blue_avg: " +  String(blue_avg));
  } else {
    Serial.println("Wrong width or height!");
  }
  return String(closest_сolor);
}