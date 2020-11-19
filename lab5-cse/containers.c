/**
 * Author:    Grigory Fedyukovich, Subhadeep Bhattacharya
 * Created:   09/30/2020
 *
 * (c) FSU
 **/

#include "containers.h"

void push_int (int i, struct node_int** r, struct node_int** t){
  if (*r == NULL){   // If root is null - empty list
    *r = (struct node_int*)malloc(sizeof(struct node_int)); //Create a new node a assign the root pointer tp tail
    (*r)->id = i;
    (*r)->next = NULL;
    *t = *r;
  }
  else{
    struct node_int* ptr; //Non-empty list
    ptr = (struct node_int*)malloc(sizeof(struct node_int)); // Create a new node, put it after tail as a new node
    ptr->id = i;
    ptr->next = NULL;
    (*t)->next = ptr;
    (*t) = ptr;
  }
}

void push_unique_int (int i, struct node_int** r){
  if (*r == NULL){   // If root is null - empty list
    *r = (struct node_int*)malloc(sizeof(struct node_int)); //Create a new node a assign the root pointer tp tail
    (*r)->id = i;
    (*r)->next = NULL;
  }
  else{
    if ((*r)->id == i) return;
    push_unique_int(i, &(*r)->next);
  }
}

int pop_int(struct node_int** r, struct node_int** t) {
  if ((*r)->next == NULL) {             //Check if next node is NULL - check for only one node
    int retval = (*r)->id;              //Store the id of the current node in retval
    free(*r);
    *r = NULL;                          //Store current node to NULL
    *t = NULL;
    return retval;                      //return retval
  }

  struct node_int** r1 = r;             //Else store the pointer in r1 - if multiple node, just traverse and then pop
  while ((*r1)->next != NULL) {
    *t = *r1;
    r1 = &((*r1)->next); //traverse the tree
  }

  int retval = (*r1)->id;               //store the last element in retval
  *r1 = NULL;                           //Store current node to NULL
  return retval;                        //return retval
}

void dequeue_int (struct node_int** r) {
  struct node_int* tmp = *r;
  (*r) = (*r)->next;
  free(tmp);
}

int pop_ast(struct node_int** r, struct node_int** t) {
  if ((*r)->next == NULL) {             //Check if next node is NULL - check for only one node
    int retval = (*r)->id;              //Store the id of the current node in retval
    *r = NULL;
    *t = *r;                            //Store current node to NULL
    free(*r);
    return retval;                      //return retval
  }

  struct node_int** r1 = r;             //Else store the pointer in r1 - if multiple node, just traverse and then pop
  struct node_int** temp = NULL;
  while ((*r1)->next != NULL) {
      temp = r1;
      r1 = &((*r1)->next);              //traverse the tree
  }

  int retval = (*r1)->id;               //store the last element in retval
  *r1 = NULL;                           //Store current node to NULL
  *t = *temp;
  return retval;                        //return retval
}

void push_str (char* c, struct node_str** r, struct node_str** t){
  if (*r == NULL){                           //If root node is null
    *r = (struct node_str*)malloc(sizeof(struct node_str)); //Create a new node
    (*r)->id = c;                           //set the id for the new node
    (*r)->next = NULL;                                      //set next pointer to null
    *t = *r;                                                //Set tail pointer == root pointer
  }
  else {                                   //If root node is not null
    struct node_str* ptr;
    ptr = (struct node_str*)malloc(sizeof(struct node_str));  //Create a temporary node
    ptr->id = c;                                            //Set id for the new node
    ptr->next = NULL;                                         //Set next pointer to NULL
    (*t)->next = ptr;                                         //Set the node after tail
    *t = ptr;                                                 //Set tail as the new pointer
  }
}

