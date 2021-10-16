#include <deque>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <cerberus/string_view.hpp>
#include <fmt/format.h>
#include <fmt/color.h>
#include "calculator_imp.hpp"
#include "lexer4C_imp.hpp"

using namespace cerb::literals;
using namespace std::string_view_literals;

const char *const input = R"(
    int main(int argc, char **argv) {
        int a = 10;
        int b = 20;
        char *c = "Hello, World!\xff";
        char d = 'x';

          /*
        This is a comment!
    */

        float a2 = 20.0f;

        a += b;
        b *= a;

        return a + b;
    }
)";

constexpr u32 SyntaxError = std::numeric_limits<u32>::max();

auto main() -> int
{
    CalculatorImp calculator{};
    Lex4CImp C_lexer{};

    // calculator.scan("50 + 20", "stdio");
    calculator.scan("20 + 30", "stdio");
    // C_lexer.scan(input, "stdio");

    return 0;
}

/*
 * TODO:
 *  1) if string is to long print half on it on error
 *
 *  1) как генеировать таблицу автоматически
 *
 */