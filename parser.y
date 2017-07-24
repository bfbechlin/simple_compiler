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
    int errors_found = 0;
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

%type <tree> prog decl_list decl func type var lit ident vec vec_init fheader params_list params params_rest fbody block cmd_list cmd attr expr read print_arg print_list print return args args_rest args_list ctrl

%start prog
%left OP_AND OP_OR '!'
%left OP_EQ OP_LE OP_GE OP_NE '>' '<'
%left '+' '-'
%left '*' '/'

%%
 /*-----RULES------*/

prog: decl_list { program = ast_create(AST_PROG, NULL, $1, NULL, NULL, NULL); if (errors_found) return 1; } ;

decl_list: decl_list decl ';' { $$ = ast_create(AST_DECL_LIST, NULL, $1, $2, NULL, NULL); }
         | decl_list error ';' { $$ = $1; }
   		 | { $$ = NULL; } ;

decl: func { }
    | var { $$ = $1; }
    | vec { $$ = $1; } ;

/* identifier */
ident: TK_ID {$$ = ast_create(AST_SYM, $1, NULL, NULL, NULL, NULL);};

/* varibales */

var: ident ':' type lit { $$ = ast_create(AST_VAR, NULL, $1, $3, $4, NULL); } ;

vec: ident ':' type '[' LIT_INT ']' vec_init
   {
   struct astree *lit_int = ast_create(AST_SYM, $5, NULL, NULL, NULL, NULL);
   $$ = ast_create(AST_VEC, NULL, $1, $3, lit_int, $7);
   } ;
vec_init: vec_init lit { $$ = ast_create(AST_VEC_INIT, NULL, $1, $2, NULL, NULL); }
        | { $$ = NULL; } ;

type: KW_BYTE { $$ = ast_create(AST_KW_BYTE, NULL, NULL, NULL, NULL, NULL); }
    | KW_SHORT { $$ = ast_create(AST_KW_SHORT, NULL, NULL, NULL, NULL, NULL); }
    | KW_LONG { $$ = ast_create(AST_KW_LONG, NULL, NULL, NULL, NULL, NULL); }
    | KW_FLOAT { $$ = ast_create(AST_KW_FLOAT, NULL, NULL, NULL, NULL, NULL); }
    | KW_DOUBLE { $$ = ast_create(AST_KW_DOUBLE, NULL, NULL, NULL, NULL, NULL); }
    ;

lit: LIT_INT { $$ = ast_create(AST_SYM, $1, NULL, NULL, NULL, NULL); }
   | LIT_REAL { $$ = ast_create(AST_SYM, $1, NULL, NULL, NULL, NULL); }
   | LIT_CHAR { $$ = ast_create(AST_SYM, $1, NULL, NULL, NULL, NULL); }
   ;

/* functions */

func: fheader fbody { $$ = ast_create(AST_FUNC, NULL, $1, $2, NULL, NULL); } ;

fheader: type ident '(' params_list ')' { $$ = ast_create(AST_FHEADER, NULL, $1, $2, $4, NULL); } ;

params_list: params { $$ = $1; }
           | { $$ = NULL; } ;

params: params_rest type ident { $$ = ast_create(AST_PARAMS, NULL, $1, $2, $3, NULL); } ;
params_rest: params_rest type ident ',' { $$ = ast_create(AST_PARAMS, NULL, $1, $2, $3, NULL); }
           | { $$ = NULL; } ;

fbody: cmd { $$ = $1; } ;

/* command block */

block: '{' cmd_list '}' { $$ = ast_create(AST_BLOCK, NULL, $2, NULL, NULL, NULL);}
     | '{' error '}' { $$ = NULL; } ;
cmd_list: cmd_list cmd ';' { $$ = ast_create(AST_CMD_LIST, NULL, $1, $2, NULL, NULL); }
        | cmd_list error ';'
		| { $$ = NULL; } ;

/* single commands single */

cmd: attr { $$ = $1; }
   | ctrl { $$ = $1; }
   | read { $$ = $1; }
   | print { $$ = $1; }
   | return { $$ = $1; }
   | block { $$ = $1; }
   | { $$ = NULL; } ;

attr: ident '=' expr { $$ = ast_create(AST_VAR_ATTR, NULL, $1, $3, NULL, NULL); }
    | ident '#' expr '=' expr { $$ = ast_create(AST_VEC_ATTR, NULL, $1, $3, $5, NULL); } ;