char* pop_str(struct node_str** r, struct node_str** t) {
  if ((*r)->next == NULL) {
    char* retval = (*r)->id;
    *r = NULL;
    free(*r);
    *t = *r;
    return retval;
  }

  struct node_str** r1 = r;
  while ((*r1)->next != NULL){
    (*t) = *r1;
    r1 = &((*r1)->next);
  }

  char* retval = (*r1)->id;
  *r1 = NULL;
  free(*r1);
  return retval;
}

int find_str(char* c, struct node_str* r){
  while (r != NULL){
    if (strcmp(c, r->id) == 0) return 0;
    r = r->next;
  }
  return 1;
}

int print_str(struct node_str* r){
  while (r != NULL){
    printf(" %s,", r->id);
    r = r->next;
  }
  printf("\n");
  return 1;
}

int print_int(struct node_int* r){
  while (r != NULL){
    printf(" %d,", r->id);
    r = r->next;
  }
  printf("\n");
  return 1;
}


void push_istr (int c1, char* c2, struct node_istr** r, struct node_istr** t){
  if (*r == NULL) {                          //If root node is null
    *r = (struct node_istr*)malloc(sizeof(struct node_istr)); //Create a new node
    (*r)->id1 = c1;                           //set the id for the new node
    (*r)->id2 = c2;                           //set the id for the new node
    (*r)->next = NULL;                                      //set next pointer to null
    *t = *r;                                                //Set tail pointer == root pointer
  }
  else {                                    //If root node is not null
    struct node_istr* ptr;
    ptr = (struct node_istr*)malloc(sizeof(struct node_istr));  //Create a temporary node
    ptr->id1 = c1;                                            //Set id for the new node
    ptr->id2 = c2;                                            //Set id for the new node
    ptr->next = NULL;                                         //Set next pointer to NULL
    (*t)->next = ptr;                                         //Set the node after tail
    *t = ptr;                                                 //Set tail as the new pointer
  }
}

char* find_istr(struct node_istr* r, int key){
  while (r != NULL){
    if (r->id1 == key) return r->id2;
    r = r->next;
  }
  return NULL;
}

void push_dstr (char* c1, char* c2, struct node_dstr** r, struct node_dstr** t){
  if (*r == NULL) {                          //If root node is null
    *r = (struct node_dstr*)malloc(sizeof(struct node_dstr)); //Create a new node
    (*r)->id1 = c1;                           //set the id for the new node
    (*r)->id2 = c2;                           //set the id for the new node
    (*r)->next = NULL;                                      //set next pointer to null
    *t = *r;                                                //Set tail pointer == root pointer
  }
  else {                                   //If root node is not null
    struct node_dstr* ptr;
    ptr = (struct node_dstr*)malloc(sizeof(struct node_dstr));  //Create a temporary node
    ptr->id1 = c1;                                            //Set id for the new node
    ptr->id2 = c2;                                            //Set id for the new node
    ptr->next = NULL;                                         //Set next pointer to NULL
    (*t)->next = ptr;                                         //Set the node after tail
    *t = ptr;                                                 //Set tail as the new pointer
  }
}

int find_dstr(char* c1, char* c2, struct node_dstr* r){
  while (r != NULL){
    if (strcmp(c1, r->id1) == 0 && strcmp(c2, r->id2) == 0) return 0;
    r = r->next;
  }
  return 1;
}

struct br_instr* mk_cbr(int id, int cond, int succ1, int succ2){
  struct br_instr* tmp = (struct br_instr*)malloc(sizeof(struct br_instr));
  tmp->id = id;
  tmp->cond = cond;
  tmp->succ1 = succ1;
  tmp->succ2 = succ2;
  tmp->next = NULL;
  return tmp;
}

struct br_instr* mk_ubr(int id, int succ1){
  struct br_instr* tmp = (struct br_instr*)malloc(sizeof(struct br_instr));
  tmp->id = id;
  tmp->cond = 0;
  tmp->succ1 = succ1;
  tmp->next = NULL;
  return tmp;
}

void push_br (struct br_instr* i, struct br_instr** r, struct br_instr** t){
  if (*r == NULL){
    *r = i;
    *t = *r;
  }
  else {
    (*t)->next = i;
    (*t) = i;
  }
}

