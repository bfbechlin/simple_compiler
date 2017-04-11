 /* DEFINITIONS*/
%{
	#include "tokens.h"

%}

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
\<=		{return OPERATOR_LE;}
\>=		{return OPERATOR_GE;}
\==		{return OPERATOR_EQ;}
!=		{return OPERATOR_NE;}
&&		{return OPERATOR_AND;}
\|\|	{return OPERATOR_OR;}

%%

 /* SUBROTINES*/
#include "main.c"

int yywrap(void){
	return 1;
}
