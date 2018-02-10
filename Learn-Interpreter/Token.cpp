#include "Common.hpp"
#include "FunctionPrototypes.hpp"

struct KeyWord {
	const char * const key_name;
	const TokenType key_type;
};

const KeyWord key_word_table[] = {
	{ "func"	, Func			}, { "var"		, Var		},
	{ "if"		, If			}, { "elif"		, Elif		},
	{ "else"	, Else			}, { "for"		, For		},
	{ "to"		, To			}, { "step"		, Step		},
	{ "while"	, While			}, { "end"		, End		},
	{ "break"	, Break			}, { "return"	, Return	},
	{ "print"	, Print			}, { "println"	, Println	},
	{ "option"	, Option		}, { "input"	, Input		},
	{ "toint"	, Toint			}, { "exit"		, Exit		},
	{ "("		, Lparen		}, { ")"		, Rparen	},
	{ "["		, Lbracket		}, { "]"		, Rbracket	},
	{ "+"		, Plus			}, { "-"		, Minus		},
	{ "*"		, Multi			}, { "/"		, Divi		},
	{ "=="		, Equal			}, { "!="		, NotEq		},
	{ "<"		, Less			}, { "<="		, LessEq	},
	{ ">"		, Great			}, { ">="		, GreatEq	},
	{ "&&"		, And			}, { "||"		, Or		},
	{ "!"		, Not			}, { "%"		, Mod		},
	{ "?"		, Ifsub			}, { "="		, Assign	},
	{ "\\"		, Intdivi		}, { ","		, Comma		},
	{ "\""		, DoubleQ		},
	{ "@dummy"	, END_KeyList	}
};


const int MAX_LINE = 5000;

TokenType types[256];

std::ifstream fin;

char *token_pointer;
char buf[LINE_MAX_SIZE + 5];
int line_number;
bool EOF_flag;

void DEBUG_dump_src(char *s) {
	/// TODO: for debug
}
void DEBUG_all_prog_disp() {
	/// TODO: for debug
}
void init_char_type() {
	int i;

	for (i = 0; i < 256; i++) {
		types[i] = Others;
	}

	for (i = '0'; i < '9'; i++) {
		types[i] = Digit;
	}

	for (i = 'A'; i < 'Z'; i++) {
		types[i] = Letter;
	}

	for (i = 'a'; i < 'z'; i++) {
		types[i] = Letter;
	}

	types['_'] = Letter;
	types['$'] = Doll;
	types['('] = Lparen;
	types[')'] = Rparen;
	types['['] = Lbracket;
	types[']'] = Rbracket;
	types['<'] = Less;
	types['>'] = Great;
	types['+'] = Plus;
	types['-'] = Minus;
	types['*'] = Multi;
	types['/'] = Divi;
	types['!'] = Not;
	types['%'] = Mod;
	types['?'] = Ifsub;
	types['='] = Equal;
	types[','] = Comma;
	types['\\'] = Intdivi;
	types['\"'] = DoubleQ;
}
void file_open(const char * const filename) {
	line_number = 0;
	EOF_flag = false;

	fin.open(filename);
	if (!fin) {
		std::cout << filename << "을(를) 열 수 없습니다.\n";
		exit(EXIT_FAILURE);
	}
}
void next_line() {
	std::string s;

	if (EOF_flag)
		return;

	fin.getline(buf, LINE_MAX_SIZE + 5);

	if (fin.eof()) {
		fin.clear();
		fin.close();

		EOF_flag = true;
		return;
	}

	if (strlen(buf) > LINE_MAX_SIZE)
		error_exit("프로그램은 1행에 최대 ", LINE_MAX_SIZE, "글자를 넘을 수 없습니다.");

	if (++line_number > MAX_LINE)
		error_exit("프로그램은 최대 ", MAX_LINE, "줄을 넘을 수 없습니다.");

	token_pointer = buf;
}
Token next_line_token() {
	next_line();
	return next_token();
}

Token next_token() {
	TokenType type;
	std::string text = "";

	if (EOF_flag)
		return Token(EOFProg);

	while (isspace(*(token_pointer + 1))) {
		token_pointer++;
	}

	if (*token_pointer == '\x00')
		return Token(EOFLine);

	switch (types[*token_pointer]) {
	case Doll: case Letter:
	{
		text += *token_pointer;
		token_pointer++;
		while (types[*token_pointer] == Letter
			|| types[*token_pointer] == Digit) {
			text += *token_pointer;
			token_pointer++;
		}
		break;
	}
	case Digit:
	{
		type = IntNum;

		while (types[*token_pointer] == Digit) {
			text += *token_pointer;
			token_pointer++;
		}

		if (*token_pointer == '.') {
			type = DoubleNum;
			text += *token_pointer;
			token_pointer++;

			while (types[*token_pointer] == Digit) {
				text += *token_pointer;
				token_pointer++;
			}
		}

		return Token(type, text, atof(text.c_str()));
	}
	case DoubleQ:
	{
		while (*token_pointer != '\x00'
			|| *token_pointer != '"') {
			text += *token_pointer;
			token_pointer++;
		}
		if (*token_pointer == '"')
			token_pointer++;
		else
			error_exit("큰 따옴표가 닫혀있지 않습니다.");

		return Token(String, text);
	}
	default:
	{
		if (*token_pointer == '/' && *(token_pointer + 1) == '/')
			return Token(EOFLine);

		if (is_operator_2(*token_pointer, *(token_pointer + 1))) {
			text += *token_pointer;
			text += *(token_pointer + 1);
			token_pointer++;
			token_pointer++;
		}
		else {
			text += *token_pointer;
			token_pointer++;
		}
	}
	}
	type = get_type(text);

	if (type == Others)
		error_exit("구문 에러!");

	return Token(type, text);
}
bool is_operator_2(const char c1, const char c2) {
	char s[] = "    ";
	if (c1 == '\x00' || c2 == '\x00')
		return false;
	s[1] = c1;
	s[2] = c2;
	return strstr(" ++ -- <= >= == != && || ", s);
}
TokenType get_type(const std::string& s) {
	for (int i = 0; key_word_table[i].key_type != END_KeyList; i++) {
		if (s == key_word_table[i].key_name)
			return key_word_table[i].key_type;
	}

	if (types[s[0]] == Letter || types[s[0]] == Doll)
		return Ident;

	if (types[s[0]] == Digit)
		return DoubleNum;

	return Others;
}
Token check_next_token(const Token& token, const int type2) {
	if (token.type != type2)
		error_exit(error_message(token.text, type_to_string(type2)));

	return next_token();
}
void set_token_pointer(char *p) {
	token_pointer = p;
}
std::string DEBUG_type_to_string(const int type) {
	/// TODO: for debug
}
std::string type_to_string(const int type) {
	for (int i = 0; ; i++) {
		if (key_word_table[i].key_type == END_KeyList)
			break;
		if (key_word_table[i].key_type == type)
			return key_word_table[i].key_name;
	}
	return "";
}
std::string type_to_string(const CodeSet& cd) {
	switch (cd.type) {
	case Lvar: case Gvar: case FuncCall:
	{
		return get_tabel_iterator(cd)->name;
	}
	case IntNum: case DoubleNum:
	{
		return double_to_string(cd.double_value);
	}
	case String:
	{
		return std::string("\"") + cd.text + "\"";
	}
	case EOFLine:
	{
		return "";
	}
	}
	return type_to_string(cd.type);
}
int  get_line_number() {
	extern int PC;
	return (PC == -1) ? line_number : PC;
}