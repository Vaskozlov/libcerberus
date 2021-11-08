#ifndef LEX4LEX_HPP
#define LEX4LEX_HPP

#include <iostream>
#include <cerberus/map.hpp>
#include <cerberus/analyzation/lex/lex.hpp>

using namespace cerb::literals;

// clang-format off

enum struct Lex4LexBlock : size_t
{
    RESERVED         = 16UL,
    DIRECTIVE        = 4096UL,
    GENERAL          = 8192UL,
    OPERATORS        = 16384UL,
};

enum struct Lex4LexItem : size_t
{
    UNDEFINED        = 16UL,
    EoF              = 0UL,
    GENERALIZED      = 4096UL,
    TRUE             = 8192UL,
    FALSE            = 8193UL,
    INT              = 8194UL,
    IDENTIFIER       = 8195UL,
    STRING           = 8196UL,
    CHAR             = 8197UL,
    ASSIGN           = 16384UL,
    ANGLE_OPENING    = 16385UL,
    ANGLE_CLOSING    = 16386UL,
    COLON            = 16387UL,
    WORD             = 16388UL,
    EoR              = 16389UL,
};

/*
%token UNDEFINED        16
%token EoF              0
%token GENERALIZED      "#GENERALIZE"
%token TRUE             "true"
%token FALSE            "false"
%token INT              8194
%token IDENTIFIER       8195
%token STRING           8196
%token CHAR             8197
%token ASSIGN           "="
%token ANGLE_OPENING    "["
%token ANGLE_CLOSING    "]"
%token COLON            ":"
%token WORD             "|"
%token EoR              "%%"


cerb::Map<Lex4LexItem, yytokentype> Lex4LexItemsNamesConverter(
    {
        {Lex4LexItem::UNDEFINED           , yytokentype::UNDEFINED},
        {Lex4LexItem::EoF                 , yytokentype::EoF},
        {Lex4LexItem::GENERALIZED         , yytokentype::GENERALIZED},
        {Lex4LexItem::TRUE                , yytokentype::TRUE},
        {Lex4LexItem::FALSE               , yytokentype::FALSE},
        {Lex4LexItem::INT                 , yytokentype::INT},
        {Lex4LexItem::IDENTIFIER          , yytokentype::IDENTIFIER},
        {Lex4LexItem::STRING              , yytokentype::STRING},
        {Lex4LexItem::CHAR                , yytokentype::CHAR},
        {Lex4LexItem::ASSIGN              , yytokentype::ASSIGN},
        {Lex4LexItem::ANGLE_OPENING       , yytokentype::ANGLE_OPENING},
        {Lex4LexItem::ANGLE_CLOSING       , yytokentype::ANGLE_CLOSING},
        {Lex4LexItem::COLON               , yytokentype::COLON},
        {Lex4LexItem::WORD                , yytokentype::WORD},
        {Lex4LexItem::EoR                 , yytokentype::EoR},
    }
);
*/

inline cerb::Map<Lex4LexBlock, cerb::string_view> Lex4LexBlockNames(
    {
        { Lex4LexBlock::RESERVED, "RESERVED"_sv },
        { Lex4LexBlock::DIRECTIVE, "DIRECTIVE"_sv },
        { Lex4LexBlock::GENERAL, "GENERAL"_sv },
        { Lex4LexBlock::OPERATORS, "OPERATORS"_sv },
    }
);

inline cerb::Map<Lex4LexItem, cerb::string_view> Lex4LexItemItemsNames(
    {
        { Lex4LexItem::UNDEFINED, "UNDEFINED"_sv },
        { Lex4LexItem::EoF, "EoF"_sv },
        { Lex4LexItem::GENERALIZED, "GENERALIZED"_sv },
        { Lex4LexItem::TRUE, "TRUE"_sv },
        { Lex4LexItem::FALSE, "FALSE"_sv },
        { Lex4LexItem::INT, "INT"_sv },
        { Lex4LexItem::IDENTIFIER, "IDENTIFIER"_sv },
        { Lex4LexItem::STRING, "STRING"_sv },
        { Lex4LexItem::CHAR, "CHAR"_sv },
        { Lex4LexItem::ASSIGN, "ASSIGN"_sv },
        { Lex4LexItem::ANGLE_OPENING, "ANGLE_OPENING"_sv },
        { Lex4LexItem::ANGLE_CLOSING, "ANGLE_CLOSING"_sv },
        { Lex4LexItem::COLON, "COLON"_sv },
        { Lex4LexItem::WORD, "WORD"_sv },
        { Lex4LexItem::EoR, "EoR"_sv },
    }
);

namespace cerb::lex {
    constexpr auto convert(Lex4LexBlock value) -> cerb::string_view
    {
        if (Lex4LexBlockNames.count(value) != 0) {
            return Lex4LexBlockNames[value];
        }
        return "UNDEFINED"_sv;
    }

    constexpr auto convert(Lex4LexItem value) -> cerb::string_view
    {
        if (Lex4LexItemItemsNames.count(value) != 0) {
            return Lex4LexItemItemsNames[value];
        }
        return "UNDEFINED"_sv;
    }
}

template<typename T>
auto operator<<(T &os, Lex4LexBlock value) -> T &
{
    os << cerb::lex::convert(value);
    return os;
}

template<typename T>
auto operator<<(T &os, Lex4LexItem value) -> T &
{
    os << cerb::lex::convert(value);
    return os;
}



namespace cerb {
    constexpr auto convert(Lex4LexItem value) -> cerb::string_view
    {
        if (Lex4LexItemItemsNames.count(value) != 0) {
            return Lex4LexItemItemsNames[value];
        }

        return "";
    }
}

#define Lex4LexTemplate                             \
    template<                                   \
        typename CharT = char,                       \
        typename TokenType = Lex4LexItem,              \
        bool MayThrow = true,                        \
        size_t UID = 0,                             \
        bool AllowStringLiterals = true,             \
        bool AllowComments = true,                   \
        size_t MaxTerminals = 64,                  \
        size_t MaxSize4Terminals = 4>

#define Lex4LexDefinition(CLASS) CalculatorTemplate struct CLASS : public Lex4Lex<>

#define Lex4LexAccess using Lex4Lex<>::parent;               \
        using parent::head;                                                         \
        using item_t           = typename parent::item_t;                           \
        using storage_t        = typename parent::storage_t;                        \
        using token_t          = typename parent::token_t;                          \
        using result_t         = typename parent::result_t;                         \
        using position_t       = typename parent::position_t;                       \
        using string_t         = typename parent::string_t;                         \
        using string_view_t    = typename parent::string_view_t;                    \
        using string_checker_t = typename parent::string_checker_t;                 \
        using item_initializer  = typename parent::item_initializer;                \
        using enum Lex4LexItem;

Lex4LexTemplate
struct Lex4Lex: public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS
    using enum Lex4LexItem;

    constexpr Lex4Lex()
    : parent(
        '\"',
        '\'',
        STRING,
        CHAR,
        {
            { GENERALIZED, "#GENERALIZE"_sv, true, 2 },
            { TRUE, "true"_sv, true, 2 },
            { FALSE, "false"_sv, true, 2 },
            { INT, "[0-9]+"_sv, false, 13 },
            { IDENTIFIER, "[a-zA-Z_]+[a-zA-Z0-9_]*"_sv, false, 13 }
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
