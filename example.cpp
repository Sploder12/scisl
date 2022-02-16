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
	std::string name = "Bob";
	scisl::registerVar("ipt", number);
	scisl::defineMacro("name", '"' + name + '"');

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

	scisl::program* prog = scisl::compile("Tests/ParseStr.scisl");

	if (prog == nullptr) return -1;
	//prog->decompile("examples/DECOMP_strVal.scisl");
	
	number = prog->run();
	
	std::cout << "Factorial of " << FACT << " is " << number;

	delete prog;

	return 0;
}