struct asgn_instr* mk_asgn(int bb, int lhs, int bin, int op1, int op2, int type){
  struct asgn_instr* tmp = (struct asgn_instr*)malloc(sizeof(struct asgn_instr));
  tmp->bb = bb;
  tmp->bin = bin;
  tmp->lhs = lhs;
  tmp->bin = 1;
  tmp->op1 = op1;
  tmp->op2 = op2;
  tmp->type = type;
  tmp->next = NULL;
  return tmp;
}

struct asgn_instr* mk_basgn(int bb, int lhs, int op1, int op2, int type){
  struct asgn_instr* tmp = (struct asgn_instr*)malloc(sizeof(struct asgn_instr));
  tmp->bb = bb;
  tmp->lhs = lhs;
  tmp->bin = 1;
  tmp->op1 = op1;
  tmp->op2 = op2;
  tmp->type = type;
  tmp->next = NULL;
  return tmp;
}

struct asgn_instr* mk_uasgn(int bb, int lhs, int op, int type){
  struct asgn_instr* tmp = (struct asgn_instr*)malloc(sizeof(struct asgn_instr));
  tmp->bb = bb;
  tmp->lhs = lhs;
  tmp->bin = 0;
  tmp->op1 = op;
  tmp->type = type;
  tmp->next = NULL;
  return tmp;
}

struct asgn_instr* mk_casgn(int bb, int lhs, char* fun){
  struct asgn_instr* tmp = (struct asgn_instr*)malloc(sizeof(struct asgn_instr));
  tmp->bb = bb;
  tmp->lhs = lhs;
  tmp->bin = 2;
  tmp->fun = fun;
  tmp->next = NULL;
  return tmp;
}

void push_asgn (struct asgn_instr* i, struct asgn_instr** r, struct asgn_instr** t){
  if (*r == NULL){
    *r = i;
    *t = *r;
  }
  else {
    (*t)->next = i;
    (*t) = i;
  }
}

void push_var_str (int begin_id, int end_id, int type, char* name, struct node_var_str** r, struct node_var_str** t){
  if (*r == NULL){
    *r = (struct node_var_str*)malloc(sizeof(struct node_var_str)); //Create a new node
    (*r)->begin_id = begin_id;
    (*r)->end_id = end_id;
    (*r)->type = type;
    (*r)->name = name;
    (*r)->next = NULL;
    *t = *r;
  }
  else {
    struct node_var_str* ptr;
    ptr = (struct node_var_str*)malloc(sizeof(struct node_var_str));  //Create a temporary node
    ptr->begin_id = begin_id;
    ptr->end_id = end_id;
    ptr->type = type;
    ptr->name = name;
    ptr->next = NULL;
    (*t)->next = ptr;                                         //Set the node after tail
    *t = ptr;
  }
}

void push_fun_str (char* name, int type, int arity, struct node_var_str* args, struct node_fun_str** r, struct node_fun_str** t) {
  if (*r == NULL){
    *r = (struct node_fun_str*)malloc(sizeof(struct node_fun_str)); //Create a new node
    (*r)->name = name;
    (*r)->type = type;
    (*r)->args = args;
    (*r)->next = NULL;
    (*r)->arity = arity;
    *t = *r;
  }
  else {
    struct node_fun_str* ptr;
    ptr = (struct node_fun_str*)malloc(sizeof(struct node_fun_str));  //Create a temporary node
    ptr->args = args;
    ptr->type = type;
    ptr->name = name;
    ptr->arity = arity;
    ptr->next = NULL;
    (*t)->next = ptr;                                         //Set the node after tail
    *t = ptr;
  }
}


struct node_var_str* find_var_str(int loc_id, char* name, struct node_var_str* r){
  while (r != NULL){
    if (strcmp(name, r->name) == 0 &&
        r->begin_id <= loc_id && loc_id <= r->end_id) return r;
    else r = r->next;
  }
  return NULL;
}


