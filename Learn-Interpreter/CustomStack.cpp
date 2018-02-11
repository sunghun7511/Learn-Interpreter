#include "CustomStack.hpp"

void CustomStack::push(const double n) {
	stk.push(n);
}

const int CustomStack::size() {
	return static_cast<int>(stk.size());
}

const bool CustomStack::empty() {
	return stk.empty();
}

const double CustomStack::pop() {
	double temp;

	if (this->empty()) {
		perror("Stack underflow!!");
		exit(EXIT_FAILURE);
	}

	temp = stk.top();
	stk.pop();

	return temp;
}

const void CustomStack::pop(const unsigned int amount) {
	int i;

	for (i = 0; i < amount; i++) {
		this->pop();
	}
}