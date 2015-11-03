%union {
	struct decl *decl;
	struct stmt *stmt;
	struct expr *expr;
	struct type *type;
	struct param_list *param_list;
	char * str;
	int ival;
};

%type <decl> program decl_list decl nonempty_decl_list
%type <stmt> stmt stmt_list nonempty_stmt_list 
%type <expr> expr primary_expr secondary_expr third_expr fourth_expr fifth_expr sixth_expr seventh_expr eighth_expr ninth_expr optional_expr expr_list nonempty_expr_list integer_literal string_literal char_literal
%type <param_list> param_list nonempty_param_list param 
%type <type> type 
%type <str> ident 

%token<str> TOKEN_STRING_LITERAL
%token<ival> TOKEN_CHAR_LITERAL
%token<ival> TOKEN_INTEGER_LITERAL

%token TOKEN_ARRAY
%token TOKEN_BOOLEAN
%token TOKEN_CHAR
%token TOKEN_INTEGER
%token TOKEN_STRING

%token TOKEN_TRUE
%token TOKEN_FALSE

%token TOKEN_ELSE
%token TOKEN_FOR
%token TOKEN_FUNCTION
%token TOKEN_IF
%token TOKEN_PRINT
%token TOKEN_RETURN
%token TOKEN_VOID
%token TOKEN_WHILE /* reserved keyword, does not used in C-minor */
%token TOKEN_NOT

%token TOKEN_LEFT_PARENTHESIS
%token TOKEN_RIGHT_PARENTHESIS
%token TOKEN_LEFT_CURLY_BRACKET
%token TOKEN_RIGHT_CURLY_BRACKET
%token TOKEN_LEFT_BRACKET
%token TOKEN_RIGHT_BRACKET

%token TOKEN_COLON
%token TOKEN_SEMI_COLON
%token TOKEN_COMMA

%token TOKEN_INCREMENT
%token TOKEN_DECREMENT
%token TOKEN_MINUS
%token TOKEN_ADD
%token TOKEN_EXPONENT
%token TOKEN_MULTIPLY
%token TOKEN_DIVIDE
%token TOKEN_MOD
%token TOKEN_ASSIGN

%token TOKEN_LT
%token TOKEN_LEQ
%token TOKEN_GT
%token TOKEN_GEQ
%token TOKEN_EQ
%token TOKEN_NEQ
%token TOKEN_AND
%token TOKEN_OR

%token<str> TOKEN_IDENTIFIER
%token TOKEN_UNDEFINED /* defined but not used */

%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	#include "./type.h"
  #include "./decl.h"
	#include "./expr.h"
  #include "./stmt.h"
	#include "./param_list.h"

	extern char *yytext;
	extern int yylex();
	extern int yyerror(char *str);

	#define nullptr 0

	char * str_create(char *str) {
		char *name = malloc(512 * sizeof(*name));
		strcpy(name, str);
		return name;
	}

%}

%nonassoc "then"
%nonassoc TOKEN_ELSE

%%

program : decl_list {$$ = $1; yylval.decl = $1;}
				;

ident : TOKEN_IDENTIFIER {$$ = str_create($1); }
			;

type : TOKEN_STRING {$$ = type_create(TYPE_STRING, nullptr, nullptr); }
		 | TOKEN_INTEGER {$$ = type_create(TYPE_INTEGER, nullptr, nullptr); }
		 | TOKEN_BOOLEAN {$$ = type_create(TYPE_BOOLEAN, nullptr, nullptr); }
		 | TOKEN_CHAR {$$ = type_create(TYPE_CHAR, nullptr, nullptr); }
		 | TOKEN_VOID {$$ = type_create(TYPE_VOID, nullptr, nullptr); }
		 | TOKEN_ARRAY TOKEN_LEFT_BRACKET expr TOKEN_RIGHT_BRACKET type {$$ = type_create(TYPE_ARRAY, param_list_create(nullptr, nullptr, $3, nullptr), $5); }
		 | TOKEN_FUNCTION type TOKEN_LEFT_PARENTHESIS param_list TOKEN_RIGHT_PARENTHESIS {$$ = type_create(TYPE_FUNCTION, $4, $2); }
		 ;

param_list : /* empty */ {$$ = nullptr;}
					 | nonempty_param_list {$$ = $1;}
					 ;

