#include "program.h"

#include <fstream>

#include "stl.h"

namespace scisl
{
	inline std::string funcToStr(scislFunc func)
	{
		for (unsigned short i = 0; i < (unsigned short)(stlFuncs::stlFuncCount); i++)
		{
			if (func == stlFuncLUT[i])
			{
				return stlFuncIDs[i];
			}
		}

		return "";
		//@TODO registered funcs
	}

	void program::decompile(const char* filename)
	{
		std::ofstream file(filename);

		if (file.is_open())
		{

			for (unsigned int i = 0; i < instructions.size(); i++)
			{
				auto& cur = instructions[i];
				std::string line = funcToStr(cur.func);

				for (unsigned short j = 0; j < cur.arguments.argCount; j++)
				{
					line += ' ' + cur.arguments.arguments[j].toString();
				}

				file << line;
				if (i != instructions.size() - 1) file << '\n';
			}

			file.close();
		}
	}

	void program::step()
	{
		instructions[curInstr++].run(*this);
	}

	void program::run()
	{
		while (curInstr < instructions.size())
		{
			step();
		}
	}

	void instruction::run(program& process)
	{
		if (func == nullptr) return;
		func(process, arguments);
	}
}