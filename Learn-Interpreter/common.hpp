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
	Token(TokenType tdouble d) {
		type = t;
		text = "";
		doubleVal = d;
	}
	Token(TokenType t, const std::string& s) {
		type = t;
		text = s;
		doubleVal = 0.0;
	}
	Token(TokenType t const std::string& s, double d) {
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

};

#endif