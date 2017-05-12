CC := gcc

etapa3: hashmap.o astree.o symbol_table.o parser.o scanner.o main.o
	$(CC) -g -o $@ $^

scanner.c: scanner.l
	lex -o $@ $<

parser.c: parser.y
	yacc -d -o $@ $<

%.o: %.c
	$(CC) -c $<

etapa3.tgz: clean
	tar cvfz etapa3.tgz *

.PHONY: clean
clean:
	rm -rf parser.c parser.h scanner.c etapa3 etapa3.tgz *.o  *~
