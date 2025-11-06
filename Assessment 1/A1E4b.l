%{
%}

%s A B C 

%%
<INITIAL>a BEGIN INITIAL;
<INITIAL>b BEGIN A;
<INITIAL>c BEGIN INITIAL;
<A>a BEGIN INITIAL;
<A>b BEGIN A;
<A>c BEGIN B;
<B>a BEGIN C;
<B>b BEGIN A;
<B>c BEGIN INITIAL;
<C>a BEGIN INITIAL; {printf("ACCEPTED");}
<C>b BEGIN INITIAL; {printf("ACCEPTED");}
<C>c BEGIN INITIAL; {printf("ACCEPTED");}
<C>\n BEGIN INITIAL; {printf("ACCEPTED");}
%%

int yywrap(){
	return 1;
}

int main(){
	printf("Enter the string to validate:\n");
	yylex();
	return 0;
}
