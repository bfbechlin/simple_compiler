#include <stdio.h>
#include <stdlib.h>

#include "tac.h"
#include "assembly.h"
#include "symbol_table.h"
#include "hashmap.h"

static void program_prologue(FILE *stream) {
	fprintf(stream, "\t.data\n");
	fprintf(stream, ".pfloat:\n"
					"\t.string\t\"%%f\"\n");
	fprintf(stream, ".pint:\n"
					"\t.string\t\"%%i\"\n");
	fprintf(stream, ".pchar:\n"
					"\t.string\t\"%%c\"\n");
}

static void asm_var(FILE *stream, struct tac *list) {
	struct tac *inst;
	for (inst = list; inst != NULL; inst = inst->next) {
		struct hm_item *res = inst->res;
		struct hm_item *op1 = inst->op1;
		struct hm_item *op2 = inst->op2;

		char *key = res? res->key : NULL;
		struct symtab_item *info = res? res->value : NULL;

		switch (inst->type) {
		case TAC_VAR:
			fprintf(stream, "%s:\n"
							"\t.long\t0\n",
							key);
			break;
		default:
			break;
		}
	}
}

static void declarations(FILE *stream, struct tac *list) {
	struct hm_item *item;

	char *key;
	struct symtab_item *info;

	for (item = symtab_first(); item != NULL; item = symtab_next()) {
		key = item->key;
		info = (struct symtab_item *)item->value;

		switch (info->code) {
		case SYMBOL_LIT_STRING:
			fprintf(stream, ".string_%d:\n"
			                "\t.string\t%s\n",
							info->unique_id, key);
			break;

		case SYMBOL_LIT_INT:
			fprintf(stream, ".int_%s:\n"
							"\t.long\t%d\n",
							key, atoi(key));
			break;

		case SYMBOL_LIT_REAL:
			fprintf(stream, ".real_%s:\n"
							"\t.float\t%f\n",
							key, atof(key));
			break;

		case SYMBOL_LIT_CHAR:
			/* key + 1 skips the single quote character */
			fprintf(stream, ".char_%c:\n"
							"\t.long\t%d\n",
							*(key + 1), *(key + 1));
			break;

		case SYMBOL_TEMPORARY:
			fprintf(stream, "%s:\n"
							"\t.long\t%d\n",
							key, 0);
			break;

		case SYMBOL_IDENTIFIER:
		case SYMBOL_LABEL:
		default:
			break;
		}
	}

	asm_var(stream, list);
}

static void main_prologue(FILE *stream) {
	fprintf(stream, "\t.text\n"
	                "\t.globl\tmain\n"
	                "main:\n"
					"\tpushq\t%%rbp\n"
					"\tmovq\t%%rsp, %%rbp\n"
					"\tsubq\t$16, %%rsp\n");
}

static void asm_print(FILE *stream, struct tac *inst) {
	struct hm_item *arg = inst->res;
	char *key = arg->key;
	struct symtab_item *info = arg->value;

	printf("%d\n", info->code);
	switch (info->code) {
	case SYMBOL_LIT_STRING:
		fprintf(stream, "\tmovl\t$.string_%d, %%edi\n"
		                "\tcall puts\n",
						info->unique_id);
		break;

	case SYMBOL_LIT_CHAR:
		fprintf(stream, "\tmovl\t$.char_%s(%%rip), %%esi\n"
		                "\tmovl\t$.pchar, %%edi\n"
						"\tmovl\t$0, %%eax\n"
		                "\tcall printf\n",
						key);
		break;

	case SYMBOL_LIT_INT:
		fprintf(stream, "\tmovl\t$.int_%s(%%rip), %%esi\n"
		                "\tmovl\t$.pint, %%edi\n"
						"\tmovl\t$0, %%eax\n"
		                "\tcall printf\n",
						key);
		break;

	case SYMBOL_LIT_REAL:
		fprintf(stream, "\tmovsd\t$.real_%s(%%rip), %%xmm0\n"
		                "\tmovl\t$.preal, %%edi\n"
						"\tmovl\t$1, %%eax\n"
		                "\tcall printf\n",
						key);
		break;

	case SYMBOL_IDENTIFIER:
	case SYMBOL_TEMPORARY:
		fprintf(stream, "\tmovl\t%s(%%rip), %%esi\n"
		                "\tmovl\t$.pint, %%edi\n"
						"\tmovl\t$0, %%eax\n"
		                "\tcall printf\n",
						key);
		break;

	}
}

static void instruction(FILE *stream, struct tac *inst) {
	struct hm_item *res = inst->res;
	struct hm_item *op1 = inst->op1;
	struct hm_item *op2 = inst->op2;

	char *key = res? res->key : NULL;
	struct symtab_item *info = res? res->value : NULL;

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
		asm_print(stream, inst);
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
		break;
	}
}

static void main_epilogue(stream) {
	fprintf(stream, "\tleave\n"
	                "\tret\n");
}

static void program_epilogue(stream) {

}

void fprint_assembly(FILE *stream, struct tac *list) {
	struct tac *inst;

	program_prologue(stream);
	declarations(stream, list);

	main_prologue(stream);

	for (inst = list; inst != NULL; inst = inst->next) {
		instruction(stream, inst);
	}

	main_epilogue(stream);
	program_epilogue(stream);
}
