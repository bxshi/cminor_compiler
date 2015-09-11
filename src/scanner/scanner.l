%{
  #include "token.h"
%}

%{
/* regexpr definition */
%}

COMMENT     (\/\*([^\"\'\*]*(['"][^\'\"]*[\'\"])*)*\*\/)|(\/\/[^\n]*\n)

DELIM       [ \t\n]
WHITESPACE  {DELIM}+

LETTER      [a-zA-Z]
DIGIT       [0-9]

IDENTIFIER  {LETTER}+({DIGIT}|{LETTER})*
NUMBER      {DIGIT}+
LITERAL     \"([^\\\"\n]*(\\[^\"])*(\\\")*)*\"

%%

%{
  /* ordered matches */
%}

%{
  /* first ignore all comment */
%}

{WHITESPACE}  {/*skip*/}
{COMMENT}     {/*skip*/}
{LITERAL}     {token_type=TOKEN_STRING_LITERAL;PROCESS_START}


%{
  /* here comes the keywords */

%}

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

%{
fprintf(stderr, "scan error: wtf is not a valid character\n");
%}

%%

//TODO: put this into

int main(int argc, char **argv) {
  if (argc != 3) return -1;
  if (argc >= 2 && strcmp(argv[1],"-scan") != 0) return -1;

  // load input file
  yyin = fopen(argv[2], "r");

  // start yylex
  while (yylex() != 0) {
    printf("%s ", token_handler(token_type));
    printf("%s\n",yytext);

  }

}
