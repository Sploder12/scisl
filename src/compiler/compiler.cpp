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
		for (unsigned int i = 0; i < bi.instr.argCount; i++)
		{
			if (i < tstr.size()) curType = tstr[i];

			type atype = bi.instr.arguments[i].val.type;
			if (!isRightType(curType, atype))
			{
				std::cout << "SCISL COMPILER ERROR: line: " << lineNum << ", Expected " << typeCharToStr(curType) << ", Got " << typeToStr(atype) << ".\n";
				return false;
			}
		}
		return true;
	}

	precompInstr noopInstr()
	{
		precompInstr opt;
		opt.meta = stlFuncMeta[(unsigned short)(stlFuncs::noop)];
		opt.instr.argCount = opt.meta.expectedArgs;
		opt.instr.func = opt.meta.fnc;
		opt.instr.arguments = nullptr;
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

		opt.instr.arguments = new arg[argCount];
		opt.instr.argCount = argCount;

		for (unsigned char i = 0; i < argCount; i++)
		{
			std::string& cur = things[i + 1];
			arg* carg = &opt.instr.arguments[i];

			//labels
			if (i == 0 && (fID == stlFuncs::label || fID == stlFuncs::jmp || fID == stlFuncs::cjmp || fID == stlFuncs::def  || fID == stlFuncs::call))
			{
				carg->argType = argType::constant;
				carg->val.type = type::integer;
				carg->val.val = new std::string(cur);
				continue;
			}

			auto [argType, valType] = strToType(cur, vars);
			carg->argType = argType;
			carg->val.type = valType;

			if (argType == argType::interop && valType == type::error)
			{
				std::cout << "SCISL COMPILER ERROR: line:" << lineNum << "\tUsing unregistered variable/macro " << cur << ".\n";
				return { opt, false };
			}

			//variables
			if (valType == type::error) // (variable hasn't been initialized yet)
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

			if (isFunc(cur.meta, stlFuncs::label) || isFunc(cur.meta, stlFuncs::def))
			{
				labels.insert({ SCISL_CAST_STRING(cur.instr.arguments[0].val.val) , i });
				delete (std::string*)(cur.instr.arguments[0].val.val);
				cur.instr.arguments[0].val.val = new SCISL_INT_PRECISION(i);
				continue;
			}

			for (unsigned int j = 0; j < instructions[i].instr.argCount; j++)
			{
				arg& cur = instructions[i].instr.arguments[j];
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
			if (isFunc(cur.meta, stlFuncs::jmp) || isFunc(cur.meta, stlFuncs::cjmp) || isFunc(cur.meta, stlFuncs::call))
			{
				unsigned int loc = labels[SCISL_CAST_STRING(cur.instr.arguments[0].val.val)];
				delete (std::string*)(cur.instr.arguments[0].val.val);
				cur.instr.arguments[0].val.val = new SCISL_INT_PRECISION(loc);
			}
			else if (isFunc(cur.meta, stlFuncs::def))
			{
				unsigned int loc = findBlockEnd(instructions, i);
				SCISL_CAST_INT(cur.instr.arguments[0].val.val) = SCISL_INT_PRECISION(loc);
			}
		}

		for (auto& [id, var] : remainingVars)
		{
			var->val = nullptr;
			delete var;
		}
	}

	bool detectErrors(std::vector<precompInstr>& instructions)
	{
		bool detected = false;
		std::map<std::string, type> vars;
		unsigned int curBlockStart = 0;
		unsigned int curBlockEnd = (unsigned int)(instructions.size());
		for (unsigned int i = 0; i < instructions.size(); i++)
		{
			precompInstr& cur = instructions[i];

			if (cur.meta.flags & SCISL_F_BLOCK)
			{
				unsigned int loc = findBlockEnd(instructions, i);
				if (loc == instructions.size())
				{
					std::cout << "SCISL COMPILER ERROR: line:" << i << "\t" << cur.meta.funcName << " has no endblock.\n";
					detected = true;
				}
				curBlockStart = i;
				curBlockEnd = loc;
			}

			if (isFunc(cur.meta, stlFuncs::call))
			{
				std::string& str = SCISL_CAST_STRING(cur.instr.arguments[0].val.val);
				unsigned int loc = findLabel(instructions, str, stlFuncs::def);
				if (loc == instructions.size())
				{
					std::cout << "SCISL COMPILER ERROR: line:" << i << "\t" << cur.meta.funcName << " has no associated function.\n";
					detected = true;
				}
				else if (i < loc)
				{
					std::cout << "SCISL COMPILER ERROR: line:" << i << "\t" << cur.meta.funcName << " cannot be before function definition.\n";
					detected = true;
				}
			}
			else if (isFunc(cur.meta, stlFuncs::jmp) || isFunc(cur.meta, stlFuncs::cjmp))
			{
				std::string& str = SCISL_CAST_STRING(cur.instr.arguments[0].val.val);
				unsigned int loc = findLabel(instructions, str, stlFuncs::label);
				if (loc == instructions.size())
				{
					std::cout << "SCISL COMPILER ERROR: line:" << i << "\t" << cur.meta.funcName << " has no associated label.\n";
					detected = true;
				}
				else if (loc >= curBlockEnd || loc <= curBlockStart)
				{
					std::cout << "SCISL COMPILER ERROR: line:" << i << "\t" << cur.meta.funcName << " jumps outside current block.\n";
					detected = true;
				}
			}
		}
		return detected;
	}

	program* compile(const char* filename)
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
				auto [instr, worked] = parseInstr(line, vars);
				if (worked)
				{
					instructions.push_back(std::move(instr));
				}
				else
				{
					file.close();
					delete opt;
					return nullptr;
				}
			}
			file.close();

			bool err = detectErrors(instructions);
			if (err)
			{
				delete opt;
				return nullptr;
			}

			removeUnusedLabels(instructions);
			evaluateConstants(instructions, vars);
			removeNOOP(instructions);
			removeUnreachableCode(instructions);
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