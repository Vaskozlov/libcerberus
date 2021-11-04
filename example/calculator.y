%{
#include <cstdlib>
#include <string>
#include <iostream>
#include "calculator_imp_yacc.hpp"
%}

%token UNDEFINED        16
%token EoF              17
%token SELF             18
%token EXPR             19
%token TERM             20
%token EMPTY            21
%token SIN              4096
%token ADD              "+"
%token LEFT_PARENTHESIS "("
%token RIGHT_PARENTHESIS ")"
%token INT              32768
%token SUB '-'

%define api.value.type { std::string }

%%
program:
	program expr 		{ std::cout << $2 << std::endl; }
	|
	;

expr:
	INT				{ $$ = $1; std::cout << $1 << std::endl; }
	| expr "+" expr 		{ $$ = std::to_string(std::atoi($1.c_str()) + std::atoi($3.c_str())); std::cout << $1 << ' ' << $2 << ' ' << $3 << std::endl; }
	;

%%


CalculatorImpYacc calculator{};

constexpr cerb::gl::Map<CalculatorItem, yytokentype, 11> CalculatorItemsNamesConverter{
    true, {
	    {CalculatorItem::UNDEFINED       , yytokentype::UNDEFINED},
	    {CalculatorItem::EoF             , yytokentype::EoF},
	    {CalculatorItem::SELF            , yytokentype::SELF},
	    {CalculatorItem::EXPR            , yytokentype::EXPR},
	    {CalculatorItem::TERM            , yytokentype::TERM},
	    {CalculatorItem::EMPTY           , yytokentype::EMPTY},
	    {CalculatorItem::SIN             , yytokentype::SIN},
	    {CalculatorItem::ADD             , yytokentype::ADD},
	    {CalculatorItem::LEFT_PARENTHESIS, yytokentype::LEFT_PARENTHESIS},
	    {CalculatorItem::RIGHT_PARENTHESIS, yytokentype::RIGHT_PARENTHESIS},
	    {CalculatorItem::INT             , yytokentype::INT}
    }
};

int yylex(void) {
    static size_t index{};

    const auto &elem = calculator.m_tokens[index++];

    if (elem.type == CalculatorItem::EoF) {
	    yylval = "";
	    return 0;
    }

    yylval = std::string(elem.repr.to_string());
    return CalculatorItemsNamesConverter[elem.type];
}

int main() {
    calculator.scan("20 + 30\n", "stdio");
    yyparse();
    return 0;
}
