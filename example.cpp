#include "compiler.h"
#include "tables.h"

#include <iostream>

#define FACT 12
int main()
{

	int number = FACT;
	scisl::registerVar("ipt", number);

	scisl::program* prog = scisl::compile("examples/factorial.scisl");

	if (prog == nullptr) return -1;
	prog->decompile("examples/DECOMP_factorial.scisl");
	prog->run();

	std::cout << "Factorial of " << FACT << " is " << number;

	delete prog;
	return 0;
}