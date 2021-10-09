#ifndef LEX4LEX_HPP
#define LEX4LEX_HPP

#include <iostream>
#include <cerberus/map.hpp>
#include <cerberus/analyzation/lex/lex.hpp>

using namespace cerb::literals;

// clang-format off

enum Lex4LexBlocks : size_t
{
    RESERVED         = 8UL,
    GENERAL          = 64UL,
    OPERATORS        = 128UL,
};

enum Lex4LexItems : size_t
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
    EoR              = static_cast<size_t>(OPERATORS) + 5UL,
};

constexpr cerb::gl::Map<Lex4LexBlocks, cerb::string_view, 2> Lex4LexBlockNames{
    true, {
        { GENERAL, "GENERAL"_sv },
        { OPERATORS, "OPERATORS"_sv },
    }
};

constexpr cerb::gl::Map<Lex4LexItems, cerb::string_view, 12> Lex4LexItemsNames{
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
        { EoR, "EoR"_sv },
    }
};


#define Lex4LexTemplate                             \
    template<                                   \
        typename CharT = char,                       \
        typename TokenType = Lex4LexItems,              \
        bool MayThrow = true,                        \
        size_t UID = 0,                             \
        bool AllowStringLiterals = true,             \
        bool AllowComments = true,                   \
        size_t MaxTerminals = 64,                  \
        size_t MaxSize4Terminals = 4>

Lex4LexTemplate
struct Lex4Lex: public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS

    constexpr Lex4Lex()
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
                { EoR, "%%"_sv },
           }
        },
        "//",
        "/*",
        "*/"
    )
    {}
};
// clang-format on

#endif /* LEX4LEX_HPP */
