#include "compiler.h"
#include "tables.h"

int main()
{

	int number = 1;
	scisl::registerVar("num", number);

	scisl::program* prog = scisl::compile("example.scisl");
	
	prog->run();

	prog->decompile("decompiled.scisl");
	return 0;
}