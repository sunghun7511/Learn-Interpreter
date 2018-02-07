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
	double			doubleVal;

	Token() {
		type = Others;
		text = "";
		doubleVal = 0.0;
	}
	Token(TokenType t) {
		type = t;
		text = "";
		doubleVal = 0.0;
	}
	Token(TokenType t, double d) {
		type = t;
		text = "";
		doubleVal = d;
	}
	Token(TokenType t, const std::string& s) {
		type = t;
		text = s;
		doubleVal = 0.0;
	}
	Token(TokenType t, const std::string& s, double d) {
		type = t;
		text = s;
		doubleVal = d;
	}
};

enum SymbolType { noId, varId, funcId, paramId };
enum DataType { NON_T, DOUBLE_T };

struct SymbolTabel {
	std::string		name;
	SymbolType		nameType;
	char			dataType;
	int				arrayLength;
	short			args;
	int				addrs;
	int				frame;

	SymbolTabel() {
		clear();
	}

	void clear() {
		name = "";
		nameType = noId;
		dataType = NON_T;
		arrayLength = 0;
		args = 0;
		addrs = 0;
		frame = 0;
	}
};

struct CodeSet {
	TokenType		type;
	const char *	text;
	double			doubleValue;
	int				symbolNumb;
	int				jumpAddrs;

	CodeSet() { clear(); }
	CodeSet(TokenType t) { clear(); type = t; }
	CodeSet(TokenType t, double d) { clear(); type = t; doubleValue = d; }
	CodeSet(TokenType t, const char *s) { clear(); type = t; text = s; }
	CodeSet(TokenType t, int symbol, int jump) {
		clear(); type = t; symbolNumb = symbol; jumpAddrs = jump;
	}

	void clear() {
		type = Others;
		text = "";
		doubleValue = 0.0;
		jumpAddrs = 0;
		symbolNumb = -1;
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