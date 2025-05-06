#include <chrono>
#include <iostream>
#include "mystring_simd_sse.h"
#include "mystring.hpp"
#include <string>
#include <vector>

void test_copy_speed() {
    const size_t sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 10000, 1000000, 1000000'0, 1000000'00};
    const int warmup_runs = 100;
    const int test_runs_small = 1000;
    const int test_runs_large = 10;

    // AI helped with table
    std::cout << "| Size (chars) | my_str_t avg time (μs) | my_str_simd_sse avg time (μs) |\n";
    std::cout << "|--------------|------------------------|---------------------------|\n";
    volatile size_t checksum = 0;
    for (size_t size : sizes) {
        volatile size_t checksum_3 = 0;
        std::string source(size, 'x');
        int runs = (size >= 1000000) ? test_runs_large : test_runs_small;

        for (int i = 0; i < warmup_runs; ++i) {
            my_str_t warmup_normal(source);
            my_str_simd_sse warmup_simd(source);
            checksum += warmup_normal.size() + warmup_simd.size();
        }

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < runs; ++i) {
            my_str_t normal(source);
            checksum += normal.size();
        }
        auto end = std::chrono::steady_clock::now();
        auto normal_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / runs;

        start = std::chrono::steady_clock::now();
        for (int i = 0; i < runs; ++i) {
            my_str_simd_sse simd(source);
            checksum += simd.size();
        }
        end = std::chrono::steady_clock::now();
        auto simd_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / runs;
        std::cout << "| " << size
                  << " | " << normal_time
                  << " | " << simd_time << " |\n";
    }
    std::cout << "Checksum: " << checksum << "\n";
}


