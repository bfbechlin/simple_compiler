 /* DEFINITIONS*/
%{
	#include "symbols_table.h"
	#include "parser.h"

	int set_input_file(char* file_name);

	int getLineNumber(void);
	int isRunning(void);

	int running = 1;
	int lineCounter = 1;
	int tokenCode = 0;

%}

%x comment

%%

 /* RULES*/

 /* KEYWORDS*/
byte	{return KW_BYTE;}
short	{return KW_SHORT;}
long	{return KW_LONG;}
float	{return KW_FLOAT;}
double	{return KW_DOUBLE;}
when	{return KW_WHEN;}
then	{return KW_THEN;}
else	{return KW_ELSE;}
while 	{return KW_WHILE;}
for 	{return KW_FOR;}
to      {return KW_TO;}
read	{return KW_READ;}
return	{return KW_RETURN;}
print	{return KW_PRINT;}

 /* SPECIAL CARACTERS*/
[,;:\(\)\[\]\{\}\+\-*<>=!&$#/] {return yytext[0];}

 /* COMPOSED OPERATORS*/
"<="	{return OP_LE;}
">="	{return OP_GE;}
"=="	{return OP_EQ;}
"!="	{return OP_NE;}
"&&"	{return OP_AND;}
"||"	{return OP_OR;}

 /* IDENTIFIERS*/
[_A-Za-z][_A-Za-z0-9]*	{symtab_insert(yytext, SYMBOL_IDENTIFIER);return TK_ID;}

 /* LITERALS*/
[0-9]+			{symtab_insert(yytext, SYMBOL_LIT_INT);return LIT_INT;}
[0-9]+\.[0-9]+	{symtab_insert(yytext, SYMBOL_LIT_REAL);return LIT_REAL;}
'.'				{symtab_insert(yytext, SYMBOL_LIT_CHAR);return LIT_CHAR;}

 /* STRINGS */
\"(\\.|[^\\"\n])*\" {symtab_insert(yytext, SYMBOL_LIT_STRING);return LIT_STRING;}

 /* LINE COUNTER*/
\n			{lineCounter++;}

 /* SINGLELINE COMMENTS*/
\/\/.*\n	{lineCounter++;}

 /* MULTILINE COMMENTS */
"/*"		BEGIN(comment);
<comment>.		{}
<comment>\n		{lineCounter++;}
<comment>"*/"	BEGIN(INITIAL);

 /* SPACES AND TABS*/
[ \t]		{}
 /* ERROR*/
.			{return TOKEN_ERROR;}
%%

 /* SUBROTINES*/

int yywrap(void){
	running = 0;
	return 1;
}

int set_input_file(char* file_name){
	if(yyin = fopen(file_name, "r"))
		return 1;
	return 0;
}

int getLineNumber(void){
	return lineCounter;
}

int isRunning(void){
	return running;
}
