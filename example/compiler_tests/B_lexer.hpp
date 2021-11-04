#ifndef CERBERUS_B_LEXER
#define CERBERUS_B_LEXER

#include <iostream>
#include <cerberus/map.hpp>
#include <cerberus/analyzation/lex/lex.hpp>

using namespace cerb::literals;

// clang-format off

enum struct Lex4BBlock : size_t
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

enum struct Lex4BItem : size_t
{
    UNDEFINED        = 16UL,
    EoF              = 17UL,
    SELF             = 18UL,
    EXPR             = 19UL,
    TERM             = 20UL,
    EMPTY            = 21UL,
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
    INCREMENT        = 4107UL,
    DECREMENT        = 4108UL,
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
    IF               = 16387UL,
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
    COLON            = 65546UL,
    VAR              = 131072UL,
    INTEGER          = 262144UL,
    IDENTIFIER       = 262145UL,
    CHAR             = 262146UL,
    STRING           = 262147UL,
};

/*
%token UNDEFINED        16
%token EoF              17
%token SELF             18
%token EXPR             19
%token TERM             20
%token EMPTY            21
%token ADD              '+'
%token SUB              '-'
%token MUL              '*'
%token DIV              '/'
%token MOD              '%'
%token ASSIGN           '='
%token ADD_EQ           "+="
%token SUB_EQ           "-="
%token MUL_EQ           "*="
%token DIV_EQ           "/="
%token MOD_EQ           "%="
%token INCREMENT        "++"
%token DECREMENT        "--"
%token OR               '|'
%token AND              '&'
%token XOR              '^'
%token NOT              '~'
%token OR_EQ            "|="
%token AND_EQ           "&="
%token XOR_EQ           "^="
%token LEFT_SHIFT       "<<"
%token RIGHT_SHIFT      ">>"
%token LEFT_SHIFT_EQ    "<<="
%token RIGHT_SHIFT_EQ   ">>="
%token FOR              16384
%token WHILE            16385
%token RETURN           16386
%token IF               16387
%token LESS             '<'
%token GREATER          '>'
%token LOGICAL_NOT      '!'
%token LESS_EQ          "<="
%token GREATER_EQ       ">="
%token LOGICAL_OR       "||"
%token LOGICAL_AND      "&&"
%token LOGICAL_EQUAL    "=="
%token LOGICAL_NOT_EQUAL "!="
%token CURLY_OPENING    '{'
%token CURLY_CLOSING    '}'
%token LEFT_PARENTHESIS '('
%token RIGHT_PARENTHESIS ')'
%token ANGLE_OPENING    '['
%token ANGLE_CLOSING    ']'
%token COMMA            ','
%token SEPARATOR        ';'
%token PREPROCESSOR     '#'
%token QUESTION_MARK    '?'
%token COLON            ':'
%token VAR              131072
%token INTEGER          262144
%token IDENTIFIER       262145
%token CHAR             262146
%token STRING           262147


constexpr cerb::gl::Map<Lex4BItem, yytokentype, 59> Lex4BItemsNamesConverter{
    true, {
    {Lex4BItem:UNDEFINED       , yytokentype::UNDEFINED},
    {Lex4BItem:EoF             , yytokentype::EoF},
    {Lex4BItem:SELF            , yytokentype::SELF},
    {Lex4BItem:EXPR            , yytokentype::EXPR},
    {Lex4BItem:TERM            , yytokentype::TERM},
    {Lex4BItem:EMPTY           , yytokentype::EMPTY},
    {Lex4BItem::ADD             , yytokentype::ADD},
    {Lex4BItem::SUB             , yytokentype::SUB},
    {Lex4BItem::MUL             , yytokentype::MUL},
    {Lex4BItem::DIV             , yytokentype::DIV},
    {Lex4BItem::MOD             , yytokentype::MOD},
    {Lex4BItem::ASSIGN          , yytokentype::ASSIGN},
    {Lex4BItem::ADD_EQ          , yytokentype::ADD_EQ},
    {Lex4BItem::SUB_EQ          , yytokentype::SUB_EQ},
    {Lex4BItem::MUL_EQ          , yytokentype::MUL_EQ},
    {Lex4BItem::DIV_EQ          , yytokentype::DIV_EQ},
    {Lex4BItem::MOD_EQ          , yytokentype::MOD_EQ},
    {Lex4BItem::INCREMENT       , yytokentype::INCREMENT},
    {Lex4BItem::DECREMENT       , yytokentype::DECREMENT},
    {Lex4BItem::OR              , yytokentype::OR},
    {Lex4BItem::AND             , yytokentype::AND},
    {Lex4BItem::XOR             , yytokentype::XOR},
    {Lex4BItem::NOT             , yytokentype::NOT},
    {Lex4BItem::OR_EQ           , yytokentype::OR_EQ},
    {Lex4BItem::AND_EQ          , yytokentype::AND_EQ},
    {Lex4BItem::XOR_EQ          , yytokentype::XOR_EQ},
    {Lex4BItem::LEFT_SHIFT      , yytokentype::LEFT_SHIFT},
    {Lex4BItem::RIGHT_SHIFT     , yytokentype::RIGHT_SHIFT},
    {Lex4BItem::LEFT_SHIFT_EQ   , yytokentype::LEFT_SHIFT_EQ},
    {Lex4BItem::RIGHT_SHIFT_EQ  , yytokentype::RIGHT_SHIFT_EQ},
    {Lex4BItem::FOR             , yytokentype::FOR},
    {Lex4BItem::WHILE           , yytokentype::WHILE},
    {Lex4BItem::RETURN          , yytokentype::RETURN},
    {Lex4BItem::IF              , yytokentype::IF},
    {Lex4BItem::LESS            , yytokentype::LESS},
    {Lex4BItem::GREATER         , yytokentype::GREATER},
    {Lex4BItem::LOGICAL_NOT     , yytokentype::LOGICAL_NOT},
    {Lex4BItem::LESS_EQ         , yytokentype::LESS_EQ},
    {Lex4BItem::GREATER_EQ      , yytokentype::GREATER_EQ},
    {Lex4BItem::LOGICAL_OR      , yytokentype::LOGICAL_OR},
    {Lex4BItem::LOGICAL_AND     , yytokentype::LOGICAL_AND},
    {Lex4BItem::LOGICAL_EQUAL   , yytokentype::LOGICAL_EQUAL},
    {Lex4BItem::LOGICAL_NOT_EQUAL, yytokentype::LOGICAL_NOT_EQUAL},
    {Lex4BItem::CURLY_OPENING   , yytokentype::CURLY_OPENING},
    {Lex4BItem::CURLY_CLOSING   , yytokentype::CURLY_CLOSING},
    {Lex4BItem::LEFT_PARENTHESIS, yytokentype::LEFT_PARENTHESIS},
    {Lex4BItem::RIGHT_PARENTHESIS, yytokentype::RIGHT_PARENTHESIS},
    {Lex4BItem::ANGLE_OPENING   , yytokentype::ANGLE_OPENING},
    {Lex4BItem::ANGLE_CLOSING   , yytokentype::ANGLE_CLOSING},
    {Lex4BItem::COMMA           , yytokentype::COMMA},
    {Lex4BItem::SEPARATOR       , yytokentype::SEPARATOR},
    {Lex4BItem::PREPROCESSOR    , yytokentype::PREPROCESSOR},
    {Lex4BItem::QUESTION_MARK   , yytokentype::QUESTION_MARK},
    {Lex4BItem::COLON           , yytokentype::COLON},
    {Lex4BItem::VAR             , yytokentype::VAR},
    {Lex4BItem::INTEGER         , yytokentype::INTEGER},
    {Lex4BItem::IDENTIFIER      , yytokentype::IDENTIFIER},
    {Lex4BItem::CHAR            , yytokentype::CHAR},
    {Lex4BItem::STRING          , yytokentype::STRING},
*/