void test_reserve_speed() {
    const size_t initial_size = 100;
    const size_t reserve_size = (1 << 19);
    std::string source(initial_size, 'x');
    my_str_t normal(source);
    my_str_simd_sse simd(source);

    auto start = std::chrono::high_resolution_clock::now();
    normal.reserve(reserve_size);
    volatile char sink = 0;
    for (size_t i = 0; i < normal.size(); i += reserve_size / 100) {
        sink ^= normal[i];
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto normal_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "my_str_t reserve time: " << normal_time << " microseconds\n";

    start = std::chrono::high_resolution_clock::now();
    simd.reserve(reserve_size);
    for (size_t i = 0; i < simd.size(); i += reserve_size / 100) {
        sink ^= simd[i];
    }
    end = std::chrono::high_resolution_clock::now();
    auto simd_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "my_str_simd_sse reserve time: " << simd_time << " microseconds\n";

    if (simd_time < normal_time) {
        std::cout << "SIMD version is faster by " << normal_time - simd_time << " microseconds!\n";
    } else {
        std::cout << "Normal version is faster by " << simd_time - normal_time << " microseconds!\n";
    }
}

void test_operator_eq_one(const char* expected, const my_str_simd_sse& actual, int repeat = 1) {
    auto start = std::chrono::high_resolution_clock::now();
    bool result = true;
    for (int i = 0; i < repeat; ++i) {
        result &= (expected == actual);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    if (!result) {
        std::cerr << "Test failed!\n";
    } else {
        std::cout << "Test passed.\n";
    }
    std::cout << "Time taken (" << repeat << "x): " << elapsed << " microseconds\n\n";
}

// для my_str_simd_sse
void test_operator_eq_one_simd(const char* expected, const my_str_simd_sse& actual, int repeat = 1) {
    auto start = std::chrono::high_resolution_clock::now();
    bool result = true;
    for (int i = 0; i < repeat; ++i) {
        result &= (expected == actual);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    if (!result) {
        std::cerr << "SIMD Test failed!\n";
    } else {
        std::cout << "SIMD Test passed.\n";
    }
    std::cout << "Time taken (" << repeat << "x): " << elapsed << " microseconds\n\n";
}

// для my_str_t
void test_operator_eq_one(const char* expected, const my_str_t& actual, int repeat = 1) {
    auto start = std::chrono::high_resolution_clock::now();
    bool result = true;
    for (int i = 0; i < repeat; ++i) {
        result &= (expected == actual);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    if (!result) {
        std::cerr << "Standard Test failed!\n";
    } else {
        std::cout << "Standard Test passed.\n";
    }
    std::cout << "Time taken (" << repeat << "x): " << elapsed << " microseconds\n\n";
}

void test_operator_eq_simd(const my_str_simd_sse& str1, const char* cstr2, int repeat = 1) {
    auto start = std::chrono::high_resolution_clock::now();
    bool result = true;
    for (int i = 0; i < repeat; ++i) {
        result &= (str1 == cstr2);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    if (!result) {
        std::cerr << "SIMD eq operator== test failed!\n";
    } else {
        std::cout << "SIMD eq operator== test passed.\n";
    }
    std::cout << "Time taken (" << repeat << "x): " << elapsed << " microseconds\n\n";
}

void test_operator_eq_norm(const my_str_t& str1, const char* cstr2, int repeat = 1) {
    auto start = std::chrono::high_resolution_clock::now();
    bool result = true;
    for (int i = 0; i < repeat; ++i) {
        result &= (str1 == cstr2);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    if (!result) {
        std::cerr << "NORMAL eq operator== test failed!\n";
    } else {
        std::cout << "NORMAL eq operator== test passed.\n";
    }
    std::cout << "Time taken (" << repeat << "x): " << elapsed << " microseconds\n\n";
}

void test_operator_eq_simd(const my_str_simd_sse& str1, const my_str_simd_sse& str2, int repeat = 1) {
    auto start = std::chrono::high_resolution_clock::now();
    bool result = true;
    for (int i = 0; i < repeat; ++i) {
        result &= (str1 == str2);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    if (!result) {
        std::cerr << "SIMD operator==(my_str_simd_sse, my_str_simd_sse) test failed!\n";
    } else {
        std::cout << "SIMD operator==(my_str_simd_sse, my_str_simd_sse) test passed.\n";
    }
    std::cout << "Time taken (" << repeat << "x): " << elapsed << " microseconds\n\n";
}

void test_operator_eq_norm_eq_eq(const my_str_t& str1, const my_str_t& str2, int repeat = 1) {
    auto start = std::chrono::high_resolution_clock::now();
    bool result = true;
    for (int i = 0; i < repeat; ++i) {
        result &= (str1 == str2);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    if (!result) {
        std::cerr << "Normal operator==(my_str_t, my_str_t) test failed!\n";
    } else {
        std::cout << "Normal operator==(my_str_t, my_str_t) test passed.\n";
    }
    std::cout << "Time taken (" << repeat << "x): " << elapsed << " microseconds\n\n";
}


void test_resize_speed_different_sizes() {
    const char fill_char = 'x';
    std::vector<size_t> sizes = {1024, 10 * 1024, 100 * 1024, 1 * 1024 * 1024, 10 * 1024 * 1024, 1024*1024*2*2*2*2};
    for (size_t initial_size : sizes) {
        size_t new_size = initial_size * 2;
        std::string source(initial_size, 'a');
        my_str_t normal(source);
        my_str_simd_sse simd(source);
        std::cout << "\nTesting with initial size: " << initial_size / 1024 << " KB\n";
        auto start = std::chrono::high_resolution_clock::now();
        normal.resize(new_size, fill_char);
        auto end = std::chrono::high_resolution_clock::now();
        auto time_normal = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_t resize time: " << time_normal << " microseconds\n";
        start = std::chrono::high_resolution_clock::now();
        simd.resize(new_size, fill_char);
        end = std::chrono::high_resolution_clock::now();
        auto time_simd = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_simd_sse resize time: " << time_simd << " microseconds\n";

        if (time_simd < time_normal) {
            std::cout << "SIMD version is faster by " << (time_normal - time_simd) << " microseconds!\n";
        } else {
            std::cout << "Normal version is faster by " << (time_simd - time_normal) << " microseconds!\n";
        }
    }
}


void test_insert_speed_different_sizes() {
    const std::vector<size_t> base_sizes = {1024, 10 * 1024, 100 * 1024, 1 * 1024 * 1024, 10 * 1024 * 1024}; // 1КБ, 10КБ, 100КБ, 1МБ, 10МБ
    const size_t insert_size = 1024;
    // 1КБ рядок
    const char base_char = 'A';
    const char insert_char = 'B';

    for (size_t base_size : base_sizes) {
        size_t insert_pos = base_size / 2;
        // paste в середину

        std::string base_source(base_size, base_char);
        std::string insert_source(insert_size, insert_char);

        my_str_t base_normal(base_source);
        my_str_t insert_normal(insert_source);

        my_str_simd_sse base_simd(base_source);
        my_str_simd_sse insert_simd(insert_source);

        std::cout << "\n=== Testing insert (base size: " << base_size / 1024 << " KB) ===\n";

        // my_str_t insert
        auto start = std::chrono::high_resolution_clock::now();
        base_normal.insert(insert_pos, insert_normal);
        auto end = std::chrono::high_resolution_clock::now();
        auto normal_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_t insert time: " << normal_time << " microseconds\n";

        // my_str_simd_sse insert
        start = std::chrono::high_resolution_clock::now();
        base_simd.insert(insert_pos, insert_simd);
        end = std::chrono::high_resolution_clock::now();
        auto simd_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_simd_sse insert time: " << simd_time << " microseconds\n";

        if (simd_time < normal_time) {
            std::cout << "SIMD version is faster by " << (normal_time - simd_time) << " microseconds!\n";
        } else {
            std::cout << "Normal version is faster by " << (simd_time - normal_time) << " microseconds!\n";
        }
        bool success = true;
        const char* data = base_simd.c_str();
        for (size_t i = 0; i < insert_pos; ++i) {
            if (data[i] != base_char) {
                success = false;
                break;
            }
        }
        for (size_t i = insert_pos; i < insert_pos + insert_size; ++i) {
            if (data[i] != insert_char) {
                success = false;
                break;
            }
        }
        for (size_t i = insert_pos + insert_size; i < base_simd.size(); ++i) {
            if (data[i] != base_char) {
                success = false;
                break;
            }
        }
        if (success) {
            std::cout << "Test passed: content verified.\n";
        } else {
            std::cout << "Test failed: content mismatch!\n";
        }
    }
}

void test_insert_cstr_speed_different_sizes() {
    const std::vector<size_t> base_sizes = {1024, 10 * 1024, 100 * 1024, 1 * 1024 * 1024, 10 * 1024 * 1024}; // 1КБ, 10КБ, 100КБ, 1МБ, 10МБ
    const std::vector<size_t> insert_sizes = {8, 64, 512, 4096};
    const char base_char = 'A';
    const char insert_char = 'B';

    for (size_t base_size : base_sizes) {
        for (size_t insert_size : insert_sizes) {
            size_t insert_pos = base_size / 2;

            std::string base_source(base_size, base_char);
            std::string insert_source(insert_size, insert_char);

            my_str_t base_normal(base_source);
            my_str_simd_sse base_simd(base_source);

            const char* cstr_insert = insert_source.c_str();

            std::cout << "\n=== Testing insert (base: " << base_size/1024 << " KB, insert: " << insert_size << " bytes) ===\n";

            auto start = std::chrono::high_resolution_clock::now();
            base_normal.insert(insert_pos, cstr_insert);
            auto end = std::chrono::high_resolution_clock::now();
            auto normal_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "my_str_t insert time: " << normal_time << " microseconds\n";

            start = std::chrono::high_resolution_clock::now();
            base_simd.insert(insert_pos, cstr_insert);
            end = std::chrono::high_resolution_clock::now();
            auto simd_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "my_str_simd_sse insert time: " << simd_time << " microseconds\n";

            if (simd_time < normal_time) {
                std::cout << "SIMD version is faster by " << (normal_time - simd_time) << " microseconds!\n";
            } else {
                std::cout << "Normal version is faster by " << (simd_time - normal_time) << " microseconds!\n";
            }
            bool success = true;
            const char* data = base_simd.c_str();
            for (size_t i = 0; i < insert_pos; ++i) {
                if (data[i] != base_char) {
                    success = false;
                    break;
                }
            }
            for (size_t i = insert_pos; i < insert_pos + insert_size; ++i) {
                if (data[i] != insert_char) {
                    success = false;
                    break;
                }
            }
            for (size_t i = insert_pos + insert_size; i < base_simd.size(); ++i) {
                if (data[i] != base_char) {
                    success = false;
                    break;
                }
            }

            if (success) {
                std::cout << "Test passed: content verified.\n";
            } else {
                std::cout << "Test failed: content mismatch!\n";
            }
        }
    }
}


void test_erase_speed_different_sizes() {
    const std::vector<size_t> base_sizes = {1024, 10 * 1024, 100 * 1024, 1 * 1024 * 1024, 10 * 1024 * 1024}; // 1КБ, 10КБ, 100КБ, 1МБ, 10МБ
    const size_t erase_size = 512;
    const char base_char = 'A';
    for (size_t base_size : base_sizes) {
        size_t erase_pos = base_size / 2;
        std::string base_source(base_size, base_char);

        my_str_t base_normal(base_source);
        my_str_simd_sse base_simd(base_source);

        std::cout << "\n=== Testing erase (base size: " << base_size / 1024 << " KB) ===\n";

        auto start = std::chrono::high_resolution_clock::now();
        base_normal.erase(erase_pos, erase_size);
        auto end = std::chrono::high_resolution_clock::now();
        auto normal_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_t erase time: " << normal_time << " microseconds\n";

        start = std::chrono::high_resolution_clock::now();
        base_simd.erase(erase_pos, erase_size);
        end = std::chrono::high_resolution_clock::now();
        auto simd_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_simd_sse erase time: " << simd_time << " microseconds\n";

        if (simd_time < normal_time) {
            std::cout << "SIMD version is faster by " << (normal_time - simd_time) << " microseconds!\n";
        } else {
            std::cout << "Normal version is faster by " << (simd_time - normal_time) << " microseconds!\n";
        }

        bool success = true;
        const char* data = base_simd.c_str();
        for (size_t i = 0; i < erase_pos; ++i) {
            if (data[i] != base_char) {
                success = false;
                break;
            }
        }
        for (size_t i = erase_pos; i < base_simd.size(); ++i) {
            if (data[i] != base_char) {
                success = false;
                break;
            }
        }
        if (success) {
            std::cout << "Test passed: content verified.\n";
        } else {
            std::cout << "Test failed: content mismatch!\n";
        }
    }
}

void test_find_speed() {
    const std::vector<size_t> sizes = {1 * 1024 * 1024, 10 * 1024 * 1024, 100 * 1024 * 1024}; // 1МБ, 10МБ, 100МБ
    const char target_char = 'A';

    for (size_t size : sizes) {
        std::string base_string(size, 'B');
        base_string[size / 2] = target_char;

        my_str_t normal(base_string);
        my_str_simd_sse simd(base_string);

        std::cout << "\n=== Testing find (size: " << size / (1024 * 1024) << " MB) ===\n";
        auto start = std::chrono::high_resolution_clock::now();
        size_t normal_pos = normal.find(target_char, 0);
        auto end = std::chrono::high_resolution_clock::now();
        auto normal_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_t find time: " << normal_time << " microseconds\n";

        start = std::chrono::high_resolution_clock::now();
        size_t simd_pos = simd.find(target_char, 0);
        end = std::chrono::high_resolution_clock::now();
        auto simd_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_simd_sse find time: " << simd_time << " microseconds\n";

        if (normal_pos != simd_pos) {
            std::cout << "Test failed: positions do not match!\n";
        } else {
            std::cout << "Test passed: found at position " << simd_pos << ".\n";
        }
        if (simd_time < normal_time) {
            std::cout << "SIMD version is faster by " << (normal_time - simd_time) << " microseconds!\n";
        } else {
            std::cout << "Normal version is faster by " << (simd_time - normal_time) << " microseconds!\n";
        }
    }
}

void test_find_substring_speed() {
    const std::vector<size_t> sizes = {1 * 1024 * 1024, 10 * 1024 * 1024, 100 * 1024 * 1024}; // 1MB, 10MB, 100MB
    const std::string substr = "CDE";
    for (size_t size : sizes) {
        // рядок, заповнений 'A', вставляємо "CDE" у середину
        std::string base_string(size, 'A');
        size_t insert_pos = size / 2;
        if (insert_pos + substr.size() < size) {
            base_string[insert_pos] = 'C';
            base_string[insert_pos + 1] = 'D';
            base_string[insert_pos + 2] = 'E';
        }

        my_str_t normal(base_string);
        my_str_simd_sse simd(base_string);

        std::cout << "\n=== Testing find substring 'CDE' (size: " << size / (1024 * 1024) << " MB) ===\n";
        auto start = std::chrono::high_resolution_clock::now();
        size_t normal_pos = normal.find(substr, 0);
        auto end = std::chrono::high_resolution_clock::now();
        auto normal_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_t find time: " << normal_time << " microseconds\n";

        start = std::chrono::high_resolution_clock::now();
        size_t simd_pos = simd.find(substr, 0);
        end = std::chrono::high_resolution_clock::now();
        auto simd_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_simd_sse find time: " << simd_time << " microseconds\n";

        if (normal_pos != simd_pos) {
            std::cout << "Test failed: positions do not match! (normal=" << normal_pos << ", simd=" << simd_pos << ")\n";
        } else {
            std::cout << "Test passed: found at position " << simd_pos << ".\n";
        }

        if (simd_time < normal_time) {
            std::cout << "SIMD version is faster by " << (normal_time - simd_time) << " microseconds\n";
        } else {
            std::cout << "Normal version is faster by " << (simd_time - normal_time) << " microseconds\n";
        }
    }
}

void test_find_substring_speed_2() {
    const std::vector<size_t> sizes = {1 * 1024 * 1024, 10 * 1024 * 1024, 100 * 1024 * 1024};
    const char* substr = "CDE";
    for (size_t size : sizes) {
        std::string base_string(size, 'A');
        size_t insert_pos = size / 2;
        if (insert_pos + 3 <= size) {
            base_string[insert_pos] = 'C';
            base_string[insert_pos + 1] = 'D';
            base_string[insert_pos + 2] = 'E';
        }
        my_str_t normal(base_string);
        my_str_simd_sse simd(base_string);

        std::cout << "\n=== Testing find(const char*) 'CDE' (size: " << size / (1024 * 1024) << " MB) ===\n";
        auto start = std::chrono::high_resolution_clock::now();
        size_t normal_pos = normal.find(substr, 0);
        auto end = std::chrono::high_resolution_clock::now();
        auto normal_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_t find time: " << normal_time << " microseconds\n";

        start = std::chrono::high_resolution_clock::now();
        size_t simd_pos = simd.find(substr, 0);
        end = std::chrono::high_resolution_clock::now();
        auto simd_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "my_str_simd_sse find time: " << simd_time << " microseconds\n";

        if (normal_pos != simd_pos) {
            std::cout << "Test failed: positions do not match! (normal=" << normal_pos << ", simd=" << simd_pos << ")\n";
        } else {
            std::cout << "Test passed: found at position " << simd_pos << ".\n";
        }
        if (simd_time < normal_time) {
            std::cout << "SIMD version is faster by " << (normal_time - simd_time) << " microseconds!\n";
        } else {
            std::cout << "Normal version is faster by " << (simd_time - normal_time) << " microseconds!\n";
        }
    }
}

void test_performance() {
    std::string big_str(16, 'x');
    auto start = std::chrono::high_resolution_clock::now();
    my_str_t normal(big_str);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Normal Copy Time std:: string: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " microseconds\n";

    start = std::chrono::high_resolution_clock::now();
    my_str_simd_sse simd(big_str);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "SIMD Copy Time std:: string : "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " microseconds\n";
}



void test_reserve_speed_table() {
    const size_t sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 10000, 1000000, 10000000};
    const int warmup_runs = 100;
    const int test_runs_small = 1000;
    const int test_runs_large = 10;

    std::cout << "| Size (chars) | reserve memcpy (μs) | reserve SIMD (μs) |\n";
    std::cout << "|--------------|---------------------|-------------------|\n";


    for (size_t size : sizes) {
        std::string source(size, 'x');
        int runs = (size >= 1000000) ? test_runs_large : test_runs_small;
        my_str_simd_sse str(source);
        volatile size_t checksum = 0;
        for (int i = 0; i < warmup_runs; ++i) {
            my_str_simd_sse tmp = str;
            tmp.reserve(tmp.capacity() + 10);
            checksum += tmp.size();
        }

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < runs; ++i) {
            my_str_simd_sse tmp = str;
            tmp.reserve(tmp.capacity() + 10);
            checksum += tmp.size();
        }
        auto end = std::chrono::steady_clock::now();
        auto memcpy_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / runs;
        start = std::chrono::steady_clock::now();
        for (int i = 0; i < runs; ++i) {
            my_str_simd_sse tmp = str;
            tmp.reserve(tmp.capacity() + 10);
            checksum += tmp.size();
        }
        end = std::chrono::steady_clock::now();
        auto simd_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / runs;
        std::cout << "| " << size
                  << " | " << memcpy_time
                  << " | " << simd_time << " |\n";
    }

}

void test_shrink_to_fit_speed() {
    const size_t sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 10000, 1000000, 10000000};
    const int warmup_runs = 100;
    const int test_runs_small = 1000;
    const int test_runs_large = 10;

    std::cout << "| Size (chars) | shrink memcpy (μs) | shrink SIMD (μs) |\n";
    std::cout << "|--------------|--------------------|------------------|\n";

    for (size_t size : sizes) {
        std::string source(size, 'x');
        int runs = (size >= 1000000) ? test_runs_large : test_runs_small;
        volatile size_t checksum = 0;
        my_str_simd_sse str(source);
        for (int i = 0; i < warmup_runs; ++i) {
            my_str_simd_sse tmp = str; // копія
            tmp.shrink_to_fit();
            checksum += tmp.size();
        }

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < runs; ++i) {
            my_str_simd_sse tmp = str;
            tmp.shrink_to_fit();
            checksum += tmp.size();
        }
        auto end = std::chrono::steady_clock::now();
        auto memcpy_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / runs;
        start = std::chrono::steady_clock::now();
        for (int i = 0; i < runs; ++i) {
            my_str_simd_sse tmp = str;
            tmp.shrink_to_fit();
            checksum += tmp.size();
        }
        end = std::chrono::steady_clock::now();
        auto simd_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / runs;
        std::cout << "| " << size
                  << " | " << memcpy_time
                  << " | " << simd_time << " |\n";
    }

}

void test_insert_speed() {
    const size_t sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 10000, 1000000};
    const int warmup_runs = 100;
    const int test_runs_small = 1000;
    const int test_runs_large = 10;

    std::cout << "| Size (chars) | insert memcpy/memmove (μs) | insert SIMD (μs) |\n";
    std::cout << "|--------------|----------------------------|-----------------|\n";

    for (size_t size : sizes) {
        std::string source(size, 'x');
        std::string insert_str_content(size / 10, 'y');
        // вставка 10% від розміру основного рядка
        int runs = (size >= 1000000) ? test_runs_large : test_runs_small;
        volatile size_t checksum = 0;
        my_str_simd_sse str(source);
        my_str_simd_sse insert_str(insert_str_content);
        size_t insert_position = size / 2;
        for (int i = 0; i < warmup_runs; ++i) {
            my_str_simd_sse tmp = str;
            tmp.insert(insert_position, insert_str);
            checksum += tmp.size();
        }
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < runs; ++i) {
            my_str_simd_sse tmp = str;
            tmp.insert(insert_position, insert_str);
            checksum += tmp.size();
        }
        auto end = std::chrono::steady_clock::now();
        auto memcpy_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / runs;
        start = std::chrono::steady_clock::now();
        for (int i = 0; i < runs; ++i) {
            my_str_simd_sse tmp = str;
            tmp.insert(insert_position, insert_str);
            checksum += tmp.size();
        }
        end = std::chrono::steady_clock::now();
        auto simd_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / runs;
        std::cout << "| " << size
                  << " | " << memcpy_time
                  << " | " << simd_time << " |\n";
    }

}




int main() {
    test_performance();
    test_copy_speed();
    test_reserve_speed();
    test_shrink_to_fit_speed();
    test_insert_speed();
    test_reserve_speed();

    // // Для my_str_simd_sse
    my_str_simd_sse test1("Hello, C++ World!");
    const char* compare1 = "Hello, C++ World!";
    my_str_simd_sse test2("Hello everyone, World!");
    const char* compare2 = "Hello everyone, World!";
    std::string big_string(10'000'000, 'x'); // 10МБ 'x'
    my_str_simd_sse big_test(big_string);
    const char* big_compare = big_string.c_str();
    test_operator_eq_one_simd(compare1, test1, 10000);
    test_operator_eq_one_simd(compare2, test2, 10000);
    test_operator_eq_one_simd(big_compare, big_test, 1);
    // Для my_str_t
    my_str_t test11("Hello, C++ World!");
    const char* compare11 = "Hello, C++ World!";
    my_str_t test22("Hello everyone, World!");
    const char* compare22 = "Hello everyone, World!";
    std::string big_string_11(10'000'000, 'x'); // 10МБ 'x'
    my_str_t big_test11(big_string_11);
    const char* big_compare11 = big_string_11.c_str();
    test_operator_eq_one(compare11, test11, 10000);
    test_operator_eq_one(compare22, test22, 10000);
    test_operator_eq_one(big_compare11, big_test11, 1);


    my_str_simd_sse test_heloo("Hello, C++ World!");
    const char* compare111 = "Hello, C++ World!";
    my_str_simd_sse test2222("Hello everyone, World!");
    const char* compare222 = "Hello everyone, World!";
    std::string bigwww_string(10'000'000, 'x');
    my_str_simd_sse bigwe_test(big_string);
    const char* bigwe_compare = big_string.c_str();
    std::cout << "Testing operator==(my_str_simd_sse, const char*):\n";
    test_operator_eq_simd(test_heloo, compare111, 10000);
    test_operator_eq_simd(test2222, compare222, 10000);
    test_operator_eq_simd(bigwe_test, bigwe_compare, 1);


    my_str_t test_theloo("Hello, C++ World!");
    const char* compare_t111 = "Hello, C++ World!";
    my_str_t test_t2222("Hello everyone, World!");
    const char* compare222_t = "Hello everyone, World!";
    std::string bigwww_string_t(10'000'000, 'x');
    my_str_t bigwe_test_t(big_string);
    const char* bigwe_compare_t = big_string.c_str();
    std::cout << "Testing operator==(my_str_simd_sse, const char*):\n";
    test_operator_eq_norm(test_theloo, compare_t111, 10000);
    test_operator_eq_norm(test_t2222, compare222_t, 10000);
    test_operator_eq_norm(bigwe_test_t, bigwe_compare_t, 1);


    my_str_simd_sse test1_eq_eq("Hello, SIMD World!");
    my_str_simd_sse test2_eq_eq("Hello, SIMD World!");
    my_str_simd_sse big_test1(std::string(10'000'000, 'x'));
    my_str_simd_sse big_test2(std::string(10'000'000, 'x'));
    std::cout << "Testing operator==(my_str_simd_sse, my_str_simd_sse):\n";
    test_operator_eq_simd(test1_eq_eq, test2_eq_eq, 10000);
    test_operator_eq_simd(big_test1, big_test2, 1);

    my_str_t test1_eq_eq_t("Hello, SIMD World!");
    my_str_t test2_eq_eq_t("Hello, SIMD World!");
    my_str_t big_test1_t(std::string(10'000'000, 'x'));
    my_str_t big_test2_t(std::string(10'000'000, 'x'));
    std::cout << "Testing operator==(my_str_simd_sse, my_str_simd_sse):\n";
    test_operator_eq_norm_eq_eq(test1_eq_eq_t, test2_eq_eq_t, 10000);
    test_operator_eq_norm_eq_eq(big_test1_t, big_test2_t, 1);
    test_shrink_to_fit_speed();
    test_resize_speed_different_sizes();
    test_insert_speed_different_sizes();
    test_insert_cstr_speed_different_sizes();
    test_erase_speed_different_sizes();
    test_find_speed();
    test_find_substring_speed();
    test_find_substring_speed_2();
    test_reserve_speed_table();
    return 0;
}