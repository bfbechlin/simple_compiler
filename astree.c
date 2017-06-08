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
			break;
		case AST_FHEADER:
			id_node = tree->children[1];
			type_node = tree->children[0];
			break;
		/* The parameters of function was definied as global variables to
			to keep the compiler more simple. If it's needed to keep difference
			in scopes variables does it necessary create others symbols tables.*/
		case AST_PARAMS:
			/*AST_VAR like*/
			id_node = tree->children[2];
			type_node = tree->children[1];
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
		case AST_PARAMS:
			item->id_type = ID_VAR;
			break;
		case AST_VEC:
			item->id_type = ID_VEC;
			break;
		case AST_FHEADER:
			item->id_type = ID_FUN;
			item->decl = tree;
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
			item->data_type = TP_SHORT;
			break;
		case SYMBOL_LIT_REAL:
			item->data_type = TP_FLOAT;
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
		case AST_PARAMS:
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

static int resolve_expr_type(struct astree *tree){
	if (tree == NULL)
		return TP_ALL;

	switch (tree->type) {
		case AST_SYM:
			return ((struct symtab_item *)tree->symbol->value)->data_type;

		/* Identifiers and Symbols
			IMPORTANT: on AST_VEC_SUB testing only the type
			of the vector. The index check will be done in
			other step*/
		case AST_VEC_SUB:
			return ((struct symtab_item *)tree->children[0]->symbol->value)->data_type;

		/* Arithmetical*/
		case AST_ADD:	case AST_SUB:	case AST_MUL:
		case AST_DIV:
			return 	(resolve_expr_type(tree->children[0]) &
					resolve_expr_type(tree->children[1]));

		/* Boolean Operators*/
		case AST_LT: 	case AST_GT: 	case AST_LE:
		case AST_GE:	case AST_EQ:	case AST_NE:
		case AST_AND:	case AST_OR:
			if ((resolve_expr_type(tree->children[0]) &
				resolve_expr_type(tree->children[1])) != TP_INCOMP)
					return TP_BOOLEAN;

			else
				return TP_INCOMP;

		/* Unary Boolean Operator*/
		case AST_NOT:
			if (resolve_expr_type(tree->children[0]) != TP_INCOMP)
				return TP_BOOLEAN;
			else
				return TP_INCOMP;

		/* Function call
			IMPORTANT: verifing only the type of the function,
			arguments will be tested in other step. It's help
			to modularization and some erros like a call function
			that isn't save in a variable.
			No test at symtab_get if it's a valid pointer*/
		case AST_CALL:
			if (((struct symtab_item *)tree->children[0]->symbol->value)->id_type != ID_FUN)
				return TP_INCOMP;
			return ((struct symtab_item *)tree->children[0]->symbol->value)->data_type;

		case AST_EXP_BLOCK:
			return resolve_expr_type(tree->children[0]);
	}
}

/* Return:
 * - 0, if control reaches end with the right return.
 * - 1, if control might reach end without returning.
 * - 2, if control might reach end with wrong return. */
static int check_if_return_is(int ret_type, struct astree *cmd) {
	switch (cmd->type) {
		case AST_RETURN:
			//if (ret_type is compatible with type(cmd->children[0])) {
			//	return 0;
			//} else {
			//	return 2;
			//}
			break;
		case AST_BLOCK:
			return check_if_return_is(ret_type, cmd->children[0]);
			break;
		case AST_CMD_LIST:
			;
			int last_command = cmd->children[0] == NULL;
			int right_return = check_if_return_is(ret_type, cmd->children[1]);
			if ((right_return == 1) && last_command) {
				return 1;
			}
			int left_return = check_if_return_is(ret_type, cmd->children[0]);
			if (right_return == 1 && left_return == 1) {
				return 1;
			}
			return right_return; /* either returns for sure or might return the wrong type */
		case AST_WHEN:
			;
			int ret = check_if_return_is(ret_type, cmd->children[1]);
			if (ret == 2) {
				return 2;
			} else {
				return 1;
			}
			break;
		case AST_WHEN_ELSE:
			;
			int when_ret = check_if_return_is(ret_type, cmd->children[1]);
			int else_ret = check_if_return_is(ret_type, cmd->children[2]);
			if (when_ret == 1 || else_ret == 1) {
				return 1;
			}

			if (when_ret == 2 || else_ret == 2) {
				return 2;
			}

			if (when_ret == 0 && else_ret == 0) {
				return 0;
			}
			break;
		case AST_FOR:
			return check_if_return_is(ret_type, cmd->children[3]);
			break;
		case AST_WHILE:
			return check_if_return_is(ret_type, cmd->children[1]);
			break;
		default:
			return 1;
	}
}

