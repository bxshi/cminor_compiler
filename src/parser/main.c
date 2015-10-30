#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"
#include "token.h"

#define nullptr 0

extern int yyparse();
extern FILE *yyin;
extern int yylex();
extern char* yytext;
extern int token_type;
extern size_t yyleng;

int main(int argc, char **argv) {

	if (argc != 3) return 1;
	if (argc >= 2 && strcmp(argv[1], "-parse") != 0) return 1;

	yyin = fopen(argv[2], "r");


// start yylex
  while (yylex() != 0) {
    // output content if not a keyword

    // deal with length limit
    switch(token_type) {
      case TOKEN_IDENTIFIER:
        if (yyleng > 256) {
          fprintf(stderr, "scan error: %s is not a valid identifier, maximum length of an identifier is 256 but actual is %zu \n", yytext, yyleng);
          return 1;
        }
        break;
      default:
        break;
    }

    switch(is_keyword(token_type)) {
      case SCANNER_KEYWORD:
        // printf("%s\n", token_handler(token_type));
        break;
      case SCANNER_NOT_KEYWORD:

        if (is_literal(token_type)) yyleng = escape(yytext);
        if (token_type == TOKEN_STRING_LITERAL && yyleng > 256) {
          fprintf(stderr, "scan error: %s is not a valid string, maximum length of a string is 256 but actual is %zu \n", yytext, yyleng);
          return 1;
        }

        // printf("%s %s\n",token_handler(token_type), yytext);
        break;
      case SCANNER_ERROR:
        fprintf(stderr, "scan error: %c is not a valid character\n", yytext[0]);
        return 1;
        break;
    }

  }


	if (yyparse() == 0) {
		printf("parse done correctly\n");
		return 0;
	} else {
		printf("parse doen incorrectly.\n");
		return 1;
	}
	
}
