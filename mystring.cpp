// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <iostream>
#include "mystring.hpp"
#include <cstring>

// by Lev Lysyk
//! допоміжний конструктор
my_str_t::my_str_t(size_t const capacity):
        capacity_m(capacity * 2 + 1), size_m(0) {
    data_m = new char[capacity_m];
    data_m[0] = '\0';
}

// by Maksym Dzoban
//! Створює стрічку із size копій літери initial
my_str_t::my_str_t(size_t size, char initial):
    capacity_m(size*2 + 1), size_m(size) {
    data_m = new char[capacity_m]; //! виділили память довжиною capacity_m
    std::memset(data_m, initial, size); //!memset заповнює перші
    data_m[size_m] = '\0'; //! на позицію останнього символа '\0', щоб була С стрічка
}

// by Maksym Dzoban
//! Копіює вміст С-стрічки, вимоги до capacity_m -- ті ж, що вище
my_str_t::my_str_t(const char* cstr) {
  	// null pointer  -> logic error
    if (cstr == nullptr) {
      throw std::logic_error("Null pointer passed to constructor");
    }
    size_m = std::strlen(cstr); //!обчислює довжину стрічки cstr, не враховуючи нульовий символ завершення
    capacity_m = 2 *size_m + 1; //! враховуємо символ завершення
    data_m = new char[capacity_m]; //! виділили память довж size_m
    std::strcpy(data_m, cstr); //! перекопіювали
}

// by Maksym Dzoban
//! Копіює стрічку С++, вимоги до capacity_m -- ті ж, що вище
my_str_t::my_str_t(const std::string& str) {
    size_m = str.size();    // Зберігаємо розмір стрічки
    capacity_m = size_m * 2 + 1;    // Виділяємо місце під символ '\0'
    data_m = new char[capacity_m];  // Виділяємо пам'ять
    std::strcpy(data_m, str.c_str());   // Копіюємо вміст std::string у data_m
    data_m[size_m] = '\0';
}

// by Maksym Dzoban
//! конструктор копій
my_str_t::my_str_t(const my_str_t & mystr):
    capacity_m(mystr.capacity_m),size_m(mystr.size_m) {
    data_m = new char[capacity_m];
    std::memcpy(data_m, mystr.data_m, size_m);
    data_m[size_m] = '\0';
}

