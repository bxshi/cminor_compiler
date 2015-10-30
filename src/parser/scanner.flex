%{
  #include "token.h"
	#include "parser.tab.h"
%}

%{
/* regexpr definition */
%}

CPP_COMMENT (\/\/[^\n]*\n)
C_COMMENT   (\/\*([^\*]|(\*+[^\/\*]))*\*+\/)
COMMENT     ({C_COMMENT}|{CPP_COMMENT})

DELIM       [ \t\n]
WHITESPACE  {DELIM}+

LETTER      [a-zA-Z]
DIGIT       [0-9]

IDENTIFIER  (({LETTER}|_)({DIGIT}|{LETTER}|_)*)
NUMBER      {DIGIT}+
CHAR        (\'([^\\\']|(\\.))\')
LITERAL     (\"([^\\\"\n]|(\\.))*\")

%%

  /* ordered matches */

  /* first ignore all comment */

{WHITESPACE}  {/*skip*/}
{COMMENT}     {/*skip*/}
{LITERAL}     {token_type=TOKEN_STRING_LITERAL;PROCESS_START}
{CHAR}        {token_type=TOKEN_CHAR_LITERAL;PROCESS_START}

  /* here comes the keywords */

array         {token_type=TOKEN_ARRAY; PROCESS_START}
boolean       {token_type=TOKEN_BOOLEAN; PROCESS_START}
char          {token_type=TOKEN_CHAR;PROCESS_START}
else          {token_type=TOKEN_ELSE;PROCESS_START}
false         {token_type=TOKEN_FALSE;PROCESS_START}
for           {token_type=TOKEN_FOR;PROCESS_START}
function      {token_type=TOKEN_FUNCTION;PROCESS_START}
if            {token_type=TOKEN_IF;PROCESS_START}
integer       {token_type=TOKEN_INTEGER;PROCESS_START}
print         {token_type=TOKEN_PRINT;PROCESS_START}
return        {token_type=TOKEN_RETURN;PROCESS_START}
string        {token_type=TOKEN_STRING;PROCESS_START}
true          {token_type=TOKEN_TRUE;PROCESS_START}
void          {token_type=TOKEN_VOID;PROCESS_START}
while         {token_type=TOKEN_WHILE;PROCESS_START}
"("           {token_type=TOKEN_LEFT_PARENTHESIS;PROCESS_START}
")"           {token_type=TOKEN_RIGHT_PARENTHESIS;PROCESS_START}
"{"           {token_type=TOKEN_LEFT_CURLY_BRACKET;PROCESS_START}
"}"           {token_type=TOKEN_RIGHT_CURLY_BRACKET;PROCESS_START}
"["           {token_type=TOKEN_LEFT_BRACKET;PROCESS_START}
"]"           {token_type=TOKEN_RIGHT_BRACKET;PROCESS_START}
":"           {token_type=TOKEN_COLON;PROCESS_START}
";"           {token_type=TOKEN_SEMI_COLON;PROCESS_START}
"++"          {token_type=TOKEN_INCREMENT;PROCESS_START}
"--"          {token_type=TOKEN_DECREMENT;PROCESS_START}
"-"           {token_type=TOKEN_MINUS;PROCESS_START}
"+"           {token_type=TOKEN_ADD;PROCESS_START}
"^"           {token_type=TOKEN_EXPONENT;PROCESS_START}
"*"           {token_type=TOKEN_MULTIPLY;PROCESS_START}
"/"           {token_type=TOKEN_DIVIDE;PROCESS_START}
"%"           {token_type=TOKEN_MOD;PROCESS_START}
"<"           {token_type=TOKEN_LT;PROCESS_START}
"<="          {token_type=TOKEN_LEQ;PROCESS_START}
">"           {token_type=TOKEN_GT;PROCESS_START}
">="          {token_type=TOKEN_GEQ;PROCESS_START}
"=="          {token_type=TOKEN_EQ;PROCESS_START}
"!="          {token_type=TOKEN_NEQ;PROCESS_START}
"&&"          {token_type=TOKEN_AND;PROCESS_START}
"||"          {token_type=TOKEN_OR;PROCESS_START}
"="           {token_type=TOKEN_ASSIGN;PROCESS_START}
","           {token_type=TOKEN_COMMA;PROCESS_START}

{IDENTIFIER}  {token_type=TOKEN_IDENTIFIER;PROCESS_START}
{NUMBER}      {token_type=TOKEN_INTEGER_LITERAL;PROCESS_START}
.             {token_type=TOKEN_UNDEFINED;PROCESS_START}

%%
/*
int main(int argc, char **argv) {
  if (argc != 3) return -1;
  if (argc >= 2 && strcmp(argv[1],"-scan") != 0) return -1;

  // load input file
  yyin = fopen(argv[2], "r");

  // start yylex
  while (yylex() != 0) {
    // output content if not a keyword

    // deal with length limit
    switch(token_type) {
      case TOKEN_IDENTIFIER:
        if (yyleng > 256) {
          fprintf(stderr, "scan error: %s is not a valid identifier, maximum length of an identifier is 256 but actual is %zu \n", yytext, yyleng);
          exit(1);
        }
        break;
      default:
        break;
    }

    switch(is_keyword(token_type)) {
      case SCANNER_KEYWORD:
        printf("%s\n", token_handler(token_type));
        break;
      case SCANNER_NOT_KEYWORD:

        if (is_literal(token_type)) yyleng = escape(yytext);
        if (token_type == TOKEN_STRING_LITERAL && yyleng > 256) {
          fprintf(stderr, "scan error: %s is not a valid string, maximum length of a string is 256 but actual is %zu \n", yytext, yyleng);
          exit(1);
        }

        printf("%s %s\n",token_handler(token_type), yytext);
        break;
      case SCANNER_ERROR:
        fprintf(stderr, "scan error: %c is not a valid character\n", yytext[0]);
        exit(1);
        break;
    }

  }

  return 0;

}
*/
