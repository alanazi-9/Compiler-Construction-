#include "y.tab.h"
#include "containers.h"
int yyparse();

// symbol table
struct node_fun_str* fun_r = NULL;
struct node_fun_str* fun_t = NULL;
struct node_var_str* vars_r = NULL;
struct node_var_str* vars_t = NULL;

// br structure
struct node_istr* ifun_r = NULL;
struct node_istr* ifun_t = NULL;


// reg_array
//int available_reg[6];
struct available_regs_list{
  int reg_id;
  int reg_num;
  bool is_available;
  struct regs_list* next;
};

struct available_regs_list* av_regs_root = NULL;
struct available_regs_list* last_av_reg = NULL;

// semantic checks

bool is_term (struct ast* node){
  int t = node->ntoken;
  if (t == CONST || t == PLUS || t == MINUS || t == MULT || t == DIV || t == MOD) return true;
  if (t == VARID) {
    struct node_var_str* tmp = find_var_str(node->id, node->token, vars_r);
    if (tmp != NULL && INTID == tmp->type) return true;
  }
  if (t == CALL) {
    struct node_fun_str* tmp = find_fun_str(node->token, fun_r);
    if (tmp != NULL && INT == tmp->type) return true;
  }
  if (t == IF) return is_term (get_child(node, 2));
  if (t == LET) return is_term (get_child(node, 3));
  return false;
}

bool is_fla (struct ast* node){
  int t = node->ntoken;
  if (t == TRUE || t == FALSE || t == EQ || t == LE || t == LT || t == GT || t == GE || t == NOT || t == AND || t == OR) return true;
  if (t == VARID) {
    struct node_var_str* tmp = find_var_str(node->id, node->token, vars_r);
    if (tmp != NULL && BOOLID == tmp->type) return true;
  }
  if (t == CALL){
    struct node_fun_str* tmp = find_fun_str(node->token, fun_r);
    if (tmp != NULL && BOOL == tmp->type) return true;
  }
  if (t == IF) return is_fla (get_child(node, 2));
  if (t == LET) return is_fla (get_child(node, 3));
  return false;
}

int get_fun_var_types(struct ast* node){
  if (node->ntoken == FUNID){
    char* fun_id = node->token;
    if (find_fun_str(fun_id, fun_r) != NULL){
      printf("Function %s defined twice\n", fun_id);
      return 1;
    }

    int arg_num = get_child_num(node->parent);
    struct ast* body = get_child(node->parent, arg_num);

    struct node_var_str* args = NULL;
    for (int i = 2; i < arg_num - 1; i++) {
      struct ast* var = get_child(node->parent, i);
      if (find_fun_str(var->token, fun_r) != NULL || strcmp(var->token, fun_id) == 0){
        printf("Variable and function %s have the same name\n", var->token);
        return -1;
      }
      if (find_var_str(var->id, var->token, vars_r) != NULL){
        printf("Variable %s declared twice in function %s\n", var->token, fun_id);
        return -1;
      }
      push_var_str(var->id, body->id, var->ntoken, var->token, &vars_r, &vars_t);
      if (args == NULL) args = vars_t;
    }
    if (is_term(body)) {
      push_fun_str(fun_id, INT, arg_num - 3, args, &fun_r, &fun_t);
    }
    else if (is_fla(body)) {
      push_fun_str(fun_id, BOOL, arg_num - 3, args, &fun_r, &fun_t);
    }
    else {
      printf("Unable to detect the type of function %s\n", fun_id);
      return -1;
    }
  }
  if (node->ntoken == LETID) {
    if (find_var_str(node->id, node->token, vars_r) != NULL){
      printf("Variable %s declared twice\n", node->token);
      return -1;
    }
    int type;
    if (is_fla(get_child(node->parent, 2))) type = BOOLID;
    else if (is_term(get_child(node->parent, 2))) type = INTID;
    else {
      printf("Unable to detect the type of variable %s\n", node->token);
      return -1;
    }
    push_var_str(node->id, get_child(node->parent, 3)->id, type, node->token, &vars_r, &vars_t);
  }
  return 0;
}

