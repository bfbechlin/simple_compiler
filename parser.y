 /*-----DEFINITIONS-----*/
%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	extern int getLineNumber(void);

	int yylex(void);
	void yyerror(char *);
%}

 /* Tokens */
 %token KW_SHORT
 %token KW_BYTE
 %token KW_LONG
 %token KW_FLOAT
 %token KW_DOUBLE
 %token KW_WHEN
 %token KW_THEN
 %token KW_ELSE
 %token KW_WHILE
 %token KW_FOR
 %token KW_READ
 %token KW_RETURN
 %token KW_PRINT
 %token OPERATOR_LE
 %token OPERATOR_GE
 %token OPERATOR_EQ
 %token OPERATOR_NE
 %token OPERATOR_AND
 %token OPERATOR_OR
 %token TK_IDENTIFIER
 %token LIT_INTEGER
 %token LIT_REAL
 %token LIT_CHAR
 %token LIT_STRING
 %token TOKEN_ERROR

%%
 /*-----RULES------*/

program:
	LIT_INTEGER				{}
	;

%%
 /*-----SUBROUTINES-----*/

void yyerror(char *s){
	int line;
	line = getLineNumber();
	fprintf(stderr, "ERROR:\n\t Program was rejected at line %d.\n", line);
	exit(3);
}
