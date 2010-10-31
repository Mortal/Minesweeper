#include "Vector.hpp"
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
	++argv; --argc;
	Coordinate args[argc];
	for (int i = 0; i < argc; ++i) {
		std::stringstream s(argv[i]);
		s >> args[i];
	}
#define TRY(L) \
	if (L == argc) {\
		CoordinateSet<L>::Type coord(args);\
		for (int i = 0; i < argc; ++i) {\
			std::cout << coord[i] << std::endl;\
		}\
		return 0;\
	}
	TRY(2); TRY(3); TRY(4);
	return 1;
}
