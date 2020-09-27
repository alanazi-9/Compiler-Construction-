%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex(void);
void yyerror(char *s);
void set_ID_value(char id, int value);
int get_ID_value(char id);

int *symTable;

int additionalPlus;
int additionalMult;
int additionalOR;
int additionalAND;

int evalFLAG;
int err;
%}

%union {
	char id;
	int num;
}

%start program
%token EVAL LP RP PLUS MINUS MULT SEMICOLON DEFINEFUN VAR GETINT GETBOOL LET IF INT BOOL PRINT DIV MOD TRUE FALSE NOT OR AND EQUAL LEQ GEQ LESS GREATER 
%token<num> NUMBER 
%token<num> ID

%type<num> term
%type<num> expr
%type<num> program
%type<num> fla
%type<num> TRUE
%type<num>FALSE

%%
program:  LP DEFINEFUN LP ID optional1 RP type expr RP program {set_ID_value(yyval.id, $8);}
        |  LP eval expr RP { if(err == 0) printf("OUTPUT : %d\n", $3); evalFLAG = 0;}
        |  LP PRINT expr RP 
        ;

eval: EVAL {evalFLAG = 1;} 
    ;
optional1: /* epsilon */ 
        | optional1
        | LP ID type RP
        ;

type: INT
    | BOOL
    ;

expr: term
    | fla
    ;

term: NUMBER {$$ = $1;}
    | ID {if(evalFLAG == 1) {err = 1; yyerror("syntax error : variables cannot be declared inside eval\n");} $$ = get_ID_value(yyval.id);}
    | LP GETINT RP {if(evalFLAG == 1) {err = 1; yyerror("syntax error : get-int cannot be declared inside eval\n");}}
    | LP PLUS term term extraPlus RP {$$ = $3 + $4 + additionalPlus; additionalPlus = 0;}
    | LP MULT term term extraMult RP {$$ = $3 * $4 * additionalMult; additionalMult = 1;}
    | LP MINUS term term RP {$$ = $3 - $4;}
    | LP DIV term term RP {$$ = $3 / $4;}
    | LP MOD term term RP  {$$ = $3 % $4;}
    | LP IF fla term term RP {if($3 == 1) $$ = $4; else $$ = $5;}
    | LP ID optional2 RP {if(evalFLAG == 1) { err = 1; yyerror("syntax error : functions cannot called inside eval\n");}}
    | LP LET LP ID expr RP term RP {if(evalFLAG == 1) {err = 1; yyerror("syntax error : let cannot be declared inside eval\n");}}
    ;
optional2: /* epsilon */ 
         | expr
         ;

extraPlus: 
     |term extraPlus {additionalPlus += $1;} 
     ;

extraMult: /* epsilon */ 
     |term extraMult {additionalMult *= $1;} 
     ;

fla: TRUE {$$ = 1;}
    | FALSE {$$ = 0;}
    | ID {if(evalFLAG == 1) {err = 1; yyerror("syntax error : variables cannot be declared inside eval\n");} $$ = get_ID_value(yyval.id);}
    | LP GETBOOL RP {if(evalFLAG == 1) {err = 1; yyerror("syntax error : get-bool cannot be declared inside eval\n");}}
    | LP EQUAL term term RP {if($3 == $4) $$ = 1; else $$ = 0;}
    | LP LESS term term RP {if($3 < $4) $$ = 1; else $$ = 0;}
    | LP LEQ term term RP {if($3 <= $4) $$ = 1; else $$ = 0;}
    | LP GREATER term term RP {if($3 > $4) $$ = 1; else $$ = 0;}
    | LP GEQ term term RP {if($3 >= $4) $$ = 1; else $$ = 0;}
    | LP NOT fla RP {if($3 == 1) $$ = 0; else $$ = 1;}
    | LP AND fla fla extraAND RP {$$ = $3 && $4 && additionalAND;}
    | LP OR fla fla extraOR RP {$$ = $3 || $4 || additionalOR; additionalOR = 0;}
    | LP IF fla fla fla RP {if($3 == 1) $$ = $4; else $$ = $5;}
    | LP ID optional3 RP {if(evalFLAG == 1) { err = 1; yyerror("syntax error : functions cannot called inside eval\n");} $$ = get_ID_value(yyval.id);}
    | LP LET LP ID expr RP fla RP {if(evalFLAG == 1) {err = 1; yyerror("syntax error : let cannot be declared inside eval\n");}}
    ;

optional3: /* epsilon */ 
         | expr
         ;
         
extraAND: /* epsilon */ 
             | fla extraAND {additionalAND *= $1;}
             ;

extraOR: /* epsilon */
            | fla extraOR {if($1 == 1) additionalOR = 1;}
            
%%

void main()
{  
    additionalPlus = 0;
    additionalMult = 1;
    additionalOR = 0;
    additionalAND = 1;
    evalFLAG = 0;
    err = 0;

    symTable = calloc(26, sizeof(int));
    yyparse();
    free(symTable);
}
void yyerror(char *s)
{
    printf("%s\n", s);
}

void set_ID_value(char id, int value)
{
    symTable[id - 'a'] = value;
}

int get_ID_value(char id) 
{
	return symTable[id - 'a'];
}
