 /*-----DEFINITIONS-----*/
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
	#include "hashmap.h"
	#include "astree.h"

    extern int getLineNumber(void);

	struct astree *program;
    int yylex(void);
    void yyerror(char *);
%}

%union {
	struct hm_item *symbol;
	struct astree *tree;
}

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
%token <symbol> TK_ID
%token <symbol> LIT_INT
%token <symbol> LIT_REAL
%token <symbol> LIT_CHAR
%token <symbol> LIT_STRING
%token TOKEN_ERROR

%type <tree> prog decl_list decl func type var lit ident

%start prog
%left OP_EQ OP_LE OP_GE OP_NE '>' '<'
%left OP_AND OP_OR '!'
%left '+' '-'
%left '*' '/'

%%
 /*-----RULES------*/

prog: decl_list { program = ast_create(AST_PROG, NULL, $1, NULL, NULL, NULL); } ;

decl_list: decl_list decl ';' { $$ = ast_create(AST_DECL_LIST, NULL, $1, $2, NULL, NULL); }
   		 | { $$ = NULL; } ;

decl: func { }
    | var { $$ = $1; }
    | vec { } ;

/* identifier */
ident: TK_ID {$$ = ast_create(AST_SYM, yyval.symbol, NULL, NULL, NULL, NULL);};

/* varibales */

var: ident ':' type lit { $$ = ast_create(AST_VAR, NULL, $1, $3, $4, NULL); } ;

vec: ident ':' type '[' LIT_INT ']' vec_init { } ;
vec_init: vec_init lit { }
        | ;

type: KW_BYTE { $$ = ast_create(AST_KW_BYTE, NULL, NULL, NULL, NULL, NULL); }
    | KW_SHORT { $$ = ast_create(AST_KW_SHORT, NULL, NULL, NULL, NULL, NULL); }
    | KW_LONG { $$ = ast_create(AST_KW_LONG, NULL, NULL, NULL, NULL, NULL); }
    | KW_FLOAT { $$ = ast_create(AST_KW_FLOAT, NULL, NULL, NULL, NULL, NULL); }
    | KW_DOUBLE { $$ = ast_create(AST_KW_DOUBLE, NULL, NULL, NULL, NULL, NULL); }
    ;

lit: LIT_INT { $$ = ast_create(AST_SYM, yylval.symbol, NULL, NULL, NULL, NULL); }
   | LIT_REAL { $$ = ast_create(AST_SYM, yylval.symbol, NULL, NULL, NULL, NULL); }
   | LIT_CHAR { $$ = ast_create(AST_SYM, yylval.symbol, NULL, NULL, NULL, NULL); }
   ;

/* functions */

func: fheader fbody { } ;

fheader: type ident '(' params_list ')' { } ;

params_list: params { }
           | ;

params: params_rest type ident { } ;
params_rest: params_rest type ident ',' { }
           | ;

fbody: cmd { } ;

/* command block */

block: '{' cmd_list '}' { } ;
cmd_list: cmd_list cmd ';' { }
		| ;

/* single commands single */

cmd: attr { }
   | ctrl { }
   | read { }
   | print { }
   | return { }
   | block { }
   | ;

attr: ident '=' expr { }
    | ident '#' expr '=' expr { } ;

read: KW_READ ident { } ;

print: KW_PRINT print_list { } ;
print_list: print_list print_arg { }
          | print_arg { } ;
print_arg: LIT_STRING { }
         | expr { } ;

return: KW_RETURN expr { } ;

/* expressions */

expr: ident {}
    | ident '[' expr ']' { }
    | LIT_INT { }
    | LIT_CHAR { }
    | LIT_REAL { }
    | ident '(' args_list ')' { }
    | '(' expr ')' { }
    | expr '+' expr { }
    | expr '-' expr { }
    | expr '*' expr { }
    | expr '/' expr { }
    | expr '<' expr { }
    | expr '>' expr { }
    | '!' expr { }
    | expr OP_LE expr { }
    | expr OP_GE expr { }
    | expr OP_EQ expr { }
    | expr OP_NE expr { }
    | expr OP_AND expr { }
    | expr OP_OR expr { }
    ;

args_list: args { }
         | ;
args: args_rest expr { } ;
args_rest: args_rest expr ',' { }
         | ;

/* control */

ctrl: KW_WHEN '(' expr ')' KW_THEN cmd { }
    | KW_WHEN '(' expr ')' KW_THEN cmd KW_ELSE cmd { }
    | KW_WHILE '(' expr ')' cmd { }
    | KW_FOR '(' ident'=' expr KW_TO expr ')' cmd { }
    ;

%%
 /*-----SUBROUTINES-----*/

void yyerror(char *s){
    fprintf(stderr, "ERROR:\n\t Program was rejected at line %d.\n", getLineNumber());
    exit(3);
}
