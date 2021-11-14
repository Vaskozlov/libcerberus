#include <deque>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <cerberus/string_view.hpp>
#include <fmt/format.h>
#include <fmt/color.h>
#include "C/lexer4C_imp.hpp"
#include <cerberus/deque2.hpp>
#include <cerberus/range.hpp>
#include <cerberus/benchmark.hpp>

using namespace cerb::literals;
using namespace std::string_view_literals;

const char *const input = R"(
    int main(int argc, char **argv) {
        int a = 10;
        int b = 20;
        char *c = "Hello, World!\xff";
        char d = 't';

          /*
        This is a comment!
    */

        float a2 = 20.0f;

        a += b;
        b *= a;

        return a + b;
    }
)";

consteval auto test() -> size_t
{
    cerb::experimental::Deque<int> ideque{};

    CERBLIB_UNROLL_N(2)
    for (int i = 0; i < 100; i++) {
        ideque.emplace_back(i);
        ideque.emplace_front(-i);
    }
    auto ideque2 = ideque;

    ideque = ideque2;
    return ideque.size();
}


auto deque_test() -> int
{
    //test();
    auto begin = std::chrono::high_resolution_clock::now();
    {
        cerb::experimental::Deque<int> ideque{};
        cerb::experimental::Deque<std::string> deque{};
        int i = 0;

        cerb::experimental::Benchmark(
            [](const auto elapsed, const char * /*name*/) {
                fmt::print("pushing elapsed in {:e}\n", elapsed.count());
            },
            [&]() {
                ideque.emplace_back(i);
                ideque.emplace_front(-i);
                deque.emplace_front(
                    fmt::format("Hello, world! It's a long string {}", i));
                deque.emplace_back(
                    fmt::format("Hello, world! It's a long string {}", -i));
                i++;
            });

        auto ideque2 = ideque;
        auto deque2  = deque;

        CERBLIB_UNROLL_N(2)
        for (auto _ : cerb::range(8)) {
            deque.pop_back();
            deque.pop_front();
        }
        deque.pop_back();

        deque = std::move(deque2);

        fmt::print("1: {} {}\n", deque.front(), deque.back());
        fmt::print("2: {} {}\n", deque2.front(), deque2.back());

        for (auto it = deque.rbegin(); it != deque.rend(); it--) {
            std::cout << *it << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - begin;
    fmt::print("{:e}\n", elapsed.count());

    return 0;
}



auto main() -> int
{

    int test[10];

    //CalculatorImp calculator{};
    Lex4CImp C_lexer{};

    // calculator.scan("50 + 20", "stdio");
    auto begin = std::chrono::high_resolution_clock::now();
    // calculator.scan("sin(20) + 30", "stdio");
    C_lexer.scan(input, "stdio");
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - begin;
    fmt::print("{:e}\n", elapsed.count());

     return 0;
}

/*
 * TODO:
 *  1) if string is to long print half on it on error
 *
 *  1) как генерировать таблицу автоматически
 *
 */