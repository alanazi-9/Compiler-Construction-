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
%token EVAL LP RP ID PLUS MINUS MULT SEMICOLON DEFINEFUN VAR GETINT GETBOOL LET IF INT BOOL PRINT DIV MOD TRUE FALSE NOT OR AND EQUAL LEQ GEQ LESS GREATER FUN
%token<num> NUMBER




%type<num> term
%type<num> expr
%type<num> program
%type<num> fla
%type<num> TRUE
%type<num>FALSE

%%
program:  LP DEFINEFUN LP FUN LP ID type RP RP type expr RP program
        |  LP EVAL expr RP {printf("%d", $3);}
        |  LP PRINT expr RP 
        ;

type: INT
    | BOOL
    ;

expr: term
    | fla
    ;

term: NUMBER {$$ = $1;}
    | ID
    |  LP GETINT RP 
    |  LP PLUS term term RP {$$ = $3 + $4;}
    |  LP MULT term term RP {$$ = $3 * $4;}
    |  LP MINUS term term RP {$$ = $3 - $4;}
    |  LP DIV term term RP {$$ = $3 / $4;}
    |  LP MOD term term RP  {$$ = $3 % $4;}
    |  LP IF fla term term RP 
    |  LP ID expr RP 
    |  LP LET  LP ID expr RP  term RP 
    ;

fla: TRUE {$$ = 0;}
    | FALSE {$$ = 1;}
    | ID
    |  LP GETBOOL RP 
    |  LP EQUAL term term RP {if($3 == $4) $$ = 0; else $$ = 1;}
    |  LP LESS term term RP {if($3 < $4) $$ = 0; else $$ = 1;}
    |  LP LEQ term term RP {if($3 <= $4) $$ = 0; else $$ = 1;}
    |  LP GREATER term term RP {if($3 > $4) $$ = 0; else $$ = 1;}
    |  LP GEQ term term RP {if($3 >= $4) $$ = 0; else $$ = 1;}
    |  LP NOT fla RP {if($3 == 0) $$ = 1; else $$ = 0;}
    |  LP AND fla fla RP {$$ = $3 && $4;}
    |  LP OR fla fla RP {$$ = $3 || $4;}
    |  LP IF fla fla fla RP {if($3 == 0 && $4 == 0 && $5 == 0) $$ = 0; else $$ = 1;}
    |  LP ID expr RP 
    |  LP LET  LP ID expr RP  fla RP 
    ;

%%

void main()
{  
    yyparse();
}
void yyerror(char *s)
 {
    printf("%s\n", s);
 }
