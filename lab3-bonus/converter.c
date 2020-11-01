#include "y.tab.h"
#include "containers.h"

#define DEBUGMODE 0

struct definedFunctions{
    char *funName;
    int argNum;
    char *retType;
    int calls;
    int call_tracker;
    bool get_int;
    struct definedFunctions *next;
};

struct varList{
    char *funName;
    char *varName;
    char  *varType;
    int letID;
    struct varList *next;
};


struct definedFunctions *funHead;
struct varList *varHead;
struct definedFunctions *lastFun;
struct varList *lastVar;

struct definedFunctions *fun_get_int;

struct ast* argID;
struct ast* argType;

int get_int_total;
int get_int_tracker;
FILE *smt_file;

int yyparse();

char* get_type(int type)
{
    if (type == INT || type == NUMBER || type == PLUS || type == MINUS || type == MULT || type == DIV || type == MOD)
        return "Int";
    
    if (type == BOOL || type == TRUE || type == FALSE || type == NOT || type == AND || type == OR ||type == EQUAL || type == GREATER || type == LESS || type == LEQ || type == GEQ)
        return "Bool";
}

struct definedFunctions* get_fun(char* name)
{
    struct definedFunctions *tmp = funHead;
    while(tmp != NULL)
    {
        if(strcmp(name, tmp->funName) == 0)
            return tmp;
        else tmp = tmp->next;
    }
    return NULL;
}

struct varList* get_var(char *funName, char *varName, int letID)
{
    struct varList *tmp = varHead;

    while(tmp != NULL)
    {
        if(strcmp(funName, tmp->funName) == 0 && strcmp(varName, tmp->varName) == 0 && tmp->letID == letID)
            return tmp;
        else tmp = tmp->next;
    }
    return NULL;
}

struct varList* get_arg(char *funName, int number)
{
    struct varList *tmp = varHead;

    while(tmp != NULL)
    {
        if(strcmp(funName, tmp->funName) == 0)
        {
            if(number == 1)
                return tmp;
            else
            {
                for(int i = 1; i < number; i++)
                    tmp = tmp->next;

                return tmp;
            }
        }
        else 
            tmp = tmp->next;
    }
    return NULL;
}

char *get_fun_name(struct ast* ast)
{
   struct ast_child* child;
    while(ast != NULL)
    {
        if(ast->ntoken == PRINT && ast->parent == NULL|| ast->ntoken == EVAL && ast->parent == NULL || ast->ntoken == ERROR && ast->parent == NULL)
        {
            if(DEBUGMODE)
                printf("get_fun_name: %s\n", ast->child->id->token);
            return ast->token;
        }
            
        if(ast->ntoken == DEFINEFUN)
        {
            if(DEBUGMODE)
                printf("get_fun_name: %s\n", ast->child->id->token);
            return ast->child->id->token;
        }
        ast = ast->parent;
    }
}


int is_fla(struct ast* ast)
{
    if(ast->ntoken == CALL)
    {
        struct definedFunctions *fun = get_fun(ast->token);
        if(fun != NULL)
        {
            if(strcmp(fun->retType, "Bool") == 0)
                return 1;
            else 
                return 0;
        }
        else 
        {
            printf("Function '%s' is NOT defined before\n", ast->token);
            return -1;
        }
    }
    else if(ast->ntoken == VARID)
    {
        struct varList* varid;
        struct ast *parent = ast->parent;
        char *funName = get_fun_name(ast->parent);
        int check = 0;

        while(parent != NULL)
        {
            varid = get_var(funName, ast->token, parent->id);
            if(varid != NULL)
            {
                check = 1;
                break;
            }
            else 
                parent = parent->next;
        }

        if(check == 0)
            varid = get_var(funName, ast->token, 0);

        if(varid != NULL)
        {
            if(strcmp(varid->varType, "Bool") == 0)
                return 1;
            else 
                return 0;
        }
        else 
        {
            printf("Variable '%s' is NOT defined before\n", ast->token);
            return -1;
        }
    }
    else if(ast->ntoken == IF)
    {
        struct ast_child* child = ast->child->next;
        return is_fla(child->id);
    }
    else if(ast->ntoken == LET)
    {
        struct ast_child* child = ast->child->next->next;
        return is_fla(child->id);
    }
    else if (ast->ntoken == BOOL || ast->ntoken == TRUE || ast->ntoken == FALSE || ast->ntoken == NOT || ast->ntoken == AND || ast->ntoken == OR ||ast->ntoken == EQUAL || ast->ntoken == GREATER || ast->ntoken == LESS || ast->ntoken == LEQ || ast->ntoken == GEQ)
            return 1;
    else 
        return 0;
}

