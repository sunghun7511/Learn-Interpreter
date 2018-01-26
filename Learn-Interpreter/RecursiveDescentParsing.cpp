#include <iostream>
#include <iomanip>

#include <cctype>

enum TokenType {
	Print, Lparen, Rparen, Plus, Minus, Multi, Divi,
	Assign, VarName, IntNum, EofToken, Others
};

struct Token {
	TokenType type;
	int intVal;
	
	Token() {
		type = Others;
		intVal = 0;
	}

	Token(TokenType t, int val = 0) {
		type = t;
		intVal = val;
	}
};

void input();
void statement();
void expression();
void term();
void factor();
Token nextToken();
int nextCh();
void operate(TokenType op);

void push(int n);
int pop();

void checkToken(TokenType kind);

const int STACK_SIZE = 100;
const int BUF_SIZE = 100;

int stack[STACK_SIZE + 1];		// ����
int stack_ptr;					// ���� ������

Token token;					// ��ū ����

char buf[BUF_SIZE], *bufp;			// �Է� ����

int ch;							// ������ ���ڿ�
int var[26];					// ���� a-z

int errF;						// ���� ����


int main_recursive_descent_parsing(int argc, char * argv[]) {
	while (true) {
		input();

		token = nextToken();
		if (token.type == Others) {
			std::exit(EXIT_FAILURE);
		}
		
		statement();

		if (errF) {
			std::cout << "ERROR!\n";
		}
	}
	return 0;
}

void input() {
	errF = 0;
	stack_ptr = 0;

	std::cin >> std::setw(BUF_SIZE) >> buf;
	bufp = buf; // ���� ��ġ�� �����س��´�.

	ch = nextCh();
}

void statement() {						// ��
	int vNbr;

	switch (token.type) {
	case VarName:
	{
		vNbr = token.intVal;

		token = nextToken();
		
		checkToken(Assign);
		if (errF) {
			break;
		}

		token = nextToken();

		expression();

		var[vNbr] = pop();
		break;
	}
	case Print:
	{
		token = nextToken();

		expression();

		checkToken(EofToken);
		if (errF) {
			break;
		}
		
		std::cout << " " << pop() << "\n";
		break;
	}
	default:
	{
		errF = 1;
		break;
	}
	}

	checkToken(EofToken);
}

void expression() {						// ��
	TokenType op;

	term();

	while (token.type == Plus
		|| token.type == Minus) {
		op = token.type;

		token = nextToken();

		term();
		operate(op);
	}
}

void term() {							// ��
	TokenType op;

	factor();
	
	while (token.type == Multi
		|| token.type == Divi) {
		op = token.type;

		token = nextToken();
		
		factor();
		operate(op);
	}
}

void factor() {							// ����
	switch (token.type) {
	case VarName:
	{
		push(var[token.intVal]);
		break;
	}
	case IntNum:
	{
		push(token.intVal);
		break;
	}
	case Lparen:
	{
		token = nextToken();

		expression();
		checkToken(Rparen);
		break;
	}
	default:
	{
		errF = 1;
		break;
	}
	}
	token = nextToken();
}

Token nextToken() {
	TokenType type = Others;
	int num;

	while (isspace(ch)) {
		ch = nextCh();
	}

	if (isdigit(ch)) {
		for (num = 0; isdigit(ch); ch = nextCh()) {
			num = num * 10 + (ch - '0');
		}
		return Token(IntNum, num);
	}
	else if (islower(ch)) {
		num = ch - 'a';
		ch = nextCh();
		return Token(VarName, num);
	}
	else {
		switch (ch) {
		case '(': { type = Lparen; break; }
		case ')': { type = Rparen; break; }
		case '+': { type = Plus; break; }
		case '-': { type = Minus; break; }
		case '*': { type = Multi; break; }
		case '/': { type = Divi; break; }
		case '=': { type = Assign; break; }
		case '?': { type = Print; break; }
		case '\x00': { type = EofToken; break; }
		}

		ch = nextCh();

		return Token(type);
	}
}

int nextCh() {
	if (*bufp == '\x00')
		return '\x00';
	else
		return *bufp++;
}

void operate(TokenType op) {
	int rhs = pop();
	int lhs = pop();

	if (op == Divi && rhs == 0) {
		std::cout << " division by 0 error! \n";
		errF = 1;
	}

	if (errF) return;
	
	switch (op) {
	case Plus: {push(lhs + rhs); break; }
	case Minus: {push(lhs - rhs); break; }
	case Multi: {push(lhs * rhs); break; }
	case Divi: {push(lhs / rhs); break; }
	}
}

void push(int n) {
	if (errF) return;

	if (stack_ptr + 1 > STACK_SIZE) {
		std::cout << "STACK OVERFLOW!!\n";
		std::exit(EXIT_FAILURE);
	}
	++stack_ptr;
	stack[stack_ptr] = n;
}

int pop() {
	if (errF) return 1;

	if (stack_ptr < 1) {
		std::cout << "STACK UNDERFLOW!!\n";
		std::exit(EXIT_FAILURE);
	}
	stack_ptr--;
	return stack[stack_ptr];
}

void checkToken(TokenType type) {
	if (token.type != type) errF = 1;
}