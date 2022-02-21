#include "compiler.h"

#include "../common.h"
#include "../interoperability/tables.h"
#include "../runtime/stl.h"

#include "compilerOptimizations.h"

#include <fstream>
#include <iostream>

namespace scisl
{
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

	constexpr bool isRightType(const char c, type tipe)
	{
		switch (c)
		{
		case 'a':
			return true;
		case 'n':
			return (tipe == type::integer || tipe == type::floating);
		case 's':
			return tipe == type::string;
		case 'i':
			return tipe == type::integer;
		case 'f':
			return tipe == type::floating;
		default:
			return false;
		}
	}

	constexpr const char* typeCharToStr(const char c)
	{
		switch (c)
		{
		case 'a':
			return "Any";
		case 'n':
			return "Numeric";
		case 's':
			return "String";
		case 'i':
			return "Integer";
		case 'f':
			return "Float";
		default:
			return "error";
		}
	}

	inline void toUpper(std::string& str)
	{
		for (char& c : str)
		{
			if (c >= 'a' && c <= 'z')
			{
				c -= ('a' - 'A');
			}
		}
	}

	size_t lineNum = 0;
	bool typeCheck(precompInstr& bi)
	{
		const std::string& tstr = bi.meta.argTypes;
		if (tstr == "") return true;

		char curType = 'a';
		for (unsigned int i = 0; i < bi.instr.arguments.argCount; i++)
		{
			if (i < tstr.size()) curType = tstr[i];

			type atype = bi.instr.arguments.arguments[i].val.type;
			if (!isRightType(curType, atype))
			{
				std::cout << "SCISL COMPILER ERROR: LINE: " << lineNum << ", Expected " << typeCharToStr(curType) << ", Got " << typeToStr(atype) << ".\n";
				return false;
			}
		}
		return true;
	}

	precompInstr noopInstr()
	{
		precompInstr opt;
		opt.meta = stlFuncMeta[(unsigned short)(stlFuncs::noop)];
		opt.instr.arguments.argCount = opt.meta.expectedArgs;
		opt.instr.func = opt.meta.fnc;
		opt.instr.arguments.arguments = nullptr;
		return opt;
	}
	
	inline std::pair<argType, type> strToType(std::string& str, std::vector<std::pair<std::string, type>>& vars)
	{
		switch (str[0])
		{
		case '#': //preprocessor
		{
			auto& macros = getMacroTable();
			std::string key = str.substr(1, str.size() - 1);
			if (!macros.contains(key))
			{
				return { argType::interop, type::error };
			}
			str = macros.at(key);
			return strToType(str, vars); //note that you can create an infinite loop with macros
		}
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '-': //numbers
			if (str.find('.') != std::string::npos)
			{
				return { argType::constant, type::floating };
			}
			return { argType::constant, type::integer };
		case '"': //strings
			return { argType::constant, type::string };
		case '$': //interops
		{
			auto& vTable = getVarTable();
			std::string key = str.substr(1, str.size() - 1);
			if (!vTable.contains(key))
			{
				return { argType::interop, type::error };
			}
			return { argType::interop, vTable.at(key)->type };
		}
		default: //variables
			{
				const unsigned int loc = (unsigned int)(findV(vars, str));
				if (loc >= vars.size())
				{
					return { argType::variable, type::error };
				}

				return { argType::variable, vars[loc].second };
			}
		}
	}

	std::pair<precompInstr, bool> parseInstr(std::string& line, std::vector<std::pair<std::string, type>>& vars)
	{
		std::vector<std::string> things = splitLine(line);
		unsigned char argCount = (unsigned char)(things.size() - 1);
		if (line == "" || line == "\t" || line[0] == ';')
		{
			return {noopInstr(), true};
		}

		auto start = things[0].find_first_not_of('\t');
		std::string funcName = things[0].substr(start, things[0].size() - start);
		stlFuncs fID = stlFuncs::stlFuncCount;
		precompInstr opt;

		if (funcName[0] != '$')
		{
			toUpper(funcName);
			fID = strToFuncID(funcName);
			if (fID >= stlFuncs::stlFuncCount)
			{
				auto& funcs = getFuncTable();
				if (funcs.contains(things[0])) // Maybe they forgot the $, warn them
				{
					std::cout << "SCISL COMPILER WARNING: line:" << lineNum << '\t' << things[0] << " registered function not preceeded with $.\n";
					scislfuncMeta& mta = funcs.at(things[0]);
					opt.meta = mta;
				}
				else
				{
					std::cout << "SCISL COMPILER ERROR: line:" << lineNum << '\t' << things[0] << " unknown STL function.\n";
					return { opt, false };
				}
			}
			else
			{
				opt.meta = stlFuncMeta[(unsigned short)(fID)];
			}
		}
		else
		{
			auto& funcs = getFuncTable();
			if (funcs.contains(things[0].substr(1)))
			{
				scislfuncMeta& mta = funcs.at(things[0].substr(1));

				opt.meta = mta;
			}
			else
			{
				std::cout << "SCISL COMPILER ERROR: line:" << lineNum << '\t' << things[0] << " unregistered function.\n";
				return { opt, false };
			}
		}

		if (opt.meta.expectedArgs != 0 && opt.meta.expectedArgs != argCount)
		{
			std::cout << "SCISL COMPILER ERROR: line:" << lineNum << '\t' << things[0] << " expects " << opt.meta.expectedArgs << " args, got " << (int)(argCount) << ".\n";
			return { opt, false }; //error
		}

		if (argCount < opt.meta.minArgs)
		{
			std::cout << "SCISL COMPILER ERROR: line:" << lineNum << '\t' << things[0] << " needs at least " << opt.meta.minArgs << " args, got " << (int)(argCount) << ".\n";
			return { opt, false }; //error
		}

		args& arguments = opt.instr.arguments;
		arguments.arguments = new arg[argCount];
		arguments.argCount = argCount;

		for (unsigned char i = 0; i < argCount; i++)
		{
			std::string& cur = things[i + 1];
			arg* carg = &arguments.arguments[i];

			//labels
			if (i == 0 && (fID == stlFuncs::label || fID == stlFuncs::jmp || fID == stlFuncs::cjmp))
			{
				carg->argType = argType::constant;
				carg->val.type = type::integer;
				carg->val.val = new std::string(cur);
				continue;
			}

			std::pair<argType, type> ctype = strToType(cur, vars);
			carg->argType = ctype.first;
			carg->val.type = ctype.second;

			if (ctype.first == argType::interop && ctype.second == type::error)
			{
				std::cout << "SCISL COMPILER ERROR: line:" << lineNum << "\tUsing unregistered variable/macro " << cur << ".\n";
				return { opt, false };
			}

			//variables
			if (ctype.second == type::error) // (variable hasn't been initialized yet)
			{
				const type rtype = inferType(opt, strToType(things[i + 2], vars).second);
				if (rtype == type::error)
				{
					if (i == 0)
					{
						std::cout << "SCISL COMPILER ERROR: line:" << lineNum << "\tInitializing variable with undeclared variable.\n";
					}
					else
					{
						std::cout << "SCISL COMPILER ERROR: line:" << lineNum << "\tUsing undeclared variable " << cur << ".\n";
					}
					return { opt, false };
				}

				carg->val.type = rtype;
				carg->val.val = new std::string(cur);
				vars.push_back({ cur, carg->val.type });
				continue;
			}
			*carg = cur; //everything else
			//see arg(str&) constructor
		}

		opt.instr.func = opt.meta.fnc;
		auto& peep = opt.meta.peep;
		if (peep != nullptr) peep(opt);
		return { opt, typeCheck(opt) };
	}

