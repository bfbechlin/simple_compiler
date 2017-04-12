 /* DEFINITIONS*/
%{
	#include "tokens.h"

	int isRunning = 0;

	int getLineNumber(void);
	int lineCounter = 1;
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
[_A-Za-z][_A-Za-z0-9]*	{return TK_IDENTIFIER;}

 /* LITERALS*/
[0-9]+			{return LIT_INTEGER;}
[0-9]+.[0-9]+	{return LIT_REAL;}
'.'				{return LIT_CHAR;}

 /* STRINGS */
\"(\\.|[^\\"\n])*\" {return LIT_STRING;}

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
#include "main.c"

int yywrap(void){
	isRunning = 0;
	return 1;
}

int getLineNumber(void){
	return lineCounter;
}
