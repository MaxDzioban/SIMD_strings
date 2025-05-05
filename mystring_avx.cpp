#ifdef __amd64__

#include "immintrin.h"
#include "mystring_avx.h"
#include <iostream>
#include <cstring>

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

my_str_avx::my_str_avx(size_t const capacity):
        capacity_m(capacity * 2 + 1), size_m(0) {
    data_m = new char[capacity_m];
    data_m[0] = '\0';
}

my_str_avx::my_str_avx(size_t size, char initial)
: capacity_m(size * 2 + 1), size_m(size) {
    data_m = new char[capacity_m];
    __m256i temp = _mm256_set1_epi8(initial);
    size_t i=0;
    for (; i+32 <= size_m; i+=32) {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data_m + i), temp);
    }
    for (; i < size_m; ++i) {
        data_m[i] = initial;
    }
}

my_str_avx::my_str_avx(const char* cstr) {
    if (cstr == nullptr) {
        throw std::logic_error("Null pointer passed to constructor");
    }
    size_m = std::strlen(cstr); //!обчислює довжину стрічки cstr, не враховуючи нульовий символ завершення
    capacity_m = 2 *size_m + 1; //! враховуємо символ завершення
    data_m = new char[capacity_m]; //! виділили память довж size_m

    avx2_memcpy(cstr, data_m, size_m);

    data_m[size_m] = '\0';
}

my_str_avx::my_str_avx(const std::string& str) {
    size_m = str.size();    // Зберігаємо розмір стрічки
    capacity_m = size_m * 2 + 1;    // Виділяємо місце під символ '\0'
    data_m = new char[capacity_m];  // Виділяємо пам'ять

    const char* src = str.data();
    avx2_memcpy(src, data_m, size_m);

    data_m[size_m] = '\0';
}

my_str_avx::my_str_avx(const my_str_avx& my_str) 
: capacity_m(my_str.capacity_m), size_m(my_str.size_m) {
    data_m = new char[capacity_m];  // Виділяємо пам'ять

    const char* src = my_str.data_m;
    avx2_memcpy(src, data_m, size_m);

    data_m[size_m] = '\0';
}

my_str_avx& my_str_avx::operator=(const my_str_avx& mystr) {
    if (this == &mystr) {
        return *this;
    }
    delete[] data_m;
    size_m = mystr.size_m;
    capacity_m = mystr.capacity_m;
    data_m = new char[capacity_m];

    const char* src = mystr.data_m;
    avx2_memcpy(src, data_m, size_m);

    data_m[size_m] = '\0';
    return *this;
}

void my_str_avx::swap(my_str_avx& other) noexcept {
    std::swap(data_m, other.data_m);
    std::swap(capacity_m, other.capacity_m);
    std::swap(size_m, other.size_m);
}

char& my_str_avx::operator[](size_t idx) {
    if (idx>= size_m)
    {throw std::out_of_range("Incorrect index!");}
    return data_m[idx];
}

const char& my_str_avx::operator[](size_t idx) const {
    if (idx>= size_m)
    {throw std::out_of_range("Incorrect index!");}
    return data_m[idx];
}

char& my_str_avx:: at(size_t idx) {
    if (idx >= size_m) {
        throw std::out_of_range("Incorrect index!");
    }
    return data_m[idx];
}

const char& my_str_avx:: at(size_t idx) const {
    if (idx >= size_m) {
        throw std::out_of_range("Incorrect index!");
    }
    return data_m[idx];
}

void my_str_avx::reserve(size_t new_capacity) {
    if (new_capacity > capacity_m){
        capacity_m = new_capacity;
        char* new_data_m = new char[capacity_m];

        const char* src = data_m;
        char* dst = new_data_m;
        avx2_memcpy(src, dst, size_m);

        delete[] data_m;
        data_m = new_data_m;
        data_m[size_m] = '\0';
    }
}

void my_str_avx:: shrink_to_fit() {
    capacity_m = size_m + 1;
    char* data = new char[capacity_m];

    const char* src = data_m;
    char* dst = data;
    avx2_memcpy(src, dst, size_m);

    delete[] data_m;
    data_m = data;
    data_m[size_m] = '\0';
}

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

