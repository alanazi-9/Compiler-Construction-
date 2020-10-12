%{
#include <stdio.h>
#include <stdlib.h>
#include "containers.h"

#define MAX 50

extern int yylex(void);
void yyerror(char *s);
void set_ID_value(char id, int value);
int get_ID_value(char id);
int check_fun_define(char *name);
void insert_fun(char *name);
int insertArg(char *funName, char *argName, int argType, int tokenID);
void insert_retType(char *funName, int retType);
void insertVar(char *name, int type, bool funArg);
int check_var(char *name);
struct definedFunctions* findFun(char *name);

struct kleeneStar{
    char *lexem;
    int tokenID;
    struct kleeneStar *next; 
};

struct funArg{
    char *funName;
    char *argName;
    int tokenID;
    int argType;
    struct funArg *next;
};
struct definedFunctions{
    char* funName;
    int argNum;
    int retType;
    struct funArg *argsHead;
    struct definedFunctions *next;
};

struct varList{
    char *varName;
    int varType;
    bool isFunArg;
    struct varList *next;
};

struct definedFunctions *funHead;
struct varList *varHead;

struct funArg *headOptinal1;
struct kleeneStar *headExtraPlus;
struct kleeneStar *headExtraMult;
struct kleeneStar *headExtraAND;
struct kleeneStar *headExtraOR;

char *lastDefinedFun;
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
%token EVAL LP RP PLUS MINUS MULT DEFINEFUN VAR GETINT GETBOOL LET IF PRINT DIV MOD TRUE FALSE NOT OR AND EQUAL LEQ GEQ LESS GREATER COMMENT FUNID VARID CALL
%token<str> ID NUMBER 
%token<num> INT BOOL NOTYET
%type<num> argtype argid optional1 varid funid expr type 
%%
program:  LP DEFINEFUN LP funid optional1 RP type expr RP program {
                struct ast* tmp = find_ast_node($4);
                
                insert_child($4);
                struct definedFunctions *fun = findFun(tmp->token);
                struct funArg *funArgs = (struct funArg *)malloc(sizeof(struct funArg));
                funArgs = fun->argsHead;
                    while(funArgs != NULL)
                    {
                        printf("ast id: %d\t token: %s\t ntoken: %d\n", tmp->id, tmp->token, tmp->ntoken);
                        printf("tmp-token: %s\n", tmp->token);
                        insert_child(funArgs->tokenID);
                        funArgs = funArgs->next;
                    }
                    if(funArgs == NULL)
                        printf("null\n");
                    
                    printf("fun ret type: %d\n", fun->retType);
                    insert_child($8);
                    insert_node("define-fun", DEFINEFUN);
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
                printf("Print: %d\n", current_node_id);
        }
        ;

funid: ID {
            if(check_fun_define($1) == 1)
            {
                lastDefinedFun = $1;
                insert_fun($1);
                $$ = insert_node($1, FUNID);
                printf("Funid: %s\n", $1);
            }
            else 
                printf("Function '%s' is already defined\n", $1);
        }
    ;
eval: EVAL {evalFLAG = 1;} 
    ;
optional1: /* epsilon */ 
        | LP argid argtype RP optional1 
        {
                struct definedFunctions *fun = findFun(lastDefinedFun);
                struct ast* node = find_ast_node($2);
                printf("op1 ast id: %d\t token: %s\t ntoken: %d\n", node->id, node->token, node->ntoken);
                if(fun->argsHead == NULL) 
                {
                    struct funArg *tmp = (struct funArg *)malloc(sizeof(struct funArg));
                    tmp->funName = fun->funName;
                    tmp->tokenID = $2; 
                    tmp->argType = $3;
                    tmp->argName = node->token;
                    tmp->next = NULL;
                    fun->argsHead = tmp;
                    fun->argNum++;

                    insertVar(tmp->argName, tmp->argType, true);
                }
                else {
                    struct funArg *curr = (struct funArg *)malloc(sizeof(struct funArg));
                    struct funArg *tmp1;
                    struct funArg *tmp2;
                    tmp1 = fun->argsHead;
                    while(tmp1 != NULL)
                    {
                        tmp2 = tmp1;
                        tmp1 = tmp1->next;
                    }
                    tmp2->next = curr;
                    //strcpy(curr->lexem, $1);
                    curr->funName = fun->funName; 
                    curr->tokenID = $2; 
                    curr->argType = $3;
                    curr->argName = node->token;
                    curr->next = NULL;
                    fun->argNum++;
                    insertVar(curr->argName, curr->argType, true);
                }
            
        }
        ;

argtype: INT {$$ = INT;}
       | BOOL {$$ = BOOL;}
       ;

argid: ID { $$ = insert_node($1, VARID);}
    ;
type: INT {$$ = INT;}
    | BOOL {$$ = BOOL;}
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
            while(headExtraPlus != NULL)
                {
                    insert_child(headExtraPlus->tokenID);
                    headExtraPlus = headExtraPlus->next;
                }
            $$ = insert_node("PLUS", PLUS);
    }
    | LP MULT expr expr extraMult RP {
            insert_children(2, $3, $4);
            while(headExtraMult != NULL)
                {
                    insert_child(headExtraMult->tokenID);
                    headExtraPlus = headExtraMult->next;
                }
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
                $$ = insert_node("LET", LET);
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
                while(headExtraAND != NULL)
                {
                    insert_child(headExtraAND->tokenID);
                    headExtraPlus = headExtraAND->next;
                }
                $$ = insert_node("AND", AND);
    }
    | LP OR expr expr extraOR RP {
                insert_children(2, $3, $4);
                while(headExtraOR != NULL)
                {
                    insert_child(headExtraOR->tokenID);
                    headExtraPlus = headExtraOR->next;
                }
                $$ = insert_node("OR", OR);
    }
    ;

