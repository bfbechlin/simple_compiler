#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void initSymbolsTable(void);
extern void setInputFile(char* file_name);
extern int yyparse(void);

#define FILE_OPT_STR 	"-file="
#define FILE_OPT_NUMBER	6

void main(int argc, char* argv[]){
	int i;
	initSymbolsTable();
	if (argc > 1) {
		for(i = 1; i < argc; i++){
			// FILE NAME OPTION
			if(strncmp(argv[i], FILE_OPT_STR, FILE_OPT_NUMBER) == 0){
				setInputFile(&argv[i][FILE_OPT_NUMBER]);
				//printf("FILE_NAME : %s\n", &argv[i][FILE_OPT_NUMBER]);
			}
		}
	}
	printf("%d\n", yyparse());
}
