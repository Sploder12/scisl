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

	//scisl::program* prog = scisl::compile("src/example.scisl");

	std::string line = "apple,orange,c++,scisl,1337,Flood";
	scisl::registerVar("line", line);

	std::string first = "";
	std::string second = "";
	std::string third = "";
	std::string fourth = "";
	std::string fifth = "";
	std::string last = "";
	scisl::registerVar("first", first);
	scisl::registerVar("second", second);
	scisl::registerVar("third", third);
	scisl::registerVar("fourth", fourth);
	scisl::registerVar("fifth", fifth);
	scisl::registerVar("last", last);
	scisl::program* prog = scisl::compile("Tests/function/ParseStrFuncs.scisl");

	for (unsigned int i = 0; i < 1000000000; i++)
	{
		
		prog->run();
	}

	if (prog == nullptr) return -1;
	//prog->decompile("src/DECOMP_example.scisl");
	
	number = prog->run();

	delete prog;

	return 0;
}