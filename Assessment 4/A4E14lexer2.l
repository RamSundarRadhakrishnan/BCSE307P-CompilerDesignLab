%{
#include "A4E14parser2.tab.h"
#include <stdlib.h>
%}

%%
[ \t\r]+        ;
\n              { return EOL; }
"a"             { return 'a'; }
"b"             { return 'b'; }
.               { return yytext[0]; }
%%
int yywrap(void){ return 1; }
