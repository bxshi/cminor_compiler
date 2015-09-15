/*
  Created by Baoxu Shi on 9/10/15.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

/**
 *  return string representation of a token
 * @token_t tk input token type
 */
const char* token_handler(token_t tk) {
    switch (tk) {
    case TOKEN_ARRAY:
        return "ARRAY";
        break;
    case TOKEN_BOOLEAN:
        return"BOOLEAN";
        break;
    case TOKEN_CHAR:
        return"CHAR";
        break;
    case TOKEN_ELSE:
        return"ELSE";
        break;
    case TOKEN_FALSE:
        return"FALSE";
        break;
    case TOKEN_FOR:
        return"FOR";
        break;
    case TOKEN_FUNCTION:
        return"FUNCTION";
        break;
    case TOKEN_IF:
        return"IF";
        break;
    case TOKEN_INTEGER:
        return"INTEGER";
        break;
    case TOKEN_PRINT:
        return"PRINT";
        break;
    case TOKEN_RETURN:
        return"RETURN";
        break;
    case TOKEN_STRING:
        return"STRING";
        break;
    case TOKEN_TRUE:
        return"TRUE";
        break;
    case TOKEN_VOID:
        return"VOID";
        break;
    case TOKEN_WHILE:
        return"WHILE";
        break;
    case TOKEN_LEFT_PARENTHESIS:
        return"LEFT_PARENTHESIS";
        break;
    case TOKEN_RIGHT_PARENTHESIS:
        return"RIGHT_PARENTHESIS";
        break;
    case TOKEN_COLON:
        return"COLON";
        break;
    case TOKEN_SEMI_COLON:
        return"SEMICOLON";
        break;
    case TOKEN_LEFT_BRACKET:
        return"LEFT_BRACKET";
        break;
    case TOKEN_RIGHT_BRACKET:
        return"RIGHT_BRACKET";
        break;
    case TOKEN_LEFT_CURLY_BRACKET:
        return"LEFT_CURLY_BRACKET";
        break;
    case TOKEN_RIGHT_CURLY_BRACKET:
        return"RIGHT_CURLY_BRACKET";
        break;
    case TOKEN_INCREMENT:
        return"INCREMENT";
        break;
    case TOKEN_DECREMENT:
        return"DECREMENT";
        break;
    case TOKEN_MINUS:
        return"MINUS";
        break;
    case TOKEN_EXPONENT:
        return"EXPONENT";
        break;
    case TOKEN_DIVIDE:
        return"DIVIDE";
        break;
    case TOKEN_MOD:
        return"MOD";
        break;
    case TOKEN_ADD:
        return"ADD";
        break;
    case TOKEN_LT:
        return"LT";
        break;
    case TOKEN_LEQ:
        return"LEQ";
        break;
    case TOKEN_GT:
        return"GT";
        break;
    case TOKEN_GEQ:
        return"GEQ";
        break;
    case TOKEN_EQ:
        return"EQ";
        break;
    case TOKEN_NEQ:
        return"NEQ";
        break;
    case TOKEN_AND:
        return"AND";
        break;
    case TOKEN_OR:
        return"OR";
        break;
    case TOKEN_ASSIGN:
        return"ASSIGN";
        break;
    case TOKEN_STRING_LITERAL:
        return"STRING_LITERAL";
        break;
    case TOKEN_INTEGER_LITERAL:
        return "INTEGER_LITERAL";
    case TOKEN_CHAR_LITERAL:
        return "CHAR_LITERAL";
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_COMMA:
        return "COMMA";
    default:
        return"Unsupported command!";
    }
}

char is_keyword(token_t tk) {
    switch (tk) {
    case TOKEN_ARRAY:
    case TOKEN_BOOLEAN:
    case TOKEN_CHAR:
    case TOKEN_ELSE:
    case TOKEN_FALSE:
    case TOKEN_FOR:
    case TOKEN_FUNCTION:
    case TOKEN_IF:
    case TOKEN_INTEGER:
    case TOKEN_PRINT:
    case TOKEN_RETURN:
    case TOKEN_STRING:
    case TOKEN_TRUE:
    case TOKEN_VOID:
    case TOKEN_WHILE:
    case TOKEN_LEFT_PARENTHESIS:
    case TOKEN_RIGHT_PARENTHESIS:
    case TOKEN_COLON:
    case TOKEN_SEMI_COLON:
    case TOKEN_LEFT_BRACKET:
    case TOKEN_RIGHT_BRACKET:
    case TOKEN_LEFT_CURLY_BRACKET:
    case TOKEN_RIGHT_CURLY_BRACKET:
    case TOKEN_INCREMENT:
    case TOKEN_DECREMENT:
    case TOKEN_MINUS:
    case TOKEN_EXPONENT:
    case TOKEN_DIVIDE:
    case TOKEN_MOD:
    case TOKEN_ADD:
    case TOKEN_LT:
    case TOKEN_LEQ:
    case TOKEN_GT:
    case TOKEN_GEQ:
    case TOKEN_EQ:
    case TOKEN_NEQ:
    case TOKEN_AND:
    case TOKEN_OR:
    case TOKEN_ASSIGN:
    case TOKEN_IDENTIFIER:
    case TOKEN_COMMA:
    case TOKEN_INTEGER_LITERAL:
        return SCANNER_KEYWORD;
    case TOKEN_STRING_LITERAL:
    case TOKEN_CHAR_LITERAL:
        return SCANNER_NOT_KEYWORD;
    default:
        return SCANNER_ERROR;
    }
}

char is_literal(token_t tk) {
  return tk == TOKEN_STRING_LITERAL || tk == TOKEN_CHAR_LITERAL;
}

int escape(char *str) {
  int opos = 1; // position in the original string, skip the first "
  int tpos = 0; // position in the target string

  while(str[opos + 1] != '\0') { // the string looks like "\"\0\""
    if(str[opos] == '\\' && str[opos + 1] != '\0') {
      ++opos; // move to the escaped character
      switch(str[opos]) {
        case 'a':
          str[tpos] = '\a';
          break;
        case 'b':
          str[tpos] = '\b';
          break;
        case 'f':
          str[tpos] = '\f';
          break;
        case 'n':
          str[tpos] = '\n';
          break;
        case 'r':
          str[tpos] = '\r';
          break;
        case 't':
          str[tpos] = '\t';
          break;
        case 'v':
          str[tpos] = '\v';
          break;
        case '\\':
          str[tpos] = '\\';
          break;
        case '\'':
          str[tpos] = '\'';
          break;
        case '\"':
          str[tpos] = '\"';
          break;
        case '?':
          str[tpos] = '\?';
          break;
        default:
          str[tpos] = '\\';
          str[++tpos] = str[opos];
      }
    } else {
      str[tpos] = str[opos];
    }
    ++opos; // move to next char
    ++tpos; // move to next position
  }
  str[tpos] = '\0';
  return tpos + 1;
}
