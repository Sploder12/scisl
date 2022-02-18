#include "program.h"

#include <fstream>
#include <iostream>

#include "stl.h"
#include "../interoperability/tables.h"

namespace scisl
{
	inline std::string funcToStr(scislFunc func)
	{
		for (unsigned short i = 0; i < (unsigned short)(stlFuncs::stlFuncCount); i++)
		{
			if (func == stlFuncMeta[i].fnc)
			{
				return stlFuncMeta[i].funcID;
			}
		}

		auto& t = getFuncTable();
		for (auto& i : t)
		{
			if (i.second.fnc = func)
			{
				return i.first;
			}
		}

		return "";
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

	SCISL_INT_PRECISION program::run(bool ignoreBreaks)
	{
		if (curInstr >= instructions.size())
		{
			curInstr = 0;
		}

		broke = false;
		if (ignoreBreaks)
		{
			while (curInstr < instructions.size())
			{
				step();
			}
			broke = false;
		}
		else 
		{
			while (curInstr < instructions.size() && !broke)
			{
				step();
			}
		}
		return retVal;
	}

	program::~program()
	{
		std::map<void*, type> varz;
		for (instruction& i : instructions)
		{
			for (unsigned int j = 0; j < i.arguments.argCount; j++)
			{
				arg& cur = i.arguments.arguments[j];
				if (cur.argType == argType::variable)
				{
					if (!varz.contains(cur.val.val))
					{
						varz.insert({ cur.val.val, cur.val.type});
					}
					cur.val.val = nullptr;
					cur.val.type = type::error;
					continue;
				}
			}

			delete[] i.arguments.arguments;
		}

		for (auto& j : varz)
		{

			switch (j.second)
			{
			case type::string:
				delete (std::string*)(j.first);
				break;
			case type::integer:
				delete (SCISL_INT_PRECISION*)(j.first);
				break;
			case type::floating:
				delete (SCISL_FLOAT_PRECISION*)(j.first);
				break;
			default:
				break;
			}

		}
	}
}