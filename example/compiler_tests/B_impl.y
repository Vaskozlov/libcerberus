%{
#include <cstdlib>
#include <string>
#include "B_lexer.hpp"
%}

%token	EoF 17
%token RETURN 16386 WHILE 16385 IF 16387 VAR 131072 IDENTIFIER 262145 NUMCONST 262144 STRINGCONST 262147
%token OR 8192 AND 8193 EQ 32775 NE 32776 PP 4107 MM 4108 PL_EQ 4102 MI_EQ 4103
%token COLON 65546
%token CURLY_OPENING 65536
%token CURLY_CLOSING 65537
%token LEFT_PARENTHESIS 65538
%token RIGHT_PARENTHESIS 65539
%token SEPARATOR 65543
%token COMMA 65542
%token ASSIGN 4101
%token ANGLE_OPENING 65540
%token ANGLE_CLOSING 65541

%%

library:	functions;
functions:	IDENTIFIER paramdecls COLON stmt
		| %empty;

paramdecls:	paramdecl | %empty;
paramdecl:	paramdecl | IDENTIFIER
		| IDENTIFIER;
stmt:		CURLY_OPENING stmt CURLY_CLOSING
		| IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt
		| WHILE LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt
		| RETURN expr SEPARATOR
		| var_defs SEPARATOR
 		| expr SEPARATOR
 		| SEPARATOR;

com_stmt:	LEFT_PARENTHESIS
		| com_stmt stmt;

var_defs:	VAR var_def1
		| var_defs COMMA var_def1;

var_def1:	IDENTIFIER ASSIGN expr
		| IDENTIFIER;

expr:		NUMCONST
		| STRINGCONST
		| IDENTIFIER
		| LEFT_PARENTHESIS expr RIGHT_PARENTHESIS
		| expr ANGLE_OPENING expr ANGLE_CLOSING
		| expr ASSIGN expr
		;

%%
