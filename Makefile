CC := gcc

.PHONY: all
all: etapa1

etapa1: hashmap.o lex.yy.o
	$(CC) -o etapa1 lex.yy.o hashmap.o

%.o: %.c
	$(CC) -c $<

lex.yy.c: scanner.l main.c
	lex scanner.l

etapa1.tgz: clean
	tar cvfz etapa1.tgz *

.PHONY: clean
clean:
	rm -rf etapa1 lex.yy.c scanner.c stage1 etapa1.tgz *.o *.out *~