int is_term(struct ast* ast)
{
    if(ast->ntoken == CALL)
    {
        struct definedFunctions *fun = get_fun(ast->token);
        if(fun != NULL)
        {
            if(strcmp(fun->retType, "Int") == 0)
                return 1;
            else 
                return 0;
        }
        else 
        {
            printf("Function '%s' is NOT defined before\n", ast->token);
            return -1;
        }
    }
    else if(ast->ntoken == VARID)
    {
        struct varList* varid;
        struct ast *parent = ast->parent;
        char *funName = get_fun_name(ast->parent);
        int check = 0;

        while(parent != NULL)
        {
            varid = get_var(funName, ast->token, parent->id);
            if(varid != NULL)
            {
                check = 1;
                break;
            }
            else 
                parent = parent->next;
        }

        if(check == 0)
            varid = get_var(funName, ast->token, 0);

        if(varid != NULL)
        {
            if(strcmp(varid->varType, "Int") == 0)
                return 1;
            else 
                return 0;
        }
        else 
        {
            printf("Variable '%s' is NOT defined before\n", ast->token);
            return -1;
        }
    }
    else if(ast->ntoken == IF)
    {
        struct ast_child* child = ast->child->next;
        return is_term(child->id);
    }
    else if(ast->ntoken == LET)
    {
        struct ast_child* child = ast->child->next->next;
        return is_term(child->id);
    }
    else if (ast->ntoken == INT || ast->ntoken == NUMBER || ast->ntoken == PLUS || ast->ntoken == MINUS || ast->ntoken == MULT || ast->ntoken == DIV || ast->ntoken == MOD)
        return 1;
    else
        return 0;
}

int needs_term(int exp)
{
    if(exp == PLUS || exp == MINUS || exp == MULT || exp == DIV || exp == MOD || exp == EQUAL || exp == GREATER || exp == LESS || exp == LEQ || exp == GEQ)
        return 1;
    else
        return 0;
}

int needs_fla(int exp)
{
    if(exp == NOT || exp == AND || exp == OR)
        return 1;
    else
        return 0;
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
    return 1;
}

int check_var(char *funName, char *varName, int letID)
{
    struct varList *tmp = varHead;

    while(tmp != NULL)
    {
        if(strcmp(funName, tmp->funName) == 0 && strcmp(varName, tmp->varName) == 0)
        {
            if(tmp->letID == 0 && letID >= 0)
                return 0;

            if(tmp->letID == letID)
                return 0;
        }
        else tmp = tmp->next;
    }
    return 1;
}

int insert_fun(struct definedFunctions *fun)
{
    if(funHead == NULL)
    {
        if(DEBUGMODE)
            printf("funHead = NULL\n");

        funHead = (struct definedFunctions *)malloc(sizeof(struct definedFunctions));
        funHead->funName = fun->funName;
        funHead->retType = fun->retType;
        funHead->argNum = fun->argNum;
        funHead->calls = fun->calls;
        funHead->call_tracker = fun->call_tracker;
        funHead->get_int = fun->get_int;
        lastFun = funHead;

        if(DEBUGMODE)
            printf("funHead name: %s\n", fun->funName);

        return 0;
    }
    else
    {
        if(check_fun_define(fun->funName) == 1)
        {
            if(DEBUGMODE)
            {
                printf("funHead NOT NULL\n");
                printf("INSIDE insert-fun {lastFun} Fun name: %s No. Args: %d Ret. Type: %s\n", lastFun->funName, lastFun->argNum, lastFun->retType);
            }
                
            struct definedFunctions *curr = (struct definedFunctions *)malloc(sizeof(struct definedFunctions));
            lastFun->next = curr; 
            curr->funName = fun->funName;
            curr->retType = fun->retType;
            curr->argNum = fun->argNum;
            curr->calls = fun->calls;
            funHead->call_tracker = fun->call_tracker;
            curr->get_int = fun->get_int;
            lastFun = curr;

            if(DEBUGMODE)
                printf("INSIDE insert-fun {curr} Fun name: %s No. Args: %d Ret. Type: %s\n", curr->funName, curr->argNum, curr->retType);

            return 0;
        }
        else{
            printf("'%s' function is already defined\n",fun->funName);
            return 1;
        }
    }
}

