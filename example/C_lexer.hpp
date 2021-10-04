#ifndef CERBERUS_LEXER_C
#define CERBERUS_LEXER_C

#include <iostream>
#include <cerberus/analyzation/lex/lex.hpp>

using namespace cerb::literals;

// clang-format off

enum TestLexCBlocks : size_t
{
    GENERAL          = 64UL,
    KEYWORD          = 128UL,
    OPERATOR         = 256UL,
};

enum TestLexCItems : size_t
{
    INT              = static_cast<size_t>(GENERAL) + 0UL,
    FLOAT            = static_cast<size_t>(GENERAL) + 1UL,
    DOUBLE           = static_cast<size_t>(GENERAL) + 2UL,
    SIN              = static_cast<size_t>(KEYWORD) + 0UL,
    COS              = static_cast<size_t>(KEYWORD) + 1UL,
    ADD              = static_cast<size_t>(OPERATOR) + 0UL,
    SUB              = static_cast<size_t>(OPERATOR) + 1UL,
    MUL              = static_cast<size_t>(OPERATOR) + 2UL,
    DIV              = static_cast<size_t>(OPERATOR) + 3UL,
};

template<
    typename CharT = char,
    typename TokenType = TestLexCItems,
    bool MayThrow = true,
    size_t UID = 0,
    bool AllowStringLiterals = true,
    bool AllowComments = true,
    size_t MaxTerminals = 128,
    size_t MaxSize4Terminals = 4>
struct TestLexC: public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS

    constexpr TestLexC()
    : parent(
    {
        { SIN, "sin"_sv, true, 5 },
        { COS, "cos"_sv, true, 5 },
        { INT, "[0-9]+"_sv },
        { FLOAT, "[0-9]+[.][0-9]*[f]"_sv },
        { DOUBLE, "[0-9]+[.][0-9]*"_sv }
    },
    {
        { 
            { ADD, '+' },
            { SUB, '-' },
            { MUL, '*' },
            { DIV, '/' },
        },
       {
       }
    }
    )
    {}

};

// clang-format on

#endif /* CERBERUS_LEXER_C */
