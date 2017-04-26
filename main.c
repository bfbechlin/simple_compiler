#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbols_table.h"

extern int set_input_file(char* file_name);
extern int yyparse(void);

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
	symtab_destroy();
	exit(0);
}
