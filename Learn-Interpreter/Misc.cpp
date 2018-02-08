#include "Common.hpp"
#include "FunctionPrototypes.hpp"

std::string double_to_string(const double d) {
	std::ostringstream ostr;
	ostr << d;
	return ostr.str();
}
std::string error_message(const std::string& a, const std::string& b) {
	if (a == "") return b + "�� �ʿ��մϴ�.";
	if (b == "") return a + "�� �ٸ��� �ʽ��ϴ�.";
	return b + "�� " + a + " �տ� �ʿ��մϴ�.";
}
void error_exit(Tobj a, ...) {
	int i, count;
	va_list vl;

	va_start(vl, count);

	std::cerr << "line_" << get_line_number() << " ERROR ";

	for (i = 0; i < count; i++) {
		Tobj t = va_arg(vl, Tobj);
		if (t.type == 'd') {
			std::cout << t.d;
		}
		else if (t.type == 's') {
			std::cout << t.s;
		}
	}

	va_end(vl);
}