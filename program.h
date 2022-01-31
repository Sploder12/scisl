#ifndef SCISL_PROGRAM_H
#define SCISL_PROGRAM_H

#include "common.h"
#include "args.h"

#include <vector>

namespace scisl
{
	struct args
	{
		arg* arguments;
		unsigned char argCount;
	};

	struct instruction;
	struct program
	{
		value* memory;
		std::vector<instruction> instructions;

		void decompile(const char* filename);
	};

	typedef void (*scislFunc)(program&, const args&);

	struct instruction
	{
		scislFunc func;
		args arguments;

		void run(program& process);
	};
}

#endif