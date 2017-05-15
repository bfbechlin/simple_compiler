#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "astree.h"

extern int set_input_file(char* file_name);
extern int yyparse(void);
extern struct astree *program;

void main(int argc, char* argv[]){
	int i;
	symtab_init();
	if(argc > 2){
		fprintf(stderr,
			"ERRO:\n\t Arguments not suported.\nUSAGE:\
			\n\t [input file name]: name of file to be analysed.\n");
		exit(1);
	}
	if(argc > 1) {
		if(!set_input_file(argv[1])){
			fprintf(stderr, "ERRO:\n\t File [%s] not found.\n", argv[1]);
			exit(2);
		}
	}
	yyparse();
	printf("SUCESS:\n\t Program was accepted.\n");
	symtab_print();
	ast_fprint(stdout, 0, program);
	printf("SOURCE:\n");
	ast_make_source(stdout, program, 0);
	symtab_destroy();
	ast_terminate(program);
	exit(0);
}
