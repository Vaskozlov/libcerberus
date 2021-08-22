#include <deque>
#include <chrono>
#include <iostream>
#include <cerberus/set.hpp>
#include <cerberus/map.hpp>
#include <cerberus/address.hpp>
#include <cerberus/deque.hpp>
#include <cerberus/container.hpp>
#include <cerberus/bitmap.hpp>

using namespace cerb::literals;

struct S
{
    int a;

    S(int b) : a(b){};

    ~S()
    {
        std::cout << "destroyed " << a << std::endl;
    }
};

auto main(int /*argc*/, char * /*argv*/[]) -> int
{

    {
        srand(0);
        std::deque<int> a{};
        auto value = rand();
        auto begin = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < 1000; ++i) {
            a.emplace_back(value);
        }

        for (size_t i = 0; i < 1000; ++i) {
            a.emplace_front(value);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - begin;
        printf("std::deque %e\n", elapsed.count());
    }

    {
        srand(0);
        cerb::Deque<int, 128> a{};
        auto value = rand();
        auto begin = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < 1000; ++i) {
            a.emplace_back(value);
        }

        for (size_t i = 0; i < 1000; ++i) {
            a.emplace_front(value);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - begin;
        printf("cerb::Deque %e\n", elapsed.count());
    }


    cerb::Deque<int, 8> a;
    a.push_back(10);
    a.push_back(20);
    a.emplace_front(1);

    std::cout << a.front() << std::endl;


    return 0;
}

/*
 * TODO:    1) do not rely on compiler and check builtin function manually
 *
 */