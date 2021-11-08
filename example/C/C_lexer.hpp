#ifndef CERBERUS_LEXER_C
#define CERBERUS_LEXER_C

#include <iostream>
#include "cerberus/map.hpp"
#include "cerberus/analyzation/lex/lex.hpp"

using namespace cerb::literals;

// clang-format off

enum struct Lex4CBlock : size_t
{
    RESERVED         = 16UL,
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
    UNDEFINED        = 16UL,
    EoF              = 0UL,
    ADD              = 4096UL,
    SUB              = 4097UL,
    MUL              = 4098UL,
    DIV              = 4099UL,
    MOD              = 4100UL,
    ASSIGN           = 4101UL,
    ADD_EQ           = 4102UL,
    SUB_EQ           = 4103UL,
    MUL_EQ           = 4104UL,
    DIV_EQ           = 4105UL,
    MOD_EQ           = 4106UL,
    OR               = 8192UL,
    AND              = 8193UL,
    XOR              = 8194UL,
    NOT              = 8195UL,
    OR_EQ            = 8196UL,
    AND_EQ           = 8197UL,
    XOR_EQ           = 8198UL,
    LEFT_SHIFT       = 8199UL,
    RIGHT_SHIFT      = 8200UL,
    LEFT_SHIFT_EQ    = 8201UL,
    RIGHT_SHIFT_EQ   = 8202UL,
    FOR              = 16384UL,
    WHILE            = 16385UL,
    RETURN           = 16386UL,
    CASE             = 16387UL,
    SWITCH           = 16388UL,
    LESS             = 32768UL,
    GREATER          = 32769UL,
    LOGICAL_NOT      = 32770UL,
    LESS_EQ          = 32771UL,
    GREATER_EQ       = 32772UL,
    LOGICAL_OR       = 32773UL,
    LOGICAL_AND      = 32774UL,
    LOGICAL_EQUAL    = 32775UL,
    LOGICAL_NOT_EQUAL = 32776UL,
    CURLY_OPENING    = 65536UL,
    CURLY_CLOSING    = 65537UL,
    LEFT_PARENTHESIS = 65538UL,
    RIGHT_PARENTHESIS = 65539UL,
    ANGLE_OPENING    = 65540UL,
    ANGLE_CLOSING    = 65541UL,
    COMMA            = 65542UL,
    SEPARATOR        = 65543UL,
    PREPROCESSOR     = 65544UL,
    QUESTION_MARK    = 65545UL,
    INT              = 131072UL,
    CHAR_T           = 131073UL,
    LONG             = 131074UL,
    SHORT            = 131075UL,
    FLOAT_T          = 131076UL,
    DOUBLE_T         = 131077UL,
    SIGNED           = 131078UL,
    UNSIGNED         = 131079UL,
    INTEGER          = 262144UL,
    FLOAT            = 262145UL,
    DOUBLE           = 262146UL,
    IDENTIFIER       = 262147UL,
    CHAR             = 262148UL,
    STRING           = 262149UL,
};

