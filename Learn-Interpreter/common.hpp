#ifndef __COMMON__
#define __COMMON__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cctype>
#include <cstdarg>

#include "CustomStack.hpp"

const int LINE_MAX_SIZE = 1024;


enum TokenType {
	Lparen		= '(',
	Rparen		= ')',
	Lbracket	= '[',
	Rbracket	= ']',
	Plus 		= '+',
	Minus		= '-',
	Multi		= '*',
	Divi		= '/',
	Mod			= '%',
	Not			= '!',
	Ifsub		= '?',
	Assign		= '=',
	Intdivi		= '\\',
	Comma		= ',',
	DoubleQ		= '"',

	Func		= 150,

	Var,
	If,
	Elif,
	Else,
	For,
	To,
	Step,
	While,
	End,
	Break,
	Return,
	Option,
	Print,
	Println,
	Input,
	Toint,
	Exit,
	Equal,
	NotEq,
	Less,
	LessEq,
	Great,
	GreatEq,
	And,
	Or,
	END_KeyList,
	Ident,
	IntNum,
	DoubleNum,
	String,
	Letter,
	Doll,
	Digit,
	Gvar,
	Lvar,
	FuncCall,
	EOFProg,
	EOFLine,
	Others
};

struct Token {
	TokenType		type;
	std::string		text;
	double			double_val;

	Token() {
		type = Others;
		text = "";
		double_val = 0.0;
	}
	Token(TokenType t) {
		type = t;
		text = "";
		double_val = 0.0;
	}
	Token(TokenType t, double d) {
		type = t;
		text = "";
		double_val = d;
	}
	Token(TokenType t, const std::string& s) {
		type = t;
		text = s;
		double_val = 0.0;
	}
	Token(TokenType t, const std::string& s, double d) {
		type = t;
		text = s;
		double_val = d;
	}
};

enum SymbolType { noId, varId, funcId, paramId };
enum DataType { NON_T, DOUBLE_T };

struct SymbolTabel {
	std::string		name;
	SymbolType		name_type;
	char			data_type;
	int				array_length;
	short			args;
	int				addrs;
	int				frame;

	SymbolTabel() {
		clear();
	}

	void clear() {
		name = "";
		name_type = noId;
		data_type = NON_T;
		array_length = 0;
		args = 0;
		addrs = 0;
		frame = 0;
	}
};

struct CodeSet {
	TokenType		type;
	const char *	text;
	double			double_value;
	int				symbol_numb;
	int				jump_addrs;

	CodeSet() { clear(); }
	CodeSet(TokenType t) { clear(); type = t; }
	CodeSet(TokenType t, double d) { clear(); type = t; double_value = d; }
	CodeSet(TokenType t, const char *s) { clear(); type = t; text = s; }
	CodeSet(TokenType t, int symbol, int jump) {
		clear(); type = t; symbol_numb = symbol; jump_addrs = jump;
	}

	void clear() {
		type = Others;
		text = "";
		double_value = 0.0;
		jump_addrs = 0;
		symbol_numb = -1;
	}
};

struct Tobj {
	char			type;
	double			d;
	std::string		s;

	Tobj() { type = '-'; d = 0.0; s = ""; }
	Tobj(double dt) { type = 'd'; d = 0.0; s = ""; }
	Tobj(const std::string& st) { type = 's'; d = 0.0; s = st; }
	Tobj(const char * st) { type = 's'; d = 0.0; s = st; }
};

class Memory {
private:
	std::vector<double> memory;
public:
	void auto_resize(int n) {
		if (n >= (int)memory.size()) {
			n = (n / 256 + 1) * 256;
			memory.resize(n);
		}
	}

	void set(int addrs, double dt) {
		memory[addrs] = dt;
	}

	void add(int addrs, double dt) {
		memory[addrs] += dt;
	}

	double get(int addrs) {
		return memory[addrs];
	}

	int size() {
		return (int)memory.size();
	}

	void resize(unsigned int n) {
		memory.resize(n);
	}
};

#endif