void my_str_avx::clear() {
    size_m = 0;     //! розмір стрінги = 0
    data_m[size_m] = '\0';  //! + символ завершення
}

void my_str_avx::insert(size_t idx, const my_str_avx& str) {
    if (idx > size_m) {
        throw std::out_of_range("index out of range");
    }

    if (size_m + str.size_m + 1 > capacity_m) {
        reserve(2 * (size_m + str.size_m) + 1);
    }

    size_t move_size = size_m - idx;
    const char* src = data_m + idx;
    char* dst = data_m + idx + str.size_m;

    if (move_size > 0){
        size_t i = move_size;
        while (i>=32) {
            i-=32;
            __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src + i));
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst + i), block);
        }
        while (i > 0) {
            --i;
            dst[i] = src[i];
        }
    }
    src = str.data_m;
    dst = data_m + idx;
    size_t copy_size = str.size_m;
    size_t j = 0;
    for (; j + 32 <= copy_size; j += 32) {
        __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src + j));
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst + j), block);
    }
    for (; j < copy_size; ++j) {
        dst[j] = src[j];
    }

    size_m += str.size_m;
    data_m[size_m] = '\0';
}

void my_str_avx::insert(size_t idx, char c) {
    if (idx > size_m) {
        throw std::out_of_range("index out of range");
    }

    if (size_m + 2 > capacity_m) {
        reserve(2 * size_m + 1);
    }

    size_t move_size = size_m - idx;
    const char* src = data_m + idx;
    char* dst = data_m + idx + 1;
    if (move_size > 0){
        size_t i = move_size;
        while (i>=32) {
            i-=32;
            __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src + i));
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst + i), block);
        }
        while (i > 0) {
            --i;
            dst[i] = src[i];
        }
    }
    data_m[idx] = c;
    ++size_m;
    data_m[size_m] = '\0';
}

void my_str_avx::insert(size_t idx, const char* cstr) {
    if (cstr == nullptr) {
        throw std::logic_error("Null pointer passed to constructor");
    }
    if (idx > size_m) {
        throw std::out_of_range("index out of range");
    }
    size_t len = std::strlen(cstr);
    if (size_m + len + 1 > capacity_m) {
        reserve(2 * (size_m + len) + 1);
    }

    size_t move_size = size_m - idx;
    const char* src = data_m + idx;
    char* dst = data_m + idx + len;

    if (move_size > 0){
        size_t i = move_size;
        while (i>=32) {
            i-=32;
            __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src + i));
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst + i), block);
        }
        while (i > 0) {
            --i;
            dst[i] = src[i];
        }
    }
    dst = data_m + idx;
    size_t copy_size = std::strlen(cstr);
    size_t j = 0;
    for (; j + 32 <= copy_size; j += 32) {
        __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(cstr + j));
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst + j), block);
    }
    for (; j < copy_size; ++j) {
        dst[j] = cstr[j];
    }

    size_m += len;
    data_m[size_m] = '\0';
}

void my_str_avx::append(char c) {
    insert(size_m, c);
}

void my_str_avx::append (const my_str_avx& str) {
    insert(size_m, str);
}

void my_str_avx::append (const char* cstr) {
    insert(size_m, cstr);
}

void my_str_avx::erase(size_t begin, size_t size) {
    if (begin > size_m) {
        throw std::out_of_range("index out of range");
    }
    size = (size + begin > size_m) ? (size_m - begin) : size;
    size_t move_size = size_m - (begin + size);
    if (move_size > 0) {
        char* src = data_m + begin + size;
        char* dst = data_m + begin;
        avx2_memcpy(src, dst, move_size);
    }
    size_m -= size;
    data_m[size_m] = '\0';
}

const char* my_str_avx::c_str() const { return data_m; }

size_t  my_str_avx::capacity() const  { return capacity_m; }

size_t my_str_avx::size() const { return size_m; }

