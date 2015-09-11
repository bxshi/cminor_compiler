/*
  Created by Baoxu Shi on 9/10/15.
*/
#include <stdio.h>
#include <stdlib.h>
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
    default:
        return"Unsupported command!";
    }
  }
