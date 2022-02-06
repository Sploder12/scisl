#include "compiler.h"
#include "tables.h"

#include <iostream>

#include <chrono>
#define TIME(funntions)\
	auto s = std::chrono::system_clock::now();\
	funntions\
	auto e = std::chrono::system_clock::now();\
	std::cout << "Took " << (e - s).count() / 10000000.0 << "s \n"\

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