struct node_fun_str* find_fun_str(char* name, struct node_fun_str* r){
  while (r != NULL){
    if (strcmp(name, r->name) == 0) return r;
    else r = r->next;
  }
  return NULL;
}

void clean_asgns (struct asgn_instr** r){
  while (*r != NULL){
    struct asgn_instr* tmp = *r;
    (*r) = (*r)->next;
    free(tmp);
  }
}

void clean_bbs (struct br_instr** r){
  while (*r != NULL){
    struct br_instr* tmp = *r;
    (*r) = (*r)->next;
    free(tmp);
  }
}

void clean_int (struct node_int** r){
  while (*r != NULL){
    struct node_int* tmp = *r;
    (*r) = (*r)->next;
    free(tmp);
  }
}

void clean_str(struct node_str** r){
  while (*r != NULL){
    struct node_str* tmp = *r;
    (*r) = (*r)->next;
    free(tmp);
  }
}

void clean_istr(struct node_istr** r){
  while (*r != NULL){
    struct node_istr* tmp = *r;
    (*r) = (*r)->next;
    free(tmp);
  }
}

void clean_dstr(struct node_dstr** r){
  while (*r != NULL){
    struct node_dstr* tmp = *r;
    (*r) = (*r)->next;
    free(tmp);
  }
}

void clean_var_str(struct node_var_str** r){
  while (*r != NULL){
    struct node_var_str* tmp = *r;
    (*r) = (*r)->next;
    free(tmp);
  }
}

void clean_fun_str(struct node_fun_str** r){
  while (*r != NULL){
    struct node_fun_str* tmp = *r;
    (*r) = (*r)->next;
    free(tmp);
  }
}

void insert_parent(struct ast* p) {
  struct ast_child* temp_ast_child_root = p->child;
  while(temp_ast_child_root != NULL){
    temp_ast_child_root->id->parent = p;
    temp_ast_child_root = temp_ast_child_root->next;
  }
}

void fill_ast_node(struct ast** t, int val, char* token, bool is_leaf, int ntoken){
  (*t) = (struct ast*)malloc(sizeof(struct ast));
  (*t)->id = val;                                     //set value into id field
  (*t)->next = NULL;                                  //set next pointer to NULL
  (*t)->token = token;
  (*t)->is_leaf = is_leaf;
  (*t)->ntoken = ntoken;
  if (ast_child_root != NULL){                          //if child doesnot exist,
    (*t)->child = ast_child_root;                     //set current child root pointer to child field
    ast_child_root = NULL;                              //Set the child root to NULL as we intend to set a new list
    insert_parent(*t);
  } else {
    (*t)->child = NULL;                               //
  }
}

void insert_node_tmp(int val, char* token, bool is_leaf, int ntoken){
  if (ast_list_root == NULL){                               //Create a new node and set the value if root is empty
    fill_ast_node(&ast_list_root, val, token, is_leaf, ntoken);
  } else {                                                  //Else create a ast graph node and assign it to next
    struct ast* ptr;                                        //Create a temp pointer
    fill_ast_node(&ptr, val, token, is_leaf, ntoken);

    //Traverse root and set the new pointer at the next of last node
    struct ast** tmp = &ast_list_root;
    while ((*tmp)->next != NULL) {
      tmp = &((*tmp)->next);    // last_node will store the pointer to last node
    }
    (*tmp)->next = ptr;
  }
}

int insert_node(char* token, int ntoken) {
  current_node_id++;
  insert_node_tmp(current_node_id, token, ast_child_root == NULL, ntoken);
  return current_node_id;
}


