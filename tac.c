#include <stdlib.h>
#include <stdio.h>
#include "symbol_table.h"
#include "tac.h"
#include "astree.h"
#include "semantic.h"

static int ast_to_tac[] = {
	TAC_SYMBOL,
	TAC_ADD,
	TAC_SUB,
	TAC_MUL,
	TAC_DIV,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	TAC_LT,
	TAC_GT,
	TAC_NOT,
	TAC_LE,
	TAC_GE,
	TAC_EQ,
	TAC_NE,
	TAC_AND,
	TAC_OR
};

static char* tac_to_string[] = {
	"SYMBOL",
	"MOVE",
	"LABEL",
	"BEGIN_FUN",
	"END_FUN",
	"IFZ",
	"JUMP",
	"CALL",
	"ARG",
	"RET",
	"PRINT",
	"READ",
	"VEC_READ",
	"VEC_WRITE",
	"",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"INC",
	"LT",
	"GT",
	"NOT",
	"LE",
	"GE",
	"EQ",
	"NE",
	"AND",
	"OR",
	"VAR",
	"VAR_INIT",
	"VEC",
	"VEC_INIT"
};

struct tac* tac_join(struct tac* l1, struct tac* l2){
	struct tac* item;
	if(l2 == NULL)
		return l1;
	/* Find last item*/
	for(item = l2; item->next != NULL; item = item->next){}
	item->next = l1;
	return l2;
}

struct tac* tac_create(int type, struct hm_item* res, struct hm_item* op1,
	struct hm_item* op2){

	struct tac *node = calloc(1, sizeof(struct tac));

	if (!node) {
		perror("tac: node alocation");
		return NULL;
	}
	node->type = type;
	node->res = res;
	node->op1 = op1;
	node->op2 = op2;

	return node;
}

void tac_terminate(struct tac* list){
	struct tac* item;

	for(item = list; item != NULL; item = item->next)
		free(item);
}

void tac_fprint(FILE *stream, struct tac* list){
	struct tac* item;

	for(item = list; item != NULL; item = item->next){
		if(item->type != TAC_SYMBOL)
			fprintf(stream, "%s %s %s %s\n",
				tac_to_string[item->type],
				item->res ? item->res->key : "",
				item->op1 ? item->op1->key : "",
				item->op2 ? item->op2->key : "");
	}
}

struct tac* tac_reorder(struct tac* list){
	struct tac *prev, *next, *iter;
	if(list == NULL)
		return NULL;
	prev = NULL;
	for(iter = list; iter != NULL; iter = next){
		next = iter->next;
		iter->next = prev;
		prev = iter;
	}
	return prev;
}

static int suitable_for_unrolling(struct astree *tree) {
	struct astree *expr1 = tree->children[1];
	struct astree *expr2 = tree->children[2];

	char are_symbol = expr1->type == AST_SYM && expr2->type == AST_SYM;
	if (!are_symbol) goto unsuitable;

	struct symtab_item *sym1 = (struct symtab_item *)expr1->symbol->value;
	struct symtab_item *sym2 = (struct symtab_item *)expr2->symbol->value;

	char are_constants = sym1->code == SYMBOL_LIT_INT && sym2->code == SYMBOL_LIT_INT;
	if (!are_constants) goto unsuitable;

	int val1 = atoi(expr1->symbol->key);
	int val2 = atoi(expr2->symbol->key);
	char are_close = (val2 - val1 <= 16);

	if (!are_close) goto unsuitable;

	return 1;

unsuitable:
	return 0;
}

static struct tac *unroll_for(struct astree *tree) {
	int i;

	int val1 = atoi(tree->children[1]->symbol->key);
	int val2 = atoi(tree->children[2]->symbol->key);

	struct hm_item *ident = tree->children[0]->symbol;
	struct tac *cmd = tac_populate(tree->children[3]);
	struct tac *inc = tac_create(TAC_INC, ident, NULL, NULL);

