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
{LITERAL}     {token_type=TOKEN_STRING_LITERAL; yylval.str = yytext; return TOKEN_STRING_LITERAL;}
{CHAR}        {token_type=TOKEN_CHAR_LITERAL; yylval.ival = (int)(*yytext); return TOKEN_CHAR_LITERAL;}

  /* here comes the keywords */

array         {token_type=TOKEN_ARRAY; return TOKEN_ARRAY;}
boolean       {token_type=TOKEN_BOOLEAN; return TOKEN_BOOLEAN;}
char          {token_type=TOKEN_CHAR; return TOKEN_CHAR;}
else          {token_type=TOKEN_ELSE; return TOKEN_ELSE;}
false         {token_type=TOKEN_FALSE; return TOKEN_FALSE;}
for           {token_type=TOKEN_FOR; return TOKEN_FOR;}
function      {token_type=TOKEN_FUNCTION; return TOKEN_FUNCTION;}
if            {token_type=TOKEN_IF; return TOKEN_IF;}
integer       {token_type=TOKEN_INTEGER; return TOKEN_INTEGER;}
print         {token_type=TOKEN_PRINT; return TOKEN_PRINT;}
return        {token_type=TOKEN_RETURN; return TOKEN_RETURN;}
string        {token_type=TOKEN_STRING; return TOKEN_STRING;}
true          {token_type=TOKEN_TRUE; return TOKEN_TRUE;}
void          {token_type=TOKEN_VOID; return TOKEN_VOID;}
while         {token_type=TOKEN_WHILE; return TOKEN_WHILE;}
"("           {token_type=TOKEN_LEFT_PARENTHESIS; return TOKEN_LEFT_PARENTHESIS;}
")"           {token_type=TOKEN_RIGHT_PARENTHESIS; return TOKEN_RIGHT_PARENTHESIS;}
"{"           {token_type=TOKEN_LEFT_CURLY_BRACKET; return TOKEN_LEFT_CURLY_BRACKET;}
"}"           {token_type=TOKEN_RIGHT_CURLY_BRACKET; return TOKEN_RIGHT_CURLY_BRACKET;}
"["           {token_type=TOKEN_LEFT_BRACKET; return TOKEN_LEFT_BRACKET;}
"]"           {token_type=TOKEN_RIGHT_BRACKET; return TOKEN_RIGHT_BRACKET;}
":"           {token_type=TOKEN_COLON; return TOKEN_COLON;}
";"           {token_type=TOKEN_SEMI_COLON; return TOKEN_SEMI_COLON;}
"++"          {token_type=TOKEN_INCREMENT; return TOKEN_INCREMENT;}
"--"          {token_type=TOKEN_DECREMENT; return TOKEN_DECREMENT;}
"-"           {token_type=TOKEN_MINUS; return TOKEN_MINUS;}
"+"           {token_type=TOKEN_ADD; return TOKEN_ADD;}
"^"           {token_type=TOKEN_EXPONENT; return TOKEN_EXPONENT;}
"*"           {token_type=TOKEN_MULTIPLY; return TOKEN_MULTIPLY;}
"/"           {token_type=TOKEN_DIVIDE; return TOKEN_DIVIDE;}
"%"           {token_type=TOKEN_MOD; return TOKEN_MOD;}
"<"           {token_type=TOKEN_LT; return TOKEN_LT;}
"<="          {token_type=TOKEN_LEQ; return TOKEN_LEQ;}
">"           {token_type=TOKEN_GT; return TOKEN_GT;}
">="          {token_type=TOKEN_GEQ; return TOKEN_GEQ;}
"=="          {token_type=TOKEN_EQ; return TOKEN_EQ;}
"!="          {token_type=TOKEN_NEQ; return TOKEN_NEQ;}
"&&"          {token_type=TOKEN_AND; return TOKEN_AND;}
"||"          {token_type=TOKEN_OR; return TOKEN_OR;}
"="           {token_type=TOKEN_ASSIGN; return TOKEN_ASSIGN;}
","           {token_type=TOKEN_COMMA; return TOKEN_COMMA;}
"!"						{token_type=TOKEN_NOT; return TOKEN_NOT;}

{IDENTIFIER}  {token_type=TOKEN_IDENTIFIER; yylval.str = yytext; return TOKEN_IDENTIFIER;}
{NUMBER}      {token_type=TOKEN_INTEGER_LITERAL; yylval.ival = atoi(yytext); return TOKEN_INTEGER_LITERAL;}
.             {token_type=TOKEN_UNDEFINED; return TOKEN_UNDEFINED;}

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
