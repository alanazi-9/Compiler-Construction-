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

enum KEYWORDS{DEFINEFUN = 1, GCD, INT, IF, LET, EVAL, GETINT, SEMICOLON}; // *
enum IDs{ID = 10};                                                        //
enum OPs{ EQUAL = 11, GREATERTHAN, MINUS};                                // The tokne types
enum PARENTHESIS{ LEFT = 14, RIGHT};                                      //
enum TOKENS{ KEWORD = 16, ID, OP, PARENTHESIS};                           // *
%}

%option yylineno

%%

[\t\n] {}
[;] {return SEMICOLON;}
[-+*/^=] {return OP;}
[()] {return PARENTHESIS;}
"define-fun" {return DEFINEFUN;} //To return the keyword "define-fun"
"gcd" {return GCD;} // Not completed yet
%%

int check(char *token)  // A function to check if the token has showed or not 
{
struct checkedTokens* searcher = first;   
   
if (strcmp(searcher->lexem, token) != 0 && searcher->next == NULL){ // To check if the token is the first or not
printf("ifsearcher\n");   
free(searcher);             
return 1; }

else if (strcmp(searcher->lexem, token) == 0 && searcher->next == NULL){ // To check if the token is the first or not
printf("found\n");   
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

int token;  // Store the token number that returned from yylex();
token = yylex();

first = (struct checkedTokens *)malloc(sizeof(struct checkedTokens)); 
current = (struct checkedTokens *)malloc(sizeof(struct checkedTokens));

strcpy(current->lexem, yytext);
current->next = NULL;
first=current;

printf("%s: ", yytext);
while(token){
	if(strcmp(yytext, first->lexem) == 0 && first == current){
		printf("%u", yylineno);
		token = yylex();
      }
	else token = yylex();
      }
    printf("\n");
    rewind(stdin);

while(token){
    if(token == DEFINEFUN && check(current->lexem) == 1 && current != first){
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
    rewind(stdin);
    }

    else if(token == DEFINEFUN && check(current->lexem) == 0)
        token = yylex();

    if(token == GCD && check(yytext) == 1 && current != first){
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