// by Maksym Dzoban
//!оператор присвоєння
my_str_t& my_str_t::operator=(const my_str_t& mystr) {
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

// by Lev Lysyk
void my_str_t::swap(my_str_t& other) noexcept {
    std::swap(data_m, other.data_m);
    std::swap(capacity_m, other.capacity_m);
    std::swap(size_m, other.size_m);
}

// by Maksym Dzoban
//! непостійний оператор індексації, повертає посилання на символ,
//! що можна змінити.
char& my_str_t::operator[](size_t idx) {
    if (idx>= size_m)
    {throw std::out_of_range("Incorrect index!");}
    return data_m[idx];
}

// by Maksym Dzoban
//! постійний оператор індексування,що використовується для постійних обєктів
const char& my_str_t::operator[](size_t idx) const {
    if (idx>= size_m)
    {throw std::out_of_range("Incorrect index!");}
    return data_m[idx];
}

// by Oksana Kotliarchuk
char& my_str_t:: at(size_t idx) {
    if (idx >= size_m) {
        throw std::out_of_range("Incorrect index!");
    }
    return data_m[idx];
}

// by Oksana Kotliarchuk
const char& my_str_t:: at(size_t idx) const {
    if (idx >= size_m) {
        throw std::out_of_range("Incorrect index!");
    }
    return data_m[idx];
}

// by Lev Lysyk
void my_str_t::reserve(size_t new_capacity) {
    if (new_capacity > capacity_m) {
        capacity_m = new_capacity;
        char* new_data_m = new char[capacity_m];
        std::memcpy(new_data_m, data_m, size_m);
        delete[] data_m;
        data_m = new_data_m;
        data_m[size_m] = '\0';
    }
}

// by Oksana Kotliarchuk
void my_str_t:: shrink_to_fit() {
    capacity_m = size_m + 1;
    char* data = new char[capacity_m];
    std::memcpy(data, data_m, size_m);
    delete[] data_m;
    data_m = data;
    data_m[size_m] = '\0';
}

// by Lev Lysyk
void my_str_t::resize(size_t new_size, char new_char) {
    if (new_size > size_m) {
        if (new_size > capacity_m) {
            reserve(new_size * 2);
        }
        memset(data_m + size_m, new_char, new_size - size_m);
    }
    size_m = new_size;
    data_m[size_m] = '\0';
}

// by Maksym Dzoban
void my_str_t::clear() {
    size_m = 0;     //! розмір стрінги = 0
    data_m[size_m] = '\0';  //! + символ завершення
}

// by Oksana Kotliarchuk
void my_str_t::insert(size_t idx, const my_str_t& str) {
    if (idx > size_m) {
        throw std::out_of_range("index out of range");
    }

    if (size_m + str.size_m + 1 > capacity_m) {
        reserve(2 * (size_m + str.size_m) + 1);
    }
    std::memmove(data_m + idx + str.size_m, data_m + idx, size_m - idx);
    std::memcpy(data_m + idx, str.data_m, str.size_m);

    size_m += str.size_m;
    data_m[size_m] = '\0';
}

// by Oksana Kotliarchuk
void my_str_t::insert(size_t idx, char c) {

    if (idx > size_m) {
        throw std::out_of_range("index out of range");
    }
    if (size_m + 2 > capacity_m) {
        reserve(size_m * 2 + 1);
    }
    std::memmove(data_m + idx + 1, data_m + idx, size_m - idx);
    data_m[idx] = c;

    ++size_m;
    data_m[size_m] = '\0';
}

// by Oksana Kotliarchuk
void my_str_t::insert(size_t idx, const char* cstr) {
  	if (cstr == nullptr) {
      throw std::logic_error("Null pointer passed to constructor");
    }
    if (idx > size_m) {
        throw std::out_of_range("index out of range");
    }
    size_t const len = std::strlen(cstr);

    if (size_m + len + 1 > capacity_m) {
        reserve(2 * (size_m + len) + 1);
    }
    std::memmove(data_m + idx + len, data_m + idx, size_m - idx);
    std::memcpy(data_m + idx, cstr, len);

    size_m += len;
    data_m[size_m] = '\0';
}

// by Lev Lysyk
void my_str_t::append(char c) {
    insert(size_m, c);
}

// by Oksana Kotliarchuk
void my_str_t::append (const my_str_t& str) {
    insert(size_m, str);
}

// by Oksana Kotliarchuk
void my_str_t::append (const char* cstr) {
    insert(size_m, cstr);
}

// by Oksana Kotliarchuk
void my_str_t::erase(size_t begin, size_t size) {
    if (begin > size_m) {
        throw std::out_of_range("index out of range");
    }
    size = (size + begin > size_m) ? size_m - begin : size;
    std::memmove(data_m + begin, data_m + begin + size, size_m - (begin + size));
    size_m -= size;
    data_m[size_m] = '\0';
}

// by Maksym Dzoban
const char* my_str_t::c_str() const { return data_m; }
// by Maksym Dzoban
size_t  my_str_t::capacity() const  { return capacity_m; }
// by Maksym Dzoban
size_t my_str_t::size() const { return size_m; }

// by Lev Lysyk
size_t my_str_t::find(char c, size_t idx) const {
    if (idx > size_m)
    {
        throw std::out_of_range("my_str_t::find");
    }
    for (size_t i = idx; i < size_m; ++i)
    {
        if (data_m[i] == c)
        {
            return i;
        }
    }
    return not_found;
}

// by Lev Lysyk
size_t my_str_t::find(const std::string& str, size_t idx) const {
    if (idx > size_m)
    {
        throw std::out_of_range("my_str_t::find");
    }
    size_t substr_i = 0;
    size_t const str_len = str.size();
    for (size_t i = idx; i < size_m; ++i)
    {
        while (substr_i < str_len && data_m[i + substr_i] == str[substr_i])
        {
            ++substr_i;
        }
        if (substr_i == str_len)
        {
            return i;
        }
        substr_i = 0;
    }
    return not_found;
}

// by Lev Lysyk
size_t my_str_t::find(const char* cstr, size_t idx) const {
    if (idx > size_m)
    {
        throw std::out_of_range("my_str_t::find");
    }
    size_t substr_i = 0;
    size_t const str_len = std::strlen(cstr);
    for (size_t i = idx; i < size_m; ++i)
    {
        while (substr_i < str_len && data_m[i + substr_i] == cstr[substr_i])
        {
            ++substr_i;
        }
        if (substr_i == str_len)
        {
            return i;
        }
        substr_i = 0;
    }
    return not_found;
}

// by Lev Lysyk
my_str_t my_str_t::substr(size_t begin, size_t size) const {
    if (data_m == nullptr) {
        throw std::logic_error("Null pointer passed to constructor");
    }
    if (begin > size_m)
    {
        throw std::out_of_range("Incorrect index!");
    }
    size_t const new_size_m = (size > size_m - begin) ? size_m - begin : size;
    my_str_t substring(new_size_m);
    substring.size_m = new_size_m;
    for (size_t i = 0; i < new_size_m; ++i)
    {
        substring[i] = data_m[begin + i];
    }
    substring.resize(new_size_m);
    return substring;
}

my_str_t::~my_str_t() {
    delete[] data_m;
}

// by Maksym Dzoban
//!<< оператор вивести в консоль
std::ostream& operator<<(std::ostream& stream, const my_str_t& str) {
    stream << str.c_str(); //! викликали гетер для data_m
    return stream; }

// by Lev Lysyk
std::istream& operator>>(std::istream& stream, my_str_t& str) {
    while (std::isspace(stream.get())) {}
    stream.unget();
    str.clear();
    while (!std::isspace(stream.peek()) && !stream.eof() && !stream.fail())
    {
        str.append(static_cast<char>(stream.get()));
    }
    return stream;
}

// by Lev Lysyk
std::istream& readline(std::istream& stream, my_str_t& str) {
    str.clear();
    for (size_t i = 0; stream.peek() != '\n'; i++)
    {
        str.append(static_cast<char>(stream.get()));
    }
    return stream;
}

// by Maksym Dzoban
//! приймає на всіх 2 обєкти мого класу і порівнює
bool operator==(const my_str_t &str1, const my_str_t &str2) {
    if (str1.size() != str2.size()) { return false; }
    for (size_t i = 0; i < str1.size(); i++) {
        if (str1[i] != str2[i]) { return false; }
    }
    return true;
}

// by Maksym Dzoban
//! приймає на вхід 1 обєкт мого класу і 1 с стрічку
bool operator==(const my_str_t& str1, const char* cstr2) {
    if (str1.size() != std::strlen(cstr2)) { return false; }
    for (size_t i = 0; i < str1.size(); i++) {
        if (str1[i] != cstr2[i]) { return false; }
    }
    return true;
}

// by Maksym Dzoban
//! приймає на вхід 1 c string  і 1 екземпляр мого класу
bool operator==(const char* cstr1, const my_str_t& str2) {
    if (std::strlen(cstr1) != str2.size()) { return false; }
    for (size_t i = 0; i < str2.size(); i++) {
        if (cstr1[i] != str2[i]) { return false; }
    }
    return true;}

// by Maksym Dzoban
bool operator!=(const my_str_t& str1, const my_str_t& str2) {
    return !(str1==str2);
}

// by Maksym Dzoban
bool operator!=(const my_str_t& str1, const char* cstr2) {
    return !(str1==cstr2);
}

// by Maksym Dzoban
bool operator!=(const char* cstr1, const my_str_t& str2) {
    return !(cstr1==str2);
}

// by Oksana Kotliarchuk
bool operator< (const my_str_t& str1, const my_str_t& str2) {
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

// by Oksana Kotliarchuk
bool operator< (const my_str_t& str1, const char* cstr2) {
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

// by Oksana Kotliarchuk
bool operator< (const char* cstr1, const my_str_t& str2) {
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

// by Oksana Kotliarchuk
bool operator> (const my_str_t& str1, const my_str_t& str2) {
  return (str2 < str1);
}

// by Oksana Kotliarchuk
bool operator> (const my_str_t& str1, const char* cstr2) {
  return (cstr2 < str1);
}

// by Oksana Kotliarchuk
bool operator> (const char* cstr1, const my_str_t& str2) {
  return (str2 < cstr1);
}

// by Oksana Kotliarchuk
bool operator<= (const my_str_t& str1, const my_str_t& str2) {
  return not (str1 > str2);
}

// by Oksana Kotliarchuk
bool operator<= (const my_str_t& str1, const char* cstr2) {
  return not (str1 > cstr2);
}

// by Oksana Kotliarchuk
bool operator<= (const char* cstr1, const my_str_t& str2) {
  return not (cstr1 > str2);
}

// by Oksana Kotliarchuk
bool operator>= (const my_str_t& str1, const my_str_t& str2) {
  return not (str1 < str2);
}

// by Oksana Kotliarchuk
bool operator>=(const my_str_t& str1, const char* cstr2) {
  return not (str1 < cstr2);
}

// by Oksana Kotliarchuk
bool operator>= (const char* cstr1, const my_str_t& str2) {
  return not (cstr1 < str2);
}

// by Lev Lysyk
my_str_t& my_str_t::operator+=(const char chr) {
    append(chr);
    return *this;
}

// by Lev Lysyk
my_str_t& my_str_t::operator+=(const my_str_t& mystr) {
    append(mystr);
    return *this;
}

// by Lev Lysyk
my_str_t& my_str_t::operator+=(const char* mystr) {
    append(mystr);
    return *this;
}

// by Maksym Dzoban
my_str_t operator+(const my_str_t& mystr, const my_str_t& mystr2) {
    my_str_t temp(mystr);
    temp += mystr2;
    return temp;
}

// by Maksym Dzoban
my_str_t operator+(const my_str_t& mystr, const char* mystr2) {
    my_str_t temp(mystr);
    temp += mystr2;
    return temp;
}

// by Maksym Dzoban
my_str_t operator+(const char* mystr, const my_str_t& mystr2) {
    return mystr2 + mystr;
}

// by Maksym Dzoban
my_str_t operator+(const my_str_t& mystr, const char chr) {
    my_str_t temp(mystr);
    temp += chr;
    return temp;
}

// by Lev Lysyk
my_str_t operator+(const char chr, const my_str_t& mystr) {
    return mystr + chr;
}

// by Lev Lysyk
my_str_t& my_str_t::operator*=(int const count) {
    if (count < 0)
    {
        throw(std::invalid_argument("my_str_t::operator*=(int const count)"));
    }
    reserve(2 * (size_m * count) + 1);
    my_str_t const str_copy(*this);
    for(int i = 1; i < count; ++i)
    {
        append(str_copy);
    }
    size_m = str_copy.size_m * count;
    data_m[size_m] = '\0';
    return *this;
};

// by Lev Lysyk
my_str_t operator*(const my_str_t& str1, int const count) {
    if (count < 0)
    {
        throw(std::invalid_argument("my_str_t::operator*=(int const count)"));
    }
    my_str_t result(str1);
    result.reserve(2 * (result.size() * count) + 1);
    for (int i = 1; i < count; ++i)
    {
        result.append(str1);
    }
    result.resize(str1.size() * count);
    return result;
}

// by Lev Lysyk
my_str_t operator*(int const count, const my_str_t& str1) {
    return str1 * count;
}

// by Oksana Kotliarchuk
my_str_t::my_str_t(my_str_t&& other_str) noexcept :
        data_m(other_str.data_m), capacity_m(other_str.capacity_m)
        , size_m(other_str.size_m) {
    other_str.data_m = nullptr;
    other_str.size_m = 0;
    other_str.capacity_m = 0;
}

// by Oksana Kotliarchuk
my_str_t& my_str_t::operator=(my_str_t&& other_str) noexcept {
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

