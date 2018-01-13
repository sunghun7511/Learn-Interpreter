#pragma once

#include <vector>

class VirtualMemory {
private:
	std::vector<double> memory;
public:
	void auto_resize(const int n);

	void set(const int addrs, const double dt);
	void add(const int addrs, const double dt);

	const double get(const int addrs);

	void push_back(const double d);

	const int size();
}