varid: ID { 
                insertVar($1, NOTYET, false);
                $$ = insert_node($1, VARID);
        }
     ;
optional2: /* epsilon */ 
         | expr
         ;

extraPlus: 
     |expr extraPlus {
         if(headExtraPlus == NULL) 
            {
                headExtraPlus = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                //strcpy(headExtraPlus->lexem, $1);
                headExtraPlus->tokenID = $1; 
                headExtraPlus->next = NULL;
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
                //strcpy(curr->lexem, $1); 
                curr->tokenID = $1;
                curr->next = NULL;
            }
        } 
     ;

extraMult: /* epsilon */ 
     |expr extraMult {if(headExtraMult == NULL) 
            {
                headExtraMult = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                //strcpy(headExtraPlus->lexem, $1);
                headExtraMult->tokenID = $1; 
                headExtraMult->next = NULL;
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
                curr->tokenID = $1;
                curr->next = NULL;
            }
        } 
     ;

optional3: /* epsilon */ 
         | expr
         ;
         
extraAND: /* epsilon */ 
             | expr extraAND {
                if(headExtraAND == NULL) 
                {
                    headExtraAND = (struct kleeneStar *)malloc(sizeof(struct kleeneStar));
                    //strcpy(headExtraPlus->lexem, $1);
                    headExtraAND->tokenID = $1; 
                    headExtraAND->next = NULL;
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
                //strcpy(curr->lexem, $1); 
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
                    //strcpy(headExtraPlus->lexem, $1);
                    headExtraOR->tokenID = $1; 
                    headExtraOR->next = NULL;
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
                //strcpy(curr->lexem, $1); 
                curr->tokenID = $1;
                curr->next = NULL;
            }}
            
%%