static int ast_keyword_to_data_type(int keyword){
	switch (keyword) {
		case AST_KW_BYTE:
			return TP_BYTE;
		case AST_KW_SHORT:
			return TP_SHORT;
		case AST_KW_LONG:
			return TP_LONG;
		case AST_KW_FLOAT:
			return TP_FLOAT;
		case AST_KW_DOUBLE:
			return TP_DOUBLE;
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

	if ((tree->type == AST_WHEN)
	    || (tree->type == AST_WHEN_ELSE)
		|| (tree->type == AST_WHILE)) {
		/* TODO: check if tree->children[0] is a boolean */
		if (resolve_expr_type(tree->children[0]) != TP_BOOLEAN)
			exit(4);
	}

	if (tree->type == AST_FOR) {
		/* TODO: checkfaz necessário if tree->children[1] and tree->children[2] are numeric */
		/* TODO: check if tree->children[0] is a varible compatible with
		 * tree->children[1] and tree->children[2] */
		int type;
		/* Test if identifier is a interger compatible type*/
		type = ((struct symtab_item *)tree->children[0]->symbol->value)->data_type;
		if((type & TP_INTEGER) != TP_INTEGER){
			fprintf(stderr, "SEMANTIC ERROR: At a FOR statement identifier %s isn't a integer type.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}
		/* Test if expressions are a interger compatible type*/
		type = resolve_expr_type(tree->children[1]);
		if((type & TP_INTEGER) != TP_INTEGER){
			fprintf(stderr,
				"SEMANTIC ERROR: At a FOR statement iterator isn't a interger type.\n");
			exit(4);
		}
		type = resolve_expr_type(tree->children[2]);
		if((type & TP_INTEGER) != TP_INTEGER){
			fprintf(stderr,
				"SEMANTIC ERROR: At a FOR statement iterator isn't a interger type.\n");
			exit(4);
		}
	}

	if (tree->type == AST_CALL) {
		/* TODO: check if tree->children[0] is a function identifier */
		/* TODO: check if tree->children[1] is compatible with the function declaration */
		int func_type, call_type, arg_counter = 0;
		struct symtab_item* info = (struct symtab_item *)tree->children[0]->symbol->value;
		if(info->id_type != ID_FUN){
			fprintf(stderr, "SEMANTIC ERROR: Identifier '%s' isn't a function name.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}

		struct astree* func_args = ((struct astree*)info->decl)->children[2];
		struct astree* call_args = tree->children[1];

		/* Iterating in arguments*/
		while((call_args != NULL) && (func_args != NULL)){
			func_type = ast_keyword_to_data_type(func_args->children[1]->type);
			call_type = resolve_expr_type(call_args->children[1]);

			if((func_type & call_type) == TP_INCOMP){
				fprintf(stderr, "SEMANTIC ERROR: Call for funcition '%s' has incompatible data argument %s.\n",
					tree->children[0]->symbol->key, func_args->children[2]->symbol->key);
				exit(4);
			}

			/*Next parameter*/
			func_args = func_args->children[0];
			call_args = call_args->children[0];
			arg_counter ++;
		}
		if(((call_args != NULL) && (func_args == NULL)) || ((call_args == NULL) && (func_args != NULL))){
			fprintf(stderr, "SEMANTIC ERROR: Call for funcition '%s' has incompatible number of arguments.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}

	}

	if (tree->type == AST_VEC_SUB) {
		/* TODO: check if tree->children[0] is a vector identifier */
		/* TODO: check if tree->children[1] is an integer */
		int vec_type, index_type;
		struct symtab_item* info = (struct symtab_item *)tree->children[0]->symbol->value;

		if(info->id_type != ID_VEC){
			fprintf(stderr, "SEMANTIC ERROR: Identifier '%s' isn't a vector.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}

		vec_type = info->data_type;
		index_type = resolve_expr_type(tree->children[1]);

		if ((index_type & TP_INTEGER) != TP_INTEGER){
			fprintf(stderr,
				"SEMANTIC ERROR: Index of vector '%s' isn't a integer type.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}
	}

	if (tree->type == AST_VEC_ATTR) {
		/* TODO: check if tree->children[0] is a vector identifier */
		/* TODO: check if tree->children[1] is an integer */
		/* TODO: check if tree->children[2] has the same type as the vector */
		int vec_type, exp_type, index_type;
		struct symtab_item* info = (struct symtab_item *)tree->children[0]->symbol->value;

		if(info->id_type != ID_VEC){
			fprintf(stderr, "SEMANTIC ERROR: Identifier '%s' isn't a vector.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}


		vec_type = info->data_type;
		index_type = resolve_expr_type(tree->children[1]);
		exp_type = resolve_expr_type(tree->children[2]);

		if ((index_type & TP_INTEGER) != TP_INTEGER){
			fprintf(stderr,
				"SEMANTIC ERROR: Index of vector '%s' isn't a integer type.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}

		if(vec_type & exp_type == TP_INCOMP){
			fprintf(stderr,
				"SEMANTIC ERROR: Attribution to vector '%s' with a incompatible type.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}

	}

	if (tree->type == AST_VAR_ATTR) {
		/* TODO: check if tree->children[0] is a variable identifier */
		/* TODO: check if tree->children[1] has the same type as the varible */
		int var_type, exp_type;
		struct symtab_item* info = (struct symtab_item *)tree->children[0]->symbol->value;

		if(info->id_type != ID_VAR){
			fprintf(stderr, "SEMANTIC ERROR: Identifier '%s' isn't a variable.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}

		var_type = info->data_type;
		exp_type = resolve_expr_type(tree->children[1]);

		if(var_type & exp_type == TP_INCOMP){
			fprintf(stderr,
				"SEMANTIC ERROR: Attribution to variable '%s' with a incompatible type.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}
	}

	if (tree->type == AST_FUNC) {
		/* TODO: check if tree->children[1] has as a return statement compatible
		 * with tree->children[0]->children[1] type. That is, the return
		 * statement has a type equal to the function identifier's data_type */
		struct astree *id_node = tree->children[0]->children[1];
		struct astree *cmd_node = tree->children[1];

		struct symtab_item *info = (struct symtab_item *)id_node->symbol->value;
		char *id_name = id_node->symbol->key;
		int ret_type = info->data_type;

		int compat = check_if_return_is(ret_type, cmd_node);
		if (compat == 1) {
			/* no return found */
			fprintf(stderr, "SEMANTIC ERROR: %s doesn't return.\n", id_name);
			exit(4);
		} else if (compat == 2) {
			/* return incompatible */
			fprintf(stderr, "SEMANTIC ERROR: %s has wrong return type.\n", id_name);
			exit(4);
		}
	}
}