int insert_var(char *funName, char *varName, char *varType, int letID)
{
    if(varHead == NULL)
    {
        if(check_fun_define(varName) == 0)
        {
            printf("'%s' variable is already defined as a function\n",varName);
            return 1;
        }

        varHead = (struct varList *)malloc(sizeof(struct varList));
        varHead->funName = funName;
        varHead->varName = varName;
        varHead->varType = varType;
        varHead->letID = letID;
        varHead->next = NULL;
        lastVar = varHead;
        return 0;
    }
    else{
        if(check_var(funName, varName, letID) == 1)
        {
            if(check_fun_define(varName) == 0)
            {
                printf("'%s' variable is already defined as a function\n", varName);
                return 1;
            }

            struct varList *curr = (struct varList *)malloc(sizeof(struct varList));
            curr->funName = funName;
            curr->varName = varName;
            curr->varType = varType;
            curr->letID = letID;
            curr->next = NULL;
            lastVar->next = curr;
            lastVar = curr;
            return 0;
        }
        else
        {
            printf("'%s' variable is already defined\n", varName);
            return 1;
        }
    }
}
int fillSymTable(struct ast* ast)
{
    if(ast->ntoken == FUNID)
    {
        if(DEBUGMODE)
            printf("fillSymTable: currToken == FUNID\n");
   
        if(DEBUGMODE)
            printf("fillSymTable: fun NOT in the symbol table\n");
            
        struct definedFunctions *fun = (struct definedFunctions *)malloc(sizeof(struct definedFunctions));
        fun->funName = ast->token;
        fun->argNum = 0;
        fun->calls = 0;
        fun->call_tracker = 0;
        fun->get_int = false;
        fun->next = NULL;

        if(ast->next->ntoken == FUNRET)
        {
            fun->retType = ast->next->token;

            if(strcmp(ast->next->next->token, "get-int") == 0)
                fun->get_int = true;

            return insert_fun(fun);
        }
        else{
                argID = ast->next;
                argType = argID->next;
                fun->argNum++;

                if(strcmp(fun->funName, argID->token) == 0)
                {
                    printf("'%s' variable is already defined as a function\n", argID->token);
                    return 1;
                }

                if(insert_var(fun->funName, argID->token, argType->token, 0) == 1)
                    return 1;

                while(argType->next->ntoken != FUNRET)
                {
                    argID = argType->next;
                    argType = argID->next;
                    fun->argNum++;

                    if(strcmp(fun->funName, argID->token) == 0)
                    {
                        printf("'%s' variable is already defined as a function\n", argID->token);
                        return 1;
                    }

                    if(insert_var(fun->funName, argID->token, argType->token, 0) == 1)
                        return 1;
                }
                fun->retType = argType->next->token;

                if(strcmp(argType->next->next->token, "get-int") == 0)
                    fun->get_int = true;

                return insert_fun(fun);
            }    
    }
    if(ast->ntoken == LET)
    {
        struct ast_child *currChild = ast->child;
        struct ast_child *nextChild = currChild->next;

        struct ast *parent = ast->parent;
        char *funName = get_fun_name(parent);
        char* type;

        if(is_term(nextChild->id) == -1)
            return 1;

        if(is_term(nextChild->id) == 1)
            type = "Int";
        else    
            type = "Bool";

        if(check_fun_define(currChild->id->token) == 0 || check_var(currChild->id->token, funName, ast->id) == 0)
        {
            printf("Variable is defioned before\n");
            return 1;
        }

        while(parent != NULL)
        {
            if(parent->ntoken == LET && strcmp(parent->child->id->token, currChild->id->token) == 0)
            {
                printf("Variable '%s' is defioned before in '%s'\n", currChild->id->token, parent->token);
                return 1;
            }  
            parent = parent->parent; 
        }
        return insert_var(funName, currChild->id->token, type, ast->id);
    }

}


