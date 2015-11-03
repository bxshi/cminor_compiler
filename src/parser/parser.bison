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

program : decl_list { printf("program\n"); $$ = $1; yylval.decl = $1;}
				;

ident : TOKEN_IDENTIFIER { printf("ident %s\n", $1); $$ = str_create($1); }
			;

type : TOKEN_STRING { printf("type_string\n"); $$ = type_create(TYPE_STRING, nullptr, nullptr); }
		 | TOKEN_INTEGER { printf("type_integer\n"); $$ = type_create(TYPE_INTEGER, nullptr, nullptr); }
		 | TOKEN_BOOLEAN { printf("type_boolean\n"); $$ = type_create(TYPE_BOOLEAN, nullptr, nullptr); }
		 | TOKEN_CHAR { printf("type_char\n"); $$ = type_create(TYPE_CHAR, nullptr, nullptr); }
		 | TOKEN_VOID { printf("type_void\n"); $$ = type_create(TYPE_VOID, nullptr, nullptr); }
		 | TOKEN_ARRAY TOKEN_LEFT_BRACKET expr TOKEN_RIGHT_BRACKET type { printf("type_array\n"); $$ = type_create(TYPE_ARRAY, param_list_create(nullptr, nullptr, $3, nullptr), $5); }
		 | TOKEN_FUNCTION type TOKEN_LEFT_PARENTHESIS param_list TOKEN_RIGHT_PARENTHESIS { printf("type_function\n"); $$ = type_create(TYPE_FUNCTION, $4, $2); }
		 ;

param_list : /* empty */ { printf("empty_param_list\n"); $$ = nullptr; }
					 | nonempty_param_list { printf("param_list\n"); $$ = $1; }
					 ;

nonempty_param_list : nonempty_param_list TOKEN_COMMA param { printf("nonempty_param_list\n"); $1->next = $3; $$ = $1; }
										| param { printf("nonempty_param\n"); $$ = $1; }
										;

param : ident TOKEN_COLON type { printf("param\n"); $$ = param_list_create($1, $3, nullptr, nullptr); }
			;

nonempty_decl_list : decl nonempty_decl_list {printf("nonempty_decl_list\n"); $1->next = $2; $$ = $1; }
										| decl { $$ = $1; }
										;

decl_list : nonempty_decl_list {$$ = $1;}
					| /* empty */ { printf("empty_decl_list\n"); $$ = nullptr; }
					;

decl : ident TOKEN_COLON type TOKEN_ASSIGN expr TOKEN_SEMI_COLON /* ident : type = expr; */ { printf("decl_1 %s\n", $1); $$ = decl_create($1, $3, $5, nullptr, nullptr); }
		 | ident TOKEN_COLON type TOKEN_SEMI_COLON /* ident : type; */ { printf("decl_2\n"); $$ = decl_create($1, $3, nullptr, nullptr, nullptr); }
		 | ident TOKEN_COLON type TOKEN_ASSIGN TOKEN_LEFT_CURLY_BRACKET stmt_list TOKEN_RIGHT_CURLY_BRACKET /* ident : type = { stmt_list } */ { printf("decl_3\n"); $$ = decl_create_func($1, $3, nullptr, $6, nullptr, 0); }
		 | ident TOKEN_COLON type TOKEN_ASSIGN TOKEN_LEFT_CURLY_BRACKET expr_list TOKEN_RIGHT_CURLY_BRACKET TOKEN_SEMI_COLON /* array */ { printf("decl_4\n"); $$ = decl_create($1, $3, $6, nullptr, nullptr);}
		 ;

