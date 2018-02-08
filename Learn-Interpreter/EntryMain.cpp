#include "Common.hpp"
#include "FunctionPrototypes.hpp"

int main(int argc, char * argv[]) {

	if (argc == 1) {
		std::cout << "사용 방법 : " << argv[0] << " <filename>\n";
		return EXIT_FAILURE;
	}

	convert_to_internal_code(argv[1]);
	syntax_check();
	execute();

	return EXIT_SUCCESS;
}