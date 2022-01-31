#include "compiler.h"

int main()
{
	scisl::program* prog = scisl::compile("example.scisl");
	for (unsigned int i = 0; i < prog->instructions.size(); i++)
	{
		prog->instructions[i].run(*prog);
	}
	prog->decompile("decompiled.scisl");
	return 0;
}