#ifndef SCISL_STL_OPT_H
#define SCISL_STL_OPT_H

#include "../common.h"
#include "program.h"

namespace scisl
{
	#define SCISL_OP_NO_MOD 1
	#define SCISL_OP_NO_JMP 2
	#define SCISL_OP_INITIALIZES 4
	#define SCISL_OP_SIMABLE 8
	#define SCISL_OP_BLOCK 16

	inline void toNOOP(precompInstr& instruct);

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
}
#endif