size_t my_str_avx::find(char c, size_t idx) const {
    if (idx > size_m) {
        throw std::out_of_range("my_str_avx::find");
    }
    const char* ptr = data_m;
    __m256i vec = _mm256_set1_epi8(c);
    size_t i = idx;
    for (; i + 32 <= size_m; i += 32) {
        __m256i data_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + i));
        __m256i cmp = _mm256_cmpeq_epi8(data_vec, vec); // use 'vec' here

        int mask = _mm256_movemask_epi8(cmp);
        if (mask != 0) {
            return i + __builtin_ctz(mask);
        }
    }
    for (; i < size_m; ++i) {
        if (ptr[i] == c) { // use 'c' here
            return i;
        }
    }
    return not_found;
}

size_t my_str_avx::find(const std::string& str, size_t idx) const {
    if (idx > size_m) {
        throw std::out_of_range("my_str_avx::find");
    }

    size_t str_len = str.size();
    if (str_len == 0) return idx;
    if (str_len > size_m - idx) return not_found;

    const char* src = data_m;
    char target = str[0];
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
                std::memcmp(src + candidate, str.data(), str_len) == 0) {
                return candidate;
            }

            mask &= (mask - 1);
        }
    }

    for (; i <= size_m - str_len; ++i) {
        if (src[i] == target &&
            // Використовуємо memcmp для швидкодії. Інший варіант писати свій аналог що буде повільнішим
            std::memcmp(src + i, str.data(), str_len) == 0) {
            return i;
        }
    }

    return not_found;
}

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

my_str_avx my_str_avx::substr(size_t begin, size_t size) const {
    if (data_m == nullptr) {
        throw std::logic_error("Null pointer passed to constructor");
    }
    if (begin > size_m)
    {
        throw std::out_of_range("Incorrect index!");
    }
    size_t const new_size_m = (size > size_m - begin) ? size_m - begin : size;
    my_str_avx substring(new_size_m);
    substring.size_m = new_size_m;
    
    const char* src = data_m + begin;
    char* dst = substring.data_m;

    avx2_memcpy(src,dst, new_size_m);
    substring.data_m[new_size_m] = '\0';
    substring.shrink_to_fit();

    return substring;
}

my_str_avx::~my_str_avx() {
    delete[] data_m;
}

std::ostream& operator<<(std::ostream& stream, const my_str_avx& str) {
    stream << str.c_str();
    return stream;
}

std::istream& operator>>(std::istream& stream, my_str_avx& str) {
    while (std::isspace(stream.get())) {}
    stream.unget();
    str.clear();
    while (!std::isspace(stream.peek()) && !stream.eof() && !stream.fail())
    {
        str.append(static_cast<char>(stream.get()));
    }
    return stream;
}

std::istream& readline(std::istream& stream, my_str_avx& str) {
    str.clear();
    for (size_t i = 0; stream.peek() != '\n'; i++)
    {
        str.append(static_cast<char>(stream.get()));
    }
    return stream;
}

