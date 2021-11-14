#ifndef CERBERUS_CERBERUS_LEXER_HPP
#define CERBERUS_CERBERUS_LEXER_HPP

#include <iostream>
#include <cerberus/map.hpp>
#include <cerberus/analyzation/lex/lex.hpp>

using namespace cerb::literals;

// clang-format off

enum struct CerberusLexerBlock : size_t
{
    RESERVED         = 16UL,
    ARITHMETIC_OPERATOR = 64UL,
    BIT_OPERATOR     = 128UL,
    KEYWORD          = 256UL,
    LOGICAL_OPERATOR = 512UL,
    OPERATOR_SEPARATOR = 1024UL,
    PARSER           = 2048UL,
    TYPE             = 4096UL,
    VALUE            = 8192UL,
};

enum struct CerberusLexerItem : size_t
{
    UNDEFINED        = 16UL,
    EoF              = 0UL,
    ADD              = 64UL,
    SUB              = 65UL,
    MUL              = 66UL,
    DIV              = 67UL,
    MOD              = 68UL,
    ASSIGN           = 69UL,
    ADD_EQ           = 70UL,
    SUB_EQ           = 71UL,
    MUL_EQ           = 72UL,
    DIV_EQ           = 73UL,
    MOD_EQ           = 74UL,
    OR               = 128UL,
    AND              = 129UL,
    XOR              = 130UL,
    NOT              = 131UL,
    OR_EQ            = 132UL,
    AND_EQ           = 133UL,
    XOR_EQ           = 134UL,
    LEFT_SHIFT       = 135UL,
    RIGHT_SHIFT      = 136UL,
    LEFT_SHIFT_EQ    = 137UL,
    RIGHT_SHIFT_EQ   = 138UL,
    FOR              = 256UL,
    WHILE            = 257UL,
    RETURN           = 258UL,
    CASE             = 259UL,
    SWITCH           = 260UL,
    FUNC             = 261UL,
    VAR              = 262UL,
    LESS             = 512UL,
    GREATER          = 513UL,
    LOGICAL_NOT      = 514UL,
    LESS_EQ          = 515UL,
    GREATER_EQ       = 516UL,
    LOGICAL_OR       = 517UL,
    LOGICAL_AND      = 518UL,
    LOGICAL_EQUAL    = 519UL,
    LOGICAL_NOT_EQUAL = 520UL,
    CURLY_OPENING    = 1024UL,
    CURLY_CLOSING    = 1025UL,
    LEFT_PARENTHESIS = 1026UL,
    RIGHT_PARENTHESIS = 1027UL,
    ANGLE_OPENING    = 1028UL,
    ANGLE_CLOSING    = 1029UL,
    COMMA            = 1030UL,
    SEPARATOR        = 1031UL,
    PREPROCESSOR     = 1032UL,
    QUESTION_MARK    = 1033UL,
    ARROW            = 1034UL,
    STMT             = 2048UL,
    GLOBAL_VIEW      = 2049UL,
    INT_T            = 4097UL,
    CHAR_T           = 4098UL,
    FLOAT_T          = 4099UL,
    DOUBLE_T         = 4100UL,
    BUILTIN_AUTO_T   = 4101UL,
    TYPE             = 4096UL,
    INTEGER          = 8192UL,
    FLOAT            = 8193UL,
    DOUBLE           = 8194UL,
    IDENTIFIER       = 8195UL,
    CHAR             = 8196UL,
    STRING           = 8197UL,
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
%token FUNC             "func"
%token VAR              "var"
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
%token ARROW            "->"
%token STMT             "____parser_statement"
%token GLOBAL_VIEW      "____globalView"
%token INT_T            "int"
%token CHAR_T           "char"
%token FLOAT_T          "float"
%token DOUBLE_T         "double"
%token BUILTIN_AUTO_T   "__builtin_auto"
%token TYPE             4096
%token INTEGER          8192
%token FLOAT            8193
%token DOUBLE           8194
%token IDENTIFIER       8195
%token CHAR             8196
%token STRING           8197


cerb::Map<CerberusLexerItem, yytokentype> CerberusLexerItemsNamesConverter(
    {
        {CerberusLexerItem::UNDEFINED           , yytokentype::UNDEFINED},
        {CerberusLexerItem::EoF                 , yytokentype::EoF},
        {CerberusLexerItem::ADD                 , yytokentype::ADD},
        {CerberusLexerItem::SUB                 , yytokentype::SUB},
        {CerberusLexerItem::MUL                 , yytokentype::MUL},
        {CerberusLexerItem::DIV                 , yytokentype::DIV},
        {CerberusLexerItem::MOD                 , yytokentype::MOD},
        {CerberusLexerItem::ASSIGN              , yytokentype::ASSIGN},
        {CerberusLexerItem::ADD_EQ              , yytokentype::ADD_EQ},
        {CerberusLexerItem::SUB_EQ              , yytokentype::SUB_EQ},
        {CerberusLexerItem::MUL_EQ              , yytokentype::MUL_EQ},
        {CerberusLexerItem::DIV_EQ              , yytokentype::DIV_EQ},
        {CerberusLexerItem::MOD_EQ              , yytokentype::MOD_EQ},
        {CerberusLexerItem::OR                  , yytokentype::OR},
        {CerberusLexerItem::AND                 , yytokentype::AND},
        {CerberusLexerItem::XOR                 , yytokentype::XOR},
        {CerberusLexerItem::NOT                 , yytokentype::NOT},
        {CerberusLexerItem::OR_EQ               , yytokentype::OR_EQ},
        {CerberusLexerItem::AND_EQ              , yytokentype::AND_EQ},
        {CerberusLexerItem::XOR_EQ              , yytokentype::XOR_EQ},
        {CerberusLexerItem::LEFT_SHIFT          , yytokentype::LEFT_SHIFT},
        {CerberusLexerItem::RIGHT_SHIFT         , yytokentype::RIGHT_SHIFT},
        {CerberusLexerItem::LEFT_SHIFT_EQ       , yytokentype::LEFT_SHIFT_EQ},
        {CerberusLexerItem::RIGHT_SHIFT_EQ      , yytokentype::RIGHT_SHIFT_EQ},
        {CerberusLexerItem::FOR                 , yytokentype::FOR},
        {CerberusLexerItem::WHILE               , yytokentype::WHILE},
        {CerberusLexerItem::RETURN              , yytokentype::RETURN},
        {CerberusLexerItem::CASE                , yytokentype::CASE},
        {CerberusLexerItem::SWITCH              , yytokentype::SWITCH},
        {CerberusLexerItem::FUNC                , yytokentype::FUNC},
        {CerberusLexerItem::VAR                 , yytokentype::VAR},
        {CerberusLexerItem::LESS                , yytokentype::LESS},
        {CerberusLexerItem::GREATER             , yytokentype::GREATER},
        {CerberusLexerItem::LOGICAL_NOT         , yytokentype::LOGICAL_NOT},
        {CerberusLexerItem::LESS_EQ             , yytokentype::LESS_EQ},
        {CerberusLexerItem::GREATER_EQ          , yytokentype::GREATER_EQ},
        {CerberusLexerItem::LOGICAL_OR          , yytokentype::LOGICAL_OR},
        {CerberusLexerItem::LOGICAL_AND         , yytokentype::LOGICAL_AND},
        {CerberusLexerItem::LOGICAL_EQUAL       , yytokentype::LOGICAL_EQUAL},
        {CerberusLexerItem::LOGICAL_NOT_EQUAL   , yytokentype::LOGICAL_NOT_EQUAL},
        {CerberusLexerItem::CURLY_OPENING       , yytokentype::CURLY_OPENING},
        {CerberusLexerItem::CURLY_CLOSING       , yytokentype::CURLY_CLOSING},
        {CerberusLexerItem::LEFT_PARENTHESIS    , yytokentype::LEFT_PARENTHESIS},
        {CerberusLexerItem::RIGHT_PARENTHESIS   , yytokentype::RIGHT_PARENTHESIS},
        {CerberusLexerItem::ANGLE_OPENING       , yytokentype::ANGLE_OPENING},
        {CerberusLexerItem::ANGLE_CLOSING       , yytokentype::ANGLE_CLOSING},
        {CerberusLexerItem::COMMA               , yytokentype::COMMA},
        {CerberusLexerItem::SEPARATOR           , yytokentype::SEPARATOR},
        {CerberusLexerItem::PREPROCESSOR        , yytokentype::PREPROCESSOR},
        {CerberusLexerItem::QUESTION_MARK       , yytokentype::QUESTION_MARK},
        {CerberusLexerItem::ARROW               , yytokentype::ARROW},
        {CerberusLexerItem::STMT                , yytokentype::STMT},
        {CerberusLexerItem::GLOBAL_VIEW         , yytokentype::GLOBAL_VIEW},
        {CerberusLexerItem::INT_T               , yytokentype::TYPE},
        {CerberusLexerItem::CHAR_T              , yytokentype::TYPE},
        {CerberusLexerItem::FLOAT_T             , yytokentype::TYPE},
        {CerberusLexerItem::DOUBLE_T            , yytokentype::TYPE},
        {CerberusLexerItem::BUILTIN_AUTO_T      , yytokentype::TYPE},
        {CerberusLexerItem::TYPE                , yytokentype::TYPE},
        {CerberusLexerItem::INTEGER             , yytokentype::INTEGER},
        {CerberusLexerItem::FLOAT               , yytokentype::FLOAT},
        {CerberusLexerItem::DOUBLE              , yytokentype::DOUBLE},
        {CerberusLexerItem::IDENTIFIER          , yytokentype::IDENTIFIER},
        {CerberusLexerItem::CHAR                , yytokentype::CHAR},
        {CerberusLexerItem::STRING              , yytokentype::STRING},
    }
);
*/

inline cerb::Map<CerberusLexerBlock, cerb::string_view> CerberusLexerBlockNames(
    {
        { CerberusLexerBlock::RESERVED, "RESERVED"_sv },
        { CerberusLexerBlock::ARITHMETIC_OPERATOR, "ARITHMETIC_OPERATOR"_sv },
        { CerberusLexerBlock::BIT_OPERATOR, "BIT_OPERATOR"_sv },
        { CerberusLexerBlock::KEYWORD, "KEYWORD"_sv },
        { CerberusLexerBlock::LOGICAL_OPERATOR, "LOGICAL_OPERATOR"_sv },
        { CerberusLexerBlock::OPERATOR_SEPARATOR, "OPERATOR_SEPARATOR"_sv },
        { CerberusLexerBlock::PARSER, "PARSER"_sv },
        { CerberusLexerBlock::TYPE, "TYPE"_sv },
        { CerberusLexerBlock::VALUE, "VALUE"_sv },
    }
);

inline cerb::Map<CerberusLexerItem, cerb::string_view> CerberusLexerItemItemsNames(
    {
        { CerberusLexerItem::UNDEFINED, "UNDEFINED"_sv },
        { CerberusLexerItem::EoF, "EoF"_sv },
        { CerberusLexerItem::ADD, "ADD"_sv },
        { CerberusLexerItem::SUB, "SUB"_sv },
        { CerberusLexerItem::MUL, "MUL"_sv },
        { CerberusLexerItem::DIV, "DIV"_sv },
        { CerberusLexerItem::MOD, "MOD"_sv },
        { CerberusLexerItem::ASSIGN, "ASSIGN"_sv },
        { CerberusLexerItem::ADD_EQ, "ADD_EQ"_sv },
        { CerberusLexerItem::SUB_EQ, "SUB_EQ"_sv },
        { CerberusLexerItem::MUL_EQ, "MUL_EQ"_sv },
        { CerberusLexerItem::DIV_EQ, "DIV_EQ"_sv },
        { CerberusLexerItem::MOD_EQ, "MOD_EQ"_sv },
        { CerberusLexerItem::OR, "OR"_sv },
        { CerberusLexerItem::AND, "AND"_sv },
        { CerberusLexerItem::XOR, "XOR"_sv },
        { CerberusLexerItem::NOT, "NOT"_sv },
        { CerberusLexerItem::OR_EQ, "OR_EQ"_sv },
        { CerberusLexerItem::AND_EQ, "AND_EQ"_sv },
        { CerberusLexerItem::XOR_EQ, "XOR_EQ"_sv },
        { CerberusLexerItem::LEFT_SHIFT, "LEFT_SHIFT"_sv },
        { CerberusLexerItem::RIGHT_SHIFT, "RIGHT_SHIFT"_sv },
        { CerberusLexerItem::LEFT_SHIFT_EQ, "LEFT_SHIFT_EQ"_sv },
        { CerberusLexerItem::RIGHT_SHIFT_EQ, "RIGHT_SHIFT_EQ"_sv },
        { CerberusLexerItem::FOR, "FOR"_sv },
        { CerberusLexerItem::WHILE, "WHILE"_sv },
        { CerberusLexerItem::RETURN, "RETURN"_sv },
        { CerberusLexerItem::CASE, "CASE"_sv },
        { CerberusLexerItem::SWITCH, "SWITCH"_sv },
        { CerberusLexerItem::FUNC, "FUNC"_sv },
        { CerberusLexerItem::VAR, "VAR"_sv },
        { CerberusLexerItem::LESS, "LESS"_sv },
        { CerberusLexerItem::GREATER, "GREATER"_sv },
        { CerberusLexerItem::LOGICAL_NOT, "LOGICAL_NOT"_sv },
        { CerberusLexerItem::LESS_EQ, "LESS_EQ"_sv },
        { CerberusLexerItem::GREATER_EQ, "GREATER_EQ"_sv },
        { CerberusLexerItem::LOGICAL_OR, "LOGICAL_OR"_sv },
        { CerberusLexerItem::LOGICAL_AND, "LOGICAL_AND"_sv },
        { CerberusLexerItem::LOGICAL_EQUAL, "LOGICAL_EQUAL"_sv },
        { CerberusLexerItem::LOGICAL_NOT_EQUAL, "LOGICAL_NOT_EQUAL"_sv },
        { CerberusLexerItem::CURLY_OPENING, "CURLY_OPENING"_sv },
        { CerberusLexerItem::CURLY_CLOSING, "CURLY_CLOSING"_sv },
        { CerberusLexerItem::LEFT_PARENTHESIS, "LEFT_PARENTHESIS"_sv },
        { CerberusLexerItem::RIGHT_PARENTHESIS, "RIGHT_PARENTHESIS"_sv },
        { CerberusLexerItem::ANGLE_OPENING, "ANGLE_OPENING"_sv },
        { CerberusLexerItem::ANGLE_CLOSING, "ANGLE_CLOSING"_sv },
        { CerberusLexerItem::COMMA, "COMMA"_sv },
        { CerberusLexerItem::SEPARATOR, "SEPARATOR"_sv },
        { CerberusLexerItem::PREPROCESSOR, "PREPROCESSOR"_sv },
        { CerberusLexerItem::QUESTION_MARK, "QUESTION_MARK"_sv },
        { CerberusLexerItem::ARROW, "ARROW"_sv },
        { CerberusLexerItem::STMT, "STMT"_sv },
        { CerberusLexerItem::GLOBAL_VIEW, "GLOBAL_VIEW"_sv },
        { CerberusLexerItem::INT_T, "INT_T"_sv },
        { CerberusLexerItem::CHAR_T, "CHAR_T"_sv },
        { CerberusLexerItem::FLOAT_T, "FLOAT_T"_sv },
        { CerberusLexerItem::DOUBLE_T, "DOUBLE_T"_sv },
        { CerberusLexerItem::BUILTIN_AUTO_T, "BUILTIN_AUTO_T"_sv },
        { CerberusLexerItem::TYPE, "TYPE"_sv },
        { CerberusLexerItem::INTEGER, "INTEGER"_sv },
        { CerberusLexerItem::FLOAT, "FLOAT"_sv },
        { CerberusLexerItem::DOUBLE, "DOUBLE"_sv },
        { CerberusLexerItem::IDENTIFIER, "IDENTIFIER"_sv },
        { CerberusLexerItem::CHAR, "CHAR"_sv },
        { CerberusLexerItem::STRING, "STRING"_sv },
    }
);

namespace cerb::lex {
    constexpr auto convert(CerberusLexerBlock value) -> cerb::string_view
    {
        if (CerberusLexerBlockNames.count(value) != 0) {
            return CerberusLexerBlockNames[value];
        }
        return "UNDEFINED"_sv;
    }

