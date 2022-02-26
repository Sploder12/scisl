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

			for (unsigned int i = 0; i < instructions.size(); i++)
			{
				auto& cur = instructions[i];
				std::string line = funcToStr(cur.func);

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
			for (unsigned int j = 0; j < i.argCount; j++)
			{
				arg& cur = i.arguments[j];
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

			delete[] i.arguments;
		}

		for (auto& [val, type] : varz)
		{
			switch (type)
			{
			case type::string:
				delete (std::string*)(val);
				break;
			case type::integer:
				delete (SCISL_INT_PRECISION*)(val);
				break;
			case type::floating:
				delete (SCISL_FLOAT_PRECISION*)(val);
				break;
			default:
				break;
			}
		}
	}
}