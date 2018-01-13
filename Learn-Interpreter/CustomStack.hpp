#pragma once

#include <stack>

class CustomStack {
private:
	std::stack<double> stk;
public:
	void push(const double n);
	const int size();
	const bool empty();
	const double pop();
};