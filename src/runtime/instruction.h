#ifndef SCISL_INSTR_H
#define SCISL_INSTR_H

#include "args.h"

#include <string>

namespace scisl
{
	struct program;
	struct instruction;
	typedef void (*scislFunc)(program&, const instruction&);

	struct instruction
	{
		scislFunc func;
		arg* arguments;
		unsigned char argCount;

		void run(program& process);

		arg* begin() noexcept
		{
			return arguments;
		};

		arg* end() noexcept
		{
			return arguments + argCount;
		}
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

		arg* begin() noexcept
		{
			return instr.begin();
		};

		arg* end() noexcept
		{
			return instr.end();
		}
	};


}

#endif