nonempty_param_list : nonempty_param_list TOKEN_COMMA param {$1->next = $3; $$ = $1; }
										| param {$$ = $1; }
										;

param : ident TOKEN_COLON type {$$ = param_list_create($1, $3, nullptr, nullptr); }
			;

nonempty_decl_list : decl nonempty_decl_list {$1->next = $2; $$ = $1; }
										| decl { $$ = $1; }
										;

decl_list : nonempty_decl_list {$$ = $1;}
					| /* empty */ {$$ = nullptr;}
					;

decl : ident TOKEN_COLON type TOKEN_ASSIGN expr TOKEN_SEMI_COLON /* ident : type = expr; */ {$$ = decl_create($1, $3, $5, nullptr, nullptr); }
		 | ident TOKEN_COLON type TOKEN_SEMI_COLON /* ident : type; */ {$$ = decl_create($1, $3, nullptr, nullptr, nullptr); }
		 | ident TOKEN_COLON type TOKEN_ASSIGN TOKEN_LEFT_CURLY_BRACKET stmt_list TOKEN_RIGHT_CURLY_BRACKET /* ident : type = { stmt_list } */ {$$ = decl_create_func($1, $3, nullptr, $6, nullptr, 0); }
		 | ident TOKEN_COLON type TOKEN_ASSIGN TOKEN_LEFT_CURLY_BRACKET expr_list TOKEN_RIGHT_CURLY_BRACKET TOKEN_SEMI_COLON /* array */ {$$ = decl_create($1, $3, $6, nullptr, nullptr);}
		 ;

stmt : decl {$$ = stmt_create(STMT_DECL, $1, nullptr, nullptr, nullptr, nullptr, nullptr); }
		 | expr TOKEN_SEMI_COLON {$$ = stmt_create(STMT_EXPR, nullptr, nullptr, $1, nullptr, nullptr, nullptr); }
		 | TOKEN_LEFT_CURLY_BRACKET stmt_list TOKEN_RIGHT_CURLY_BRACKET {$$ = stmt_create(STMT_BLOCK, nullptr, nullptr, nullptr, nullptr, $2, nullptr); }
		 | TOKEN_IF TOKEN_LEFT_PARENTHESIS expr TOKEN_RIGHT_PARENTHESIS stmt %prec "then" {$$ = stmt_create(STMT_IF_ELSE, nullptr, nullptr, $3, nullptr, $5, nullptr); }
		 | TOKEN_IF TOKEN_LEFT_PARENTHESIS expr TOKEN_RIGHT_PARENTHESIS stmt TOKEN_ELSE stmt {$$ = stmt_create(STMT_IF_ELSE, nullptr, nullptr, $3, nullptr, $5, $7); }
		 | TOKEN_RETURN optional_expr TOKEN_SEMI_COLON { $$ = stmt_create(STMT_RETURN, nullptr, nullptr, $2, nullptr, nullptr, nullptr); }
		 | TOKEN_PRINT expr_list TOKEN_SEMI_COLON  { $$ = stmt_create(STMT_PRINT, nullptr, nullptr, $2, nullptr, nullptr, nullptr); }
		 | TOKEN_FOR TOKEN_LEFT_PARENTHESIS optional_expr TOKEN_SEMI_COLON optional_expr TOKEN_SEMI_COLON optional_expr TOKEN_RIGHT_PARENTHESIS stmt { $$ = stmt_create(STMT_FOR, nullptr, $3, $5, $7, $9, nullptr); }
		 ;

stmt_list : /* empty */ { $$ = nullptr; }
					| nonempty_stmt_list { $$ = $1; }
					;

nonempty_stmt_list : stmt nonempty_stmt_list { $1->next = $2; $$ = $1; }
									 | stmt { $$ = $1; }
									 ;

integer_literal : TOKEN_INTEGER_LITERAL { $$ = expr_create_integer_literal($1);}
								;

string_literal : TOKEN_STRING_LITERAL { $$ = expr_create_string_literal(str_create($1)); }
							 ;

char_literal : TOKEN_CHAR_LITERAL { $$ = expr_create_character_literal(str_create(yytext)[1]);}

