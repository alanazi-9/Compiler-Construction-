%{
#include <stdio.h>
#include <stdlib.h>
#include "containers.h"

extern int yylex(void);
void yyerror(char *s);

struct kleeneStar{
    char *lexem;
    int tokenID;
    int funID;
    struct kleeneStar *next;
    struct kleeneStar *prev;
    struct ast* optional2;
};

struct kleeneStar *headOptional1;
struct kleeneStar *headOptional2;
struct kleeneStar *headExtraPlus;
struct kleeneStar *headExtraMult;
struct kleeneStar *headExtraAND;
struct kleeneStar *headExtraOR;

int lastFunID;

int evalFLAG;
int err;

%}

%union {
	char* str;
	int num;
}

%start program
%token EVAL LP RP PLUS MINUS MULT DEFINEFUN VAR GETINT GETBOOL LET IF PRINT DIV MOD TRUE FALSE NOT OR AND EQUAL LEQ GEQ LESS GREATER COMMENT FUNID VARID CALL ARGID ARGTYPE FUNRET ERROR
%token<str> ID NUMBER 
%token<num> INT BOOL NOTYET
%type<num> funtype argtype argid optional1 varid funid expr
%%
program:  LP DEFINEFUN LP funid optional1 RP funtype expr RP program {
                
                struct kleeneStar *head = headOptional1;

                if(head != NULL)
                    while(head->next != NULL)
                        head = head->next;
                        
                insert_child($4);
                int check = $4;

                while(head != NULL)
                {   if(check == head->funID)
                    {
                        insert_child(head->prev->tokenID);
                        insert_child(head->tokenID);
                    }
                    head = head->prev->prev;
                }
                    insert_child($7);
                    insert_child($8);
                    insert_node("define-fun", DEFINEFUN);

        }
        |  LP eval expr RP { 
                 

                insert_child($3);
                insert_node("eval", EVAL);
            }
        |  LP PRINT expr RP {
                insert_child($3);
                insert_node("print", PRINT);
        }
        |  LP ERROR expr RP {
                insert_child($3);
                insert_node("print", ERROR);
        }
        ;

funid: ID {
                $$ = insert_node($1, FUNID);
                lastFunID = $$;
        }
    ;
eval: EVAL {evalFLAG = 1;} 
    ;

funtype: INT {$$ = insert_node("Int", FUNRET);}
       | BOOL {$$ = insert_node("Bool", FUNRET);}
       ;
optional1: /* epsilon */ 
        | LP argid argtype RP optional1 
        {
            if(headOptional1 == NULL) 
            {
                headOptional1 = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                struct kleeneStar *tmp = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                //strcpy(headExtraPlus->lexem, $1);
                headOptional1->tokenID = $2; 
                headOptional1->funID = lastFunID;
                headOptional1->prev = NULL;
                headOptional1->next = tmp;
                tmp->tokenID = $3;
                tmp->funID = lastFunID;
                tmp->prev = headOptional1;
                tmp->next = NULL;
            }
            else {
                struct kleeneStar *curr1 = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                struct kleeneStar *curr2 = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                struct kleeneStar *tmp1;
                struct kleeneStar *tmp2;
                tmp1 = headOptional1;
                while(tmp1 != NULL)
                {
                    tmp2 = tmp1;
                    tmp1 = tmp1->next;
                }
                tmp2->next = curr1;
                //strcpy(curr->lexem, $1); 
                curr1->tokenID = $2;
                curr1->prev = tmp2;
                curr1->next = curr2;
                curr1->funID = lastFunID;
                curr2->tokenID = $3;
                curr2->funID = lastFunID;
                curr2->prev = curr1;
                curr2->next = NULL;
            }

            
        }
        ;

argtype: INT {$$ = insert_node("Int", ARGTYPE);}
       | BOOL {$$ = insert_node("Bool", ARGTYPE);}
       ;

argid: ID { $$ = insert_node($1, ARGID);}
    ;