int type_check(struct ast* node){
  int t = node->ntoken;
  int needs_term = (t == EQ || t == LE || t == LT || t == GT || t == GE || t == PLUS || t == MINUS || t == MULT || t == DIV || t == MOD);
  int needs_fla  = (t == NOT || t == AND || t == OR);
  struct ast_child* temp_child_root = node -> child;
  while(temp_child_root != NULL) {
    struct ast* child_node = temp_child_root->id;
    if ((needs_term && !is_term(child_node)) ||
        (needs_fla && !is_fla(child_node))) {
      printf("Does not type check: operator %s vs operand %s\n", node->token, child_node->token);
      return 1;
    }
    temp_child_root = temp_child_root -> next;
  }

  if (t == IF) {
    if (!is_fla (get_child(node, 1))) {
      printf("Does not type check: the if-guard %s is not a formula\n", get_child(node, 1)->token);
      return 1;
    }
    else if ((is_fla (get_child(node, 2)) && !is_fla (get_child(node, 3))) ||
             (is_term (get_child(node, 2)) && !is_term (get_child(node, 3)))) {
      printf("Does not type check: the then-branch %s and the else-branch %s should have the same type\n", get_child(node, 2)->token, get_child(node, 3)->token);
      return 1;
    }
  }

  if (node->ntoken == DEFFUN) {
    char* fun_id = get_child(node, 1)->token;
    struct node_fun_str* deffun = find_fun_str(fun_id, fun_r);
    if (NULL == deffun){
      printf("Function %s has not been defined\n", fun_id);
      return 1;
    }

    int num_chl = get_child_num(node);
    if (get_child(node, num_chl - 1)->ntoken != deffun->type) {
      printf("Declared type of function %s does not match the type of the body\n", fun_id);
      return -1;
    }
  }

  if (node->ntoken == CALL) {
    char* call_id = node->token;
    struct node_fun_str* fun = find_fun_str(call_id, fun_r);
    if (NULL == fun){
      printf("Function %s has not been defined\n", call_id);
      return 1;
    }
    if (get_child_num(node) != fun->arity){
      printf("Wrong number of arguments of function %s\n", call_id);
      return 1;
    }

    struct node_var_str* args = fun->args;
    for (int i = 1; i <= fun->arity; i++){
      if ((is_fla (get_child(node, i)) && args->type == INTID) ||
          (is_term (get_child(node, i)) && args->type == BOOLID)){
        printf("Does not type check: argument %s vs type of %s\n", get_child(node, i)->token, args->name);
        return 1;
      }
      args = args->next;
    }
  }

  return 0;
}

// AST -> CFG translation

int tmp;
int cur_num = 0;

struct node_int* bb_beg_root = NULL;
struct node_int* bb_beg_tail = NULL;

struct node_int* bb_num_root = NULL;
struct node_int* bb_num_tail = NULL;

struct br_instr* bb_root = NULL;
struct br_instr* bb_tail = NULL;

struct asgn_instr* assgn_tmp_root = NULL;
struct asgn_instr* assgn_tmp_tail = NULL;

struct asgn_instr* asgn_root = NULL;
struct asgn_instr* asgn_tail = NULL;

struct br_instr* br_instrs = NULL;

void proc_rec(struct ast* node) {
  
  if (node->ntoken == IF) {
    // encoding of the guard

    int tmp_num1 = cur_num + 1;
    int tmp_num2 = cur_num + 2;
    int tmp_num3 = cur_num + 3;
    cur_num += 4;

    struct br_instr* bri = mk_cbr(bb_num_tail->id, 999, tmp_num1, tmp_num2);
    push_br(bri, &bb_root, &bb_tail);

    push_int(get_child(node, 1)->next->id, &bb_beg_root, &bb_beg_tail);
    push_int(tmp_num1, &bb_num_root, &bb_num_tail);

    proc_rec(get_child(node, 2));
    bri = mk_ubr(bb_num_tail->id, tmp_num3);
    push_br(bri, &bb_root, &bb_tail);

    push_int(get_child(node, 2)->next->id, &bb_beg_root, &bb_beg_tail);
    push_int(tmp_num2, &bb_num_root, &bb_num_tail);

    proc_rec(get_child(node, 3));

    bri = mk_ubr(bb_num_tail->id, tmp_num3);
    push_br(bri, &bb_root, &bb_tail);

    push_int(tmp_num3, &bb_num_root, &bb_num_tail);

    struct asgn_instr* asgn = mk_uasgn(tmp_num3, node->parent->id, node->id, REGID);
    push_asgn(asgn, &assgn_tmp_root, &assgn_tmp_tail);
    push_int(get_child(node, 3)->next->id, &bb_beg_root, &bb_beg_tail);
  }
  else if (node -> child != NULL){
    struct ast_child* temp_ast_child_root = node -> child;
    while(temp_ast_child_root != NULL){
      proc_rec(temp_ast_child_root -> id);
      temp_ast_child_root = temp_ast_child_root -> next;
    }
  }
}

int compute_br_structure(struct ast* node){
  if (node->ntoken == FUNID) {
    push_istr(cur_num, node->token, &ifun_r, &ifun_t);
    push_int(cur_num, &bb_num_root, &bb_num_tail);
    push_int(node->id, &bb_beg_root, &bb_beg_tail);
    proc_rec(get_child(node->parent, get_child_num(node->parent)));
    struct br_instr* bri = mk_ubr(bb_num_tail->id, -1);
    push_br(bri, &bb_root, &bb_tail);
    cur_num++;
  }
  return 0;
}

int inp_counter;

