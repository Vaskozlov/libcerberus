#ifndef CERBERUS_CALCULATOR_HPP
#define CERBERUS_CALCULATOR_HPP

#include <iostream>
#include "cerberus/map.hpp"
#include "cerberus/analyzation/lex/lex.hpp"

using namespace cerb::literals;

// clang-format off

enum struct CalculatorBlock : size_t
{
    RESERVED         = 16UL,
    FUNCTION         = 64UL,
    OPERATORS        = 128UL,
    SEPARATOR        = 256UL,
    VALUE            = 512UL,
};

enum struct CalculatorItem : size_t
{
    UNDEFINED        = 16UL,
    EoF              = 0UL,
    SIN              = 64UL,
    ADD              = 128UL,
    SUB              = 129UL,
    MUL              = 130UL,
    LEFT_PARENTHESIS = 256UL,
    RIGHT_PARENTHESIS = 257UL,
    INT              = 512UL,
};

/*
%token UNDEFINED        16
%token EoF              0
%token SIN              "sin"
%token ADD              "+"
%token SUB              "-"
%token MUL              "*"
%token LEFT_PARENTHESIS "("
%token RIGHT_PARENTHESIS ")"
%token INT              512


cerb::Map<CalculatorItem, yytokentype> CalculatorItemsNamesConverter(
    {
        {CalculatorItem::UNDEFINED           , yytokentype::UNDEFINED},
        {CalculatorItem::EoF                 , yytokentype::EoF},
        {CalculatorItem::SIN                 , yytokentype::SIN},
        {CalculatorItem::ADD                 , yytokentype::ADD},
        {CalculatorItem::SUB                 , yytokentype::SUB},
        {CalculatorItem::MUL                 , yytokentype::MUL},
        {CalculatorItem::LEFT_PARENTHESIS    , yytokentype::LEFT_PARENTHESIS},
        {CalculatorItem::RIGHT_PARENTHESIS   , yytokentype::RIGHT_PARENTHESIS},
        {CalculatorItem::INT                 , yytokentype::INT},
    }
);
*/

inline cerb::Map<CalculatorBlock, cerb::string_view> CalculatorBlockNames(
    {
        { CalculatorBlock::RESERVED, "RESERVED"_sv },
        { CalculatorBlock::FUNCTION, "FUNCTION"_sv },
        { CalculatorBlock::OPERATORS, "OPERATORS"_sv },
        { CalculatorBlock::SEPARATOR, "SEPARATOR"_sv },
        { CalculatorBlock::VALUE, "VALUE"_sv },
    }
);

inline cerb::Map<CalculatorItem, cerb::string_view> CalculatorItemItemsNames(
    {
        { CalculatorItem::UNDEFINED, "UNDEFINED"_sv },
        { CalculatorItem::EoF, "EoF"_sv },
        { CalculatorItem::SIN, "SIN"_sv },
        { CalculatorItem::ADD, "ADD"_sv },
        { CalculatorItem::SUB, "SUB"_sv },
        { CalculatorItem::MUL, "MUL"_sv },
        { CalculatorItem::LEFT_PARENTHESIS, "LEFT_PARENTHESIS"_sv },
        { CalculatorItem::RIGHT_PARENTHESIS, "RIGHT_PARENTHESIS"_sv },
        { CalculatorItem::INT, "INT"_sv },
    }
);

namespace cerb::lex {
    constexpr auto convert(CalculatorBlock value) -> cerb::string_view
    {
        if (CalculatorBlockNames.count(value) != 0) {
            return CalculatorBlockNames[value];
        }
        return "UNDEFINED"_sv;
    }

    constexpr auto convert(CalculatorItem value) -> cerb::string_view
    {
        if (CalculatorItemItemsNames.count(value) != 0) {
            return CalculatorItemItemsNames[value];
        }
        return "UNDEFINED"_sv;
    }
}

template<typename T>
auto operator<<(T &os, CalculatorBlock value) -> T &
{
    os << cerb::lex::convert(value);
    return os;
}

template<typename T>
auto operator<<(T &os, CalculatorItem value) -> T &
{
    os << cerb::lex::convert(value);
    return os;
}



namespace cerb {
    constexpr auto convert(CalculatorItem value) -> cerb::string_view
    {
        if (CalculatorItemItemsNames.count(value) != 0) {
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
            { INT, "[0-9]+"_sv, false, 9 }
        },
        {
            { 
                { ADD, '+' },
                { SUB, '-' },
                { MUL, '*' },
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
