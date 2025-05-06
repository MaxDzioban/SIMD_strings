#include <iostream>
#include "mystring_simd_sse.h"
#include <cstring>
#include <stdexcept>
#include <smmintrin.h>
#include <cstdint>
#include <string>

// sse 4.1

//! допоміжний конструктор. Аналогічний як ПОК лабі
my_str_simd_sse::my_str_simd_sse(size_t const capacity):
        capacity_m(capacity * 2 + 1), size_m(0) {
    data_m = new char[capacity_m];
    data_m[0] = '\0';
}

// ПОК
// Створює стрічку із size копій літери initial
// my_str_t::my_str_t(size_t size, char initial):
//     capacity_m(size*2 + 1), size_m(size) {
//     data_m = new char[capacity_m]; // виділили память довжиною capacity_m
//     std::memset(data_m, initial, size); // memset заповнює перші
//     data_m[size_m] = '\0'; // на позицію останнього символа '\0', щоб була С стрічка
// }

//! АКС
//! Створює стрічку із size копій літери initial

my_str_simd_sse::my_str_simd_sse(size_t size, char initial)
    : capacity_m(size * 2 + 1), size_m(size) {
    data_m = new char[capacity_m];
    __m128i vinit = _mm_set1_epi8(initial);
    size_t i = 0;
    for (; i + 16 <= size_m; i += 16) {
        _mm_storeu_si128(reinterpret_cast<__m128i*>(data_m + i), vinit);
    }

    for (; i < size_m; ++i) {
        data_m[i] = initial;
    }
    data_m[size_m] = '\0';
}

//! ПОК
//! Копіює вміст С-стрічки, вимоги до capacity_m -- ті ж, що вище
// my_str_simd::my_str_simd(const char* cstr) {
//   	// null pointer  -> logic error
//     if (cstr == nullptr) {
//       throw std::logic_error("Null pointer passed to constructor");
//     }
//     size_m = std::strlen(cstr); //!обчислює довжину стрічки cstr, не враховуючи нульовий символ завершення
//     capacity_m = 2 *size_m + 1; //! враховуємо символ завершення
//     data_m = new char[capacity_m]; //! виділили память довж size_m
//     std::strcpy(data_m, cstr); //! перекопіювали
// }


//! Копіює вміст С-стрічки, вимоги до capacity_m -- ті ж, що вище
//! АКС
my_str_simd_sse::my_str_simd_sse(const char* cstr) {
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
        __m128i block = _mm_loadu_si128(reinterpret_cast<const __m128i*>(cstr + i));
        // завантаження 16 байт
        _mm_storeu_si128(reinterpret_cast<__m128i*>(data_m + i), block);
        // збереження 16 байт
    }
    for (; i < size_m; ++i) {
        data_m[i] = cstr[i];
    }
    data_m[size_m] = '\0';
}

//! ПОК
//! Копіює стрічку С++, вимоги до capacity_m -- ті ж, що вище
// my_str_simd::my_str_simd(const std::string& str) {
//     size_m = str.size();    // Зберігаємо розмір стрічки
//     capacity_m = size_m * 2 + 1;    // Виділяємо місце під символ '\0'
//     data_m = new char[capacity_m];  // Виділяємо пам'ять
//     std::strcpy(data_m, str.c_str());   // Копіюємо вміст std::string у data_m
//     data_m[size_m] = '\0';
// }

//! АКС
my_str_simd_sse::my_str_simd_sse(const std::string& str) {
    size_m = str.size();
    // розмір стрічки
    capacity_m = size_m * 2 + 1;
    data_m = new char[capacity_m];
    // виділяємо пам'ять
    const char* src = str.data();
    char* dst = data_m;

    size_t i = 0;
    // по 32 байти (2 x 16)
    for (; i + 32 <= size_m; i += 32) {
        __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i + 16));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v0);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i + 16), v1);
    }

    // по 16 байт
    for (; i + 16 <= size_m; i += 16) {
        __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v);
    }
    // по 1 байту
    for (; i < size_m; ++i) {
        dst[i] = src[i];
    }
    data_m[size_m] = '\0';
}

//! конструктор копій
//! ПОК
// my_str_simd::my_str_simd(const my_str_simd & mystr):
//     capacity_m(mystr.capacity_m),size_m(mystr.size_m) {
//     data_m = new char[capacity_m];
//     std::memcpy(data_m, mystr.data_m, size_m);
//     data_m[size_m] = '\0';
// }

my_str_simd_sse::my_str_simd_sse(const my_str_simd_sse& mystr)
    : capacity_m(mystr.capacity_m), size_m(mystr.size_m)
{
    data_m = new char[capacity_m];
    // виділяємо пам'ять
    const char* src = mystr.data_m;
    char* dst = data_m;

    size_t i = 0;
    // по 32 байти (2 x 16)
    for (; i + 32 <= size_m; i += 32) {
        __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i + 16));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v0);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i + 16), v1);
    }

    // по 16 байт
    for (; i + 16 <= size_m; i += 16) {
        __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v);
    }
    // по 1 байту
    for (; i < size_m; ++i) {
        dst[i] = src[i];
    }
    data_m[size_m] = '\0';
    // завершуємо стрічку
}

