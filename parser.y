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
%token KW_TO
%token KW_READ
%token KW_RETURN
%token KW_PRINT
%token OP_LE
%token OP_GE
%token OP_EQ
%token OP_NE
%token OP_AND
%token OP_OR
%token TK_ID
%token LIT_INT
%token LIT_REAL
%token LIT_CHAR
%token LIT_STRING
%token TOKEN_ERROR

%start prog
%left OP_EQ OP_LE OP_GE OP_NE '>' '<'
%left OP_AND OP_OR '!'
%left '+' '-'
%left '*' '/'

%%
 /*-----RULES------*/

prog: prog decl ';' | ;

decl: func | var | vec ;

/* varibales */

var: TK_ID ':' type lit ;

vec: TK_ID ':' type '[' LIT_INT ']' vec_init ;
vec_init: vec_init lit | ;

type: KW_BYTE
	| KW_SHORT
	| KW_LONG
	| KW_FLOAT
	| KW_DOUBLE
	;

lit: LIT_INT
   | LIT_REAL
   | LIT_CHAR
   ;

/* functions */

func: fheader fbody ;

fheader: type TK_ID '(' params_list ')' ;

params_list: params | ;

params: params_rest type TK_ID ;
params_rest: params_rest type TK_ID ',' | ;

fbody: cmd ;

/* command block */

block: '{' '}' | '{' cmd_list '}' ;
cmd_list: cmd_list cmd ';' | ;

/* single commands single */

cmd: attr | ctrl | read | print | return | block | ;

attr: TK_ID '=' expr | TK_ID '#' expr '=' expr ;

read: KW_READ TK_ID ;

print: KW_PRINT print_list ;
print_list: print_list print_arg | print_arg;
print_arg: LIT_STRING | expr ;

return: KW_RETURN expr ;

/* expressions */

expr: TK_ID | TK_ID '[' expr ']' | LIT_INT | LIT_CHAR | LIT_REAL | TK_ID '(' args_list ')'
    | expr '+' expr
	| expr '-' expr
	| expr '*' expr
	| expr '/' expr
	| expr '<' expr
	| expr '>' expr
	| '!' expr
	| expr OP_LE expr
	| expr OP_GE expr
	| expr OP_EQ expr
	| expr OP_NE expr
	| expr OP_AND expr
	| expr OP_OR expr
	;

args_list: args | ;
args: args_rest expr ;
args_rest: args_rest expr ',' | ;

/* control */

ctrl: KW_WHEN '(' expr ')' KW_THEN cmd
    | KW_WHEN '(' expr ')' KW_THEN cmd KW_ELSE cmd
	| KW_WHILE '(' expr ')' cmd
	| KW_FOR '(' TK_ID'=' expr KW_TO expr ')' cmd
	;

%%
 /*-----SUBROUTINES-----*/

void yyerror(char *s){
	int line;
	line = getLineNumber();
	fprintf(stderr, "ERROR:\n\t Program was rejected at line %d.\n", line);
	exit(3);
}
