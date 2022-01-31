#include "compiler.h"
#include "tables.h"

int main()
{

	int number = 1;
	scisl::registerVar("num", number);

	scisl::program* prog = scisl::compile("example.scisl");
	for (unsigned int i = 0; i < prog->instructions.size(); i++)
	{
		prog->instructions[i].run(*prog);
	}
	prog->decompile("decompiled.scisl");
	return 0;
}