	void finalize(std::vector<precompInstr>& instructions)
	{
		std::vector<std::pair<std::string, value*>> remainingVars;
		std::map<std::string, unsigned int> labels;
		for (unsigned int i = 0; i < instructions.size(); i++)
		{
			precompInstr& cur = instructions[i];

			scislPeephole peep = cur.meta.peep;
			if (peep != nullptr) peep(cur);

			if (isFunc(cur.meta, stlFuncs::label))
			{
				labels.insert({ SCISL_CAST_STRING(cur.instr.arguments.arguments[0].val.val) , i });
				delete (std::string*)(cur.instr.arguments.arguments[0].val.val);
				cur.instr.arguments.arguments[0].val.val = new SCISL_INT_PRECISION(i);
				continue;
			}

			for (unsigned int j = 0; j < instructions[i].instr.arguments.argCount; j++)
			{
				arg& cur = instructions[i].instr.arguments.arguments[j];
				if (cur.argType == argType::variable)
				{
					unsigned short loc = (unsigned short)(findV(remainingVars, SCISL_CAST_STRING(cur.val.val)));
					if (loc == remainingVars.size())
					{
						value* space = new value(createTemporary(cur.val.type));
						remainingVars.push_back({ SCISL_CAST_STRING(cur.val.val), space });
					}

					delete (std::string*)(cur.val.val);
					cur.val.val = remainingVars[loc].second->val;
					cur.finalized = true;
				}
			}
		}

		for (unsigned int i = 0; i < instructions.size(); i++)
		{
			precompInstr& cur = instructions[i];
			if (isFunc(cur.meta, stlFuncs::jmp) || isFunc(cur.meta, stlFuncs::cjmp))
			{
				unsigned int loc = labels[SCISL_CAST_STRING(cur.instr.arguments.arguments[0].val.val)];
				delete (std::string*)(cur.instr.arguments.arguments[0].val.val);
				cur.instr.arguments.arguments[0].val.val = new SCISL_INT_PRECISION(loc);
			}
		}

		for (auto& t : remainingVars)
		{
			t.second->val = nullptr;
			delete t.second;
		}
	}

	program* compile(const char* filename, bool nullOnError)
	{
		std::ifstream file(filename);
		if (file.is_open())
		{
			program* opt = new program();
			
			std::string line;

			std::vector<precompInstr> instructions;
			std::vector<std::pair<std::string, type>> vars = {};

			lineNum = 0;
			while (std::getline(file, line))
			{
				lineNum++;
				auto o = parseInstr(line, vars);
				if (o.second)
				{
					instructions.push_back(std::move(o.first));
					continue;
				}
				file.close();

				if (nullOnError)
				{
					delete opt;
					return nullptr;
				}
				else
				{
					finalize(instructions);
					opt->instructions.reserve(instructions.size());
					for (precompInstr& i : instructions)
					{
						opt->instructions.push_back(std::move(i.instr));
					}
					return opt;
				}
			}
			file.close();

			removeUnusedLabels(instructions);
			evaluateConstants(instructions, vars);
			removeNOOP(instructions);
			if (!removeUnreachableCode(instructions)) return nullptr;
			removeUnusedVars(instructions);
			removeUnusedLabels(instructions);

			finalize(instructions);
			removeNOOP(instructions);
			opt->instructions.reserve(instructions.size());
			for (precompInstr& i : instructions)
			{
				opt->instructions.push_back(std::move(i.instr));
			}

			return opt;
		}
		std::cout << "SCISL COMPILER ERROR: Could not open file " << filename << ".\n";
		return nullptr;
	}
}