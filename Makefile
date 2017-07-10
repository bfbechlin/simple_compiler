CC := gcc

etapa6: assembly.o hashmap.o astree.o tac.o symbol_table.o semantic.o parser.o scanner.o main.o
	$(CC) -g -o $@ $^

scanner.c: scanner.l
	lex -o $@ $<

parser.c: parser.y
	yacc -d -o $@ $<

%.o: %.c
	$(CC) -c $<

etapa5.tgz: clean
	tar cvfz etapa5.tgz *

.PHONY: clean
clean:
	rm -rf parser.c parser.h scanner.c etapa* etapa*.tgz *.o  *~