expr: NUMBER {$$ = insert_node($1, NUMBER);}
    | ID {
            if(evalFLAG == 1) 
            {
                err = 1; 
                yyerror("syntax error : variables cannot be declared inside eval\n");
            }

            $$ = insert_node($1, VARID);
        }
    | LP GETINT RP {
            if(evalFLAG == 1) 
            {
                err = 1; 
                yyerror("syntax error : get-int cannot be declared inside eval\n");
            }
            $$ = insert_node("get-int", CALL);
            }
    | LP PLUS expr expr extraPlus RP {
            insert_children(2, $3, $4);

            while(headExtraPlus != NULL)
                {
                    insert_child(headExtraPlus->tokenID);
                    headExtraPlus = headExtraPlus->next;
                }
            $$ = insert_node("+", PLUS);
    }
    | LP MULT expr expr extraMult RP {
            insert_children(2, $3, $4);
            
            while(headExtraMult != NULL)
                {
                    insert_child(headExtraMult->tokenID);
                    headExtraMult = headExtraMult->next;
                }
            $$ = insert_node("*", MULT);
    }
    | LP MINUS expr expr RP {
            insert_children(2, $3, $4);
            $$ = insert_node("-", MINUS);
    }
    | LP DIV expr expr RP {
            insert_children(2, $3, $4);
            $$ = insert_node("/", DIV);
    }
    | LP MOD expr expr RP  {
            insert_children(2, $3, $4);
            $$ = insert_node("mod", MOD);
    }
    | LP IF expr expr expr RP {
            insert_children(3, $3, $4, $5);
            $$ = insert_node("if", IF);
    }
    | LP ID optional2 RP {
                                if(evalFLAG == 1) 
                                {
                                    err = 1; 
                                    yyerror("syntax error : functions cannot called inside eval\n");
                                }

                                if(headOptional2 != NULL)
                                    while(headOptional2->next != NULL)
                                        headOptional2 = headOptional2->next;

                                while(headOptional2 != NULL)
                                {   
                                    insert_child(headOptional2->tokenID);
                                    headOptional2 = headOptional2->prev;
                                }
                                $$ = insert_node($2, CALL);
                            }
    | LP LET LP varid expr RP expr RP {
                if(evalFLAG == 1) 
                {
                    err = 1; 
                    yyerror("syntax error : let cannot be declared inside eval\n");
                }
                insert_children(3, $4, $5, $7);
                $$ = insert_node("let", LET);
                }
    | TRUE {$$ = insert_node("true", TRUE);}
    | FALSE {$$ = insert_node("false", FALSE);}
    | LP GETBOOL RP { 
                if(evalFLAG == 1)
                {
                    err = 1; 
                    yyerror("syntax error : get-bool cannot be declared inside eval\n");
                } 
                $$ = insert_node("get-bool", GETBOOL);
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
                $$ = insert_node("not", NOT);
    }
    | LP AND expr expr extraAND RP {
                insert_children(2, $3, $4);

                while(headExtraAND != NULL)
                {
                    insert_child(headExtraAND->tokenID);
                    headExtraAND = headExtraAND->next;
                }
                $$ = insert_node("and", AND);
    }
    | LP OR expr expr extraOR RP {
                insert_children(2, $3, $4);

                while(headExtraOR != NULL)
                {
                    insert_child(headExtraOR->tokenID);
                    headExtraOR = headExtraOR->next;
                }
                $$ = insert_node("or", OR);
    }
    ;

varid: ID { 
                $$ = insert_node($1, VARID);
        }
     ;
optional2: /* epsilon */ 
         | expr optional2{
            if(headOptional2 == NULL) 
            {
                headOptional2 = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                headOptional2->tokenID = $1; 
                headOptional2->next = NULL;
                headOptional2->prev = NULL;
            }
            else {
                struct kleeneStar *curr = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                struct kleeneStar *tmp1;
                struct kleeneStar *tmp2;
                tmp1 = headOptional2;
                while(tmp1 != NULL)
                {
                    tmp2 = tmp1;
                    tmp1 = tmp1->next;
                }
                tmp2->next = curr;
                curr->prev = tmp2;
                curr->tokenID = $1;
                curr->next = NULL;
            }


         }
         ;

extraPlus: 
     |expr extraPlus {
         if(headExtraPlus == NULL) 
            {
                headExtraPlus = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                headExtraPlus->tokenID = $1; 
                headExtraPlus->next = NULL;
                headExtraPlus->prev = NULL;
            }
            else {
                struct kleeneStar *curr = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                struct kleeneStar *tmp1;
                struct kleeneStar *tmp2;
                tmp1 = headExtraPlus;
                while(tmp1 != NULL)
                {
                    tmp2 = tmp1;
                    tmp1 = tmp1->next;
                }
                tmp2->next = curr;
                curr->prev = tmp2;
                curr->tokenID = $1;
                curr->next = NULL;
            }
        } 
     ;

extraMult: /* epsilon */ 
     |expr extraMult {
         if(headExtraMult == NULL) 
            {
                headExtraMult = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                headExtraMult->tokenID = $1; 
                headExtraMult->next = NULL;
                headExtraMult->prev = NULL;
            }
            else {
                struct kleeneStar *curr = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                struct kleeneStar *tmp1;
                struct kleeneStar *tmp2;
                tmp1 = headExtraMult;
                while(tmp1 != NULL)
                {
                    tmp2 = tmp1;
                    tmp1 = tmp1->next;
                }
                tmp2->next = curr; 
                curr->prev = tmp2;
                curr->tokenID = $1;
                curr->next = NULL;
            }
        } 
     ;

extraAND: /* epsilon */ 
             | expr extraAND {
                if(headExtraAND == NULL) 
                {
                    headExtraAND = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                    headExtraAND->tokenID = $1; 
                    headExtraAND->next = NULL;
                    headExtraAND->prev = NULL;
                }
                else {
                struct kleeneStar *curr = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                struct kleeneStar *tmp1;
                struct kleeneStar *tmp2;
                tmp1 = headExtraAND;
                while(tmp1 != NULL)
                {
                    tmp2 = tmp1;
                    tmp1 = tmp1->next;
                }
                tmp2->next = curr;
                curr->prev = tmp2;
                curr->tokenID = $1;
                curr->next = NULL;
                }
             }
             ;

extraOR: /* epsilon */
            | expr extraOR {
                if(headExtraOR == NULL) 
                {
                    headExtraOR = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                    headExtraOR->tokenID = $1; 
                    headExtraOR->next = NULL;
                    headExtraOR->prev = NULL;
                }
                else {
                struct kleeneStar *curr = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                struct kleeneStar *tmp1;
                struct kleeneStar *tmp2;
                tmp1 = headExtraOR;
                while(tmp1 != NULL)
                {
                    tmp2 = tmp1;
                    tmp1 = tmp1->next;
                }
                tmp2->next = curr;
                curr->prev = tmp2;
                curr->tokenID = $1;
                curr->next = NULL;
            }}
            
%%

void yyerror(char *s)
{
    printf("%s\n", s);
}