constexpr cerb::gl::Map<Lex4BBlock, cerb::string_view, 8> Lex4BBlockNames{
    true, {
        { Lex4BBlock::RESERVED, "RESERVED"_sv },
        { Lex4BBlock::ARITHMETIC_OPERATOR, "ARITHMETIC_OPERATOR"_sv },
        { Lex4BBlock::BIT_OPERATOR, "BIT_OPERATOR"_sv },
        { Lex4BBlock::KEYWORD, "KEYWORD"_sv },
        { Lex4BBlock::LOGICAL_OPERATOR, "LOGICAL_OPERATOR"_sv },
        { Lex4BBlock::OPERATOR_SEPARATOR, "OPERATOR_SEPARATOR"_sv },
        { Lex4BBlock::TYPE, "TYPE"_sv },
        { Lex4BBlock::VALUES, "VALUES"_sv },
    }
};

constexpr cerb::gl::Map<Lex4BItem, cerb::string_view, 59> Lex4BItemItemsNames{
    true, {
        { Lex4BItem::UNDEFINED, "UNDEFINED"_sv },
        { Lex4BItem::EoF, "EoF"_sv },
        { Lex4BItem::SELF, "SELF"_sv },
        { Lex4BItem::EXPR, "EXPR"_sv },
        { Lex4BItem::TERM, "TERM"_sv },
        { Lex4BItem::EMPTY, "EMPTY"_sv },
        { Lex4BItem::ADD, "ADD"_sv },
        { Lex4BItem::SUB, "SUB"_sv },
        { Lex4BItem::MUL, "MUL"_sv },
        { Lex4BItem::DIV, "DIV"_sv },
        { Lex4BItem::MOD, "MOD"_sv },
        { Lex4BItem::ASSIGN, "ASSIGN"_sv },
        { Lex4BItem::ADD_EQ, "ADD_EQ"_sv },
        { Lex4BItem::SUB_EQ, "SUB_EQ"_sv },
        { Lex4BItem::MUL_EQ, "MUL_EQ"_sv },
        { Lex4BItem::DIV_EQ, "DIV_EQ"_sv },
        { Lex4BItem::MOD_EQ, "MOD_EQ"_sv },
        { Lex4BItem::INCREMENT, "INCREMENT"_sv },
        { Lex4BItem::DECREMENT, "DECREMENT"_sv },
        { Lex4BItem::OR, "OR"_sv },
        { Lex4BItem::AND, "AND"_sv },
        { Lex4BItem::XOR, "XOR"_sv },
        { Lex4BItem::NOT, "NOT"_sv },
        { Lex4BItem::OR_EQ, "OR_EQ"_sv },
        { Lex4BItem::AND_EQ, "AND_EQ"_sv },
        { Lex4BItem::XOR_EQ, "XOR_EQ"_sv },
        { Lex4BItem::LEFT_SHIFT, "LEFT_SHIFT"_sv },
        { Lex4BItem::RIGHT_SHIFT, "RIGHT_SHIFT"_sv },
        { Lex4BItem::LEFT_SHIFT_EQ, "LEFT_SHIFT_EQ"_sv },
        { Lex4BItem::RIGHT_SHIFT_EQ, "RIGHT_SHIFT_EQ"_sv },
        { Lex4BItem::FOR, "FOR"_sv },
        { Lex4BItem::WHILE, "WHILE"_sv },
        { Lex4BItem::RETURN, "RETURN"_sv },
        { Lex4BItem::IF, "IF"_sv },
        { Lex4BItem::LESS, "LESS"_sv },
        { Lex4BItem::GREATER, "GREATER"_sv },
        { Lex4BItem::LOGICAL_NOT, "LOGICAL_NOT"_sv },
        { Lex4BItem::LESS_EQ, "LESS_EQ"_sv },
        { Lex4BItem::GREATER_EQ, "GREATER_EQ"_sv },
        { Lex4BItem::LOGICAL_OR, "LOGICAL_OR"_sv },
        { Lex4BItem::LOGICAL_AND, "LOGICAL_AND"_sv },
        { Lex4BItem::LOGICAL_EQUAL, "LOGICAL_EQUAL"_sv },
        { Lex4BItem::LOGICAL_NOT_EQUAL, "LOGICAL_NOT_EQUAL"_sv },
        { Lex4BItem::CURLY_OPENING, "CURLY_OPENING"_sv },
        { Lex4BItem::CURLY_CLOSING, "CURLY_CLOSING"_sv },
        { Lex4BItem::LEFT_PARENTHESIS, "LEFT_PARENTHESIS"_sv },
        { Lex4BItem::RIGHT_PARENTHESIS, "RIGHT_PARENTHESIS"_sv },
        { Lex4BItem::ANGLE_OPENING, "ANGLE_OPENING"_sv },
        { Lex4BItem::ANGLE_CLOSING, "ANGLE_CLOSING"_sv },
        { Lex4BItem::COMMA, "COMMA"_sv },
        { Lex4BItem::SEPARATOR, "SEPARATOR"_sv },
        { Lex4BItem::PREPROCESSOR, "PREPROCESSOR"_sv },
        { Lex4BItem::QUESTION_MARK, "QUESTION_MARK"_sv },
        { Lex4BItem::COLON, "COLON"_sv },
        { Lex4BItem::VAR, "VAR"_sv },
        { Lex4BItem::INTEGER, "INTEGER"_sv },
        { Lex4BItem::IDENTIFIER, "IDENTIFIER"_sv },
        { Lex4BItem::CHAR, "CHAR"_sv },
        { Lex4BItem::STRING, "STRING"_sv },
    }
};



