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
	struct precompInstr;
	struct program
	{
		value* memory;
		unsigned short memsize;
		std::vector<instruction> instructions;
		unsigned int curInstr = 0;

		void decompile(const char* filename);

		void step();

		void run();

		void dumpMemory();
		
		~program();
	};

	typedef void (*scislFunc)(program&, const args&);
	typedef void (*scislPeephole)(precompInstr&);

	struct instruction
	{
		scislFunc func;
		args arguments;

		void run(program& process);
	};

	struct scislfuncMeta
	{
		std::string funcID;
		scislFunc fnc;
		scislPeephole peep;
		unsigned int expectedArgs;
		std::string argTypes;
		unsigned int optimizerFlags;
	};

	struct precompInstr
	{
		instruction instr;
		scislfuncMeta meta;
	};
}

#endif