void insert_child(int val){        //This function helps to fill the child list
  struct ast* node = find_ast_node(val);
  if (ast_child_root == NULL) {     //If null list
    (ast_child_root) = (struct ast_child*)malloc(sizeof(struct ast_child));  //allocate memory
    // Set id and next field of the ast_child_root
    (ast_child_root)->id = node;
    (ast_child_root)->next = NULL;
  } else {
    //Create the node
    struct ast_child* ptr;
    ptr = (struct ast_child*)malloc(sizeof(struct ast_child));
    ptr->id = node;
    ptr->next = NULL;

    //Traverse the list and set the node at top
    struct ast_child** temp = &ast_child_root;     // Set child root in a temporary variable
    struct ast_child** last_child_node = NULL;
    while ((*temp)->next != NULL) {           // Until next is NULL traverse the list
      temp = &((*temp)->next);              // temp will hold the address of next node
    }
    (*temp)->next = ptr;
  }
}

void insert_children (int n, ...)
{
  va_list vl;
  va_start(vl,n);
  for (int i = 0; i < n; i++)
    insert_child(va_arg(vl, int));
  va_end(vl);
}

struct ast* find_ast_node(int id){
  struct ast* temp_root = ast_list_root;
  while(temp_root != NULL){
    if (id == temp_root->id) return temp_root;
    temp_root = temp_root->next;
  }
  return NULL;
}

struct ast* find_parent(struct ast* ast_node, struct ast* ast_parent){
  struct ast* tmp = ast_node;
  while(tmp != NULL) {
    if (tmp == ast_parent) return tmp;
    tmp = tmp->parent;
  }
  return NULL;
}

struct ast* get_root(struct ast* ast_node){
  struct ast* tmp = ast_node;
  while(tmp->parent != NULL) {
    tmp = tmp->parent;
  }
  return tmp;
}

struct ast* get_child(struct ast* ast_node, int id){
  struct ast_child* temp_child_root = ast_node->child;
  int child_num = 0;
  while(temp_child_root != NULL) {
    child_num++;
    if (child_num == id) {
      return temp_child_root->id;
    }
    temp_child_root = temp_child_root->next;
  }
  return NULL;
}

int get_child_num(struct ast* ast_node){
  struct ast_child* temp_child_root = ast_node->child;
  int child_num = 0;
  while(temp_child_root != NULL) {
    child_num++;
    temp_child_root = temp_child_root->next;
  }
  return child_num;
}

int visit_ast(int (*f)(struct ast* ast_node)){
  struct ast* temp_root = ast_list_root;
  while(temp_root != NULL){
    if (f (temp_root) != 0) return 1;
    temp_root = temp_root->next;
  }
  return 0;
}

FILE *fp;
int print(struct ast* temp_root) {
  if (! temp_root->is_leaf){
    fprintf(fp, "%d [label=\"%s\", fontname=\"monospace\", style=filled, fillcolor=mintcream];\n ", temp_root->id, temp_root->token);
  } else {
    fprintf(fp, "%d [label=\"%s\", fontname=\"monospace\"];\n ", temp_root->id, temp_root->token);
  }
  if (temp_root->child != NULL){
    struct ast_child* temp_ast_child_root = temp_root->child;
    while(temp_ast_child_root != NULL){
      fprintf(fp, "%d->%d\n ", temp_root->id, temp_ast_child_root->id->id);
      temp_ast_child_root = temp_ast_child_root->next;
    }
  }
  return 0;
}

void print_ast(){
  fp = fopen("ast.dot", "w");
  fprintf(fp, "digraph print {\n ");
  visit_ast(print);
  fprintf(fp, "}\n ");
  fclose(fp);
}

void free_ast() {
  struct ast* temp_root = ast_list_root;
  struct ast* current_root = NULL;
  while(temp_root != NULL){
    if (temp_root->child != NULL){
      struct ast_child* temp_ast_child_root = temp_root->child;
      struct ast_child* current_child = NULL;
      while(temp_ast_child_root != NULL){
        current_child = temp_ast_child_root;
        temp_ast_child_root = temp_ast_child_root->next;
        free(current_child);
      }
    }
    current_root = temp_root;
    temp_root = temp_root->next;
    free(current_root);
  }
}
