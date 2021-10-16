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
    OPERATORS        = 4096UL,
    SEPARATOR        = 8192UL,
    VALUE            = 16384UL,
};

enum struct CalculatorItem : size_t
{
    UNDEFINED        = static_cast<size_t>(CalculatorBlock::RESERVED) + 0UL,
    EoF              = static_cast<size_t>(CalculatorBlock::RESERVED) + 1UL,
    SELF             = static_cast<size_t>(CalculatorBlock::RESERVED) + 2UL,
    EXPR             = static_cast<size_t>(CalculatorBlock::RESERVED) + 3UL,
    TERM             = static_cast<size_t>(CalculatorBlock::RESERVED) + 4UL,
    EMPTY            = static_cast<size_t>(CalculatorBlock::RESERVED) + 5UL,
    ADD              = static_cast<size_t>(CalculatorBlock::OPERATORS) + 0UL,
    LEFT_PARENTHESIS = static_cast<size_t>(CalculatorBlock::SEPARATOR) + 0UL,
    RIGHT_PARENTHESIS = static_cast<size_t>(CalculatorBlock::SEPARATOR) + 1UL,
    INT              = static_cast<size_t>(CalculatorBlock::VALUE) + 0UL,
};

constexpr cerb::gl::Map<CalculatorBlock, cerb::string_view, 4> CalculatorBlockNames{
    true, {
        { CalculatorBlock::RESERVED, "RESERVED"_sv },
        { CalculatorBlock::OPERATORS, "OPERATORS"_sv },
        { CalculatorBlock::SEPARATOR, "SEPARATOR"_sv },
        { CalculatorBlock::VALUE, "VALUE"_sv },
    }
};

constexpr cerb::gl::Map<CalculatorItem, cerb::string_view, 10> CalculatorItemItemsNames{
    true, {
        { CalculatorItem::UNDEFINED, "UNDEFINED"_sv },
        { CalculatorItem::EoF, "EoF"_sv },
        { CalculatorItem::SELF, "SELF"_sv },
        { CalculatorItem::EXPR, "EXPR"_sv },
        { CalculatorItem::TERM, "TERM"_sv },
        { CalculatorItem::EMPTY, "EMPTY"_sv },
        { CalculatorItem::ADD, "ADD"_sv },
        { CalculatorItem::LEFT_PARENTHESIS, "LEFT_PARENTHESIS"_sv },
        { CalculatorItem::RIGHT_PARENTHESIS, "RIGHT_PARENTHESIS"_sv },
        { CalculatorItem::INT, "INT"_sv },
    }
};


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
            { INT, "[0-9]+"_sv, false, 14 }
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