int calc_get_int(struct ast* ast)
{
     if(ast->ntoken == CALL && strcmp(ast->token, "get-int") == 0)
    {
        if(ast->parent->ntoken == DEFINEFUN)
        {
            struct ast* parent = ast->parent;
            struct ast_child* child = parent->child;
            
            while(child->id->ntoken != FUNRET)
                child = child->next;

            child = child->next;

            if(strcmp(child->id->token, "get-int") == 0)
                return 0;
        }
        else
        {
            get_int_total++;
            return 0;
        }
    }
}

int child_tree(struct ast* ast)
{
    if(ast->child == NULL)
    {
        if(ast->ntoken == CALL)
        {
            if(strcmp(ast->token, "get-int") == 0)
            {
                fprintf(smt_file,"get_int_out_%d", get_int_tracker);
                get_int_tracker++;
            }
            else
            {
                fun_get_int = get_fun(ast->token);

                if(fun_get_int->get_int == true && fun_get_int->calls > 1)
                {
                    //printf("call no. %d of %s", fun_get_int->call_tracker, ast->token);
                    fprintf(smt_file,"%s%d", ast->token, fun_get_int->call_tracker);
                    fun_get_int->call_tracker++;  
                }
                else 
                    fprintf(smt_file,"%s", ast->token);
            }
            if(ast->parent->ntoken == LET && ast->parent->child->next->id->id == ast->id)
                    fprintf(smt_file,"))");
        }
        else
        {
            if(ast->parent->ntoken == LET && ast->ntoken == VARID)
                    fprintf(smt_file,"((");

            fprintf(smt_file,"%s", ast->token);

            if(ast->parent->ntoken == LET && ast->parent->child->next->id->id == ast->id)
                    fprintf(smt_file,"))");
        }  
            
        
    }
    else
    {
        struct ast_child* child = ast->child;
        if(ast->ntoken == CALL && strcmp(ast->token, "get-int") != 0)
        {
            fun_get_int = get_fun(ast->token);
            if(fun_get_int->get_int == true && fun_get_int->call_tracker > 1)
            {
                fprintf(smt_file,"(%s%d", ast->token, fun_get_int->call_tracker);
                fun_get_int->call_tracker++;
                while(child != NULL)
                {
                    fprintf(smt_file," ");
                    child_tree(child->id);
                    child = child->next;
                }
                fprintf(smt_file,")");
            }
            else
            {
                fprintf(smt_file,"(%s", ast->token);
                while(child != NULL)
                {
                    fprintf(smt_file," ");
                    child_tree(child->id);
                    child = child->next;
                }
                fprintf(smt_file,")");
            }
            
        }  
        else 
        {
            fprintf(smt_file,"(%s", ast->token);

            while(child != NULL)
            {
                fprintf(smt_file," ");
                child_tree(child->id);
                child = child->next;
            }
            fprintf(smt_file,")");
        }
    }
}

int calc_calls(struct ast* ast)
{
    if(ast->ntoken == CALL && strcmp(ast->token, "get-int") != 0)
    {
        fun_get_int = get_fun(ast->token);
        if(fun_get_int != NULL)
            if(fun_get_int->get_int == true)
            {
                fun_get_int->calls++;
                if(DEBUGMODE)
                    printf("calls: %d\n", fun_get_int->calls);
                get_int_total++;
            }
    }
    return 0;
}