//!оператор присвоєння
//! ПОК
// my_str_simd& my_str_simd::operator=(const my_str_simd& mystr) {
//     if (this == &mystr) {
//         return *this;
//     }
//     delete[] data_m;
//     size_m = mystr.size_m;
//     capacity_m = mystr.capacity_m;
//     data_m = new char[capacity_m];
//     std::memcpy(data_m, mystr.data_m, size_m);
//     data_m[size_m] = '\0';
//     return *this;
// }

//! АКС
my_str_simd_sse& my_str_simd_sse::operator=(const my_str_simd_sse& mystr) {
    if (this == &mystr) {
        return *this;
    }
    delete[] data_m; // Звільняємо старі дані
    size_m = mystr.size_m;
    capacity_m = mystr.capacity_m;
    data_m = new char[capacity_m];
    // Виділяємо нову пам'ять
    const char* src = mystr.data_m;
    char* dst = data_m;
    size_t i = 0;
    // по 32 байти (2 x 16)
    for (; i + 32 <= size_m; i += 32) {
        __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i + 16));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v0);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i + 16), v1);
    }
    // по 16 байт
    for (; i + 16 <= size_m; i += 16) {
        __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v);
    }
    // по 1 байту
    for (; i < size_m; ++i) {
        dst[i] = src[i];
    }
    data_m[size_m] = '\0';
    return *this;
}

// by Lev Lysyk
//! POK
//! Немає сенсу у SIMD оптимізації для цієї функції
void my_str_simd_sse::swap(my_str_simd_sse& other) noexcept {
    std::swap(data_m, other.data_m);
    std::swap(capacity_m, other.capacity_m);
    std::swap(size_m, other.size_m);
}

//! by Maksym Dzoban POK
//! Немає сенсу у SIMD оптимізації для цієї функції
//! непостійний оператор індексації, повертає посилання на символ, що можна змінити.
char& my_str_simd_sse::operator[](size_t idx) {
    if (idx>= size_m)
    {throw std::out_of_range("Incorrect index!");}
    return data_m[idx];
}

//! by Maksym Dzoban POK
//! Немає сенсу у SIMD оптимізації для цієї функції
//! постійний оператор індексування,
//! що використовується для постійних обєктів
const char& my_str_simd_sse::operator[](size_t idx) const {
    if (idx>= size_m)
    {throw std::out_of_range("Incorrect index!");}
    return data_m[idx];
}

//! by Oksana Kotliarchuk POK
//! Немає сенсу у SIMD оптимізації для цієї функції
char& my_str_simd_sse:: at(size_t idx) {
    if (idx >= size_m) {
        throw std::out_of_range("Incorrect index!");
    }
    return data_m[idx];
}

//! by Oksana Kotliarchuk POK
//! Немає сенсу у SIMD оптимізації для цієї функції
const char& my_str_simd_sse:: at(size_t idx) const {
    if (idx >= size_m) {
        throw std::out_of_range("Incorrect index!");
    }
    return data_m[idx];
}


//! ПОК  by Lev Lysyk
// void my_str_simd::reserve(size_t new_capacity) {
//     if (new_capacity > capacity_m) {
//         capacity_m = new_capacity;
//         char* new_data_m = new char[capacity_m];
//         std::memcpy(new_data_m, data_m, size_m);
//         delete[] data_m;
//         data_m = new_data_m;
//         data_m[size_m] = '\0';
//     }
// }

void my_str_simd_sse::reserve(size_t new_capacity) {
    if (new_capacity > capacity_m) {
        capacity_m = new_capacity;
        auto new_data_m = new char[capacity_m];
        const char* src = data_m;
        char* dst = new_data_m;
        size_t i = 0;
        // по 32 байти (2 x 16)
        for (; i + 32 <= size_m; i += 32) {
            __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
            __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i + 16));
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v0);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i + 16), v1);
        }
        // по 16 байт
        for (; i + 16 <= size_m; i += 16) {
            __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v);
        }
        // по 1 байту
        for (; i < size_m; ++i) {
            dst[i] = src[i];
        }
        new_data_m[size_m] = '\0';
        delete[] data_m;
        data_m = new_data_m;
    }
}


//! POK by Oksana Kotliarchuk
//! обрізає
// void my_str_simd:: shrink_to_fit() {
//     capacity_m = size_m + 1;
//     char* data = new char[capacity_m];
//     std::memcpy(data, data_m, size_m);
//     delete[] data_m;
//     data_m = data;
//     data_m[size_m] = '\0';
// }

//! АКС by Maksym Dzoban
//! обрізає
void my_str_simd_sse::shrink_to_fit() {
    capacity_m = size_m + 1;
    char* data = new char[capacity_m];
    const char* src = data_m;
    char* dst = data;
    size_t i = 0;
    for (; i + 32 <= size_m; i += 32) {
        __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i + 16));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v0);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i + 16), v1);
    }
    for (; i + 16 <= size_m; i += 16) {
        __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v);
    }
    for (; i < size_m; ++i) {
        dst[i] = src[i];
    }
    delete[] data_m;
    data_m = data;
    data_m[size_m] = '\0';
}

//! ПОК by Lev Lysyk
// void my_str_simd::resize(size_t new_size, char new_char) {
//     if (new_size > size_m) {
//         if (new_size > capacity_m) {
//             reserve(new_size * 2);
//         }
//         memset(data_m + size_m, new_char, new_size - size_m);
//     }
//     size_m = new_size;
//     data_m[size_m] = '\0';
// }

