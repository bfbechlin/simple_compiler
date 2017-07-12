#include <stdio.h>
#include <stdlib.h>

#include "tac.h"
#include "assembly.h"
#include "symbol_table.h"
#include "hashmap.h"
#include "astree.h"

static void program_prologue(FILE *stream) {
	fprintf(stream, "\t.data\n");
	fprintf(stream, ".preal:\n"
					"\t.string\t\"%%lf\"\n");
	fprintf(stream, ".pint:\n"
					"\t.string\t\"%%i\"\n");
	fprintf(stream, ".pchar:\n"
					"\t.string\t\"%%c\"\n");
}

static void asm_print_var(FILE *stream, struct hm_item* var,
	struct hm_item* init, int vec_list){
		char zero[3] = "00";
		struct hm_item dummy;
		dummy.key = zero;
		if(init == NULL)
			init = &dummy;

		int data_type = ((struct symtab_item*)var->value)->data_type;
		if(vec_list == 0)
			fprintf(stream, "\t.globl\t_%s\n"
							"_%s:\n", var->key, var->key);

		switch (data_type) {
			case TP_DOUBLE:
			case TP_FLOAT:
				//fprintf(stream, "\t.double\t%lf\n", atof(init->key));
				fprintf(stream, "\t.long\t%i\n", atoi(init->key));
				break;
			case TP_LONG:
			case TP_SHORT:
				fprintf(stream, "\t.long\t%i\n", atoi(init->key));
				break;
			case TP_BYTE:
				//fprintf(stream, "\t.byte\t%i\n", *(init->key + 1));
				fprintf(stream, "\t.long\t%i\n", *(init->key + 1));
				break;
			case TP_BOOLEAN:
				fprintf(stream, "\t.long\t%i\n", atoi(init->key));
				break;
		}
}

static void variables(FILE *stream, struct tac *inst) {
	struct hm_item* info;
	struct tac *next;
	int i, next_type, vec_lenght, first;

	switch (inst->type){
	case TAC_VAR:
		next = inst->next;
		next_type = next == NULL ? TAC_VAR : next->type;
		if(next_type == TAC_VARINIT)
			asm_print_var(stream, inst->res, next->op1, 0);
		else
			asm_print_var(stream, inst->res, NULL, 0);
		break;
	case TAC_VEC:
		info = inst->op1;
		vec_lenght = atoi(info->key);
		first = 0;
		next = inst->next == NULL ? inst : inst->next;
		for(int i = 0; i < vec_lenght; i++){
			next_type = next == NULL ? TAC_VEC : next->type;
			if((next_type == TAC_VECINIT) && (next->res == inst->res))
				asm_print_var(stream, inst->res, next->op2, first);
			else
				asm_print_var(stream, inst->res, NULL, first);

			next = next->next == NULL ? next : next->next;
			first = 1;
		}

	default:
		break;
	}
}

static void declarations(FILE *stream) {
	struct hm_item *item;

	char *key;
	struct symtab_item *info;

	for (item = symtab_first(); item != NULL; item = symtab_next()) {
		key = item->key;
		info = (struct symtab_item *)item->value;

		switch (info->code) {
		case SYMBOL_LIT_STRING:
			fprintf(stream, "\t.globl\t.string_%d\n"
						".string_%d:\n"
			    		"\t.string\t%s\n",
						info->unique_id, info->unique_id, key);
			break;

		case SYMBOL_LIT_INT:
			fprintf(stream, "\t.globl\t_%s\n"
							"_%s:\n"
							"\t.long\t%d\n",
							key, key, atoi(key));
			break;

		case SYMBOL_LIT_REAL:
			fprintf(stream, "\t.globl\t_%s\n"
							"_%s:\n"
							//"\t.double\t%f\n"
							"\t.long\t%i\n"
							,key, key,
							//atof(key)
							atoi(key)
							);
			break;

		case SYMBOL_LIT_CHAR:
			/* key + 1 skips the single quote character */
			fprintf(stream, "\t.globl\t_%s\n"
							"_%s:\n"
							//"\t.byte\t%d\n",
							"\t.long\t%d\n",
							key, key, *(key + 1));
			break;

		case SYMBOL_TEMPORARY:
			asm_print_var(stream, item, NULL, 0);
			break;
		case SYMBOL_IDENTIFIER:
		case SYMBOL_LABEL:
		default:
			break;
		}
	}
}

