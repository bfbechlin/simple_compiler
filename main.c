#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "astree.h"
#include "semantic.h"

extern FILE *set_input_file(char* file_name);
extern int yyparse(void);
extern struct astree *program;

void main(int argc, char* argv[]){
	FILE *out, *in;
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
	int status = yyparse();
	fprintf(stderr, "------------- Parsing successful\n");

	ast_semantic_check(program);
	fprintf(stderr, "------------- Semantic check successful\n");

	fprintf(stderr, "------------- Symbol table:\n");
	symtab_print();

	fprintf(stderr, "------------- Abstract syntax tree\n");
	ast_fprint(stdout, 0, program);

	fprintf(stderr, "------------- Source code (check output file, if this is empty):\n");
	ast_make_source(out, program, 0);

	fprintf(stderr, "------------- Exiting...\n");
	symtab_destroy();
	ast_terminate(program);
	exit(0);
}