/*
%token UNDEFINED        16
%token EoF              0
%token ADD              "+"
%token SUB              "-"
%token MUL              "*"
%token DIV              "/"
%token MOD              "%"
%token ASSIGN           "="
%token ADD_EQ           "+="
%token SUB_EQ           "-="
%token MUL_EQ           "*="
%token DIV_EQ           "/="
%token MOD_EQ           "%="
%token OR               "|"
%token AND              "&"
%token XOR              "^"
%token NOT              "~"
%token OR_EQ            "|="
%token AND_EQ           "&="
%token XOR_EQ           "^="
%token LEFT_SHIFT       "<<"
%token RIGHT_SHIFT      ">>"
%token LEFT_SHIFT_EQ    "<<="
%token RIGHT_SHIFT_EQ   ">>="
%token FOR              "for"
%token WHILE            "while"
%token RETURN           "return"
%token CASE             "case"
%token SWITCH           "switch"
%token LESS             "<"
%token GREATER          ">"
%token LOGICAL_NOT      "!"
%token LESS_EQ          "<="
%token GREATER_EQ       ">="
%token LOGICAL_OR       "||"
%token LOGICAL_AND      "&&"
%token LOGICAL_EQUAL    "=="
%token LOGICAL_NOT_EQUAL "!="
%token CURLY_OPENING    "{"
%token CURLY_CLOSING    "}"
%token LEFT_PARENTHESIS "("
%token RIGHT_PARENTHESIS ")"
%token ANGLE_OPENING    "["
%token ANGLE_CLOSING    "]"
%token COMMA            ","
%token SEPARATOR        ";"
%token PREPROCESSOR     "#"
%token QUESTION_MARK    "?"
%token INT              "int"
%token CHAR_T           "char"
%token LONG             "long"
%token SHORT            "short"
%token FLOAT_T          "float"
%token DOUBLE_T         "double"
%token SIGNED           "signed"
%token UNSIGNED         "unsigned"
%token INTEGER          262144
%token FLOAT            262145
%token DOUBLE           262146
%token IDENTIFIER       262147
%token CHAR             262148
%token STRING           262149


cerb::Map<Lex4CItem, yytokentype> Lex4CItemsNamesConverter(
    {
        {Lex4CItem::UNDEFINED           , yytokentype::UNDEFINED},
        {Lex4CItem::EoF                 , yytokentype::EoF},
        {Lex4CItem::ADD                 , yytokentype::ADD},
        {Lex4CItem::SUB                 , yytokentype::SUB},
        {Lex4CItem::MUL                 , yytokentype::MUL},
        {Lex4CItem::DIV                 , yytokentype::DIV},
        {Lex4CItem::MOD                 , yytokentype::MOD},
        {Lex4CItem::ASSIGN              , yytokentype::ASSIGN},
        {Lex4CItem::ADD_EQ              , yytokentype::ADD_EQ},
        {Lex4CItem::SUB_EQ              , yytokentype::SUB_EQ},
        {Lex4CItem::MUL_EQ              , yytokentype::MUL_EQ},
        {Lex4CItem::DIV_EQ              , yytokentype::DIV_EQ},
        {Lex4CItem::MOD_EQ              , yytokentype::MOD_EQ},
        {Lex4CItem::OR                  , yytokentype::OR},
        {Lex4CItem::AND                 , yytokentype::AND},
        {Lex4CItem::XOR                 , yytokentype::XOR},
        {Lex4CItem::NOT                 , yytokentype::NOT},
        {Lex4CItem::OR_EQ               , yytokentype::OR_EQ},
        {Lex4CItem::AND_EQ              , yytokentype::AND_EQ},
        {Lex4CItem::XOR_EQ              , yytokentype::XOR_EQ},
        {Lex4CItem::LEFT_SHIFT          , yytokentype::LEFT_SHIFT},
        {Lex4CItem::RIGHT_SHIFT         , yytokentype::RIGHT_SHIFT},
        {Lex4CItem::LEFT_SHIFT_EQ       , yytokentype::LEFT_SHIFT_EQ},
        {Lex4CItem::RIGHT_SHIFT_EQ      , yytokentype::RIGHT_SHIFT_EQ},
        {Lex4CItem::FOR                 , yytokentype::FOR},
        {Lex4CItem::WHILE               , yytokentype::WHILE},
        {Lex4CItem::RETURN              , yytokentype::RETURN},
        {Lex4CItem::CASE                , yytokentype::CASE},
        {Lex4CItem::SWITCH              , yytokentype::SWITCH},
        {Lex4CItem::LESS                , yytokentype::LESS},
        {Lex4CItem::GREATER             , yytokentype::GREATER},
        {Lex4CItem::LOGICAL_NOT         , yytokentype::LOGICAL_NOT},
        {Lex4CItem::LESS_EQ             , yytokentype::LESS_EQ},
        {Lex4CItem::GREATER_EQ          , yytokentype::GREATER_EQ},
        {Lex4CItem::LOGICAL_OR          , yytokentype::LOGICAL_OR},
        {Lex4CItem::LOGICAL_AND         , yytokentype::LOGICAL_AND},
        {Lex4CItem::LOGICAL_EQUAL       , yytokentype::LOGICAL_EQUAL},
        {Lex4CItem::LOGICAL_NOT_EQUAL   , yytokentype::LOGICAL_NOT_EQUAL},
        {Lex4CItem::CURLY_OPENING       , yytokentype::CURLY_OPENING},
        {Lex4CItem::CURLY_CLOSING       , yytokentype::CURLY_CLOSING},
        {Lex4CItem::LEFT_PARENTHESIS    , yytokentype::LEFT_PARENTHESIS},
        {Lex4CItem::RIGHT_PARENTHESIS   , yytokentype::RIGHT_PARENTHESIS},
        {Lex4CItem::ANGLE_OPENING       , yytokentype::ANGLE_OPENING},
        {Lex4CItem::ANGLE_CLOSING       , yytokentype::ANGLE_CLOSING},
        {Lex4CItem::COMMA               , yytokentype::COMMA},
        {Lex4CItem::SEPARATOR           , yytokentype::SEPARATOR},
        {Lex4CItem::PREPROCESSOR        , yytokentype::PREPROCESSOR},
        {Lex4CItem::QUESTION_MARK       , yytokentype::QUESTION_MARK},
        {Lex4CItem::INT                 , yytokentype::INT},
        {Lex4CItem::CHAR_T              , yytokentype::CHAR_T},
        {Lex4CItem::LONG                , yytokentype::LONG},
        {Lex4CItem::SHORT               , yytokentype::SHORT},
        {Lex4CItem::FLOAT_T             , yytokentype::FLOAT_T},
        {Lex4CItem::DOUBLE_T            , yytokentype::DOUBLE_T},
        {Lex4CItem::SIGNED              , yytokentype::SIGNED},
        {Lex4CItem::UNSIGNED            , yytokentype::UNSIGNED},
        {Lex4CItem::INTEGER             , yytokentype::INTEGER},
        {Lex4CItem::FLOAT               , yytokentype::FLOAT},
        {Lex4CItem::DOUBLE              , yytokentype::DOUBLE},
        {Lex4CItem::IDENTIFIER          , yytokentype::IDENTIFIER},
        {Lex4CItem::CHAR                , yytokentype::CHAR},
        {Lex4CItem::STRING              , yytokentype::STRING},
    }
);
*/