int fill_instrs (struct ast* node) {

  if (bb_beg_root != NULL && bb_beg_root->id == node->id){
    if (br_instrs == NULL){
      br_instrs = bb_root;
    }
    dequeue_int(&bb_beg_root);
  }

  if (node->ntoken == FUNID) {
    inp_counter = 1;
  }
  else if (node->ntoken == INTID || node->ntoken == BOOLID){    // var inputs
    struct asgn_instr* asgn = mk_uasgn(br_instrs->id, node->id, -inp_counter, INP);
    push_asgn(asgn, &asgn_root, &asgn_tail);
    inp_counter++;
  } else if (node->ntoken == CALL){
    for (int i = 1; i <= get_child_num(node); i++) {
      struct asgn_instr* asgn = mk_uasgn(br_instrs->id, -i, get_child(node, i)->id, REGID);
      push_asgn(asgn, &asgn_root, &asgn_tail);
    }
    if (node->parent->ntoken == IF &&
        (get_child(node->parent, 2) == node ||
         get_child(node->parent, 3) == node)) {
      struct asgn_instr* asgn = mk_casgn(br_instrs->id, node->parent->id, node->token);
      push_asgn(asgn, &asgn_root, &asgn_tail);
    } else {
      struct asgn_instr* asgn = mk_casgn(br_instrs->id, node->id, node->token);
      push_asgn(asgn, &asgn_root, &asgn_tail);
    }
  }
  else if (node->ntoken == CONST || node->ntoken == TRUE || node->ntoken == FALSE || node->ntoken == VARID || node->ntoken == LET){

    int val, type;
    if (node->ntoken == VARID) {val = find_var_str(node->id, node->token, vars_r)->begin_id; type = REGID;}
    else if (node->ntoken == LET) {val = get_child(node, 3)->id; type = REGID;}
    else if (node->ntoken == TRUE) {val = 1; type = CONST;}
    else if (node->ntoken == FALSE) {val = 0; type = CONST;}
    else {val = atoi(node->token); type = CONST;}

    if (node->parent->ntoken == IF &&
        (get_child(node->parent, 2) == node ||
         get_child(node->parent, 3) == node)){
      struct asgn_instr* asgn = mk_uasgn(br_instrs->id, node->parent->id, val, type);
      push_asgn(asgn, &asgn_root, &asgn_tail);
    } else{
      struct asgn_instr* asgn = mk_uasgn(br_instrs->id, node->id, val, type);
      push_asgn(asgn, &asgn_root, &asgn_tail);
    }
  }
  else if (node->ntoken == EQ || node->ntoken == LT || node->ntoken == LE || node->ntoken == GT ||
           node->ntoken == GE || node->ntoken == DIV || node->ntoken == MOD ||
           node->ntoken == MINUS || node->ntoken == MULT || node->ntoken == OR ||
           node->ntoken == AND || node->ntoken == MULT || node->ntoken == PLUS){
    struct asgn_instr* asgn;
    int lhs;
    int op1 = get_child(node, 1)->id;
    int op2 = get_child(node, 2)->id;

    if (node->parent->ntoken == IF &&
        (get_child(node->parent, 2) == node ||
         get_child(node->parent, 3) == node)){
      lhs = node->parent->id;
    } else {
      lhs = node->id;
    }

    asgn = mk_basgn(br_instrs->id, lhs, op1, op2, node->ntoken);
    push_asgn(asgn, &asgn_root, &asgn_tail);

    // additional ops
    if (node->ntoken == OR || node->ntoken == AND || node->ntoken == MULT || node->ntoken == PLUS){
      for (int i = 3; i <= get_child_num(node); i++) {
        struct asgn_instr* asgn;
        int op2 = get_child(node, i)->id;
        
        asgn = mk_basgn(br_instrs->id, lhs, lhs, op2, node->ntoken);
        push_asgn(asgn, &asgn_root, &asgn_tail);
      }
    }
  }
  else if (node->ntoken == NOT){
    struct asgn_instr* asgn;
    int lhs;
    int op1 = get_child(node, 1)->id;

    if (node->parent->ntoken == IF &&
        (get_child(node->parent, 2) == node ||
         get_child(node->parent, 3) == node)){
          lhs = node->parent->id;
        } else {
      lhs = node->id;
    }

    asgn = mk_uasgn(br_instrs->id, lhs, op1, NOT);
    push_asgn(asgn, &asgn_root, &asgn_tail);
  }

  if (is_term(node) || is_fla(node)) {
    struct ast* parent_node = node->parent;
    if (parent_node->ntoken == LET && get_child(parent_node, 2) == node) {
      struct asgn_instr* asgn = mk_uasgn(br_instrs->id, get_child(parent_node, 1)->id, node->id, REGID);
      push_asgn(asgn, &asgn_root, &asgn_tail);
    }
    else if (parent_node->ntoken == DEFFUN) {
      struct asgn_instr* asgn = mk_uasgn(br_instrs->id, 0, node->id, REGID);
      push_asgn(asgn, &asgn_root, &asgn_tail);
    }
    else if (parent_node->ntoken == PRINT || parent_node->ntoken == EVAL) {
      struct asgn_instr* asgn = mk_uasgn(br_instrs->id, -1, get_child(parent_node, 2)->id, REGID);
      push_asgn(asgn, &asgn_root, &asgn_tail);
      asgn = mk_casgn(br_instrs->id, parent_node->id, "print");
      push_asgn(asgn, &asgn_root, &asgn_tail);
    }
  }
  
  if (bb_beg_root != NULL && node->next != NULL && bb_beg_root->id == node->next->id){
    if (br_instrs->cond == 0){
      struct asgn_instr* tmp = assgn_tmp_root;
      while (tmp != NULL){
        if (tmp->bb == br_instrs->id && asgn_tail->bb != br_instrs->id){
          struct asgn_instr* copy = mk_asgn(tmp->bb, tmp->lhs, tmp->bin, tmp->op1, tmp->op2, tmp->type);
          push_asgn(copy, &asgn_root, &asgn_tail);
        }
        tmp = tmp->next;
      }
    } else {
      br_instrs->cond = node->id;
    }
    br_instrs = br_instrs->next;
    dequeue_int(&bb_beg_root);
  }
  
  return 0;
}