	struct tac *unrolled_for = tac_join(cmd, inc);

	for (i = val1 + 1; i <= val2; i++) {
		cmd = tac_populate(tree->children[3]);
		inc = tac_create(TAC_INC, ident, NULL, NULL);
		unrolled_for = tac_join(unrolled_for,
		               tac_join(cmd, inc));
	}

	return unrolled_for;
}

static struct tac* control(struct astree* tree){
	struct tac* expr;
	struct tac* cmd[2];
	struct hm_item* label[2];
	struct hm_item* ident;
	struct hm_item* tmp;

	switch (tree->type){
		case AST_WHEN:
			label[0] = symtab_make_label();
			expr = tac_populate(tree->children[0]);
			cmd[0] = tac_populate(tree->children[1]);
			return tac_join(expr, tac_join(
				tac_create(TAC_IFZ, label[0], expr->res, NULL), tac_join(
					cmd[0], tac_create(TAC_LABEL, label[0], NULL, NULL))));

		case AST_WHEN_ELSE:
			label[0] = symtab_make_label();
			label[1] = symtab_make_label();
			expr = tac_populate(tree->children[0]);
			cmd[0] = tac_populate(tree->children[1]);
			cmd[1] = tac_populate(tree->children[2]);

			cmd[0] = tac_join(expr, tac_join(
				tac_create(TAC_IFZ, label[0], expr->res, NULL), cmd[0]));
			return tac_join(cmd[0],
				tac_join(tac_create(TAC_JUMP, label[1], NULL, NULL),
					tac_join(tac_create(TAC_LABEL, label[0], NULL, NULL),
						tac_join(cmd[1],
						tac_create(TAC_LABEL, label[1], NULL, NULL)))));

		case AST_WHILE:
			label[0] = symtab_make_label();
			label[1] = symtab_make_label();
			expr = tac_populate(tree->children[0]);
			cmd[0] = tac_populate(tree->children[1]);

			cmd[0] = tac_join(tac_create(TAC_LABEL, label[0], NULL, NULL),
				tac_join(expr, tac_join(
					tac_create(TAC_IFZ, label[1], expr->res, NULL), cmd[0])));

			return tac_join(cmd[0], tac_join(tac_create(TAC_JUMP, label[0], NULL,
				NULL), tac_create(TAC_LABEL, label[1], NULL, NULL)));

		case AST_FOR:
			if (suitable_for_unrolling(tree)) {
				return unroll_for(tree);
			} else {
				label[0] = symtab_make_label();
				label[1] = symtab_make_label();
				ident = tree->children[0]->symbol;

				tmp = symtab_make_tmp();
				((struct symtab_item*)tmp->value)->data_type = TP_BOOLEAN;

				expr = tac_populate(tree->children[1]);
				cmd[0] = tac_populate(tree->children[3]);
				cmd[1] = tac_join(expr, tac_join(
					tac_create(TAC_MOVE, ident, expr->res, NULL),
					tac_create(TAC_LABEL, label[0], NULL, NULL)));
				expr = tac_populate(tree->children[2]);
				cmd[1] = tac_join(cmd[1], tac_join(expr, tac_join(
					tac_create(TAC_LE, tmp, ident, expr->res),
					tac_create(TAC_IFZ, label[1], tmp, NULL))));
				return tac_join(cmd[1], tac_join(cmd[0], tac_join(
					tac_create(TAC_INC, ident, NULL, NULL), tac_join(
					tac_create(TAC_JUMP, label[0], NULL, NULL),
					tac_create(TAC_LABEL, label[1], NULL, NULL)))));
			}

		default:
			return NULL;
	}
}

static struct tac* symbol(struct astree* tree){
	struct hm_item* tmp;
	switch(((struct symtab_item*)tree->symbol->value)->code){
		case SYMBOL_LIT_INT:
		case SYMBOL_LIT_REAL:
		case SYMBOL_LIT_CHAR:
		case SYMBOL_LIT_STRING:
		default:
			return tac_create(TAC_SYMBOL, tree->symbol, NULL, NULL);
	}
}

