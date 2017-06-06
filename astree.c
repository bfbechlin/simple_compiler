#include "astree.h"
#include "symbol_table.h"
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

void ast_terminate(struct astree *tree) {
	if (!tree) {
		return;
	}

	int i;
	for (i = 0; i < AST_MAXCHILDREN; i++) {
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

void print_identation(FILE* stream, int level){
	int i;
	for(i = 0; i < level; i++)
		fprintf(stream, "  ");
}

static void first_pass(struct astree* tree, struct hashmap *declared_variables);
static void second_pass(struct astree *tree, struct hashmap *declared_variables);

/* `yes` is a dummy variable. Our hashmap implementation is being used here as
 * a hashset. I.e., we only care about the keys. So `yes` is just a constant
 * with no meaning to be put in the value field. */
static char yes = 1;

/* Annotate symbol table when tree is a declaration (i.e., of type AST_VAR,
 * AST_VEC or AST_FHEADER). */
static void annotate_declaration(struct astree *tree, struct hashmap *declared_variables) {
	struct astree *id_node;
	struct astree *type_node;
	switch (tree->type) {
		case AST_VAR:
			id_node = tree->children[0];
			type_node = tree->children[1];
			break;
		case AST_VEC:
			id_node = tree->children[0];
			type_node = tree->children[1];
			first_pass(tree->children[2], declared_variables);
			first_pass(tree->children[3], declared_variables);
			break;
		case AST_FHEADER:
			id_node = tree->children[1];
			type_node = tree->children[0];

			/* TODO: Since here tree->children[2]->children[2] is a list of parameters,
			 * we might have to create a new scope for those variables declared here.
			 * Right now, this accomplishes nothing, since this node is of type
			 * AST_IDENTIFIER. Test program with the argument of `tests/fun_dec.txt`
			 * to see. */
			first_pass(tree->children[2], declared_variables);
			break;
	}

	/* check redeclarations */
	if (hm_getref(declared_variables, id_node->symbol->key) != NULL) {
		fprintf(stderr, "Redeclaration: %s\n", id_node->symbol->key);
		exit(4);
	}
	hm_put(declared_variables, id_node->symbol->key, &yes);

	struct symtab_item *item = id_node->symbol->value;
	switch (tree->type) {
		case AST_VAR:
			item->id_type = ID_VAR;
			break;
		case AST_VEC:
			item->id_type = ID_VEC;
			break;
		case AST_FHEADER:
			item->id_type = ID_FUN;
			break;
	}

	switch (type_node->type) {
		case AST_KW_BYTE:
			item->data_type = TP_BYTE;
			break;
		case AST_KW_SHORT:
			item->data_type = TP_SHORT;
			break;
		case AST_KW_LONG:
			item->data_type = TP_LONG;
			break;
		case AST_KW_FLOAT:
			item->data_type = TP_FLOAT;
			break;
		case AST_KW_DOUBLE:
			item->data_type = TP_DOUBLE;
			break;
	}
}

/* Annotate symbol table when tree is a literal or an id (i.e., of
 * type AST_SYM) */
static void annotate_symbol(struct astree *tree) {
	struct symtab_item *item = tree->symbol->value;
	switch (item->code) {
		case SYMBOL_LIT_INT:
			item->data_type = TP_LONG;
			break;
		case SYMBOL_LIT_REAL:
			item->data_type = TP_DOUBLE;
			break;
		case SYMBOL_LIT_CHAR:
			item->data_type = TP_BYTE;
			break;
		case SYMBOL_LIT_STRING:
			break;
		case SYMBOL_IDENTIFIER:
			break;
	}
}

void ast_semantic_check(struct astree *tree) {
	struct hashmap declared_variables;
	hm_initialize(20, 0.6, sizeof(char), &declared_variables);

	first_pass(tree, &declared_variables);
	second_pass(tree, &declared_variables);

	hm_terminate(&declared_variables);
}

/* Traverses tree checking:
 * 1. redeclarations
 * 2. data_type in symbol table
 * 3. id_type in symbol table */
static void first_pass(struct astree *tree, struct hashmap *declared_variables) {
	if (tree == NULL) {
		return;
	}

	/* Annotating children before the root seems more intuitive, but I don't think
	 * it makes any difference. */
	for (int i = 0; i < AST_MAXCHILDREN; i++) {
		first_pass(tree->children[i], declared_variables);
	}

	switch (tree->type) {
		case AST_SYM:
			annotate_symbol(tree);
			break;
		case AST_VAR:
		case AST_VEC:
		case AST_FHEADER:
			annotate_declaration(tree, declared_variables);
			break;
	}
}

static void check_if_declared(struct astree *tree, struct hashmap *declared_variables) {
	char *id = tree->symbol->key;
	if (hm_getref(declared_variables, id) == NULL) {
		fprintf(stderr, "%s not declared\n", id);
		exit(4);
	}
}

/* Traverses tree checking if:
 * 1. variables used are declared
 * 2. TODO: variables are used correctly according to their id_type
 * 3. TODO: variables are used correctly according to their data_type
 * 4. TODO: functions return the data type they were declared with
 * 5. TODO: funcion calls have the correct type of arguments
 * 6. TODO: vectors indexing is done using integers */
static void second_pass(struct astree *tree, struct hashmap *declared_variables) {
	if (tree == NULL) {
		return;
	}

	/* Annotating children before the root seems more intuitive, but I don't think
	 * it makes any difference. */
	for (int i = 0; i < AST_MAXCHILDREN; i++) {
		second_pass(tree->children[i], declared_variables);
	}

	/* it's a symbol and it's an identifier */
	if ((tree->type == AST_SYM) &&
	    (((struct symtab_item *)tree->symbol->value)->code == SYMBOL_IDENTIFIER)) {
		check_if_declared(tree, declared_variables);
	}

	if (   (tree->type == AST_ADD)
		|| (tree->type == AST_SUB)
		|| (tree->type == AST_MUL)
		|| (tree->type == AST_DIV)
		|| (tree->type == AST_LT)
		|| (tree->type == AST_GT)
		|| (tree->type == AST_LE)
		|| (tree->type == AST_GE)
		|| (tree->type == AST_EQ)
		|| (tree->type == AST_NE)) {
		/* TODO: check if tree->children[0] and tree->children[1] are numeric */
	}

	if (   (tree->type == AST_WHEN)
	    || (tree->type == AST_WHEN_ELSE)
		|| (tree->type == AST_WHILE)) {
		/* TODO: check if tree->children[0] is a boolean */
	}

	if (tree->type == AST_FOR) {
		/* TODO: check if tree->children[1] and tree->children[2] are numeric */
		/* TODO: check if tree->children[0] is a varible compatible with
		 * tree->children[1] and tree->children[2] */
	}

	if (tree->type == AST_CALL) {
		/* TODO: check if tree->children[0] is a function identifier */
		/* TODO: check if tree->children[1] is compatible with the function declaration */
	}

	if (tree->type == AST_VEC_SUB) {
		/* TODO: check if tree->children[0] is a vector identifier */
	}

	if (tree->type == AST_VEC_ATTR) {
		/* TODO: check if tree->children[0] is a vector identifier */
		/* TODO: check if tree->children[1] is an integer */
		/* TODO: check if tree->children[2] has the same type as the vector */
	}

	if (tree->type == AST_VAR_ATTR) {
		/* TODO: check if tree->children[0] is a variable identifier */
		/* TODO: check if tree->children[1] has the same type as the varible */
	}

	if (tree->type == AST_FUNC) {
		/* TODO: check if tree->children[1] has as a return statement compatible
		 * with tree->children[0]->children[0] type. That is, the return
		 * statement has a type equal to the function identifier's data_type */
	}
}