//BEGIN: From the demo on Nov, 19, 2020 by Prof. Fedyukovic
int visit_instr(struct br_instr* br, struct asgn_instr* asgn,
                int (*f)(struct asgn_instr* asgn, struct br_instr* br, int arg1, int arg2), 
                int arg1, int arg2)
{
  while(asgn != NULL)
  {
    if(asgn->bb != br->id)
    {
      if(br->cond == 0 && br->succ1 == -1)
      {
        return 0;
      }
      br = br->next;
    }
    if(f(asgn, br, arg1, arg2) != 0) return 1;
    if(asgn->bb == br->id) asgn = asgn->next;
  }
  return 0;         
}
//END:  From the demo on Nov, 19, 2020 by Prof. Fedyukovic

int available_regs[7];
FILE *s_file;

char* get_input_reg(int num)
{
  if(num == 1)
    return "%rdi";
  if(num == 2)
    return "%rsi";
  if(num == 3)
    return "%rdx";
  if(num == 4)
    return "%rcx";
  if(num == 5)
    return "%r8";
  if(num == 6)
    return "%r9";

  if(num > 6)
  {
    printf("Error : Function arguments are more than 6!");
    exit(1);
  }
}

char* get_general_reg(int num)
{
  int i;
  int reg_num;
  for(i = 0; i < 7; i++)
  {
    if(available_regs[i] == num)
    {
      reg_num = i+1;
    }
  }
  if(reg_num == 1)
    return "%r10";
  if(reg_num == 2)
    return "%r11";
  if(reg_num == 3)
    return "%r12";
  if(reg_num == 4)
    return "%r13";
  if(reg_num == 5)
    return "%r14";
  if(reg_num == 6)
    return "%r15";
  if(reg_num == 7)
    return "%rbx";
}

//BEGIN:  From the demo on Nov, 19, 2020 by Prof. Fedyukovic
struct node_int* regs;

bool is_op1_reg(struct asgn_instr* asgn)
{
  return asgn->bin == 1 || (asgn->bin == 0 && asgn->type != CONST && asgn->type != INP);
}

bool is_op1_pure_reg(struct asgn_instr* asgn)
{
  return asgn->bin == 1 || (asgn->bin == 0 && asgn->type != CONST && asgn->type != NOT && asgn->type != INP);
}

int get_registers(struct asgn_instr* asgn, struct br_instr* br, int empty1, int empty2)
{
  if(asgn->lhs > 0)
  {
    push_unique_int(asgn->lhs, &regs);
  }
    
  if(is_op1_pure_reg(asgn))
  {
    push_unique_int(asgn->op1, &regs);
  }
  if(asgn->bin == 1)
  {
    push_unique_int(asgn->op2, &regs);
  }

  return 0;
}
//END:  From the demo on Nov, 19, 2020 by Prof. Fedyukovic

int add_regs(struct node_int* r){
  int counter = 0;
  while (r != NULL){
    if(counter > 7)
    {
      printf("Sorry! This program needs more registers!\n");
      printf("Total regs: %d\n", counter);
      exit(1);
    }
    available_regs[counter] = r->id;
    counter++;
    r = r->next;
  }
  return 1;
}

