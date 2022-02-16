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

		substr,
		sstrlen, //strings can be used as vectors
		chrset,
		chrat,

		less,
		great,
		equal,
		nequal,

		land, //logical
		lor,

		band, //bitwise
		bor,
		bxor,
		lshift,
		rshift,

		label,
		jmp,
		cjmp,

		exit,
		breakp,
		noop,
		stlFuncCount
	};

	const std::vector<std::string> funcAliases[(unsigned short)(stlFuncs::stlFuncCount)] =
	{
		{ "=" },
		{ "+" },
		{ "ADDEQUALS", "+=" },
		{ "SUBTRACT", "-" },
		{ "SUBEQUALS", "SUBTRACTEQUALS", "-=" },
		{ "MULTIPLY", "*" },
		{ "MULTEQUALS", "MULTIPLYEQUALS", "*=" },
		{ "DIVIDE", "/" },
		{ "DIVEQUALS", "DIVIDEEQUALS", "/=" },
		{ "COUT", "PRINTLN" },
		{ "SUBSTRING" },
		{ "STRINGLEN", "STRLENGTH", "STRINGLENGTH", "LENSTR" },
		{ "CHARSET" },
		{ "CHARAT" },
		{ "LESSER", "<" },
		{ "GREATER", ">" },
		{ "EQUALS", "==",},
		{ "NEQUALS", "NOTEQUAL", "NOTEQUALS", "!=" },
		{ "LOGICALAND", "AND", "&&" },
		{ "LOGICALOR", "OR", "||" },
		{ "BITWISEAND", "&" },
		{ "BITWISEOR", "|" },
		{ "BITWISEXOR", "XOR", "^" },
		{ "LEFTSHIFT", "<<" },
		{ "RIGHTSHIFT", ">>" },
		{ "MARK" },
		{ "JUMP", "GOTO" },
		{ "CJUMP", "CONDITIONALJMP", "CONDITIONALJUMP", "CGOTO", "CONDITIONALGOTO"},
		{ "FINISH" },
		{ "HALT" },
		{ "NOTHING" }
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

	void substr(program& process, const args& args);
	void sstrlen(program& process, const args& args);
	void chrset(program& process, const args& args);
	void chrat(program& process, const args& args);

	void less(program& process, const args& args);
	void great(program& process, const args& args);
	void equal(program& process, const args& args);
	void nequal(program& process, const args& args);

	void land(program& process, const args& args);
	void lor(program& process, const args& args);

	void band(program& process, const args& args);
	void bor(program& process, const args& args);
	void bxor(program& process, const args& args);
	void lshift(program& process, const args& args);
	void rshift(program& process, const args& args);

	void jmp(program& process, const args& args);
	void cjmp(program& process, const args& args);

	void end(program& process, const args& args);
	void breakp(program& process, const args& args);

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

	void substrPeep(precompInstr& instruct);
	void strlenPeep(precompInstr& instruct);
	void chrsetPeep(precompInstr& instruct);
	void chratPeep(precompInstr& instruct);

	void lessPeep(precompInstr& instruct);
	void greatPeep(precompInstr& instruct);
	void equalPeep(precompInstr& instruct);
	void nequalPeep(precompInstr& instruct);

	void landPeep(precompInstr& instruct);
	void lorPeep(precompInstr& instruct);

	void bandPeep(precompInstr& instruct);
	void borPeep(precompInstr& instruct);
	void bxorPeep(precompInstr& instruct);
	void lshiftPeep(precompInstr& instruct);
	void rshiftPeep(precompInstr& instruct);

	void cjmpPeep(precompInstr& instruct);

	#define SCISL_OP_NO_MOD 1
	#define SCISL_OP_NO_JMP 2

	//ID, func, peep, args, argTypes, optimizerFlags
	//args 0 is veriadic
	const scislfuncMeta stlFuncMeta[(unsigned short)(stlFuncs::stlFuncCount)] =
	{
		{ "SET", set, setPeep, 2, 2, "aa", SCISL_OP_NO_JMP },
		{ "ADD", add, addPeep, 0, 2, "a", SCISL_OP_NO_JMP },
		{ "ADDE", adde, addePeep, 0, 2, "a", SCISL_OP_NO_JMP },
		{ "SUB", sub, subPeep, 3, 3, "ann", SCISL_OP_NO_JMP },
		{ "SUBE", sube, subePeep, 2, 2, "nn", SCISL_OP_NO_JMP },
		{ "MULT", mult, multPeep, 0, 2, "an", SCISL_OP_NO_JMP },
		{ "MULTE", multe, multePeep, 0, 2, "an", SCISL_OP_NO_JMP },
		{ "DIV", div, divPeep, 3, 3, "ann", SCISL_OP_NO_JMP },
		{ "DIVE", dive, divePeep, 2, 2, "nn", SCISL_OP_NO_JMP },
		{ "PRINT", print, printPeep, 0, 0, "a", SCISL_OP_NO_MOD | SCISL_OP_NO_JMP },

		{ "SUBSTR", substr, substrPeep, 4, 4, "ssii", SCISL_OP_NO_JMP},
		{ "STRLEN", sstrlen, strlenPeep, 2, 2, "ns", SCISL_OP_NO_JMP},
		{ "CHRSET", chrset, chrsetPeep, 3, 3, "sii", SCISL_OP_NO_JMP},
		{ "CHRAT", chrat, chratPeep, 3, 3, "nsi", SCISL_OP_NO_JMP},

		{ "LESS", less, lessPeep, 3, 3, "nnn", SCISL_OP_NO_JMP},
		{ "GREAT", great, greatPeep, 3, 3, "nnn", SCISL_OP_NO_JMP},
		{ "EQUAL", equal, equalPeep, 0, 3, "nnn", SCISL_OP_NO_JMP},
		{ "NEQUAL", nequal, nequalPeep, 3, 3, "nnn", SCISL_OP_NO_JMP},

		{ "LAND", land, landPeep, 3, 3, "nnn", SCISL_OP_NO_JMP},
		{ "LOR", lor, lorPeep, 3, 3, "nnn", SCISL_OP_NO_JMP},

		{ "BAND", band, bandPeep, 2, 2, "ii", SCISL_OP_NO_JMP},
		{ "BOR", bor, borPeep, 2, 2, "ii", SCISL_OP_NO_JMP},
		{ "BXOR", bxor, bxorPeep, 2, 2, "ii", SCISL_OP_NO_JMP},
		{ "LSHIFT", lshift, lshiftPeep, 2, 2, "ii", SCISL_OP_NO_JMP},
		{ "RSHIFT", rshift, rshiftPeep, 2, 2, "ii", SCISL_OP_NO_JMP},

		{ "LABEL", nullptr, nullptr, 1, 1, "a", SCISL_OP_NO_MOD },
		{ "JMP", jmp, nullptr, 1, 1, "a", SCISL_OP_NO_MOD  },
		{ "CJMP", cjmp, cjmpPeep, 2, 2, "an", SCISL_OP_NO_MOD  },

		{ "EXIT", end, nullptr, 1, 1, "i", SCISL_OP_NO_MOD | SCISL_OP_NO_JMP},
		{ "BREAK", breakp, nullptr, 1, 1, "i", SCISL_OP_NO_MOD | SCISL_OP_NO_JMP },
		{ "NOOP", nullptr, nullptr, 0, 0, "", SCISL_OP_NO_MOD | SCISL_OP_NO_JMP }
	};
}

#endif
