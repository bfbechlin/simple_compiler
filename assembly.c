#include <stdio.h>

#include "tac.h"
#include "assembly.h"

void fprint_assembly(FILE *stream, struct tac *list) {
	struct tac *inst;
	for (inst = list; inst != NULL; inst = inst->next) {
		fprint_instruction(stream, inst);
	}
}

/* Print a non-null TAC instruction */
void fprint_instruction(FILE *stream, struct tac *inst) {
	switch (inst->type) {
	case TAC_SYMBOL:
		break;
	case TAC_MOVE:
		break;
	case TAC_LABEL:
		break;
	case TAC_BEGINFUN:
		break;
	case TAC_ENDFUN:
		break;
	case TAC_IFZ:
		break;
	case TAC_JUMP:
		break;
	case TAC_CALL:
		break;
	case TAC_ARG:
		break;
	case TAC_RET:
		break;
	case TAC_PRINT:
		break;
	case TAC_READ:
		break;
	case TAC_VECREAD:
		break;
	case TAC_VECWRITE:
		break;

	case TAC_ADD:
		break;
	case TAC_SUB:
		break;
	case TAC_MUL:
		break;
	case TAC_DIV:
		break;
	case TAC_INC:
		break;
	case TAC_LT:
		break;
	case TAC_GT:
		break;
	case TAC_NOT:
		break;
	case TAC_LE:
		break;
	case TAC_GE:
		break;
	case TAC_EQ:
		break;
	case TAC_NE:
		break;
	case TAC_AND:
		break;
	case TAC_OR:
		break;

	case TAC_VAR:
		break;
	case TAC_VARINIT:
		break;
	case TAC_VEC:
		break;
	case TAC_VECINIT:
		break;
	default:
		fprintf(stderr, "ERROR: tac type unknown: %d\n", inst->type);
	}
}
