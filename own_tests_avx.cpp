#include "mystring_avx.h"
int main() {
    std::cout << "Testing my_str_avx class functions..." << std::endl;
    try {
        // Тест конструктора з розміром і початковим символом
        my_str_avx str1(10, 'A');
        std::cout << "Constructor with size and initial char: " << str1 << std::endl;

        // Тест конструктора з C-стрічкою
        my_str_avx str2("Hello, World!");
        std::cout << "Constructor with C-string: " << str2 << std::endl;

        // Тест конструктора з C++ стрічкою
        std::string cpp_str = "Test C++ string";
        my_str_avx str3(cpp_str);
        std::cout << "Constructor with std::string: " << str3 << std::endl;

        // Тест конструктора копіювання
        my_str_avx str4(str2);
        std::cout << "Copy constructor: " << str4 << std::endl;

        // Тест оператора присвоєння
        str1 = str3;
        std::cout << "Assignment operator: str1 =" << str1 << std::endl;

        // Тест конструктора переміщення
        my_str_avx str5(std::move(str3));
        std::cout << "Move constructor: " << str5 << std::endl;

//видалення 5 стрінги

        // Тест оператора переміщення присвоєння
        str1 = std::move(str5);
        std::cout << "Move assignment operator: " << str1 << std::endl;

        // Тест swap
        str1.swap(str2);
        std::cout << "Swap: str1 = " << str1 << ", str2 = " << str2 << std::endl;

        // Тест оператора індексації
        char ch = str1[0];
        std::cout << "Index operator: str1[0] = " << ch << std::endl;

        const my_str_avx test_index = my_str_avx("hi, hello, hola!");
        // Тест на індексацію
        if (test_index[1] != 'i') {
            std::cerr << "Test failed: test_index[1] expected 'i' but got '" << test_index[1] << "'." << std::endl;
        } else {
            std::cout << "Test passed: test_index[1] == 'i'." << std::endl;
        }

        if (test_index.at(2) != ',') {
            std::cerr << "Test failed: test_index.at(2) expected ',' but got '" << test_index.at(2) << "'." << std::endl;
        } else {
            std::cout << "Test passed: test_index.at(2) == ','." << std::endl;
        }

        // Тест на вихід за межі
        try {
            test_index.at(100);
            std::cerr << "Test failed: No exception thrown for out-of-range access." << std::endl;
        } catch (const std::out_of_range&) {
            std::cout << "Test passed: Out of range access threw exception as expected." << std::endl;
        }

        my_str_avx test_index_2 = my_str_avx("hi, hello, hola!");

        // Тест на індексацію
        if (test_index_2[1] != 'i') {
            std::cerr << "Test failed: test_index_2[1] expected 'i' but got '" << test_index_2[1] << "'." << std::endl;
        } else {
            std::cout << "Test passed: test_index_2[1] == 'i'." << std::endl;
        }

        if (test_index_2.at(2) != ',') {
            std::cerr << "Test failed: test_index_2.at(2) expected ',' but got '" << test_index_2.at(2) << "'." << std::endl;
        } else {
            std::cout << "Test passed: test_index_2.at(2) == ','." << std::endl;
        }

        // Тест на вихід за межі
        try {
            test_index_2.at(100);
            std::cerr << "Test failed: No exception thrown for out-of-range access." << std::endl;
        } catch (const std::out_of_range&) {
            std::cout << "Test passed: Out of range access threw exception as expected." << std::endl;
        }

        // Зміна символу за індексом
        test_index_2[3] = 'h';
        if (test_index_2 != my_str_avx("hi,hhello, hola!")) {
            std::cerr << "Test failed: test_index_2 expected 'hi,hhello, hola!' but got '" << test_index_2 << "'." << std::endl;
        } else {
            std::cout << "Test passed: test_index_2 == 'hi,hhello, hola!'." << std::endl;
        }


        // Тест оператора at
        char at_ch = str1.at(0);
        std::cout << "At operator: str1.at(0) = " << at_ch << std::endl;

        str1.reserve(123);
        if (str1.capacity() < 123) {
            std::cerr << "Test failed: Expected capacity to be at least 123, but got " << str1.capacity() << "." << std::endl;
        } else {
            std::cout << "Test passed: Capacity is greater than or equal to 123." << std::endl;
        }


    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
    }

    my_str_avx str8(15, 'b');
    size_t original_capacity = str8.capacity();
    size_t original_size = str8.size();
    // Виклик функції shrink_to_fit
    str8.shrink_to_fit();
    // Тест: перевірка, що ємність після shrink_to_fit дорівнює розміру
    if (str8.capacity() != (str8.size()+1)) {
        std::cerr << "Test failed: Expected capacity to be " <<  (str8.size()+1) << " but got " << str8.capacity() << "." << std::endl;
    } else {
        std::cout << "Test passed: Capacity has been reduced to fit the size (" << str8.capacity() << ")." << std::endl;
    }

    // Перевірка, що зміст стрічки не змінився
    if (str8 != my_str_avx(15, 'b')) {
        std::cerr << "Test failed: Expected content 'bbbbbbbbbbbbbbb' but got '" << str8 << "'." << std::endl;
    } else {
        std::cout << "Test passed: Content is unchanged." << std::endl;
    }

    my_str_avx str(10, 'a'); // Створює стрічку з 10 'a'
    std::cout << "Initial string: " << str.c_str() << std::endl;
    str.resize(5); // Зменшує розмір до 5
    std::cout << "Resized string: " << str.c_str() << std::endl;
    str.clear(); // Очищає стрічку
    std::cout << "After clear: str.size() = " << str.size() << std::endl;

    str.append('b'); // Додає 'b'
    std::cout << "After append 'b': " << str.c_str() << std::endl;
    str.append("Hello"); // Додає C-стрічку
    std::cout << "After append 'Hello': " << str.c_str() << std::endl;
    my_str_avx append_test1("Hello");
    my_str_avx append_str(" World!");
    append_test1.append(append_str);
    if (append_test1 != my_str_avx("Hello World!")) {
        std::cerr << "Test failed: append_test1 expected 'Hello World!' but got '" << append_test1 << "'." << std::endl;
    } else {
        std::cout << "Test passed: append_test1 == 'Hello World!'." << std::endl;
    }

    my_str_avx append_test2("Hello");
    const char* append_cstr = ", C++!";
    append_test2.append(append_cstr);
    if (append_test2 != my_str_avx("Hello, C++!")) {
        std::cerr << "Test failed: append_test2 expected 'Hello, C++!' but got '" << append_test2 << "'." << std::endl;
    } else {
        std::cout << "Test passed: append_test2 == 'Hello, C++!'." << std::endl;
    }

    my_str_avx append_test3("Welcome to");
    std::string append_cppstr = " C++ World!";
    append_test3.append(append_cppstr.c_str());
    if (append_test3 != my_str_avx("Welcome to C++ World!")) {
        std::cerr << "Test failed: append_test3 expected 'Welcome to C++ World!' but got '" << append_test3 << "'." << std::endl;
    } else {
        std::cout << "Test passed: append_test3 == 'Welcome to C++ World!'." << std::endl;
    }

    str.insert(1, 'c'); // Вставляє 'c' на позицію 1
    std::cout << "After insert 'c' at index 1: " << str.c_str() << std::endl;
    str.erase(2, 2); // Видаляє 2 символи з позиції 2
    std::cout << "After erase from index 2, size 2: " << str.c_str() << std::endl;

    my_str_avx insert_test1("Hello, World!");
    my_str_avx insert_str(" C++");
    insert_test1.insert(6, insert_str);
    if (insert_test1 != my_str_avx("Hello, C++ World!")) {
        std::cerr << "Test failed: insert_test1 expected 'Hello, C++ World!' but got '" << insert_test1 << "'." << std::endl;
    } else {
        std::cout << "Test passed: insert_test1 == 'Hello, C++ World!'." << std::endl;
    }

    my_str_avx insert_test2("Hello, World!");
    insert_test2.insert(5, " everyone");
    if (insert_test2 != my_str_avx("Hello everyone, World!")) {
        std::cerr << "Test failed: insert_test2 expected 'Hello everyone, World!' but got '" << insert_test2 << "'." << std::endl;
    } else {
        std::cout << "Test passed: insert_test2 == 'Hello everyone, World!'." << std::endl;
    }

    my_str_avx insert_test3("Hello, World!");
    std::string insert_cppstr = " and everyone";
    insert_test3.insert(6, insert_cppstr.c_str());

    if (insert_test3 != my_str_avx("Hello, and everyone World!")) {
        std::cerr << "Test failed: insert_test3 expected 'Hello, and everyone World!' but got '" << insert_test3 << "'." << std::endl;
    } else {
        std::cout << "Test passed: insert_test3 == 'Hello, and everyone World!'." << std::endl;
    }

    size_t index = str.find('b'); // Шукає 'e'
    std::cout << "Index of 'b': " << index << std::endl;

    my_str_avx sub = str.substr(1, 3); // Копіює підстрічку
    std::cout << "Substring (1, 3): " << sub.c_str() << std::endl;

    my_str_avx test("Yaroshko");

    // Перевірка розміру і ємності
    if (test.size() != 8) {
        std::cerr << "Test failed: size expected 8 but got " << test.size() << "." << std::endl;
    } else {
        std::cout << "Test passed: size is " << test.size() << "." << std::endl;
    }

    if (test.capacity() < test.size()) {
        std::cerr << "Test failed: capacity expected to be >= size but got " << test.capacity() << "." << std::endl;
    } else {
        std::cout << "Test passed: capacity is " << test.capacity() << "." << std::endl;
    }

    index = test.find(std::string("osh"));
    if (index != 3) {
        std::cerr << "Test failed: find(std::string('osh')) expected 3 but got " << index << "." << std::endl;
    } else {
        std::cout << "Test passed: find(std::string('osh')) is " << index << "." << std::endl;
    }

    // Об'єкти класу my_str_avx
    my_str_avx str1("apple");
    my_str_avx str2("banana");
    my_str_avx str3("apple");

    // C-style рядок
    const char* cstr1 = "apple";
    const char* cstr2 = "banana";

    // Рядок C++
    std::string cppstr1 = "apple";
    std::string cppstr2 = "banana";

    // Тест операторів порівняння між my_str_avx і my_str_avx
    if (str1 == str3) {
        std::cout << "Test passed: str1 == str3 (my_str_avx == my_str_avx)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 != str3 (my_str_avx == my_str_avx)" << std::endl;
    }

    if (str1 != str2) {
        std::cout << "Test passed: str1 != str2 (my_str_avx != my_str_avx)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 == str2 (my_str_avx != my_str_avx)" << std::endl;
    }

    // Тест операторів порівняння між my_str_avx і C-style рядком
    if (str1 == cstr1) {
        std::cout << "Test passed: str1 == cstr1 (my_str_avx == const char*)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 != cstr1 (my_str_avx == const char*)" << std::endl;
    }

    if (str1 != cstr2) {
        std::cout << "Test passed: str1 != cstr2 (my_str_avx != const char*)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 == cstr2 (my_str_avx != const char*)" << std::endl;
    }

    // Тест операторів порівняння між my_str_avx і C++ рядком
    if (str1 == cppstr1) {
        std::cout << "Test passed: str1 == cppstr1 (my_str_avx == std::string)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 != cppstr1 (my_str_avx == std::string)" << std::endl;
    }

    if (str1 != cppstr2) {
        std::cout << "Test passed: str1 != cppstr2 (my_str_avx != std::string)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 == cppstr2 (my_str_avx != std::string)" << std::endl;
    }

    // Тест операторів порівняння між C-style рядком і my_str_avx
    if (cstr1 == str1) {
        std::cout << "Test passed: cstr1 == str1 (const char* == my_str_avx)" << std::endl;
    } else {
        std::cerr << "Test failed: cstr1 != str1 (const char* == my_str_avx)" << std::endl;
    }

    if (cstr2 != str1) {
        std::cout << "Test passed: cstr2 != str1 (const char* != my_str_avx)" << std::endl;
    } else {
        std::cerr << "Test failed: cstr2 == str1 (const char* != my_str_avx)" << std::endl;
    }

    // Тест операторів порівняння між C++ рядком і my_str_avx
    if (cppstr1 == str1) {
        std::cout << "Test passed: cppstr1 == str1 (std::string == my_str_avx)" << std::endl;
    } else {
        std::cerr << "Test failed: cppstr1 != str1 (std::string == my_str_avx)" << std::endl;
    }

    if (cppstr2 != str1) {
        std::cout << "Test passed: cppstr2 != str1 (std::string != my_str_avx)" << std::endl;
    } else {
        std::cerr << "Test failed: cppstr2 == str1 (std::string != my_str_avx)" << std::endl;
    }
    // Додаткові тести на конструктори переміщення та оператори +
    {
        my_str_avx concat_str1("Hello, ");
        my_str_avx concat_str2("World!");
        const char* concat_cstr = " C++!";
        std::string concat_cppstr = " Welcome!";

        // Тест оператора +
        my_str_avx concat_result = concat_str1 + concat_str2;
        if (concat_result == my_str_avx("Hello, World!")) {
            std::cout << "Test passed: concat_str1 + concat_str2 == 'Hello, World!'" << std::endl;
        } else {
            std::cerr << "Test failed: concat_str1 + concat_str2 != 'Hello, World!'" << std::endl;
        }

        // Тест оператора + з C-style рядком
        concat_result = concat_str1 + concat_cstr;
        if (concat_result == my_str_avx("Hello,  C++!")) {
            std::cout << "Test passed: concat_str1 + concat_cstr == 'Hello,  C++!'" << std::endl;
        } else {
            std::cerr << "Test failed: concat_str1 + concat_cstr != 'Hello,  C++!'" << std::endl;
        }

        // Тест оператора + з C++ рядком
        concat_result = concat_str1 + concat_cppstr;
        if (concat_result == my_str_avx("Hello,  Welcome!")) {
            std::cout << "Test passed: concat_str1 + concat_cppstr == 'Hello,  Welcome!'" << std::endl;
        } else {
            std::cerr << "Test failed: concat_str1 + concat_cppstr != 'Hello,  Welcome!'" << std::endl;
        }

        // Тест оператора += з my_str_avx
        concat_str1 += concat_str2;
        if (concat_str1 == my_str_avx("Hello, World!")) {
            std::cout << "Test passed: concat_str1 += concat_str2 == 'Hello, World!'" << std::endl;
        } else {
            std::cerr << "Test failed: concat_str1 += concat_str2 != 'Hello, World!'" << std::endl;
        }

        // Тест оператора += з C-style рядком
        concat_str1 += concat_cstr;
        if (concat_str1 == my_str_avx("Hello, World! C++!")) {
            std::cout << "Test passed: concat_str1 += concat_cstr == 'Hello, World! C++!'" << std::endl;
        } else {
            std::cerr << "Test failed: concat_str1 += concat_cstr != 'Hello, World! C++!'" << std::endl;
        }

        // Тест оператора += з C++ рядком
        concat_str1 += concat_cppstr;
        if (concat_str1 == my_str_avx("Hello, World! C++! Welcome!")) {
            std::cout << "Test passed: concat_str1 += concat_cppstr == 'Hello, World! C++! Welcome!'" << std::endl;
        } else {
            std::cerr << "Test failed: concat_str1 += concat_cppstr != 'Hello, World! C++! Welcome!'" << std::endl;
        }

        // Оператори * і *=
        my_str_avx repeat_str1("abc");

        // Тест оператора *
        my_str_avx repeat_result = repeat_str1 * 3;
        if (repeat_result == my_str_avx("abcabcabc")) {
            std::cout << "Test passed: repeat_str1 * 3 == 'abcabcabc'" << std::endl;
        } else {
            std::cerr << "Test failed: repeat_str1 * 3 != 'abcabcabc'" << std::endl;
        }

        // Тест оператора *=
        repeat_str1 *= 2;
        if (repeat_str1 == my_str_avx("abcabc")) {
            std::cout << "Test passed: repeat_str1 *= 2 == 'abcabc'" << std::endl;
        } else {
            std::cerr << "Test failed: repeat_str1 *= 2 != 'abcabc'" << std::endl;
        }
        my_str_avx* test_obj = new my_str_avx("Test string");
        delete test_obj;
        test_obj = nullptr;
        // Порівняння my_str_avx з my_str_avx
        if (str2 > str1) {
            std::cout << "Test passed: str2 > str1" << std::endl;
        } else {
            std::cerr << "Test failed: str2 !> str1" << std::endl;
        }

        if (str1 < str2) {
            std::cout << "Test passed: str1 !> str2" << std::endl;
        } else {
            std::cerr << "Test failed: str1 > str2" << std::endl;
        }

        if (!(str1 > str3)) {
            std::cout << "Test passed: str1 !> str3" << std::endl;
        } else {
            std::cerr << "Test failed: str1 > str3" << std::endl;
        }

        // Порівняння my_str_avx з const char*
        if (cstr2 > str1) {
            std::cout << "Test passed: 'banana' > str1" << std::endl;
        } else {
            std::cerr << "Test failed: 'banana' !> str1" << std::endl;
        }

        if (str1 < cstr2) {
            std::cout << "Test passed: str1 < 'banana'" << std::endl;
        } else {
            std::cerr << "Test failed: str1 < 'banana'" << std::endl;
        }

        // Порівняння my_str_avx з std::string
        if (str2 > cppstr1.c_str()) {
            std::cout << "Test passed: str2 > cppstr1" << std::endl;
        } else {
            std::cerr << "Test failed: str2 !> cppstr1" << std::endl;
        }

        if (!(str1 > cppstr2.c_str())) {
            std::cout << "Test passed: str1 !> cppstr2" << std::endl;
        } else {
            std::cerr << "Test failed: str1 > cppstr2" << std::endl;
        }
        if (str2 >= str1) {
            std::cout << "Test passed: str2 >= str1" << std::endl;
        } else {
            std::cerr << "Test failed: str2 !>= str1" << std::endl;
        }


        if (str1 >= str3) {
            std::cout << "Test passed: str1 >= str3" << std::endl;
        } else {
            std::cerr << "Test failed: str1 !>= str3" << std::endl;
        }

        // Порівняння my_str_avx з const char*
        if (cstr2 >= str1) {
            std::cout << "Test passed: 'banana' >= str1" << std::endl;
        } else {
            std::cerr << "Test failed: 'banana' !>= str1" << std::endl;
        }

        if (cstr1 >= str1) {
            std::cout << "Test passed: 'apple' >= str1" << std::endl;
        } else {
            std::cerr << "Test failed: 'apple' !>= str1" << std::endl;
        }

    }
    my_str_avx str1lesseq("apple");
    my_str_avx str2lesseq("banana");
    my_str_avx str3lesseq("apple");
    my_str_avx str4lesseq("appl");

    // Test str1 <= str2
    if (str1lesseq <= str2lesseq) {
        std::cout << "Test passed: str1 <= str2 (my_str_avx <= my_str_avx)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 > str2 (my_str_avx <= my_str_avx)" << std::endl;
    }

    // Test str1 <= str3
    if (str1lesseq <= str3lesseq) {
        std::cout << "Test passed: str1 <= str3 (my_str_avx <= my_str_avx)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 > str3 (my_str_avx <= my_str_avx)" << std::endl;
    }

    // Test str4 <= str1
    if (str4lesseq <= str1lesseq) {
        std::cout << "Test passed: str4 <= str1 (my_str_avx <= my_str_avx)" << std::endl;
    } else {
        std::cerr << "Test failed: str4 > str1 (my_str_avx <= my_str_avx)" << std::endl;
    }

    // Test with C-string
    const char* cstr1lesseq = "apple";
    const char* cstr2lesseq = "banana";

    if (str1lesseq <= cstr2lesseq) {
        std::cout << "Test passed: str1 <= cstr2 (my_str_avx <= const char*)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 > cstr2 (my_str_avx <= const char*)" << std::endl;
    }

    if (str1lesseq <= cstr1lesseq) {
        std::cout << "Test passed: str1 <= cstr1 (my_str_avx <= const char*)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 > cstr1 (my_str_avx <= const char*)" << std::endl;
    }

    // Test with std::string
    std::string cppstr1lesseq = "apple";
    std::string cppstr2lesseq = "banana";

    if (str1lesseq <= cppstr2lesseq) {
        std::cout << "Test passed: str1 <= cppstr2 (my_str_avx <= std::string)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 > cppstr2 (my_str_avx <= std::string)" << std::endl;
    }

    if (str1lesseq <= cppstr1lesseq) {
        std::cout << "Test passed: str1 <= cppstr1 (my_str_avx <= std::string)" << std::endl;
    } else {
        std::cerr << "Test failed: str1 > cppstr1 (my_str_avx <= std::string)" << std::endl;
    }
    return 0;
}