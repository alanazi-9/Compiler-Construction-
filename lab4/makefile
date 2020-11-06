CC	= gcc
YACC	= yacc
LEX	= lex

lab4:	y.tab.c lex.yy.c containers.c comp.c
	$(CC) lex.yy.c y.tab.c containers.c comp.c -o comp

y.tab.c: lab4.y
	$(YACC) -d lab4.y

lex.yy.c: lab4.l y.tab.h
	$(LEX) lab4.l

clean: 
	rm comp lex.yy.c y.tab.c y.tab.h
