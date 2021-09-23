#include <deque>
#include <chrono>
#include <iostream>
#include <cerberus/set.hpp>
#include <cerberus/map.hpp>
#include <cerberus/hash.hpp>
#include <cerberus/address.hpp>
#include <cerberus/deque.hpp>
#include <cerberus/container.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/lex/lex.hpp>
#include <cerberus/vector.hpp>
#include <random>
#include <cerberus/string_view.hpp>
#include "lexical_generator.hpp"

using namespace cerb::literals;
using namespace std::string_view_literals;

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

    constexpr void yield(size_t first, size_t last) override
    {
        for (; first != last; ++first) {
            auto &token = m_tokens[first];
            std::cout << std::setw(8) << std::left << token.repr << ' ' << token.type
                      << ' ' << token.pos + 1 << std::endl;
        }
    }

    constexpr void error(const position_t &pos, const string_view_t &repr) override
    {
        std::cout << "Lexical analyzer error! At: " << pos
                  << ", token repr: " << repr << std::endl;
        throw std::runtime_error("");
    }

    constexpr CERBERUS_LEX_INITIALIZER(Lex)
    {}
};

CERBERUS_LEX_TEMPLATES
struct Calculator : public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS

    size_t TokenIndex{};
    token_t Token{};

    void input() {
        Token = m_tokens[TokenIndex++];

        switch (Token.type){
        case 2:
        case 5:
            expression(); break;
        default:
            error();
        }
    }

    void expression()
    {
        switch (Token.type){
        case 2:
        case 5:
            term(); rest_expression(); break;
        default:
            error();
        }
    }

    void term()
    {
        switch (Token.type) {
        case 2:
            token(2); break;

        case 5:
            parenthesized_expression(); break;
        default:
            error();
        }
    }

    void parenthesized_expression()
    {
        switch (Token.type) {
        case 5:
            token(5); expression(); token(6);
            break;

        default:
            error();
        }
    }

    void rest_expression()
    {
        switch (Token.type) {
        case 3:
            token(3); expression(); break;
        case 6:
            break;
        default:
            error();
        }
    }

    void token(TokenType tk)
    {
       if (tk != Token.type) error();
       Token = m_tokens[TokenIndex++];
    }

    void error()
    {
        throw std::runtime_error("Syntax error!");
    }

    constexpr void yield(size_t first, size_t last) override
    {
        for (; first != last; ++first) {
            auto &token = m_tokens[first];
            std::cout << std::setw(8) << std::left << token.repr << ' ' << token.type
                      << ' ' << token.pos + 1 << std::endl;
        }
    }

    constexpr void error(const position_t &pos, const string_view_t &repr) override
    {
        std::cout << "Lexical analyzer error! At: " << pos
                  << ", token repr: " << repr << std::endl;
        throw std::runtime_error("");
    }

    constexpr CERBERUS_LEX_INITIALIZER(Calculator)
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

Calculator<char, int> calcualtor(
    { { 0, "sin"_sv }, { 1, "cos"_sv } },
    { { 2, "[0-9]+" } },
    { { { 3, '+' }, { 4, '-' }, { 5, '(' }, { 6, ')' } }, {} });

auto main(int argc, char *argv[]) -> int
{
    const char *input = R"(
    int main(int argc, char **argv) {
        int a = 10;
        int b = 20;

        float a2 = 20.0f;

        a += b;
        b *= a;

        return a + b;
    }
)";

    // controller.scan(input, "stdio");

    /*calcualtor.scan("50 + (50 + 20)", "stdio");
    calcualtor.input();
*/
    main2(argc, argv);

    // puts(input);

     //item2.scan("1+2 + 3+4", "stdio");

    /*
    size_t Iterations = cerb::test::uniform_dist(cerb::test::random_engine);
    std::vector<cerb::ConstructorChecker> std_vector{};
    cerb::Vector<cerb::ConstructorChecker> cerb_vector{};

    {
        auto begin = std::chrono::high_resolution_clock::now();
        CERBLIB_UNROLL_N(16)
        for (size_t i = 0; i < Iterations; ++i) {
            std_vector.push_back({});
            std_vector.push_back({});
            std_vector.emplace_back();
            std_vector.pop_back();
        }
        std_vector.insert(
            std_vector.begin() + Iterations % 1551, { {}, {}, {}, {} });
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - begin;
        printf("Std vector: %e\n", elapsed.count());
        std_vector.clear();
    }

    if (cerb::ConstructorChecker::counter() != 0) {
        throw std::runtime_error(
            "Not all elements were freed ("s +
            std::to_string(cerb::ConstructorChecker::counter()) + ")! at "s +
            std::string(__FILE__) + " "s + std::to_string(__LINE__));
    }

    std::cout << "Iterations: " << Iterations << ", "
              << cerb::ConstructorChecker{ 1 } << std::endl;
    cerb::ConstructorChecker::clear();

    {
        auto begin = std::chrono::high_resolution_clock::now();
        CERBLIB_UNROLL_N(16)
        for (size_t i = 0; i < Iterations; ++i) {
            cerb_vector.push_back({});
            cerb_vector.push_back({});
            cerb_vector.emplace_back();
            cerb_vector.pop_back();
        }
        // fix insert
        //cerb_vector.insert(Iterations % 1551, { {}, {}, {}, {} });
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - begin;
        printf("Cerb vector: %e\n", elapsed.count());
        cerb_vector.clear();
    }

    if (cerb::ConstructorChecker::counter() != 0) {
        throw std::runtime_error(
            "Not all elements were freed ("s +
            std::to_string(cerb::ConstructorChecker::counter()) + ")! at "s +
            std::string(__FILE__) + " "s + std::to_string(__LINE__));
    }

    std::cout << "Iterations: " << Iterations << ", "
              << cerb::ConstructorChecker{ 1 } << std::endl;
    cerb::ConstructorChecker::clear();
    */

    return 0;
}

/*
 * TODO:    1) do not rely on compiler and check builtin function manually
 *          2) isolate token should check for terminals (or print hole line and show
 *          error's position
 */