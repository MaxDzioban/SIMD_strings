#ifndef my_str_h_simd
#define my_str_h_simd
#pragma once

#include <iostream>
#include <cstring>
#include <cstddef>
#include <string>
#include <iostream>
#include <arm_neon.h>

class my_str_simd {
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
    my_str_simd(size_t size, char initial);
    explicit my_str_simd(size_t capacity = 0);

    //! Копіює вміст С-стрічки, вимоги до capacity_m -- ті ж, що вище
    my_str_simd(const char* cstr);

    //! Копіює стрічку С++, вимоги до capacity_m -- ті ж, що вище
    my_str_simd(const std::string& str);

    //!оператор присвоєння
    my_str_simd& operator=(const my_str_simd& mystr);

    //! оператор копіювання
    my_str_simd(const my_str_simd& mystr);

    const char* c_str() const;
    size_t capacity() const;
    size_t size() const;

    char* getDataPtr() const {
        return data_m;
    }
    void swap(my_str_simd& other ) noexcept;
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
    void insert(size_t idx, const my_str_simd& str);
    //! Ця функція -- служить для оптимізації, щоб не
    //! довелося спочатку створювати із літери c стрічку my_str_t, а //! потім вставляти. Навіть якщо компілятор зробив би це
    //! автоматично -- це повільно.
    void insert(size_t idx, char c);
    //! Аналогічна оптимізація для C-стрічок
    void insert(size_t idx, const char* cstr);
    void append(const my_str_simd& str);
    void append(char c);
    void append(const char* cstr);
    void erase(size_t begin, size_t size);
    my_str_simd substr(size_t begin, size_t size) const;
    static constexpr size_t not_found = -1;
    size_t find(char c, size_t idx = 0) const;
    size_t find(const std::string& str, size_t idx = 0) const;
    size_t find(const char* cstr, size_t idx = 0) const;


    my_str_simd& operator+=(char chr);
    my_str_simd& operator+=(const my_str_simd& mystr);
    my_str_simd& operator+=(const char* mystr);
    my_str_simd& operator*=(int count);

    my_str_simd(my_str_simd &&other_str) noexcept;
    my_str_simd &operator=(my_str_simd &&other_str) noexcept;

    //! Деструктор.
    ~my_str_simd();
};
std::ostream& operator<<(std::ostream &stream,const my_str_simd &str);
std::istream& operator>>(std::istream& is, my_str_simd& str);
// std::ostream& operator>>(std::ostream &stream,const my_str_t &str);
std::istream& readline (std::istream &stream, my_str_simd &str);

bool operator==(const my_str_simd& str1, const my_str_simd& str2);
bool operator==(const char* cstr1, const my_str_simd& str2);
bool operator==(const my_str_simd& str1, const char* cstr2);

bool operator!=(const my_str_simd& str1, const my_str_simd& str2);
bool operator!=(const char* cstr1, const my_str_simd& str2);
bool operator!=(const my_str_simd& str1, const char* cstr2);

bool operator>(const my_str_simd& str1, const my_str_simd& str2);
bool operator>(const char* cstr1, const my_str_simd& str2);
bool operator>(const my_str_simd& str1, const char* cstr2);

bool operator<(const my_str_simd& str1, const my_str_simd& str2);
bool operator<(const char* cstr1, const my_str_simd& str2);
bool operator<(const my_str_simd& str1, const char* cstr2);

bool operator>=(const my_str_simd& str1, const my_str_simd& str2);
bool operator>=(const char* cstr1, const my_str_simd& str2);
bool operator>=(const my_str_simd& str1, const char* cstr2);

bool operator<=(const my_str_simd& str1, const my_str_simd& str2);
bool operator<=(const char* cstr1, const my_str_simd& str2);
bool operator<=(const my_str_simd& str1, const char* cstr2);


my_str_simd operator+(const my_str_simd& mystr, const my_str_simd& mystr2);
my_str_simd operator+(const my_str_simd& mystr, const char* mystr2);
my_str_simd operator+(const my_str_simd& mystr2, char chr);
my_str_simd operator+(char chr, const my_str_simd& mystr2);

my_str_simd operator*(const my_str_simd& str1, int count);
my_str_simd operator*(int count, const my_str_simd& str1);

#endif
