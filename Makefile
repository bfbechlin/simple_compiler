all: lex.yy.c
	gcc -o stage1 lex.yy.c 

lex.yy.c: scanner.l
	lex scanner.l

clean:
	rm lex.yy.c stage1
