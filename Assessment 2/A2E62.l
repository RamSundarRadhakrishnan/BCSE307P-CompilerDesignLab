%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char target[256];
char newword[256];
FILE* fileout;
%}

%%
[A-Za-z]+ {
	if(strcmp(yytext, target)==0){
		fputs(newword, fileout);
	}
	else{ fputs(yytext, fileout); }
}
[^A-Za-z]+ { fputs(yytext, fileout); }
%%

int yywrap(){
	return 1;
}

int main(){
	printf("Enter the word to be replaced:\n");
	scanf("%s", target);
	printf("Enter the word to be inserted:\n");
	scanf("%s", newword);
	FILE* fptr;
	fptr = fopen("input.txt", "r");
	yyin = fptr;
	fileout = fopen("output.txt", "w");
	yylex();
	fclose(fptr);
	fclose(fileout);
	return 0;
}
	
