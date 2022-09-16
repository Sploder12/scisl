#ifndef SCISL_ARGS_H
#define SCISL_ARGS_H

#include "../common.h"

namespace scisl
{
	enum class argType : unsigned char
	{
		constant,
		variable,
		interop,

		error
	};

	struct arg
	{
		argType argType;
		value val;
		bool finalized = false;
		//val is either a pointer to a constant OR a pointer to string name of var
		//variables are resolved at compile time, interops at runtime
		//variables are resolved to a program memory location as unsigned short

		arg() :
			argType(argType::error) {}

		//args only move around before/during the compilation, never during runtime
		arg(arg& other);
		arg(arg&& cmove) noexcept;

		arg& operator=(arg& other);
		arg& operator=(arg&& cmove) noexcept;

		arg& operator=(std::string& v);

		value& getValue();

		std::string toString() const;

		~arg();
	};
}
#endif