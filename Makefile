all: hashmap.o lex.yy.c
	gcc -o stage1 lex.yy.c hashmap.o

lex.yy.c: scanner.l
	lex scanner.l

hashmap.o: hashmap.c
	ggc -c hashmap.c

clean:
	rm lex.yy.c stage1
