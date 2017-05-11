#ifndef AST_H
#define AST_H

#include "hashmap.h"

#define AST_MAXCHILDREN 4

#define AST_SYM 0
#define AST_ADD 1
#define AST_SUB 2
#define AST_MUL 3
#define AST_DIV 4

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
