#include "compiler/compiler.h"
#include "interoperability/tables.h"

#include <iostream>

#include <chrono>
#define TIME(funntions)\
	auto s = std::chrono::system_clock::now();\
	funntions\
	auto e = std::chrono::system_clock::now();\
	std::cout << "Took " << (e - s).count() / 10000000.0 << "s \n"\

int main()
{
	
	int number = 5;
	scisl::registerVar("num", number);

	scisl::program* prog = scisl::compile("src/example.scisl");

	if (prog == nullptr) return -1;
	prog->decompile("src/DECOMP_example.scisl");
	
	number = prog->run();

	delete prog;

	return 0;
}