%{
#include <stdio.h>
#include "y.tab.h"

extern YYSTYPE yylval;
%}

%option yylineno
%option caseless

%%

[ \t\n] {}
";" {return SEMICOLON;}
[-] {return MINUS;}
[+] {return PLUS;}
[*] {return MULT;}
[(] {return LP;}
[)] {return RP;}
"fun" {return FUN;}
"define-fun" {return DEFINEFUN;}
"get-int" {return GETINT;}
"get-bool" {return GETBOOL;}
"eval" {return EVAL;}
"let" {return LET;}
"if" {return IF;}
"int" {return INT;}
"bool" {return BOOL;}
"print" {return PRINT;}
"div" {return DIV;}
"mod" {return MOD;}
"true" {return TRUE;}
"false" {return FALSE;}
"not" {return NOT;}
"or" {return OR;}
"and" {return AND;}
[A-Za-z][A-Za-z0-9]* {return ID;} 
[-]?[0-9]* {yylval.num = atoi(yytext); return NUMBER;}
"=" {return EQUAL;}
"<=" {return LEQ;}
">=" {return GEQ;}
"<" {return LESS;}
">" {return GREATER;}

%%

int yywrap(void){
    return 1;
}
