%{
#include <stdio.h>
int yylex(void);
void yyerror(const char* s){ fprintf(stderr,"%s\n", s); }
static int ipow(int a,int b){ int r=1; while(b>0){ if(b&1) r*=a; a*=a; b>>=1; } return r; }
%}

%union { int i; }
%token <i> NUM
%token EOL
%type  <i> expr
%start line

%left  '+' '-'
%left  '*' '/' '%'
%right '^'
%right UMINUS

%%

line
  : expr EOL             { printf("%d\n", $1); }
  ;

expr
  : expr '+' expr        { $$ = $1 + $3; }
  | expr '-' expr        { $$ = $1 - $3; }
  | expr '*' expr        { $$ = $1 * $3; }
  | expr '/' expr        { $$ = $3==0 ? 0 : $1 / $3; }
  | expr '%' expr        { $$ = $3==0 ? 0 : $1 % $3; }
  | expr '^' expr        { $$ = ipow($1, $3); }
  | '-' expr %prec UMINUS{ $$ = -$2; }
  | '(' expr ')'         { $$ = $2; }
  | NUM                  { $$ = $1; }
  ;

%%

int main(void){ return yyparse(); }
