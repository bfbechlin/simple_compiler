CC := gcc

.PHONY: all
all: etapa2

etapa2: hashmap.o parser.o scanner.o symbol_table.o main.o
	$(CC) -o $@ $^

scanner.c: scanner.l
	lex -o $@ $<

parser.c: parser.y
	yacc -d -o $@ $<

%.o: %.c
	$(CC) -c $<

etapa2.tgz: clean
	tar cvfz etapa2.tgz *

.PHONY: clean
clean:
	rm -rf parser.c parser.h scanner.c etapa2 etapa2.tgz *.o  *~
