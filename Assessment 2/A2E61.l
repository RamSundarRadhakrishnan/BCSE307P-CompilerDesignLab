%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
char target[256];
int freq=0;
%}
%%
[A-Za-z]+ { if(strcmp(yytext, target)==0) freq++; }
[^A-Za-z]+ ;
%%
int yywrap(){
	return 1;
}

int main(){
	printf("Enter the word to find frequency:\n");
	scanf("%s", target);
	FILE* fptr;
	fptr = fopen("input.txt", "r");
	yyin = fptr;
	yylex();
	printf("Count of given word %s is %d\n", target, freq);
	return 0;
}
