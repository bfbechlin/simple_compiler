#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.c"
#include "scanner.c"

#define FILE_OPT_STR 	"-file="
#define FILE_OPT_NUMBER	6

void main(int argc, char* argv[]){
	int i;
	if (argc > 1) {
		for(i = 1; i < argc; i++){
			// FILE NAME OPTION
			if(strncmp(argv[i], FILE_OPT_STR, FILE_OPT_NUMBER) == 0){
				yyin = fopen(&argv[i][FILE_OPT_NUMBER], "r");
				printf("FILE_NAME : %s\n", &argv[i][FILE_OPT_NUMBER]);
			}
		}
	}
	yyparse();
	fprintf(stderr, "SUCESSFUL: program accepted.\n");
}
