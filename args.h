#ifndef SCISL_ARGS_H
#define SCISL_ARGS_H

#include "common.h"

namespace scisl
{
	enum class argType : unsigned char
	{
		constant,
		variable,
		interop,

		error
	};

	struct program;

	struct arg
	{
		argType argType;
		type type;
		void* val;
		//val is either a pointer to a constant OR a pointer to string name of var
		//variables are resolved at compile time, interops at runtime
		//variables are resolved to a program memory location as unsigned short

		arg() :
			argType(argType::error), type(type::error), val(nullptr) {}

		//args only move around before/during the compilation, never during runtime
		arg(arg& other);
		arg(arg&& cmove) noexcept;

		arg& operator=(arg& other);
		arg& operator=(arg&& cmove) noexcept;

		value getValue(program& process);

		std::string toString();

		~arg();
	};
}

#endif