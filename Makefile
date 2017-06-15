CC := gcc

etapa4: hashmap.o astree.o symbol_table.o parser.o scanner.o main.o
	$(CC) -g -o $@ $^

scanner.c: scanner.l
	lex -o $@ $<

parser.c: parser.y
	yacc -d -o $@ $<

%.o: %.c
	$(CC) -c $<

etapa4.tgz: clean
	tar cvfz etapa4.tgz *

.PHONY: clean
clean:
	rm -rf parser.c parser.h scanner.c etapa* etapa*.tgz *.o  *~
