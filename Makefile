CC := gcc

.PHONY: all
all: hashmap.o lex.yy.o
	$(CC) -o stage1 lex.yy.o hashmap.o

%.o: %.c
	$(CC) -c $<

scanner.c: scanner.l
	lex scanner.l

etapa1.tgz: clean
	tar cvfz etapa1.tgz *

.PHONY: clean
clean:
	rm -rf scanner.c stage1 etapa1.tgz *.o *.out
