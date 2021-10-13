#ifndef CERBERUS_LEXER_C
#define CERBERUS_LEXER_C

#include <iostream>
#include <cerberus/map.hpp>
#include <cerberus/analyzation/lex/lex.hpp>

using namespace cerb::literals;

// clang-format off

enum struct Lex4CBlock : size_t
{
    RESERVED         = 8UL,
    ARITHMETIC_OPERATOR = 4096UL,
    BIT_OPERATOR     = 8192UL,
    KEYWORD          = 16384UL,
    LOGICAL_OPERATOR = 32768UL,
    OPERATOR_SEPARATOR = 65536UL,
    TYPE             = 131072UL,
    VALUES           = 262144UL,
};

enum struct Lex4CItem : size_t
{
    UNDEFINED        = static_cast<size_t>(Lex4CBlock::RESERVED) + 0UL,
    EoF              = static_cast<size_t>(Lex4CBlock::RESERVED) + 1UL,
    SELF             = static_cast<size_t>(Lex4CBlock::RESERVED) + 2UL,
    EXPR             = static_cast<size_t>(Lex4CBlock::RESERVED) + 3UL,
    TERM             = static_cast<size_t>(Lex4CBlock::RESERVED) + 4UL,
    EMPTY            = static_cast<size_t>(Lex4CBlock::RESERVED) + 5UL,
    ADD              = static_cast<size_t>(Lex4CBlock::ARITHMETIC_OPERATOR) + 0UL,
    SUB              = static_cast<size_t>(Lex4CBlock::ARITHMETIC_OPERATOR) + 1UL,
    MUL              = static_cast<size_t>(Lex4CBlock::ARITHMETIC_OPERATOR) + 2UL,
    DIV              = static_cast<size_t>(Lex4CBlock::ARITHMETIC_OPERATOR) + 3UL,
    MOD              = static_cast<size_t>(Lex4CBlock::ARITHMETIC_OPERATOR) + 4UL,
    ASSIGN           = static_cast<size_t>(Lex4CBlock::ARITHMETIC_OPERATOR) + 5UL,
    ADD_EQ           = static_cast<size_t>(Lex4CBlock::ARITHMETIC_OPERATOR) + 6UL,
    SUB_EQ           = static_cast<size_t>(Lex4CBlock::ARITHMETIC_OPERATOR) + 7UL,
    MUL_EQ           = static_cast<size_t>(Lex4CBlock::ARITHMETIC_OPERATOR) + 8UL,
    DIV_EQ           = static_cast<size_t>(Lex4CBlock::ARITHMETIC_OPERATOR) + 9UL,
    MOD_EQ           = static_cast<size_t>(Lex4CBlock::ARITHMETIC_OPERATOR) + 10UL,
    OR               = static_cast<size_t>(Lex4CBlock::BIT_OPERATOR) + 0UL,
    AND              = static_cast<size_t>(Lex4CBlock::BIT_OPERATOR) + 1UL,
    XOR              = static_cast<size_t>(Lex4CBlock::BIT_OPERATOR) + 2UL,
    NOT              = static_cast<size_t>(Lex4CBlock::BIT_OPERATOR) + 3UL,
    OR_EQ            = static_cast<size_t>(Lex4CBlock::BIT_OPERATOR) + 4UL,
    AND_EQ           = static_cast<size_t>(Lex4CBlock::BIT_OPERATOR) + 5UL,
    XOR_EQ           = static_cast<size_t>(Lex4CBlock::BIT_OPERATOR) + 6UL,
    LEFT_SHIFT       = static_cast<size_t>(Lex4CBlock::BIT_OPERATOR) + 7UL,
    RIGHT_SHIFT      = static_cast<size_t>(Lex4CBlock::BIT_OPERATOR) + 8UL,
    LEFT_SHIFT_EQ    = static_cast<size_t>(Lex4CBlock::BIT_OPERATOR) + 9UL,
    RIGHT_SHIFT_EQ   = static_cast<size_t>(Lex4CBlock::BIT_OPERATOR) + 10UL,
    FOR              = static_cast<size_t>(Lex4CBlock::KEYWORD) + 0UL,
    WHILE            = static_cast<size_t>(Lex4CBlock::KEYWORD) + 1UL,
    RETURN           = static_cast<size_t>(Lex4CBlock::KEYWORD) + 2UL,
    CASE             = static_cast<size_t>(Lex4CBlock::KEYWORD) + 3UL,
    SWITCH           = static_cast<size_t>(Lex4CBlock::KEYWORD) + 4UL,
    LESS             = static_cast<size_t>(Lex4CBlock::LOGICAL_OPERATOR) + 0UL,
    GREATER          = static_cast<size_t>(Lex4CBlock::LOGICAL_OPERATOR) + 1UL,
    LOGICAL_NOT      = static_cast<size_t>(Lex4CBlock::LOGICAL_OPERATOR) + 2UL,
    LES_EQ           = static_cast<size_t>(Lex4CBlock::LOGICAL_OPERATOR) + 3UL,
    GREATER_EQ       = static_cast<size_t>(Lex4CBlock::LOGICAL_OPERATOR) + 4UL,
    LOGICAL_OR       = static_cast<size_t>(Lex4CBlock::LOGICAL_OPERATOR) + 5UL,
    LOGICAL_AND      = static_cast<size_t>(Lex4CBlock::LOGICAL_OPERATOR) + 6UL,
    LOGICAL_EQUAL    = static_cast<size_t>(Lex4CBlock::LOGICAL_OPERATOR) + 7UL,
    LOGICAL_NOT_EQUAL = static_cast<size_t>(Lex4CBlock::LOGICAL_OPERATOR) + 8UL,
    CURLY_OPENING    = static_cast<size_t>(Lex4CBlock::OPERATOR_SEPARATOR) + 0UL,
    CURLY_CLOSING    = static_cast<size_t>(Lex4CBlock::OPERATOR_SEPARATOR) + 1UL,
    LEFT_PARENTHESIS = static_cast<size_t>(Lex4CBlock::OPERATOR_SEPARATOR) + 2UL,
    RIGHT_PARENTHESIS = static_cast<size_t>(Lex4CBlock::OPERATOR_SEPARATOR) + 3UL,
    ANGLE_OPENING    = static_cast<size_t>(Lex4CBlock::OPERATOR_SEPARATOR) + 4UL,
    ANGLE_CLOSING    = static_cast<size_t>(Lex4CBlock::OPERATOR_SEPARATOR) + 5UL,
    COMMA            = static_cast<size_t>(Lex4CBlock::OPERATOR_SEPARATOR) + 6UL,
    SEPARATOR        = static_cast<size_t>(Lex4CBlock::OPERATOR_SEPARATOR) + 7UL,
    PREPROCESSOR     = static_cast<size_t>(Lex4CBlock::OPERATOR_SEPARATOR) + 8UL,
    QUESTION_MARK    = static_cast<size_t>(Lex4CBlock::OPERATOR_SEPARATOR) + 9UL,
    INT              = static_cast<size_t>(Lex4CBlock::TYPE) + 0UL,
    CHAR_T           = static_cast<size_t>(Lex4CBlock::TYPE) + 1UL,
    LONG             = static_cast<size_t>(Lex4CBlock::TYPE) + 2UL,
    SHORT            = static_cast<size_t>(Lex4CBlock::TYPE) + 3UL,
    FLOAT_T          = static_cast<size_t>(Lex4CBlock::TYPE) + 4UL,
    DOUBLE_T         = static_cast<size_t>(Lex4CBlock::TYPE) + 5UL,
    SIGNED           = static_cast<size_t>(Lex4CBlock::TYPE) + 6UL,
    UNSIGNED         = static_cast<size_t>(Lex4CBlock::TYPE) + 7UL,
    INTEGER          = static_cast<size_t>(Lex4CBlock::VALUES) + 0UL,
    FLOAT            = static_cast<size_t>(Lex4CBlock::VALUES) + 1UL,
    DOUBLE           = static_cast<size_t>(Lex4CBlock::VALUES) + 2UL,
    IDENTIFIER       = static_cast<size_t>(Lex4CBlock::VALUES) + 3UL,
    CHAR             = static_cast<size_t>(Lex4CBlock::VALUES) + 4UL,
    STRING           = static_cast<size_t>(Lex4CBlock::VALUES) + 5UL,
};

