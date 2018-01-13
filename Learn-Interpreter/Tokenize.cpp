#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cctype>

enum TokenType {
	Lparen=1,	Rparen,	Plus,	Minus,		Multi,	Divi,
	Assign,		Comma,	DblQ,
	Equal,		NotEq,	Less,	LessEq,		Great,	GreatEq,
	If,			Else,	End,	Print,		Ident,	IntNum,
	String,		Letter,	Digit,	EofToken,	Others,	END_list
};

struct Token {
	TokenType type;
	std::string text;
	int intVal;

	Token () { 
		type = Others;
		text = "";
		intVal = 0;
	}
	Token(const TokenType t, const std::string& s, int iVal) {
		type = t;
		text = s;
		intVal = iVal;
	}
};

struct KeyWord {
	const char *keyName;
	TokenType keyType;
};

void initChType();
Token nextToken();
int nextCh();
bool is_ope2(int c1, int c2);
TokenType get_Type(const std::string& s);

TokenType ctyp[256];
Token token;
std::ifstream fin;

KeyWord keyWdTable[] = {
	{"if",	If		}, {"else",		Else	},
	{"end",	End		}, {"print",	Print	},
	{"(",	Lparen	}, {")",		Rparen	},
	{"+",	Plus	}, {"-",		Minus	},
	{"*",	Multi	}, {"/",		Divi	},
	{"=",	Assign	}, {",",		Comma	},
	{"==",	Equal	}, {"!=",		NotEq	},
	{"<",	Less	}, {"<=",		LessEq	},
	{">",	Great	}, {">=",		GreatEq	},
	{"",	END_list}
};

int main(int argc, char * argv[]) {
	if (argc == 1) {
		exit(EXIT_FAILURE);
	}

	fin.open(argv[1]);

	if (!fin) {
		exit(EXIT_FAILURE);
	}

	std::cout << "text	type	intValue";
	initChType();

	for (token = nextToken(); token.type != EofToken; token = nextToken()) {
		std::cout << std::left << std::setw(10) << token.text
			<< std::right << std::setw(3) << token.type
			<< " " << token.intVal << "\n";
	}
	
	return 0;
}