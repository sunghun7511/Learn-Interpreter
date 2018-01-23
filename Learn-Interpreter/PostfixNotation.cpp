#include <iostream>
#include <iomanip>

const int STK_SIZE = 100;
const int MAX_INPUT = 100;

void polish(const char * s);
int execute();
int getvalue(const int ch);
int order(const int ch);
void push(const int n);
int pop();

int stack[STK_SIZE + 1];
int stack_ptr;

char polish_out[80];

int main_postfix_notation(int argc, char * argv[]) {
	
	char buf[MAX_INPUT];
	int result = 0;

	std::cout << "식을 입력해주세요 : ";
	std::cin >> std::setw(MAX_INPUT) >> buf;

	polish(buf);

	if (polish_out[0] == '\x00')
		std::exit(EXIT_FAILURE);

	result = execute();

	std::cout << "반환 : " << polish_out << "\n"
		<< "결과 : " << result << "\n";

	return 0;
}

void polish(const char * s) {
	int tmp = 0;
	char *out = polish_out;

	stack_ptr = 0;
	while (true) {
		while (isspace(*s)) { ++s; }

		if (*s == '\x00') {
			while (stack_ptr > 0) {
				*out = pop();
				if (*out == '(') {
					std::cout << "'('가 올바르지 않습니다.\n";
					std::exit(EXIT_FAILURE);
				}
				out++;
			}
			break;
		}

		if (islower(*s) || isdigit(*s)) {
			*out = *s;

			out++;
			s++;
			continue;
		}

		switch (*s) {
		case '(':
		{
			push(*s);
			break;
		}
		case ')':
		{
			while ((tmp = pop()) != '(') {
				*out = tmp;
				out++;

				if (stack_ptr == 0) {
					std::cout << "'('가 올바르지 않습니다.\n";
					std::exit(EXIT_FAILURE);
				}
			}
			break;
		}
		default:
		{
			if (order(*s) == -1) {
				std::cout << "올바르지 않은 문자입니다 : " << *s << "\n";
				std::exit(EXIT_FAILURE);
			}
			tmp = stack[stack_ptr];
			while (stack_ptr > 0 
				&& (order(tmp) >= order(*s))) {
				*out = pop();
				out++;
			}
			push(*s);
		}
		}
		s++;
	}
	*out = '\x00';
}

int execute() {
	int lhs = 0;
	int rhs = 0;

	char *s;

	stack_ptr = 0;

	for (s = polish_out; *s; s++) {
		if (islower(*s)) {
			push(getvalue(*s));
		}
		else if (isdigit(*s)) {
			push(*s - '0');
		}
		else {
			rhs = pop(); lhs = pop();

			switch (*s) {
			case '+':
			{
				push(lhs + rhs);
				break;
			}
			case '-':
			{
				push(lhs - rhs);
				break;
			}
			case '*':
			{
				push(lhs * rhs);
				break;
			}
			case '/':
			{
				if (rhs == 0) {
					std::cout << "0으로 나눌 수 없습니다.\n";
					std::exit(EXIT_FAILURE);
				}
				push(lhs / rhs);
				break;
			}
			}

		}
	}
	if (stack_ptr != 1) {
		std::cout << "Error!\n";
		std::exit(EXIT_FAILURE);
	}

	return pop();
}

int getvalue(const int ch) {
	int result = 0;

	if (islower(ch)) {
		result = ch - 'a' + 1;
	}

	return 0;
}

int order(const int ch) {
	switch (ch) {
	case '*': case '/':
	{
		return 3;
		break;
	}
	case '+': case '-':
	{
		return 2;
		break;
	}
	case '(': case ')':
	{
		return 1;
		break;
	}
	}

	return -1;
}

void push(const int n) {
	if (stack_ptr > STK_SIZE) {
		std::cout << "STACK OVERFLOW!!\n";
		std::exit(EXIT_FAILURE);
	}
	stack[++stack_ptr] = n;
}

int pop() {
	if (stack_ptr < 1) {
		std::cout << "STACK UNDERFLOW\n";
		std::exit(EXIT_FAILURE);
	}
	return stack[stack_ptr--];
}