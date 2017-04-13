#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* IMPORTANT: compile this before the lex file, so link all*/

#include "test_tokens.c"

#define FILE_OPT_STR 	"-file="
#define FILE_OPT_NUMBER	6

void main(int argc, char* argv[]){
	int i;
	initMe();
	/* keep previous functionality */
	if (argc > 1) {
		for(i = 1; i < argc; i++){
			// FILE NAME OPTION
			if(strncmp(argv[i], FILE_OPT_STR, FILE_OPT_NUMBER) == 0){
				yyin = fopen(&argv[i][FILE_OPT_NUMBER], "r");
				printf("FILE_NAME : %s\n", &argv[i][FILE_OPT_NUMBER]);
			}
		}
		i = yylex();
		printf("%c %i\n", i, i);
	/* add test if nothing else is supplied */
	} else {
		int *exp, given, line; /* expected and given token */
		yyin = fopen("test_tokens.txt", "r");

		exp = expected_codes;
		while (*exp != -1) {
			given = yylex();
			line = getLineNumber();
			if (given != *exp) {
				printf("WRONG: in text `%s` at line %i expected %d, got %d\n", yytext, line, *exp, given);
			} else {
				printf("RIGHT: in text `%s` at line %i expected %d, got %d\n", yytext, line, *exp, given);
			}
			exp++;
		}
	}
}
