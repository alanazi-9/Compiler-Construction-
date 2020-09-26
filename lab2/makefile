lab2: lex.yy.o y.tab.o
	gcc $^ -o $@

lex.yy.o: lex.yy.c y.tab.h
	gcc $< -c

%.o: %.c
	gcc $< -c

lex.yy.c: lab2.l
	lex $<

y.tab.h y.tab.c: lab2.y
	yacc -d $<

clean:
	rm -rf lab2 *.o lex.yy.c y.tab.c y.tab.h
