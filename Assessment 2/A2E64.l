%{
#include <stdio.h>
#include <stdlib.h>
FILE* fileout;
%}

KEYWORD (int|float|char|double|if|else|while|for|switch|do|return|break|continue)
IDENTIFIER [A-Za-z_][A-Za-z0-9_]*
NUMBERS [0-9]+
ARITHMETIC (\+|-|\*|\/|%|\+\+|--)
RELATIONAL (<|>|<=|>=|==|!=)
LOGICAL (&&|\|\|)
DELIMITERS [(){}\[\],;]
ASSIGNMENT =

%%
{KEYWORD} {
	fprintf(fileout, "Keyword: %s\n", yytext);
}
{IDENTIFIER} {
	fprintf(fileout, "Identifier: %s\n", yytext);
}
{ARITHMETIC} {
	fprintf(fileout, "Arithmetic Operator: %s\n", yytext);
}
{RELATIONAL} {
	fprintf(fileout, "Relational Operator: %s\n", yytext);
}
{LOGICAL} {
	fprintf(fileout, "Logical Operator: %s\n", yytext);
}
{NUMBERS} {
	fprintf(fileout, "Number: %s\n", yytext);
}
{DELIMITERS} {
	fprintf(fileout, "Delimiter: %s\n", yytext);
}
{ASSIGNMENT} {
	fprintf(fileout, "Assignment Operator: %s\n", yytext);
}
%%

int yywrap(){
	return 1;
}

int main(){
	FILE* fptr;
	fptr = fopen("code.txt", "r");
	fileout = fopen("lexed.txt", "w");
	yyin = fptr;
	yylex();
	fclose(fptr);
	fclose(fileout);
	return 0;
}


