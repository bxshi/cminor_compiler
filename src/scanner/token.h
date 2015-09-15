/*
  Created by Baoxu Shi on 9/10/15.
*/

#ifndef CMINOR_COMPILER_TOKEN_H
#define CMINOR_COMPILER_TOKEN_H

#define PROCESS_START return(1);
#define SCANNER_KEYWORD 1
#define SCANNER_NOT_KEYWORD 0
#define SCANNER_ERROR 3

enum token {
    TOKEN_ARRAY, /* array */
    TOKEN_BOOLEAN, /* boolean */
    TOKEN_CHAR, /* char */
    TOKEN_ELSE, /* else */
    TOKEN_FALSE, /* false */
    TOKEN_FOR, /* for */
    TOKEN_FUNCTION, /* function */
    TOKEN_IF, /* if */
    TOKEN_INTEGER, /* integer */
    TOKEN_PRINT, /* print */
    TOKEN_RETURN, /* return */
    TOKEN_STRING, /* string */
    TOKEN_TRUE, /* true */
    TOKEN_VOID, /* void */
    TOKEN_WHILE, /* while */

    TOKEN_LEFT_PARENTHESIS, /* ( */
    TOKEN_RIGHT_PARENTHESIS, /* ) */
    TOKEN_COLON, /* : */
    TOKEN_SEMI_COLON, /* ; */
    TOKEN_LEFT_BRACKET, /* [ */
    TOKEN_RIGHT_BRACKET, /* ] */
    TOKEN_LEFT_CURLY_BRACKET, /* { */
    TOKEN_RIGHT_CURLY_BRACKET, /* } */

    TOKEN_INCREMENT, /* ++ */
    TOKEN_DECREMENT, /* -- */
    TOKEN_MINUS, /* - */
    TOKEN_EXPONENT, /* ^ */
    TOKEN_MULTIPLY, /* * */
    TOKEN_DIVIDE, /* / */
    TOKEN_MOD, /* % */
    TOKEN_ADD, /* + */
    TOKEN_LT, /* < */
    TOKEN_LEQ, /* <= */
    TOKEN_GT, /* > */
    TOKEN_GEQ, /* >= */
    TOKEN_EQ, /* == */
    TOKEN_NEQ, /* != */
    TOKEN_AND, /* && */
    TOKEN_OR, /* || */
    TOKEN_ASSIGN, /* = */
    TOKEN_COMMA, /* , */

    TOKEN_STRING_LITERAL, /* strings */
    TOKEN_INTEGER_LITERAL, /* integers */
    TOKEN_CHAR_LITERAL, /* char */
    TOKEN_IDENTIFIER,

    TOKEN_UNDEFINED
};

typedef enum token token_t;

/**
 * return name string of a token
 */
const char* token_handler(token_t tk);

char is_literal(token_t tk);

/* check if `tk` is a keyword */
char is_keyword(token_t tk);

void escape(char *str);

token_t token_type;

#endif /*CMINOR_COMPILER_TOKEN_H*/
