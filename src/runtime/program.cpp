#include "program.h"

#include <fstream>
#include <iostream>

#include "stl.h"
#include "../interoperability/tables.h"

namespace scisl
{
	inline std::string funcToStr(scislFunc func)
	{
		for (unsigned short i = 0; i < (unsigned short)(stlFuncs::stlFuncCount); ++i)
		{
			if (func == stlFuncMeta[i].fnc)
			{
				return stlFuncMeta[i].funcName;
			}
		}

		auto& t = getFuncTable();
		for (auto& [id, fnc] : t)
		{
			if (fnc.fnc = func)
			{
				return '$' + id;
			}
		}

		return "";
	}

	void program::decompile(const char* filename)
	{
		std::ofstream file(filename);

		if (file.is_open())
		{

			for (unsigned int i = 0; i < instructions.size(); ++i)
			{
				auto& cur = instructions[i];
				std::string&& line = funcToStr(cur.func);

				for (unsigned short j = 0; j < cur.argCount; j++)
				{
					line += ' ' + cur.arguments[j].toString();
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

	SCISL_INT_PRECISION program::run(bool ignoreBreaks)
	{
		size_t size = instructions.size();
		if (curInstr >= size)
		{
			curInstr = 0;
		}

		broke = false;
		if (ignoreBreaks)
		{
			while (curInstr < size)
			{
				step();
			}
			broke = false;
		}
		else 
		{
			while (curInstr < size && !broke)
			{
				step();
			}
		}
		return retVal;
	}

	program::~program()
	{
		std::vector<void*> varz;
		varz.reserve(instructions.size()); //We're going to assume worst case which is one init per instructions

		for (instruction& i : instructions)
		{
			for (arg& cur : i) //we need to make sure we're not deleting vars twice
			{
				if (cur.argType == argType::variable)
				{
					if (std::find(varz.begin(), varz.end(), cur.val.val) == varz.end())
					{
						varz.emplace_back(cur.val.val);
						cur.argType = argType::constant; //let the delete[] handle it
					}
				}
			}

			delete[] i.arguments;
		}
	}
}