int conert_to_smt(struct ast* ast)
{
    int currToken = ast->ntoken;
    int term_needed = needs_term(currToken);
    int fla_needed = needs_fla(currToken);
    int num;
    

    smt_file = fopen("converted.smt","a");

    if(smt_file == NULL)
    {
        printf("Error!");   
        return 1;             
    }

    if(currToken == FUNID)
    {
        struct ast* parent = ast->parent;
        struct ast_child* child = parent->child;

        fun_get_int = get_fun(child->id->token);

        if(fun_get_int->get_int == true)
        {
            if(DEBUGMODE)
                printf("currToken == FUNID\n"); 
            for(int i = 0; i < fun_get_int->calls; i++)
            {
                fprintf(smt_file,"(define-fun ");
                if(fun_get_int->calls > 1)
                    fprintf(smt_file,"%s%d (", child->id->token, i);
                else
                    fprintf(smt_file,"%s (", child->id->token);

                if(DEBUGMODE)
                    printf("currToken == FUNID i = %d fun: %s%d\n", i, child->id->token, i);

                child = child->next;
                if(DEBUGMODE)
                    printf("currToken == FUNID funret: %s\n",child->id->token);

                if(child->id->ntoken != FUNRET) 
                {
                    if(DEBUGMODE)
                        printf("currToken == FUNID i = %d\n", i);

                    int i = 1;
                    while(child->id->ntoken != FUNRET)
                    {
                        if(DEBUGMODE)
                            printf("argument %d: %s type: %s\n", i, child->id->token, child->next->id->token);

                        fprintf(smt_file,"(%s %s)", child->id->token, child->next->id->token);
                        child = child->next->next;

                        if(child->id->ntoken != FUNRET) //to add a space between arguments if it is not the last argument
                            fprintf(smt_file," ");

                        i++;
                    }
            
                }
                child = child->next;
                fprintf(smt_file,") ");
                child_tree(child->id);
                child = parent->child;
                fprintf(smt_file,")\n");

            }
            
        }
        else
        {
                if(DEBUGMODE)
                    printf("define-fun first child: %s\n", child->id->token);
        
                fprintf(smt_file,"(define-fun ");
                fprintf(smt_file,"%s (", child->id->token);
                child = child->next;
                
                if(child->id->ntoken != FUNRET) 
                {
                    int i = 1;
                    while(child->id->ntoken != FUNRET)
                    {
                        if(DEBUGMODE)
                            printf("argument %d: %s type: %s\n", i, child->id->token, child->next->id->token);

                        fprintf(smt_file,"(%s %s)", child->id->token, child->next->id->token);
                        child = child->next->next;

                        if(child->id->ntoken != FUNRET) //to add a space between arguments if it is not the last argument
                            fprintf(smt_file," ");

                        i++;
                    }
                    
                }

                if(DEBUGMODE)
                    printf("return type: %s\n", child->id->token);

                fprintf(smt_file,") %s ", child->id->token);
                child = child->next;
                
                
                    fprintf(smt_file,"(%s", child->id->token);

                    if(child->id->child != NULL)
                    {
                        int i = 1;
                        struct ast_child* grand_child = child->id->child;
                        while(grand_child != NULL)
                        {
                            if(DEBUGMODE)
                            {
                                printf("[define-fun] expr(%s) (child %d): %s\n", child->id->token, i, grand_child->id->token);
                                i++;
                            }

                            fprintf(smt_file," ");
                            child_tree(grand_child->id);
                            grand_child = grand_child->next;
                        }
                    }  
            fprintf(smt_file,")");
            fprintf(smt_file,")\n");  
        }
        
    }
    if(currToken == ERROR)
    {
        struct ast_child* child = ast->child;

        fprintf(smt_file,"(assert ");
        fprintf(smt_file,"(%s", child->id->token);
        
        if(DEBUGMODE)
            printf("assert first child: %s\n", child->id->token);

        /*if(child->id->ntoken == LET)
        {
            printf("LET child\n");
            struct ast_child* letchild = child->id->child;

            fprintf(smt_file,"(%s", letchild->id->token);
            printf("LET child: %s\n", letchild->id->token);
            letchild = letchild->next;
            struct ast_child* grand_let_child = letchild->id->child;

            int i = 1;
            while(grand_let_child != NULL)
            {
                if(DEBUGMODE)
                    printf("[error] expr(%s) (child %d): %s\n", child->id->token, i, grand_let_child->id->token);

                fprintf(smt_file," ");
                child_tree(grand_let_child->id);
                grand_let_child = grand_let_child->next;
            }
            
            letchild = letchild->next;
            grand_let_child = grand_let_child->id->child;
            while(grand_let_child != NULL)
            {
                if(DEBUGMODE)
                    printf("[error] expr(%s) (child %d): %s\n", child->id->token, i, grand_let_child->id->token);

                fprintf(smt_file," ");
                child_tree(grand_let_child->id);
                grand_let_child = grand_let_child->next;
            }
        }*/
       if(child->id->child != NULL)
        {
            int i = 1;
            struct ast_child* grand_child = child->id->child;
            while(grand_child != NULL)
            {
                if(DEBUGMODE)
                    printf("[error] expr(%s) (child %d): %s\n", child->id->token, i, grand_child->id->token);

                fprintf(smt_file," ");
                child_tree(grand_child->id);
                grand_child = grand_child->next;
            }
        }
        fprintf(smt_file,")");
        fprintf(smt_file,")\n");
    }
    
    fclose(smt_file);
    return 0;
}

