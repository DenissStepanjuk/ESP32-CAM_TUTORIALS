#ifndef TENSORFLOW_LITE_CONFIG
#define TENSORFLOW_LITE_CONFIG

// Keeping these as constant expressions allow us to allocate fixed-sized arrays
// on the stack for our working memory.

// All of these values are derived from the values used during model training,
// if you change your model you'll need to update these constants.

// Ширина изображения подаваемого на вход модели.
// (Кол-во столбцов входной матрицы)
//constexpr int kNumCols = 28;
constexpr int kNumCols = 32;
// Высота изображения подаваемого на вход модели.
// (Кол-во строк входной матрицы)
//constexpr int kNumRows = 28;
constexpr int kNumRows = 32;

// Кол-во каналов изображения подаваемого на вход модели.
// 1 канал - чёрно-белое изображение.
// 3 канала - цветное изображение.
// (Глубина входной матрицы)
//constexpr int kNumChannels = 1;
constexpr int kNumChannels = 3;
// Кол-во элементов входной матрицы.
constexpr int kMaxImageSize = kNumCols * kNumRows * kNumChannels;

// Кол-во выходов модели, число категорий, которые модель может классифицировать.
constexpr int kCategoryCount = 3;
constexpr int kPersonIndex = 1;
constexpr int kNotAPersonIndex = 0;
// Массив для наименований категорий, которые модель может классифицировать.
//extern const char* kCategoryLabels[kCategoryCount];

// Наименования категорий, которые модель может классифицировать.
const char* kCategoryLabels[kCategoryCount] = {"Matrjona", "Mouse", "Spanner"};


#endif  // TENSORFLOW_LITE_CONFIG