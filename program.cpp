#include "program.h"

#include <fstream>
#include <iostream>

#include "stl.h"
#include "tables.h"

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

		auto& t = getFuncTable();
		for (auto& i : t)
		{
			if (i.second.func = func)
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

	void program::run()
	{
		while (curInstr < instructions.size())
		{
			step();
		}
	}

	void program::dumpMemory()
	{
		for (unsigned int i = 0; i < memsize; i++)
		{
			value& cur = memory[i];
			switch (cur.type)
			{
			case type::string:
				std::cout << "Type: String\tValue: " << SCISL_CAST_STRING(cur.val) << '\n';
				break;
			case type::integer:
				std::cout << "Type: Int\tValue: " << SCISL_CAST_INT(cur.val) << '\n';
				break;
			case type::floating:
				std::cout << "Type: Float\tValue: " << SCISL_CAST_FLOAT(cur.val) << '\n';
				break;
			default:
				std::cout << "Type: Error\tValue: " << (unsigned long long)(cur.val) << '\n';
				break;
			}
		}
	}

	program::~program()
	{
		for (unsigned int i = 0; i < memsize; i++)
		{
			memory[i].isTemporary = true;
		}

		delete[] memory;
		for (instruction& i : instructions)
		{
			delete[] i.arguments.arguments;
		}
	}

	void instruction::run(program& process)
	{
		if (func == nullptr) return;
		func(process, arguments);
	}
}