//! АКС by Maksym Dzoban
//! resize - забирає кінець стрічки.
void my_str_simd_sse::resize(size_t new_size, char new_char) {
    if (new_size > size_m) {
        if (new_size > capacity_m) {
            reserve(new_size * 2);
        }
        uint8_t* dst = reinterpret_cast<uint8_t*>(data_m + size_m);
        size_t fill_size = new_size - size_m;
        size_t i = 0;
        __m128i fill_value = _mm_set1_epi8(static_cast<char>(new_char));
        for (; i + 16 <= fill_size; i += 16) {
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), fill_value);
        }
        for (; i < fill_size; ++i) {
            dst[i] = static_cast<uint8_t>(new_char);
        }
    }
    size_m = new_size;
    data_m[size_m] = '\0';
}


//! ПОК by Maksym Dzoban
//! Немає потреби у сімд
void my_str_simd_sse::clear() {
    size_m = 0;     //! розмір стрінги = 0
    data_m[size_m] = '\0';  //! + символ завершення
}

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

//! AKS by Maksym Dzo

void my_str_simd_sse::insert(size_t idx, const my_str_simd_sse& str) {
    if (idx > size_m) {
        throw std::out_of_range("index out of range");
    }

    if (str.size_m == 0) return;

    // 🔁 розширити буфер за потреби
    if (size_m + str.size_m + 1 > capacity_m) {
        reserve((size_m + str.size_m + 1) * 2);
    }

    size_t move_size = size_m - idx;

    // 🔁 зсунути вміст вправо (з кінця!)
    if (move_size > 0) {
        uint8_t* src = reinterpret_cast<uint8_t*>(data_m + idx);
        uint8_t* dst = reinterpret_cast<uint8_t*>(data_m + idx + str.size_m);

        size_t i = move_size;
        while (i >= 32) {
            i -= 32;
            __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
            __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i + 16));
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v0);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i + 16), v1);
        }
        while (i >= 16) {
            i -= 16;
            __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v);
        }
        while (i > 0) {
            --i;
            dst[i] = src[i];
        }
    }

    // 🧩 вставити нові символи
    const uint8_t* insert_src = reinterpret_cast<const uint8_t*>(str.data_m);
    uint8_t* insert_dst = reinterpret_cast<uint8_t*>(data_m + idx);

    size_t j = 0, copy_size = str.size_m;
    for (; j + 32 <= copy_size; j += 32) {
        __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(insert_src + j));
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(insert_src + j + 16));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(insert_dst + j), v0);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(insert_dst + j + 16), v1);
    }
    for (; j + 16 <= copy_size; j += 16) {
        __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(insert_src + j));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(insert_dst + j), v);
    }
    for (; j < copy_size; ++j) {
        insert_dst[j] = insert_src[j];
    }

    size_m += str.size_m;
    data_m[size_m] = '\0';
}


//! POK by Oksana Kotliarchuk
//! якусь букву на якусь позицію
// void my_str_simd::insert(size_t idx, char c) {
//     if (idx > size_m) {
//         throw std::out_of_range("index out of range");
//     }
//     if (size_m + 2 > capacity_m) {
//         reserve(size_m * 2 + 1);
//     }
//     std::memmove(data_m + idx + 1, data_m + idx, size_m - idx);
//     data_m[idx] = c;
//
//     ++size_m;
//     data_m[size_m] = '\0';
// }
void my_str_simd_sse::insert(size_t idx, char c) {
    if (idx > size_m) {
        throw std::out_of_range("index out of range");
    }

    if (size_m + 2 > capacity_m) {
        reserve(size_m * 2 + 1);
    }

    size_t move_size = size_m - idx;
    uint8_t* src = reinterpret_cast<uint8_t*>(data_m + idx);
    uint8_t* dst = reinterpret_cast<uint8_t*>(data_m + idx + 1);
    size_t j = move_size;
    while (j >= 32) {
        j -= 32;
        __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + j));
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + j + 16));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + j), v0);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + j + 16), v1);
    }
    while (j >= 16) {
        j -= 16;
        __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + j));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + j), v);
    }
    while (j > 0) {
        --j;
        dst[j] = src[j];
    }

    data_m[idx] = c;
    ++size_m;
    data_m[size_m] = '\0';
}

//! ПОК by Oksana Kotliarchuk
// void my_str_simd::insert(size_t idx, const char* cstr) {
//   	if (cstr == nullptr) {
//       throw std::logic_error("Null pointer passed to constructor");
//     }
//     if (idx > size_m) {
//         throw std::out_of_range("index out of range");
//     }
//     size_t const len = std::strlen(cstr);
//
//     if (size_m + len + 1 > capacity_m) {
//         reserve(2 * (size_m + len) + 1);
//     }
//     std::memmove(data_m + idx + len, data_m + idx, size_m - idx);
//     std::memcpy(data_m + idx, cstr, len);
//
//     size_m += len;
//     data_m[size_m] = '\0';
// }

//! АКС by Maksym Dzoban
//! додає до стрічки іншу стрічку на певну позицію