void print_interm() {

  FILE* regs_alloc = fopen("regs_alloc.interm", "w");
  fprintf(regs_alloc, "\nfunction %s\n", find_istr(ifun_r, bb_root->id));
  fprintf(regs_alloc, "entry:\n");

  clean_int(&regs);

  printf("\nfunction %s\n", find_istr(ifun_r, bb_root->id));
  visit_instr(bb_root, asgn_root, get_registers, 0, 0);
  printf("; registers: ");
  print_int(regs);

  add_regs(regs);

  printf("\nentry:\n");
  //remove_regs_from_list();

  struct asgn_instr* asgn = asgn_root;
  struct br_instr* br = bb_root;

  while (asgn != NULL){
    if (asgn->bb != br->id){
      if (br->cond == 0){
        if (br->succ1 != -1)
        {
            printf("br bb%d\n\n", br->succ1);
            fprintf(regs_alloc,"br bb%d\n\n", br->succ1);
        }
          
        else
        {
          printf("br exit\n\n");
          fprintf(regs_alloc,"br exit\n\n");
        }
      }
      else 
      {
        printf("br v%d bb%d bb%d\n\n", br->cond, br->succ1, br->succ2);
        fprintf(regs_alloc,"br v%d bb%d bb%d\n\n", br->cond, br->succ1, br->succ2);
      }

      br = br->next;
      if (br == NULL) return;
      char* fun_name = find_istr(ifun_r, br->id);
      if (fun_name != NULL){
        printf("\nfunction %s\n", fun_name);
        printf("; registers: ");

        clean_int(&regs);
          
        for(int i = 0; i < 7; i++)
          available_regs[i] = 0;

        visit_instr(br, asgn, get_registers, 0, 0);
        print_int(regs);

        add_regs(regs);
        printf("\nentry:\n", fun_name);

        fprintf(regs_alloc,"\nfunction %s\n\n", fun_name);
        fprintf(regs_alloc,"entry:\n", fun_name);
      } else {
        printf("bb%d:\n", br->id);
        fprintf(regs_alloc,"bb%d:\n", br->id);
      }
    }
    if (asgn->bin == 0){
      if (asgn->type == CONST) {
        printf("v%d := %d\n", asgn->lhs, asgn->op1);
        fprintf(regs_alloc,"v%d := %d\n", asgn->lhs, asgn->op1);
      }
      else if (asgn->type == NOT){
      printf("v%d := not v%d\n", asgn->lhs, asgn->op1);
      fprintf(regs_alloc,"v%d := not v%d\n", asgn->lhs, asgn->op1);
      }
      else if (asgn->type == INP) {
        printf("v%d := a%d\n", asgn->lhs, -asgn->op1);
        fprintf(regs_alloc,"v%d := a%d\n", asgn->lhs, -asgn->op1);
      }
        
      else if (asgn->lhs == 0)
      {
        printf("rv := v%d\n", asgn->op1);
        fprintf(regs_alloc,"rv := v%d\n", asgn->op1);
      }
        
      else if (asgn->lhs < 0)
      {
        printf("a%d := v%d\n", -asgn->lhs, asgn->op1);
        fprintf(regs_alloc,"a%d := v%d\n", -asgn->lhs, asgn->op1);
      }
      else
      {
        printf("v%d := v%d\n", asgn->lhs, asgn->op1);
        fprintf(regs_alloc,"v%d := v%d\n", asgn->lhs, asgn->op1);
      }

    }
    else if (asgn->bin == 1){
      if (asgn->type == EQ) {
        printf("v%d := v%d = v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d = v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }
        
      else if (asgn->type == LT){
        printf("v%d := v%d < v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d < v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }
      else if (asgn->type == PLUS){
        printf("v%d := v%d + v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d + v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }
      else if (asgn->type == MINUS){
        printf("v%d := v%d - v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d - v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }
        
      else if (asgn->type == AND){
        printf("v%d := v%d and v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d and v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }
      else if (asgn->type == LE) {
        printf("v%d := v%d <= v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d <= v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }
        
      else if (asgn->type == MULT){
        printf("v%d := v%d * v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d * v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }
        
      else if (asgn->type == DIV){
        printf("v%d := v%d div v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d div v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }
      else if (asgn->type == MOD){
        printf("v%d := v%d mod v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d mod v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }
        
      else if (asgn->type == GT){
        printf("v%d := v%d > v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d > v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }

      else if (asgn->type == GE) {
        printf("v%d := v%d >= v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d >= v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }
        
      else if (asgn->type == OR) {
        printf("v%d := v%d or v%d\n", asgn->lhs, asgn->op1, asgn->op2);
        fprintf(regs_alloc,"v%d := v%d or v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      }
        
    }
    else if (asgn->bin == 2){
      printf("call %s \n", asgn->fun);
      fprintf(regs_alloc,"call %s \n", asgn->fun);
      if (strcmp(asgn->fun, "print") != 0)
      {
        printf("v%d := rv\n", asgn->lhs);
        fprintf(regs_alloc,"v%d := rv\n", asgn->lhs);
      }
    }
    asgn = asgn->next;
  }
  printf("br exit\n");
  fprintf(regs_alloc,"br exit\n");
  fclose(regs_alloc);
}

//BEGIN:  From the demo on Nov, 19, 2020 by Prof. Fedyukovic
void rm_asgn(struct asgn_instr* i, struct asgn_instr** r, struct asgn_instr** t)
{
  struct asgn_instr* pred = (*r);
  while(pred != NULL && pred->next != i) pred = pred->next;
  if(pred == NULL) return;
  if(i == *t) (*t) = pred;
  else
    (*pred).next = i->next;
  
  free(i);
}

bool remove_redun()
{
  bool done = false;
  struct asgn_instr* asgn = asgn_root;

  while(asgn != NULL)
  {
    struct asgn_instr* succ = asgn->next;
    if(is_op1_pure_reg(asgn) && asgn->bin == 0 && asgn->lhs > 0 && asgn->lhs == asgn->op1)
    {
      rm_asgn(asgn, &asgn_root, &asgn_tail);
      done = true;
    }
    asgn = succ;
  }
  return done;
}
 
int rename_reg(struct asgn_instr* asgn, struct br_instr* br, int reg_src, int reg_dst)
{
  if(br->cond == reg_src) {
      (* (&br))->cond = reg_dst;
      }
  if(asgn->lhs == reg_src) 
  {
      (*(&asgn))->lhs = reg_dst;
  } 
  if(is_op1_pure_reg(asgn) && asgn->op1 == reg_src)
  {
      (*(&asgn))->op1 = reg_dst;
  } 
  if(asgn->bin == 1 && asgn->op2 == reg_src) 
  {
      (*(&asgn))->op2 = reg_dst;
  }
  
  if(asgn->bin == 2 && asgn->lhs == reg_src)
    (*(&asgn))->lhs = reg_dst;

  if(asgn->bin < 0 && asgn->op1 == reg_src)
    (*(&asgn))->op1 = reg_dst;

  return 0;
}


int rename_all(struct br_instr* br, struct asgn_instr* asgn, int reg_src, int reg_dst)
{
  return visit_instr(br, asgn, rename_reg, reg_src, reg_dst);
}

int avail_reg(struct asgn_instr* asgn, struct br_instr* br, int reg, int empty)
{
  if(asgn->lhs == reg) return 1;
  if(is_op1_pure_reg(asgn) && asgn->op1 == reg) return 1;
  if(asgn->bin == 1 && asgn->op2 == reg) return 1;


  return 0;
}

int is_available(struct br_instr* br, struct asgn_instr* asgn, int reg)
{
  return visit_instr(br, asgn, avail_reg, reg, 0);
}

void register_alloca(){
  struct asgn_instr* asgn = asgn_root;
  struct br_instr* br = bb_root;
  struct asgn_instr* fun_beg =asgn_root;

	while(asgn != NULL){
	if(asgn->bb != br->id){
	 if(br->cond == 0 && br->succ1 == -1){
	  fun_beg = asgn->next;
    //reg_counter = 1;

    //reset_regs();
	 }
	 br= br->next;	
	}

  if(asgn->bin == 0 && asgn->type == CONST || asgn->bin == 0 && asgn->type == INP)
  {
    for(int i = 0; i < 7; i++)
    {
      if(is_available (br, asgn->next, available_regs[i]) == 0 && available_regs[i] != 0)
        rename_all(br, asgn, asgn->lhs, available_regs[i]);
    }
  }
	if(asgn->lhs > 0)
  {

	  if(is_available (br, asgn->next, asgn->op1) == 0 && is_op1_reg(asgn))
    {
	      struct asgn_instr* temp =fun_beg;
	      while (temp != asgn)
        {
	        if (temp->lhs == asgn->lhs) break;
	        temp = temp->next;
	      }

        if(temp == asgn)
        {
          rename_all(br, asgn, asgn->lhs, asgn->op1);
        }
	  }
	}
  if(asgn->bb == br->id) asgn = asgn->next;
	}

  remove_redun();
}
//END:  From the demo on Nov, 19, 2020 by Prof. Fedyukovic

int write_s_file()
{
  s_file = fopen("prog.s", "w");
  FILE* h_file = fopen("x86_aux_stuff.file", "r");

  if(s_file == NULL)
  {
    printf("ERR: Couldn't open 'prog.s' file\n");
    exit(1);
  }

  if(h_file == NULL)
  {
    printf("ERR: Couldn't open 'x86_aux_stuff.file' file\n");
    exit(1);
  }
  
  char ch = fgetc(h_file);
    while (ch != EOF)
    {
        fputc(ch, s_file);
        ch = fgetc(h_file);
    }

  fclose(h_file);
  fclose(s_file);
  s_file = fopen("prog.s", "a");

  char* str1;
  char* str2;

  clean_int(&regs);
        
  for(int i = 0; i < 7; i++)
      available_regs[i] = 0;
      
  visit_instr(bb_root, asgn_root, get_registers, 0, 0);
  add_regs(regs);

  if (strcmp(find_istr(ifun_r, bb_root->id), "print") == 0 || strcmp(find_istr(ifun_r, bb_root->id), "eval") == 0)
  {
      fprintf (s_file,"\nmain:\n");
      fprintf (s_file,".bb0:\n");
      //main_flag = true;
  }       
  else
  { 
      fprintf (s_file,"\n%s:\n", find_istr(ifun_r, bb_root->id));
      fprintf (s_file,".bb_%s:\n", find_istr(ifun_r, bb_root->id));
  }

  fprintf(s_file,"\tpushq %rbp\n");
  fprintf(s_file,"\tmovq  %rsp, %rbp\n");
  fprintf(s_file,"\tsubq  $64, %rsp\n");

  struct asgn_instr* asgn = asgn_root;
  struct br_instr* br = bb_root;

  while (asgn != NULL){
    if (asgn->bb != br->id){
      if (br->cond == 0){
        if (br->succ1 != -1)
          fprintf(s_file, "jmp .bb%d\n\n", br->succ1);
        else
        {
          fprintf(s_file,"\tleave\n\tret\n");
        }
      }
      else 
      {
        str1 = get_general_reg(br->cond);
        fprintf(s_file,"\tcmpq $0, %s\n", str1);
        fprintf(s_file,"\tje .bb%d\n", br->succ2);
        fprintf(s_file,"\tjmp .bb%d\n", br->succ1);
      }

      br = br->next;
      if (br == NULL) return 0;
      char* fun_name = find_istr(ifun_r, br->id);
      if (fun_name != NULL){
        
        clean_int(&regs);
        
        for(int i = 0; i < 7; i++)
          available_regs[i] = 0;

        visit_instr(br, asgn, get_registers, 0, 0);
        add_regs(regs);

        if (strcmp(fun_name, "print") == 0 || strcmp(fun_name, "eval") == 0)
        {
          fprintf (s_file,"\nmain:\n");
          fprintf (s_file,".bb0:\n");
        }
          
        else
        { 
          fprintf (s_file,"\n%s:\n", fun_name);
          fprintf (s_file,".bb_%s:\n", fun_name);
        }

        fprintf(s_file,"\tpushq %rbp\n");
        fprintf(s_file,"\tmovq  %rsp, %rbp\n");
        fprintf(s_file,"\tsubq  $64, %rsp\n");
        /*
        fprintf (s_file,"\n%s:\n", fun_name);
        fprintf (s_file,".bb_%s:\n", fun_name);
        */
      } else {
        fprintf (s_file,".bb%d:\n", br->id);
      }
    }
    if (asgn->bin == 0){
      if (asgn->type == CONST) {
        str1 = get_general_reg(asgn->lhs);
        fprintf(s_file,"\tmovq $%d, %s\n", asgn->op1, str1);
      }
      else if (asgn->type == NOT){
      str1 = get_general_reg(asgn->op1);
      str2 = get_general_reg(asgn->lhs);
      fprintf(s_file,"\tmovq %s, %s\n", str1, str2);
      str1 = get_general_reg(asgn->lhs);
      fprintf(s_file,"\tnotq %s\n", str1);
      }
      else if (asgn->type == INP) {
        str1 = get_input_reg(-asgn->op1);
        str2 = get_general_reg(asgn->lhs);
        fprintf(s_file,"\tmovq %s, %s \n", str1, str2);
      }
        
      else if (asgn->lhs == 0)
      {
        str1 = get_general_reg(asgn->op1);
        fprintf(s_file,"\tmovq %s, %%rax \n", str1);
      }
        
      else if (asgn->lhs < 0)
      {
        //if(main_flag)
          //fprintf(s_file, "\tmovq %s, %%rsi \n", get_general_reg(asgn->op1));
        //else
        //{
          str1 = get_general_reg(asgn->op1);
          str2 = get_input_reg(-asgn->lhs);

          if(asgn->next->bin == 2 && strcmp(asgn->next->fun, "print") == 0)
            fprintf(s_file, "\tmovq %s, %%rsi \n", str1);
          else
            fprintf(s_file, "\tmovq %s, %s \n", str1, str2);
        //}
      }
      else
      {
        str1 =  get_general_reg(asgn->op1);
        str2 = get_general_reg(asgn->lhs);
        fprintf(s_file,"\tmovq %s, %s \n", str1, str2);
      }

    }
    else if (asgn->bin == 1){
      if (asgn->type == EQ) {
        str1 = get_general_reg(asgn->op1);
        str2 = get_general_reg(asgn->op2);
        fprintf(s_file,"\tcmpq %s, %s\n", str1, str2);
        fprintf(s_file,"\tsete %%al\n");
        fprintf(s_file,"\tmovzbl %%al, %s\n", get_general_reg(asgn->lhs));
      }
        
      else if (asgn->type == LT){
        fprintf(s_file,"\tcmpq %s, %s\n", get_general_reg(asgn->op2), get_general_reg(asgn->op1));
        fprintf(s_file,"\tsetl %%al\n");
        fprintf(s_file,"\tmovzbl %%al, %s\n", get_general_reg(asgn->lhs));
      }
      else if (asgn->type == PLUS){
        fprintf(s_file,"\tmovq %s, %s\n", get_general_reg(asgn->op1), get_general_reg(asgn->lhs));
        fprintf(s_file,"\taddq %s, %s\n", get_general_reg(asgn->op2), get_general_reg(asgn->lhs));
      }
      else if (asgn->type == MINUS){
        fprintf(s_file,"\tmovq %s, %s\n", get_general_reg(asgn->op1), get_general_reg(asgn->lhs));
        fprintf(s_file,"\tsubq %s, %s\n", get_general_reg(asgn->op2), get_general_reg(asgn->lhs));
      }
        
      else if (asgn->type == AND){
      fprintf(s_file,"\tmovq %s, %s\n", get_general_reg(asgn->op1), get_general_reg(asgn->lhs));
      fprintf(s_file,"\tandq %s, %s\n", get_general_reg(asgn->op2), get_general_reg(asgn->lhs));
      }
      else if (asgn->type == LE) {
        fprintf(s_file,"\tcmpq %s, %s\n", get_general_reg(asgn->op2), get_general_reg(asgn->op1));
        fprintf(s_file,"\tsetle %%al\n");
        fprintf(s_file,"\tmovzbl %al, %s\n", get_general_reg(asgn->lhs));
      }
        
      else if (asgn->type == MULT){
        fprintf(s_file,"\tmovq %s, %s\n", get_general_reg(asgn->op1), get_general_reg(asgn->lhs));
        fprintf(s_file,"\timulq %s, %s\n", get_general_reg(asgn->op2), get_general_reg(asgn->lhs));
      }
        
      else if (asgn->type == DIV){
        fprintf(s_file,"\tmovq %s, %rax\n", get_general_reg(asgn->op1));
        fprintf(s_file,"\tmovq $0, %rdx\n");
        fprintf(s_file,"\tdivq %s\n", get_general_reg(asgn->op2));
        fprintf(s_file,"\tmovq %rax, %s\n", get_general_reg(asgn->lhs));
      }
      else if (asgn->type == MOD){
        fprintf(s_file,"\tmovq %s, %rax\n", get_general_reg(asgn->op1));
        fprintf(s_file,"\tmovq $0, %rdx\n");
        fprintf(s_file,"\tdivq %s\n", get_general_reg(asgn->op2));
        fprintf(s_file,"\tmovq %rdx, %s\n", get_general_reg(asgn->lhs));
      }
        
      else if (asgn->type == GT){
        fprintf(s_file,"\tcmpq %s, %s\n", get_general_reg(asgn->op2), get_general_reg(asgn->op1));
        fprintf(s_file,"\tsetg %%al\n");
        fprintf(s_file,"\tmovzbl %%al, %s\n", get_general_reg(asgn->lhs));
      }

      else if (asgn->type == GE) {
        fprintf(s_file,"\tcmpq %s, %s\n", get_general_reg(asgn->op2), get_general_reg(asgn->op1));
        fprintf(s_file,"\tsetge %%al\n");
        fprintf(s_file,"\tmovzbl %%al, %s\n", get_general_reg(asgn->lhs));
      }
        
      else if (asgn->type == OR) {
        fprintf(s_file,"\tmovq %s, %s\n", get_general_reg(asgn->op1), get_general_reg(asgn->lhs));
        fprintf(s_file,"\torq %s, %s\n", get_general_reg(asgn->op2), get_general_reg(asgn->lhs));
      }
        
    }
    else if (asgn->bin == 2){
      fprintf(s_file,"\tcall %s \n", asgn->fun);
      if (strcmp(asgn->fun, "print") != 0)
      {
        fprintf(s_file,"\tmovq %rax, %s\n", get_general_reg(asgn->lhs));
      }
    }
    asgn = asgn->next;
  }
  fprintf(s_file,"\tleave\n\tret\n");
  fclose(s_file);

}
int main (int argc, char **argv) {
  int retval = yyparse();

  push_fun_str("get_int", INT, 0, NULL, &fun_r, &fun_t);
  push_fun_str("get_bool", BOOL, 0, NULL, &fun_r, &fun_t);
  if (retval == 0) retval = visit_ast(get_fun_var_types);
  if (retval == 0) retval = visit_ast(type_check);
  if (retval == 0) print_ast();      // run "dot -Tpdf ast.dot -o ast.pdf" to create a PDF
  else {
    clean_fun_str(&fun_r);
    clean_var_str(&vars_r);
    return 1;
  }

  visit_ast(compute_br_structure);
  visit_ast(fill_instrs);

  register_alloca();
  write_s_file();
  print_interm();

  clean_int(&regs);
  clean_asgns(&asgn_root);
  clean_asgns(&assgn_tmp_root);
  clean_bbs(&bb_root);
  clean_istr(&ifun_r);
  clean_fun_str(&fun_r);
  clean_var_str(&vars_r);

  free_ast();
  return retval;
}