void main()
{   funHead = NULL;
    varHead = NULL;
    headOptinal1 = NULL;
    headExtraPlus = NULL;
    additionalPlus = 0;
    additionalMult = 1;
    additionalOR = 0;
    additionalAND = 1;
    evalFLAG = 0;
    err = 0;

    symTable = calloc(26, sizeof(int));
    yyparse();
    
    struct definedFunctions* tmp = funHead;
    struct funArg* tmp2;
    while(tmp != NULL){
        printf("fun name: %s No. Args: %d =>", tmp->funName, tmp->argNum);
        tmp2 = tmp->argsHead;
        while(tmp2 != NULL)
        {
            printf(" %s => %d", tmp2->argName, tmp2->argType);
            tmp2= tmp2->next;
        }
        tmp = tmp->next;
    }
    printf("\n");
    struct varList* tmp3 = varHead;
    while(tmp3 != NULL){
        printf("var name: %s type: %d => is it fun arg?: %d", tmp3->varName, tmp3->varType, tmp3->isFunArg);
        tmp3 = tmp3->next;
    }

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

int check_fun_define(char *name)
{
    struct definedFunctions *tmp = funHead;
    while(tmp != NULL)
    {
        if(strcmp(name, tmp->funName) == 0)
            return 0;
        else tmp = tmp->next;
    }
    printf("check-fun: not found\n");
    return 1;
}

void insert_fun(char *name)
{
    if(funHead == NULL)
    {
        funHead = (struct definedFunctions *)malloc(sizeof(struct definedFunctions));
        funHead->funName = name;
        funHead->next = NULL;
        funHead->argsHead = NULL;
        funHead->argNum = 0;
    }
    else
    {
        if(check_fun_define(name) == 1)
        {
            struct definedFunctions *curr = (struct definedFunctions *)malloc(sizeof(struct definedFunctions));
            struct definedFunctions *tmp1;
            struct definedFunctions *tmp2;
            tmp1 = funHead;

            while(tmp1 != NULL)
            {
                tmp2 = tmp1;
                tmp1 = tmp1->next;
            }
            
            tmp2->next = curr; 
            curr->funName = name;
            curr->next = NULL;
            curr->argsHead = NULL;
            curr->argNum = 0;
        }
    }
}

int insertArg(char *funName, char *argName, int argType, int tokenID)
{
    struct definedFunctions *tmp = funHead;
    
    if(funHead == NULL)
        printf("fun-head = null\n");

    while(tmp != NULL)
    {
        if(strcmp(funName, tmp->funName) == 0)
        {
            if(tmp->argsHead == NULL)
            {
                struct funArg *head = (struct funArg *)malloc(sizeof(struct funArg));
                head->funName = funName;
                head->argName = argName;
                head->tokenID = tokenID;
                head->argType = argType;
                head->next = NULL;
                tmp->argsHead = head;
                tmp->argNum++;
                printf("insert-arg: argsHead = NULL\n");
                return 0;
            }
            else
            {
                struct funArg *curr = (struct funArg *)malloc(sizeof(struct funArg));
                struct funArg *tmp1;
                struct funArg *tmp2;
                tmp1 = tmp->argsHead;
                while(tmp1 != NULL)
                {
                    tmp2 = tmp1;
                    tmp1 = tmp1->next;
                }
                tmp2->next = curr;
                //strcpy(curr->lexem, $1); 
                curr->funName = funName;
                curr->argName = argName;
                curr->tokenID = tokenID;
                curr->argType = argType;
                curr->next = NULL;
                tmp->argNum++;
                printf("insert-arg: argsHead NOT NULL\n");
                return 0;
            }
        }
        tmp = tmp->next;
    }
    return 0;
}

void insert_retType(char *funName, int retType)
{
    struct definedFunctions *tmp = funHead;
    while(tmp != NULL)
    {
        if(strcmp(funName, tmp->funName))
            tmp->retType = retType;
        else tmp = tmp->next;
    }
}

struct definedFunctions* findFun(char *name)
{
    struct definedFunctions *tmp = funHead;
    while(tmp != NULL)
    {
        if(strcmp(name, tmp->funName) == 0)
            return tmp;
        else tmp = tmp->next;
    }
}

int check_var(char *name)
{
    struct varList *tmp = varHead;
    while(tmp != NULL)
    {
        if(strcmp(name, tmp->varName) == 0)
            return 0;
        else tmp = tmp->next;
    }
    printf("var: not found\n");
    return 1;
}

void insertVar(char *name, int type, bool funArg)
{
    if(varHead == NULL)
    {
        varHead = (struct varList *)malloc(sizeof(struct varList));
        varHead->varName = name;
        varHead->varType = type;
        varHead->isFunArg = funArg;
        varHead->next = NULL;
    }
    else
    {
            struct varList *curr = (struct varList *)malloc(sizeof(struct varList));
            struct varList *tmp1;
            struct varList *tmp2;
            tmp1 = varHead;

            while(tmp1 != NULL)
            {
                tmp2 = tmp1;
                tmp1 = tmp1->next;
            }
            
            tmp2->next = curr; 
            curr->varName = name;
            curr->varType = type;
            curr->isFunArg = funArg;
            curr->next = NULL;
        
    }
}
