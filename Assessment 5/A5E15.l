%{
#include "A5E15yacc.tab.h"
#include <stdlib.h>
#include <string.h>

void yyerror(char *);
%}

%%
[a-zA-Z_][a-zA-Z0-9_]* { 
                            yylval.str = strdup(yytext); 
                            return ID; 
                        }
[0-9]+                  { 
                            yylval.str = strdup(yytext); 
                            return NUM; 
                        }
[ \t\n]+                ; /* Ignore whitespace */
.                       { return yytext[0]; }
%%

int yywrap(void) {
    return 1;
}