%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex(void);
void yyerror(char *s);

%}

%union {
	char id;
	int num;
}

%start program
%token EVAL eval LP RP ID const PLUS MINUS MULT SEMICOLON DEFINEFUN fun expr var  GETINT int GETBOOL let LET if IF INT bool BOOL print PRINT DIV MOD TRUE FALSE NOT OR AND EQUAL LEQ GEQ LESS GREATER STAR

%%
program:  LP DEFINEFUN LP fun LP var type RP STAR RP  type expr RP  program
        |  LP eval expr RP 
        |  LP print expr RP 
        ;

type: int
    | BOOL
    ;

term: const
    | var
    |  LP GETINT RP 
    |  LP PLUS term term RP 
    |  LP MULT term term RP 
    |  LP MINUS term term RP 
    |  LP DIV term term RP 
    |  LP MOD term term RP  
    |  LP IF fla term term RP 
    |  LP fun expr STAR RP 
    |  LP LET  LP var expr RP  term RP 
    ;

fla: TRUE
    | FALSE
    | var
    |  LP GETBOOL RP 
    |  LP EQUAL term term RP 
    |  LP LESS term term RP 
    |  LP LEQ term term RP 
    |  LP GREATER term term RP 
    |  LP GEQ term term RP 
    |  LP NOT fla RP 
    |  LP AND fla fla RP 
    |  LP OR fla fla RP 
    |  LP IF fla fla fla RP 
    |  LP fun expr STAR RP 
    |  LP let  LP var expr RP  fla RP 
    ;

%%

/*
    
term: LP PLUS term term extra RP {$$ = $3 + $4;}
    | LP MINUS term term RP {$$ = $3 - $4;}
    | LP MULT term term extra RP{$$ = $3 * $4;}
    | LP DIV term term extra RP{$$ = $3 / $4;}
    | LP MOD term term extra RP{$$ = $3 % $4;}
    | logical
    | NUMBER {$$ = $1;}
    ;

logical: LP OR logical logical RP{$$ = $3 || $4;}
       | LP AND logical logical RP{$$ = $3 && $4;}
       | TRUE {$$ = 0;}
       | FALSE {$$ = 1;}
       ;

*/

void main()
{  
    yyparse();
}
void yyerror(char *s)
 {
    printf("%s\n", s);
 }