    constexpr auto convert(CerberusLexerItem value) -> cerb::string_view
    {
        if (CerberusLexerItemItemsNames.count(value) != 0) {
            return CerberusLexerItemItemsNames[value];
        }
        return "UNDEFINED"_sv;
    }
}

template<typename T>
auto operator<<(T &os, CerberusLexerBlock value) -> T &
{
    os << cerb::lex::convert(value);
    return os;
}

template<typename T>
auto operator<<(T &os, CerberusLexerItem value) -> T &
{
    os << cerb::lex::convert(value);
    return os;
}



namespace cerb {
    constexpr auto convert(CerberusLexerItem value) -> cerb::string_view
    {
        if (CerberusLexerItemItemsNames.count(value) != 0) {
            return CerberusLexerItemItemsNames[value];
        }

        return "";
    }
}

#define CerberusLexerTemplate                             \
    template<                                   \
        typename CharT = char,                       \
        typename TokenType = CerberusLexerItem,              \
        bool MayThrow = true,                        \
        size_t UID = 0,                             \
        bool AllowStringLiterals = true,             \
        bool AllowComments = true,                   \
        size_t MaxTerminals = 64,                  \
        size_t MaxSize4Terminals = 4>

#define CerberusLexerDefinition(CLASS) CalculatorTemplate struct CLASS : public CerberusLexer<>

#define CerberusLexerAccess using CerberusLexer<>::parent;               \
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
        using enum CerberusLexerItem;

CerberusLexerTemplate
struct CerberusLexer: public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS
    using enum CerberusLexerItem;

    constexpr CerberusLexer()
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
            { FUNC, "func"_sv, true, 2 },
            { VAR, "var"_sv, true, 2 },
            { STMT, "____parser_statement"_sv, true, 2 },
            { GLOBAL_VIEW, "____globalView"_sv, true, 2 },
            { INT_T, "int"_sv, true, 2 },
            { CHAR_T, "char"_sv, true, 2 },
            { FLOAT_T, "float"_sv, true, 2 },
            { DOUBLE_T, "double"_sv, true, 2 },
            { BUILTIN_AUTO_T, "__builtin_auto"_sv, true, 2 },
            { TYPE, "TYPE"_sv, false, 12 },
            { INTEGER, "[0-9]+"_sv, false, 13 },
            { FLOAT, "[0-9]+[.][0-9]*[f]"_sv, false, 13 },
            { DOUBLE, "[0-9]+[.][0-9]*"_sv, false, 13 },
            { IDENTIFIER, "[a-zA_Z_]+[a-zA-Z0-9_]*"_sv, false, 13 }
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
                { ARROW, "->"_sv },
           }
        },
        "//",
        "/*",
        "*/"
    )
    {}
};

//clang-format on

#endif /* CERBERUS_CERBERUS_LEXER_HPP */
