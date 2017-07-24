#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "astree.h"
#include "semantic.h"
#include "tac.h"
#include "assembly.h"

extern FILE *set_input_file(char* file_name);
extern int yyparse(void);
extern struct astree *program;

void main(int argc, char* argv[]){
	FILE *out, *in;
	struct tac* intermed_code;
	switch (argc) {
		case 1:
			in = stdin;
			out = stdout;
			break;
		case 2:
			in = set_input_file(argv[1]);
			out = stdout;
			break;
		case 3:
			in = set_input_file(argv[1]);
			out = fopen(argv[2], "w");
			break;
		default:
			fprintf(stderr, "Usage: %s <input file> <output file>.\n", argv[0]);
			exit(1);
			break;
	}

	if (!out || !in) {
		fprintf(stderr, "Input or output file not found");
		exit(2);
	}

	symtab_init();
	int err = yyparse();

	if (err) exit(3);
	fprintf(stderr, "------------- Parsing successful\n");

	ast_semantic_check(program);
	fprintf(stderr, "------------- Semantic check successful\n");

	fprintf(stderr, "------------- Symbol table:\n");
	symtab_print();

	fprintf(stderr, "------------- Abstract syntax tree\n");
	ast_fprint(stdout, 0, program);

	fprintf(stderr, "------------- Source code (check output file, if this is empty):\n");
	ast_make_source(out, program, 0);

	intermed_code = tac_populate(program);
	intermed_code = tac_reorder(intermed_code);
	fprintf(stderr, "------------- TAC list:\n");
	tac_fprint(stderr, intermed_code);

	fprintf(stderr, "------------- Assembly:\n");
	FILE *asm_file = fopen("asm.s", "w");
	fprint_assembly(asm_file, intermed_code);
	fclose(asm_file);

	fprintf(stderr, "------------- Exiting...\n");
	symtab_destroy();
	ast_terminate(program);
	exit(0);
}
