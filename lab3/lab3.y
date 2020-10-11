%{
#include <stdio.h>
#include <stdlib.h>
#include "containers.h"

#define MAX 50

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
	char* str;
	int num;
}

%start program
%token ID NUMBER EVAL LP RP PLUS MINUS MULT DEFINEFUN VAR GETINT GETBOOL LET IF PRINT DIV MOD TRUE FALSE NOT OR AND EQUAL LEQ GEQ LESS GREATER COMMENT FUNID VARID CALL
%token<str> ID NUMBER 
%token<num> INT BOOL 
%type<num> varid funid expr type EVAL LP RP PLUS MINUS MULT DEFINEFUN VAR GETINT GETBOOL LET IF PRINT DIV MOD TRUE FALSE NOT OR AND EQUAL LEQ GEQ LESS GREATER COMMENT FUNID VARID CALL
%%
program:  LP DEFINEFUN LP funid optional1 RP type expr RP program {
                insert_children(3, $4, $7, $8);
                insert_node("Define-fun", DEFINEFUN);
        }
        |  LP eval expr RP { 
                if(err == 0) printf("OUTPUT : %d\n", $3); 
                evalFLAG = 0; 
                insert_child($3);
                insert_node("EVAL", EVAL);
            }
        |  LP PRINT expr RP {
                insert_child($3);
                insert_node("Print", PRINT);
        }
        ;

funid: ID {$$ = insert_node($1, FUNID);}
    ;
eval: EVAL {evalFLAG = 1;} 
    ;
optional1: /* epsilon */ 
        | LP varid argtype RP optional1 { }
        ;

argtype: INT
       | BOOL
       ;

varid: ID {$$ = insert_node($1, VARID);}
     ;
type: INT {$$ = insert_node("INT", INT);}
    | BOOL {$$ = insert_node("BOOL", BOOL);}
    ;

expr: NUMBER {$$ = insert_node($1, NUMBER);}
    | ID {
            if(evalFLAG == 1) {
                err = 1; yyerror("syntax error : variables cannot be declared inside eval\n");
                } 
            $$ = insert_node($1, VARID);
        }
    | LP GETINT RP {
            if(evalFLAG == 1) {
                err = 1; yyerror("syntax error : get-int cannot be declared inside eval\n");
                }
            $$ = insert_node("GET-INT", CALL);
            }
    | LP PLUS expr expr extraPlus RP {
            insert_children(2, $3, $4);
            $$ = insert_node("PLUS", PLUS);
    }
    | LP MULT expr expr extraMult RP {
            insert_children(2, $3, $4);
            $$ = insert_node("MULT", MULT);
    }
    | LP MINUS expr expr RP {
            insert_children(2, $3, $4);
            $$ = insert_node("MINUS", MINUS);
    }
    | LP DIV expr expr RP {
            insert_children(2, $3, $4);
            $$ = insert_node("DIV", DIV);
    }
    | LP MOD expr expr RP  {
            insert_children(2, $3, $4);
            $$ = insert_node("MOD", MOD);
    }
    | LP IF expr expr expr RP {
            insert_children(3, $3, $4, $5);
            $$ = insert_node("IF", IF);
    }
    | LP ID optional2 RP {if(evalFLAG == 1) { 
                            err = 1; yyerror("syntax error : functions cannot called inside eval\n");
                            }
                            $$ = insert_node($2, CALL);
                            }
    | LP LET LP varid expr RP expr RP {
                if(evalFLAG == 1) {err = 1; yyerror("syntax error : let cannot be declared inside eval\n");
                }
                insert_children(3, $4, $5, $7);
                insert_node("LET", LET);
                }
    | TRUE {$$ = insert_node("TRUE", TRUE);}
    | FALSE {$$ = insert_node("FALSE", FALSE);}
    | LP GETBOOL RP { 
                if(evalFLAG == 1) 
                    err = 1; yyerror("syntax error : get-bool cannot be declared inside eval\n");
                $$ = insert_node("GET-BOOL", GETBOOL);
                }
    | LP EQUAL expr expr RP {
                insert_children(2, $3, $4);
                $$ = insert_node("=", EQUAL);}
    | LP LESS expr expr RP {
                insert_children(2, $3, $4);
                $$ = insert_node("<", LESS);
    }
    | LP LEQ expr expr RP {
                insert_children(2, $3, $4);
                $$ = insert_node("<=", LEQ);
    }
    | LP GREATER expr expr RP {
                insert_children(2, $3, $4);
                $$ = insert_node(">", GREATER);
    }
    | LP GEQ expr expr RP {
                insert_children(2, $3, $4);
                $$ = insert_node(">=", GEQ);
    }
    | LP NOT expr RP {
                insert_child($3);
                $$ = insert_node("NOT", NOT);
    }
    | LP AND expr expr extraAND RP {
                insert_children(2, $3, $4);
                $$ = insert_node("AND", AND);
    }
    | LP OR expr expr extraOR RP {
                insert_children(2, $3, $4);
                $$ = insert_node("OR", OR);
    }
    ;

varid: ID { $$ = insert_node($1, VARID); }
     ;
optional2: /* epsilon */ 
         | expr
         ;

extraPlus: 
     |expr extraPlus {additionalPlus += $1;} 
     ;

extraMult: /* epsilon */ 
     |expr extraMult {additionalMult *= $1;} 
     ;

optional3: /* epsilon */ 
         | expr
         ;
         
extraAND: /* epsilon */ 
             | expr extraAND {additionalAND *= $1;}
             ;

extraOR: /* epsilon */
            | expr extraOR {if($1 == 1) additionalOR = 1;}
            
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
    print_ast();
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
