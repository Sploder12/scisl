#ifndef SCISL_COMP_OPT_H
#define SCISL_COMP_OPT_H

#include <vector>

#include "../runtime/program.h"

namespace scisl
{
	template <typename T>
	inline size_t findV(std::vector<std::pair<std::string, T>>& vars, std::string& cur)
	{
		for (unsigned short i = 0; i < vars.size(); i++)
		{
			if (vars[i].first == cur)
			{
				return i;
			}
		}
		return vars.size();
	}

	inline unsigned int findBlockEnd(std::vector<precompInstr>& instructions, unsigned int start);

	type inferType(const precompInstr& instr, type nextArgType = type::error);

	bool evaluateConstants(std::vector<precompInstr>& process, std::vector<std::pair<std::string, type>>& vars);

	void removeNOOP(std::vector<precompInstr>& instructions);

	void removeUnusedVars(std::vector<precompInstr>& instructions);

	bool removeUnusedLabels(std::vector<precompInstr>& instructions);

	bool removeUnreachableCode(std::vector<precompInstr>& instructions);
}

#endif