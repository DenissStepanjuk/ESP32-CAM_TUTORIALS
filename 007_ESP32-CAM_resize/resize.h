/** Функция понижает разрешение цветного изображения.
      uint8_t * buf_in - Буфер хранящий исходное изображение в формате RGB888.
      size_t len_in - Размер исходного изображения.
      uint8_t * &buf_out - Буфер для записи в него обработоного изображения.
      size_t &len_out - Размер обработоного изображения.
      int width_in - Ширина исходного изображения.
      int height_in - Высота исходного изображения.
      int width_out - Ширина до которой надо привести исходное изображение.
      int height_out - Высота до которой надо привести исходное изображение.**/
void resizeRGBMatrix(uint8_t * buf_in, size_t len_in, uint8_t * &buf_out, size_t &len_out, int width_in, int height_in, int width_out, int height_out) {
  // Коэффициент масштабирования по оси Y.
  float Y_scale = (float)height_in / height_out;
  // Коэффициент масштабирования по оси X.
  float X_scale = (float)width_in / width_out;
  // Размер буффера для нового изображения.
  len_out = width_out * height_out * 3;
  // Выделить память под буфер для хранение нового изображения.
  buf_out = (uint8_t *)ps_malloc(len_out);

  // Пройти через буфер для нового изображения по оси Y.
  for (int y = 0; y < height_out; y++) {
    // Пройти через буфер для нового изображения по оси X.
    for (int x = 0; x < width_out; x++) {
      // Найти координаты писеля на исходном изображении соответствующие 
      // текущему пикселю на новом изображении.
      int Y_in = y * Y_scale;
      int X_in = x * X_scale;
      // Присвоить пикселю нового изображения соответствующие значение пикселя с исходного изображения.
      buf_out[((y * width_out + x) * 3)] = buf_in[((Y_in * width_in + X_in)*3)];
      buf_out[((y * width_out + x) * 3) + 1] = buf_in[((Y_in * width_in + X_in)*3) + 1];
      buf_out[((y * width_out + x) * 3) + 2] = buf_in[((Y_in * width_in + X_in)*3) + 2];
    }
  }
}