bool operator==(const my_str_avx &str1, const my_str_avx &str2) {
    if (str1.size() != str2.size()) {
        return false;
    }
    size_t size = str1.size();
    const char* p1 = str1.c_str();
    const char* p2 = str2.c_str();

    size_t i = 0;
    for (; i + 32 <= size; i += 32) {
        __m256i v1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p1 + i));
        __m256i v2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p2 + i));
        __m256i cmp = _mm256_cmpeq_epi8(v1, v2);
        if (_mm256_movemask_epi8(cmp) != -1) { // all bits must be 1 (== 0xFFFFFFFF)
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

bool operator==(const my_str_avx &str1, const char* cstr2) {
    size_t size = str1.size();
    if (size != std::strlen(cstr2)) {
        return false;
    }
    
    const char* p1 = str1.c_str();
    const char* p2 = cstr2;

    size_t i = 0;
    for (; i + 32 <= size; i += 32) {
        __m256i v1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p1 + i));
        __m256i v2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p2 + i));
        __m256i cmp = _mm256_cmpeq_epi8(v1, v2);
        if (_mm256_movemask_epi8(cmp) != -1) { // all bits must be 1 (== 0xFFFFFFFF)
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

bool operator==(const char* cstr1, const my_str_avx& str2) {
    size_t size = str2.size();
    if (std::strlen(cstr1) != size) {
        return false;
    }
    
    const char* p1 = cstr1;
    const char* p2 = str2.c_str();

    size_t i = 0;
    for (; i + 32 <= size; i += 32) {
        __m256i v1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p1 + i));
        __m256i v2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p2 + i));
        __m256i cmp = _mm256_cmpeq_epi8(v1, v2);
        if (_mm256_movemask_epi8(cmp) != -1) {
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
bool operator!=(const my_str_avx& str1, const my_str_avx& str2) {
    return !(str1==str2);
}

bool operator!=(const my_str_avx& str1, const char* cstr2) {
    return !(str1==cstr2);
}

bool operator!=(const char* cstr1, const my_str_avx& str2) {
    return !(cstr1==str2);
}

bool operator< (const my_str_avx& str1, const my_str_avx& str2) {
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

bool operator< (const my_str_avx& str1, const char* cstr2) {
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

bool operator< (const char* cstr1, const my_str_avx& str2) {
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

bool operator> (const my_str_avx& str1, const my_str_avx& str2) {
  return (str2 < str1);
}

bool operator> (const my_str_avx& str1, const char* cstr2) {
  return (cstr2 < str1);
}

bool operator> (const char* cstr1, const my_str_avx& str2) {
  return (str2 < cstr1);
}

bool operator<= (const my_str_avx& str1, const my_str_avx& str2) {
  return not (str1 > str2);
}

bool operator<= (const my_str_avx& str1, const char* cstr2) {
  return not (str1 > cstr2);
}

bool operator<= (const char* cstr1, const my_str_avx& str2) {
  return not (cstr1 > str2);
}

bool operator>= (const my_str_avx& str1, const my_str_avx& str2) {
  return not (str1 < str2);
}

bool operator>=(const my_str_avx& str1, const char* cstr2) {
  return not (str1 < cstr2);
}

bool operator>= (const char* cstr1, const my_str_avx& str2) {
  return not (cstr1 < str2);
}

my_str_avx& my_str_avx::operator+=(const char chr) {
    append(chr);
    return *this;
}

my_str_avx& my_str_avx::operator+=(const my_str_avx& mystr) {
    append(mystr);
    return *this;
}

my_str_avx& my_str_avx::operator+=(const char* mystr) {
    append(mystr);
    return *this;
}

my_str_avx operator+(const my_str_avx& mystr, const my_str_avx& mystr2) {
    my_str_avx temp(mystr);
    temp += mystr2;
    return temp;
}

my_str_avx operator+(const my_str_avx& mystr, const char* mystr2) {
    my_str_avx temp(mystr);
    temp += mystr2;
    return temp;
}

my_str_avx operator+(const char* mystr, const my_str_avx& mystr2) {
    return mystr2 + mystr;
}

my_str_avx operator+(const my_str_avx& mystr, const char chr) {
    my_str_avx temp(mystr);
    temp += chr;
    return temp;
}

my_str_avx operator+(const char chr, const my_str_avx& mystr) {
    return mystr + chr;
}

my_str_avx& my_str_avx::operator*=(int const count) {
    if (count < 0)
    {
        throw(std::invalid_argument("my_str_t::operator*=(int const count)"));
    }
    reserve(2 * (size_m * count) + 1);
    my_str_avx const str_copy(*this);
    for(int i = 1; i < count; ++i)
    {
        append(str_copy);
    }
    size_m = str_copy.size_m * count;
    data_m[size_m] = '\0';
    return *this;
};

my_str_avx operator*(const my_str_avx& str1, int const count) {
    if (count < 0)
    {
        throw(std::invalid_argument("my_str_t::operator*=(int const count)"));
    }
    my_str_avx result(str1);
    result.reserve(2 * (result.size() * count) + 1);
    for (int i = 1; i < count; ++i)
    {
        result.append(str1);
    }
    result.resize(str1.size() * count);
    return result;
}

my_str_avx operator*(int const count, const my_str_avx& str1) {
    return str1 * count;
}

my_str_avx::my_str_avx(my_str_avx&& other_str) noexcept :
        data_m(other_str.data_m), capacity_m(other_str.capacity_m)
        , size_m(other_str.size_m) {
    other_str.data_m = nullptr;
    other_str.size_m = 0;
    other_str.capacity_m = 0;
}

my_str_avx& my_str_avx::operator=(my_str_avx&& other_str) noexcept {
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
#else

#error "AVX2 not supported by the compiler"

#endif