#include "astree.h"
#include "symbol_table.h"
#include <stdlib.h>
#include <stdio.h>

/*REMOVE!!!*/
static const char *data_type_to_string[] = {
	"", "double", "", "float", "", "long", "", "",
	"", "", "", "", "", "", "", "short",
	"", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "byte",
	"boolean"
};

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

void ast_terminate(struct astree *tree) {
	if (!tree) {
		return;
	}
	if(tree->type == AST_RETURN){
		free(tree->symbol);
	}

	for (int i = 0; i < AST_MAXCHILDREN; i++) {
		ast_terminate(tree->children[i]);
	}

	free(tree);
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
	"var",
	"vec",
	"vec_init",
	"func",
	"fheader",
	"params",
	"cmd_list",
	"var_attr",
	"vec_attr",
	"read",
	"print",
	"print_list",
	"return",
	"vec_sub",
	"call",
	"args",
	"<",
	">",
	"!",
	"<=",
	">=",
	"==",
	"!=",
	"&&",
	"||",
	"when",
	"when_else",
	"while",
	"for",
	"block",
	"expr_block"
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
		fprintf(stream, ": %s, ", tree->symbol->key);
		symtab_fprint_item(stream, tree->symbol->value);
	}
	fprintf(stream, "\n");

	for (i = 0; i < AST_MAXCHILDREN; i++) {
		ast_fprint(stream, level + 1, tree->children[i]);
	}
}

static void print_identation(FILE* stream, int level){
	int i;
	for(i = 0; i < level; i++)
		fprintf(stream, "  ");
}

