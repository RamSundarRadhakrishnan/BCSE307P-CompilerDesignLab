%{
#include "A4E12parser.tab.h"
#include <stdlib.h>
%}

%%
[ \t\r]+        ;
\n              { return EOL; }
[0-9]+          { yylval.i = atoi(yytext); return NUM; }
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
