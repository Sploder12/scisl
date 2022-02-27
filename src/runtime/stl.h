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

		label, //jumps
		jmp,
		cjmp,
		blockend,

		def, //fnc
		call,

		exit, //ctrl
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
		{ "BLOCK_END", "CLOSEBLOCK", "CLOSE_BLOCK", "ENDBLOCK", "END_BLOCK", "EB", "[]}" },
		{ "DEFINE", "FNC", "FUNC", "FUNCTION"},
		{ "RUN" },
		{ "FINISH", "END" },
		{ "HALT" },
		{ "NOTHING" }
	};

	bool isSTLfunc(stlFuncs fnc);
	bool isFunc(scislFunc func, stlFuncs fnc);
	stlFuncs strToFuncID(const std::string& str);


	void set(program& process, const instruction& instr);
	void add(program& process, const instruction& instr);
	void adde(program& process, const instruction& instr);
	void sub(program& process, const instruction& instr);
	void sube(program& process, const instruction& instr);
	void mult(program& process, const instruction& instr);
	void multe(program& process, const instruction& instr);
	void div(program& process, const instruction& instr);
	void dive(program& process, const instruction& instr);
	void print(program& process, const instruction& instr);

	void substr(program& process, const instruction& instr);
	void sstrlen(program& process, const instruction& instr);
	void chrset(program& process, const instruction& instr);
	void chrat(program& process, const instruction& instr);

	void less(program& process, const instruction& instr);
	void great(program& process, const instruction& instr);
	void equal(program& process, const instruction& instr);
	void nequal(program& process, const instruction& instr);

	void land(program& process, const instruction& instr);
	void lor(program& process, const instruction& instr);

	void band(program& process, const instruction& instr);
	void bor(program& process, const instruction& instr);
	void bxor(program& process, const instruction& instr);
	void lshift(program& process, const instruction& instr);
	void rshift(program& process, const instruction& instr);

	void jmp(program& process, const instruction& instr);
	void cjmp(program& process, const instruction& instr);
	void blockend(program& process, const instruction& instr);

	void def(program& process, const instruction& instr);
	void call(program& process, const instruction& instr);

	void end(program& process, const instruction& instr);
	void breakp(program& process, const instruction& instr);

	//ID, func, peep, args, argTypes, flags
	//args 0 is veriadic
	const scislfuncMeta stlFuncMeta[(unsigned short)(stlFuncs::stlFuncCount)] =
	{
		{ stlFuncs::set, "SET", set, setPeep, 2, 2, "aa", type::error, SCISL_F_NJIS },
		{ stlFuncs::add, "ADD", add, addPeep, 0, 2, "a", type::error, SCISL_F_NJIS },
		{ stlFuncs::adde, "ADDE", adde, addePeep, 0, 2, "a", type::error, SCISL_F_NO_JMP | SCISL_F_SIMABLE },
		{ stlFuncs::sub, "SUB", sub, subPeep, 3, 3, "ann", type::error, SCISL_F_NJIS },
		{ stlFuncs::sube, "SUBE", sube, subePeep, 2, 2, "nn", type::error, SCISL_F_NO_JMP | SCISL_F_SIMABLE },
		{ stlFuncs::mult, "MULT", mult, multPeep, 0, 2, "an", type::error, SCISL_F_NJIS },
		{ stlFuncs::multe, "MULTE", multe, multePeep, 0, 2, "an", type::error, SCISL_F_NO_JMP | SCISL_F_SIMABLE },
		{ stlFuncs::div, "DIV", div, divPeep, 3, 3, "ann", type::error, SCISL_F_NJIS },
		{ stlFuncs::dive, "DIVE", dive, divePeep, 2, 2, "nn", type::error, SCISL_F_NO_JMP | SCISL_F_SIMABLE },
		{ stlFuncs::print, "PRINT", print, printPeep, 0, 0, "a", type::error, SCISL_F_NMNJ },

		{ stlFuncs::substr, "SUBSTR", substr, substrPeep, 4, 4, "ssii", type::string, SCISL_F_NJIS },
		{ stlFuncs::sstrlen, "STRLEN", sstrlen, strlenPeep, 2, 2, "ns", type::integer, SCISL_F_NJIS },
		{ stlFuncs::chrset, "CHRSET", chrset, chrsetPeep, 3, 3, "sii", type::error, SCISL_F_NO_JMP | SCISL_F_SIMABLE },
		{ stlFuncs::chrat, "CHRAT", chrat, chratPeep, 3, 3, "nsi", type::integer, SCISL_F_NJIS },

		{ stlFuncs::less, "LESS", less, lessPeep, 3, 3, "nnn", type::integer, SCISL_F_NJIS },
		{ stlFuncs::great, "GREAT", great, greatPeep, 3, 3, "nnn", type::integer, SCISL_F_NJIS },
		{ stlFuncs::equal, "EQUAL", equal, equalPeep, 0, 3, "nnn", type::integer, SCISL_F_NJIS },
		{ stlFuncs::nequal, "NEQUAL", nequal, nequalPeep, 3, 3, "nnn", type::integer, SCISL_F_NJIS },

		{ stlFuncs::land, "LAND", land, landPeep, 3, 3, "nnn", type::integer, SCISL_F_NJIS },
		{ stlFuncs::lor, "LOR", lor, lorPeep, 3, 3, "nnn", type::integer, SCISL_F_NJIS },

		{ stlFuncs::band, "BAND", band, bandPeep, 2, 2, "ii", type::error, SCISL_F_NO_JMP | SCISL_F_SIMABLE },
		{ stlFuncs::bor, "BOR", bor, borPeep, 2, 2, "ii", type::error, SCISL_F_NO_JMP | SCISL_F_SIMABLE },
		{ stlFuncs::bxor, "BXOR", bxor, bxorPeep, 2, 2, "ii", type::error, SCISL_F_NO_JMP | SCISL_F_SIMABLE },
		{ stlFuncs::lshift, "LSHIFT", lshift, lshiftPeep, 2, 2, "ii", type::error, SCISL_F_NO_JMP | SCISL_F_SIMABLE },
		{ stlFuncs::rshift, "RSHIFT", rshift, rshiftPeep, 2, 2, "ii", type::error, SCISL_F_NO_JMP | SCISL_F_SIMABLE },

		{ stlFuncs::label, "LABEL", nullptr, nullptr, 1, 1, "a", type::error, SCISL_F_NO_MOD },
		{ stlFuncs::jmp, "JMP", jmp, nullptr, 1, 1, "a", type::error, SCISL_F_NO_MOD  },
		{ stlFuncs::cjmp, "CJMP", cjmp, cjmpPeep, 2, 2, "an", type::error, SCISL_F_NO_MOD  },
		{ stlFuncs::blockend, "BLOCKEND", blockend, nullptr, 0, 0, "", type::error, SCISL_F_NO_MOD  },

		{ stlFuncs::def, "DEF", def, nullptr, 1, 1, "a", type::error, SCISL_F_NO_MOD | SCISL_F_BLOCK },
		{ stlFuncs::call, "CALL", call, nullptr, 1, 1, "a", type::error, SCISL_F_NO_MOD },

		{ stlFuncs::exit, "EXIT", end, nullptr, 1, 1, "i", type::error, SCISL_F_NMNJ },
		{ stlFuncs::breakp, "BREAK", breakp, nullptr, 1, 1, "i", type::error, SCISL_F_NMNJ },
		{ stlFuncs::noop, "NOOP", nullptr, nullptr, 0, 0, "", type::error, SCISL_F_NMNJ }
	};
}
#endif