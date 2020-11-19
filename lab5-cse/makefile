CC	= gcc
YACC	= yacc
LEX	= lex

comp:	y.tab.c lex.yy.c containers.c comp.c 
	$(CC) lex.yy.c y.tab.c containers.c comp.c -o comp

y.tab.c: lab_yacc.y
	$(YACC) -d lab_yacc.y

lex.yy.c: lab_lex.l y.tab.h
	$(LEX) lab_lex.l

clean: 
	rm a.out lex.yy.c y.tab.c y.tab.h
