#include "compiler.h"
#include "tables.h"

#include <iostream>

void exampleFunc(scisl::program& process, const scisl::args& arguments)
{
	scisl::value cur = arguments.arguments[0].getValue(process);
	if (SCISL_CAST_INT(cur.val) % 2 == 0)
	{
		std::cout << "Divisible by two!\n";
	}
	else
	{
		std::cout << "Not divisible by two\n";
	}
}

int main()
{

	int number = 1;
	scisl::registerVar("num", number);
	scisl::registerFunc("ExampleFunc", exampleFunc, 1);

	scisl::program* prog = scisl::compile("example.scisl");

	prog = scisl::compile("example.scisl");
	

	prog->decompile("decompiled.scisl");
	prog->run();

	prog->dumpMemory();

	
	return 0;
}