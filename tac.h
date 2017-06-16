#include "astree.h"

#define TAC_SYMBOL 		0
#define TAC_MOVE 		1
#define TAC_LABEL 		2
#define TAC_BEGINFUN 	3
#define TAC_ENDFUN 		4
#define TAC_IFZ 		5
#define TAC_JUMP 		6
#define TAC_CALL 		7
#define TAC_ARG 		8
#define TAC_RET 		9
#define TAC_PRINT 		10
#define TAC_READ 		11

/* Expression operators*/
#define TAC_ADD 		15
#define TAC_SUB			16
#define TAC_MUL 		17
#define TAC_DIV 		18

struct tac{
	int type;

	struct hm_item* res;
	struct hm_item* op1;
	struct hm_item* op2;

//	struct tac* prev;
	struct tac* next;
};

/* Join two lists, l1 and l2, of tacs and return the first item poiter the joined list*/
struct tac* tac_join(struct tac* l1, struct tac* l2);

struct tac* tac_create(int type, struct hm_item* res, struct hm_item* op1,
	struct hm_item* op2, struct tac* next);

void tac_terminate(struct tac* list);

void tac_fprint(FILE *stream, struct tac* list);

struct tac* tac_reorder(struct tac* list);

struct tac* tac_populate(struct astree* tree);
