#include "compiler.h"

#include "tables.h"

#include <fstream>

namespace scisl
{
	constexpr bool isNumeric(char chr)
	{
		switch (chr)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return true;
		default:
			return false;
		}
	}

	std::vector<std::string> splitLine(const std::string& line, const char delim = ' ')
	{
		std::vector<std::string> opt;
		std::string tmp = "";
		bool inQuote = false;
		for (char c : line)
		{
			if (c == '"') inQuote = !inQuote;

			if (c == delim && !inQuote)
			{
				opt.push_back(tmp);
				tmp = "";
				continue;
			}
			tmp += c;
		}
		if (tmp != "") opt.push_back(tmp);

		return opt;
	}

	inline size_t findV(std::vector<std::pair<std::string, type>>& vars, std::string& cur)
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

	instruction parseInstr(std::string& line, std::vector<std::pair<std::string, type>>& vars)
	{
		std::vector<std::string> things = splitLine(line);
		unsigned char argCount = things.size() - 1;
		stlFuncs fID = strToFuncID(things[0]);

		instruction opt;

		if (fID != stlFuncs::stlFuncCount)
		{
			opt.func = stlFuncLUT[(unsigned short)(fID)];
			unsigned char expectedArgs = stlArgExpect[(unsigned short)(fID)];
			if (expectedArgs != 0)
			{
				if (expectedArgs != argCount)
				{
					opt.func = nullptr;
					return opt;
					//error
				}
			}

			opt.arguments.arguments = new arg[argCount];
			opt.arguments.argCount = argCount;

			for (unsigned char i = 0; i < argCount; i++)
			{
				std::string& cur = things[i + 1];
				arg* carg = &opt.arguments.arguments[i];

				//constants
				if (isNumeric(cur[0]))
				{
					carg->argType = argType::constant;
					if (cur.find('.') != std::string::npos)
					{
						carg->type = type::floating;
						carg->val = new SCISL_FLOAT_PRECISION(std::stod(cur));
						continue;
					}
					else
					{
						carg->type = type::integer;
						carg->val = new SCISL_INT_PRECISION(std::stol(cur));
						continue;
					}
				}
				else if (cur[0] == '"')
				{
					carg->argType = argType::constant;
					carg->type = type::string;
					carg->val = new std::string(cur.substr(1, cur.size() - 2));
					continue;
				}

				//interoperables
				if (cur[0] == '$')
				{
					auto& vTable = getVarTable();
					carg->argType = argType::interop;
					carg->val = new std::string(cur.substr(2, cur.size() - 3));
					carg->type = vTable.at(*(std::string*)carg->val).type;
					continue;
				}

				//normal variables
				carg->argType = argType::variable;
				unsigned short loc = findV(vars, cur);
				if (loc != vars.size())
				{
					carg->type = vars[loc].second;
					carg->val = new unsigned short(loc);
					continue;
				}

				std::string& next = things[i + 2];
				if (isNumeric(next[0]))
				{
					if (cur.find('.') != std::string::npos)
					{
						carg->type = type::floating;
					}
					else
					{
						carg->type = type::integer;
					}
				}
				else if (next[0] == '"')
				{
					carg->type = type::string;
				}

				carg->val = new unsigned short(loc);
				vars.push_back({ cur, carg->type });
			}

			return opt;
		}

		//@TODO registered functions
		opt.func = nullptr;
		opt.arguments.argCount = 0;
		opt.arguments.arguments = nullptr;
		return opt;
	}

	program* compile(const char* filename)
	{
		std::ifstream file(filename);
		if (file.is_open())
		{
			program* opt = new program();
			opt->memory = {};
			
			std::string line;
			std::vector<std::pair<std::string, type>> vars = {};

			while (std::getline(file, line))
			{
				opt->instructions.push_back(parseInstr(line, vars));
			}

			opt->memory = new value[vars.size()];
			for (unsigned int i = 0; i < vars.size(); i++)
			{
				opt->memory[i].type = vars[i].second;
				switch (vars[i].second)
				{
				case type::integer:
					opt->memory->val = new SCISL_INT_PRECISION(0);
					break;
				case type::floating:
					opt->memory->val = new SCISL_FLOAT_PRECISION(0);
					break;
				case type::string:
					opt->memory->val = new std::string("");
					break;
				}
			}

			file.close();
			return opt;
		}
		return nullptr;
	}
}