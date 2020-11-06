#include "comp.h"

struct block{
    char* name;
    int block_no;
    struct assignment* assignment_head;
    int br1_no;
    int br2_no;
    char* header_name;
    struct block* prev;
    struct block* next;
};

struct bb{
    int bb_no;
    int bb_id;
    struct bb* next;
};
struct br{
    int br_no;
    int br_id;
    struct br* next;
};

struct assignment{
    char* line;
    int block_no;
    struct assignment* prev;
    struct assignment* next;
};

struct variables{
    char* name;
    int reg;
    int define_id;
    int let_id;
    struct variables* prev;
    struct variables* next;
};

struct block* block_head;
struct block* last_block;

struct assignment* last_assignment;

struct variables* variables_head;
struct variables* lastVarID;

struct bb* bb_head;
struct br* br_head;

struct bb* last_bb;
struct br* last_br;

int last_define_fun_id = 0;
int bb_num = 0;				
int last_block_no = 0;
int print_check = 0;
int eval_check = 0;
int last_reg;
int block_end = -2;
char* line;
char* header; // holds header name for blocks
int block_ended = 0;
int no_if = 0;
struct definedFunctions *fun;

void free_everything()
{
    struct block* block = block_head;
    struct assignment* assignment_head = block_head->assignment_head;
    struct assignment* assignment = block_head->assignment_head;
    struct variables* variables = variables_head;

    while(block_head != NULL)
    {
        while(assignment_head != NULL)
        {
            free(assignment);
            assignment_head = assignment_head->next;
            assignment = assignment_head;
        }
        free(block);
        block_head = block_head->next;
        block = block_head;

        if(block_head != NULL)
            assignment_head = block_head->assignment_head;
        
    }

    while(variables_head != NULL)
    {
        free(variables);
        variables_head = variables_head->next;
        variables = variables_head;
    }
}
void instructions_to_file()
{
    FILE* instructions = fopen("instructions.s", "w");
    struct block* block = block_head;
    struct assignment *assignment = block->assignment_head;

    while(block != NULL)
    {
        if(DEBUGMODE)
            printf("\t\tBLOCK-%d ( %s ):\n", block->block_no, block->name);
        while(assignment != NULL)
        {
            fprintf(instructions, "%s\n", assignment->line);

            if(DEBUGMODE)
                printf("%s\n", assignment->line);

            assignment = assignment->next;
        }
        
        block = block->next;
        //if(block->block_no == -2)
            //break;
        if(block != NULL)
            assignment = block->assignment_head;
    }
    fclose(instructions);
}

void print_blocks()
{
    struct block* block = block_head;
    struct assignment *assignment = block->assignment_head;

    while(block != NULL)
    {
        
        printf("\t\tBLOCK-%d ( %s ):\n", block->block_no, block->name);
        while(assignment != NULL)
        {
            printf("%s\n", assignment->line);
            assignment = assignment->next;
        }
        
        block = block->next;
        //if(block->block_no == -2)
            //break;
        if(block != NULL)
            assignment = block->assignment_head;
    }
}

void cfg_file(struct block* block) {
	FILE* fp = fopen("cfg.dot", "w");
	fprintf(fp, "digraph print {\n ");
	while(block != NULL) {
		//node
        if(block->block_no == 0)
		    fprintf(fp, "%d [label=\"entry(%s)\", fontname=\"monospace\"];\n ", block->block_no, block->header_name);
        else if(block_ended == 1)
        {
            fprintf(fp, "%d [label=\"entry(%s)\", fontname=\"monospace\"];\n ", block->block_no, block->header_name);
            block_ended = 0;
        }
        else
            fprintf(fp, "%d [label=\"%s\", fontname=\"monospace\"];\n ", block->block_no, block->name);
		//edges
		if (block->br1_no != -1)
			fprintf(fp, "%d->%d\n", block->block_no, block->br1_no);
		if (block->br2_no != -1)
			fprintf(fp, "%d->%d\n", block->block_no, block->br2_no);

        if (block->block_no == -2)
            block_ended = 1;

		block = block->next;
	}
	fprintf(fp, "}\n ");
	fclose(fp);
}

int check_if_is_parent(struct ast* ast)
{
    struct ast* parent = ast->parent;

    if(parent->ntoken == IF)
            return 1;

    while(parent->parent != NULL)
    {
        //printf("parent: %s\n", parent->token);
        if(parent->ntoken == IF)
            return 1;
        parent = parent->parent;
    }
    return 0;
}

struct ast* get_first_parent(struct ast* ast)
{
    struct ast* parent = ast->parent;

