#ifndef SYMTAB_H
#define SYMTAB_H

#include "hashmap.h" /* struct hm_item */
#include "astree.h" /* struct astree */

#define SYMBOL_LIT_INT		1
#define SYMBOL_LIT_REAL		2
#define SYMBOL_LIT_CHAR		3
#define SYMBOL_LIT_STRING	4
#define SYMBOL_IDENTIFIER	5
#define SYMBOL_TEMPORARY	6
#define SYMBOL_LABEL		7


#define ID_VAR 1
#define ID_VEC 2
#define ID_FUN 3


#define TP_DOUBLE 	1
#define TP_FLOAT 	3
#define TP_LONG 	5
#define TP_SHORT 	15
#define TP_BYTE 	31
#define TP_BOOLEAN 	32

/* Test masks compatibility*/
#define TP_ALL 		~(0)
#define TP_INCOMP 	0
#define TP_INTEGER 	5
#define TP_FLOATING 1

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
	struct astree *decl;
	int unique_id;
};

void symtab_init(void);

struct hm_item *symtab_insert(const char* symbol, int code);
int symtab_get(const char* symbol, struct symtab_item* dummy);
struct hm_item *symtab_first();
struct hm_item *symtab_next();

struct hm_item* symtab_make_label(void);
struct hm_item* symtab_make_tmp(void);
struct hm_item* symtab_make_const(int i);

void symtab_print(void);
void symtab_fprint_item(FILE *stream, struct symtab_item *item);

void symtab_destroy(void);

#endif
