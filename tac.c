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
	"DIV"
	/* and more...*/
};

struct tac* tac_join(struct tac* l1, struct tac* l2){
	struct tac* item;
	if(l1 == NULL)
		return l2;
	/* Find last item*/
	for(item = l1; item->next != NULL; item = item->next){}
	item->next = l2;
	return l1;
}

struct tac* tac_create(int type, struct hm_item* res, struct hm_item* op1,
	struct hm_item* op2, struct tac* next){

	struct tac *node = calloc(1, sizeof(struct tac));

	if (!node) {
		perror("tac: node alocation");
		return NULL;
	}
	node->type = type;
	node->res = res;
	node->op1 = op1;
	node->op2 = op2;
	node->next = next;

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
		switch (item->type) {
			case TAC_SYMBOL:
				break;
			default:
				fprintf(stream, "%s %s %s %s\n",
					tac_to_string[item->type],
					item->res ? item->res->key : "",
					item->op1 ? item->op1->key : "",
					item->op2 ? item->op2->key : "");
		}
	}
}

struct tac* tac_populate(struct astree* tree){
	if(tree == NULL)
		return NULL;

	struct tac* item[AST_MAXCHILDREN];
	switch (tree->type){
		case AST_CMD_LIST: case AST_DECL_LIST:
			return tac_join(tac_populate(tree->children[0]),
			 	tac_populate(tree->children[1]));

		case AST_VAR_ATTR:
			item[0] = tac_populate(tree->children[1]);
			return tac_join(item[0], tac_create(TAC_MOVE, tree->children[0]->symbol,
				item[0]->res, NULL, NULL));

		case AST_ADD: case AST_SUB:	case AST_MUL: case AST_DIV:
			item[0] = tac_populate(tree->children[0]);
			item[1] = tac_populate(tree->children[1]);

			return tac_join(item[0],
				tac_create(ast_to_tac[tree->type], symtab_make_tmp(),
				item[0] == NULL? tree->children[0]->symbol: item[0]->res,
				item[1] == NULL? tree->children[1]->symbol: item[1]->res,
				item[1]));


		case AST_FUNC:
			return tac_join(tac_create(TAC_BEGINFUN, symtab_make_label(),
				NULL, NULL, tac_populate(tree->children[1])),
				tac_create(TAC_ENDFUN, NULL, NULL, NULL, NULL));

		default:
			/* Tests */
			return tac_populate(tree->children[0]);
	}

}
