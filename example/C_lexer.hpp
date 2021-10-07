#ifndef CERBERUS_LEXER_C
#define CERBERUS_LEXER_C

#include <iostream>
#include <cerberus/analyzation/lex/lex.hpp>

using namespace cerb::literals;

// clang-format off

enum TestLexCBlocks : size_t
{
    RESERVED         = 8UL,
    GENERAL          = 64UL,
    OPERATORS        = 128UL,
};

enum TestLexCItems : size_t
{
    UNDEFINED        = static_cast<size_t>(RESERVED) + 0UL,
    TRUE             = static_cast<size_t>(GENERAL) + 0UL,
    FALSE            = static_cast<size_t>(GENERAL) + 1UL,
    INT              = static_cast<size_t>(GENERAL) + 2UL,
    IDENTIFIER       = static_cast<size_t>(GENERAL) + 3UL,
    STRING           = static_cast<size_t>(GENERAL) + 4UL,
    CHAR             = static_cast<size_t>(GENERAL) + 5UL,
    ANGLE_OPENING    = static_cast<size_t>(OPERATORS) + 0UL,
    ASSIGN           = static_cast<size_t>(OPERATORS) + 1UL,
    ANGLE_CLOSING    = static_cast<size_t>(OPERATORS) + 2UL,
    COLON            = static_cast<size_t>(OPERATORS) + 3UL,
    WORD             = static_cast<size_t>(OPERATORS) + 4UL,
    EoF              = static_cast<size_t>(OPERATORS) + 5UL,
};

template<
    typename CharT = char,
    typename TokenType = TestLexCItems,
    bool MayThrow = true,
    size_t UID = 0,
    bool AllowStringLiterals = ,
    bool AllowComments = ,
    size_t MaxTerminals = 64,
    size_t MaxSize4Terminals = 4>
struct TestLexC: public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS

    constexpr TestLexC()
    : parent(
        '\"',
        '\'',
        STRING,
        CHAR,
        {
            { TRUE, "true"_sv, true, 5 },
            { FALSE, "false"_sv, true, 5 },
            { INT, "[0-9]+"_sv },
            { IDENTIFIER, "[a-zA-Z_]+[a-zA-Z0-9_]*"_sv },
            { ANGLE_OPENING, "["_sv }
        },
        {
            { 
                { ASSIGN, '=' },
                { ANGLE_CLOSING, ']' },
                { COLON, ':' },
                { WORD, '|' },
            },
           {
                { STRING, "\""_sv },
                { CHAR, "\'"_sv },
                { EoF, "%%"_sv },
           }
        }
    )
    {}
};
// clang-format on

#endif /* CERBERUS_LEXER_C */