constexpr cerb::gl::Map<Lex4CBlock, cerb::string_view, 8> Lex4CBlockNames{
    true, {
        { Lex4CBlock::RESERVED, "RESERVED"_sv },
        { Lex4CBlock::ARITHMETIC_OPERATOR, "ARITHMETIC_OPERATOR"_sv },
        { Lex4CBlock::BIT_OPERATOR, "BIT_OPERATOR"_sv },
        { Lex4CBlock::KEYWORD, "KEYWORD"_sv },
        { Lex4CBlock::LOGICAL_OPERATOR, "LOGICAL_OPERATOR"_sv },
        { Lex4CBlock::OPERATOR_SEPARATOR, "OPERATOR_SEPARATOR"_sv },
        { Lex4CBlock::TYPE, "TYPE"_sv },
        { Lex4CBlock::VALUES, "VALUES"_sv },
    }
};

constexpr cerb::gl::Map<Lex4CItem, cerb::string_view, 66> Lex4CItemItemsNames{
    true, {
        { Lex4CItem::UNDEFINED, "UNDEFINED"_sv },
        { Lex4CItem::EoF, "EoF"_sv },
        { Lex4CItem::SELF, "SELF"_sv },
        { Lex4CItem::EXPR, "EXPR"_sv },
        { Lex4CItem::TERM, "TERM"_sv },
        { Lex4CItem::EMPTY, "EMPTY"_sv },
        { Lex4CItem::ADD, "ADD"_sv },
        { Lex4CItem::SUB, "SUB"_sv },
        { Lex4CItem::MUL, "MUL"_sv },
        { Lex4CItem::DIV, "DIV"_sv },
        { Lex4CItem::MOD, "MOD"_sv },
        { Lex4CItem::ASSIGN, "ASSIGN"_sv },
        { Lex4CItem::ADD_EQ, "ADD_EQ"_sv },
        { Lex4CItem::SUB_EQ, "SUB_EQ"_sv },
        { Lex4CItem::MUL_EQ, "MUL_EQ"_sv },
        { Lex4CItem::DIV_EQ, "DIV_EQ"_sv },
        { Lex4CItem::MOD_EQ, "MOD_EQ"_sv },
        { Lex4CItem::OR, "OR"_sv },
        { Lex4CItem::AND, "AND"_sv },
        { Lex4CItem::XOR, "XOR"_sv },
        { Lex4CItem::NOT, "NOT"_sv },
        { Lex4CItem::OR_EQ, "OR_EQ"_sv },
        { Lex4CItem::AND_EQ, "AND_EQ"_sv },
        { Lex4CItem::XOR_EQ, "XOR_EQ"_sv },
        { Lex4CItem::LEFT_SHIFT, "LEFT_SHIFT"_sv },
        { Lex4CItem::RIGHT_SHIFT, "RIGHT_SHIFT"_sv },
        { Lex4CItem::LEFT_SHIFT_EQ, "LEFT_SHIFT_EQ"_sv },
        { Lex4CItem::RIGHT_SHIFT_EQ, "RIGHT_SHIFT_EQ"_sv },
        { Lex4CItem::FOR, "FOR"_sv },
        { Lex4CItem::WHILE, "WHILE"_sv },
        { Lex4CItem::RETURN, "RETURN"_sv },
        { Lex4CItem::CASE, "CASE"_sv },
        { Lex4CItem::SWITCH, "SWITCH"_sv },
        { Lex4CItem::LESS, "LESS"_sv },
        { Lex4CItem::GREATER, "GREATER"_sv },
        { Lex4CItem::LOGICAL_NOT, "LOGICAL_NOT"_sv },
        { Lex4CItem::LES_EQ, "LES_EQ"_sv },
        { Lex4CItem::GREATER_EQ, "GREATER_EQ"_sv },
        { Lex4CItem::LOGICAL_OR, "LOGICAL_OR"_sv },
        { Lex4CItem::LOGICAL_AND, "LOGICAL_AND"_sv },
        { Lex4CItem::LOGICAL_EQUAL, "LOGICAL_EQUAL"_sv },
        { Lex4CItem::LOGICAL_NOT_EQUAL, "LOGICAL_NOT_EQUAL"_sv },
        { Lex4CItem::CURLY_OPENING, "CURLY_OPENING"_sv },
        { Lex4CItem::CURLY_CLOSING, "CURLY_CLOSING"_sv },
        { Lex4CItem::LEFT_PARENTHESIS, "LEFT_PARENTHESIS"_sv },
        { Lex4CItem::RIGHT_PARENTHESIS, "RIGHT_PARENTHESIS"_sv },
        { Lex4CItem::ANGLE_OPENING, "ANGLE_OPENING"_sv },
        { Lex4CItem::ANGLE_CLOSING, "ANGLE_CLOSING"_sv },
        { Lex4CItem::COMMA, "COMMA"_sv },
        { Lex4CItem::SEPARATOR, "SEPARATOR"_sv },
        { Lex4CItem::PREPROCESSOR, "PREPROCESSOR"_sv },
        { Lex4CItem::QUESTION_MARK, "QUESTION_MARK"_sv },
        { Lex4CItem::INT, "INT"_sv },
        { Lex4CItem::CHAR_T, "CHAR_T"_sv },
        { Lex4CItem::LONG, "LONG"_sv },
        { Lex4CItem::SHORT, "SHORT"_sv },
        { Lex4CItem::FLOAT_T, "FLOAT_T"_sv },
        { Lex4CItem::DOUBLE_T, "DOUBLE_T"_sv },
        { Lex4CItem::SIGNED, "SIGNED"_sv },
        { Lex4CItem::UNSIGNED, "UNSIGNED"_sv },
        { Lex4CItem::INTEGER, "INTEGER"_sv },
        { Lex4CItem::FLOAT, "FLOAT"_sv },
        { Lex4CItem::DOUBLE, "DOUBLE"_sv },
        { Lex4CItem::IDENTIFIER, "IDENTIFIER"_sv },
        { Lex4CItem::CHAR, "CHAR"_sv },
        { Lex4CItem::STRING, "STRING"_sv },
    }
};