static void print(FILE *stream, struct tac *inst) {
	struct hm_item *arg = inst->res;
	char *key = arg->key;
	struct symtab_item *info = arg->value;

	switch (info->data_type) {
		case TP_DOUBLE:
		case TP_FLOAT:
			fprintf(stream, "\tmovsd\t_%s(%%rip), %%xmm0\n"
				"\tmovl\t$.preal, %%edi\n"
				"\tmovl\t$1, %%eax\n"
				"\tcall printf\n",
				key);
			break;
		case TP_LONG:
		case TP_SHORT:
			fprintf(stream, "\tmovl\t_%s(%%rip), %%esi\n"
				"\tmovl\t$.pint, %%edi\n"
				"\tmovl\t$0, %%eax\n"
				"\tcall printf\n",
					key);
			break;
		case TP_BYTE:
			fprintf(stream, "\tmovl\t_%s(%%rip), %%esi\n"
				"\tmovl\t$.pchar, %%edi\n"
				"\tmovl\t$0, %%eax\n"
				"\tcall printf\n",
				key);
			break;
		case TP_BOOLEAN:
			fprintf(stream, "\tmovl\t_%s(%%rip), %%esi\n"
				"\tmovl\t$.pint, %%edi\n"
				"\tmovl\t$0, %%eax\n"
				"\tcall printf\n",
				key);
			break;
		case TP_STRING:
			fprintf(stream, "\tmovl\t$.string_%d, %%edi\n"
				"\tmovl\t$0, %%eax\n"
				"\tcall printf\n",
				info->unique_id);
		break;
	}
}

static void functions(FILE *stream, struct tac *inst){
	if(inst->type == TAC_BEGINFUN){
		struct hm_item *item = inst->res;
		struct symtab_item* fheader = item->value;
		struct hm_item* param;

		fprintf(stream, "\t.data\n");
		for(param = next_param(fheader->decl, INIT_PARAM);
			param != NULL; param = next_param(NULL, NEXT_PARAM)){
			asm_print_var(stream, param, NULL, 0);

		}

		fprintf(stream, "\t.text\n"
            "\t.globl\t%s\n"
            "%s:\n"
			"\tpushq\t%%rbp\n"
			"\tmovq\t%%rsp, %%rbp\n"
			"\tsubq\t$16, %%rsp\n", item->key, item->key);
	}
	else if(inst->type == TAC_ENDFUN){
		fprintf(stream, "\tleave\n"
		    "\tret\n");
	}
}

static void arith_op(FILE *stream, struct tac* inst){
	struct symtab_item* info1 =  inst->op1->value;
	struct symtab_item* info2 =  inst->op2->value;
	//if((info1->data_type == TP_LONG) && (info2->data_type == TP_LONG)){
		fprintf(stream, "\tmovl\t_%s(%%rip), %%edx\n"
			"\tmovl\t_%s(%%rip), %%eax\n", inst->op1->key, inst->op2->key);
		switch (inst->type){
		case TAC_ADD:
			fprintf(stream, "\tmovl\t_%s(%%rip), %%edx\n"
			"\tmovl\t_%s(%%rip), %%eax\n", inst->op2->key, inst->op1->key);
			fprintf(stream, "\taddl\t%%edx, %%eax\n");
			break;
		case TAC_SUB:
			fprintf(stream, "\tmovl\t_%s(%%rip), %%edx\n"
			"\tmovl\t_%s(%%rip), %%eax\n", inst->op2->key, inst->op1->key);
			fprintf(stream, "\tsubl\t%%edx, %%eax\n");
			break;
		case TAC_MUL:
			fprintf(stream, "\tmovl\t_%s(%%rip), %%edx\n"
			"\tmovl\t_%s(%%rip), %%eax\n", inst->op1->key, inst->op2->key);
			fprintf(stream, "\timull\t%%edx, %%eax\n");
			break;
		case TAC_DIV:
			fprintf(stream, "\tmovl\t_%s(%%rip), %%eax\n"
			"\tmovl\t_%s(%%rip), %%ecx\n", inst->op1->key, inst->op2->key);
			fprintf(stream, "\tcltd\n"
						"\tidiv\t%%ecx\n");
			break;
		}
		fprintf(stream, "\tmovl\t%%eax, _%s(%%rip)\n", inst->res->key);
	//}
}

static void arith_boolean_op(FILE *stream, struct tac* inst){
	struct symtab_item* info1 =  inst->op1->value;
	struct symtab_item* info2 =  inst->op2->value;
	//if((info1->data_type == TP_LONG) && (info2->data_type == TP_LONG)){
		fprintf(stream, "\tmovl\t_%s(%%rip), %%edx\n"
			"\tmovl\t_%s(%%rip), %%eax\n"
			"\tcmpl\t%%eax, %%edx\n", inst->op1->key, inst->op2->key);
		switch (inst->type) {
		case TAC_LT:
			fprintf(stream, "\tsetb\t%%al\n");
			break;
		case TAC_GT:
			fprintf(stream, "\tseta\t%%al\n");
			break;
		case TAC_LE:
			fprintf(stream, "\tsetbe\t%%al\n");
			break;
		case TAC_GE:
			fprintf(stream, "\tsetae\t%%al\n");
			break;
		case TAC_EQ:
			fprintf(stream, "\tsete\t%%al\n");
			break;
		case TAC_NE:
			fprintf(stream, "\tsetne\t%%al\n");
			break;
		}
		fprintf(stream, "\tmovzbl\t%%al, %%eax\n"
			"\tmovl\t%%eax, _%s(%%rip)\n", inst->res->key);
	//}
}

