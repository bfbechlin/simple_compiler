#include "astree.h"
#include <stdlib.h>
#include <stdio.h>

struct astree *ast_create(int type, struct hm_item *symbol,
	struct astree *c0,
	struct astree *c1,
	struct astree *c2,
	struct astree *c3)
{
	struct astree *node = calloc(1, sizeof(struct astree));

	if (!node) {
		perror("astree: node alocation");
		return NULL;
	}

	node->type = type;
	node->symbol = symbol;
	node->children[0] = c0;
	node->children[1] = c1;
	node->children[2] = c2;
	node->children[3] = c3;

	return node;
}

static const char *type_to_string[] = {
	"symbol",
	"+",
	"-",
	"*",
	"/",
	"prog",
	"byte",
	"short",
	"long",
	"float",
	"double",
	"decl_list",
	"var"
};

void ast_fprint(FILE *stream, int level, struct astree *tree) {
	if (!tree) {
		return;
	}

	int i;
	for (i = 0; i < level; i++) {
		fprintf(stream, "| ");
	}

	fprintf(stream, "%s", type_to_string[tree->type]);
	if (tree->type == AST_SYM) {
		fprintf(stream, ": %s", tree->symbol->key);
	}
	fprintf(stream, "\n");

	for (i = 0; i < AST_MAXCHILDREN; i++) {
		ast_fprint(stream, level + 1, tree->children[i]);
	}
}

void ast_make_source(FILE* stream, struct astree* tree){
	if (!tree)
		return;

	switch(tree->type){
		case AST_SYM:
			fprintf(stream, "%s", tree->symbol->key);
			break;
		/* Operators*/
		case AST_ADD: case AST_SUB: case AST_MUL: case AST_DIV:
			ast_make_source(stream, tree->children[0]);
			fprintf(stream, "%s", type_to_string[tree->type]);
			ast_make_source(stream, tree->children[1]);
			break;
		/* Types*/
		case AST_KW_BYTE: case AST_KW_SHORT: case AST_KW_LONG:
		case AST_KW_FLOAT: case AST_KW_DOUBLE:
			fprintf(stream, "%s", type_to_string[tree->type]);
		 	break;
		/* Variable declarition*/
		case AST_VAR:
			ast_make_source(stream, tree->children[0]);
			fprintf(stream, " : ");
			ast_make_source(stream, tree->children[1]);
			fprintf(stream, " ");
			ast_make_source(stream, tree->children[2]);
			break;

		/* CONTROL TYPES*/
		case AST_PROG:
			ast_make_source(stream, tree->children[0]);
			break;
		case AST_DECL_LIST:
			ast_make_source(stream, tree->children[0]);
			ast_make_source(stream, tree->children[1]);
			fprintf(stream, ";\n");
			break;
		default:
			break;
	}

}
