%{
#include <stdio.h>
int yylex(void);
void yyerror(const char* s){ /* quiet reject */ }
%}

%token EOL
%start line

%%

line
  : S EOL            { puts("ACCEPT"); }
  | error EOL        { puts("REJECT"); yyerrok; }
  ;

S
  : 'a' 'b' T
  ;

T
  : 'b' 'b' 'a' 'a' T 'b' 'a'   
  | 'b' 'b' 'a'                 
  ;

%%
int main(void){ return yyparse(); }
