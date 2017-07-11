#include "astree.h"
#include "symbol_table.h"
#include <stdlib.h>
#include <stdio.h>

static void first_pass(struct astree* tree, struct hashmap *declared_variables, struct astree* func);
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
		fprintf(stderr, "SEMANTIC ERROR: Redeclaration of identifier %s.\n",
			id_node->symbol->key);
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
			item->data_type = TP_LONG;
			break;
		case SYMBOL_LIT_REAL:
			item->data_type = TP_DOUBLE;
			break;
		case SYMBOL_LIT_CHAR:
			item->data_type = TP_BYTE;
			break;
		case SYMBOL_LIT_STRING:
			item->data_type = TP_STRING;
			break;
		case SYMBOL_IDENTIFIER:
			break;
	}
}

void ast_semantic_check(struct astree *tree) {
	struct hashmap declared_variables;
	struct astree* func;
	hm_initialize(20, 0.6, sizeof(char), &declared_variables);

	first_pass(tree, &declared_variables, func);
	second_pass(tree, &declared_variables);

	hm_terminate(&declared_variables);
}

/* Traverses tree checking:
 * 1. redeclarations
 * 2. data_type in symbol table
 * 3. id_type in symbol table */
static void first_pass(struct astree *tree, struct hashmap *declared_variables, struct astree* func) {
	int i;
	if (tree == NULL) {
		return;
	}

	struct hm_item* symbol;
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
		case AST_FUNC:
			func = tree;
			break;
		case AST_RETURN:
			symbol = (struct hm_item *)calloc(1, sizeof(struct hm_item));
			symbol->value = func;
			tree->symbol = symbol;
			break;
	}

	/* Annotating children before the root seems more intuitive, but I don't think
	 * it makes any difference. */
	for (i = 0; i < AST_MAXCHILDREN; i++)
		first_pass(tree->children[i], declared_variables, func);

}


static void check_if_declared(struct astree *tree, struct hashmap *declared_variables) {
	char *id = tree->symbol->key;
	if (hm_getref(declared_variables, id) == NULL) {
		fprintf(stderr, "SEMANTIC ERROR: Identifier %s isn't declared.\n", id);
		exit(4);
	}
}

int resolve_expr_type(struct astree *tree){
	if (tree == NULL)
		return TP_ALL;

	switch (tree->type) {
		case AST_SYM:
			switch(((struct symtab_item *)tree->symbol->value)->code){
				case SYMBOL_LIT_INT:
				case SYMBOL_LIT_REAL:
				case SYMBOL_LIT_CHAR:
					return ((struct symtab_item *)tree->symbol->value)->data_type;
				case SYMBOL_IDENTIFIER:
					if(((struct symtab_item *)tree->symbol->value)->id_type == ID_VAR)
						return ((struct symtab_item *)tree->symbol->value)->data_type;
				case SYMBOL_LIT_STRING:
					return TP_INCOMP;
			}
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
	int i;
	if (tree == NULL) {
		return;
	}

	/* Annotating children before the root seems more intuitive, but I don't think
	 * it makes any difference. */
	for (i = 0; i < AST_MAXCHILDREN; i++) {
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
			func_type = ((struct symtab_item*)func_args->children[2]->symbol->value)->data_type;
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

		if((vec_type & exp_type) == TP_INCOMP){
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

		if((var_type & exp_type) == TP_INCOMP){
			fprintf(stderr,
				"SEMANTIC ERROR: Attribution to variable '%s' with a incompatible type.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}
	}

	if (tree->type == AST_RETURN) {
		/* TODO: check if tree->children[1] has as a return statement compatible
		 * with tree->children[0]->children[0] type. That is, the return
		 * statement has a type equal to the function identifier's data_type */
		int func_type, ret_type;
		struct astree* func = (struct astree*)tree->symbol->value;

		func_type = ((struct symtab_item*)func->children[0]->children[1]->symbol->value)->data_type;
		ret_type = resolve_expr_type(tree->children[0]);

		if((func_type & ret_type) == TP_INCOMP){
			fprintf(stderr,
				"SEMANTIC ERROR: Return of function '%s' has incompatible type.\n",
				func->children[0]->children[1]->symbol->key);
			exit(4);
		}
	}

	if (tree->type == AST_READ){
		/* TODO: check if tree->children[0] is a variable*/
		struct symtab_item* info = (struct symtab_item *)tree->children[0]->symbol->value;

		if (info->id_type != ID_VAR){
			fprintf(stderr,
				"SEMANTIC ERROR: Identifier '%s' isn't a variable to be user in read statement.\n",
				tree->children[0]->symbol->key);
			exit(4);
		}
	}
}
