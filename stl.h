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
		adde,
		sub,
		sube,
		mult,
		multe,
		div,
		dive,
		print,

		label,
		jmp,
		cjmp,

		noop,
		stlFuncCount
	};

	const std::string stlFuncIDs[(unsigned short)(stlFuncs::stlFuncCount)] =
	{
		"SET",
		"ADD",
		"ADDE",
		"SUB",
		"SUBE",
		"MULT",
		"MULTE",
		"DIV",
		"DIVE",
		"PRINT",

		"LABEL",
		"JMP",
		"CJMP",

		"NOOP"
	};

	//0 is veriadic arg count, these are only checked at compile time
	constexpr unsigned char stlArgExpect[(unsigned short)(stlFuncs::stlFuncCount)]
	{
		2,
		0,
		0,
		3,
		2,
		0,
		0,
		3,
		2,
		0,

		1,
		1,
		2,

		0
	};

	stlFuncs strToFuncID(const std::string& str);

	//expects 2 args
	void set(program& process, const args& args);

	//veriadic args
	void add(program& process, const args& args);

	//veriadic args
	void adde(program& process, const args& args);

	//3 args
	void sub(program& process, const args& args);

	//2 args
	void sube(program& process, const args& args);

	//veriadic args
	void mult(program& process, const args& args);

	//veriadic args
	void multe(program& process, const args& args);

	//3 args
	void div(program& process, const args& args);

	//2 args
	void dive(program& process, const args& args);

	//veriadic args
	void print(program& process, const args& args);

	void jmp(program& process, const args& args);

	void cjmp(program& process, const args& args);

	constexpr scislFunc stlFuncLUT[(unsigned short)(stlFuncs::stlFuncCount)] =
	{
		set,
		add,
		adde,
		sub,
		sube,
		mult,
		multe,
		div,
		dive,
		print,

		nullptr,
		jmp,
		cjmp,

		nullptr
	};

	bool isSTLfunc(scislFunc fnc);

	void setPeep(instruction& instruct);
	void addPeep(instruction& instruct);
	void addePeep(instruction& instruct);
	void subPeep(instruction& instruct);
	void subePeep(instruction& instruct);
	void multPeep(instruction& instruct);
	void multePeep(instruction& instruct);
	void divPeep(instruction& instruct);
	void divePeep(instruction& instruct);
	void printPeep(instruction& instruct);

	void cjmpPeep(instruction& instruct);

	constexpr scislPeephole stlFuncPeep[(unsigned short)(stlFuncs::stlFuncCount)] =
	{
		setPeep,
		addPeep,
		addePeep,
		subPeep,
		subePeep,
		multPeep,
		multePeep,
		divPeep,
		divePeep,
		printPeep,

		nullptr,
		nullptr,
		cjmpPeep,

		nullptr
	};
}

#endif
