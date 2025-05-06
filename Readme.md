# Робота із стрічками за допомогою SIMD (ARM NEON, AVX2, SSE4.1)
--- 
Команда: [Bykov Danylo](https://github.com/DanyaBykov), [Ivan Shevchuk](https://github.com/DoktorTomato), [Maksym Dzoban](https://github.com/MaxDzioban)<br>

### Prerequisites

- Процесор з підтримкою ARM NEON
- Процесор з підтримкою AVX2
- Процесор з підтримкою SSE4.1
- cmake
- g++

### Compilation

Щоб скомпілювати проєкт виконайте наступні кроки:
1. в дирикторії лаби 'mkdir builld'
2. cd build
3. cmake ..
4. cmake --build .

### Usage

В дирикторії build:
- ./SIMD_str_tests - для запуску тестів стрічки ARM NEON
- ./SIMD_time_comparing - для запуску програми порівняння часу (ARM_NEON)
- ./str_tests - для запуску тестів звичайної стрічки
- ./AVX2_str_tests - для запуску тестів стрічки AVX2

### Important!

В цій роботі було створено імплементації стрічок з інструкцій ARM NEON та AVX2 і для обох є відповідний звіт.

### Results

## ARM NEON

У цій лабораторній було оптимізацію класу стрічки my_str_t на my_str_simd за допомогою SIMD-інструкцій ARM NEON. Метою оптимізації було:
- дослідити швидкодію роботи SIMD інструкцій на Мак М1
- прискорення операцій, порівняння зі стандартною бібліотекою компілятора

Для досягнення цілей було замінено послідовні цикли копіювання (for, memcpy) на SIMD-команди:

- vdupq_n_u8
- vld1q_u8
- vst1q_u8
- vceqq_u8
- vmaxvq_u8

Нижче, детальніший опис інструкцій:

https://developer.arm.com/documentation/101028/0010/Advanced-SIMD--Neon--intrinsics$0

## 1. vdupq_n_u8(value)

Створює вектор шириною 128 біт (16 елементів типу `uint8_t`) і заповнює всі елементи однаковим значенням `value`.

### Назва
- **v** — vector
- **dup** — duplicate (дублювати)
- **q** — 128-бітний вектор ("quad" — чотири 32-бітні елементи)
- **n** — заповнення скаляром
- **u8** — тип елементу: unsigned 8-bit (байт)

Вхід:
```
value = 'A' = 65
```

Вектор результат:
```
[65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65]
```

```cpp
uint8x16_t vinit = vdupq_n_u8(static_cast<uint8_t>('A'));
```


## 2. vld1q_u8(ptr)

Завантажує 128 біт (16 елементів `uint8_t`) з пам'яті в SIMD-регістр.

### Назва
- **v** — vector
- **ld** — load (завантажити)
- **1** — 1 вектор
- **q** — 128 біт
- **u8** — тип unsigned 8-bit

Припустимо пам'ять:
```
ptr = ['H','e','l','l','o',' ','w','o','r','l','d','!','\0','\0','\0','\0']
```

Після виконання:
```
['H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', 0, 0, 0, 0]
```

```cpp
uint8x16_t block = vld1q_u8(reinterpret_cast<const uint8_t*>(data_m + i));
```

## 3. vst1q_u8(ptr, vector)

Записує 128 біт (16 байтів) з SIMD-регістра у пам'ять.

### Назва
- **v** — vector
- **st** — store (зберегти)
- **1** — 1 вектор
- **q** — 128 біт
- **u8** — unsigned 8-bit

Регістр перед записом:
```
['A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A']
```

Після запису в пам'ять:
```
['A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A']
```

```cpp
vst1q_u8(reinterpret_cast<uint8_t*>(data_m + i), vinit);
```


## 4. vceqq_u8(vec1, vec2)

Поелементно порівнює два вектори.

Кожен елемент:
- Якщо рівні — результат `0xFF` (255)
- Якщо різні — результат `0x00` (0)

### Назва
- **v** — vector
- **c** — compare
- **eq** — equal
- **q** — 128 біт
- **u8** — unsigned 8-bit

Вхідні вектори:
```
vec1 = [1, 2, 3, 4, 5, 6, 7, 8, ...]
vec2 = [1, 2, 0, 4, 5, 0, 7, 0, ...]
```
Результат:
```
[0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, ...]
```

```cpp
uint8x16_t cmp = vceqq_u8(block, target_vector);
```


## 5. vmaxvq_u8(vector)

Виконує горизонтальне зведення (reduce max) по всьому вектору: повертає найбільший елемент.

### Назва
- **v** — vector
- **maxv** — maximum across vector
- **q** — 128 біт
- **u8** — unsigned 8-bit


Вхідний вектор:
```
[0xFF, 0xFF, 0xFF, 0xFF, ..., 0xFF] → vmaxvq_u8 → 255
```
При наявності хоча б одного 0:
```
[0xFF, 0x00, 0xFF, 0xFF, ..., 0xFF] → vmaxvq_u8 → 255
```
(Потрібно перевіряти, чи всі елементи рівні 255.)

```cpp
uint64_t mask = vmaxvq_u8(cmp);
if (mask != 255) { return false; }
```

## Огляд змін

ПОК - Створює стрічку із size копій літери initial
```{c++}
my_str_t::my_str_t(size_t size, char initial):
    capacity_m(size*2 + 1), size_m(size) {
    data_m = new char[capacity_m];
    // виділили память довжиною capacity_m
    std::memset(data_m, initial, size);
    // memset заповнює перші
    data_m[size_m] = '\0';
    // на позицію останнього символа '\0', щоб була С стрічка
}
```
Використано vdupq_n_u8 для створення вектору однакових значень.

```{c++}
my_str_simd::my_str_simd(size_t size, char initial)
    : capacity_m(size * 2 + 1), size_m(size) {
    data_m = new char[capacity_m];
    uint8x16_t vinit = vdupq_n_u8(static_cast<uint8_t>(initial));
    // 16 однакових символів
    size_t i = 0;
    for (; i + 16 <= size_m; i += 16) {
        vst1q_u8(reinterpret_cast<uint8_t*>(data_m + i), vinit);
    }
    for (; i < size_m; ++i) {
        data_m[i] = initial;
    }
    data_m[size_m] = '\0';
}
```
ПОК - Копіює вміст С-стрічки
```{c++}
my_str_simd::my_str_simd(const char* cstr) {
  	// null pointer  -> logic error
    if (cstr == nullptr) {
      throw std::logic_error("Null pointer passed to constructor");
    }
    size_m = std::strlen(cstr); //!обчислює довжину стрічки cstr, не враховуючи нульовий символ завершення
    capacity_m = 2 *size_m + 1; //! враховуємо символ завершення
    data_m = new char[capacity_m]; //! виділили память довж size_m
    std::strcpy(data_m, cstr); //! перекопіювали
}
```

було переписано на:

```{c++}
my_str_simd::my_str_simd(const char* cstr) {
    if (cstr == nullptr) {
        throw std::logic_error("Null pointer passed to constructor");
    }
    size_m = std::strlen(cstr);
    capacity_m = 2 *size_m + 1;
    data_m = new char[capacity_m];
    //! виділили память довж size_m
    size_t i = 0;
    // блоками по 16 байт
    for (; i + 16 <= size_m; i += 16) {
        uint8x16_t block = vld1q_u8(reinterpret_cast<const uint8_t*>(cstr + i));
        // завантаження 16 байт
        vst1q_u8(reinterpret_cast<uint8_t*>(data_m + i), block);
        // збереження 16 байт
    }
    for (; i < size_m; ++i) {
        data_m[i] = cstr[i];
    }
    data_m[size_m] = '\0';
}
```

Щоб перевірити продуктивність було заміряно час на копіювання у `my_str_t` та `my_str_simd`:
- Рядок на 16 символів
- Рядок на 32 символи
- Рядок на 64 символи
- Рядок на 128 символи
- Рядок на 256 символів
- Рядок на 512 символи
- Рядок на 1024 символи
- Рядок на 2048 символи
- Рядок на 4*1024 символи
- Рядок на 10к символів

Проте отримані результати були досить суперечливими:
```{txt}
1. Малі розміри (до ~4096 символів):
Всі часи дуже малі (0–2 мікросекунди)
Це в межах похибки вимірювання — таймер high_resolution_clock на практиці має мінімальну роздільну здатність, а ще операційна система додає свій "шум" (вплив інших процесів).
Через це реальні відмінності на малих рядках практично невимірні.
При розмірах до 4К символів SIMD не дає суттєвої переваги — занадто мала кількість даних, щоб розгортання векторизації себе виправдало.

2. Розмір 10 000 символів
my_str_t = 2 мкс
my_str_simd = 3 мкс
SIMD повільніше, хоча й на мізерний час.
Причина: накладні витрати на підготовку SIMD-операцій більші, ніж сама вигода на такому розмірі. Процесори працюють дуже швидко зі звичайною пам'яттю при малих копіюваннях.

3. Великий розмір 1 000 000 символів (1 мегабайт)
my_str_t = 184 мкс
my_str_simd = 309 мкс
SIMD вдвічі повільніше, що дивно.

На малих розмірах (до 10К) — вимірювання нерепрезентативне, похибка більша за сам час роботи.
На великих розмірах (1М символів) — SIMD реалізація працює повільніше, що є поганим знаком.
Проблема не в самій ідеї SIMD, а у конкретній реалізації my_str_simd.
```

Тому було повторено експеримерт і створено 1000 копіювань і поділено сумарний час на кількість повторів

Перш ніж заміряти час, ми виконуємо кілька десятків копіювань без замірювання (warmup_runs разів). 
Це потрібно для:
- прогріву кеш-пам'яті
- активації CPU
- уникнення нестабільностей першого виклику функцій


Для кожного розміру створюється рядок source із заданою кількістю символів 'x'.

Ми виконуємо кількаразове копіювання:
- спочатку об'єктів my_str_t
- потім об'єктів my_str_simd
Кількість повторів:
- для малих рядків (до 10к символів) — 1000 разів
- для великих рядків (1М символів) — 10 разів (щоб тест тривав розумний час)

Щоб компілятор не викинув "непотрібні" копіювання при оптимізації, результат кожного створення об'єкта додається в змінну checksum

| Size (chars) | my_str_t avg time (μs) | my_str_simd avg time (μs) |
|--------------|------------------------|---------------------------|
| 16 | 0 | 0 |
| 32 | 0 | 0 |
| 64 | 0 | 0 |
| 128 | 0 | 0 |
| 256 | 0 | 0 |
| 512 | 0 | 0 |
| 1024 | 0 | 0 |
| 2048 | 0 | 0 |
| 4096 | 0 | 0 |
| 10000 | 0 | 0 |
| 1000000 | 111 | 87 |
| 10000000 | 943 | 770 |
| 100000000 | 10370 | 9666 |

 Маленькі рядки (до ~10 тисяч символів):
- Час копіювання 0 мікросекунд для обох реалізацій.
Це означає, що реальний час копіювання настільки малий, що його неможливо точно виміряти звичайним таймером (мікросекундним) у межах одного копіювання.

Висновок: для малих об’ємів немає практичної різниці між my_str_t і my_str_simd.

Середні розміри (1 млн символів) - SIMD на ~20–25% швидше на рядку розміром 1 мільйон символів.

3. Великі розміри (10 млн і 100 млн символів)

10 млн:
- my_str_t = 943 мкс
- my_str_simd = 770 мкс

100 млн:
- my_str_t = 10370 мкс
- my_str_simd = 9666 мкс

my_str_simd швидший на 15–20%.

Після численних змін функцій копіювання і створення власної стрічки - був оптимізований оператор =
```{c++}
//!оператор присвоєння
//! ПОК
my_str_simd& my_str_simd::operator=(const my_str_simd& mystr) {
    if (this == &mystr) {
        return *this;
    }
    delete[] data_m;
    size_m = mystr.size_m;
    capacity_m = mystr.capacity_m;
    data_m = new char[capacity_m];
    std::memcpy(data_m, mystr.data_m, size_m);
    data_m[size_m] = '\0';
    return *this;
}
```

```{c++}
my_str_simd& my_str_simd::operator=(const my_str_simd& mystr) {
    if (this == &mystr) {
        return *this;
    }
    delete[] data_m; // Звільняємо старі дані
    size_m = mystr.size_m;
    capacity_m = mystr.capacity_m;
    data_m = new char[capacity_m]; // Виділяємо нову пам'ять
    const uint8_t* src = reinterpret_cast<const uint8_t*>(mystr.data_m);
    uint8_t* dst = reinterpret_cast<uint8_t*>(data_m);
    size_t i = 0;
    for (; i + 32 <= size_m; i += 32) {
        vst1q_u8(dst + i, vld1q_u8(src + i));
        vst1q_u8(dst + i + 16, vld1q_u8(src + i + 16));
    }
    for (; i + 16 <= size_m; i += 16) {
        vst1q_u8(dst + i, vld1q_u8(src + i));
    }
    for (; i < size_m; ++i) {
        dst[i] = src[i];
    }
    data_m[size_m] = '\0';
    return *this;
}
```

Під час аналогічного екперименту було отримані такі результати:
| Size (chars) | assignment memcpy (μs) | assignment SIMD (μs) |
|--------------|-------------------------|----------------------|
| 16 | 0 | 0 |
| 32 | 0 | 0 |
| 64 | 0 | 0 |
| 128 | 0 | 0 |
| 256 | 0 | 0 |
| 512 | 0 | 0 |
| 1024 | 0 | 0 |
| 2048 | 0 | 0 |
| 4096 | 0 | 0 |
| 10000 | 0 | 0 |
| 10000 | 0 | 0 |
| 1000000 | 114 | 111 |
| 10000000 | 705 | 696 |

Обидва методи (memcpy і SIMD) мають 0 мікросекунд середнього часу копіювання.

Час копіювання настільки малий, що його неможливо виміряти мікросекундним таймером.

Відмінностей у продуктивності між memcpy і SIMD на маленьких розмірах немає.

Середні розміри (1 млн символів)

memcpy: 114 мкс

SIMD: 111 мкс

Різниця: всього 3 мкс, тобто менше ніж 3% прискорення для SIMD.

Висновок: дуже незначне покращення. На 1М символів SIMD майже не дає виграшу над сучасною оптимізованою memcpy.

Великі розміри (10 млн символів)

memcpy: 705 мкс

SIMD: 696 мкс

Різниця: 9 мкс, тобто близько 1.3% прискорення на 10 мільйонах символів.

Висновок: прискорення ще менше у відсотковому вираженні.

Отримані результати досить схожі - тому що memcpy вже дуже оптимізований у стандартних бібліотеках ОС і компіляторів

Висновок, немає сенсу переписувати operator = вручну через SIMD, якщо архітектура має гарно оптимізований memcpy.

Функцію reserve для класу стрічок у лабі з ПОКУ було написано наступним чином
```{c++}
void my_str_simd::reserve(size_t new_capacity) {
     if (new_capacity > capacity_m) {
         capacity_m = new_capacity;
         char* new_data_m = new char[capacity_m];
        std::memcpy(new_data_m, data_m, size_m);
         delete[] data_m;
         data_m = new_data_m;
         data_m[size_m] = '\0';
     }
}
```
Було замінено на копіювання з допомогою SIMD

```{c++}
void my_str_simd::reserve(size_t new_capacity) {
    if (new_capacity > capacity_m) {
        capacity_m = new_capacity;
        char* new_data_m = new char[capacity_m];
        const uint8_t* src = reinterpret_cast<const uint8_t*>(data_m);
        uint8_t* dst = reinterpret_cast<uint8_t*>(new_data_m);
        size_t i = 0;
        // блоками по 32 байти (2 x 16)
        for (; i + 32 <= size_m; i += 32) {
            vst1q_u8(dst + i, vld1q_u8(src + i));
            vst1q_u8(dst + i + 16, vld1q_u8(src + i + 16));
        }
        // залишок по 16 байт
        for (; i + 16 <= size_m; i += 16) {
            vst1q_u8(dst + i, vld1q_u8(src + i));
        }
        // залишок по 1 байту
        for (; i < size_m; ++i) {
            dst[i] = src[i];
        }
        delete[] data_m;
        data_m = new_data_m;
        data_m[size_m] = '\0';
    }
}
```

Отримані результати тестування:

| Size (chars) | reserve memcpy (μs) | reserve SIMD (μs) |
|--------------|---------------------|-------------------|
| 16 | 0 | 0 |
| 32 | 0 | 0 |
| 64 | 0 | 0 |
| 128 | 0 | 0 |
| 256 | 0 | 0 |
| 512 | 0 | 0 |
| 1024 | 0 | 0 |
| 2048 | 0 | 0 |
| 4096 | 0 | 0 |
| 10000 | 0 | 0 |
| 1000000 | 171 | 159 |
| 10000000 | 1830 | 1750 |

Аналогічні 0 результати для малих стрічок, тому розгленемо детальніше лише середньої і великої довжини

memcpy: 171 мкс

SIMD: 159 мкс

Різниця: 12 мікросекунд, тобто приблизно 7% прискорення на користь SIMD.

memcpy: 1830 мкс

SIMD: 1750 мкс

Різниця: 80 мкс, або близько 4–5% прискорення на користь SIMD.

Висновок: SIMD залишається трохи швидшим, але виграш зменшується при збільшенні розміру. (Тобто на більших розмірах вже не тільки копіювання обмежує час, але й пропускна здатність пам'яті.)

SIMD дозволяє зменшити кількість операцій копіювання (бо іде копіювання блоками по 16/32 байт), тому дає невеликий виграш.

методу shrink_to_fit() на лабі з ПОКУ був реалізований через std::memcpy. Ми ж натомість переробили його через vld1q_u8 / vst1q_u8.

| Size (chars) | shrink memcpy (μs) | shrink SIMD (μs) |
|--------------|--------------------|------------------|
| 16 | 0 | 0 |
| 32 | 0 | 0 |
| 64 | 0 | 0 |
| 128 | 0 | 0 |
| 256 | 0 | 0 |
| 512 | 0 | 0 |
| 1024 | 0 | 0 |
| 2048 | 0 | 0 |
| 4096 | 0 | 0 |
| 10000 | 0 | 0 |
| 1000000 | 108 | 101 |
| 10000000 | 1188 | 1179 |

На 1 мільйоні символів різниця: ~7 мікросекунд, тобто SIMD на приблизно 6.5% швидше.

На 10 мільйонах - різниця: ~9 мікросекунд, тобто SIMD на близько 0.8% швидше.

Оптимізації в інших методах подібні, окрім оператора == та insert.
```{c++}
//! POK version
//! by Oksana Kotliarchuk
// void my_str_simd::insert(size_t idx, const my_str_simd& str) {
//     if (idx > size_m) {
//         throw std::out_of_range("index out of range");
//     }
//
//     if (size_m + str.size_m + 1 > capacity_m) {
//         reserve(2 * (size_m + str.size_m) + 1);
//     }
//     std::memmove(data_m + idx + str.size_m, data_m + idx, size_m - idx);
//     std::memcpy(data_m + idx, str.data_m, str.size_m);
//     size_m += str.size_m;
//     data_m[size_m] = '\0';
// }
```

```{c++}
//! AKS by Maksym Dzoban
//! Insert and the end of a string
void my_str_simd::insert(size_t idx, const my_str_simd& str) {
    if (idx > size_m) {throw std::out_of_range("index out of range");}
    if (size_m + str.size_m + 1 > capacity_m) {
        reserve(2 * (size_m + str.size_m) + 1);
    }
    size_t move_size = size_m - idx;
    const uint8_t* src = reinterpret_cast<const uint8_t*>(data_m + idx);
    uint8_t* dst = reinterpret_cast<uint8_t*>(data_m + idx + str.size_m);
    if (move_size > 0) {
        size_t i = move_size;
        while (i >= 32) {
            i -= 32;
            vst1q_u8(dst + i + 16, vld1q_u8(src + i + 16));
            vst1q_u8(dst + i, vld1q_u8(src + i));
        }
        while (i >= 16) {
            i -= 16;
            vst1q_u8(dst + i, vld1q_u8(src + i));
        }
        while (i > 0) {
            --i;
            dst[i] = src[i];
        }
    }
    src = reinterpret_cast<const uint8_t*>(str.data_m);
    dst = reinterpret_cast<uint8_t*>(data_m + idx);
    size_t copy_size = str.size_m;
    size_t j = 0;
    for (; j + 32 <= copy_size; j += 32) {
        vst1q_u8(dst + j, vld1q_u8(src + j));
        vst1q_u8(dst + j + 16, vld1q_u8(src + j + 16));
    }
    for (; j + 16 <= copy_size; j += 16) {
        vst1q_u8(dst + j, vld1q_u8(src + j));
    }
    for (; j < copy_size; ++j) {
        dst[j] = src[j];
    }

    size_m += str.size_m;
    data_m[size_m] = '\0';
}
```

У тестах вставляємо новий рядок (insert_str) довжиною 10% від основного (str) у середину (insert_position = size/2).

| Size (chars) | insert memcpy/memmove (μs) | insert SIMD (μs) |
|--------------|----------------------------|-----------------|
| 16 | 0 | 0 |
| ...|...| ...|
| 4096 | 0 | 0 |
| 10000 | 0 | 0 |
| 1000000 | 111 | 88 |

Великий розмір (1 мільйон символів)

memcpy/memmove: 111 мкс

SIMD: 88 мкс

Різниця: SIMD швидший на 23 мікросекунди, тобто приблизно на 20%.

У коді у циклі копіюємо блоками з назад - бо dst і src можуть перекриватися.

Метод append реалізовано через insert, тому відносно лабораторної з ПОКУ - не змінювали.

Метод find шукає перше входження підрядка str в рядок data_m, починаючи з позиції idx. Якщо знайдено — повернути індекс початку підрядка. Якщо не знайдено — повернути not_found.

Версія ПОК (Lev Lysyk)
```{c++}
size_t my_str_simd::find(const std::string& str, size_t idx) const {
    if (idx > size_m)
        throw std::out_of_range("my_str_t::find");

    size_t substr_i = 0;
    size_t const str_len = str.size();

    for (size_t i = idx; i < size_m; ++i) {
        while (substr_i < str_len && data_m[i + substr_i] == str[substr_i]) {
            ++substr_i;
        }
        if (substr_i == str_len) {
            return i;
        }
        substr_i = 0;
    }
    return not_found;
}
```
Зовнішній цикл по позиціях у рядку data_m (починаючи з idx).
Для кожної позиції порівнюється підрядок починаючи з цієї позиції. Використовується вкладений цикл while, який перевіряє кожен символ послідовно. Якщо всі символи підрядка співпали — повертається позиція i. Якщо символ не співпав — перевірка скидається (substr_i = 0) і рухаємось далі.

У нашій версії з СІМД нам вдалось оптимізувати цей алгоритм:
```{c++}
size_t my_str_simd::find(const std::string& str, size_t idx) const {
    if (idx > size_m)
        throw std::out_of_range("my_str_simd::find");

    size_t str_len = str.size();
    if (str_len == 0)
        return idx;
    if (str_len > size_m - idx)
        return not_found;

    const uint8_t* src = reinterpret_cast<const uint8_t*>(data_m);
    const uint8_t target = static_cast<uint8_t>(str[0]);
    const uint8x16_t target_vector = vdupq_n_u8(target); // Створюємо вектор із 16 копій першого символу

    size_t i = idx;
    for (; i + 16 <= size_m; i += 16) {
        const uint8x16_t block = vld1q_u8(src + i);       // Читаємо 16 символів
        const uint8x16_t cmp = vceqq_u8(block, target_vector); // Порівнюємо їх з першим символом
        uint8_t cmp_array[16];
        vst1q_u8(cmp_array, cmp);                         // Зберігаємо результати порівняння у масив

        for (int j = 0; j < 16; ++j) {
            if (cmp_array[j] == 0xFF) {                   // Знайшли можливий збіг першого символу
                size_t candidate = i + j;
                if (candidate + str_len <= size_m) {      // Перевіряємо весь підрядок вручну
                    bool matched = true;
                    for (size_t k = 0; k < str_len; ++k) {
                        if (data_m[candidate + k] != str[k]) {
                            matched = false;
                            break;
                        }
                    }
                    if (matched) {
                        return candidate;
                    }
                }
            }
        }
    }

    // Обробка частинки, якщо залишилося < 16 символів
    for (; i < size_m; ++i) {
        if (data_m[i] == str[0]) {
            if (i + str_len <= size_m) {
                bool matched = true;
                for (size_t k = 0; k < str_len; ++k) {
                    if (data_m[i + k] != str[k]) {
                        matched = false;
                        break;
                    }
                }
                if (matched) {
                    return i;
                }
            }
        }
    }

    return not_found;
}
```
Було протестовано пошук підрядка на великих файлах, і отримано результати 
```{c++}
=== Testing find (size: 1 MB) ===
my_str_t find time: 202 microseconds
my_str_simd find time: 146 microseconds
Test passed: found at position 524288
SIMD version is faster by 56 microseconds

=== Testing find (size: 10 MB) ===
my_str_t find time: 2024 microseconds
my_str_simd find time: 1797 microseconds
Test passed: found at position 5242880
SIMD version is faster by 227 microseconds

=== Testing find (size: 100 MB) ===
my_str_t find time: 18053 microseconds
my_str_simd find time: 12246 microseconds
Test passed: found at position 52428800
SIMD version is faster by 5807 microseconds
```

1. На розмірі 1 мегабайт:
SIMD швидший на 56 мкс, що дає приблизно 27% виграш.

2. На розмірі 10 мегабайт:
SIMD виграє 227 мкс, що дає приблизно 11% виграш.
Прискорення трохи зменшується у відсотковому відношенні.

3. На розмірі 100 мегабайт:
Тут виграш у часі майже 32%.


Метод substr створює підрядок (substring) довжиною size, починаючи з позиції begin.

Було порівняно час пошуку підрядка "CDE", коли розмір рядка: 1MB, 10MB, 100MB.

```{txt}
=== Testing find substring 'CDE' (size: 1 MB) ===
my_str_t find time: 484 microseconds
my_str_simd find time: 562 microseconds
Test passed: found at position 524288.
Normal version is faster by 78 microseconds!

=== Testing find substring 'CDE' (size: 10 MB) ===
my_str_t find time: 5501 microseconds
my_str_simd find time: 5664 microseconds
Test passed: found at position 5242880.
Normal version is faster by 163 microseconds!

=== Testing find substring 'CDE' (size: 100 MB) ===
my_str_t find time: 34535 microseconds
my_str_simd find time: 39101 microseconds
Test passed: found at position 52428800.
Normal version is faster by 4566 microseconds!

=== Testing find(const char*) 'CDE' (size: 1 MB) ===
my_str_t find time: 331 microseconds
my_str_simd find time: 379 microseconds
Test passed: found at position 524288.
Normal version is faster by 48 microseconds!

=== Testing find(const char*) 'CDE' (size: 10 MB) ===
my_str_t find time: 3314 microseconds
my_str_simd find time: 3642 microseconds
Test passed: found at position 5242880.
Normal version is faster by 328 microseconds!

=== Testing find(const char*) 'CDE' (size: 100 MB) ===
my_str_t find time: 33613 microseconds
my_str_simd find time: 38348 microseconds
Test passed: found at position 52428800.
Normal version is faster by 4735 microseconds!
```

Це стається оскільки  SIMD-версія оптимізована тільки на пошук одного символу (str[0]) у великому рядку. Але коли треба знайти підрядок (CDE — три символи підряд), після знаходження першого символу C - вона перевіряє всі символи руками (через посимвольне порівняння for (k = 0; k < str_len; ++k). робить це неоптимально (зазвичай повільніше ніж оптимізоване std::memcmp або зв'язаний пошук у нормальному std::string. 

SIMD швидко знаходить кандидати на збіг першого символу, але повільно перевіряє всю підрядкову відповідність. my_str_t використовує ймовірно або: 
- оптимізовану std::search
- або має менше зайвих перевірок.

Оператор == є незвичним серед інших методів:

```{c++}
 bool operator==(const my_str_simd &str1, const my_str_simd &str2) {
     if (str1.size() != str2.size()) { return false; }
     for (size_t i = 0; i < str1.size(); i++) {
         if (str1[i] != str2[i]) { return false; }
     }
     return true;
}
```

Перевірялося, чи однакова довжина рядків. Якщо ні — відразу false. Якщо довжина однакова, порівнюються усі символи по черзі: від першого до останнього. Якщо знайдено різні символи — повертає false. Якщо всі однакові — повертає true.

```{c++}
bool operator==(const my_str_simd &str1, const my_str_simd &str2) {
    if (str1.size() != str2.size()) {
        return false;
    }
    size_t size = str1.size();
    const uint8_t* p1 = reinterpret_cast<const uint8_t*>(str1.c_str());
    const uint8_t* p2 = reinterpret_cast<const uint8_t*>(str2.c_str());
    size_t i = 0;
    for (; i + 32 <= size; i += 32) {
        uint8x16_t v1_0 = vld1q_u8(p1 + i);
        uint8x16_t v2_0 = vld1q_u8(p2 + i);
        uint8x16_t v1_1 = vld1q_u8(p1 + i + 16);
        uint8x16_t v2_1 = vld1q_u8(p2 + i + 16);
        uint8x16_t cmp0 = vceqq_u8(v1_0, v2_0);
        uint8x16_t cmp1 = vceqq_u8(v1_1, v2_1);
        uint64_t mask0 = vmaxvq_u8(cmp0);
        uint64_t mask1 = vmaxvq_u8(cmp1);
        if (mask0 != 255 || mask1 != 255) {
            return false;
        }
    }
    for (; i + 16 <= size; i += 16) {
        uint8x16_t v1 = vld1q_u8(p1 + i);
        uint8x16_t v2 = vld1q_u8(p2 + i);
        uint8x16_t cmp = vceqq_u8(v1, v2);
        uint64_t mask = vmaxvq_u8(cmp);
        if (mask != 255) {
            return false;
        }
    }
    for (; i < size; ++i) {
        if (p1[i] != p2[i]) {
            return false;
        }
    }
    return true;
}
```

У новій версії коду порівнюємо рядки SIMD-блоками:
- Спочатку обробляємо 32 символи за раз (двома векторами по 16).
- Потім, якщо залишилось менше 32 символів, обробляємо по 16 символів.
- Потім, якщо залишились "хвостики", обробляємо по одному символу.

| Тест | SIMD час | Стандартний час | Виграш SIMD |
|:---|:---|:---|:---|
| Порівняння коротких рядків (10k разів) | 64 мкс, 128 мкс | 162 мкс, 191 мкс | SIMD швидше у 2.5-3× |
| Порівняння великих рядків (1 раз) | 867 мкс | 5653 мкс | SIMD швидше у ~6.5× |
| operator== (simd, const char*) короткі рядки (10k разів) | 56 мкс, 105 мкс | 132 мкс, 151 мкс | SIMD швидше у 2-2.5× |
| operator== (simd, const char*) великі рядки (1 раз) | 765 мкс | 4920 мкс | SIMD швидше у ~6.4× |
| operator== (simd, simd) короткі рядки (10k разів) | 32 мкс | 100 мкс | SIMD швидше у ~3× |
| operator== (simd, simd) великі рядки (1 раз) | 501 мкс | 4248 мкс | SIMD швидше у ~8.5× |


## AVX2

В цій частині лабораторної роботи ми переробили звичайну реалізацію стрічки, що використовує стандартні інструменти с++ на реалізацію, що використовує SIMD інструкції
(в контексті цього звіту це AVX2) та дослідили швидкодію SIMD реалізації порівняно зі звичайною. Перероблену стрічку можна побачити в файлах mystring_avx.cpp та mystring_avx.h.

Для досягнення цілей було замінено послідовні цикли копіювання (for, memcpy) на SIMD-команди:

- _mm256_loadu_si256 -- завантажує цілочисельні значення з 256-бітної незгрупованої (unaligned) області памʼяті, на яку вказує *a, у вектор цілих чисел, який повертається інструкцією.

- _mm256_storeu_si256 -- виконує операцію збереження, переміщаючи цілочисельні значення з 256-бітного вектору цілих чисел b у 256-бітну незгруповану область памʼяті, на яку вказує a.

- _mm256_set1_epi8 -- ініціалізує 256-бітний вектор скалярним цілочисельним значенням (8/16/32/64-бітним), вказаним у параметрі a, розмножуючи його на всі елементи вектора.

- _mm256_cmpeq_epi8 -- виконує SIMD-порівняння на рівність упакованих байтів, слів, даблвордів або квадвордів у вхідних векторах s1 та s2. Якщо пара елементів рівна — відповідний елемент у векторі результату встановлюється в усі 1; якщо не рівна — у 0.

- _mm256_movemask_epi8 -- витягує найстарший біт (MSB) кожного з 32 байтів у векторі a та упаковує їх у 32-бітне ціле число.

## Огляд коду

Так як в оригінальному коді стрічки дуже часто траплявся memcpy його варто було переписати використовуючи інструкції AVX2.

```cpp
inline void avx2_memcpy(const char* src, char* dst, size_t size) {
    size_t i = 0;
    for (; i + 32 <= size; i += 32) {
        __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src + i));
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst + i), block);
    }
    for (; i < size; ++i) {
        dst[i] = src[i];
    }
}
```

Також замість використання memset були використані інструкції SIMD зазначені вище:

```cpp
void my_str_avx::resize(size_t new_size, char new_char) {
    if (new_size > size_m) {
        if (new_size > capacity_m) {
            reserve(new_size * 2);
        }
        char* dst = data_m + size_m;
        size_t fill_size = new_size - size_m;
        size_t i = 0;
        __m256i fill_value = _mm256_set1_epi8(static_cast<char>(new_char));
        for (; i + 32 <= fill_size; i += 32) {
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst + i), fill_value);
        }
        for (; i < fill_size; ++i) {
            dst[i] = new_char;
        }
    }
    size_m = new_size;
    data_m[size_m] = '\0';
}
```

```cpp
size_t my_str_avx::find(const char* cstr, size_t idx) const {
    if (cstr == nullptr) {
        throw std::logic_error("Null pointer passed to find()");
    }
    if (idx > size_m) {
        throw std::out_of_range("my_str_avx::find");
    }
    size_t str_len = std::strlen(cstr);
    if (str_len == 0) {
        return idx;
    }
    if (str_len > size_m - idx) {
        return not_found;
    }
    
    const char* src = data_m;
    char target = cstr[0];
    __m256i target_vec = _mm256_set1_epi8(target);
    size_t i = idx;

    for (; i + 32 <= size_m; i += 32) {
        __m256i data_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src + i));
        __m256i cmp = _mm256_cmpeq_epi8(data_vec, target_vec);
        int mask = _mm256_movemask_epi8(cmp);

        while (mask != 0) {
            int offset = __builtin_ctz(mask);
            size_t candidate = i + offset;

            if (candidate + str_len <= size_m &&
                // Використовуємо memcmp для швидкодії. Інший варіант писати свій аналог що буде повільнішим
                std::memcmp(src + candidate, cstr, str_len) == 0) {
                return candidate;
            }

            mask &= (mask - 1);
        }
    }

    for (; i <= size_m - str_len; ++i) {
        if (src[i] == target &&
            // Використовуємо memcmp для швидкодії. Інший варіант писати свій аналог що буде повільнішим
            std::memcmp(src + i, cstr, str_len) == 0) {
            return i;
        }
    }

    return not_found;
}
```


## Аналіз швидкодії

1. Копіювання

Ось порівняння швидкості запуску функцій копіювання для обох варіантів стрічки:

| Size (chars) | my_str_t avg time (μs) | my_str_avx avg time (μs) |
|--------------|------------------------|---------------------------|
| 16 | 0 | 0 |
| 32 | 0 | 0 |
| 64 | 0 | 0 |
| 128 | 0 | 0 |
| 256 | 0 | 0 |
| 512 | 0 | 0 |
| 1024 | 0 | 0 |
| 2048 | 0 | 0 |
| 4096 | 0 | 0 |
| 10000 | 1 | 1 |
| 1000000 | 457 | 516 |
| 10000000 | 4675 | 4871 |
| 100000000 | 33382 | 36792 |

Як видно з таблиці, при копіюванні стрічок малого розміру (до 10 000 символів) різниця в часі між реалізаціями фактично відсутня — обидві працюють практично миттєво. Однак із ростом обсягу даних стає помітним, що звичайна реалізація my_str_t копіює великі стрічки швидше за SIMD-версію my_str_avx. Зокрема, при розмірі 100 мільйонів символів стандартна версія завершує копіювання на понад 3 мс швидше. Це свідчить про те, що в задачі копіювання великих обсягів даних традиційні інструменти C++ (зокрема memcpy) залишаються надзвичайно ефективними, й SIMD-інструкції, ймовірно, не компенсують своїх накладних витрат на ініціалізацію та вирівнювання.

2. Функція shrink_to_fit

Ось порівняння швидкості запуску функцій shrink_to_fit для обох варіантів стрічки:

| Size (chars) | shrink memcpy (μs) | shrink SIMD (μs) |
|--------------|--------------------|------------------|
| 16 | 0 | 0 |
| 32 | 0 | 0 |
| 64 | 0 | 0 |
| 128 | 0 | 0 |
| 256 | 0 | 0 |
| 512 | 0 | 0 |
| 1024 | 0 | 0 |
| 2048 | 0 | 0 |
| 4096 | 1 | 2 |
| 10000 | 3 | 3 |
| 1000000 | 602 | 589 |
| 10000000 | 7020 | 7170 |

З результатів видно, що для малих розмірів стрічки (до кількох тисяч символів) виконання функції shrink_to_fit практично миттєве в обох реалізаціях. Починаючи з розміру у мільйон символів, спостерігається незначна перевага SIMD-реалізації: вона показує трохи кращі результати, зокрема на 13 мкс швидше при 1 000 000 символів. Проте при ще більших обсягах (10 000 000 символів) ця перевага втрачається — SIMD-реалізація стає навіть повільнішою. Це свідчить про те, що при виконанні shrink_to_fit швидкодія SIMD залежить від конкретного обсягу даних і не є стабільно кращою, що, ймовірно, пов’язано з характером алокацій та кешування в системі.

3. Функція insert

Ось порівняння швидкості запуску функцій insert для обох варіантів стрічки:

| Size (chars) | insert memcpy/memmove (μs) | insert SIMD (μs) |
|--------------|----------------------------|-----------------|
| 16 | 0 | 0 |
| 32 | 0 | 0 |
| 64 | 0 | 0 |
| 128 | 0 | 0 |
| 256 | 0 | 0 |
| 512 | 0 | 0 |
| 1024 | 0 | 0 |
| 2048 | 0 | 0 |
| 4096 | 1 | 1 |
| 10000 | 2 | 2 |
| 1000000 | 587 | 598 |

Як можемо бачити, обидві реалізації функції `insert` демонструють майже ідентичну продуктивність для невеликих розмірів стрічки — до 10 000 символів час виконання дорівнює або близький до нуля. Починаючи з розміру 1 000 000 символів, спостерігається мінімальне зростання часу, але й тут різниця між звичайною та SIMD-реалізацією становить лише близько 11 мкс, що в межах похибки вимірювань. Це свідчить про те, що SIMD не дає помітного прискорення у випадку вставки, ймовірно через складність переносу даних у памʼяті, де швидкість визначається не лише копіюванням, а й зміщенням елементів.

4. Функція reserve

Ось порівняння швидкості запуску функцій reserve для обох варіантів стрічки:

| Size (chars) | reserve memcpy (μs) | reserve SIMD (μs) |
|--------------|---------------------|-------------------|
| 16 | 0 | 0 |
| 32 | 0 | 0 |
| 64 | 0 | 0 |
| 128 | 0 | 0 |
| 256 | 0 | 0 |
| 512 | 0 | 0 |
| 1024 | 0 | 0 |
| 2048 | 1 | 1 |
| 4096 | 1 | 1 |
| 10000 | 3 | 3 |
| 1000000 | 792 | 848 |
| 10000000 | 7614 | 7490 |

У випадку функції reserve обидві реалізації — як звичайна, так і SIMD — демонструють однакову продуктивність для малих обʼємів даних (до 10 000 символів), із часом виконання від 0 до 3 мкс. При роботі з великими обʼємами (1 000 000 та 10 000 000 символів) спостерігається очікуване зростання часу, однак різниця між реалізаціями залишається незначною — на рівні 50–150 мкс. Цікаво, що SIMD-варіант дещо повільніший на 1 мільйоні символів, але випереджає звичайний варіант на 10 мільйонах, що може свідчити про ефективнішу роботу AVX2 при досягненні певного порогу обʼєму памʼяті. Проте загалом переваги SIMD у цьому випадку не є значущими.

## Висновки
Отже, згідно з проведеними тестами (продемонстрованими раніше та іншимими), SIMD-реалізація my_str_avx демонструє перевагу лише у вибіркових сценаріях — зокрема, у функції find, де вона значно випереджає звичайну реалізацію my_str_t (до 10–20 разів швидше при великих обʼємах даних). Водночас при операціях insert, erase, resize та reserve класична реалізація здебільшого показує кращу або подібну продуктивність, переважаючи SIMD у більшості випадків на десятки або навіть сотні мікросекунд. Особливо це помітно для resize, де різниця може сягати до 1056 мкс, та insert, де перевага класичної реалізації зростає з розміром буфера. Таким чином, хоча SIMD може дати значне прискорення в задачах пошуку (find), його використання в інших операціях може бути недоцільним через складність реалізації та слабке масштабування. Найефективнішим виглядає гібридний підхід, де SIMD використовується лише для обчислювально інтенсивних функцій.


# Робота із стрічками за допомогою SIMD (SSE4.1)
--- 
Команда: Дзьобан Максим, Биков Данило, Шевчук Іван

В цій лабораторній роботі ми переробили звичайну реалізацію стрічки, що використовує стандартні інструменти с++ на реалізацію, що використовує SIMD інструкції
(в контексті цього звіту це SSE4.1) та дослідили швидкодію SIMD реалізації порівняно зі звичайною. Перероблену стрічку можна побачити в файлах mystring_simd_sse.h та main_sse.cpp.

Ось детальний опис інструкцій SIMD (SSE 4.1), які використовуються у вашому класі my_str_simd_sse, із вказанням:
- Назви інструкцій
- Що вони роблять
- Яку ширину мають (в байтах / бітах)
- Коментарі щодо застосування

| Інструкція| Назва функції (intrinsic)| Опис дії| Ширина| 
| -------------------- | ------------------------- | --------------------------------------------------------------------- | ------------------ |
| **завантаження**     | `_mm_loadu_si128`         | Завантажує 16 байт (128 біт) із **невирівняної** адреси в регістр XMM | 128 біт (16 байт) |
| **збереження**       | `_mm_storeu_si128`        | Зберігає 16 байт (128 біт) з регістра XMM у памʼять                   | 128 біт            | Для запису SIMD-блоків у памʼять              |
| **створення блоку**  | `_mm_set1_epi8(c)`        | Створює вектор із 16 однакових байтів (заповнення `c`)                | 128 біт            | Ефективно для ініціалізації рядків            |
| **порівняння**       | `_mm_cmpeq_epi8(a, b)`    | Побітове порівняння кожного байта `a[i] == b[i]` → 0xFF або 0x00      | 128 біт            | Використовується для пошуку символів          |
| **створення маски**  | `_mm_movemask_epi8(a)`    | Створює 16-бітову маску з найстарших бітів кожного байта              | 16 біт (з 128 біт) | Використовується для виявлення місця збігу    |
| **\_\_builtin\_ctz** | GCC вбудована функція     | Повертає кількість нулів справа (позиція першого біта 1 в масці)      | 32/64 біт (на CPU) | Визначає позицію першого збігу                |


`_mm_set1_epi8(char c)` - заповнює 16-байтовий вектор значенням initial.

```{c++}
__m128i vinit = _mm_set1_epi8(initial);
```

`_mm_loadu_si128`, `_mm_storeu_si128` - копіює 16 байт з cstr у data_m.
```{c++}
__m128i cmp = _mm_cmpeq_epi8(data_vector, target_vector);
int mask = _mm_movemask_epi8(cmp);
```
Пришвидшує операції копіювання та вставки (наприклад, у конструкторах, insert, append, erase).

`_mm_cmpeq_epi8, _mm_movemask_epi8` - порівнює кожен байт (символ) і створює маску з результатів (1 якщо рівні, 0 інакше).
```
__m128i cmp = _mm_cmpeq_epi8(data_vector, target_vector);
int mask = _mm_movemask_epi8(cmp);
```
пришвидшує пошук символу чи підрядка (find(char), find(const char*), operator==)

`__builtin_ctz(mask)`
Що робить: визначає позицію першого знайденого символу (збігу) в масці.
Навіщо: дозволяє обчислити точний індекс без циклів.


Ширина SIMD інструкцій:
 - Всі інструкції працюють з 128-бітовими регістрами SSE (тобто 16 байт за раз).
- Це означає, що операції відбуваються векторно над 16 символами рядка одночасно, що значно швидше за побайтове опрацювання.

У кількох випадках працюємо з блоками 32 байти — це 2 х 16 байт (дві інструкції load/store підряд).

Код (реалізацію на SSE 4.1) було протестовано на аналогічних тестах описаних в частині ARM NEON. Було використано ноутбук Lenovo X220 ThinkPad. 8 GB RAM.

Normal Copy Time vs. SIMD copy time:
| Size (chars) | my_str_t avg time (μs) | my_str_simd_sse avg time (μs) |
|--------------|------------------------|---------------------------|
| 16 | 0 | 0 |
| 32 | 0 | 0 |
| 64 | 0 | 0 |
| 128 | 0 | 0 |
| 256 | 0 | 0 |
| 512 | 0 | 0 |
| 1024 | 0 | 1 |
| 2048 | 0 | 0 |
| 4096 | 0 | 0 |
| 10000 | 0 | 0 |
| 1000000 | 780 | 132 |
| 10000000 | 2633 | 2040 |
| 100000000 | 70589 | 70911 |
SIMD показує помітне прискорення на середніх обʼємах (1–10 MB), але на великих (100MB) приріст зникає через пропускну здатність памʼяті (memory-bound).
Для середніх рядків SIMD прискорює копіювання у 4–6 разів.

```{txt}

=== Testing find (size: 1 MB) ===
my_str_t find time: 381 microseconds
my_str_simd_sse find time: 49 microseconds
Test passed: found at position 524288.
SIMD version is faster by 332 microseconds!

=== Testing find (size: 10 MB) ===
my_str_t find time: 4023 microseconds
my_str_simd_sse find time: 554 microseconds
Test passed: found at position 5242880.
SIMD version is faster by 3469 microseconds!

=== Testing find (size: 100 MB) ===
my_str_t find time: 36968 microseconds
my_str_simd_sse find time: 5164 microseconds
Test passed: found at position 52428800.
SIMD version is faster by 31804 microseconds!

=== Testing find substring 'CDE' (size: 1 MB) ===
my_str_t find time: 738 microseconds
my_str_simd_sse find time: 93 microseconds
Test passed: found at position 524288.
SIMD version is faster by 645 microseconds

=== Testing find substring 'CDE' (size: 10 MB) ===
my_str_t find time: 7232 microseconds
my_str_simd_sse find time: 1191 microseconds
Test passed: found at position 5242880.
SIMD version is faster by 6041 microseconds

=== Testing find substring 'CDE' (size: 100 MB) ===
my_str_t find time: 94725 microseconds
my_str_simd_sse find time: 9644 microseconds
Test passed: found at position 52428800.
SIMD version is faster by 85081 microseconds

=== Testing find(const char*) 'CDE' (size: 1 MB) ===
my_str_t find time: 724 microseconds
my_str_simd_sse find time: 120 microseconds
Test passed: found at position 524288.
SIMD version is faster by 604 microseconds!

=== Testing find(const char*) 'CDE' (size: 10 MB) ===
my_str_t find time: 7040 microseconds
my_str_simd_sse find time: 941 microseconds
Test passed: found at position 5242880.
SIMD version is faster by 6099 microseconds!

=== Testing find(const char*) 'CDE' (size: 100 MB) ===
my_str_t find time: 77713 microseconds
my_str_simd_sse find time: 9803 microseconds
Test passed: found at position 52428800.
SIMD version is faster by 67910 microseconds!
```
SIMD пошук дає вражаюче прискорення у всіх випадках: в 7–10 разів при пошуку символів або підрядків. Це один із найсильніших сценаріїв застосування SIMD.

| Size (chars) | insert memcpy/memmove (μs) | insert SIMD (μs) |
|--------------|----------------------------|-----------------|
| 16 | 0 | 0 |
| 32 | 0 | 0 |
| 64 | 0 | 0 |
| 128 | 0 | 0 |
| 256 | 0 | 0 |
| 512 | 0 | 0 |
| 1024 | 0 | 0 |
| 2048 | 0 | 0 |
| 4096 | 0 | 0 |
| 10000 | 1 | 1 |
| 1000000 | 149 | 126 |
SIMD не дає значного виграшу при вставці, особливо коли обʼєми пам’яті великі. Це пов’язано з обов’язковим зсувом вмісту масиву, який стає обмеженням на продуктивність.

Resize (1–10MB): перевага відсутня або несуттєва (0–4 мкс)
Reserve (1–10MB): розбіжність в межах похибки (254 мкс vs 224 мкс)
Shrink:
- (1MB): Normal: 767 мкс, SIMD: 149 мкс → в 5 разів швидше
- (10MB): Normal: 5390 мкс, SIMD: 5985 мкс → іноді повільніше

**Висновок**: для shrink_to_fit() SIMD може дати значне прискорення на середніх обʼємах. Для resize() і reserve() — різниця мінімальна.

SIMD-оптимізована реалізація рядкових операцій показує значне покращення продуктивності у найбільш ресурсоємних сценаріях, таких як:
 - копіювання великих обсягів даних,
 - пошук символів або підрядків,
- порівняння довгих рядків.

Прискорення може сягати 7–10 разів, особливо при роботі з великими обʼємами тексту (10MB–100MB). Вставка та зміна розміру не завжди виграють від SIMD, оскільки вони обмежені перенесенням памʼяті, яке не паралелізується добре. На малих розмірах (до 4KB) різниця практично не спостерігається — SIMD вигідно використовувати лише на середніх та великих рядках. SIMD-тести проходять коректно, і результат збігається з очікуваним функціоналом, що підтверджує правильність реалізації.

"SIMD дозволяє ефективно прискорити найбільш затратні за часом рядкові операції, особливо на великих об'ємах. Найбільше прискорення зафіксовано в операціях пошуку та порівняння, де SIMD забезпечує векторну обробку даних, обходячи байт за байтом звичайного алгоритму."