int type_check(struct ast* ast)
{
    int currToken = ast->ntoken;
    int term_needed = needs_term(currToken);
    int fla_needed = needs_fla(currToken);

    if(currToken == ERROR)
    {
        struct ast_child* child = ast->child;
        struct ast* node = child->id;

        if(DEBUGMODE)
            printf("type-check: %s fla_needed\n", ast->token);

        while(child != NULL)
        {
            if(!is_fla(node))
            {
                printf("'%s' needs fla, '%s' is NOT fla\n", ast->token, node->token);
                return 1;
            }
            
            child = child->next;
            if(child != NULL)
                node = child->id;
        }
        return 0;
    }

    if(currToken == FUNID)
    {
        struct ast* parent = ast->parent;
        struct ast_child *child = parent->child;
        struct definedFunctions *fun = get_fun(ast->token);
        char *type;
        while(1)
        {
            if(child->id->ntoken == FUNRET)
            {
                child = child->next;
                break;
            }
            child = child->next;
        }
        
        if(is_term(child->id) == -1)
            return 1;
        
        if(is_term(child->id) == 1)
            type = "Int";
        else 
            type = "Bool";

        if(strcmp(fun->retType, type) == 0)
            return 0;

        if(strcmp(fun->retType, type) != 0)
        {
            printf("'%s' function's return type is '%s', NOT '%s'\n", fun->funName, fun->retType, type);
            return 1;
        }
    }

    if(currToken == CALL)
    {
        if(check_fun_define(ast->token) != 0)
        {
            printf("'%s' is NOT defined before\n", ast->token);
            return 1;
        }

        struct definedFunctions *fun = get_fun(ast->token);
        struct ast_child* arg = ast->child;
        struct varList* var;
        char *call_type;
        int i;

        if(fun->argNum == 0 && arg == NULL)
                return 0;

        if(!fun->argNum == 0 && arg == NULL || fun->argNum == 0 && arg != NULL)
        {
            printf("Function '%s': error with argument(s) number\n", fun->funName);
            return 1;
        }

        for(i = 1; i <= fun->argNum; i++)
        {
            if(arg->next == NULL && i < fun->argNum)
            {
                printf("Function '%s': less argument to be passed ('%s' has %d args)\n", fun->funName, fun->funName, fun->argNum);
                return 1;
            }
            if(arg->next != NULL && i == fun->argNum)
            {
                printf("Function '%s': there are more argument to be passed ('%s' has %d args)\n", fun->funName, fun->funName, fun->argNum);
                return 1;
            }
            var = get_arg(fun->funName, i);

            if(is_term(arg->id) == -1)
                return 1;
            
            if(is_term(arg->id) == 1)
                call_type = "Int";
            else 
                call_type = "Bool";
            
            if(DEBUGMODE)
                printf("Fun arg no. %d: name: %s type: %s call: name: %s type: %s ntoken: %d\n", i, var->varName, var->varType, arg->id->token, call_type, arg->id->ntoken);
            
            if(strcmp(var->varType, call_type) != 0)
            {
                printf("Function '%s': error with argument type of '%s' (%s is %s)\n", fun->funName, var->varName, var->varName, var->varType);
                return 1;
            }
            
            arg = arg->next;
            
        }
        if(i == fun->argNum && arg == NULL)
            return 0;
    }

    if(DEBUGMODE)
            printf("type-check: %s term_needed: %d fla_needed: %d\n", ast->token, term_needed, fla_needed);
    if(term_needed == 1)
    {
        struct ast_child* child = ast->child;
        struct ast* node = child->id;

        if(DEBUGMODE)
            printf("type-check: %s term_needed\n", ast->token);

        while(child != NULL)
        {
            if(!is_term(node))
            {
                printf("'%s' needs term, '%s' is NOT term\n", ast->token, node->token);
                return 1;
            }
            
            child = child->next;
            if(child != NULL)
                node = child->id;
        }
        return 0;
    }

    if(fla_needed)
    {
        struct ast_child* child = ast->child;
        struct ast* node = child->id;

        if(DEBUGMODE)
            printf("type-check: %s fla_needed\n", ast->token);

        while(child != NULL)
        {
            if(!is_fla(node))
            {
                printf("'%s' needs fla, '%s' is NOT fla\n", ast->token, node->token);
                return 1;
            }
            
            child = child->next;
            if(child != NULL)
                node = child->id;
        }
        return 0;
    }
    
    if(currToken == IF)
    {
        if(DEBUGMODE)
            printf("In IF before declearartion\n");

        struct ast* fla;
        struct ast* term_fla_1;
        struct ast* term_fla_2;
        struct varList* var;
        struct ast_child* child = ast->child;
        fla = child->id;
        child = child->next;
        term_fla_1 = child->id;
        child = child->next;
        term_fla_2 = child->id;

        if(is_fla(fla) == -1 || is_term(term_fla_1) == -1 || is_term(term_fla_2) == -1)
            return 1;

        if(is_fla(fla) && is_term(term_fla_1) && is_term(term_fla_2) || is_fla(fla) && is_fla(term_fla_1) && is_fla(term_fla_2))
            return 0;

        if(!(is_fla(fla) && is_term(term_fla_1) && is_term(term_fla_2)) || !(is_fla(fla) && is_fla(term_fla_1) && is_fla(term_fla_2)))
        {
            printf("IF needs to be constructed like: 'if fla term term' or 'if fla fla fla'\n");
            return 1;
        }
    }
    return 0;
}

