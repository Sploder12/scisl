#ifndef SCISL_COMP_OPT_H
#define SCISL_COMP_OPT_H

#include <vector>

#include "../runtime/program.h"

namespace scisl
{
	type inferType(const precompInstr& instr, type nextArgType = type::error);

	bool evaluateConstants(std::vector<precompInstr>& process, std::vector<std::pair<std::string, type>>& vars);

	void removeNOOP(std::vector<precompInstr>& instructions);

	void removeUnusedVars(std::vector<precompInstr>& instructions);

	void removeUnusedLabels(std::vector<precompInstr>& instructions);

	bool removeUnreachableCode(std::vector<precompInstr>& instructions);
}

#endif