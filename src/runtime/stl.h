#ifndef SCISL_STL_H
#define SCISL_STL_H

#include "../common.h"
#include "stlOptimizations.h"
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
		{ "VAR", "LET", "=" },
		{ "PLUS", "+" },
		{ "PLUSE", "PLUSEQUALS", "ADDEQUALS", "+=", "PLUS_EQUALS" },
		{ "MINUS", "SUBTRACT", "-" },
		{ "MINUSE", "MINUSEQUALS", "SUBEQUALS", "SUBTRACTEQUALS", "-=", "MINUS_EQUALS" },
		{ "TIMES", "MULTIPLY", "*" },
		{ "TIMESE", "TIMESEQUALS", "MULTEQUALS", "MULTIPLYEQUALS", "*=", "TIMES_EQUALS" },
		{ "DIVIDE", "/" },
		{ "DIVEQUALS", "DIVIDEEQUALS", "/=", "DIVIDE_EQUALS" },
		{ "COUT", "PRINTLN", "PRINTLINE", "PRINT_LINE" },
		{ "SUBSTRING", "SUB_STRING" },
		{ "STRINGLEN", "STRLENGTH", "STRINGLENGTH", "LENSTR", "STRING_LENGTH" },
		{ "CHARSET", "SETCHAR", "SETCHR", "SET_CHAR" },
		{ "CHARAT", "CHAR_AT"},
		{ "LESSER", "<", "LESS_THAN" },
		{ "GREATER", ">", "GREATER_THAN" },
		{ "EQUALS", "==", "EQUAL_TO", "SAME?"},
		{ "NEQUALS", "NOTEQUAL", "NOTEQUALS", "!=", "NOT_EQUALS"},
		{ "LOGICALAND", "AND", "&&", "LOGICAL_AND" },
		{ "LOGICALOR", "OR", "||", "LOGICAL_OR" },
		{ "BITWISEAND", "&", "BITWISE_AND" },
		{ "BITWISEOR", "|", "BITWISE_OR" },
		{ "BITWISEXOR", "XOR", "^", "BITWISE_XOR" },
		{ "LEFTSHIFT", "<<", "LEFT_SHIFT" },
		{ "RIGHTSHIFT", ">>", "RIGHT_SHIFT" },
		{ "MARK" },
		{ "JUMP", "GOTO" },
		{ "CJUMP", "CONDITIONALJMP", "CONDITIONALJUMP", "CGOTO", "CONDITIONALGOTO", "CONDITIONAL_JUMP" },
		{ "FINISH", "END" },
		{ "HALT" },
		{ "NOTHING" }
	};

	bool isSTLfunc(stlFuncs fnc);
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

	#define SCISL_OP_NO_MOD 1
	#define SCISL_OP_NO_JMP 2

	//ID, func, peep, args, argTypes, optimizerFlags
	//args 0 is veriadic
	const scislfuncMeta stlFuncMeta[(unsigned short)(stlFuncs::stlFuncCount)] =
	{
		{ (unsigned short)(stlFuncs::set), "SET", set, setPeep, 2, 2, "aa", SCISL_OP_NO_JMP },
		{ (unsigned short)(stlFuncs::add), "ADD", add, addPeep, 0, 2, "a", SCISL_OP_NO_JMP },
		{ (unsigned short)(stlFuncs::adde), "ADDE", adde, addePeep, 0, 2, "a", SCISL_OP_NO_JMP },
		{ (unsigned short)(stlFuncs::sub), "SUB", sub, subPeep, 3, 3, "ann", SCISL_OP_NO_JMP },
		{ (unsigned short)(stlFuncs::sube), "SUBE", sube, subePeep, 2, 2, "nn", SCISL_OP_NO_JMP },
		{ (unsigned short)(stlFuncs::mult), "MULT", mult, multPeep, 0, 2, "an", SCISL_OP_NO_JMP },
		{ (unsigned short)(stlFuncs::multe), "MULTE", multe, multePeep, 0, 2, "an", SCISL_OP_NO_JMP },
		{ (unsigned short)(stlFuncs::div), "DIV", div, divPeep, 3, 3, "ann", SCISL_OP_NO_JMP },
		{ (unsigned short)(stlFuncs::dive), "DIVE", dive, divePeep, 2, 2, "nn", SCISL_OP_NO_JMP },
		{ (unsigned short)(stlFuncs::print), "PRINT", print, printPeep, 0, 0, "a", SCISL_OP_NO_MOD | SCISL_OP_NO_JMP },

		{ (unsigned short)(stlFuncs::substr), "SUBSTR", substr, substrPeep, 4, 4, "ssii", SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::sstrlen), "STRLEN", sstrlen, strlenPeep, 2, 2, "ns", SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::chrset), "CHRSET", chrset, chrsetPeep, 3, 3, "sii", SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::chrat), "CHRAT", chrat, chratPeep, 3, 3, "nsi", SCISL_OP_NO_JMP},

		{ (unsigned short)(stlFuncs::less), "LESS", less, lessPeep, 3, 3, "nnn", SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::great), "GREAT", great, greatPeep, 3, 3, "nnn", SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::equal), "EQUAL", equal, equalPeep, 0, 3, "nnn", SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::nequal), "NEQUAL", nequal, nequalPeep, 3, 3, "nnn", SCISL_OP_NO_JMP},

		{ (unsigned short)(stlFuncs::land), "LAND", land, landPeep, 3, 3, "nnn", SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::lor), "LOR", lor, lorPeep, 3, 3, "nnn", SCISL_OP_NO_JMP},

		{ (unsigned short)(stlFuncs::band), "BAND", band, bandPeep, 2, 2, "ii", SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::bor), "BOR", bor, borPeep, 2, 2, "ii", SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::bxor), "BXOR", bxor, bxorPeep, 2, 2, "ii", SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::lshift), "LSHIFT", lshift, lshiftPeep, 2, 2, "ii", SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::rshift), "RSHIFT", rshift, rshiftPeep, 2, 2, "ii", SCISL_OP_NO_JMP},

		{ (unsigned short)(stlFuncs::label), "LABEL", nullptr, nullptr, 1, 1, "a", SCISL_OP_NO_MOD },
		{ (unsigned short)(stlFuncs::jmp), "JMP", jmp, nullptr, 1, 1, "a", SCISL_OP_NO_MOD  },
		{ (unsigned short)(stlFuncs::cjmp), "CJMP", cjmp, cjmpPeep, 2, 2, "an", SCISL_OP_NO_MOD  },

		{ (unsigned short)(stlFuncs::exit), "EXIT", end, nullptr, 1, 1, "i", SCISL_OP_NO_MOD | SCISL_OP_NO_JMP},
		{ (unsigned short)(stlFuncs::breakp), "BREAK", breakp, nullptr, 1, 1, "i", SCISL_OP_NO_MOD | SCISL_OP_NO_JMP },
		{ (unsigned short)(stlFuncs::noop), "NOOP", nullptr, nullptr, 0, 0, "", SCISL_OP_NO_MOD | SCISL_OP_NO_JMP }
	};

	constexpr type initializes[(unsigned int)(stlFuncs::stlFuncCount)] =
	{
		type::error,
		type::error,
		type::error,
		type::error,
		type::error,
		type::error,
		type::error,
		type::error,
		type::error,
		type::error,

		type::string,
		type::integer,
		type::error,
		type::integer,

		type::integer,
		type::integer,
		type::integer,
		type::integer,

		type::integer,
		type::integer,

		type::integer,
		type::integer,
		type::integer,
		type::integer,
		type::integer,

		type::error,
		type::error,
		type::error,

		type::error,
		type::error,
		type::error,
	};
}

#endif
