#include <iostream>
#include <random>
#include <chrono>
#include "HT.h"

std::string GenerateWord(std::random_device& rd, std::default_random_engine& dre, size_t size) {
    const int LETTES_SIZE = 26;
    const char LETTERS[LETTES_SIZE] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

    std::uniform_int_distribution<int> rnd_let(0, LETTES_SIZE - 1);

    std::string word = "";

    for (int i = 0; i < size; i++) {
        word += LETTERS[rnd_let(dre)];
    }

    return word;
}

int main() {
    const int WORD_COUNT = 6;
    const int MAX_ORDER = 5;

    static std::random_device rd;
    static std::default_random_engine dre(rd());
    std::uniform_int_distribution<int> rnd_num(0, MAX_ORDER * 1000);

    HT::HashTable<int>* ht = new HT::HashTable<int>();

    for (int i = 1; i <= MAX_ORDER; i++) {
        std::cout << "--------------------------------" << std::endl;
        std::cout << "Test: " << i << std::endl << std::endl;

        int n = pow(10, i);

        std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
        for (int j = 1; j <= n; j++) {
            std::string key = GenerateWord(rd, dre, WORD_COUNT);
            int value = rnd_num(dre);
            try {
                ht->Push(key, value);
            }
            catch (const std::exception& ex) {
                std::cerr << "Eror in push " << j << " -> " << ex.what() << std::endl;
            }
        }
        std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> pushing_time = end_time - start_time;
        std::cout << "Pushing time: " << pushing_time.count() << "s" << std::endl;
        std::cout << ht->ToString(8) << std::endl;

        int hits = 0;
        const int m = pow(10, 4);

        start_time = std::chrono::high_resolution_clock::now();
        for (int j = 1; j <= m; j++) {
            try {
                std::string key = GenerateWord(rd, dre, WORD_COUNT);
                /*HT::HashTable<int>::Node* entry = ht->Find(key);
                if (entry) {
                    hits++;
                }*/
            }
            catch (const std::exception& ex) {
                std::cerr << "Eror in find " << j << " -> " << ex.what() << std::endl;
            }
        }
        end_time = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> removing_time = end_time - start_time;
        std::cout << "Removing time: " << removing_time.count() << "s" << std::endl;
        std::cout << "Hits: " << hits << std::endl;
        std::cout << ht->ToString(8) << std::endl;

        double total_time = pushing_time.count() + removing_time.count();
        std::cout << "Total time: " << total_time << "s" << std::endl;

        ht->Erase();
    }

    delete ht;
    return 0;
}