static void logic_boolean_op(FILE *stream, struct tac* inst){
	struct symtab_item* info1 =  inst->op1->value;
	struct symtab_item* info2 =  inst->op2->value;

	//if((info1->data_type == TP_BOOLEAN) && (info2->data_type == TP_BOOLEAN)){
		fprintf(stream, "\tmovl\t_%s(%%rip), %%edx\n"
			"\tmovl\t_%s(%%rip), %%eax\n"
			"\tcmpl\t%%eax, %%edx\n", inst->op1->key, inst->op2->key);
		switch (inst->type) {
		case TAC_AND:
			fprintf(stream, "\tandl\t%%edx, %%eax\n");
			break;
		case TAC_OR:
			fprintf(stream, "\torl\t%%edx, %%eax\n");
			break;
		}
		fprintf(stream, "\tmovl\t%%eax, _%s(%%rip)\n", inst->res->key);
	//}
}

static void unary_logic_boolean_op(FILE *stream, struct tac* inst){
	struct symtab_item* info1 =  inst->op1->value;
	if(info1->data_type == TP_BOOLEAN){
		fprintf(stream, "\tmovl\t_%s(%%rip), %%eax\n"
			"\ttestl\t%%eax, %%eax\n", inst->op1->key);
		switch (inst->type) {
		case TAC_NOT:
			fprintf(stream, "\tsete\t%%al\n");
			break;
		}
		fprintf(stream, "\tmovzbl\t%%al, %%eax\n"
			"\tmovl\t%%eax, _%s(%%rip)\n", inst->res->key);
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
		fprintf(stream, "\tmovl\t_%s(%%rip), %%eax\n"
						"\tmovl\t%%eax, _%s(%%rip)\n"
						,inst->op1->key, inst->res->key);
		break;
	case TAC_LABEL:
		fprintf(stream, "%s:\n", ((struct hm_item*)inst->res)->key);
		break;

	case TAC_BEGINFUN:
	case TAC_ENDFUN:
		functions(stream, inst);
		break;

	case TAC_IFZ:
		fprintf(stream, "\tcmpl\t$0, _%s(%%rip)\n"
	 					"\tje\t%s\n"
						, inst->op1->key, inst->res->key);
		break;
	case TAC_JUMP:
		fprintf(stream, "\tjmp\t%s\n", inst->res->key);
		break;
	case TAC_CALL:
		fprintf(stream, "\tcall\t%s\n"
						"\tmovl\t%%eax, _%s(%%rip)\n"
						,inst->op1->key, inst->res->key);
		break;
	case TAC_ARG:
		fprintf(stream, "\tmovl\t_%s(%%rip), %%eax\n"
						"\tmovl\t%%eax, _%s(%%rip)\n"
						,inst->op1->key, inst->op2->key);
		break;
	case TAC_RET:
		fprintf(stream, "\tmovl\t_%s(%%rip), %%eax\n", inst->res->key);
		break;
	case TAC_PRINT:
		print(stream, inst);
		break;
	case TAC_READ:
		fprintf(stream, "\tmovl\t$_%s, %%esi\n"
						"\tmovl\t$.pint, %%edi\n"
						"\tmovl\t$0, %%eax\n"
						"\tcall\t__isoc99_scanf\n"
						,inst->res->key);
		break;
	case TAC_VECREAD:
		fprintf(stream, "\tmovl\t_%s(%%rip), %%eax\n"
						"\tmovl\t_%s(,%%rax,4), %%eax\n"
						"\tmovl\t%%eax, _%s(%%rip)\n"
						,inst->op2->key, inst->op1->key, inst->res->key);

		break;
	case TAC_VECWRITE:
		fprintf(stream, "\tmovl\t_%s(%%rip), %%eax\n"
						"\tmovl\t_%s(%%rip), %%edx\n"
						"\tmovl\t%%edx, _%s(,%%rax,4)\n"

						,inst->op1->key, inst->op2->key, inst->res->key);
		break;

	case TAC_ADD:
	case TAC_SUB:
	case TAC_MUL:
	case TAC_DIV:
		arith_op(stream, inst);
		break;
	case TAC_INC:
		fprintf(stream, "\tmovl\t_%s(%%rip), %%eax\n"
						"\taddl\t$1, %%eax\n"
						"\tmovl\t%%eax, _%s(%%rip)\n"
						, inst->res->key, inst->res->key);
		break;

	case TAC_NOT:
		unary_logic_boolean_op(stream, inst);
		break;
	case TAC_LT:
	case TAC_GT:
	case TAC_LE:
	case TAC_GE:
	case TAC_EQ:
	case TAC_NE:
		arith_boolean_op(stream, inst);
		break;
	case TAC_AND:
	case TAC_OR:
		logic_boolean_op(stream, inst);
		break;

	case TAC_VAR:
	case TAC_VEC:
		variables(stream, inst);
		break;
	case TAC_VECINIT:
	case TAC_VARINIT:
		break;

	default:
		fprintf(stderr, "ERROR: tac type unknown: %d\n", inst->type);
		break;
	}
}


static void program_epilogue(FILE* stream) {

}

void fprint_assembly(FILE *stream, struct tac *list) {
	struct tac *inst;

	program_prologue(stream);
	declarations(stream);

	for (inst = list; inst != NULL; inst = inst->next) {
		instruction(stream, inst);
	}

	program_epilogue(stream);
}
