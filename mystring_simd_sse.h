#ifndef my_str_h_simd_sse
#define my_str_h_simd_sse
#pragma once

#include <iostream>
#include <cstring>
#include <cstddef>
#include <string>
#include <iostream>
#include <smmintrin.h>

class my_str_simd_sse {
private:
    char* data_m;
    size_t capacity_m;
    size_t size_m;

public:
    //! Створює стрічку із size копій літери initial
    //! capacity_m встановлює рівним або більшим за size
    ////! Обробка помилок конструкторами:
    //! Не повинні заважати пропагуванню виключення
    //! std::bad_alloc.
    my_str_simd_sse(size_t size, char initial);
    explicit my_str_simd_sse(size_t capacity = 0);

    //! Копіює вміст С-стрічки, вимоги до capacity_m -- ті ж, що вище
    my_str_simd_sse(const char* cstr);

    //! Копіює стрічку С++, вимоги до capacity_m -- ті ж, що вище
    my_str_simd_sse(const std::string& str);

    //!оператор присвоєння
    my_str_simd_sse& operator=(const my_str_simd_sse& mystr);

    //! оператор копіювання
    my_str_simd_sse(const my_str_simd_sse& mystr);

    const char* c_str() const;
    size_t capacity() const;
    size_t size() const;

    char* getDataPtr() const {
        return data_m;
    }
    void swap(my_str_simd_sse& other ) noexcept;
    void reserve(size_t new_capacity);

    char& operator[](size_t idx);
    const char& operator [] (size_t idx) const ;

    char& at(size_t idx);
    const char& at ( size_t idx ) const ;
    void shrink_to_fit ();
    void resize(size_t new_size, char new_char = ' ');
    void clear();
    //! Вставляє передану стрічку типу my_str_t, чи літеру, //! чи С-стрічку, починаючи з літери idx,
    //! зсуваючи літеру з позиції idx і правіше праворуч. //! Обробка помилок:
    //! Якщо idx > size_m -- кидає виключення std::out_of_range
    void insert(size_t idx, const my_str_simd_sse& str);
    //! Ця функція -- служить для оптимізації, щоб не
    //! довелося спочатку створювати із літери c стрічку my_str_t, а //! потім вставляти. Навіть якщо компілятор зробив би це
    //! автоматично -- це повільно.
    void insert(size_t idx, char c);
    //! Аналогічна оптимізація для C-стрічок
    void insert(size_t idx, const char* cstr);
    void append(const my_str_simd_sse& str);
    void append(char c);
    void append(const char* cstr);
    void erase(size_t begin, size_t size);
    my_str_simd_sse substr(size_t begin, size_t size) const;
    static constexpr size_t not_found = -1;
    size_t find(char c, size_t idx = 0) const;
    size_t find(const std::string& str, size_t idx = 0) const;
    size_t find(const char* cstr, size_t idx = 0) const;


    my_str_simd_sse& operator+=(char chr);
    my_str_simd_sse& operator+=(const my_str_simd_sse& mystr);
    my_str_simd_sse& operator+=(const char* mystr);
    my_str_simd_sse& operator*=(int count);

    my_str_simd_sse(my_str_simd_sse &&other_str) noexcept;
    my_str_simd_sse &operator=(my_str_simd_sse &&other_str) noexcept;

    //! Деструктор.
    ~my_str_simd_sse();
};
std::ostream& operator<<(std::ostream &stream,const my_str_simd_sse &str);
std::istream& operator>>(std::istream& is, my_str_simd_sse& str);
// std::ostream& operator>>(std::ostream &stream,const my_str_t &str);
std::istream& readline (std::istream &stream, my_str_simd_sse &str);

bool operator==(const my_str_simd_sse& str1, const my_str_simd_sse& str2);
bool operator==(const char* cstr1, const my_str_simd_sse& str2);
bool operator==(const my_str_simd_sse& str1, const char* cstr2);

bool operator!=(const my_str_simd_sse& str1, const my_str_simd_sse& str2);
bool operator!=(const char* cstr1, const my_str_simd_sse& str2);
bool operator!=(const my_str_simd_sse& str1, const char* cstr2);

bool operator>(const my_str_simd_sse& str1, const my_str_simd_sse& str2);
bool operator>(const char* cstr1, const my_str_simd_sse& str2);
bool operator>(const my_str_simd_sse& str1, const char* cstr2);

bool operator<(const my_str_simd_sse& str1, const my_str_simd_sse& str2);
bool operator<(const char* cstr1, const my_str_simd_sse& str2);
bool operator<(const my_str_simd_sse& str1, const char* cstr2);

bool operator>=(const my_str_simd_sse& str1, const my_str_simd_sse& str2);
bool operator>=(const char* cstr1, const my_str_simd_sse& str2);
bool operator>=(const my_str_simd_sse& str1, const char* cstr2);

bool operator<=(const my_str_simd_sse& str1, const my_str_simd_sse& str2);
bool operator<=(const char* cstr1, const my_str_simd_sse& str2);
bool operator<=(const my_str_simd_sse& str1, const char* cstr2);


my_str_simd_sse operator+(const my_str_simd_sse& mystr, const my_str_simd_sse& mystr2);
my_str_simd_sse operator+(const my_str_simd_sse& mystr, const char* mystr2);
my_str_simd_sse operator+(const my_str_simd_sse& mystr2, char chr);
my_str_simd_sse operator+(char chr, const my_str_simd_sse& mystr2);

my_str_simd_sse operator*(const my_str_simd_sse& str1, int count);
my_str_simd_sse operator*(int count, const my_str_simd_sse& str1);

#endif
