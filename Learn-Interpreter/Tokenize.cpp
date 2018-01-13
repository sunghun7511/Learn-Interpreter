#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cctype>

enum TokenType {
	Lparen = 1, Rparen, Plus, Minus, Multi, Divi,
	Assign, Comma, DblQ,
	Equal, NotEq, Less, LessEq, Great, GreatEq,
	If, Else, End, Print, Ident, IntNum,
	String, Letter, Digit, EofToken, Others, END_list
};

struct Token {
	TokenType type;
	std::string text;
	int intVal;

	Token() {
		type = Others;
		text = "";
		intVal = 0;
	}
	Token(const TokenType t, const std::string& s, int iVal = 0) {
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
void helpInit(const int start, const int last, const TokenType keyword);
Token nextToken();
int nextCh();
bool is_ope2(const int c1, const int c2);
TokenType get_Type(const std::string& s);

TokenType ctyp[256];
Token token;
std::ifstream fin;

KeyWord keyWordTable[] = {
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

void initChType() {
	int i;

	helpInit(0, 255, Others);

	helpInit('0', '9', Digit);
	helpInit('A', 'Z', Letter);
	helpInit('a', 'z', Letter);

	ctyp['('] = Lparen;
	ctyp[')'] = Rparen;
	ctyp['<'] = Less;
	ctyp['>'] = Great;
	ctyp['+'] = Plus;
	ctyp['-'] = Minus;
	ctyp['*'] = Multi;
	ctyp['/'] = Divi;
	ctyp['_'] = Letter;
	ctyp['='] = Assign;
	ctyp[','] = Comma;
	ctyp['"'] = DblQ;
}

void helpInit(const int start, const int last, const TokenType keyword) {
	int i;
	for (i = start; i <= last; i++) {
		ctyp[i] = keyword;
	}
}

Token nextToken() {
	TokenType type;
	int ch0, num = 0;
	std::string txt = "";

	static int ch = ' ';

	while (std::isspace(ch)) ch = nextCh();

	if (ch == EOF) return Token(EofToken, txt);

	switch (ctyp[ch]) {
	case Letter:
	{
		for (; ctyp[ch] == Letter || ctyp[ch] == Digit; ch = nextCh())
			txt += ch;
	}
	break;
	case Digit:
	{
		for (num = 0; ctyp[ch] == Digit; ch = nextCh())
			num = num * 10 + (ch - '0');
	}
	break;
	case DblQ:
	{
		for (ch = nextCh(); ch != EOF && ch != '\n' && ch != '"'; ch = nextCh())
			txt += ch;

		if (ch != '"') {
			std::cout << "문자열 리터럴을 닫지 않았습니다..\n\n"
				<< txt;
			exit(EXIT_FAILURE);
		}

		ch = nextCh();
		return Token(String, txt);
	}
	default:
	{
		txt += ch;
		ch0 = ch;
		ch = nextCh();
		if (is_ope2(ch0, ch)) {
			txt += ch;
			ch = nextCh();
		}
	}
	}

	type = get_Type(txt);

	if (type == Others) {
		std::cout << "알 수 없는 토큰입니다..\n\n"
			<< txt;
		exit(EXIT_FAILURE);
	}

	return Token(type, txt);
}

int nextCh() {
	static int c = 0;

	if (c == EOF) return c;
	if ((c = fin.get()) == EOF) fin.close();

	return c;
}

bool is_ope2(const int c1, const int c2) {
	char s[] = "   ";

	if (c1 == '\0' || c2 == '\0') return false;

	s[1] = c1;
	s[2] = c2;

	return strstr(" <= >= == !=", s) != NULL;
}

TokenType get_Type(const std::string& s) {
	int i;
	
	for (i = 0; keyWordTable[i].keyType != END_list; i++) {
		if (s == keyWordTable[i].keyName) return keyWordTable[i].keyType;
	}

	if (ctyp[s[0]] == Letter) return Ident;
	if (ctyp[s[0]] == Digit) return IntNum;

	return Others;
}