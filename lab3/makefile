CC	= gcc
YACC	= yacc
LEX	= lex

ambig:	y.tab.c lex.yy.c containers.c comp.c
	$(CC) lex.yy.c y.tab.c containers.c comp.c

y.tab.c: lab3.y
	$(YACC) -d lab3.y

lex.yy.c: lab3.l y.tab.h
	$(LEX) lab3.l

clean: 
	rm a.out lex.yy.c y.tab.c y.tab.h
