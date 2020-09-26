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

%start begin
%token EVAL LP RP ID PLUS MINUS MULT SEMICOLON DEFINEFUN GETINT GETBOOL LET IF INT BOOL PRINT DIV MOD TRUE FALSE NOT OR AND EQUAL LEQ GEQ LESS GREATER
%token<num> NUMBER

%type<num> term
%type<num> exp
%type<num> extra
%type<num> logical

%%
begin: exp 
     ;

exp: LP EVAL term RP {printf("%d\n", $3);}
   ;
    
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

extra: /* epsilon */
     | term
     ;
    

%%

int main(void)
{
    return yyparse();
}

void yyerror(char *s)
{
    printf("%s\n", s);
}
