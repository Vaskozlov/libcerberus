#ifndef CERBERUS_LEXER_C
#define CERBERUS_LEXER_C

#include <iostream>
#include <cerberus/analyzation/lex/lex.hpp>

using namespace cerb::literals;

// clang-format off

enum Lex4LexParentBlocks : size_t
{
    RESERVED         = 8UL,
    GENERAL          = 64UL,
    OPERATORS        = 128UL,
};

enum Lex4LexParentItems : size_t
{
    UNDEFINED        = static_cast<size_t>(RESERVED) + 0UL,
    TRUE             = static_cast<size_t>(GENERAL) + 0UL,
    FALSE            = static_cast<size_t>(GENERAL) + 1UL,
    INT              = static_cast<size_t>(GENERAL) + 2UL,
    IDENTIFIER       = static_cast<size_t>(GENERAL) + 3UL,
    STRING           = static_cast<size_t>(GENERAL) + 4UL,
    CHAR             = static_cast<size_t>(GENERAL) + 5UL,
    ASSIGN           = static_cast<size_t>(OPERATORS) + 0UL,
    ANGLE_OPENING    = static_cast<size_t>(OPERATORS) + 1UL,
    ANGLE_CLOSING    = static_cast<size_t>(OPERATORS) + 2UL,
    COLON            = static_cast<size_t>(OPERATORS) + 3UL,
    WORD             = static_cast<size_t>(OPERATORS) + 4UL,
    EoF              = static_cast<size_t>(OPERATORS) + 5UL,
};

template<
    typename CharT = char,
    typename TokenType = Lex4LexParentItems,
    bool MayThrow = true,
    size_t UID = 0,
    bool AllowStringLiterals = true,
    bool AllowComments = true,
    size_t MaxTerminals = 64,
    size_t MaxSize4Terminals = 4>
struct Lex4LexParent: public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS

    constexpr Lex4LexParent()
    : parent(
        '\"',
        '\'',
        STRING,
        CHAR,
        {
            { TRUE, "true"_sv, true, 2 },
            { FALSE, "false"_sv, true, 2 },
            { INT, "[0-9]+"_sv, false, 6 },
            { IDENTIFIER, "[a-zA-Z_]+[a-zA-Z0-9_]*"_sv, false, 6 }
        },
        {
            { 
                { ASSIGN, '=' },
                { ANGLE_OPENING, '[' },
                { ANGLE_CLOSING, ']' },
                { COLON, ':' },
                { WORD, '|' },
            },
           {
                { EoF, "%%"_sv },
           }
        }
    )
    {}
};
// clang-format on

#endif /* CERBERUS_LEXER_C */