struct tac* function(struct astree* tree){
	struct tac* item;
	struct tac* concat;
	struct astree* iter;
	struct hm_item* tmp;
	struct hm_item* func;

	switch(tree->type){
		case AST_FUNC:
			return tac_join(tac_create(TAC_BEGINFUN,
				tree->children[0]->children[1]->symbol, NULL, NULL),
				tac_join(tac_populate(tree->children[1]),
					tac_create(TAC_ENDFUN, tree->children[0]->children[1]->symbol,
						NULL, NULL)));

		case AST_RETURN:
			item = tac_populate(tree->children[0]);
			return tac_join(item, tac_create(TAC_RET, item->res, NULL, NULL));

		case AST_CALL:
			tmp = symtab_make_tmp();
			func = (struct hm_item*) tree->children[0]->symbol;
			struct astree* fheader = ((struct symtab_item*)func->value)->decl;
			struct astree* param = fheader->children[2];
			((struct symtab_item*)tmp->value)->data_type =
				((struct symtab_item*)func->value)->data_type;
			concat = NULL;
			/* Passing over arguments*/
			for(iter = tree->children[1]; iter != NULL; iter = iter->children[0]){
				item = tac_populate(iter->children[1]);

				concat = tac_join(item, tac_join(tac_create(TAC_ARG,
					tree->children[0]->symbol, item->res,
					param->children[2]->symbol),
					concat));
				param = param->children[0];
			}

			return tac_join(concat,
				tac_create(TAC_CALL, tmp, tree->children[0]->symbol, NULL));

		default:
			return NULL;
	}
}

static struct tac* print(struct astree* tree){
	struct tac* item;

	switch(tree->type){
		case AST_PRINT:
			return tac_populate(tree->children[0]);

		case AST_PRINT_LIST:
			item = tac_populate(tree->children[1]);
			return tac_join(tac_populate(tree->children[0]), tac_join(
				item, tac_create(TAC_PRINT, item->res, NULL, NULL)));

		default:
			return NULL;
	}
}

static struct tac* attribution(struct astree* tree){
	struct tac* item;
	struct tac* index;
	struct hm_item* tmp;

	switch(tree->type){
		case AST_VAR_ATTR:
			item = tac_populate(tree->children[1]);
			return tac_join(item, tac_create(TAC_MOVE, tree->children[0]->symbol,
				item->res, NULL));

		case AST_VEC_ATTR:
			index = tac_populate(tree->children[1]);
			item = tac_populate(tree->children[2]);
			return tac_join(item, tac_join(index,
				tac_create(TAC_VECWRITE, tree->children[0]->symbol,
				index->res, item->res)));

		case AST_VEC_SUB:
			tmp = symtab_make_tmp();
			((struct symtab_item*)tmp->value)->data_type =
				resolve_expr_type(tree->children[1]);
			index = tac_populate(tree->children[1]);
			return tac_join(index, tac_create(TAC_VECREAD, tmp,
				tree->children[0]->symbol, index->res));

		default:
			return NULL;
	}
}

struct tac* operators(struct astree* tree){
	struct tac* expr[2];
	struct hm_item* tmp;
	struct hm_item* label[2];