stmt : decl { printf("stmt_1\n"); $$ = stmt_create(STMT_DECL, $1, nullptr, nullptr, nullptr, nullptr, nullptr); }
		 | expr TOKEN_SEMI_COLON { printf("stmt_2\n"); $$ = stmt_create(STMT_EXPR, nullptr, nullptr, $1, nullptr, nullptr, nullptr); }
		 | TOKEN_LEFT_CURLY_BRACKET stmt_list TOKEN_RIGHT_CURLY_BRACKET { printf("stmt_3\n"); $$ = stmt_create(STMT_BLOCK, nullptr, nullptr, nullptr, nullptr, $2, nullptr); }
		 | TOKEN_IF TOKEN_LEFT_PARENTHESIS expr TOKEN_RIGHT_PARENTHESIS stmt %prec "then" { printf("stmt_4\n"); $$ = stmt_create(STMT_IF_ELSE, nullptr, nullptr, $3, nullptr, $5, nullptr); }
		 | TOKEN_IF TOKEN_LEFT_PARENTHESIS expr TOKEN_RIGHT_PARENTHESIS stmt TOKEN_ELSE stmt { printf("stmt_5\n"); $$ = stmt_create(STMT_IF_ELSE, nullptr, nullptr, $3, nullptr, $5, $7); }
		 | TOKEN_RETURN optional_expr TOKEN_SEMI_COLON { printf("stmt_6\n"); $$ = stmt_create(STMT_RETURN, nullptr, nullptr, $2, nullptr, nullptr, nullptr); }
		 | TOKEN_PRINT expr_list TOKEN_SEMI_COLON  { printf("stmt_7\n"); $$ = stmt_create(STMT_PRINT, nullptr, nullptr, $2, nullptr, nullptr, nullptr); }
		 | TOKEN_FOR TOKEN_LEFT_PARENTHESIS optional_expr TOKEN_SEMI_COLON optional_expr TOKEN_SEMI_COLON optional_expr TOKEN_RIGHT_PARENTHESIS stmt { printf("stmt_8\n"); $$ = stmt_create(STMT_FOR, nullptr, $3, $5, $7, $9, nullptr); }
		 ;

stmt_list : /* empty */ { printf("empty_stmt_list\n"); $$ = nullptr; }
					| nonempty_stmt_list { printf("stmt_list\n"); $$ = $1; }
					;

nonempty_stmt_list : stmt nonempty_stmt_list { printf("nonempty_stmt_list\n"); $1->next = $2; $$ = $1; }
									 | stmt { printf("nonempty_stmt\n"); $$ = $1; }
									 ;

integer_literal : TOKEN_INTEGER_LITERAL { printf("integer_literal %d \n", $1); $$ = expr_create_integer_literal($1);}
								;

string_literal : TOKEN_STRING_LITERAL {printf("string_literal %s strlen %lu\n", $1, strlen($1)); $$ = expr_create_string_literal(str_create($1)); printf(" parsed string_literal %s strlen %lu\n", $$->string_literal, strlen($$->string_literal));}
							 ;

char_literal : TOKEN_CHAR_LITERAL { printf("char_literal %lu %c\n", strlen(yytext), str_create(yytext)[1]); $$ = expr_create_character_literal(str_create(yytext)[1]);}

primary_expr : ident { printf("primary_expr_1\n"); $$ = expr_create_name($1); }
						 | integer_literal { printf("primary_expr_2\n"); $$ = $1; }
						 | string_literal { printf("primary_expr_3 %s %lu\n", $1->string_literal, strlen($1->string_literal)); $$ = $1; }
						 | char_literal { printf("primary_expr_4\n"); $$ = $1; }
						 | TOKEN_TRUE { printf("primary_expr_5\n"); $$ = expr_create_boolean_literal(1); }
						 | TOKEN_FALSE { printf("primary_expr_6\n"); $$ = expr_create_boolean_literal(0); }
						 | TOKEN_LEFT_PARENTHESIS optional_expr TOKEN_RIGHT_PARENTHESIS { printf("primary_expr_7\n"); $$ = expr_create_parent($2); }
						 | ident TOKEN_LEFT_PARENTHESIS expr_list TOKEN_RIGHT_PARENTHESIS { printf("primary_expr_8\n"); $$ = expr_create_function_call($1, $3); }
						 ;

secondary_expr : primary_expr { printf("secondary_expr\n"); $$ = $1; }
						   | secondary_expr TOKEN_INCREMENT { printf("increment\n"); $$ = expr_create(EXPR_INCR, nullptr, $1); }
						   | secondary_expr TOKEN_DECREMENT { printf("decrement\n"); $$ = expr_create(EXPR_DECR, nullptr, $1); }
						   ;

