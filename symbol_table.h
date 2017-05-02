#ifndef SYMTAB_H
#define SYMTAB_H

#define SYMBOL_LIT_INT		1
#define SYMBOL_LIT_REAL		2
#define SYMBOL_LIT_CHAR		3
#define SYMBOL_LIT_STRING	4
#define SYMBOL_IDENTIFIER	5

void symtab_init(void);
void symtab_insert(char* symbol, int code);
void symtab_print(void);
void symtab_destroy(void);

#endif