inline cerb::Map<Lex4CBlock, cerb::string_view> Lex4CBlockNames(
    {
        { Lex4CBlock::RESERVED, "RESERVED"_sv },
        { Lex4CBlock::ARITHMETIC_OPERATOR, "ARITHMETIC_OPERATOR"_sv },
        { Lex4CBlock::BIT_OPERATOR, "BIT_OPERATOR"_sv },
        { Lex4CBlock::KEYWORD, "KEYWORD"_sv },
        { Lex4CBlock::LOGICAL_OPERATOR, "LOGICAL_OPERATOR"_sv },
        { Lex4CBlock::OPERATOR_SEPARATOR, "OPERATOR_SEPARATOR"_sv },
        { Lex4CBlock::TYPE, "TYPE"_sv },
        { Lex4CBlock::VALUES, "VALUES"_sv },
    }
);

inline cerb::Map<Lex4CItem, cerb::string_view> Lex4CItemItemsNames(
    {
        { Lex4CItem::UNDEFINED, "UNDEFINED"_sv },
        { Lex4CItem::EoF, "EoF"_sv },
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
        { Lex4CItem::LESS_EQ, "LESS_EQ"_sv },
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
);

namespace cerb::lex {
    constexpr auto convert(Lex4CBlock value) -> cerb::string_view
    {
        if (Lex4CBlockNames.count(value) != 0) {
            return Lex4CBlockNames[value];
        }
        return "UNDEFINED"_sv;
    }

    constexpr auto convert(Lex4CItem value) -> cerb::string_view
    {
        if (Lex4CItemItemsNames.count(value) != 0) {
            return Lex4CItemItemsNames[value];
        }
        return "UNDEFINED"_sv;
    }
}

template<typename T>
auto operator<<(T &os, Lex4CBlock value) -> T &
{
    os << cerb::lex::convert(value);
    return os;
}

template<typename T>
auto operator<<(T &os, Lex4CItem value) -> T &
{
    os << cerb::lex::convert(value);
    return os;
}



namespace cerb {
    constexpr auto convert(Lex4CItem value) -> cerb::string_view
    {
        if (Lex4CItemItemsNames.count(value) != 0) {
            return Lex4CItemItemsNames[value];
        }

        return "";
    }
}

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
                { LESS_EQ, "<="_sv },
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
