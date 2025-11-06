/* exp15.y: Yacc program to generate three-address code */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(char *);
char* new_temp();
void emit(char *result, char *arg1, char op, char *arg2);

int temp_count = 0;
%}

/* Union to hold the value of tokens */
%union {
    char *str;
}

/* Declare tokens and their types */
%token <str> ID NUM
%type <str> expr stmt

/* Define operator precedence and associativity */
%left '+' '-'
%left '*' '/'

%%
/* Grammar Rules */
program: stmt { printf("\nCode generation successful.\n"); }
       ;

stmt: ID '=' expr ';' { emit($1, $3, '=', NULL); }
    ;

expr: expr '+' expr { $$ = new_temp(); emit($$, $1, '+', $3); }
    | expr '-' expr { $$ = new_temp(); emit($$, $1, '-', $3); }
    | expr '*' expr { $$ = new_temp(); emit($$, $1, '*', $3); }
    | expr '/' expr { $$ = new_temp(); emit($$, $1, '/', $3); }
    | '(' expr ')'  { $$ = $2; }
    | ID            { $$ = $1; }
    | NUM           { $$ = $1; }
    ;

%%

/* Function to generate a new temporary variable */
char* new_temp() {
    char *temp = (char*)malloc(sizeof(char)*4);
    sprintf(temp, "t%d", temp_count++);
    return temp;
}

/* Function to emit (print) a three-address code instruction */
void emit(char *result, char *arg1, char op, char *arg2) {
    if (op == '=') {
        printf("%s = %s\n", result, arg1);
    } else {
        printf("%s = %s %c %s\n", result, arg1, op, arg2);
    }
}

/* Error handling function */
void yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

/* Main function */
int main() {
    printf("Enter the expression (end with a semicolon):\n");
    yyparse();
    return 0;
}