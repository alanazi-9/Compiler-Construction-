%{
  #include "y.tab.h"
  int yylex();
%}
%%

[ \t\n] {}
";".+$ ;
[-]?([1-9][0-9]*|0) { yylval.str = strdup(yytext); return CONST;}
"=" {return EQ;}
"<" {return LT;}
">" {return GT;}
"<=" {return LE;}
">=" {return GE;}
"+" {return PLUS;}
"-" {return MINUS;}
"*" {return MULT;}
"div" {return DIV;}
"mod" {return MOD;}
"define-fun" return DEFFUN;
"get-int" return GETINT;
"get-bool" return GETBOOL;
"true" return TRUE;
"false" return FALSE;
"and" return AND;
"or" return OR;
"not" return NOT;
"if" return IF;
"let" return LET;
"int" return INT;
"bool" return BOOL;
"print" return PRINT;
"eval" return EVAL;
[a-zA-Z][a-zA-Z0-9]* { yylval.str = strdup(yytext); return ID;}
[(] {return LPAR;}
[)] {return RPAR;}
. {printf("Lex error: %s\n", yytext); return ERR;}
%%

int yywrap(void) {return 1;}
