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

constexpr cerb::gl::Map<Lex4LexParentBlocks, cerb::string_view, 2> Lex4LexParentBlockNames{
    true, {
        { GENERAL, "GENERAL"_sv },
        { OPERATORS, "OPERATORS"_sv },
    }
};

constexpr cerb::gl::Map<Lex4LexParentItems, cerb::string_view, 12> Lex4LexParentItemsNames{
    true, {
        { TRUE, "TRUE"_sv },
        { FALSE, "FALSE"_sv },
        { INT, "INT"_sv },
        { IDENTIFIER, "IDENTIFIER"_sv },
        { STRING, "STRING"_sv },
        { CHAR, "CHAR"_sv },
        { ASSIGN, "ASSIGN"_sv },
        { ANGLE_OPENING, "ANGLE_OPENING"_sv },
        { ANGLE_CLOSING, "ANGLE_CLOSING"_sv },
        { COLON, "COLON"_sv },
        { WORD, "WORD"_sv },
        { EoF, "EoF"_sv },
    }
};


#define Lex4LexParentTemplate                             \
    template<                                   \
        typename CharT = char,                       \
        typename TokenType = Lex4LexParentItems,              \
        bool MayThrow = true,                        \
        size_t UID = 0,                             \
        bool AllowStringLiterals = true,             \
        bool AllowComments = true,                   \
        size_t MaxTerminals = 64,                  \
        size_t MaxSize4Terminals = 4>

Lex4LexParentTemplate
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
        },
        "//",
        "/*",
        "*/"
    )
    {}
};
// clang-format on

#endif /* CERBERUS_LEXER_C */