namespace cerb {
    constexpr auto convert(Lex4BItem value) -> cerb::string_view
    {
        if (Lex4BItemItemsNames.contains(value)) {
            return Lex4BItemItemsNames[value];
        }

        return "";
    }
}

#define Lex4BTemplate                             \
    template<                                   \
        typename CharT = char,                       \
        typename TokenType = Lex4BItem,              \
        bool MayThrow = true,                        \
        size_t UID = 0,                             \
        bool AllowStringLiterals = true,             \
        bool AllowComments = true,                   \
        size_t MaxTerminals = 128,                  \
        size_t MaxSize4Terminals = 4>

#define Lex4BDefinition(CLASS) CalculatorTemplate struct CLASS : public Lex4B<>

#define Lex4BAccess using Lex4B<>::parent;               \
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
        using enum Lex4BItem;

Lex4BTemplate
struct Lex4B: public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS
    using enum Lex4BItem;

    constexpr Lex4B()
    : parent(
        '\"',
        '\'',
        STRING,
        CHAR,
        {
            { FOR, "for"_sv, true, 2 },
            { WHILE, "while"_sv, true, 2 },
            { RETURN, "return"_sv, true, 2 },
            { IF, "if"_sv, true, 2 },
            { VAR, "var"_sv, true, 2 },
            { INTEGER, "[0-9]+"_sv, false, 18 },
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
                { COLON, ':' },
            },
           {
                { ADD_EQ, "+="_sv },
                { SUB_EQ, "-="_sv },
                { MUL_EQ, "*="_sv },
                { DIV_EQ, "/="_sv },
                { MOD_EQ, "%="_sv },
                { INCREMENT, "++"_sv },
                { DECREMENT, "--"_sv },
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

#endif /* CERBERUS_B_LEXER */
