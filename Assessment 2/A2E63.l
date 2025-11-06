%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char longest[256];
int lengthOfLongest = 0;
%}

%%
[A-Za-z]+ {
	if(strlen(yytext)>lengthOfLongest){
		strcpy(longest, yytext);
		lengthOfLongest = strlen(yytext);
	}
}
[^A-Za-z]+ ;
%%

int yywrap(){
	return 1;
}

int main(){
	FILE* fptr;
	fptr = fopen("input.txt", "r");
	yyin = fptr;
	yylex();
	printf("The longest word in the file is %s with length %d\n", longest, lengthOfLongest);
	fclose(fptr);
	return 0;
}
