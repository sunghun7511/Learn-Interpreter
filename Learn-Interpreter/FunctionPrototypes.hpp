#ifndef __FUNCTION_PROTOTYPES__
#define __FUNCTION_PROTOTYPES__

#include "Common.hpp"

void init();
void convert_to_internal_code(const char * const fname);
void convert();
void convert_block_set();
void convert_block();
void convert_rest();
void option_set();
void var_decl();
void var_name_check(const Token& token);
void set_name();
void set_array_length();
void func_decl();
void back_patch(const int line, const int n);
void set_code(const int cd);
int  set_code(const int cd, const int number);
void set_code_rest();
void set_code_end();
void set_code_EOF_line();
void push_intercode();
bool is_local_scope();

void DEBUG_dump_src(char *s);
void DEBUG_all_prog_disp();
void init_char_type();
void file_open(const char * const filename);
void next_line();
Token next_line_token();
Token next_token();
bool is_operator_2(const char c1, const char c2);
TokenType get_type(const std::string& s);
Token check_next_token(const Token& token, const int type2);
void set_token_pointer(char *p);
std::string DEBUG_type_to_string(const int type);
std::string type_to_string(const int type);
std::string type_to_string(const CodeSet& cd);
int  get_line_number();

int  enter(SymbolTabel& tb, SymbolType type);
void set_start_local_tabel();
bool is_local_name(const std::string& name, SymbolType type);
int  search_name(const std::string& s, const int mode);
std::vector<SymbolTabel>::iterator get_tabel_iterator(const CodeSet& cd);

void DEBUG_display_memory();
void DEBUG_display_symbol_tabel();
void syntax_check();
void set_start_PC(const int n);
void execute();
void statement();
void block();
double get_expression(const int type1 = 0, const int type2 = 0);
void expression(const int type1, const int type2);
void expression();
void term(const int n);
void factor();
int  operation_order(const TokenType type);
void binary_expression(const TokenType type);
void post_if_set(const bool& flag);
void func_call_syntax(const int func_number);
void func_call(const int func_number);
void func_execute(const int func_number);
void system_func_execute_syntax(const TokenType type);
void system_func_execute(const TokenType type);
int  get_member_address(const CodeSet& cd);
int  get_top_address(const CodeSet& cd);
int  end_line_of_if(const int line);
void check_EOF_line();
TokenType look_code(const int line);
CodeSet check_next_code(const CodeSet& cd, const int type2);
CodeSet first_code(const int line);
CodeSet next_code();
void check_data_type(const CodeSet& cd);
void set_data_type(const CodeSet& cd, const char type);
int  set_LITERAL(const double d);
int  set_LITERAL(const std::string& s);
void DEBUG_stack();

std::string double_to_string(const double d);
std::string error_message(const std::string& a, const std::string& b);
void error_exit(Tobj a, ...);

#endif