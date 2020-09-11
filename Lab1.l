%{
#include <stdio.h>

#define MAX 50

//A structure for checking the tokens to implement a linked list
struct checkedTokens{
   char lexem[MAX];
   struct checkedTokens *next;
};

struct checkedTokens *first; // Points to the first token
struct checkedTokens *current; // Points to the current token
struct checkedTokens *new; // Points to the newer token

enum KEYWORDS{DEFINEFUN = 1, VAR, EVAL, PRINT, INT, BOOL, GETINT, IF, LET, GETBOOL, SEMICOLON}; // *
enum OPs{ EQUAL = 12, GREATERTHAN, MINUS};                                // The tokne types
enum PARENTHESIS{ LEFT = 15, RIGHT};                                      //
enum TOKENS{ KEWORD = 17, ID, OP, PARENTHESIS, GCD};                           // *
%}

%option yylineno

%%

[\t\n] {}
[;] {return SEMICOLON;}
[-+*/^=] {return OP;}
[()] {return PARENTHESIS;}
"define-fun" {return DEFINEFUN;} //To return the keyword "define-fun"
"gcd" {return GCD;} // Not completed yet (just for testing)
([a-zA-Z0-9])* {return ID;}
%%

int check(char *token)  // A function to check if the token has showed or not 
{
struct checkedTokens* searcher = first;   
   
if (strcmp(searcher->lexem, token) != 0 && searcher->next == NULL || strcmp(searcher->lexem, token) == 0 && searcher->next == NULL){ // To check if the token is the first or not
//printf("first IF in check();\n"); //for testing  
free(searcher);             
return 1; }

 while (searcher != NULL) // While loop to check every token in the linked list
    { 
        if (strcmp(searcher->lexem, token) == 0){ 
		
		printf("innerloop\n");
        return 0; }
        searcher = searcher->next; 
    } 
    return 1; 
}

int main(int argc, char* argv[]) {

int firstToken;  // Store the first token number that returned from yylex();
firstToken = yylex();

first = (struct checkedTokens *)malloc(sizeof(struct checkedTokens)); 
current = (struct checkedTokens *)malloc(sizeof(struct checkedTokens));

strcpy(first->lexem, yytext);
first->next = NULL;

strcpy(current->lexem, yytext);
current->next = NULL;

printf("%s: ", yytext);
while(firstToken){
	if(strcmp(yytext, first->lexem) == 0){
		printf("%u", yylineno);
		firstToken = yylex();
      }
	else firstToken = yylex();
      }
    printf("\n");
    yylineno = 1;
    rewind(stdin);

int token;

firstToken = yylex(); 
token = yylex();

while(token){
    if(token == DEFINEFUN && check(current->lexem) == 1){
    	printf("%s: ", yytext);
	new = (struct checkedTokens *)malloc(sizeof(struct checkedTokens));
	strcpy(new->lexem, yytext);
	current->next = new;
	new->next = NULL;
	while(token){
	if(token == DEFINEFUN){
		printf("%u", yylineno);
		token = yylex();
      }
	else token = yylex();
      }
    printf("\n");
    yylineno = 1;
    rewind(stdin);
    }

    else if(token == DEFINEFUN && check(current->lexem) == 0)
        token = yylex();

    else if(token == GCD && check(yytext) == 1){
        printf("%s: ", yytext);
	      new = (struct checkedTokens *)malloc(sizeof(struct checkedTokens));
	strcpy(new->lexem, yytext);
        current->next = new;
	new->next = NULL;
        while(token){
        if(token == GCD){
                printf("%u", yylineno);
                token = yylex();
      }
        else token = yylex();
      }
    printf("\n");
    yylineno = 1;
    rewind(stdin);
    }
    else if(token == GCD && check(yytext) == 1)
        token = yylex();
    else break;
  }

  free(first);
  free(new);	
  return 0;

}