void my_str_simd_sse::insert(size_t idx, const char* cstr)
{
    if (cstr == nullptr) {
        throw std::logic_error("Null pointer passed to constructor");
    }
    if (idx > size_m) {
        throw std::out_of_range("index out of range");
    }

    const size_t len = std::strlen(cstr);
    if (size_m + len + 1 > capacity_m) {
        reserve(2 * (size_m + len) + 1);
    }

    // 🔁 Збережи cstr у тимчасовий буфер (щоб уникнути перезапису)
    char* temp = new char[len];
    std::memcpy(temp, cstr, len);

    const size_t move_size = size_m - idx;
    uint8_t* src = reinterpret_cast<uint8_t*>(data_m + idx);
    uint8_t* dst = reinterpret_cast<uint8_t*>(data_m + idx + len);

    size_t j = move_size;
    while (j >= 32) {
        j -= 32;
        __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + j));
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + j + 16));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + j), v0);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + j + 16), v1);
    }
    while (j >= 16) {
        j -= 16;
        __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + j));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + j), v);
    }
    while (j > 0) {
        --j;
        dst[j] = src[j];
    }

    // 🔁 Копіюємо з тимчасового буфера
    const uint8_t* src_insert = reinterpret_cast<const uint8_t*>(temp);
    dst = reinterpret_cast<uint8_t*>(data_m + idx);
    j = 0;

    for (; j + 32 <= len; j += 32) {
        __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_insert + j));
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_insert + j + 16));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + j), v0);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + j + 16), v1);
    }
    for (; j + 16 <= len; j += 16) {
        __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_insert + j));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + j), v);
    }
    for (; j < len; ++j) {
        dst[j] = src_insert[j];
    }

    size_m += len;
    data_m[size_m] = '\0';

    delete[] temp; // 🔁 звільняємо тимчасову пам’ять
}


void my_str_simd_sse::append(const my_str_simd_sse& str) {
    if (str.size_m == 0) return;
    if (size_m + str.size_m + 1 > capacity_m) {
        reserve(2 * (size_m + str.size_m) + 1);
    }

    uint8_t* dst = reinterpret_cast<uint8_t*>(data_m + size_m);
    const uint8_t* src = reinterpret_cast<const uint8_t*>(str.data_m);
    size_t copy_size = str.size_m;
    size_t i = 0;

    for (; i + 32 <= copy_size; i += 32) {
        __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i + 16));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v0);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i + 16), v1);
    }
    for (; i + 16 <= copy_size; i += 16) {
        __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v);
    }
    for (; i < copy_size; ++i) {
        dst[i] = src[i];
    }

    size_m += str.size_m;
    data_m[size_m] = '\0';
}


//! POK by Lev Lysyk
//! немає потреби у сімд, бо інсерт вже має сімд обчислення
void my_str_simd_sse::append(char c) {
    insert(size_m, c);
}

//! ПОК by Oksana Kotliarchuk
//! немає потреби у сімд, бо інсерт вже має сімд обчислення
// void my_str_simd_sse::append (const my_str_simd_sse& str) {
//     insert(size_m, str);
// }

//! ПОК by Oksana Kotliarchuk
//! немає потреби у сімд, бо інсерт вже має сімд обчислення
void my_str_simd_sse::append (const char* cstr) {
    insert(size_m, cstr);
}

//! ПОК by Oksana Kotliarchuk
// void my_str_simd::erase(size_t begin, size_t size) {
//     if (begin > size_m) {
//         throw std::out_of_range("index out of range");
//     }
//     size = (size + begin > size_m) ? size_m - begin : size;
//     std::memmove(data_m + begin, data_m + begin + size, size_m - (begin + size));
//     size_m -= size;
//     data_m[size_m] = '\0';
// }

//! AKS
void my_str_simd_sse::erase(size_t begin, size_t size) {
    if (begin > size_m) {
        throw std::out_of_range("index out of range");
    }
    size = (begin + size > size_m) ? (size_m - begin) : size;
    size_t move_size = size_m - (begin + size);
    if (move_size > 0) {
        uint8_t* src = reinterpret_cast<uint8_t*>(data_m + begin + size);
        uint8_t* dst = reinterpret_cast<uint8_t*>(data_m + begin);
        size_t i = 0;
        // блоками по 32 байти (2 x 16)
        for (; i + 32 <= move_size; i += 32) {
            __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
            __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i + 16));
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v0);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i + 16), v1);
        }

        // Копіюємо по 16 байт
        for (; i + 16 <= move_size; i += 16) {
            __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v);
        }
        for (; i < move_size; ++i) {
            dst[i] = src[i];
        }
    }
    size_m -= size;
    data_m[size_m] = '\0';
}


//! POK by Maksym Dzoban
//! окільки це геттери - сеттери то змін не потребують
const char* my_str_simd_sse::c_str() const { return data_m; }

//! POK by Maksym Dzoban
//! окільки це геттери - сеттери то змін не потребують
size_t  my_str_simd_sse::capacity() const  { return capacity_m; }

//! POK by Maksym Dzoban
//! окільки це геттери - сеттери то змін не потребують
size_t my_str_simd_sse::size() const { return size_m; }

//! POK by Lev Lysyk
// size_t my_str_simd::find(char c, size_t idx) const {
//     if (idx > size_m)
//     {
//         throw std::out_of_range("my_str_t::find");
//     }
//     for (size_t i = idx; i < size_m; ++i)
//     {
//         if (data_m[i] == c)
//         {
//             return i;
//         }
//     }
//     return not_found;
// }

