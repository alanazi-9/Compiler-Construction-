%{
#include <stdio.h>

#define KEWORD  1           // *
#define ID 2                //
#define OP 3                // 
#define PARENTHESIS 4       // Types of tokens
#define SEMICOLON 5         //
#define COMMENT 6           // *

#define ERROR -1

#define MAX 50              // Max size of the token 

#define DEBUGGMODE 0        // Debugging flag


//A structure for for each token to implement a linked list
struct Token{
   char lexem[MAX];      // Holds the name of the token (lexem) 
   int type;             // Holds the type of the token (ex: keyword, operator, ..etc)
   int lineNo;           // Holds the line number of the token
   int printed;          // A flag to check if the token has printed or not
   struct Token *next;   // A pointer to the next token
};

struct Token *first;        // Points to the first token
struct Token *current;      // Points to the current token
struct Token *nextToken;    // Points to the next token
int commentLine;            // Holds the comment line number 
%}

%option yylineno
%option caseless

%%

[\t\n] {}
";" {commentLine = yylineno;}
[-+*/^] {addToken(OP);}
[()] {addToken(PARENTHESIS);}
"define-fun" {addToken(KEWORD);} 
"get-int" {addToken(KEWORD);}
[A-Za-z][A-Za-z0-9]* {addToken(ID);}
[0-9][A-Za-z0-9]+ {} 
"=" {addToken(OP);}
"<=" {addToken(OP);}
">=" {addToken(OP);}
"<" {addToken(OP);}
">" {addToken(OP);}

%%

void printLinkedList() // To print the linked list [used for debugging]
{
    current = first;

    while (current != NULL) // While loop to check every token in the linked list
    { 
        printf("\nLexem: %s Type: %d  lineNo: %d\n", current->lexem, current->type, current->lineNo);
        current = current->next; 
    } 
}
int stringCmpi(char *s1,char *s2, int type) // A function to ignore a sensitive cases like what we have in the lab1
{
    if(type != OP)
    {
        int i=0,diff=0;
        for(i=0; s1[i]!='\0'; i++)
        {
            if(toupper(s1[i])!=toupper(s2[i]))
                return 1;           
        }
        return 0;
    }
    if(type == OP)
        return strcmp(s1, s2);
}
int addToken(int type)  // A function to check if the token has showed or not [Returns the type of the token]
{
    if(first == NULL && yylineno != commentLine) //Add the first token to the linked list
    {
        current = (struct Token *)malloc(sizeof(struct Token));

        strcpy(current->lexem, yytext);   // Copy from the scanner to the lexem string
        current->type = type;             // Add the type of the token 
        current->lineNo = yylineno;       // Add the line number of the token
        current->printed = 0;             // Zeroing the flag
        current->next = NULL;             // Making the next pointer points to NULL
        first = current;                  // Making the first pointer points the current
    }
    else if(first != NULL && yylineno != commentLine)   // The the first token has already inserted
    {
        struct Token *new;                                     // A pointer for a new token to be added to the linked list
        new = (struct Token *)malloc(sizeof(struct Token));    // Allocating memory for the new token
        current->next = new;                                   // Maikng the next pointer of the current token points to the new token 
        strcpy(new->lexem, yytext);                            // Copy from the scanner to the lexem string  
        new->type = type;                                      // Add the type of the token
        new->lineNo = yylineno;                                // Add the line number of the token
        new->printed = 0;                                      // Zeroing the flag
        new->next = NULL;                                      // Making the next pointer points to NULL
        current = new;                                         // Making the current pointer points the new token

    }
    else    
        return COMMENT;
    
    return type;   // Return the type of the token
}

void printTokensLines(){  // A function to print each token with its lines numbers
    
    current = first;  // To start from the beginning of the linked list

    while (current != NULL) // While loop to check every token in the linked list
    {  
        nextToken = first;
        
        if(current->printed == 0) // Checking if the current token has printed or not
        {
            printf("%-25s:", current->lexem); // Printing the token to be followed by the lines numbers

            while(nextToken != NULL)
            {
                if(stringCmpi(current->lexem, nextToken->lexem, current->type) == 0) // here to check and compare to solve the sensitive cases
                {
                    printf("%d  ", nextToken->lineNo); // Printing the lines numbers
                    nextToken->printed = 1;          // Setting the flag to [printed] to ignore it for the next same token
                }
                
                nextToken = nextToken->next;
            }
            printf("\n");
        }
        current = current->next; 
    } 

}

int main(int argc, char* argv[]) 
{
    first = NULL;           //
    commentLine = 0;        //  Initializing variables
    int token = 0;          //

    token = yylex();
    printf("\n");

    printTokensLines(); 

    if(DEBUGGMODE)          // For debug purposes 
        printLinkedList();

    return 0;
}
