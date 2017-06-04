#ifndef SYMTAB_H
#define SYMTAB_H

#include "hashmap.h" /* hm_item */

#define SYMBOL_LIT_INT		1
#define SYMBOL_LIT_REAL		2
#define SYMBOL_LIT_CHAR		3
#define SYMBOL_LIT_STRING	4
#define SYMBOL_IDENTIFIER	5

#define ID_VAR 1;
#define ID_VEC 2;
#define ID_FUN 3;

#define TP_BYTE 1;
#define TP_SHORT 2;
#define TP_LONG 3;
#define TP_FLOAT 4;
#define TP_DOUBLE 5;

/**
 * Symbols are:
 *     - identifiers
 *     - integer literals
 *     - real literals
 *     - character literals
 *     - string literals
 *
 * The fields in a symbol table item are:
 *     - code: the type of symbol. It's filled for all symbols.
 *     - data_type: the type of data attached to that symbol. It's filled for all
 *       symbols.
 *     - id_type: the "nature" of the identifier: varible, vector or function.
 *       It's only *not* zero when code == SYMBOL_IDENTIFIER.
 *     - decl: pointer to the symbol declaration in the astree. It's only *not*
 *       NULL when code == SYMBOL_IDENTIFIER.
 */

struct symtab_item {
	int code;
	int data_type;
	int id_type;
	struct hm_item *decl;
};

void symtab_init(void);
struct hm_item *symtab_insert(char* symbol, int code);
void symtab_print(void);
void symtab_fprint_item(FILE *stream, struct symtab_item *item);
void symtab_destroy(void);

#endif
