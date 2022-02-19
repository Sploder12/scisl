#ifndef SCISL_INSTR_H
#define SCISL_INSTR_H

#include <string>

namespace scisl
{
	struct arg;
	struct program;

	struct args
	{
		arg* arguments;
		unsigned char argCount;
	};

	typedef void (*scislFunc)(program&, const args&);

	struct instruction
	{
		scislFunc func;
		args arguments;

		void run(program& process);
	};

	struct precompInstr;
	typedef void (*scislPeephole)(precompInstr&);
	struct scislfuncMeta
	{
		unsigned short funcID;
		std::string funcName;
		scislFunc fnc;
		scislPeephole peep;
		unsigned int expectedArgs;
		unsigned int minArgs;
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