size_t my_str_simd_sse::find(char c, size_t idx) const {
    if (idx > size_m) {
        throw std::out_of_range("my_str_simd_sse::find");
    }
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data_m);
    __m128i target_vector = _mm_set1_epi8(static_cast<char>(c));
    size_t i = idx;

    for (; i + 16 <= size_m; i += 16) {
        __m128i data_vector = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr + i));
        __m128i cmp = _mm_cmpeq_epi8(data_vector, target_vector);
        int mask = _mm_movemask_epi8(cmp);

        if (mask != 0) {
            // знайдено збіг: повертаємо перший індекс
            return i + __builtin_ctz(mask);
            // ctz = count trailing zeros
        }
    }
    // залишок без SIMD
    for (; i < size_m; ++i) {
        if (ptr[i] == static_cast<uint8_t>(c)) {
            return i;
        }
    }
    return not_found;
}



//! ПОК by Lev Lysyk
// size_t my_str_simd::find(const std::string& str, size_t idx) const {
//     if (idx > size_m)
//     {
//         throw std::out_of_range("my_str_t::find");
//     }
//     size_t substr_i = 0;
//     size_t const str_len = str.size();
//     for (size_t i = idx; i < size_m; ++i)
//     {
//         while (substr_i < str_len && data_m[i + substr_i] == str[substr_i])
//         {
//             ++substr_i;
//         }
//         if (substr_i == str_len)
//         {
//             return i;
//         }
//         substr_i = 0;
//     }
//     return not_found;
// }

//! AKS by Max Dzioban
size_t my_str_simd_sse::find(const std::string& str, size_t idx) const {
    if (idx > size_m) {
        throw std::out_of_range("my_str_simd_sse::find");
    }
    size_t str_len = str.size();
    if (str_len == 0) return idx;
    if (str_len > size_m - idx) return not_found;
    const uint8_t* src = reinterpret_cast<const uint8_t*>(data_m);
    __m128i target_vector = _mm_set1_epi8(static_cast<char>(str[0]));
    size_t i = idx;
    for (; i + 16 <= size_m; i += 16) {
        __m128i block = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        __m128i cmp = _mm_cmpeq_epi8(block, target_vector);
        int mask = _mm_movemask_epi8(cmp);

        while (mask != 0) {
            int offset = __builtin_ctz(mask);  // позиція першого збігу
            size_t candidate = i + offset;
            if (candidate + str_len <= size_m) {
                if (std::memcmp(data_m + candidate, str.data(), str_len) == 0) {
                    return candidate;
                }
            }
            // очистити перевірений біт
            mask &= (mask - 1);
        }
    }

    // залишок без SIMD
    for (; i <= size_m - str_len; ++i) {
        if (data_m[i] == str[0] &&
            std::memcmp(data_m + i, str.data(), str_len) == 0) {
            return i;
            }
    }

    return not_found;
}



//! POK by Lev Lysyk
// size_t my_str_simd::find(const char* cstr, size_t idx) const {
//     if (idx > size_m)
//     {
//         throw std::out_of_range("my_str_t::find");
//     }
//     size_t substr_i = 0;
//     size_t const str_len = std::strlen(cstr);
//     for (size_t i = idx; i < size_m; ++i)
//     {
//         while (substr_i < str_len && data_m[i + substr_i] == cstr[substr_i])
//         {
//             ++substr_i;
//         }
//         if (substr_i == str_len)
//         {
//             return i;
//         }
//         substr_i = 0;
//     }
//     return not_found;
// }

//! Aks version
size_t my_str_simd_sse::find(const char* cstr, size_t idx) const {
    if (cstr == nullptr) {
        throw std::logic_error("Null pointer passed to find()");
    }
    if (idx > size_m) {
        throw std::out_of_range("my_str_simd_sse::find");
    }

    size_t str_len = std::strlen(cstr);
    if (str_len == 0) return idx;
    if (str_len > size_m - idx) return not_found;

    const uint8_t* src = reinterpret_cast<const uint8_t*>(data_m);
    __m128i target_vector = _mm_set1_epi8(static_cast<char>(cstr[0]));

    size_t i = idx;

    // SIMD пошук першого символу
    for (; i + 16 <= size_m; i += 16) {
        __m128i block = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        __m128i cmp = _mm_cmpeq_epi8(block, target_vector);
        int mask = _mm_movemask_epi8(cmp);

        // для кожного знайденого байта
        while (mask != 0) {
            int offset = __builtin_ctz(mask);  // знайти позицію першого 1
            size_t candidate = i + offset;

            if (candidate + str_len <= size_m &&
                std::memcmp(data_m + candidate, cstr, str_len) == 0) {
                return candidate;
                }

            mask &= (mask - 1);  // обнулити наймолодший встановлений біт
        }
    }

    // залишок — посимвольно
    for (; i <= size_m - str_len; ++i) {
        if (data_m[i] == cstr[0] &&
            std::memcmp(data_m + i, cstr, str_len) == 0) {
            return i;
            }
    }

    return not_found;
}


//! POK by Lev Lysyk
//! робить підстрічку
// my_str_simd my_str_simd::substr(size_t begin, size_t size) const {
//     if (data_m == nullptr) {
//         throw std::logic_error("Null pointer passed to constructor");
//     }
//     if (begin > size_m)
//     {
//         throw std::out_of_range("Incorrect index!");
//     }
//     size_t const new_size_m = (size > size_m - begin) ? size_m - begin : size;
//     my_str_simd substring(new_size_m);
//     substring.size_m = new_size_m;
//     for (size_t i = 0; i < new_size_m; ++i)
//     {
//         substring[i] = data_m[begin + i];
//     }
//     substring.resize(new_size_m);
//     return substring;
// }