third_expr : secondary_expr { printf("third_expr\n"); $$ = $1; }
					 | TOKEN_MINUS third_expr {  printf("minus\n"); $$ = expr_create(EXPR_SUB, nullptr, $2); }
					 | TOKEN_NOT third_expr  { printf("not\n"); $$ = expr_create(EXPR_NOT, nullptr, $2); }
					 ;

fourth_expr : third_expr { printf("fourth_expr\n"); $$ = $1; }
						| fourth_expr TOKEN_EXPONENT third_expr { printf("exponent\n"); $$ = expr_create(EXPR_EXP, $1, $3); }
						;

fifth_expr : fourth_expr { printf("fifth_expr\n"); $$ = $1; }
					 | fifth_expr TOKEN_MULTIPLY fourth_expr { printf("multiply\n"); $$ = expr_create(EXPR_MUL, $1, $3); }
					 | fifth_expr TOKEN_DIVIDE fourth_expr  { printf("divide\n"); $$ = expr_create(EXPR_DIV, $1, $3); }
					 | fifth_expr TOKEN_MOD fourth_expr { printf("mod\n"); $$ = expr_create(EXPR_MOD, $1, $3); }
					 ;

sixth_expr : fifth_expr { printf("sixth_expr\n"); $$ = $1; }
					 | sixth_expr TOKEN_ADD fifth_expr { printf("add\n"); $$ = expr_create(EXPR_ADD, $1, $3); }
					 | sixth_expr TOKEN_MINUS fifth_expr { printf("minus\n"); $$ = expr_create(EXPR_SUB, $1, $3); }
					 ;

seventh_expr : sixth_expr { printf("seventh_expr\n"); $$ = $1; }
						 | seventh_expr TOKEN_EQ sixth_expr { printf("eq\n"); $$ = expr_create(EXPR_EQ, $1, $3); }
						 | seventh_expr TOKEN_NEQ sixth_expr { printf("neq"); $$ = expr_create(EXPR_NEQ, $1, $3); }
						 | seventh_expr TOKEN_LT sixth_expr { printf("lt\n"); $$ = expr_create(EXPR_LT, $1, $3); }
						 | seventh_expr TOKEN_LEQ sixth_expr  { printf("leq\n"); $$ = expr_create(EXPR_LEQ, $1, $3); }
						 | seventh_expr TOKEN_GT sixth_expr { printf("gt\n"); $$ = expr_create(EXPR_GT, $1, $3); }
						 | seventh_expr TOKEN_GEQ sixth_expr { printf("geq\n"); $$ = expr_create(EXPR_GEQ, $1, $3); }
						 ;

eighth_expr : seventh_expr { printf("eighth_expr\n"); $$ = $1; }
						| eighth_expr TOKEN_AND seventh_expr { printf("and\n"); $$ = expr_create(EXPR_AND, $1, $3); }
						;

ninth_expr : eighth_expr { printf("ninth_expr\n"); $$ = $1; }
					 | ninth_expr TOKEN_OR eighth_expr { printf("or\n"); $$ = expr_create(EXPR_OR, $1, $3); }
					 ;

expr : ninth_expr { printf("expr\n"); $$ = $1; }
		 | ident TOKEN_ASSIGN ninth_expr { printf("assign\n"); $$ = expr_create(EXPR_ASSIGN, expr_create_name($1), $3); }
		 | ident TOKEN_ASSIGN TOKEN_LEFT_CURLY_BRACKET expr_list TOKEN_RIGHT_CURLY_BRACKET /* {1,2,3} */ { printf("={}\n"); $$ = expr_create(EXPR_ASSIGN, expr_create_name($1), expr_create_curly($4)); }
		 ;

optional_expr : /* empty */ { printf("empty_optional_expr\n"); $$ = nullptr; }
							| expr { printf("optional_expr\n"); $$ = $1; }
							;

expr_list : /* empty */ { printf("empty_expr_list\n"); $$ = nullptr; }
					| nonempty_expr_list { printf("expr_list\n"); $$ = $1; }
					;

nonempty_expr_list : expr TOKEN_COMMA nonempty_expr_list { printf("nonempty_expr_list\n"); $1->next = $3; $$ = $1; }
									 | expr { printf("nonempty_expr\n"); $$ = $1; }
									 ;

%%

int yyerror(char *str) {
	fprintf(stderr, "parse error: %s \n", str);
	return -1;
}
