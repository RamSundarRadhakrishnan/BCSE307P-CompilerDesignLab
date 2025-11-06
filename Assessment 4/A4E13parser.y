%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int yylex(void);
void yyerror(const char* s){ fprintf(stderr,"%s\n",s); }

typedef struct Node{
  char* op;
  struct Node* l;
  struct Node* r;
} Node;

static Node* leaf(char* s){ Node* n=malloc(sizeof(Node)); n->op=s; n->l=n->r=NULL; return n; }
static Node* node(char* s,Node* a,Node* b){ Node* n=malloc(sizeof(Node)); n->op=strdup(s); n->l=a; n->r=b; return n; }
static void post(Node* n){ if(!n) return; post(n->l); post(n->r); printf("%s ", n->op); }
static void freet(Node* n){ if(!n) return; freet(n->l); freet(n->r); if(n->l||n->r) free(n->op); else free(n->op); free(n); }
%}

%union { char* s; struct Node* n; }
%token <s> NUM ID
%token EOL
%type  <n> expr

%start line
%left  '+' '-'
%left  '*' '/' '%'
%right '^'
%right UMINUS

%%

line
  : expr EOL        { post($1); printf("\n"); freet($1); }
  ;

expr
  : expr '+' expr   { $$ = node("+",$1,$3); }
  | expr '-' expr   { $$ = node("-",$1,$3); }
  | expr '*' expr   { $$ = node("*",$1,$3); }
  | expr '/' expr   { $$ = node("/",$1,$3); }
  | expr '%' expr   { $$ = node("%",$1,$3); }
  | expr '^' expr   { $$ = node("^",$1,$3); }
  | '-' expr %prec UMINUS { $$ = node("~",leaf(strdup("0")),$2); }
  | '(' expr ')'    { $$ = $2; }
  | NUM             { $$ = leaf($1); }
  | ID              { $$ = leaf($1); }
  ;

%%

int main(void){ return yyparse(); }
