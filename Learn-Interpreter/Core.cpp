#include "Common.hpp"
#include "FunctionPrototypes.hpp"

int start_PC;
int PC = -1;
int base_register;
int stack_register;
int max_line;

double return_value;
char *code_pointer;

bool break_flag;
bool return_flag;
bool exit_flag;
bool syntax_check_mode = false;

std::vector<char *> intercode;
std::vector<std::string> string_LITERAL;
std::vector<double> number_LITERAL;

CustomStack stack;
Memory memory;
CodeSet code;

extern std::vector<SymbolTable> Gtable;


void DEBUG_display_memory() {
	/// TODO: for debug
}
void DEBUG_display_symbol_tabel() {
	/// TODO: for debug
}
void syntax_check() {
	syntax_check_mode = true;

	for (PC = 1; PC < (int)intercode.size(); PC++) {
		code = first_code(PC);
		switch (code.type) {
		case Func: case Option: case Var:
		{
			break;
		}
		case Else: case End: case Exit:
		{
			code = next_code();
			check_EOF_line();
			break;
		}
		case If: case Elif: case While:
		{
			code = next_code();
			get_expression(0, EOFLine);
			break;
		}
		case For:
		{
			code = next_code();

			get_member_address(code);
			get_expression('=', 0);
			get_expression(To, 0);

			if (code.type == Step)
				get_expression(Step, 0);

			check_EOF_line();
			break;
		}
		case FuncCall:
		{
			func_call_syntax(code.symbol_numb);
			check_EOF_line();
			stack.pop();
			break;
		}
		case Print: case Println:
		{
			system_func_execute_syntax(code.type);
			break;
		}
		case Gvar: case Lvar:
		{
			get_member_address(code);
			get_expression('=', EOFLine);
			break;
		}
		case Return:
		{
			code = next_code();
			if (code.type != '?' && code.type != EOFLine)
				get_expression();
			if (code.type == '?')
				get_expression('?', 0);
			
			check_EOF_line();
			break;
		}
		case Break:
		{
			code = next_code();
			if (code.type == '?')
				get_expression('?', 0);
			
			check_EOF_line();
			break;
		}
		case EOFLine:
		{
			break;
		}
		default:
		{
			error_exit("구문 오류 : ", type_to_string(code.type));
		}
		}
	}

	syntax_check_mode = false;
}
void set_start_PC(const int n) {
	start_PC = n;
}
void execute() {
	base_register = 0;
	stack_register = memory.size();
	memory.resize(stack_register + 1000);

	break_flag = return_flag = exit_flag = false;

	PC = start_PC;
	max_line = intercode.size() - 1;
	while (PC <= max_line && !exit_flag) {
		statement();
	}
	PC = -1;
}
void statement() {
	CodeSet save;
	int top_line, end_line, var_address;
	double temp_return_value, end_data, step_data;

	if (PC > max_line || exit_flag)
		return;

	code = save = first_code(PC);
	top_line = PC;
	end_line = code.jump_addrs;

	if (code.type == If)
		end_line = end_line_of_if(PC);

	switch (code.type) {
	case If:
	{
		if (get_expression(If, 0)) {
			PC++;
			block();
			PC = end_line + 1;
			return;
		}
		PC = save.jump_addrs;

		while (look_code(PC) == Elif) {
			save = first_code(PC);
			code = next_code();
			if (get_expression()) {
				PC++;
				block();
				PC = end_line + 1;
				return;
			}
			PC = save.jump_addrs;
		}

		if (look_code(PC) == Else) {
			PC++;
			block();
			PC = end_line + 1;
			return;
		}

		PC++;
		break;
	}
	case While:
	{
		while(1) {
			if (!get_expression(While, 0))
				break;
			PC++;
			block();
			if (break_flag || return_flag || exit_flag) {
				break_flag = false;
				break;
			}
			PC = top_line;
			code = first_code(PC);
		}
		PC = end_line + 1;
		break;
	}
	case For:
	{
		save = next_code();
		var_address = get_member_address(save);

		expression('=', 0);
		set_data_type(save, DOUBLE_T);
		memory.set(var_address, stack.pop());

		end_data = get_expression(To, 0);

		if (code.type == Step)
			step_data = get_expression(Step, 0);
		else
			step_data = 1.0;

		PC = top_line;
		while (1) {
			if (step_data >= 0) {
				if (memory.get(var_address) > end_data)
					break;
			}
			else {
				if (memory.get(var_address) < end_data)
					break;
			}
			PC++;
			block();
			if (break_flag || return_flag || exit_flag) {
				break_flag = false;
				break;
			}
			memory.add(var_address, step_data);
		}
		PC = end_line + 1;
		break;
	}
	case FuncCall:
	{
		func_call(code.symbol_numb);
		stack.pop();
		PC++;
		break;
	}
	case Func:
	{
		PC = end_line + 1;
		break;
	}
	case Print: case Println:
	{
		system_func_execute(code.type);
		PC++;
		break;
	}
	case Gvar: case Lvar:
	{
		var_address = get_member_address(code);
		expression('=', 0);
		set_data_type(save, DOUBLE_T);
		memory.set(var_address, stack.pop());
		PC++;
		break;
	}
	case Return:
	{
		temp_return_value = return_value;
		code = next_code();

		if (code.type != '?' && code.type != EOFLine)
			temp_return_value = get_expression();

		post_if_set(return_flag);
		if (return_flag)
			return_value = temp_return_value;
		else
			PC++;
		break;
	}
	case Break:
	{
		code = next_code();
		post_if_set(break_flag);

		if (!break_flag)
			PC++;
		break;
	}
	case Exit:
	{
		code = next_code();
		exit_flag = true;
		break;
	}
	case Option: case Var: case EOFLine:
	{
		PC++;
		break;
	}
	default:
	{
		error_exit("구문 오류 : ", type_to_string(code.type));
	}
	}
}
void block() {
	TokenType t;
	while (!break_flag && !return_flag && !exit_flag) {
		t = look_code(PC);
		if (t == Elif || t == Else || t == End)
			break;
		statement();
	}
}
double get_expression(const int type1 = 0, const int type2 = 0) {
	expression(type1, type2);
	return stack.pop();
}
void expression(const int type1, const int type2) {
	if (type1 != 0)
		code = check_next_code(code, type1);
	expression();
	if (type2 != 0)
		code = check_next_code(code, type2);
}
void expression() {
	term(1);
}
void term(const int n) {
	TokenType op;

	if (n == 7) {
		factor();
		return;
	}

	term(n + 1);

	while (n == operation_order(code.type)) {
		op = code.type;
		code = next_code();
		term(n + 1);
		if (syntax_check_mode) {
			stack.pop((unsigned int)2);
			stack.push(1.0);
		}
		else
			binary_expression(op);
	}
}
void factor() {
	TokenType type = code.type;

	if (syntax_check_mode) {
		switch (type) {
		case Not: case Minus: case Plus:
		{
			code = next_code();
			factor();
			stack.pop();
			stack.push(1.0);
			break;
		}
		case Lparen:
		{
			expression('(', ')');
			break;
		}
		case IntNum: case DoubleNum:
		{
			stack.push(1.0);
			code = next_code();
			break;
		}
		case Gvar: case Lvar:
		{
			get_member_address(code);
			stack.push(1.0);
			break;
		}
		case Toint: case Input:
		{
			system_func_execute_syntax(type);
			break;
		}
		case FuncCall:
		{
			func_call_syntax(code.symbol_numb);
			break;
		}
		case EOFLine:
		{
			error_exit("구문 오류");
			break;
		}
		default:
		{
			error_exit("구문 오류 : ", type_to_string(code));
		}
		}
		return;
	}

	switch (type) {
	case Not: case Minus: case Plus:
	{
		code = next_code();
		factor();
		
		if (type == Not)
			stack.push(!stack.pop());
		if (type == Minus)
			stack.push(-stack.pop());
		break;
	}
	case Lparen:
	{
		expression('(', ')');
		break;
	}
	case IntNum: case DoubleNum:
	{
		stack.push(code.double_value);
		code = next_code();
		break;
	}
	case Gvar: case Lvar:
	{
		check_data_type(code);
		stack.push(memory.get(get_member_address(code)));
		break;
	}
	case Toint: case Input:
	{
		system_func_execute(type);
		break;
	}
	case FuncCall:
	{
		func_call(code.symbol_numb);
		break;
	}
	}
}
int  operation_order(const TokenType type) {
	switch (type) {
	case Multi: case Divi: case Mod: case Intdivi:
	{
		return 6;
	}
	case Plus: case Minus:
	{
		return 5;
	}
	case Less: case LessEq: case Great: case GreatEq:
	{
		return 4;
	}
	case Equal: case NotEq:
	{
		return 3;
	}
	case And:
	{
		return 2;
	}
	case Or:
	{
		return 1;
	}
	default:
	{
		return 0;
	}
	}
}
void binary_expression(const TokenType type) {
	double output = 0;
	
	double rvalue = stack.pop();
	double lvalue = stack.pop();

	if ((type == Divi || type == Mod || type == Intdivi) && rvalue == 0)
		error_exit("0으로 나눌 수 없습니다.");

	switch (type) {
	case Plus:
	{
		output = lvalue + rvalue;
		break;
	}
	case Minus:
	{
		output = lvalue - rvalue;
		break;
	}
	case Multi:
	{
		output = lvalue * rvalue;
		break;
	}
	case Divi:
	{
		output = lvalue / rvalue;
		break;
	}
	case Mod:
	{
		output = (int)lvalue % (int)rvalue;
		break;
	}
	case Intdivi:
	{
		output = (int)lvalue / (int)rvalue;
		break;
	}
	case Less:
	{
		output = lvalue < rvalue;
		break;
	}
	case LessEq:
	{
		output = lvalue <= rvalue;
		break;
	}
	case Great:
	{
		output = lvalue > rvalue;
		break;
	}
	case GreatEq:
	{
		output = lvalue >= rvalue;
		break;
	}
	case Equal:
	{
		output = lvalue == rvalue;
		break;
	}
	case NotEq:
	{
		output = lvalue != rvalue;
		break;
	}
	case And:
	{
		output = lvalue && rvalue;
		break;
	}
	case Or:
	{
		output = lvalue || rvalue;
		break;
	}
	}

	stack.push(output);
}
void post_if_set(bool& const flag) {
	if (code.type == EOFLine) {
		flag = true;
		return;
	}
	if (get_expression('?', 0)) {
		flag = true;
		return;
	}
}
void func_call_syntax(const int func_number) {
	int argc;

	code = next_code();
	code = check_next_code(code, '(');
	if (code.type != ')') {
		code = next_code();

		while (1) {
			get_expression();
			argc++;
			if(code.type != ',')
				break;
		}
	}

	code = check_next_code(code, ')');

	if (argc != Gtable[func_number].args)
		error_exit(Gtable[func_number].name, " 함수의 인수 개수가 올바르지 않습니다.");

	stack.push(1.0);
}
void func_call(const int func_number) {
	int n;
	int argc = 0;

	std::vector<double> for_reverse;

	next_code();
	code = next_code();

	if (code.type != ')') {
		code = next_code();

		while (1) {
			expression();
			++argc;
			if (code.type != ',')
				break;
		}
	}

	code = next_code();

	for (n = 0; n < argc; n++)
		for_reverse.push_back(stack.pop());
	for (n = 0; n < argc; n++)
		stack.push(for_reverse[n]);

	func_execute(func_number);
}
void func_execute(const int func_number) {
	int save_PC = PC;
	int save_base_register = base_register;
	int save_stack_register = stack_register;

	char *save_code_pointer = code_pointer;

	CodeSet save_code = code;


	PC = Gtable[func_number].addrs;
	
	base_register = stack_register;
	stack_register += Gtable[func_number].frame;

	memory.auto_resize(stack_register);

	return_value = 1.0;
	code = first_code(PC);

	next_code();
	code = next_code();

	if (code.type != ')') {
		code = next_code();
		while (1) {
			set_data_type(code, DOUBLE_T);
			memory.set(get_member_address(code), stack.pop());
			if (code.type != ',')
				break;
		}
	}

	code = next_code();

	PC++;
	block();
	return_flag = false;

	stack.push(return_value);
	
	PC = save_PC;
	base_register = save_base_register;
	stack_register = save_stack_register;

	code_pointer = save_code_pointer;

	code = save_code;
}
void system_func_execute_syntax(const TokenType type) {
	switch (type) {
	case Toint:
	{
		code = next_code();
		get_expression('(', ')');

		stack.push(1.0);
		break;
	}
	case Input:
	{
		code = next_code();
		code = check_next_code(code, '(');
		code = check_next_code(code, ')');

		stack.push(1.0);
		break;
	}
	case Print: case Println:
	{
		do {
			code = next_code();
			if (code.type == String) code = next_code();
			else get_expression();
		} while (code.type == ',');

		check_EOF_line();
		break;
	}
	}
}
void system_func_execute(const TokenType type) {
	double d;
	std::string s;

	switch (type) {
	case Toint:
	{
		code = next_code();
		stack.push((int)get_expression('(', ')'));
		break;
	}
	case Input:
	{
		next_code();
		next_code();

		code = next_code();

		getline(std::cin, s);
		stack.push(atof(s.c_str()));
		break;
	}
	case Print: case Println:
	{
		do {
			code = next_code();
			if (code.type == String) {
				std::cout << code.text;
				code = next_code();
			}
			else {
				d = get_expression();
				if (!exit_flag) {
					std::cout << d;
				}
			}
		} while (code.type == ',');

		if (type == Println)
			std::cout << "\n";

		break;
	}
	}
}
int  get_member_address(CodeSet& const cd) {
	int addr = 0;
	int index, len;
	double d;

	addr = get_top_address(cd);
	len = get_tabel_iterator(cd)->array_length;

	code = next_code();
	if (len == 0)
		return addr;

	d = get_expression('[', ']');
	if ((int)d != d)
		error_exit("배열의 길이는 정수만 가능합니다.");

	if (syntax_check_mode)
		return addr;

	index = (int)d;
	if (index <0 || len <= index)
		error_exit(index, "배열의 범위를 벗어났습니다. (0-", len - 1, "）");

	return addr + index;
}
int  get_top_address(const CodeSet& const cd) {
	switch (cd.type) {
	case Gvar:
	{
		return get_tabel_iterator(cd)->addrs;
	}
	case Lvar:
	{
		return get_tabel_iterator(cd)->addrs + base_register;
	}
	default:
	{
		error_exit("변수명이 필요합니다 : ", type_to_string(cd));
	}
	}
	return 0;
}
int  end_line_of_if(int line) {
	CodeSet cd;
	char *save = code_pointer;
	int jump;

	cd = first_code(line);
	while (1) {
		line = cd.jump_addrs;
		code = first_code(line);

		if (cd.type == Elif || cd.type == Else)
			continue;
		if (cd.type == End)
			break;
	}

	code_pointer = save;
	return line;
}
void check_EOF_line() {
	if (code.type != EOFLine)
		error_exit("구문 오류 : ", type_to_string(code));
}
TokenType look_code(const int line) {
	return (TokenType)(unsigned char)intercode[line][0];
}
CodeSet check_next_code(const CodeSet& cd, const int type2) {
	if (cd.type != type2) {
		if (type2 == EOFLine)
			error_exit("구문 오류 : ", type_to_string(cd));
		if (cd.type == EOFLine)
			error_exit(type_to_string(type2), "가 필요합니다.");
		error_exit(type_to_string(type2) + " 가(이) " + type_to_string(cd) + "앞에 필요합니다.");
	}
	return next_code();
}
CodeSet first_code(const int line) {
	code_pointer = intercode[line];
	return next_code();
}
CodeSet next_code() {
	TokenType type;
	short int jump_address, table_number;

	if (*code_pointer == '\x00')
		return CodeSet(EOFLine);

	type = (TokenType) *(unsigned char*)(code_pointer++);
	switch (type) {
	case Func: case For: case While: case If: case Elif: case Else:
	{
		jump_address = *(short *)code_pointer;
		code_pointer += sizeof(short);
		return CodeSet(type, -1, jump_address);
	}
	case String:
	{
		table_number = *(short *)(code_pointer);
		code_pointer += sizeof(short);
		return CodeSet(type, string_LITERAL[table_number].c_str());
	}
	case IntNum: case DoubleNum:
	{
		table_number = *(short *)code_pointer;
		code_pointer += sizeof(short);
		return CodeSet(type, number_LITERAL[table_number]);
	}
	case FuncCall: case Gvar: case Lvar:
	{
		table_number = *(short *)code_pointer;
		code_pointer += sizeof(short);
		return CodeSet(type, table_number, -1);
	}
	default:
	{
		return CodeSet(type);
	}
	}
}
void check_data_type(const CodeSet& cd) {
	if (get_tabel_iterator(cd)->data_type == NON_T)
		error_exit("초기화되지 않은 변수입니다. : ", type_to_string(cd));
}
void set_data_type(const CodeSet& cd, const char type) {
	int memory_address = get_top_address(cd);
	std::vector<SymbolTable>::iterator p = get_tabel_iterator(cd);

	if (p->data_type != NON_T)
		return;

	p->data_type = type;
	
	if (p->array_length != 0)
		for (int n = 0; n < p->array_length; n++)
			memory.set(memory_address + n, 0);
}
int  set_LITERAL(const double d) {
	for (int n = 0; n < (int)number_LITERAL.size(); n++) {
		if (number_LITERAL[n] == d) {
			return n;
		}
	}
	number_LITERAL.push_back(d);
	return number_LITERAL.size() - 1;
}
int  set_LITERAL(const std::string& s) {
	for (int n = 0; n < (int)string_LITERAL.size(); n++) {
		if (string_LITERAL[n] == s) {
			return n;
		}
	}
	string_LITERAL.push_back(s);
	return string_LITERAL.size() - 1;
}
void DEBUG_stack() {
	/// TODO: for debug
}