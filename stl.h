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

	bool isSTLfunc(scislFunc fnc);
	bool isFunc(scislFunc func, stlFuncs fnc);
	bool isFunc(scislfuncMeta meta, stlFuncs fnc);
	stlFuncs strToFuncID(const std::string& str);


	void set(program& process, const args& args);
	void add(program& process, const args& args);
	void adde(program& process, const args& args);
	void sub(program& process, const args& args);
	void sube(program& process, const args& args);
	void mult(program& process, const args& args);
	void multe(program& process, const args& args);
	void div(program& process, const args& args);
	void dive(program& process, const args& args);

	void print(program& process, const args& args);
	void jmp(program& process, const args& args);
	void cjmp(program& process, const args& args);


	void setPeep(precompInstr& instruct);
	void addPeep(precompInstr& instruct);
	void addePeep(precompInstr& instruct);
	void subPeep(precompInstr& instruct);
	void subePeep(precompInstr& instruct);
	void multPeep(precompInstr& instruct);
	void multePeep(precompInstr& instruct);
	void divPeep(precompInstr& instruct);
	void divePeep(precompInstr& instruct);
	void printPeep(precompInstr& instruct);

	void cjmpPeep(precompInstr& instruct);

	#define SCISL_OP_NO_MOD 1
	#define SCISL_OP_NO_JMP 2

	//ID, func, peep, args, argTypes, optimizerFlags
	//args 0 is veriadic
	const scislfuncMeta stlFuncMeta[(unsigned short)(stlFuncs::stlFuncCount)] =
	{
		{ "SET", set, setPeep, 2, "aa", SCISL_OP_NO_JMP },
		{ "ADD", add, addPeep, 0, "a", SCISL_OP_NO_JMP },
		{ "ADDE", adde, addePeep, 0, "a", SCISL_OP_NO_JMP },
		{ "SUB", sub, subPeep, 3, "ann", SCISL_OP_NO_JMP },
		{ "SUBE", sube, subePeep, 2, "nn", SCISL_OP_NO_JMP },
		{ "MULT", mult, multPeep, 0, "an", SCISL_OP_NO_JMP },
		{ "MULTE", multe, multePeep, 0, "an", SCISL_OP_NO_JMP },
		{ "DIV", div, divPeep, 3, "ann", SCISL_OP_NO_JMP },
		{ "DIVE", dive, divePeep, 2, "nn", SCISL_OP_NO_JMP },
		{ "PRINT", print, printPeep, 0, "a", SCISL_OP_NO_MOD | SCISL_OP_NO_JMP },

		{ "LABEL", nullptr, nullptr, 1, "a", SCISL_OP_NO_MOD },
		{ "JMP", jmp, nullptr, 1, "a", SCISL_OP_NO_MOD  },
		{ "CJMP", cjmp, cjmpPeep, 2, "an", SCISL_OP_NO_MOD  },

		{ "NOOP", nullptr, nullptr, 0, "", SCISL_OP_NO_MOD | SCISL_OP_NO_JMP },
	};
}

#endif