primary_expr : ident { $$ = expr_create_name($1); }
						 | integer_literal { $$ = $1; }
						 | string_literal { $$ = $1; }
						 | char_literal { $$ = $1; }
						 | TOKEN_TRUE { $$ = expr_create_boolean_literal(1); }
						 | TOKEN_FALSE { $$ = expr_create_boolean_literal(0); }
						 | TOKEN_LEFT_PARENTHESIS optional_expr TOKEN_RIGHT_PARENTHESIS { $$ = expr_create_parent($2); }
						 | ident TOKEN_LEFT_PARENTHESIS expr_list TOKEN_RIGHT_PARENTHESIS { $$ = expr_create_function_call($1, $3); }
						 ;

secondary_expr : primary_expr { $$ = $1; }
						   | secondary_expr TOKEN_INCREMENT { $$ = expr_create(EXPR_INCR, nullptr, $1); }
						   | secondary_expr TOKEN_DECREMENT { $$ = expr_create(EXPR_DECR, nullptr, $1); }
						   ;

third_expr : secondary_expr { $$ = $1; }
					 | TOKEN_MINUS third_expr { $$ = expr_create(EXPR_SUB, nullptr, $2); }
					 | TOKEN_NOT third_expr  { $$ = expr_create(EXPR_NOT, nullptr, $2); }
					 ;

fourth_expr : third_expr { $$ = $1; }
						| fourth_expr TOKEN_EXPONENT third_expr { $$ = expr_create(EXPR_EXP, $1, $3); }
						;

fifth_expr : fourth_expr { $$ = $1; }
					 | fifth_expr TOKEN_MULTIPLY fourth_expr { $$ = expr_create(EXPR_MUL, $1, $3); }
					 | fifth_expr TOKEN_DIVIDE fourth_expr  { $$ = expr_create(EXPR_DIV, $1, $3); }
					 | fifth_expr TOKEN_MOD fourth_expr { $$ = expr_create(EXPR_MOD, $1, $3); }
					 ;

sixth_expr : fifth_expr { $$ = $1; }
					 | sixth_expr TOKEN_ADD fifth_expr { $$ = expr_create(EXPR_ADD, $1, $3); }
					 | sixth_expr TOKEN_MINUS fifth_expr { $$ = expr_create(EXPR_SUB, $1, $3); }
					 ;

seventh_expr : sixth_expr { $$ = $1; }
						 | seventh_expr TOKEN_EQ sixth_expr { $$ = expr_create(EXPR_EQ, $1, $3); }
						 | seventh_expr TOKEN_NEQ sixth_expr { $$ = expr_create(EXPR_NEQ, $1, $3); }
						 | seventh_expr TOKEN_LT sixth_expr { $$ = expr_create(EXPR_LT, $1, $3); }
						 | seventh_expr TOKEN_LEQ sixth_expr  { $$ = expr_create(EXPR_LEQ, $1, $3); }
						 | seventh_expr TOKEN_GT sixth_expr { $$ = expr_create(EXPR_GT, $1, $3); }
						 | seventh_expr TOKEN_GEQ sixth_expr { $$ = expr_create(EXPR_GEQ, $1, $3); }
						 ;

eighth_expr : seventh_expr { $$ = $1; }
						| eighth_expr TOKEN_AND seventh_expr { $$ = expr_create(EXPR_AND, $1, $3); }
						;

ninth_expr : eighth_expr { $$ = $1; }
					 | ninth_expr TOKEN_OR eighth_expr { $$ = expr_create(EXPR_OR, $1, $3); }
					 ;

expr : ninth_expr { $$ = $1; }
		 | ident TOKEN_ASSIGN ninth_expr { $$ = expr_create(EXPR_ASSIGN, expr_create_name($1), $3); }
		 | ident TOKEN_ASSIGN TOKEN_LEFT_CURLY_BRACKET expr_list TOKEN_RIGHT_CURLY_BRACKET /* {1,2,3} */ { $$ = expr_create(EXPR_ASSIGN, expr_create_name($1), expr_create_curly($4)); }
		 ;

optional_expr : /* empty */ { $$ = nullptr; }
							| expr { $$ = $1; }
							;

expr_list : /* empty */ { $$ = nullptr; }
					| nonempty_expr_list { $$ = $1; }
					;

nonempty_expr_list : expr TOKEN_COMMA nonempty_expr_list { $1->next = $3; $$ = $1; }
									 | expr { $$ = $1; }
									 ;

%%

int yyerror(char *str) {
	fprintf(stderr, "parse error: %s \n", str);
	return -1;
}
