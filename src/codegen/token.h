/*
  Created by Baoxu Shi on 9/10/15.
*/

#include "parser.tab.h"

#ifndef CMINOR_COMPILER_TOKEN_H
#define CMINOR_COMPILER_TOKEN_H

#define PROCESS_START return(1);
#define SCANNER_KEYWORD 1
#define SCANNER_NOT_KEYWORD 0
#define SCANNER_ERROR 3

/**
 * return name string of a token
 */
const char* token_handler(int tk);

char is_literal(int tk);

/* check if `tk` is a keyword */
char is_keyword(int tk);

int escape(char *str);

int token_type;

#endif /*CMINOR_COMPILER_TOKEN_H*/
