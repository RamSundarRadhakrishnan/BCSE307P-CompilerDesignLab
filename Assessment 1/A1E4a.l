%{
%}

%s A B

%%
<INITIAL>a BEGIN A;
<INITIAL>\n BEGIN INITIAL; printf("NOT ACCEPTED");
<A>a BEGIN INITIAL;
<A>b BEGIN B;
<A>\n BEGIN INITIAL; printf("ACCEPTED");
<B>b BEGIN A;
<B>\n BEGIN INITIAL; printf("NOT ACCEPTED");
%%

int yywrap(){
	return 1;
}

int main(){
	printf("Enter the string to validate:\n ");
	yylex();
	return 0;
}