    if(parent == NULL && (ast->ntoken == DEFINEFUN || ast->ntoken == PRINT || EVAL))
        return ast;

    while(parent->parent != NULL)
    {
        //printf("parent: %s\n", parent->token);
        parent = parent->parent;
    }
    return parent;
}

int get_define_fun_id(struct ast* ast)
{
    struct ast* parent = ast->parent;

    if(parent == NULL && (ast->ntoken == DEFINEFUN || ast->ntoken == PRINT || ast->ntoken == EVAL))
        return ast->id;

    while(parent->parent != NULL)
    {
        //printf("parent: %s\n", parent->token);
        parent = parent->parent;
    }
    return parent->id;
}

int test_define_fun(struct ast* ast)
{
    if(ast->ntoken == DEFINEFUN)
    {
        printf("define-fun no: %d fun name: %s\n", ast->id, ast->child->id->token);
    }
}

int add_block(char* name, int block_no, int br1_no, int br2_no, char* header_name)
{
    if(block_head == NULL)
    {
        block_head = (struct block *)malloc(sizeof(struct block));
        block_head->name = name;
        block_head->block_no = block_no;
        block_head->br1_no = br1_no;
        block_head->br2_no = br2_no;
        block_head->assignment_head = NULL;
        block_head->next = NULL;
        block_head->header_name = header_name;
        last_block = block_head;
    }
    else
    {
        struct block* curr = (struct block *)malloc(sizeof(struct block));
        last_block->next = curr;
        curr->name = name;
        curr->block_no = block_no;
        curr->br1_no = br1_no;
        curr->br2_no = br2_no;
        curr->assignment_head = NULL;
        curr->next = NULL;
        curr->header_name = header_name;
        last_block = curr;
    }
}

struct block* get_block(int block_no)
{
    struct block* tmp = block_head;

