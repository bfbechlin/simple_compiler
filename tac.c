#include <stdlib.h>
#include <stdio.h>
#include "symbol_table.h"
#include "tac.h"
#include "astree.h"

static int ast_to_tac[] = {
	TAC_SYMBOL,
	TAC_ADD,
	TAC_SUB,
	TAC_MUL,
	TAC_DIV,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, TAC_EQ/* <--TEST*/
	/* and more...*/
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
	"",
	"",
	"",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"INC",
	"EQ" /* <--TEST*/
	/* and more...*/
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

static struct tac* control(struct astree* tree){
	struct tac* expr;
	struct tac* cmd[2];
	struct hm_item* label[2];
	struct hm_item* ident;

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
			label[0] = symtab_make_label();
			label[1] = symtab_make_label();
			ident = tree->children[0]->symbol;
			expr = tac_populate(tree->children[1]);
			cmd[0] = tac_populate(tree->children[3]);
			cmd[1] = tac_join(expr, tac_join(
				tac_create(TAC_MOVE, ident, expr->res, NULL),
				tac_create(TAC_LABEL, label[0], NULL, NULL)));
			expr = tac_populate(tree->children[2]);
			cmd[1] = tac_join(cmd[1], tac_join(expr, tac_join(
				tac_create(TAC_SUB, expr->res, ident, expr->res),
				tac_create(TAC_IFZ, label[1], expr->res, NULL))));
			return tac_join(cmd[1], tac_join(cmd[0], tac_join(
				tac_create(TAC_INC, ident, NULL, NULL), tac_join(
				tac_create(TAC_JUMP, label[0], NULL, NULL),
				tac_create(TAC_LABEL, label[1], NULL, NULL)))));

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
			tmp = symtab_make_tmp();
			return tac_create(TAC_SYMBOL, tmp, tree->symbol, NULL);
		default:
			return tac_create(TAC_SYMBOL, tree->symbol, NULL, NULL);
	}
}

struct tac* function(struct astree* tree){
	struct tac* item;
	struct hm_item* tmp;

	switch(tree->type){
		case AST_FUNC:
			return tac_join(tac_create(TAC_BEGINFUN,
				tree->children[0]->children[1]->symbol, NULL, NULL),
				tac_join(tac_populate(tree->children[1]),
					tac_create(TAC_ENDFUN, NULL, NULL, NULL)));

		case AST_RETURN:
			item = tac_populate(tree->children[0]);
			return tac_join(item, tac_create(TAC_RET, item->res, NULL, NULL));

		case AST_CALL:
			tmp = symtab_make_tmp();
			return tac_join(tac_populate(tree->children[1]),
				tac_create(TAC_CALL, tmp, tree->children[0]->symbol, NULL));

		case AST_ARGS:
			item = tac_populate(tree->children[1]);
			return tac_join(tac_populate(tree->children[0]), tac_join(
				item, tac_create(TAC_ARG, item->res, NULL, NULL)));

		default:
			return NULL;
	}
}

static struct tac* print(struct astree* tree){
	struct tac* item;

	switch(tree->type){
		case AST_PRINT:
			return tac_join(tac_populate(tree->children[0]),
				tac_create(TAC_PRINT, NULL, NULL, NULL));

		case AST_PRINT_LIST:
			item = tac_populate(tree->children[1]);
			return tac_join(tac_populate(tree->children[0]), tac_join(
				item, tac_create(TAC_ARG, item->res, NULL, NULL)));

		default:
			return NULL;
	}
}

struct tac* tac_populate(struct astree* tree){
	if(tree == NULL)
		return NULL;

	struct tac* item[AST_MAXCHILDREN];

	switch (tree->type){
		case AST_SYM:
			return symbol(tree);

		case AST_CMD_LIST: case AST_DECL_LIST:
			return tac_join(tac_populate(tree->children[0]),
			 	tac_populate(tree->children[1]));

		case AST_FUNC: case AST_RETURN: case AST_CALL: case AST_ARGS:
			return function(tree);

		case AST_PRINT: case AST_PRINT_LIST:
			return print(tree);

		case AST_WHEN: case AST_WHEN_ELSE: case AST_WHILE: case AST_FOR:
			return control(tree);

		case AST_VAR_ATTR:
			item[0] = tac_populate(tree->children[1]);
			return tac_join(item[0], tac_create(TAC_MOVE, tree->children[0]->symbol,
				item[0]->res, NULL));

		case AST_ADD: case AST_SUB:	case AST_MUL: case AST_DIV:
		/*To test control_block -->*/ case AST_EQ:
			item[0] = tac_populate(tree->children[0]);
			item[1] = tac_populate(tree->children[1]);

			return tac_join(item[0], tac_join(item[1],
					tac_create(ast_to_tac[tree->type], symtab_make_tmp(),
					item[0] == NULL? tree->children[0]->symbol: item[0]->res,
					item[1] == NULL? tree->children[1]->symbol: item[1]->res)));

		case AST_READ:
			return tac_create(TAC_READ, tree->children[0]->symbol, NULL, NULL);

		default:
			return NULL;
	}
	/*TODO:
		* Boolean Expressions
		* Vector Attribution
		* Init Variables ???
	*/
}
