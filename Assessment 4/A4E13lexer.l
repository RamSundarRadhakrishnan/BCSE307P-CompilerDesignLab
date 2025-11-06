%{
#include "A4E13parser.tab.h"
#include <stdlib.h>
#include <string.h>
%}

%%
[ \t\r]+        ;
\n              { return EOL; }
[0-9]+          { yylval.s = strdup(yytext); return NUM; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.s = strdup(yytext); return ID; }
"^"             { return '^'; }
"%"             { return '%'; }
"*"             { return '*'; }
"/"             { return '/'; }
"+"             { return '+'; }
"-"             { return '-'; }
"("             { return '('; }
")"             { return ')'; }
.               { return yytext[0]; }
%%
int yywrap(void){ return 1; }