void printSymTable()
{
    struct definedFunctions *fun = funHead;
    struct varList *var = varHead;

    while(fun != NULL)
    {
        printf("Fun name: %s No. Args: %d Ret. Type: %s\n", fun->funName, fun->argNum, fun->retType);
        fun = fun->next;
    }
    while(var != NULL)
    {
        printf("Var name: %s Type: %s Fun Name: %s\n", var->varName, var->varType, var->funName);
        var = var->next;
    }
}

void freeSymTable()
{
    struct definedFunctions *fun = funHead;
    struct varList *var = varHead;

    while(funHead != NULL)
    {
        free(fun);
        funHead = funHead->next;
        fun = funHead;
    }

    while(varHead != NULL)
    {
        free(var);
        varHead = varHead->next;
        var = varHead;
    }
}

int main(void)
{
    int retval = yyparse();
    FILE *smt_file;
    
    if(retval == 0)
    {
        if(DEBUGMODE)  
            printf("first\n");

        funHead = NULL;
        varHead = NULL;
        struct definedFunctions *fun = (struct definedFunctions *)malloc(sizeof(struct definedFunctions));
        fun->funName = "get-int";
        fun->retType = "Int";
        fun->argNum = 0;
        insert_fun(fun);
        fun->funName = "get-bool";
        fun->retType = "Bool";
        fun->argNum = 0;
        insert_fun(fun);

        retval = visit_ast(fillSymTable);
    }

    if(retval == 0)
    {
        retval = visit_ast(type_check);
    }

    if(retval == 0)
    {
        smt_file = fopen("converted.smt","w");

        visit_ast(calc_get_int);
        visit_ast(calc_calls);

        if(DEBUGMODE)
            printf("get-int total: %d\n", get_int_total);

        if(smt_file == NULL)
        {
            printf("Error!");   
            exit(1);             
        }

        for(int i = 0; i < get_int_total ; i++)
        {
            fprintf(smt_file,"(declare-var get_int_out_%d Int)\n",i);
        }
        fclose(smt_file);
        
        smt_file = fopen("converted.smt","a");
        retval = visit_ast(conert_to_smt);
        smt_file = fopen("converted.smt","a");
        fprintf(smt_file,"(check-sat)\n");
        fclose(smt_file);

        if(retval == 0)
            printf("The program is converted successfully!\n");
    }
    else 
        printf("The program couldn't be converted! There are semantic errors\n");
    

    if(DEBUGMODE) 
        printSymTable();

    print_ast();
    freeSymTable();
    free_ast();

    return retval;
}
