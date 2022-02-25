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

	enum class stlFuncs : unsigned short;
	inline unsigned int findLabel(std::vector<precompInstr>& instructions, std::string id, stlFuncs lblFnc)
	{
		for (unsigned int i = 0; i < instructions.size(); i++)
		{
			precompInstr& cur = instructions[i];
			if (cur.meta.funcID == lblFnc)
			{
				std::string& v = SCISL_CAST_STRING(cur.instr.arguments[0].val.val);
				if (v == id)
				{
					return i;
				}
			}
		}
		return (unsigned int)(instructions.size());
	}

	inline unsigned int findBlockEnd(std::vector<precompInstr>& instructions, unsigned int start);

	type inferType(const precompInstr& instr, type nextArgType = type::error);

	void evaluateConstants(std::vector<precompInstr>& process, std::vector<std::pair<std::string, type>>& vars);

	void removeNOOP(std::vector<precompInstr>& instructions);

	void removeUnusedVars(std::vector<precompInstr>& instructions);

	void removeUnusedLabels(std::vector<precompInstr>& instructions);

	void removeUnreachableCode(std::vector<precompInstr>& instructions);
}

#endif