
%{
#include <cstdlib>
#include <string>
#include "calculator_imp_yacc.hpp"

%}

%token ADD
%token INTEGERAL
%define api.value.type { std::string }

%%
program:
	program expr 		{ std::cout << $2 << std::endl; }
	|
	;

expr:
	INTEGERAL			{ $$ = $1; }
	| expr ADD expr 		{ $$ = std::to_string(std::atoi($1.c_str()) + std::atoi($3.c_str())); }
	;

%%

cerb::gl::Map<CalculatorItem, int, 3> Table
{
    {CalculatorItem::INT, INTEGERAL},
    {CalculatorItem::ADD, ADD},
    {CalculatorItem::EoF, 0}
};


CalculatorImpYacc calculator{};

int yylex(void) {
    static size_t index{};

    const auto &elem = calculator.m_tokens[index++];

    if (elem.type == CalculatorItem::EoF) {
	    yylval = "";
	    return 0;
    }

    yylval = std::string(elem.repr.to_string());
    return Table[elem.type];
}

int main() {
    calculator.scan("20 + 30\n", "stdio");
    yyparse();
    return 0;
}
