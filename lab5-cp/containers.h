/**
 * Author:    Grigory Fedyukovich, Subhadeep Bhattacharya
 * Created:   09/30/2020
 *
 * (c) FSU
 **/


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct dstr { char* id1; char* id2; };          // A pair of strings: can be useful whenstoring variables associated to a function

struct node_int { int id; struct node_int* next; };
struct node_str { char* id; struct node_str* next; };
struct node_dstr { char* id1; char* id2; struct node_dstr* next; };
struct node_istr { int id1; char* id2; struct node_istr* next; };

struct node_var_str { int begin_id; int end_id; int type; char* name; struct node_var_str* next; }; // used for vars in symbol tables
struct node_fun_str { char* name; int type; int arity; struct node_var_str* args; struct node_fun_str* next; };    // used for funs in symbol tables

void push_var_str (int begin_id, int end_id, int type, char* name, struct node_var_str** r, struct node_var_str** t);
struct node_var_str* find_var_str(int loc_id, char* name, struct node_var_str* r);
void clean_var_str(struct node_var_str** r);

void push_fun_str (char* name, int type, int arity, struct node_var_str* args, struct node_fun_str** r, struct node_fun_str** t);
struct node_fun_str* find_fun_str(char* name, struct node_fun_str* r);
void clean_fun_str(struct node_fun_str** r);

void push_istr (int c1, char* c2, struct node_istr** r, struct node_istr** t);
char* find_istr(struct node_istr* r, int key);
void clean_istr(struct node_istr** r);


struct br_instr { int id; int cond; int succ1; int succ2; struct br_instr* next;};
struct br_instr* mk_cbr(int id, int cond, int succ1, int succ2);
struct br_instr* mk_ubr(int id, int succ1);
void push_br (struct br_instr* i, struct br_instr** r, struct br_instr** t);
void clean_bbs (struct br_instr** r);

struct asgn_instr {int bb; int lhs; int bin; int op1; int op2; int type; char* fun; struct asgn_instr* next; };
struct asgn_instr* mk_asgn(int bb, int lhs, int bin, int op1, int op2, int type);
struct asgn_instr* mk_basgn(int bb, int lhs, int op1, int op2, int type);
struct asgn_instr* mk_uasgn(int bb, int lhs, int op, int type);
struct asgn_instr* mk_casgn(int bb, int lhs, char* fun);
void push_asgn (struct asgn_instr* i, struct asgn_instr** r, struct asgn_instr** t);
void clean_asgns (struct asgn_instr** r);

struct ast;
struct ast_child {struct ast* id; struct ast_child* next; }; //Dynamic data structure for the list of children
struct ast {int id; char *token; bool is_leaf; int ntoken; char *fun; struct ast_child* child; struct ast* parent; struct ast* next;}; //Dynamic data structure for the AST. Each node points to a terminal/nonterminal and its children

void push_str (char* c, struct node_str** r, struct node_str** t);
char* pop_str (struct node_str** r, struct node_str** t);
int find_str(char* c, struct node_str* r);
int print_str(struct node_str* r);
void clean_str(struct node_str** r);

void push_dstr (char* c1, char* c2, struct node_dstr** r, struct node_dstr** t);
int find_dstr(char* c1, char* c2, struct node_dstr* r);
void clean_dstr(struct node_dstr** r);

void push_int (int i, struct node_int** r, struct node_int** t);
void push_unique_int (int i, struct node_int** r);
void clean_int (struct node_int** r);
int pop_int (struct node_int** r, struct node_int** t);
int print_int(struct node_int* r);
void dequeue_int (struct node_int** r);

int pop_ast(struct node_int** r, struct node_int** t);

void insert_child(int val);       // Helper methods to specify child/children for the upcoming node
void insert_children(int, ...);   // Variadic version (expects the # of children to be given in the first place)
                                  // Note that it is not necessary to insert all children at once. You can call either insert_child or insert_children many times -- they all will be aggregated and used during the next call of insert_node/insert_node_tmp
void insert_node_tmp(int val, char * token, bool is_leaf, int ntoken);
int insert_node(char* token, int ntoken);

struct ast* find_parent(struct ast* ast_node, struct ast* ast_parent);
struct ast* get_root(struct ast* ast_node);
struct ast* find_ast_node(int id);
struct ast* get_child(struct ast* ast_node, int id);
int get_child_num(struct ast* ast_node);

int visit_ast(int (*f)(struct ast* ast_node));
void print_ast(); //    run "dot -Tpdf ast.dot -o ast.pdf" to create a PDF. Requires a preinstalled graphviz package (https://graphviz.org/download/)

void free_ast();

// static data structres used in the lab:

static int current_node_id = 0;
static struct ast_child* ast_child_root = NULL;
static struct ast* ast_list_root = NULL;
