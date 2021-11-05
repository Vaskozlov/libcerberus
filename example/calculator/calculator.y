%{
#include <cstdlib>
#include <string>
#include <iostream>
#include "calculator_yacc.hpp"
#include <cerberus/analyzation/parser/tree.hpp>
%}

%token UNDEFINED        16
%token EoF              0
%token SELF             18
%token EXPR             19
%token TERM             20
%token EMPTY            21
%token SIN              "sin"
%token ADD              "+"
%token SUB              "-"
%token MUL              "*"
%token LEFT_PARENTHESIS "("
%token RIGHT_PARENTHESIS ")"
%token INT              32768

%left  "+" "-"
%left  "*"
%left  "("

%define api.value.type { ParserNode* }

%%
program:
	program expr 		{ ParserNode::print($2); std::cout << "Parsing finished" << std::endl; }
	|
	;

expr:
	INT			{ $$ = $1; }
	| expr "*" expr 	{ $$ = ParserNode::construct_root($2, {$1, $3});  }
	| expr "+" expr 	{ $$ = ParserNode::construct_root($2, {$1, $3});  }
	| "(" expr ")"		{ $$ = $2;  }
	;

%%

CalculatorImpYacc calculator{};

constexpr cerb::gl::Map<CalculatorItem, yytokentype, 13> CalculatorItemsNamesConverter{
    true, {
    {CalculatorItem::UNDEFINED       , yytokentype::UNDEFINED},
    {CalculatorItem::EoF             , yytokentype::EoF},
    {CalculatorItem::SELF            , yytokentype::SELF},
    {CalculatorItem::EXPR            , yytokentype::EXPR},
    {CalculatorItem::TERM            , yytokentype::TERM},
    {CalculatorItem::EMPTY           , yytokentype::EMPTY},
    {CalculatorItem::SIN             , yytokentype::SIN},
    {CalculatorItem::ADD             , yytokentype::ADD},
    {CalculatorItem::SUB             , yytokentype::SUB},
    {CalculatorItem::MUL             , yytokentype::MUL},
    {CalculatorItem::LEFT_PARENTHESIS, yytokentype::LEFT_PARENTHESIS},
    {CalculatorItem::RIGHT_PARENTHESIS, yytokentype::RIGHT_PARENTHESIS},
    {CalculatorItem::INT             , yytokentype::INT}
    }
};

int yylex(void) {
    static size_t index{};

    const auto &elem = calculator.m_tokens[index++];

    if (elem.type == CalculatorItem::EoF) {
	    yylval = ParserNode::construct_node(elem);
	    return 0;
    }

    yylval = ParserNode::construct_node(elem);
    return CalculatorItemsNamesConverter[elem.type];
}

int main() {
    calculator.scan("20 + 30 + 40 * (2 + 8)\n", "stdio");
    yyparse();
    return 0;
}