void ast_make_source(FILE* stream, struct astree* tree, int level){
	if (tree == NULL)
		return;

	switch (tree->type) {
		case AST_VAR_ATTR: case AST_VEC_ATTR: case AST_READ:
		case AST_PRINT: case AST_RETURN: case AST_BLOCK:
		case AST_WHEN: case AST_WHEN_ELSE: case AST_WHILE: case AST_FOR:
			print_identation(stream, level);
			break;
		default:
			break;
	}

	switch(tree->type){
		/* Init program*/
		case AST_PROG:
			ast_make_source(stream, tree->children[0], level);
			break;
		/* Lists*/
		case AST_CMD_LIST: case AST_DECL_LIST:
			ast_make_source(stream, tree->children[0], level);
			ast_make_source(stream, tree->children[1], level);
			if(tree->children[1] == NULL)
				print_identation(stream, level);
			fprintf(stream, ";\n");
			break;
		/* All types of symbols*/
		case AST_SYM:
			fprintf(stream, "%s", tree->symbol->key);
			break;
		/* Types*/
		case AST_KW_BYTE: case AST_KW_SHORT: case AST_KW_LONG:
		case AST_KW_FLOAT: case AST_KW_DOUBLE:
			fprintf(stream, "%s", type_to_string[tree->type]);
		 	break;
		/* Variable declarition*/
		case AST_VAR:
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, " : ");
			ast_make_source(stream, tree->children[1], level);
			fprintf(stream, " ");
			ast_make_source(stream, tree->children[2], level);
			break;
		/* Vector declarition*/
		case AST_VEC:
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, " : ");
			ast_make_source(stream, tree->children[1], level);
			fprintf(stream, " [");
			ast_make_source(stream, tree->children[2], level);
			fprintf(stream, "]");
			ast_make_source(stream, tree->children[3], level);
			break;
		/* Vector inicialization*/
		case AST_VEC_INIT:
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, " ");
			ast_make_source(stream, tree->children[1], level);
			break;
		/* Function declarition*/
		case AST_FUNC:
			ast_make_source(stream, tree->children[0], level);
			if(tree->children[1] != NULL && tree->children[1]->type == AST_BLOCK)
				ast_make_source(stream, tree->children[1], level);
			else
				ast_make_source(stream, tree->children[1], level + 1);
			break;
		/* Function Header declarition*/
		case AST_FHEADER:
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, " ");
			ast_make_source(stream, tree->children[1], level);
			fprintf(stream, "(");
			ast_make_source(stream, tree->children[2], level);
			fprintf(stream, ")\n");
			break;
		/* Parameters*/
		case AST_PARAMS:
			if(tree->children[0] != NULL) {
				ast_make_source(stream, tree->children[0], level);
				fprintf(stream, ", ");
			}
			ast_make_source(stream, tree->children[1], level);
			fprintf(stream, " ");
			ast_make_source(stream, tree->children[2], level);
			break;
		/* Block of commands*/
		case AST_BLOCK:
			fprintf(stream, "{\n");
			ast_make_source(stream, tree->children[0], level + 1);
			print_identation(stream, level);
			fprintf(stream, "}");
			break;
		/* Attribution command*/
		case AST_VAR_ATTR:
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, " = ");
			ast_make_source(stream, tree->children[1], level);
			break;
		/* Attribution command*/
		case AST_VEC_ATTR:
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, "#");
			ast_make_source(stream, tree->children[1], level);
			fprintf(stream, " = ");
			ast_make_source(stream, tree->children[2], level);
			break;
		/* Read command*/
		case AST_READ:
			fprintf(stream, "%s ", type_to_string[tree->type]);
			ast_make_source(stream, tree->children[0], level);
			break;
		/* Print command*/
		case AST_PRINT:
			fprintf(stream, "%s ", type_to_string[tree->type]);
			ast_make_source(stream, tree->children[0], level);
			break;
		case AST_PRINT_LIST:
			ast_make_source(stream, tree->children[0], level);
			ast_make_source(stream, tree->children[1], level);
			//if(tree->children[0] != NULL)
				fprintf(stream, " ");
			break;
		/* Return statement*/
		case AST_RETURN:
			fprintf(stream, "%s ", type_to_string[tree->type]);
			ast_make_source(stream, tree->children[0], level);
			break;

		/* Expressions*/
		/* Unary operatiors*/
		case AST_NOT:
			fprintf(stream, "%s ", type_to_string[tree->type]);
			ast_make_source(stream, tree->children[0], level);
			break;
		/* Operators*/
		case AST_ADD: case AST_SUB: case AST_MUL: case AST_DIV:
		case AST_LT: case AST_GT: case AST_LE: case AST_GE:
		case AST_EQ: case AST_NE: case AST_AND: case AST_OR:
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, " %s ", type_to_string[tree->type]);
			ast_make_source(stream, tree->children[1], level);
			break;
		/* Call operatior*/
		case AST_CALL:
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, "(");
			ast_make_source(stream, tree->children[1], level);
			fprintf(stream, ")");
			break;
		/* Vector as expression*/
		case AST_VEC_SUB:
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, "[");
			ast_make_source(stream, tree->children[1], level);
			fprintf(stream, "]");
			break;
		case AST_EXP_BLOCK:
			fprintf(stream, "(");
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, ")");
			break;

		/* Arguments*/
		case AST_ARGS:
			ast_make_source(stream, tree->children[0], level);
			if(tree->children[0] != NULL)
				fprintf(stream, ", ");
			ast_make_source(stream, tree->children[1], level);
			break;

		/* Control statements*/
		case AST_WHEN:
			fprintf(stream, "when (");
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, ") then\n");
			if(tree->children[1] == NULL)
				print_identation(stream, level + 1);
			else if(tree->children[1]->type == AST_BLOCK)
				ast_make_source(stream, tree->children[1], level);
			else
				ast_make_source(stream, tree->children[1], level + 1);
			break;
		case AST_WHEN_ELSE:
			fprintf(stream, "when (");
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, ") then\n");
			if(tree->children[1] == NULL)
				print_identation(stream, level + 1);
			else if(tree->children[1]->type == AST_BLOCK)
				ast_make_source(stream, tree->children[1], level);
			else
				ast_make_source(stream, tree->children[1], level + 1);
			fprintf(stream, "\n");
			print_identation(stream, level);
			fprintf(stream, "else\n");
			if(tree->children[2] == NULL)
				print_identation(stream, level + 1);
			else if(tree->children[2]->type == AST_BLOCK)
				ast_make_source(stream, tree->children[2], level);
			else
				ast_make_source(stream, tree->children[2], level + 1);
			break;
		case AST_WHILE:
			fprintf(stream, "while (");
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, ")\n");
			if(tree->children[1] == NULL)
				print_identation(stream, level + 1);
			else if(tree->children[1]->type == AST_BLOCK)
				ast_make_source(stream, tree->children[1], level);
			else
				ast_make_source(stream, tree->children[1], level + 1);
			break;
		case AST_FOR:
			fprintf(stream, "for (");
			ast_make_source(stream, tree->children[0], level);
			fprintf(stream, " = ");
			ast_make_source(stream, tree->children[1], level);
			fprintf(stream, " to ");
			ast_make_source(stream, tree->children[2], level);
			fprintf(stream, ")\n");
			if(tree->children[3] == NULL)
				print_identation(stream, level + 1);
			else if(tree->children[3]->type == AST_BLOCK)
				ast_make_source(stream, tree->children[3], level);
			else
				ast_make_source(stream, tree->children[3], level + 1);
			break;

		default:
			break;
	}
}