#define Lex4CTemplate                             \
    template<                                   \
        typename CharT = char,                       \
        typename TokenType = Lex4CItem,              \
        bool MayThrow = true,                        \
        size_t UID = 0,                             \
        bool AllowStringLiterals = true,             \
        bool AllowComments = true,                   \
        size_t MaxTerminals = 128,                  \
        size_t MaxSize4Terminals = 4>

#define Lex4CDefinition(CLASS) CalculatorTemplate struct CLASS : public Lex4C<>

#define Lex4CAccess using Lex4C<>::parent;               \
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
        using enum Lex4CItem;

Lex4CTemplate
struct Lex4C: public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS
    using enum Lex4CItem;

    constexpr Lex4C()
    : parent(
        '\"',
        '\'',
        STRING,
        CHAR,
        {
            { FOR, "for"_sv, true, 2 },
            { WHILE, "while"_sv, true, 2 },
            { RETURN, "return"_sv, true, 2 },
            { CASE, "case"_sv, true, 2 },
            { SWITCH, "switch"_sv, true, 2 },
            { INT, "int"_sv, true, 2 },
            { CHAR_T, "char"_sv, true, 2 },
            { LONG, "long"_sv, true, 2 },
            { SHORT, "short"_sv, true, 2 },
            { FLOAT_T, "float"_sv, true, 2 },
            { DOUBLE_T, "double"_sv, true, 2 },
            { SIGNED, "signed"_sv, true, 2 },
            { UNSIGNED, "unsigned"_sv, true, 2 },
            { INTEGER, "[0-9]+"_sv, false, 18 },
            { FLOAT, "[0-9]+[.][0-9]*[f]"_sv, false, 18 },
            { DOUBLE, "[0-9]+[.][0-9]*"_sv, false, 18 },
            { IDENTIFIER, "[a-zA_Z_]+[a-zA-Z0-9_]*"_sv, false, 18 }
        },
        {
            { 
                { ADD, '+' },
                { SUB, '-' },
                { MUL, '*' },
                { DIV, '/' },
                { MOD, '%' },
                { ASSIGN, '=' },
                { OR, '|' },
                { AND, '&' },
                { XOR, '^' },
                { NOT, '~' },
                { LESS, '<' },
                { GREATER, '>' },
                { LOGICAL_NOT, '!' },
                { CURLY_OPENING, '{' },
                { CURLY_CLOSING, '}' },
                { LEFT_PARENTHESIS, '(' },
                { RIGHT_PARENTHESIS, ')' },
                { ANGLE_OPENING, '[' },
                { ANGLE_CLOSING, ']' },
                { COMMA, ',' },
                { SEPARATOR, ';' },
                { PREPROCESSOR, '#' },
                { QUESTION_MARK, '?' },
            },
           {
                { ADD_EQ, "+="_sv },
                { SUB_EQ, "-="_sv },
                { MUL_EQ, "*="_sv },
                { DIV_EQ, "/="_sv },
                { MOD_EQ, "%="_sv },
                { OR_EQ, "|="_sv },
                { AND_EQ, "&="_sv },
                { XOR_EQ, "^="_sv },
                { LEFT_SHIFT, "<<"_sv },
                { RIGHT_SHIFT, ">>"_sv },
                { LEFT_SHIFT_EQ, "<<="_sv },
                { RIGHT_SHIFT_EQ, ">>="_sv },
                { LES_EQ, "<="_sv },
                { GREATER_EQ, ">="_sv },
                { LOGICAL_OR, "||"_sv },
                { LOGICAL_AND, "&&"_sv },
                { LOGICAL_EQUAL, "=="_sv },
                { LOGICAL_NOT_EQUAL, "!="_sv },
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
