#ifndef AST_H
#define AST_H

#include "hashmap.h"

#define AST_MAXCHILDREN 4

#define AST_SYM 0
#define AST_ADD 1
#define AST_SUB 2
#define AST_MUL 3
#define AST_DIV 4
#define AST_PROG 5
#define AST_KW_BYTE 6
#define AST_KW_SHORT 7
#define AST_KW_LONG 8
#define AST_KW_FLOAT 9
#define AST_KW_DOUBLE 10
#define AST_LIT_INT 11
#define AST_LIT_REAL 12
#define AST_LIT_CHAR 13
#define AST_DECL_LIST 14
#define AST_VAR 15

struct astree {
	int type;
	struct hm_item *symbol;
	struct astree *children[AST_MAXCHILDREN];
};

/* Create a tree node with a type, a pointer to the symbol table
 * and children.*/
struct astree *ast_create(int type, struct hm_item *symbol,
	struct astree *c0,
	struct astree *c1,
	struct astree *c2,
	struct astree *c3);

/* Pretty print a tree to a stream. */
void ast_fprint(FILE *stream, int level, struct astree *tree);

#endif /* ifndef AST_H */
