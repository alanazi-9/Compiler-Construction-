#include "y.tab.h"
#include "containers.h"
int yyparse();

struct condition_path{
  int cond;
  int succ1;
  int succ2;
  struct condition_path *next;
};

struct condition_path* path_head = NULL;
struct condition_path* last_path = NULL;
// symbol table
struct node_fun_str* fun_r = NULL;
struct node_fun_str* fun_t = NULL;
struct node_var_str* vars_r = NULL;
struct node_var_str* vars_t = NULL;

// br structure
struct node_istr* ifun_r = NULL;
struct node_istr* ifun_t = NULL;

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

void print_interm() {
  printf ("\nfunction %s\n\n", find_istr(ifun_r, bb_root->id));
  printf ("entry:\n");

  struct asgn_instr* asgn = asgn_root;
  struct br_instr* br = bb_root;

  while (asgn != NULL){
    if (asgn->bb != br->id){
      if (br->cond == 0){
        if (br->succ1 != -1)
          printf("br bb%d\n\n", br->succ1);
        else
          printf("br exit\n\n");
      }
      else printf("br v%d bb%d bb%d\n\n", br->cond, br->succ1, br->succ2);
      br = br->next;
      if (br == NULL) return;
      char* fun_name = find_istr(ifun_r, br->id);
      if (fun_name != NULL){
        printf ("\nfunction %s\n\n", fun_name);
        printf ("bb entry:\n");
      } else {
        printf ("bb%d:\n", br->id);
      }
    }
    if (asgn->bin == 0){
      if (asgn->type == CONST)
        printf("v%d := %d\n", asgn->lhs, asgn->op1);
      else if (asgn->type == NOT)
        printf("v%d := not v%d\n", asgn->lhs, asgn->op1);
      else if (asgn->type == INP)
        printf("v%d := a%d\n", asgn->lhs, -asgn->op1);
      else if (asgn->lhs == 0)
        printf("rv := v%d\n", asgn->op1);
      else if (asgn->lhs < 0)
        printf("a%d := v%d\n", -asgn->lhs, asgn->op1);
      else
        printf("v%d := v%d\n", asgn->lhs, asgn->op1);

    }
    else if (asgn->bin == 1){
      if (asgn->type == EQ)
        printf("v%d := v%d = v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      else if (asgn->type == LT)
        printf("v%d := v%d < v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      else if (asgn->type == PLUS)
        printf("v%d := v%d + v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      else if (asgn->type == MINUS)
        printf("v%d := v%d - v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      else if (asgn->type == AND)
        printf("v%d := v%d and v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      else if (asgn->type == LE)
        printf("v%d := v%d <= v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      else if (asgn->type == MULT)
        printf("v%d := v%d * v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      else if (asgn->type == DIV)
        printf("v%d := v%d div v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      else if (asgn->type == MOD)
        printf("v%d := v%d mod v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      else if (asgn->type == GT)
        printf("v%d := v%d > v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      else if (asgn->type == GE)
        printf("v%d := v%d >= v%d\n", asgn->lhs, asgn->op1, asgn->op2);
      else if (asgn->type == OR)
        printf("v%d := v%d or v%d\n", asgn->lhs, asgn->op1, asgn->op2);
    }
    else if (asgn->bin == 2){
      printf("call %s \n", asgn->fun);
      if (strcmp(asgn->fun, "print") != 0)
        printf("v%d := rv\n", asgn->lhs);
    }
    asgn = asgn->next;
  }
  printf("br exit\n");
}

void print_declare_var()
{
  struct asgn_instr* asgn = asgn_root;
  struct br_instr* br = bb_root;

  while(asgn != NULL)
  { 
    if (asgn->bin == 0){
    
      if (asgn->type == INP)
      {
        struct node_var_str* tmp = find_var_str(asgn->lhs, find_ast_node(asgn->lhs)->token, vars_r);
        printf("(declare-var a%d ", -asgn->op1);
        if(tmp->type == BOOLID)
            printf("Bool)\n");
        else
            printf("Int)\n");

        printf("(declare-var v%d ", asgn->lhs);
        if(tmp->type == BOOLID)
            printf("Bool)\n");
        else
            printf("Int)\n");
      }
      
      else if (asgn->lhs == 0)
        printf("(declare-var rv \n");

      else if(asgn->lhs > 0 && asgn->type != INP)
        printf("(declare-var v%d ", asgn->lhs);
    
      if(asgn->lhs > 0)
      {
        if(is_fla(find_ast_node(asgn->lhs)))
            printf("Bool)\n");
        else
            printf("Int)\n");
      }
    }

    asgn = asgn->next;
  }
  while(br != NULL)
  {
    if (br->cond > 0)
    {
      printf("(declare-var v%d ", br->cond);
      if(is_fla(find_ast_node(br->cond)))
            printf("Bool)\n");
        else
            printf("Int)\n");
    }
    br = br->next;
  }

}

void insert_path(int cond)
{
  struct condition_path* tmp = (struct condition_path*)malloc(sizeof(struct condition_path));
  if(path_head == NULL)
  {
    tmp->cond = cond;
    tmp->next = NULL;
    path_head = tmp;
    last_path = path_head;
  }
  else
  {
    struct condition_path* tmp = (struct condition_path*)malloc(sizeof(struct condition_path));
    last_path->next = tmp;
    tmp->cond = cond;
    tmp->next = NULL;
    last_path = tmp;
  }
}

/*int get_path()
{
  int cond;
  if(path_head == NULL)
    return 0;
  
  if(path_head->next == NULL)
  {
    cond = path_head->cond;
    free(path_head);
    return cond;
  }

  cond = last_path->cond;
  struct condition_path* tmp = last_path;
  free(tmp);

  last_path = before_last_path; 
}*/

struct br_instr* get_parent_br_cond(int b)
{
  struct br_instr* br = bb_root;

  while(br != NULL)
  {
    if(br->cond != 0 && (b == br->succ1 || b == br->succ2))
      return br;

    br = br->next;
  }
  return NULL;
}

struct br_instr* get_br(int b)
{
  struct br_instr* br = bb_root;

  while(br != NULL)
  {
    if(b == br->id)
      return br;

    br = br->next;
  }
  return NULL;
}

struct br_instr* br_head = NULL;

int print_assertions()
{
  struct asgn_instr* asgn = asgn_root;
  struct br_instr* br = bb_root;
  struct br_instr* last_if;
  int last_cond;
  if(br != NULL)
  {
    if(br == bb_root)
        printf("(assert (=> true (and\n");

      while (asgn != NULL){
      if (asgn->bb != br->id){
        if (br->cond == 0){

          if (br->succ1 != -1)
          {
            if(br_head->succ2 == br->id)
              printf("b_id: %d b_cond: %d (assert (=> (and v%d) (and\n", br->id, br->cond, br->cond);
            
            else
            {
              struct br_instr* br_parent = br_head;

              if(br->id == br_parent->succ1)
              {
                printf("(assert (=> (and v%d v%d) (and", br_parent->cond, br->cond); 
              }
              else if(br->id == br_parent->succ2)
              {
                printf("(assert (=> (and v%d (not v%d)) (and", br_parent->cond, br->cond);
              } 
            } 
          }
            
          else
          {
            printf(")))\n");
            return 1;
            //printf("(assert (and v%d v%d) (and\n", last_cond, );
          }
        }
        else
        {
            //insert_path(br->cond);
            struct br_instr* br_parent = get_parent_br_cond(br->id); 
            struct br_instr* br_grand_parent;

            if(br_head == NULL)
              br_head = br;
            
            if(br_head == br)
              printf("b_id: %d b_cond: %d (assert (=> (and v%d) (and\n", br->id, br->cond, br->cond);
            else
            {
              br_parent = br_head;

              if(br->id == br_parent->succ1)
              {
                printf("(assert (=> (and v%d v%d) (and", br_parent->cond, br->cond); 
              }
              else if(br->id == br_parent->succ2)
              {
                printf("(assert (=> (and v%d (not v%d)) (and", br_parent->cond, br->cond);
              } 
            }
        } 
          
        br = br->next;
        if (br == NULL) return 1;
      }
      if (asgn->bin == 0){
        if (asgn->type == CONST)
          printf("  (= v%d %d)\n", asgn->lhs, asgn->op1);
        else if (asgn->type == NOT)
          printf("  (= v%d (not v%d))\n", asgn->lhs, asgn->op1);
        else if (asgn->type == INP)
          printf("  (= v%d a%d)\n", asgn->lhs, -asgn->op1);
        else if (asgn->lhs == 0)
          printf("  (= rv v%d)\n", asgn->op1);
        else if (asgn->lhs < 0)
          printf("  (= a%d v%d)\n", -asgn->lhs, asgn->op1);
        else
          printf("  (= v%d v%d)\n", asgn->lhs, asgn->op1);

      }
      else if (asgn->bin == 1){
        if (asgn->type == EQ)
          printf("  (= v%d (= v%d v%d))\n", asgn->lhs, asgn->op1, asgn->op2);
        else if (asgn->type == LT)
          printf("  (= v%d (< v%d v%d))\n", asgn->lhs, asgn->op1, asgn->op2);
        else if (asgn->type == PLUS)
          printf("  (= v%d (+ v%d v%d))\n", asgn->lhs, asgn->op1, asgn->op2);
        else if (asgn->type == MINUS)
          printf("  (= v%d (- v%d v%d))\n", asgn->lhs, asgn->op1, asgn->op2);
        else if (asgn->type == AND)
          printf("  (= v%d (and v%d v%d))\n", asgn->lhs, asgn->op1, asgn->op2);
        else if (asgn->type == LE)
          printf("  (= v%d (<= v%d v%d))\n\n", asgn->lhs, asgn->op1, asgn->op2);
        else if (asgn->type == MULT)
          printf("  (= v%d (* v%d v%d))\n", asgn->lhs, asgn->op1, asgn->op2);
        else if (asgn->type == DIV)
          printf("  (= v%d (div v%d v%d))\n", asgn->lhs, asgn->op1, asgn->op2);
        else if (asgn->type == MOD)
          printf("  (= v%d (mod v%d v%d))\n", asgn->lhs, asgn->op1, asgn->op2);
        else if (asgn->type == GT)
          printf("  (= v%d (> v%d v%d))\n", asgn->lhs, asgn->op1, asgn->op2);
        else if (asgn->type == GE)
          printf("  (= v%d (>= v%d v%d))\n", asgn->lhs, asgn->op1, asgn->op2);
        else if (asgn->type == OR)
          printf("  (= v%d (or v%d v%d))\n", asgn->lhs, asgn->op1, asgn->op2);
      }
      else if (asgn->bin == 2){
        if (strcmp(asgn->fun, "print") != 0)
          printf("  (= v%d rv)\n", asgn->lhs);
      }
      asgn = asgn->next;
    }
    printf(")))\n");
  }
  
}


int main (int argc, char **argv) {
  int retval = yyparse();

  push_fun_str("GET-INT", INT, 0, NULL, &fun_r, &fun_t);
  push_fun_str("GET-BOOL", BOOL, 0, NULL, &fun_r, &fun_t);
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

  print_interm();

  print_declare_var();
  print_assertions();

  clean_asgns(&asgn_root);
  clean_asgns(&assgn_tmp_root);
  clean_bbs(&bb_root);
  clean_istr(&ifun_r);
  clean_fun_str(&fun_r);
  clean_var_str(&vars_r);

  free_ast();
  return retval;
}

