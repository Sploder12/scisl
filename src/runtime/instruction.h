#ifndef SCISL_INSTR_H
#define SCISL_INSTR_H

#include <string>

namespace scisl
{
	struct program;
	struct instruction;
	typedef void (*scislFunc)(program&, const instruction&);

	struct arg;
	struct instruction
	{
		scislFunc func;
		arg* arguments;
		unsigned char argCount;

		void run(program& process);
	};

	struct precompInstr;
	typedef void (*scislPeephole)(precompInstr&);
	enum class stlFuncs : unsigned short;
	enum class type : unsigned char;
	struct scislfuncMeta
	{
		stlFuncs funcID;
		std::string funcName;
		scislFunc fnc;
		scislPeephole peep;
		unsigned int expectedArgs;
		unsigned int minArgs;
		std::string argTypes;
		type initializes;
		unsigned int flags;
	};

	struct precompInstr
	{
		instruction instr;
		scislfuncMeta meta;
	};


}

#endif