	switch(tree->type){
		/* Arith. Op*/
		case AST_ADD: case AST_SUB:	case AST_MUL: case AST_DIV:


			expr[0] = tac_populate(tree->children[0]);
			expr[1] = tac_populate(tree->children[1]);

			tmp = symtab_make_tmp();
			((struct symtab_item*)tmp->value)->data_type = resolve_expr_type(tree);

			return tac_join(expr[0], tac_join(expr[1],
				tac_create(ast_to_tac[tree->type], tmp,
				expr[0] == NULL? tree->children[0]->symbol: expr[0]->res,
				expr[1] == NULL? tree->children[1]->symbol: expr[1]->res)));


		/* Logic Op*/
		case AST_AND: case AST_OR:
		/* Logic Op*/
		case AST_LT: case AST_GT: case AST_LE: case AST_GE:
		case AST_EQ: case AST_NE:

			expr[0] = tac_populate(tree->children[0]);
			expr[1] = tac_populate(tree->children[1]);

			tmp = symtab_make_tmp();
			((struct symtab_item*)tmp->value)->data_type = TP_BOOLEAN;

			return tac_join(expr[0], tac_join(expr[1],
				tac_create(ast_to_tac[tree->type], tmp,
				expr[0] == NULL? tree->children[0]->symbol: expr[0]->res,
				expr[1] == NULL? tree->children[1]->symbol: expr[1]->res)));

		/* Unary Op*/
		case AST_NOT:
			expr[0] = tac_populate(tree->children[0]);

			tmp = symtab_make_tmp();
			((struct symtab_item*)tmp->value)->data_type = TP_BOOLEAN;

			return tac_join(expr[0],
				tac_create(ast_to_tac[tree->type], tmp,
				expr[0] == NULL? tree->children[0]->symbol: expr[0]->res,
				NULL));

		default:
			return NULL;
	}
}

struct tac* init_var(struct astree* tree){

	switch(tree->type){
		case AST_VAR:;
			struct tac* var = tac_create(TAC_VAR, tree->children[0]->symbol,
				NULL, NULL);
			struct tac* move = tac_create(TAC_VARINIT, tree->children[0]->symbol,
				tree->children[2]->symbol, NULL);
			return tac_join(var, move);

		case AST_VEC:;
			struct astree* iter;
			struct tac* concat = NULL;
			struct hm_item* vec_id = tree->children[0]->symbol;
			struct hm_item* vec_len = tree->children[2]->symbol;
			int index = atoi(vec_len->key)-1;
			/* Passing over arguments*/
			for(iter = tree->children[3]; iter != NULL; iter = iter->children[0]){
				struct hm_item* vec_index = symtab_make_const(index--);
				struct tac* vec_init = tac_create(TAC_VECINIT, vec_id, vec_index,
					iter->children[1]->symbol);
				concat = tac_join(vec_init, concat);
			}
			return tac_join(tac_create(TAC_VEC, vec_id, vec_len, NULL), concat);

		default:
			return NULL;
	}

}

struct tac* tac_populate(struct astree* tree){
	if(tree == NULL)
		return NULL;

	switch (tree->type){
		case AST_SYM:
			return symbol(tree);

		case AST_CMD_LIST: case AST_DECL_LIST:
			return tac_join(tac_populate(tree->children[0]),
			 	tac_populate(tree->children[1]));

		case AST_VAR: case AST_VEC: case AST_VEC_INIT:
			return init_var(tree);

		case AST_FUNC: case AST_RETURN: case AST_CALL: case AST_ARGS:
			return function(tree);

		case AST_PRINT: case AST_PRINT_LIST:
			return print(tree);

		case AST_WHEN: case AST_WHEN_ELSE: case AST_WHILE: case AST_FOR:
			return control(tree);

		case AST_VAR_ATTR: case AST_VEC_ATTR: case AST_VEC_SUB:
			return attribution(tree);

		case AST_ADD: 	case AST_SUB:	case AST_MUL: 	case AST_DIV:
		case AST_LT: 	case AST_GT: 	case AST_LE: 	case AST_GE:
		case AST_AND: 	case AST_OR:	case AST_NOT:
		case AST_EQ: 	case AST_NE:
			return operators(tree);

		case AST_READ:
			return tac_create(TAC_READ, tree->children[0]->symbol, NULL, NULL);

		default:
			return tac_populate(tree->children[0]);
	}
}