my_str_simd_sse my_str_simd_sse::substr(size_t begin, size_t size) const
{
    if (data_m == nullptr) {
        throw std::logic_error("Null pointer passed to substr()");
    }
    if (begin > size_m) {
        throw std::out_of_range("Index out of bounds in substr()");
    }
    size_t new_size_m = (size > size_m - begin) ? size_m - begin : size;
    my_str_simd_sse substring(new_size_m);  // викликає конструктор з prealloc
    const uint8_t* src = reinterpret_cast<const uint8_t*>(data_m + begin);
    uint8_t* dst = reinterpret_cast<uint8_t*>(substring.data_m);
    size_t i = 0;
    // блоками по 32 байти (2 x 16)
    for (; i + 32 <= new_size_m; i += 32)
    {
        __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i + 16));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v0);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i + 16), v1);
    }
    for (; i + 16 <= new_size_m; i += 16) {
        __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), v);
    }
    for (; i < new_size_m; ++i) {
        dst[i] = src[i];
    }
    substring.size_m = new_size_m;
    substring.data_m[new_size_m] = '\0';
    substring.shrink_to_fit();
    return substring;
}

//! POK by Maksym Dzoban
//! немає потреби у СІМД обчисленнчх
my_str_simd_sse::~my_str_simd_sse() {
    delete[] data_m;
}

//! POK by Lev Lysyk
//! немає потреби у СІМД обчисленнчх
std::ostream& operator<<(std::ostream& stream, const my_str_simd_sse& str)
{
    stream << str.c_str();
    return stream;
}

//! POK by Lev Lysyk
//! немає потреби у СІМД обчисленнчх
std::istream& operator>>(std::istream& stream, my_str_simd_sse& str) {
    while (std::isspace(stream.get())) {}
    stream.unget();
    str.clear();
    while (!std::isspace(stream.peek()) && !stream.eof() && !stream.fail())
    {
        str.append(static_cast<char>(stream.get()));
    }
    return stream;
}

//! POK by Lev Lysyk
//! немає потреби у СІМД обчисленнч
std::istream& readline(std::istream& stream, my_str_simd_sse& str) {
    str.clear();
    for (size_t i = 0; stream.peek() != '\n'; i++)
    {
        str.append(static_cast<char>(stream.get()));
    }
    return stream;
}

//! POK by Maksym Dzoban
//! приймає на всіх 2 обєкти мого класу і порівнює
// bool operator==(const my_str_simd &str1, const my_str_simd &str2) {
//     if (str1.size() != str2.size()) { return false; }
//     for (size_t i = 0; i < str1.size(); i++) {
//         if (str1[i] != str2[i]) { return false; }
//     }
//     return true;
// }

