#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "type.h"
#include "scope.h"

#define nullptr 0

extern int yyparse();
extern FILE *yyin;
extern int yylex();
extern char* yytext;
extern int token_type;
extern size_t yyleng;

int main(int argc, char **argv) {

	if (argc != 3) return 1;

	if (argc >= 2 && strcmp(argv[1], "-print") == 0) {
		yyin = fopen(argv[2], "r");

		// yyparse
		while(!feof(yyin)) {
			if (yyparse() == 0) {
				
				if (yylval.decl) {
					decl_print(yylval.decl, 0);
				}
				return 0;
	
			} else {
				printf("parse done incorrectly.\n");
				return 1;
			}
		}
	}

	if (argc >= 2 && strcmp(argv[1], "-resolve") == 0) {
		yyin = fopen(argv[2], "r");

		while(!feof(yyin)) {
			if (yyparse() == 0) {
				if (yylval.decl) {
					init();
					decl_resolve(yylval.decl);
				}
				return 0;
			} else {
				printf("parse done incorrectly.\n");
				return 1;
			}
		}
	}

	if (argc >= 2 && strcmp(argv[1], "-typecheck") == 0) {
		yyin = fopen(argv[2], "r");

		while(!feof(yyin)) {
			if (yyparse() == 0) {
				if (yylval.decl) {
				}
				return 0;
			} else {
				printf("parse done incorrectly.\n");
				return 1;
			}
		}
	}

	return 1;
}
