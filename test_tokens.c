int expected_codes[] = {
	KW_BYTE,
	KW_SHORT,
	KW_LONG,
	KW_FLOAT,
	KW_DOUBLE,
	KW_WHEN,
	KW_THEN,
	KW_ELSE,
	KW_WHILE,
	KW_FOR,
	KW_READ,
	KW_RETURN,
	KW_PRINT,

	OPERATOR_LE,
	OPERATOR_GE,
	OPERATOR_EQ,
	OPERATOR_NE,
	OPERATOR_AND,
	OPERATOR_OR,

	TK_IDENTIFIER,

	LIT_INTEGER,
	LIT_REAL,
	LIT_CHAR,
	LIT_STRING,

	',',
	';',
	':',
	'(',
	')',
	'[',
	']',
	'{',
	'}',
	'+',
	'-',
	'*',
	'/',
	'<',
	'>',
	'=',
	'!',
	'&',
	'$',
	'#',

	TK_IDENTIFIER,
	TK_IDENTIFIER,
	TK_IDENTIFIER,

	LIT_STRING, LIT_STRING,
	LIT_STRING,
	LIT_STRING,
	LIT_STRING,



	TK_IDENTIFIER, TOKEN_ERROR,
	
	TK_IDENTIFIER,
	-1
};