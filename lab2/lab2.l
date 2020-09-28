%{
#include <stdio.h>
#include "y.tab.h"

extern YYSTYPE yylval;
int commentLine;
%}

%option yylineno
%option caseless

%%

[ \t\n] {}
";" {commentLine = yylineno;}
[-] {if(yylineno != commentLine) return MINUS;}
[+] {if(yylineno != commentLine) return PLUS;}
[*] {if(yylineno != commentLine) return MULT;}
[(] {if(yylineno != commentLine) return LP;}
[)] {if(yylineno != commentLine) return RP;}
"define-fun" {if(yylineno != commentLine) return DEFINEFUN;}
"get-int" {if(yylineno != commentLine) return GETINT;}
"get-bool" {if(yylineno != commentLine) return GETBOOL;}
"eval" {if(yylineno != commentLine) return EVAL;}
"let" {if(yylineno != commentLine) return LET;}
"if" {if(yylineno != commentLine) return IF;}
"int" {if(yylineno != commentLine) return INT;}
"bool" {if(yylineno != commentLine) return BOOL;}
"print" {if(yylineno != commentLine) return PRINT;}
"div" {if(yylineno != commentLine) return DIV;}
"mod" {if(yylineno != commentLine) return MOD;}
"true" {if(yylineno != commentLine) return TRUE;}
"false" {if(yylineno != commentLine) return FALSE;}
"not" {if(yylineno != commentLine) return NOT;}
"or" {if(yylineno != commentLine) return OR;}
"and" {if(yylineno != commentLine) return AND;}
[A-Za-z][A-Za-z0-9]* {yylval.id = yytext[0]; if(yylineno != commentLine) return ID;} 
[-]?[0-9]* {yylval.num = atoi(yytext); if(yylineno != commentLine) return NUMBER;}
"=" {if(yylineno != commentLine) return EQUAL;}
"<=" {if(yylineno != commentLine) return LEQ;}
">=" {if(yylineno != commentLine) return GEQ;}
"<" {if(yylineno != commentLine) return LESS;}
">" {if(yylineno != commentLine) return GREATER;}

%%

int yywrap(void){
    return 1;
}
