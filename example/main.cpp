#include <deque>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <cerberus/string_view.hpp>
#include <fmt/format.h>
#include <fmt/color.h>
#include "C_lexer.hpp"
#include "calculator.hpp"

using namespace cerb::literals;
using namespace std::string_view_literals;

/*
enum TokenType : u32
{
    PLUS,
    SUB,
    MUL,
    DIV,
    PERCENT,
    PLUS_EQ,
    SUB_EQ,
    MUL_EQ,
    DIV_EQ,
    PERCENT_EQ,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    LEFT_SHIFT_EQ,
    RIGHT_SHIFT_EQ,
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,
    QUESTION_MARK,
    COLON,
    NOT,
    AND,
    OR,
    XOR,
    AND_EQ,
    OR_EQ,
    XOR_EQ,
    ASSIGN,
    EQUAL,
    NOT_EQUAL,
    GREATER,
    LESS,
    GREATER_EQ,
    LESS_EQ,
    FOR,
    WHILE,
    RETURN,
    SWITCH,
    CASE,
    CHAR,
    INT,
    LONG,
    SHORT,
    SIGNED,
    UNSIGNED,
    FLOAT_T,
    DOUBLE_T,
    IDENTIFIER,
    INTEGERAL,
    FLOAT,
    DOUBLE,
    CURLY_OPENING,    // '{'
    CURLY_CLOSING,    // '}'
    LEFT_PARENTHESIS, // '('
    RIGHT_PARENTHESIS,// ')'
    ANGLE_OPENING,    // '['
    ANGLE_CLOSING,    // ']'
    COMMA,            // ','
    SEPARATOR,        // ';'
    PREPROCESSOR,     // '#'
};

CERBERUS_LEX_TEMPLATES
struct Lex : public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS

    constexpr void yield(const token_t &token) override
    {
        std::cout << std::setw(8) << std::left << token.repr << ' ' << token.type
                  << ' ' << token.pos << std::setw(0) << std::endl;
    }

    constexpr void error(
        const position_t &pos, const string_view_t &line,
        const string_view_t &repr) override
    {
        fmt::print(
            "Lexical analyzer error! At: file: {}, line: {}, column: {}\n",
            pos.filename.to_string(), pos.line_number, pos.char_number);

        fmt::print(fmt::emphasis::italic, "{}\n", line.to_string());
        CERBLIB_UNROLL_N(4)
        for (int i = 0; i < static_cast<int>(repr.begin() - line.begin()); ++i) {
            putchar(' ');
        }
        fmt::print(fmt::fg(fmt::color::green), "^\n");
        fmt::print("Representation of token: {}", repr.to_string());
        std::cout << std::endl;
        throw std::runtime_error("");
    }

    constexpr CERBERUS_LEX_INITIALIZER(Lex)
    {}
};

Lex<char, TokenType> controller{ { { FOR, "for"_sv },
                                   { WHILE, "while"_sv },
                                   { CHAR, "char"_sv },
                                   { INT, "int"_sv },
                                   { RETURN, "return"_sv },
                                   { SWITCH, "switch"_sv },
                                   { CASE, "case"_sv },
                                   { FLOAT_T, "float"_sv },
                                   { DOUBLE_T, "double"_sv },
                                   { LONG, "long"_sv },
                                   { SHORT, "short"_sv },
                                   { SIGNED, "signed"_sv },
                                   { UNSIGNED, "unsigned"_sv } },
                                 { { IDENTIFIER, "[a-zA-Z_]+[a-zA-Z0-9_]*"_sv },
                                   { INTEGERAL, "[0-9]+"_sv },
                                   { FLOAT, "[0-9]+[.][0-9]+[f]"_sv },
                                   { DOUBLE, "[0-9]+[.][0-9]+"_sv } },
                                 { { { PLUS, '+' },
                                     { SUB, '-' },
                                     { MUL, '*' },
                                     { DIV, '/' },
                                     { GREATER, '>' },
                                     { LESS, '<' },
                                     { ASSIGN, '=' },
                                     { CURLY_OPENING, '{' },
                                     { CURLY_CLOSING, '}' },
                                     { LEFT_PARENTHESIS, '(' },
                                     { RIGHT_PARENTHESIS, ')' },
                                     { ANGLE_OPENING, '[' },
                                     { ANGLE_CLOSING, ']' },
                                     { COMMA, ',' },
                                     { SEPARATOR, ';' },
                                     { AND, '&' },
                                     { OR, '|' },
                                     { XOR, '^' },
                                     { NOT, '~' },
                                     { LOGICAL_NOT, '!' },
                                     { PREPROCESSOR, '#' },
                                     { QUESTION_MARK, '?' },
                                     { COLON, ':' },
                                     { PERCENT, '%' } },
                                   {
                                       { PLUS_EQ, "+="_sv },
                                       { SUB_EQ, "-="_sv },
                                       { MUL_EQ, "*="_sv },
                                       { DIV_EQ, "/="_sv },
                                       { PERCENT_EQ, "%="_sv },
                                       { GREATER_EQ, ">="_sv },
                                       { LESS_EQ, "<="_sv },
                                       { EQUAL, "=="_sv },
                                       { NOT_EQUAL, "!="_sv },
                                       { LEFT_SHIFT, "<<"_sv },
                                       { RIGHT_SHIFT, ">>"_sv },
                                       { LEFT_SHIFT_EQ, "<<="_sv },
                                       { RIGHT_SHIFT_EQ, ">>=" },
                                       { LOGICAL_AND, "&&"_sv },
                                       { LOGICAL_OR, "||"_sv },
                                       { AND_EQ, "&="_sv },
                                       { OR_EQ, "|="_sv },
                                       { XOR_EQ, "^="_sv },
                                   } } };
                                   */

CalculatorTemplate struct CalculatorImp final : public Calculator<>
{
    CalculatorAccess;

    constexpr auto yield(const token_t &token) -> bool override
    {
        fmt::print(
            "{:<16} {:<20} ", token.repr.to_string(),
            CalculatorItemItemsNames[token.type].to_string());

        std::cout << token.pos << std::endl;

        return true;
    }

    constexpr auto error(const item_t &item, const string_view_t &repr)
        -> void override
    {
        cerb::analysis::basic_lexical_error(
            item, repr, "Unable to find suitable dot item for: ");
    }

    constexpr auto finish() -> void override
    {}
};

auto main(int argc, char *argv[]) -> int
{
    CalculatorImp calculator{};

    static const char *input = R"(
    int main(int argc, char **argv) {
        int a = 10;
        int b = 20;
        char *c = "Hello, World!\xff";
        char d = 'x';

        float a2 = 20.0f;

        a += b;
        b *= a;

        return a + b;
    }
)";


    // controller.scan(input, "stdio");
    calculator.scan("sin(50) + cos(50 + 20)", "stdio");
    // calculator_u16.scan(u"\"Hello!\" sin(50) + cos(50 + 20)", u"stdio");
    //     calculator.input();


    return 0;
}

/*
 * TODO:
 *  1) if string is to long print half on it on error
 *  2) add an opportunity to set block for char, string (and make them optional!)
 */