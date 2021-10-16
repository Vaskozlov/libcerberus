// clang-format off

%{
#include "calculator_imp_yacc.hpp"
%}

%token INTEGERAL
%token ADD

%%


%%

// clang-format on

cerb::gl::Map<CalculatorItem, int> Table
{
    {CalculatorItem::INT, INTEGERAL},
    {CalculatorItem::ADD, ADD}
};

size_t index{};
CalculatorImpYacc calculator{};

int yylex(void) {
    yylvalue = calculator.m_tokens[index++];
    return Table[yylvalue.type];
}

int main() {

    calculator.scan("20 + 30", "stdio");
}
