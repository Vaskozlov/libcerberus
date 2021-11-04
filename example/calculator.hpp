#ifndef CERBERUS_CALCULATOR_HPP
#define CERBERUS_CALCULATOR_HPP

#include <iostream>
#include <cerberus/map.hpp>
#include <cerberus/analyzation/lex/lex.hpp>

using namespace cerb::literals;

// clang-format off

enum struct CalculatorBlock : size_t
{
    RESERVED         = 16UL,
    FUNCTION         = 4096UL,
    OPERATORS        = 8192UL,
    SEPARATOR        = 16384UL,
    VALUE            = 32768UL,
};

enum struct CalculatorItem : size_t
{
    UNDEFINED        = 16UL,
    EoF              = 17UL,
    SELF             = 18UL,
    EXPR             = 19UL,
    TERM             = 20UL,
    EMPTY            = 21UL,
    SIN              = 4096UL,
    ADD              = 8192UL,
    LEFT_PARENTHESIS = 16384UL,
    RIGHT_PARENTHESIS = 16385UL,
    INT              = 32768UL,
};

/*
%token UNDEFINED        16
%token EoF              17
%token SELF             18
%token EXPR             19
%token TERM             20
%token EMPTY            21
%token SIN              4096
%token ADD              '+'
%token LEFT_PARENTHESIS '('
%token RIGHT_PARENTHESIS ')'
%token INT              32768


constexpr cerb::gl::Map<CalculatorItem, yytokentype, 11> CalculatorItemsNamesConverter{
    true, {
    {CalculatorItem:UNDEFINED       , yytokentype::UNDEFINED},
    {CalculatorItem:EoF             , yytokentype::EoF},
    {CalculatorItem:SELF            , yytokentype::SELF},
    {CalculatorItem:EXPR            , yytokentype::EXPR},
    {CalculatorItem:TERM            , yytokentype::TERM},
    {CalculatorItem:EMPTY           , yytokentype::EMPTY},
    {CalculatorItem::SIN             , yytokentype::SIN},
    {CalculatorItem::ADD             , yytokentype::ADD},
    {CalculatorItem::LEFT_PARENTHESIS, yytokentype::LEFT_PARENTHESIS},
    {CalculatorItem::RIGHT_PARENTHESIS, yytokentype::RIGHT_PARENTHESIS},
    {CalculatorItem::INT             , yytokentype::INT},
*/

constexpr cerb::gl::Map<CalculatorBlock, cerb::string_view, 5> CalculatorBlockNames{
    true, {
        { CalculatorBlock::RESERVED, "RESERVED"_sv },
        { CalculatorBlock::FUNCTION, "FUNCTION"_sv },
        { CalculatorBlock::OPERATORS, "OPERATORS"_sv },
        { CalculatorBlock::SEPARATOR, "SEPARATOR"_sv },
        { CalculatorBlock::VALUE, "VALUE"_sv },
    }
};

constexpr cerb::gl::Map<CalculatorItem, cerb::string_view, 11> CalculatorItemItemsNames{
    true, {
        { CalculatorItem::UNDEFINED, "UNDEFINED"_sv },
        { CalculatorItem::EoF, "EoF"_sv },
        { CalculatorItem::SELF, "SELF"_sv },
        { CalculatorItem::EXPR, "EXPR"_sv },
        { CalculatorItem::TERM, "TERM"_sv },
        { CalculatorItem::EMPTY, "EMPTY"_sv },
        { CalculatorItem::SIN, "SIN"_sv },
        { CalculatorItem::ADD, "ADD"_sv },
        { CalculatorItem::LEFT_PARENTHESIS, "LEFT_PARENTHESIS"_sv },
        { CalculatorItem::RIGHT_PARENTHESIS, "RIGHT_PARENTHESIS"_sv },
        { CalculatorItem::INT, "INT"_sv },
    }
};



namespace cerb {
    constexpr auto convert(CalculatorItem value) -> cerb::string_view
    {
        if (CalculatorItemItemsNames.contains(value)) {
            return CalculatorItemItemsNames[value];
        }

        return "";
    }
}

#define CalculatorTemplate                             \
    template<                                   \
        typename CharT = char,                       \
        typename TokenType = CalculatorItem,              \
        bool MayThrow = true,                        \
        size_t UID = 0,                             \
        bool AllowStringLiterals = true,             \
        bool AllowComments = true,                   \
        size_t MaxTerminals = 16,                  \
        size_t MaxSize4Terminals = 4>

#define CalculatorDefinition(CLASS) CalculatorTemplate struct CLASS : public Calculator<>

#define CalculatorAccess using Calculator<>::parent;               \
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
        using enum CalculatorItem;

CalculatorTemplate
struct Calculator: public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS
    using enum CalculatorItem;

    constexpr Calculator()
    : parent(
        '\0',
        '\0',
        UNDEFINED,
        UNDEFINED,
        {
            { SIN, "sin"_sv, true, 2 },
            { INT, "[0-9]+"_sv, false, 15 }
        },
        {
            { 
                { ADD, '+' },
                { LEFT_PARENTHESIS, '(' },
                { RIGHT_PARENTHESIS, ')' },
            },
           {
           }
        },
        "//",
        "/*",
        "*/"
    )
    {}
};
// clang-format on

#endif /* CERBERUS_CALCULATOR_HPP */
