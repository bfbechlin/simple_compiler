CC := gcc

.PHONY: all
all: etapa2

debug: hashmap.o scanner.c parser.c main.o
	$(CC) -o etapa2 main.o hashmap.o -g

etapa2: hashmap.o scanner.c parser.c main.o
	$(CC) -o etapa2 main.o hashmap.o

scanner.c: scanner.l
	lex -o scanner.c scanner.l

parser.c: parser.y
	yacc -d -o parser.c parser.y

%.o: %.c
	$(CC) -c $<

etapa2.tgz: clean
	tar cvfz etapa2.tgz *

.PHONY: clean
clean:
	rm -rf parser.c parser.h scanner.c etapa2 etapa2.tgz *.o  *~
