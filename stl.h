#ifndef SCISL_STL_H
#define SCISL_STL_H

#include "common.h"
#include "program.h"

namespace scisl
{
	enum class stlFuncs : unsigned short
	{
		set,
		add,
		sub,
		mult,
		div,
		print,

		noop,
		stlFuncCount
	};

	const std::string stlFuncIDs[(unsigned short)(stlFuncs::stlFuncCount)] =
	{
		"SET",
		"ADD",
		"SUB",
		"MULT",
		"DIV",
		"PRINT",


		"NOOP"
	};

	//0 is veriadic arg count, these are only checked at compile time
	constexpr unsigned char stlArgExpect[(unsigned short)(stlFuncs::stlFuncCount)]
	{
		2,
		0,
		3,
		0,
		3,
		0,

		0
	};

	stlFuncs strToFuncID(const std::string& str);

	//expects 2 args
	void set(program& process, const args& args);

	//veriadic args
	void add(program& process, const args& args);

	//3 args
	void sub(program& process, const args& args);

	//veriadic args
	void mult(program& process, const args& args);

	//3 args
	void div(program& process, const args& args);

	//veriadic args
	void print(program& process, const args& args);

	constexpr scislFunc stlFuncLUT[(unsigned short)(stlFuncs::stlFuncCount)] =
	{
		set,
		add,
		sub,
		mult,
		div,
		print,

		nullptr
	};
}

#endif