    while(tmp != NULL)
    {
        if(tmp->block_no == block_no)
            return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

int add_assignment(char* assignment, int block_no)
{
    struct block* block = get_block(block_no);

    if(DEBUGMODE)
        printf("block no: %d\n", block->block_no);
    if(block->assignment_head == NULL)
    {
        if(DEBUGMODE)
            printf("here: add_assignment\n");

        struct assignment* head = (struct assignment *)malloc(sizeof(struct assignment));
        block->assignment_head = head;
        //strcpy(head->line, assignment);
        head->line = assignment;
        head->block_no = block_no;
        head->next = NULL;
        last_assignment = head;
        if(DEBUGMODE)
            printf("assignment: ( %s ) block no: %d\n", block->assignment_head->line, block->block_no);
    }
    else
    {
        if(DEBUGMODE)
            printf("Not head here: add_assignment\n");
        struct assignment* curr = (struct assignment *)malloc(sizeof(struct assignment));
        last_assignment->next = curr;
        //strcpy(curr->line, assignment);
        curr->line = assignment;
        curr->block_no = block_no;
        curr->next = NULL;
        last_assignment = curr;

        if(DEBUGMODE)
            printf("assignment: ( %s ) block no: %d\n", curr->line, block->block_no);
    }
}
int add_bb(int bb_no, int bb_id)
{
    if(bb_head == NULL)
    {
        bb_head = (struct bb *)malloc(sizeof(struct bb));
        bb_head->bb_no = bb_no;
        bb_head->bb_id = bb_id;
        bb_head->next = NULL;
        last_bb = bb_head;
    }
    else
    {
        struct bb* curr = (struct bb *)malloc(sizeof(struct bb));
        last_bb->next = curr;
        curr->bb_no = bb_no;
        curr->bb_id = bb_id;
        curr->next = NULL;
        last_bb = curr;
    }
}

int add_br(int br_no, int br_id)
{
    if(br_head == NULL)
    {
        br_head = (struct br *)malloc(sizeof(struct br));
        br_head->br_no = br_no;
        br_head->br_id = br_id;
        br_head->next = NULL;
        last_br = br_head;
    }
    else
    {
        struct br* curr = (struct br *)malloc(sizeof(struct br));
        last_br->next = curr;
        curr->br_no = br_no;
        curr->br_id = br_id;
        curr->next = NULL;
        last_br = curr;
    }
}

struct bb* get_bb_no(int id)
{
    struct bb* tmp = bb_head;
    while(tmp != NULL)
    {
        if(tmp->bb_id == id )
            return tmp;
        else
            tmp = tmp->next;
    } 
    return NULL;
}

struct br* get_br_no(int id)
{
    struct br* tmp = br_head;
    while(tmp != NULL)
    {
        if(tmp->br_id == id)
            return tmp;
        else
            tmp = tmp->next;
    } 
    return NULL;
}

int get_let_id(struct ast* ast)
{
    struct ast* parent = ast->parent;

    while(parent != NULL)
    {
        if(parent->ntoken == LET)
        {
            if(strcmp(parent->child->id->token, ast->token) == 0)
                return parent->id;
        }
        parent = parent->next;
    }
    return 0;
}

int get_reg(char* name, int define_id, int let_id)
{
    struct variables* tmp = variables_head;
    while(variables_head != NULL)
    {
        if(strcmp(name, tmp->name) == 0 && tmp->define_id == define_id && tmp->let_id == let_id)
            return tmp->reg;
        else
            tmp = tmp->next;
    }
}

int assign_a_reg(char* name, int reg, int define_id, int let_id)
{
    if(variables_head == NULL)
    {
        variables_head = (struct variables *)malloc(sizeof(struct variables));
        variables_head->name = name;
        variables_head->reg = reg;
        variables_head->define_id = define_id;
        variables_head->let_id = let_id;
        variables_head->next = NULL;
        lastVarID = variables_head;
    }
    else
    {
        struct variables *curr = (struct variables *)malloc(sizeof(struct variables));
        lastVarID->next = curr;
        curr->name = name;
        curr->reg = reg;
        curr->define_id = define_id;
        curr->let_id = let_id;
        curr->next = NULL;
        lastVarID = curr;
    }
}
  void rec_descend(struct ast* node)	
  {
  	if(node->ntoken == IF) 			
  	{
        no_if = 1;
		int bb_then_num= bb_num +1;
		int bb_else_num= bb_num +2;
		int bb_join_num= bb_num +3;
		bb_num += 3;

        char* block_name = (char*)malloc ( 50 * sizeof (char));
        sprintf(block_name, "bb%d", last_block_no);

        if(DEBUGMODE)
        {
            printf("block name: %s\n", block_name);

		    printf("          <<<<<  NODE ID %d  >>>>>\n",get_child(node, 1)->id);	
  		    printf(" br v<cond> bb%d, bb%d\n",bb_then_num,bb_else_num);
  		    printf("bb%d:\n",bb_then_num);
        }
        
        add_bb(bb_then_num, get_child(node, 1)->id);
        add_block(block_name, last_block_no, bb_then_num, bb_else_num, header);
        //add_bb(bb_else_num, get_child(node, 1)->id);

        last_block_no = bb_then_num;

  		rec_descend(get_child(node,2));	
  		
        block_name = (char*)malloc ( 50 * sizeof (char));
        sprintf(block_name, "bb%d", last_block_no);

        if(DEBUGMODE)
            printf("block name: %s\n", block_name);

        add_block(block_name, last_block_no, bb_join_num, -1, header);

        if(DEBUGMODE)
        {
            printf("          <<<<<  NODE ID %d  >>>>>\n",get_child(node, 2)->id);	
  		    printf(" br bb%d\n",bb_join_num );
  		    printf("bb%d:\n",bb_else_num);
        }
  		
        add_br(bb_join_num, get_child(node, 2)->id);
        add_bb(bb_else_num, get_child(node, 2)->id);

        last_block_no = bb_else_num;

        block_name = (char*)malloc ( 50 * sizeof (char));
        sprintf(block_name, "bb%d", last_block_no);

        if(DEBUGMODE)
            printf("block name: %s\n", block_name);

        add_block(block_name, last_block_no, bb_join_num, -1, header);

  		rec_descend(get_child(node,3));	
  		
        if(DEBUGMODE)
        {
            printf("          <<<<<  NODE ID %d  >>>>>\n",get_child(node, 3)->id);	
  		    printf(" br bb%d\n",bb_join_num );
  		    printf("bb%d:\n",bb_join_num);
        }
  		
        add_br(bb_join_num, get_child(node, 3)->id);
        add_bb(bb_join_num, get_child(node, 3)->id);
        
        last_block_no = bb_join_num;

        block_name = (char*)malloc ( 50 * sizeof (char));
        sprintf(block_name, "bb%d", last_block_no);

        if(DEBUGMODE)
            printf("block name: %s\n", block_name);

        if(check_if_is_parent(node) == 0)
        {
            add_block(block_name, last_block_no, block_end, -1, header);
            add_block("exit", block_end, -1, -1, header);
        } 
  	}
  	else
  	{
  		struct ast_child* tmp = node->child;
  		while(tmp != NULL)
  		{
  			rec_descend(tmp->id);
  			tmp = tmp->next;
  		}
  	}
  }
  int br_struct(struct ast* node)	 	
  {
  	if(node->ntoken == FUNID)			
  	{
        header = node->token;

        if(DEBUGMODE)
  		    printf("= = function %s\n", node->token);

  		rec_descend(node->parent);
        
        if(no_if == 0)
        {
            char* block_name = (char*)malloc ( 50 * sizeof (char));
            sprintf(block_name, "bb%d", last_block_no);
            add_block(block_name, last_block_no, block_end, -1, header);
            add_block("exit", block_end, -1, -1, header);
        }
        bb_num++;
        last_block_no = bb_num;
        block_end--;
        no_if = 0;
  	}
    if(node->ntoken == PRINT || node->ntoken == EVAL)	
    {
        header = node->token;
        rec_descend(node);
        if(no_if == 0)
        {
            char* block_name = (char*)malloc ( 50 * sizeof (char));
            sprintf(block_name, "bb%d", last_block_no);
            add_block(block_name, last_block_no, block_end, -1, header);
            add_block("exit", block_end, -1, -1, header);
        }
        bb_num++;
        last_block_no = bb_num;
        block_end--;
        no_if = 0;
    }
  	return 0;
  	
  }
  
//int first = 1;

 int translate (struct ast* node)
{
    //rec_descend(node);
    //printf("%d: ", first);
    //first++;
    if(last_define_fun_id == 0)
    {
        //printf("before get_define\n");
        last_define_fun_id = get_define_fun_id(node);
        //printf("after get_define\n");
    }
    
    if(last_define_fun_id == get_define_fun_id(node))
    {
        if(get_first_parent(node)->ntoken == PRINT && print_check == 0)
        {
            if(DEBUGMODE)
                printf("FIRST PRINT: %s\n", node->token);

            add_assignment("\n\nPRINT", last_block_no);

            if(DEBUGMODE)
                printf("PRINT\n");

            add_assignment("entry:", last_block_no);

            if(DEBUGMODE)
                printf("entry:\n");

            print_check = 1;
        }

        if(get_first_parent(node)->ntoken == EVAL && eval_check == 0)
        {
            if(DEBUGMODE)
                printf("FIRST EVAL: %s\n", node->token);

            add_assignment("\nEVAL", last_block_no);

            if(DEBUGMODE)
                printf("EVAL\n");

            add_assignment("entry:", last_block_no);

            if(DEBUGMODE)
                printf("entry:\n");

            eval_check = 1;
        }

        if(node->ntoken == FUNID) 
        {
            if(DEBUGMODE)
            {
                printf("= = function %s\n", node->token);
                printf("entry:\n");
            }
            

            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "\nfunction %s", node->token);
            add_assignment(line, last_block_no);
            add_assignment("entry:", last_block_no);

            fun = get_fun(node->token);

            last_define_fun_id = node->parent->id;

            struct ast_child* tmp = node->parent->child;
            tmp = tmp->next;
            if(fun->argNum != 0)
            {
                
                for(int i = 1; i <= fun->argNum; i++)
                {   
                    //char reg_no[4];
                    //line = malloc(strlen(line));
                    line = (char*)malloc ( 50 * sizeof (char));
                    sprintf(line, "v%d := a%d", tmp->id->id, i);
                    add_assignment(line, last_block_no);

                    if(DEBUGMODE)
                        printf("v%d := a%d {arg: %s}\n", tmp->id->id, i, tmp->id->token);
                    //sprintf(reg_no, "v%d", tmp->id->id);
                    //printf("reg: %s\n", reg_no);
                    //printf("define-id: %d\n", node->parent->id);
                    assign_a_reg(tmp->id->token, tmp->id->id, node->parent->id, 0);
                    tmp = tmp->next->next;
                }
            }
        }
	if(node->ntoken == GEQ ||node->ntoken == LEQ|| node->ntoken == EQUAL || node->ntoken == MINUS ||
	   node->ntoken == LESS ||node->ntoken == GREATER || node->ntoken == DIV || node->ntoken == MOD)
	{
		if (node->parent->ntoken == IF &&
		   (get_child(node->parent, 2)==node || get_child(node->parent, 3) == node))
           {
               if(node->parent->ntoken == LET && get_child(node->parent, 2) == node)
           {
               line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d %s v%d", node->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                add_assignment(line, last_block_no);

                if(DEBUGMODE)
                    printf(" v%d := v%d %s v%d\n",node->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                
                last_reg = get_child(node->parent,1)->id;
                //line = malloc(strlen(line));
               
                line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d", get_child(node->parent,1)->id, node->id);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := v%d\n", get_child(node->parent,1)->id, node->id);
                
                last_reg = get_child(node->parent,1)->id;

           }
		    else if(node->parent->ntoken == LET && get_child(node->parent, 3) == node)
           {
               line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d %s v%d", node->parent->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := v%d %s v%d\n",node->parent->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                
                last_reg = node->parent->id;
                //line = malloc(strlen(line));
                last_reg = node->parent->id;

           }	
           else 
           {
               //line = malloc(strlen(line));
                line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d %s v%d", node->parent->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                add_assignment(line, last_block_no);

                if(DEBUGMODE)
                    printf(" v%d := v%d %s v%d\n",node->parent->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                
                last_reg = node->parent->id;
           }
           }
			else if(node->parent->ntoken == LET && get_child(node->parent, 2) == node)
           {
               line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d %s v%d", node->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                add_assignment(line, last_block_no);

                if(DEBUGMODE)
                    printf(" v%d := v%d %s v%d\n",node->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                
                last_reg = get_child(node->parent,1)->id;
                //line = malloc(strlen(line));
               
                line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d", get_child(node->parent,1)->id, node->id);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := v%d\n", get_child(node->parent,1)->id, node->id);
                
                last_reg = get_child(node->parent,1)->id;

           }
		    else if(node->parent->ntoken == LET && get_child(node->parent, 3) == node)
           {
               line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d %s v%d", node->parent->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := v%d %s v%d\n",node->parent->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                
                last_reg = node->parent->id;
                //line = malloc(strlen(line));
                last_reg = node->parent->id;

           }	
		else
        {
            int var1;
            int var2;
            if(get_child(node,1)->ntoken == VARID)
            {
                var1 = get_reg(get_child(node,1)->token, last_define_fun_id, get_let_id(get_child(node,1)));
            }
            else
            {
                var1 = get_child(node,1)->id;
            }
                
            if(get_child(node,2)->ntoken == VARID)
            {
                var2 = get_reg(get_child(node,2)->token, node->parent->parent->id, get_let_id(get_child(node,2)));
            }
            else
            {
                var2 =  get_child(node,2)->id;
            }

            //line = malloc(strlen(line));
            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "v%d := v%d %s v%d",node->id, var1, node->token, var2);
            add_assignment(line, last_block_no);
            
            if(DEBUGMODE)
                printf(" v%d := v%d %s v%d\n",node->id, var1, node->token, var2);
            
            last_reg = node->id;
        }
    }
    if(node->ntoken == PLUS || node->ntoken == AND || node->ntoken == MULT || node->ntoken == OR)
	{
		if ((node->parent->ntoken == IF &&
		   (get_child(node->parent, 2)==node || get_child(node->parent, 3) == node)))
           {
                line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d %s v%d", node->parent->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := v%d %s v%d\n",node->parent->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                
                last_reg = node->parent->id;
                //line = malloc(strlen(line));
                if(get_child(node,3) != NULL)
                {
                    int i = 3;
                    while(get_child(node,i) != NULL)
                    {
                        line = (char*)malloc ( 50 * sizeof (char));
                        sprintf(line, "v%d := v%d %s v%d", node->parent->id, node->parent->id, node->token, get_child(node,i)->id);
                        add_assignment(line, last_block_no);
                        
                        if(DEBUGMODE)
                            printf(" v%d := v%d %s v%d\n",node->parent->id, node->parent->id, node->token, get_child(node,i)->id);
                        
                        i++;
                    }
                }  
           }
           else if(node->parent->ntoken == LET && get_child(node->parent, 2) == node)
           {
               line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d %s v%d", node->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := v%d %s v%d\n",node->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                
                last_reg = get_child(node->parent,1)->id;
                //line = malloc(strlen(line));
                if(get_child(node,3) != NULL)
                {
                    int i = 3;
                    while(get_child(node,i) != NULL)
                    {
                        line = (char*)malloc ( 50 * sizeof (char));
                        sprintf(line, "v%d := v%d %s v%d", node->id, node->id, node->token, get_child(node,i)->id);
                        add_assignment(line, last_block_no);
                        
                        if(DEBUGMODE)
                            printf(" v%d := v%d %s v%d\n",node->id, node->id, node->token, get_child(node,i)->id);
                        i++;
                    }
                }
                line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d", get_child(node->parent,1)->id, node->id);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := v%d\n", get_child(node->parent,1)->id, node->id);
                last_reg = get_child(node->parent,1)->id;

           }
		    else if(node->parent->ntoken == LET && get_child(node->parent, 3) == node)
           {
               line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d %s v%d", node->parent->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := v%d %s v%d\n",node->parent->id, get_child(node,1)->id, node->token, get_child(node,2)->id);
                
                last_reg = node->parent->id;
                //line = malloc(strlen(line));
                if(get_child(node,3) != NULL)
                {
                    int i = 3;
                    while(get_child(node,i) != NULL)
                    {
                        line = (char*)malloc ( 50 * sizeof (char));
                        sprintf(line, "v%d := v%d %s v%d", node->parent->id, node->parent->id, node->token, get_child(node,i)->id);
                        add_assignment(line, last_block_no);
                        
                        if(DEBUGMODE)
                            printf(" v%d := v%d %s v%d\n",node->parent->id, node->parent->id, node->token, get_child(node,i)->id);
                        
                        i++;
                    }
                }
                last_reg = node->parent->id;

           }
		else
        {
            int var1;
            int var2;
            if(get_child(node,1)->ntoken == VARID)
            {
                var1 = get_reg(get_child(node,1)->token, last_define_fun_id, get_let_id(get_child(node,1)));
            }
            else
            {
                var1 = get_child(node,1)->id;
            }
                
            if(get_child(node,2)->ntoken == VARID)
            {
                var2 = get_reg(get_child(node,2)->token, node->parent->parent->id, get_let_id(get_child(node,2)));
            }
            else
            {
                var2 =  get_child(node,2)->id;
            }

            //line = malloc(strlen(line));
            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "v%d := v%d %s v%d",node->id, var1, node->token, var2);
            add_assignment(line, last_block_no);
            
            if(DEBUGMODE)
                printf(" v%d := v%d %s v%d\n",node->id, var1, node->token, var2);
            
            last_reg = node->id;

            if(get_child(node,3) != NULL)
                {
                    int i = 3;
                    while(get_child(node,i) != NULL)
                    {
                        line = (char*)malloc ( 50 * sizeof (char));
                        sprintf(line, "v%d := v%d %s v%d", node->id, node->id, node->token, get_child(node,i)->id);
                        add_assignment(line, last_block_no);
                        
                        if(DEBUGMODE)
                            printf(" v%d := v%d %s v%d\n",node->id, node->id, node->token, get_child(node,i)->id);
                        
                        i++;
                    }
                }  
        }

	}

	if(node->ntoken == NOT)
    {
         int var1;
        //line = malloc(strlen(line));
        if(get_child(node,1)->ntoken == VARID)
        {
            var1 = get_reg(get_child(node,1)->token, last_define_fun_id, get_let_id(get_child(node,1)));
        }
        else
            var1 = get_child(node,1)->id;

        line = (char*)malloc ( 50 * sizeof (char));
        sprintf(line, "v%d := %s v%d",node->id, node->token, var1);
        add_assignment(line, last_block_no);
        
        if(DEBUGMODE)
            printf(" v%d := %s v%d\n",node->id, node->token, var1);

        last_reg = node->id;
    }
	
    if(node->parent != NULL && node->parent->ntoken != DEFINEFUN && node->ntoken == VARID && node->parent->ntoken == IF && get_child(node->parent, 1) != node)
    {
        if(node->parent->ntoken == IF && (get_child(node->parent, 2)==node || get_child(node->parent, 3) == node))
           {
                int var1 = get_reg(node->token, last_define_fun_id, get_let_id(node));
                line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d",node->parent->id, var1);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := v%d\n",node->parent->id, var1);
                last_reg = node->parent->id;
           }
           else
           {
               if(get_let_id(node) != 0)
               {
                    assign_a_reg(node->token, find_ast_node(get_let_id(node))->child->id->id, get_define_fun_id(node), get_let_id(node));
                    line = (char*)malloc ( 50 * sizeof (char));
                    sprintf(line, "v%d := v%d",node->id, find_ast_node(get_let_id(node))->child->id->id);
                    add_assignment(line, last_block_no);
                    
                    if(DEBUGMODE)
                        printf(" v%d := v%d\n",node->id, find_ast_node(get_let_id(node))->child->id->id);
                    
                    last_reg = node->id;
               }
               else
               {
                   int var1 = get_reg(node->token, last_define_fun_id, get_let_id(node));
                    line = (char*)malloc ( 50 * sizeof (char));
                    sprintf(line, "v%d := v%d",node->id, var1);
                    add_assignment(line, last_block_no);
                    
                    if(DEBUGMODE)
                        printf(" v%d := v%d\n",node->id, var1);
                    
                    last_reg = node->id;
               }
           }
        
    }


	if(node->ntoken == NUMBER && node->parent->ntoken != CALL)
	{
        if(DEBUGMODE)
            printf("number: %s parent: %s\n", node->token, node->parent->token);
		
        if (node->parent->ntoken == IF &&
		   (get_child(node->parent, 2)==node || get_child(node->parent, 3) == node))
           {
                //line = malloc(strlen(line));
                line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := %s",node->parent->id,node->token);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := %s\n",node->parent->id,node->token);
                
                last_reg = node->parent->id;
           }
		else if(node->parent->ntoken == LET && get_child(node->parent, 2) == node)
           {
               if(DEBUGMODE)
                    printf(" v%d := %s\n",node->id,node->token);
                //line = malloc(strlen(line));
                line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := %s",node->id,node->token);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := %s\n",node->id,node->token);
                
                last_reg = node->id;

                line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d", get_child(node->parent,1)->id, node->id);
                add_assignment(line, last_block_no);
                
                if(DEBUGMODE)
                    printf(" v%d := v%d\n", get_child(node->parent,1)->id, node->id);
                
                last_reg = get_child(node->parent,1)->id;

           }
		    	
		else //if(node->parent->ntoken != CALL)
        {
            if(DEBUGMODE)
                printf(" v%d := %s\n",node->id,node->token);
            //line = malloc(strlen(line));
            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "v%d := %s",node->id,node->token);
            add_assignment(line, last_block_no);
            
            if(DEBUGMODE)
                printf(" v%d := %s\n",node->id,node->token);
            
            last_reg = node->id;
        }
			
	}

    if(node->parent != NULL && node->parent->ntoken == IF && get_child(node->parent, 1)->id == node->id && get_bb_no(node->id) != NULL && get_bb_no(node->id)->bb_id == node->id)
    {
        //printf("if 1: node id: %d\n", node->id);
        struct bb* tmp = get_bb_no(node->id);
        if(tmp != NULL)
        {
            int var1;
            if(node->ntoken == VARID)
                var1 = get_reg(node->token, last_define_fun_id, get_let_id(node));
            else 
                var1 = node->id;

            if(DEBUGMODE)
                printf("br v%d bb%d bb%d\n", var1, tmp->bb_no, tmp->bb_no+1);
            //line = malloc(strlen(line));
            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "br v%d bb%d bb%d", var1, tmp->bb_no, tmp->bb_no+1);
            add_assignment(line, last_block_no);

            last_block_no = tmp->bb_no;

            if(DEBUGMODE)
                printf("bb%d:\n", tmp->bb_no);
            //line = malloc(strlen(line));
            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "bb%d:", tmp->bb_no);
            add_assignment(line, last_block_no);
        }
        
    }

     if(node->parent != NULL && node->parent->ntoken == IF && get_child(node->parent, 2)->id == node->id && get_br_no(node->id) != NULL && get_br_no(node->id)->br_id == node->id)
    {
        //printf("if 2: node id: %d\n", node->id);
        struct br* br = get_br_no(node->id);
        struct bb* bb = get_bb_no(node->id);

        if(br != NULL && bb != NULL)
        {
            if(DEBUGMODE)
                printf("br bb%d\n", br->br_no);
            //line = malloc(strlen(line));
            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "br bb%d", br->br_no);
            add_assignment(line, last_block_no);

            last_block_no = bb->bb_no;

            if(DEBUGMODE)
                printf("bb%d:\n", bb->bb_no);
            //line = malloc(strlen(line));
            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "bb%d:", bb->bb_no);
            add_assignment(line, last_block_no);
            
        }
        
    }

     if(node->parent != NULL && node->parent->ntoken == IF && get_child(node->parent, 3)->id == node->id && get_br_no(node->id) != NULL && get_br_no(node->id)->br_id == node->id)
    {
        printf("if 3: node id: %d __ token: %s\n", node->id, node->token);
        struct br* br = get_br_no(node->id);
        struct bb* bb = get_bb_no(node->id);

        if(br != NULL && bb != NULL)
        {
            if(DEBUGMODE)
                printf("br bb%d\n", br->br_no);
            //line = malloc(strlen(line));
            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "br bb%d", br->br_no);
            add_assignment(line, last_block_no);

            last_block_no = bb->bb_no;

            if(DEBUGMODE)
                printf("bb%d:\n", bb->bb_no);
            //line = malloc(strlen(line));
            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "bb%d:", bb->bb_no);
            add_assignment(line, last_block_no);
            
            if(get_block(last_block_no)->br1_no > 0)
            {
                line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := v%d", node->parent->parent->id, last_reg);
                add_assignment(line, last_block_no);
            }
            
        }
        
    }

	if(node->ntoken == CALL) 
	{
        fun = get_fun(node->token);
        int var1;
        struct ast_child* tmp = node->child;
        if(fun->argNum != 0)
        {
            for(int i = 1; i <= fun->argNum; i++)
            {   
                //char reg_no[4];
                if(tmp->id->ntoken == VARID)
                {
                    var1 = get_reg(tmp->id->token, last_define_fun_id, get_let_id(tmp->id));
                    line = (char*)malloc ( 50 * sizeof (char));
                    sprintf(line, "a%d := v%d", i, var1);
                    add_assignment(line, last_block_no);
                }
                else
                {
                    line = (char*)malloc ( 50 * sizeof (char));
                    sprintf(line, "a%d := %s", i, tmp->id->token);
                    add_assignment(line, last_block_no);
                }
                if(DEBUGMODE) 
                    printf("a%d := v%d {arg: %s}\n", i, var1, tmp->id->token);

                //line = malloc(strlen(line));
                
                //sprintf(reg_no, "v%d", tmp->id->id);
                //printf("reg: %s\n", reg_no);
                tmp = tmp->next;
            }
        }

        if(DEBUGMODE)
		    printf(" call %s\n",node->token);
        line = (char*)malloc ( 50 * sizeof (char));
        sprintf(line, "call %s",node->token);
        add_assignment(line, last_block_no);

		if (node->parent->ntoken == IF &&
		   (get_child(node->parent, 2)==node || get_child(node->parent, 3) == node))
           {
                last_reg = node->parent->id;

                if(DEBUGMODE)
                    printf(" v%d := rv\n",node->parent->id);  
                //line = malloc(strlen(line));
                line = (char*)malloc ( 50 * sizeof (char));
                sprintf(line, "v%d := rv", node->parent->id);
                add_assignment(line, last_block_no);
           }
									
		else
        {
            last_reg = node->id;

            if(DEBUGMODE)
                printf(" v%d := rv\n",node->id); 

            //line = malloc(strlen(line));
            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "v%d := rv", node->id);
            add_assignment(line, last_block_no);
        }
			 					
	}
    }
    else 
    {
        //printf("token: %s\n", node->token);
        if(find_ast_node(last_define_fun_id)->ntoken == PRINT)
        {
            if(DEBUGMODE)
                printf("print v%d\n", last_reg);

            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "print v%d", last_reg);
            add_assignment(line, last_block_no);
        } 
        else if(find_ast_node(last_define_fun_id)->ntoken == EVAL)
        {
            if(DEBUGMODE)
                printf("eval v%d\n", last_reg);

            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "eval v%d", last_reg);
            add_assignment(line, last_block_no);
        }
        else
        {
            if(DEBUGMODE)
                printf("rv := v%d\n", last_reg);
            //line = malloc(strlen(line));
            line = (char*)malloc ( 50 * sizeof (char));
            sprintf(line, "rv := v%d", last_reg);
            add_assignment(line, last_block_no);
        }

        if(DEBUGMODE)
            printf("br exit\n");
        //line = malloc(strlen(line));
        line = (char*)malloc ( 50 * sizeof (char));
        sprintf(line, "br exit");
        add_assignment(line, last_block_no);
        last_define_fun_id = get_define_fun_id(node);
        last_block_no++;
        translate(node);
    }
	
	return 0;
}


int main(void)
{
    int retval = yyparse();

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
        printf("Correct program\n");
    else 
        printf("Incorrect program\n");

    print_ast();
    
    if(DEBUGMODE)
        visit_ast(test_define_fun);
    visit_ast(br_struct);

    last_block_no = 0;

    visit_ast(translate);
    cfg_file(block_head);
    instructions_to_file();
    //line = malloc(strlen(line));
    
    /*while(variables_head != NULL)
    {
        printf("var: %s reg: %d\n", variables_head->name, variables_head->reg);
        variables_head = variables_head->next;
    }*/
    if(DEBUGMODE)
        print_blocks();
    
    if(DEBUGMODE)
    {
        struct block* tmp = block_head;
        while(tmp != NULL)
        {
            printf("block name: %s block no: %d br1: %d br2: %d\n", tmp->name, tmp->block_no, tmp->br1_no, tmp->br2_no);
            tmp = tmp->next;
        }
    }

    free_everything();

    if(DEBUGMODE) 
        printSymTable();

    print_ast();
    freeSymTable();
    free_ast();

    return retval;
}
