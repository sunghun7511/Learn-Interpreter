#include "Common.hpp"
#include "FunctionPrototypes.hpp"

std::vector<SymbolTabel> Gtable;
std::vector<SymbolTabel> Ltable;
int start_Ltable;

int  enter(SymbolTabel& tb, SymbolType type) {
	int n, mem_size;
	bool is_local = is_local_name(tb.name, type);
	extern int local_address;
	extern Memory memory;
	
	mem_size = tb.array_length;
	if (mem_size == 0)
		mem_size = 1;

	if (type != varId && tb.name[0] == '$')
		error_exit("변수명 이외에서 $ 를 사용할 수 없습니다.", tb.name);
	tb.name_type = type;

	n = -1;
	if (type == funcId)
		n = search_name(tb.name, 'G');
	if (type == paramId)
		n = search_name(tb.name, 'L');

	if (n != -1)
		error_exit("이름이 중복되었습니다.", tb.name);

	if (type == funcId)
		tb.addrs = get_line_number();
	else {
		if (is_local) {
			tb.addrs = local_address;
			local_address += mem_size;
		}
		else {
			tb.addrs = memory.size();
			memory.resize(memory.size() + mem_size);
		}
	}

	if (is_local) {
		n = Ltable.size();
		Ltable.push_back(tb);
	}
	else {
		n = Gtable.size();
		Gtable.push_back(tb);
	}

	return n;
}
void set_start_local_tabel() {
	start_Ltable = Ltable.size();
}
bool is_local_name(const std::string& name, SymbolType type) {
	if (type == paramId)
		return true;
	if (type == varId) {
		return is_local_scope && name[0] != '$';
	}
	return false;
}
int  search_name(const std::string& s, const int mode) {
	int i;

	switch (mode) {
	case 'G':
	{
		for (i = 0; i < (int)Gtable.size(); i++) {
			if (Gtable[i].name == s) {
				return i;
			}
		}
		break;
	}
	case 'L':
	{
		for (i = 0; i < (int)Ltable.size(); i++) {
			if (Ltable[i].name == s) {
				return i;
			}
		}
		break;
	}
	case 'F':
	{
		i = search_name(s, 'G');
		if (i != -1 && Gtable[i].name_type == funcId)
			return i;
		break;
	}
	case 'V':
	{
		if (search_name(s, 'F') != -1)
			error_exit("함수명과 중복된 변수명입니다. : ", s);
		if (s[0] == '$')
			return search_name(s, 'G');
		if (is_local_scope())
			return search_name(s, 'L');
		else
			return search_name(s, 'G');
		break;
	}
	}
	return -1;
}
std::vector<SymbolTabel>::iterator get_tabel_iterator(const CodeSet& cd) {
	if (cd.type == Lvar)
		return Ltable.begin() + cd.symbol_numb;
	return Gtable.begin() + cd.symbol_numb;
}