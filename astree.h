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
#define AST_DECL_LIST 11
#define AST_VAR 12
#define AST_VEC 13
#define AST_VEC_INIT 14
#define AST_FUNC 15
#define AST_FHEADER 16
#define AST_PARAMS 17
#define AST_CMD_LIST 18
#define AST_VAR_ATTR 19
#define AST_VEC_ATTR 20
#define AST_READ 21
#define AST_PRINT 22
#define AST_PRINT_LIST 23
#define AST_RETURN 24
#define AST_VEC_SUB 25
#define AST_CALL 26
#define AST_ARGS 27

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

/* Recreate the program trought tree to a stream. */
void ast_make_source(FILE* stream, struct astree* tree);
#endif /* ifndef AST_H */
