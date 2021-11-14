%{
#include <fstream>
#include <sstream>
#include <iostream>
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

%left  "+" "-"
%left  "*"
%left  "("
%left IDENTIFIER
%left  "func" "return"

%define api.value.type { CL::ParserNode* }

%%

library:
		library function	{ $$ = CL::ParserNode::construct_root( CL::parser.global_view_node, {$2}); }
		| %empty		{ $$ = CL::parser.global_view_node; }
		;

function:	"func" IDENTIFIER "(" ")" "{" stmt "}"	{ $$ = CL::ParserNode::construct_root($1, {$2, $6}); }
		| %empty;

expr:
	INTEGER						{ $$ = $1; }
	| FLOAT						{ $$ = $1; }
	| DOUBLE					{ $$ = $1; }
	| CHAR						{ $$ = $1; }
	| STRING					{ $$ = $1; }
	| IDENTIFIER					{ $$ = $1; }
	| expr "+" expr					{ $$ = CL::ParserNode::construct_root($2, {$1, $3}); }
	| expr "*" expr					{ $$ = CL::ParserNode::construct_root($2, {$1, $3}); }
	| "(" expr ")"					{ $$ = $2; }
	;


stmt:
	stmt expr ";"					{ $$ = CL::ParserNode::construct_root($1, {$2});  }
	| stmt "return" expr ";"			{ $$ = CL::ParserNode::construct_root($1, {CL::ParserNode::construct_root($2, {$3})}); }
	| stmt ";" 					{ $$ = CL::ParserNode::construct_root($1, {$2}); }
	| stmt "{" stmt "}"				{ $$ = CL::ParserNode::construct_root($1, {$3}); }
	| %empty 					{ $$ = CL::Parser::new_statement(); }

%%


#include "Cerberus_lexerYACC.hpp"

extern "C" void yyerror(const char *message) {
    std::cerr << message << std::endl;
    CL::ParserNode::print(yylval);
    CL::parser.cerberus_lexer.syntax_error(CL::parser());
}


int yylex(void) {
    static size_t index{};
    yylval = CL::ParserNode::construct_node(*CL::parser.next_token());
    return CerberusLexerItemsNamesConverter[CL::parser()->type];
}


auto main() -> int {
    std::ifstream t("/home/vaskozlov/projects/libcerberus/example/cerberus/example.lcerb");
    std::stringstream buffer{};
    buffer << t.rdbuf();
    t.close();
    std::string data = buffer.str();

    CL::parser.cerberus_lexer.scan(data.c_str(), "example.lcerb");
    yyparse();
    CL::parser.print();
}
