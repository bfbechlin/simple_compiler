 /* DEFINITIONS*/
%{
	#include "tokens.h"
	#include "hashmap.h"

	void initMe(void);
	int getLineNumber(void);
	int isRunning(void);

	void hashPut(int code);
	
	int running = 1;
	int lineCounter = 1;
	int tokenCode = 0;
	struct hashmap hash;

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
read	{return KW_READ;}
return	{return KW_RETURN;}
print	{return KW_PRINT;}

 /* SPECIAL CARACTERS*/
[,;:\(\)\[\]\{\}\+\-*<>=!&$#/] {return yytext[0];}

 /* COMPOSED OPERATORS*/
"<="	{return OPERATOR_LE;}
">="	{return OPERATOR_GE;}
"=="	{return OPERATOR_EQ;}
"!="	{return OPERATOR_NE;}
"&&"	{return OPERATOR_AND;}
"||"	{return OPERATOR_OR;}

 /* IDENTIFIERS*/
[_A-Za-z][_A-Za-z0-9]*	{hashPut(TK_IDENTIFIER);return TK_IDENTIFIER;}

 /* LITERALS*/
[0-9]+			{hashPut(LIT_INTEGER);return LIT_INTEGER;}
[0-9]+.[0-9]+	{hashPut(LIT_REAL);return LIT_REAL;}
'.'				{hashPut(LIT_CHAR);return LIT_CHAR;}

 /* STRINGS */
\"(\\.|[^\\"\n])*\" {hashPut(LIT_STRING);return LIT_STRING;}

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
void initMe(){
	hm_initialize(8, 0.5, sizeof(int), &hash);
}

#include "main.c"

int yywrap(void){
	running = 0;
	return 1;
}

void hashPut(int code){
	hm_put(&hash, yytext, &code);
}

int getLineNumber(void){
	return lineCounter;
}

int isRunning(void){
	return running;
}