bool operator==(const my_str_simd_sse& str1, const my_str_simd_sse& str2) {
    if (str1.size() != str2.size()) {
        return false;
    }
    size_t size = str1.size();
    const uint8_t* p1 = reinterpret_cast<const uint8_t*>(str1.c_str());
    const uint8_t* p2 = reinterpret_cast<const uint8_t*>(str2.c_str());

    size_t i = 0;
    for (; i + 32 <= size; i += 32) {
        __m128i v1_0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1 + i));
        __m128i v2_0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2 + i));
        __m128i cmp0 = _mm_cmpeq_epi8(v1_0, v2_0);
        int mask0 = _mm_movemask_epi8(cmp0);

        __m128i v1_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1 + i + 16));
        __m128i v2_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2 + i + 16));
        __m128i cmp1 = _mm_cmpeq_epi8(v1_1, v2_1);
        int mask1 = _mm_movemask_epi8(cmp1);

        if (mask0 != 0xFFFF || mask1 != 0xFFFF) {
            return false;
        }
    }
    for (; i + 16 <= size; i += 16) {
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1 + i));
        __m128i v2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2 + i));
        __m128i cmp = _mm_cmpeq_epi8(v1, v2);
        int mask = _mm_movemask_epi8(cmp);
        if (mask != 0xFFFF) {
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

//! POK by Maksym Dzoban
//! приймає на вхід 1 обєкт мого класу і 1 с стрічку
// bool operator==(const my_str_simd& str1, const char* cstr2) {
//     if (str1.size() != std::strlen(cstr2)) { return false; }
//     for (size_t i = 0; i < str1.size(); i++) {
//         if (str1[i] != cstr2[i]) { return false; }
//     }
//     return true;
// }

//! AKS SIMD by Maksym Dzoban
//! приймає на всіх 1 обєкс мого класу і 1 с стрічку
bool operator==(const my_str_simd_sse& str1, const char* cstr2) {
    if (!cstr2) {
        return false;
    }
    size_t size = str1.size();
    if (std::strlen(cstr2) != size) {
        return false;
    }
    const uint8_t* p1 = reinterpret_cast<const uint8_t*>(str1.c_str());
    const uint8_t* p2 = reinterpret_cast<const uint8_t*>(cstr2);
    size_t i = 0;
    for (; i + 32 <= size; i += 32) {
        __m128i v1_0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1 + i));
        __m128i v2_0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2 + i));
        __m128i cmp0 = _mm_cmpeq_epi8(v1_0, v2_0);
        if (_mm_movemask_epi8(cmp0) != 0xFFFF) {
            return false;
        }
        __m128i v1_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1 + i + 16));
        __m128i v2_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2 + i + 16));
        __m128i cmp1 = _mm_cmpeq_epi8(v1_1, v2_1);
        if (_mm_movemask_epi8(cmp1) != 0xFFFF) {
            return false;
        }
    }
    for (; i + 16 <= size; i += 16) {
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1 + i));
        __m128i v2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2 + i));
        __m128i cmp = _mm_cmpeq_epi8(v1, v2);
        if (_mm_movemask_epi8(cmp) != 0xFFFF) {
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


//! POK by Maksym Dzoban
//! приймає на вхід 1 c string  і 1 екземпляр мого класу
// bool operator==(const char* cstr1, const my_str_simd& str2) {
//     if (std::strlen(cstr1) != str2.size()) { return false; }
//     for (size_t i = 0; i < str2.size(); i++) {
//         if (cstr1[i] != str2[i]) { return false; }
//     }
//     return true;}

bool operator==(const char* cstr1, const my_str_simd_sse& str2) {
    if (!cstr1) return false;

    size_t size = str2.size();
    if (std::strlen(cstr1) != size) {
        return false;
    }

    const uint8_t* p1 = reinterpret_cast<const uint8_t*>(cstr1);
    const uint8_t* p2 = reinterpret_cast<const uint8_t*>(str2.c_str());

    size_t i = 0;
    for (; i + 32 <= size; i += 32) {
        __m128i a0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1 + i));
        __m128i b0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2 + i));
        __m128i cmp0 = _mm_cmpeq_epi8(a0, b0);
        if (_mm_movemask_epi8(cmp0) != 0xFFFF) return false;

        __m128i a1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1 + i + 16));
        __m128i b1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2 + i + 16));
        __m128i cmp1 = _mm_cmpeq_epi8(a1, b1);
        if (_mm_movemask_epi8(cmp1) != 0xFFFF) return false;
    }
    for (; i + 16 <= size; i += 16)
    {
        __m128i a = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1 + i));
        __m128i b = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2 + i));
        __m128i cmp = _mm_cmpeq_epi8(a, b);
        if (_mm_movemask_epi8(cmp) != 0xFFFF) return false;
    }
    for (; i < size; ++i) {
        if (p1[i] != p2[i]) return false;
    }
    return true;
}

//! POK  by Maksym Dzoban
//! SIMD не потрібен тут Dzoban
bool operator!=(const my_str_simd_sse & str1, const my_str_simd_sse& str2) {
    return !(str1==str2);
}

//! POK  by Maksym Dzoban
//! SIMD не потрібен тут
bool operator!=(const my_str_simd_sse& str1, const char* cstr2) {
    return !(str1==cstr2);
}

//! POK by Maksym Dzoban
//! SIMD не потрібен тут
bool operator!=(const char* cstr1, const my_str_simd_sse& str2) {
    return !(cstr1==str2);
}

//! POK by Oksana Kotliarchuk
//! SIMD не потрібен тут
bool operator< (const my_str_simd_sse& str1, const my_str_simd_sse& str2) {
  if (str1.size() == str2.size()) {
    int i = 0;
    while (i < str1.size() && str1.c_str()[i] == str2.c_str()[i]) {
      i++;
      }
    if (i < str1.size()) {
      return str1.c_str()[i] < str2.c_str()[i];
    }
      return false;
  }
  return str1.size() < str2.size();
}

//! POK by Oksana Kotliarchuk
//! SIMD не потрібен тут
bool operator< (const my_str_simd_sse& str1, const char* cstr2) {
  if (str1.size() == strlen(cstr2)) {
    int i = 0;
    while (i < str1.size() && str1.c_str()[i] == cstr2[i]) {
      i++;
    }
    if (i < str1.size()) {
      return str1.c_str()[i] < cstr2[i];
    }
      return false;
  }
  return str1.size() < strlen(cstr2);
}

//! POK by Oksana Kotliarchuk
//! SIMD не потрібен тут
bool operator< (const char* cstr1, const my_str_simd_sse& str2) {
    if (str2.size() == strlen(cstr1)) {
        int i = 0;
        while (i < str2.size() && str2.c_str()[i] == cstr1[i]) {
            i++;
        }
        if (i < str2.size()) {
            return str2.c_str()[i] > cstr1[i];
        }
          return false;
    }
    return str2.size() > strlen(cstr1);
}

//! ПОК by Oksana Kotliarchuk
//! Ніякої SIMD оптимізації тут не потрібно
bool operator> (const my_str_simd_sse & str1, const my_str_simd_sse & str2) {
  return (str2 < str1);
}

//! ПОК by Oksana Kotliarchuk
//! Ніякої SIMD оптимізації тут не потрібно
bool operator> (const my_str_simd_sse& str1, const char* cstr2) {
  return (cstr2 < str1);
}

//! ПОК by Oksana Kotliarchuk
//! Ніякої SIMD оптимізації тут не потрібно
bool operator> (const char* cstr1, const my_str_simd_sse& str2) {
  return (str2 < cstr1);
}

