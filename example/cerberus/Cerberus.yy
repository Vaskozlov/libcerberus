%{
#include "Cerberus_yacc_helper.hpp"
%}

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
%token INT_T            "int"
%token CHAR_T           "char"
%token FLOAT_T          "float"
%token DOUBLE_T         "double"
%token BUILTIN_AUTO_T   "__builtin_auto"
%token TYPE             262144
%token INTEGER          524288
%token FLOAT            524289
%token DOUBLE           524290
%token IDENTIFIER       524291
%token CHAR             524292
%token STRING           524293

%left  "+" "-"
%left  "*"
%left  "("
%left  "func" IDENTIFIER

%define api.value.type { CL::ParserNode* }

%%
program:
	program functions 			{ CL::ParserNode::print($2); std::cout << "Parsing finished" << std::endl; }
	| %empty
	;

functions: 	"func" IDENTIFIER "(" ")" "{" stmt "}"				{ $$ = CL::ParserNode::construct_root($1, {$2, CL::ParserNode::construct_node(auto_token), $6}); }
		| "func" IDENTIFIER "(" ")" "->" TYPE "{" stmt "}"	{ $$ = CL::ParserNode::construct_root($1, {$2, $6, $8}); }
		| %empty
		;

expr:
	INTEGER					{ $$ = $1; }
	| FLOAT					{ $$ = $1; }
	| DOUBLE				{ $$ = $1; }
	| STRING				{ $$ = $1; }
	| CHAR					{ $$ = $1; }
	| IDENTIFIER				{ $$ = $1; }
	| expr "+" expr				{ $$ = CL::ParserNode::construct_root($2, {$1, $3});  }
	| expr "*" expr				{ $$ = CL::ParserNode::construct_root($2, {$1, $3});  }
	| "(" expr ")"				{ $$ = $2; }
	;


stmt:   stmt expr ";" 	{ if ($1 == nullptr) {$$ = $2; } else { $$ = CL::ParserNode::construct_root($1, {$2}); $$ = $1; } }
	| %empty	{ $$ = CL::ParserNode::construct_node(statement_token); }
	;
%%

#include "Cerberus_lexerYACC.hpp"

extern "C" void yyerror(const char *message) {
    std::cerr << message << std::endl;
    CL::ParserNode::print(yylval);
    cerberus_lexer.syntax_error(current_token);
}


int yylex(void) {
    static size_t index{};
    current_token = &cerberus_lexer.m_tokens[index++];
    yylval = CL::ParserNode::construct_node(*current_token);
    return CerberusLexerItemsNamesConverter[current_token->type];
}


auto main() -> int {
    cerberus_lexer.scan("func test() -> int { 10 + 20; 20; 20 + 30 * (2 + 8); 30; }", "stdio");
    yyparse();
}
