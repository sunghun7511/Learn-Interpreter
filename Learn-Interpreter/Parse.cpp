#include "Common.hpp"
#include "FunctionPrototypes.hpp"

const int NO_FIX_ADDRS = 0;

Token token;
SymbolTabel temp_table;

int block_depth;
int local_address;
int main_tabel_number;
int loop_depth;

bool func_decl_now;
bool explicit_variable;

char codebuf[LINE_MAX_SIZE + 1];
char *code_pointer;

extern std::vector<char *> intercode;

void init() {
	init_char_type();

	main_tabel_number = -1;
	block_depth = loop_depth = 0;
	func_decl_now = explicit_variable = false;

	code_pointer = codebuf;
}

void convert_to_internal_code(const char * const fname) {
	init();

	file_open(fname);
	
	while (token = next_line_token(), token.type != EOFProg) {
		if (token.type == Func) {
			token = next_token();
			set_name();
			enter(temp_table, funcId);
		}
	}

	push_intercode();
	file_open(fname);
	token = next_line_token();

	while (token.type != EOFProg) {
		convert();
	}

	set_start_PC(1);

	if (main_tabel_number != -1) {
		set_start_PC(intercode.size());
		
		set_code(FuncCall, main_tabel_number);
		set_code('(');
		set_code(')');

		push_intercode();
	}
}
void convert() {
	switch (token.type) {
	case Option:
	{
		option_set();
		break;
	}
	case Var:
	{
		var_decl();
		break;
	}
	case Func:
	{
		func_decl();
		break;
	}
	case While: case For:
	{
		++loop_depth;
		convert_block_set();
		set_code_end();
		--loop_depth;
		break;
	}
	case If:
	{
		convert_block_set();
		while (token.type == Elif) { convert_block_set(); }
		if (token.type == Else) { convert_block_set(); }
		set_code_end();
		break;
	}
	case Break:
	{
		if (loop_depth <= 0) {
			error_exit("Unknown break..");
		}
		set_code(token.type);
		token = next_token();
		convert_rest();
		break;
	}
	case Return:
	{
		if (!func_decl_now) {
			error_exit("Unknown return..");
		}
		set_code(token.type);
		token = next_token();
		convert_rest();
		break;
	}
	case Exit:
	{
		set_code(token.type);
		token = next_token();
		convert_rest();
		break;
	}
	case Print: case Println:
	{
		set_code(token.type);
		token = next_token();
		convert_rest();
		break;
	}
	case End:
	{
		error_exit("Unknown end..");
		break;
	}
	default:
	{
		convert_rest();
		break;
	}
	}
}
void convert_block_set() {
	const int patch_line = set_code(token.type, NO_FIX_ADDRS);
	token = next_token();
	convert_rest();
	convert_block();
	back_patch(patch_line, get_line_number());
}
void convert_block() {
	TokenType t;

	++block_depth;
	while (t = token.type, t != Elif && t != Else && t != End && t != EOFProg) {
		convert();
	}
	--block_depth;
}
void convert_rest() {
	int tabel_number;

	while (1) {
		if (token.type == EOFLine)
			break;
		switch (token.type) {
		case If:
		case Elif:
		case Else:
		case For:
		case While:
		case Break:
		case Func:
		case Return:
		case Exit:
		case Print:
		case Println:
		case Option:
		case Var:
		case End:
		{
			error_exit("구문이 잘못되었습니다 : ", token.text);
			break;
		}
		case Ident:
		{
			set_name();
			if ((tabel_number = search_name(temp_table.name, 'F')) != -1) {
				if (temp_table.name == "main")
					error_exit("main 함수는 호출할 수 없습니다.");
				set_code(FuncCall, tabel_number);
				continue;
			}
			if ((tabel_number = search_name(temp_table.name, 'V')) != -1) {
				if (explicit_variable)
					error_exit("변수 선언이 필요합니다 : ", temp_table.name);
				tabel_number = enter(temp_table, varId);
			}
			if (is_local_name(temp_table.name, varId))
				set_code(Lvar, tabel_number);
			else
				set_code(Gvar, tabel_number);
			continue;
		}
		case IntNum:
		case DoubleNum:
		{
			set_code(token.type, set_LITERAL(token.double_val));
			break;
		}
		case String:
		{
			set_code(token.type, set_LITERAL(token.text));
			break;
		}
		default:
		{
			set_code(token.type);
			break;
		}
		}
		token = next_token();
	}
	push_intercode();
	token = next_line_token();
}
void option_set() {
	set_code(Option);
	set_code_rest();

	token = next_token();
	if (token.type == String && token.text == "var")
		explicit_variable = true;
	else
		error_exit("option 사용법이 올바르지 않습니다.");
	
	token = next_token();
	set_code_EOF_line();
}

