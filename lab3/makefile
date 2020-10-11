CC	= gcc
YACC	= yacc
LEX	= lex

lab3:	y.tab.c lex.yy.c containers.c
	$(CC) lex.yy.c y.tab.c containers.c

y.tab.c: lab3.y
	$(YACC) -d lab3.y

lex.yy.c: lab3.l y.tab.h
	$(LEX) lab3.l

clean: 
	rm a.out lex.yy.c y.tab.c y.tab.h
