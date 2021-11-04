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
    GENERAL          = 4096UL,
    OPERATORS        = 8192UL,
};

enum struct Lex4LexItem : size_t
{
    UNDEFINED        = 16UL,
    EoF              = 17UL,
    SELF             = 18UL,
    EXPR             = 19UL,
    TERM             = 20UL,
    EMPTY            = 21UL,
    TRUE             = 4096UL,
    FALSE            = 4097UL,
    INT              = 4098UL,
    IDENTIFIER       = 4099UL,
    STRING           = 4100UL,
    CHAR             = 4101UL,
    ASSIGN           = 8192UL,
    ANGLE_OPENING    = 8193UL,
    ANGLE_CLOSING    = 8194UL,
    COLON            = 8195UL,
    WORD             = 8196UL,
    EoR              = 8197UL,
};

/*
%token UNDEFINED        16
%token EoF              17
%token SELF             18
%token EXPR             19
%token TERM             20
%token EMPTY            21
%token TRUE             4096
%token FALSE            4097
%token INT              4098
%token IDENTIFIER       4099
%token STRING           4100
%token CHAR             4101
%token ASSIGN           '='
%token ANGLE_OPENING    '['
%token ANGLE_CLOSING    ']'
%token COLON            ':'
%token WORD             '|'
%token EoR              "%%"


constexpr cerb::gl::Map<Lex4LexItem, yytokentype, 18> Lex4LexItemsNamesConverter{
    true, {
    {Lex4LexItem:UNDEFINED       , yytokentype::UNDEFINED},
    {Lex4LexItem:EoF             , yytokentype::EoF},
    {Lex4LexItem:SELF            , yytokentype::SELF},
    {Lex4LexItem:EXPR            , yytokentype::EXPR},
    {Lex4LexItem:TERM            , yytokentype::TERM},
    {Lex4LexItem:EMPTY           , yytokentype::EMPTY},
    {Lex4LexItem::TRUE            , yytokentype::TRUE},
    {Lex4LexItem::FALSE           , yytokentype::FALSE},
    {Lex4LexItem::INT             , yytokentype::INT},
    {Lex4LexItem::IDENTIFIER      , yytokentype::IDENTIFIER},
    {Lex4LexItem::STRING          , yytokentype::STRING},
    {Lex4LexItem::CHAR            , yytokentype::CHAR},
    {Lex4LexItem::ASSIGN          , yytokentype::ASSIGN},
    {Lex4LexItem::ANGLE_OPENING   , yytokentype::ANGLE_OPENING},
    {Lex4LexItem::ANGLE_CLOSING   , yytokentype::ANGLE_CLOSING},
    {Lex4LexItem::COLON           , yytokentype::COLON},
    {Lex4LexItem::WORD            , yytokentype::WORD},
    {Lex4LexItem::EoR             , yytokentype::EoR},
*/

constexpr cerb::gl::Map<Lex4LexBlock, cerb::string_view, 3> Lex4LexBlockNames{
    true, {
        { Lex4LexBlock::RESERVED, "RESERVED"_sv },
        { Lex4LexBlock::GENERAL, "GENERAL"_sv },
        { Lex4LexBlock::OPERATORS, "OPERATORS"_sv },
    }
};

constexpr cerb::gl::Map<Lex4LexItem, cerb::string_view, 18> Lex4LexItemItemsNames{
    true, {
        { Lex4LexItem::UNDEFINED, "UNDEFINED"_sv },
        { Lex4LexItem::EoF, "EoF"_sv },
        { Lex4LexItem::SELF, "SELF"_sv },
        { Lex4LexItem::EXPR, "EXPR"_sv },
        { Lex4LexItem::TERM, "TERM"_sv },
        { Lex4LexItem::EMPTY, "EMPTY"_sv },
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
};



namespace cerb {
    constexpr auto convert(Lex4LexItem value) -> cerb::string_view
    {
        if (Lex4LexItemItemsNames.contains(value)) {
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
            { TRUE, "true"_sv, true, 2 },
            { FALSE, "false"_sv, true, 2 },
            { INT, "[0-9]+"_sv, false, 12 },
            { IDENTIFIER, "[a-zA-Z_]+[a-zA-Z0-9_]*"_sv, false, 12 }
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