read: KW_READ ident { $$ = ast_create(AST_READ, NULL, $2, NULL, NULL, NULL); } ;

print: KW_PRINT print_list { $$ = ast_create(AST_PRINT, NULL, $2, NULL, NULL, NULL); } ;
print_list: print_list print_arg { $$ = ast_create(AST_PRINT_LIST, NULL, $1, $2, NULL, NULL); }
          | print_arg { $$ = ast_create(AST_PRINT_LIST, NULL, NULL, $1, NULL, NULL); } ;
print_arg: LIT_STRING { $$ = ast_create(AST_SYM, $1, NULL, NULL, NULL, NULL); }
         | expr { $$ = $1; } ;

return: KW_RETURN expr { $$ = ast_create(AST_RETURN, NULL, $2, NULL, NULL, NULL); } ;

/* expressions */

expr: ident { $$ = $1; }
    | ident '[' expr ']' { $$ = ast_create(AST_VEC_SUB, NULL, $1, $3, NULL, NULL); }
    | LIT_INT { $$ = ast_create(AST_SYM, $1, NULL, NULL, NULL, NULL); }
    | LIT_CHAR { $$ = ast_create(AST_SYM, $1, NULL, NULL, NULL, NULL); }
    | LIT_REAL { $$ = ast_create(AST_SYM, $1, NULL, NULL, NULL, NULL); }
    | ident '(' args_list ')' { $$ = ast_create(AST_CALL, NULL, $1, $3, NULL, NULL); }
    | '(' expr ')' { $$ = ast_create(AST_EXP_BLOCK, NULL, $2, NULL, NULL, NULL); }
    | expr '+' expr { $$ = ast_create(AST_ADD, NULL, $1, $3, NULL, NULL); }
    | expr '-' expr { $$ = ast_create(AST_SUB, NULL, $1, $3, NULL, NULL); }
    | expr '*' expr { $$ = ast_create(AST_MUL, NULL, $1, $3, NULL, NULL);  }
    | expr '/' expr { $$ = ast_create(AST_DIV, NULL, $1, $3, NULL, NULL);  }
    | expr '<' expr { $$ = ast_create(AST_LT, NULL, $1, $3, NULL, NULL);  }
    | expr '>' expr { $$ = ast_create(AST_GT, NULL, $1, $3, NULL, NULL);  }
    | '!' expr { $$ = ast_create(AST_NOT, NULL, $2, NULL, NULL, NULL);  }
    | expr OP_LE expr { $$ = ast_create(AST_LE, NULL, $1, $3, NULL, NULL);  }
    | expr OP_GE expr { $$ = ast_create(AST_GE, NULL, $1, $3, NULL, NULL);  }
    | expr OP_EQ expr { $$ = ast_create(AST_EQ, NULL, $1, $3, NULL, NULL);  }
    | expr OP_NE expr { $$ = ast_create(AST_NE, NULL, $1, $3, NULL, NULL);  }
    | expr OP_AND expr { $$ = ast_create(AST_AND, NULL, $1, $3, NULL, NULL);  }
    | expr OP_OR expr { $$ = ast_create(AST_OR, NULL, $1, $3, NULL, NULL);  }
    ;

args_list: args { $$ = $1; }
         | { $$ = NULL; } ;
args: args_rest expr { $$ = ast_create(AST_ARGS, NULL, $1, $2, NULL, NULL); } ;
args_rest: args_rest expr ',' { $$ = ast_create(AST_ARGS, NULL, $1, $2, NULL, NULL); }
         | { $$ = NULL; } ;

/* control */

ctrl: KW_WHEN '(' expr ')' KW_THEN cmd { $$ = ast_create(AST_WHEN, NULL, $3, $6, NULL, NULL); }
    | KW_WHEN '(' expr ')' KW_THEN cmd KW_ELSE cmd { $$ = ast_create(AST_WHEN_ELSE, NULL, $3, $6, $8, NULL);  }
    | KW_WHILE '(' expr ')' cmd { $$ = ast_create(AST_WHILE, NULL, $3, $5, NULL, NULL);  }
    | KW_FOR '(' ident'=' expr KW_TO expr ')' cmd { $$ = ast_create(AST_FOR, NULL, $3, $5, $7, $9);  }
    ;

%%
 /*-----SUBROUTINES-----*/

void yyerror(char *s){
    fprintf(stderr, "ERROR: %s at line %d.\n", s, getLineNumber());
    errors_found = 1;
}