//! ПОК by Oksana Kotliarchuk
//! Ніякої SIMD оптимізації тут не потрібно
bool operator<= (const my_str_simd_sse& str1, const my_str_simd_sse& str2) {
  return not (str1 > str2);
}

//! ПОК by Oksana Kotliarchuk
//! Ніякої SIMD оптимізації тут не потрібно
bool operator<= (const my_str_simd_sse& str1, const char* cstr2) {
  return not (str1 > cstr2);
}

//! ПОК by Oksana Kotliarchuk
//! Ніякої SIMD оптимізації тут не потрібно
bool operator<= (const char* cstr1, const my_str_simd_sse& str2) {
  return not (cstr1 > str2);
}

//! ПОК by Oksana Kotliarchuk
//! Ніякої SIMD оптимізації тут не потрібно
bool operator>= (const my_str_simd_sse& str1, const my_str_simd_sse& str2) {
  return not (str1 < str2);
}

//! ПОК by Oksana Kotliarchuk
//! Ніякої SIMD оптимізації тут не потрібно
bool operator>=(const my_str_simd_sse& str1, const char* cstr2) {
  return not (str1 < cstr2);
}

//! ПОК by Oksana Kotliarchuk
//! Ніякої SIMD оптимізації тут не потрібно
bool operator>= (const char* cstr1, const my_str_simd_sse& str2) {
  return not (cstr1 < str2);
}

//! POK by Lev Lysyk
//! append реалізований через SIMD
my_str_simd_sse& my_str_simd_sse::operator+=(const char chr) {
    append(chr);
    return *this;
}

//! POK by Lev Lysyk
//! append реалізований через SIMD
my_str_simd_sse& my_str_simd_sse::operator+=(const my_str_simd_sse& mystr) {
    append(mystr);
    return *this;
}

//! POK by Lev Lysyk
//! append реалізований через SIMD
my_str_simd_sse& my_str_simd_sse::operator+=(const char* mystr) {
    append(mystr);
    return *this;
}

//! POK by Maksym Dzoban
//! SIMD не потр. бо оператор лише склеює дві великі операції разом
my_str_simd_sse operator+(const my_str_simd_sse& mystr, const my_str_simd_sse& mystr2) {
    my_str_simd_sse temp(mystr);
    temp += mystr2;
    return temp;
}

//! POK by Maksym Dzoban
//! SIMD не потр. бо оператор лише склеює дві великі операції разом
my_str_simd_sse operator+(const my_str_simd_sse& mystr, const char* mystr2) {
    my_str_simd_sse temp(mystr);
    temp += mystr2;
    return temp;
}

// by Maksym Dzoban
my_str_simd_sse operator+(const char* mystr, const my_str_simd_sse& mystr2) {
    return mystr2 + mystr;
}

// by Maksym Dzoban
my_str_simd_sse operator+(const my_str_simd_sse& mystr, const char chr) {
    my_str_simd_sse temp(mystr);
    temp += chr;
    return temp;
}

// by Lev Lysyk
my_str_simd_sse operator+(const char chr, const my_str_simd_sse& mystr) {
    return mystr + chr;
}

//! ПОК by Lev Lysyk
//! reserve реалізований через SIMD
//! append реалізований через SIMD
my_str_simd_sse& my_str_simd_sse::operator*=(int const count) {
    if (count < 0)
    {
        throw(std::invalid_argument("my_str_t::operator*=(int const count)"));
    }
    reserve(2 * (size_m * count) + 1);
    my_str_simd_sse const str_copy(*this);
    for(int i = 1; i < count; ++i)
    {
        append(str_copy);
    }
    size_m = str_copy.size_m * count;
    data_m[size_m] = '\0';
    return *this;
};

//! ПОК by Lev Lysyk
//! reserve реалізований через SIMD
//! append реалізований через SIMD
my_str_simd_sse operator*(const my_str_simd_sse& str1, int const count) {
    if (count < 0)
    {
        throw(std::invalid_argument("my_str_t::operator*=(int const count)"));
    }
    my_str_simd_sse result(str1);
    result.reserve(2 * (result.size() * count) + 1);
    for (int i = 1; i < count; ++i)
    {
        result.append(str1);
    }
    result.resize(str1.size() * count);
    return result;
}

//! ПОК by Lev Lysyk
//! один редірект функції, тому без сімд
my_str_simd_sse operator*(int const count, const my_str_simd_sse& str1) {
    return str1 * count;
}

//! POK by Oksana Kotliarchuk
//! Ніякої SIMD оптимізації тут не потрібно
my_str_simd_sse::my_str_simd_sse(my_str_simd_sse&& other_str) noexcept :
        data_m(other_str.data_m), capacity_m(other_str.capacity_m)
        , size_m(other_str.size_m) {
    other_str.data_m = nullptr;
    other_str.size_m = 0;
    other_str.capacity_m = 0;
}

//! ПОК by Oksana Kotliarchuk
//! Немає сенсу у сімд
my_str_simd_sse& my_str_simd_sse::operator=(my_str_simd_sse&& other_str) noexcept {
    if (this == &other_str) {
        return *this;
    }
    delete[] data_m;
    data_m = other_str.data_m;
    size_m = other_str.size_m;
    capacity_m = other_str.capacity_m;
    other_str.data_m = nullptr;
    other_str.size_m = 0;
    other_str.capacity_m = 0;
    return *this;
}
