%{
#include <stdio.h>
int yylex(void);
void yyerror(const char* s){ /* quiet */ }

static int ca=0, cb=0;
%}

%token EOL
%start line

%%

line
  : S EOL            { puts((ca+cb>0 && ca!=cb) ? "ACCEPT" : "REJECT"); ca=cb=0; }
  | error EOL        { puts("REJECT"); ca=cb=0; yyerrok; }
  ;


S : A B ;

A : /* empty */
  | A 'a'            { ca++; }
  ;

B : /* empty */
  | B 'b'            { cb++; }
  ;

%%

int main(void){ return yyparse(); }