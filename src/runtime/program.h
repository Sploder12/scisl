#ifndef SCISL_PROGRAM_H
#define SCISL_PROGRAM_H

#include "../common.h"
#include "instruction.h"
#include "args.h"

#include <vector>

namespace scisl
{
	struct program
	{
		std::vector<instruction> instructions;
		unsigned int curInstr = 0;
		SCISL_INT_PRECISION retVal = 0;
		bool broke = false;

		void decompile(const char* filename);

		void step();

		SCISL_INT_PRECISION run(bool ignoreBreaks = false);

		~program();
	};
}

#endif