void var_decl() {
	set_code(Var);
	set_code_rest();

	while (1) {
		token = next_token();
		var_name_check(token);
		set_name();
		set_array_length();
		enter(temp_table, varId);

		if (token.type != ',')
			break;
	}

	set_code_EOF_line();
}
void var_name_check(const Token& token) {
	if (token.type != Ident)
		error_exit(error_message(token.text, "식별자"));
	if (is_local_scope() && token.text[0] == '$')
		error_exit("함수 내에 var 선언에서는 $ 키워드를 사용할 수 없습니다 : ", token.text);
	if (search_name(token.text, 'V') != -1)
		error_exit("이미 존재하는 변수입니다 : ", token.text);
}
void set_name() {
	if (token.type != Ident)
		error_exit("식별자가 필요합니다 : ", token.text);
	temp_table.clear();
	temp_table.name = token.text;
	token = next_token();
}
void set_array_length() {
	temp_table.array_length = 0;
	if (token.type != '[')
		return;

	token = next_token();

	if (token.type != IntNum)
		error_exit("배열의 길이가 올바르지 않습니다 : ", token.text);

	temp_table.array_length = (int)token.double_val;
	token = check_next_token(next_token(), ']');
	
	if (token.type == '[')
		error_exit("최대 배열은 1차원까지만 가능합니다.");
}
void func_decl() {
	extern std::vector<SymbolTabel> Gtable;

	int tabel_number, patch_line, func_table_number;
	
	if (block_depth)
		error_exit("함수 정의 위치가 올바르지 않습니다.");

	func_decl_now = true;
	local_address = 0;

	set_start_local_tabel();
	patch_line = set_code(Func, NO_FIX_ADDRS);
	token = next_token();

	func_table_number = search_name(token.text, 'F');
	Gtable[func_table_number].data_type = DOUBLE_T;

	token = next_token();
	token = check_next_token(token, '(');
	set_code('(');

	if (token.type != ')') {
		for (;; token = next_token()) {
			set_name();
			tabel_number = enter(temp_table, paramId);
			set_code(Lvar, tabel_number);
			++Gtable[func_table_number].args;
			if (token.type != ',')
				break;
			set_code(',');
		}
	}

	token = check_next_token(token, ')');
	set_code(')');
	set_code_EOF_line();
	convert_block();

	back_patch(patch_line, get_line_number());
	set_code_end();
	Gtable[func_table_number].frame = local_address;

	if (Gtable[func_table_number].name == "main") {
		main_tabel_number = func_table_number;
		if (Gtable[main_tabel_number].args != 0) {
			error_exit("main 함수에는 인수를 지정할 수 없습니다.");
		}
	}
	func_decl_now = false;
}
void back_patch(const int line, const int n) {
	*(short *)(intercode[line] + 1) = (short)n;
}
void set_code(const int cd) {
	*code_pointer = (char)cd;
	code_pointer++;
}
int  set_code(const int cd, const int number) {
	*code_pointer = (char)cd;
	code_pointer++;

	*(short *)code_pointer = (short)number;
	code_pointer += sizeof(short);
	return get_line_number();
}
void set_code_rest() {
	extern char *token_pointer;
	strcpy(code_pointer, token_pointer);
	code_pointer += strlen(token_pointer) + 1;
}
void set_code_end() {
	if (token.type != End)
		error_exit(error_message(token.text, "end"));

	set_code(End);
	token = next_token();
	set_code_EOF_line();
}
void set_code_EOF_line() {
	if (token.type != EOFLine)
		error_exit("잘못된 사용입니다 : ", token.text);

	push_intercode();
	token = next_line_token();
}
void push_intercode() {
	const int len = code_pointer - codebuf;

	if (len >= LINE_MAX_SIZE)
		error_exit("문장이 너무 깁니다. 식을 간소화해주세요.");

	*code_pointer = '\x00';

	try {
		char * const p = new char[len];
		memcpy(p, codebuf, len);
		intercode.push_back(p);
	}
	catch (std::bad_alloc) {
		error_exit("메모리를 확보할 수 없습니다.");
	}
	code_pointer = codebuf;
}
bool is_local_scope() {
	return func_decl_now;
}