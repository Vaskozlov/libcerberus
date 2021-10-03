#ifndef CERBERUS_LEXER_C
#define CERBERUS_LEXER_C

#include <iostream>
#include <cerberus/analyzation/lex/lex.hpp>

// clang-format off


CERBERUS_LEX_TEMPLATES
struct TestLexC: public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS

    enum Blocks : u32
    {
        general          = 16,
        keywords         = 32,
        operators        = 64,
    };

    enum Items : u32
    {
        INT              = 16,
        SIN              = 32,
        COS              = 33,
        ADD              = 64,
        SUB              = 65,
        MUL              = 66,
        DIV              = 67,
    };

    constexpr TestLexC()
    : parent(
    {
        { SIN, "sin" },
        { COS, "cos" }
    },
    {
        { INT, "[0-9]+" }
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
