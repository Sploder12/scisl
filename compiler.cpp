#include "compiler.h"

#include "common.h"
#include "tables.h"
#include "stl.h"

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

	precompInstr noopInstr()
	{
		precompInstr opt;
		opt.meta = stlFuncMeta[(unsigned short)(stlFuncs::noop)];
		opt.instr.arguments.argCount = opt.meta.expectedArgs;
		opt.instr.func = opt.meta.fnc;
		opt.instr.arguments.arguments = nullptr;
		return opt;
	}

	precompInstr setInstr(const std::string& varName, value* var)
	{
		precompInstr opt;
		opt.meta = stlFuncMeta[(unsigned short)(stlFuncs::set)];
		opt.instr.func = opt.meta.fnc;
		opt.instr.arguments.argCount = 2;
		opt.instr.arguments.arguments = new arg[2];
		opt.instr.arguments.arguments[0].argType = argType::variable;
		opt.instr.arguments.arguments[0].val.type = var->type;
		opt.instr.arguments.arguments[0].val.val = new std::string(varName);

		opt.instr.arguments.arguments[1].argType = argType::constant;
		opt.instr.arguments.arguments[1].val.type = var->type;
		switch (var->type)
		{
		case type::string:
			opt.instr.arguments.arguments[1].val.val = new std::string(SCISL_CAST_STRING(var->val));
			break;
		case type::integer:
			opt.instr.arguments.arguments[1].val.val = new SCISL_INT_PRECISION(SCISL_CAST_INT(var->val));
			break;
		case type::floating:
			opt.instr.arguments.arguments[1].val.val = new SCISL_FLOAT_PRECISION(SCISL_CAST_FLOAT(var->val));
			break;
		}
		return opt;
	}

	size_t lineNum = 0;
	inline std::pair<argType, type> strToType(std::string& str, std::vector<std::pair<std::string, type>>& vars)
	{
		switch (str[0])
		{
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '-':
			if (str.find('.') != std::string::npos)
			{
				return { argType::constant, type::floating };
			}
			return { argType::constant, type::integer };
		case '"':
			return { argType::constant, type::string };
		case '$':
		{
			auto& vTable = getVarTable();
			return { argType::interop, vTable.at(str.substr(1, str.size() - 1))->type };
		}
		default:
			{
				const unsigned int loc = findV(vars, str);
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
		unsigned char argCount = things.size() - 1;
		if (line == "" || line == "\t" || line[0] == ';')
		{
			return {noopInstr(), true};
		}

		const stlFuncs fID = strToFuncID(things[0]);
		precompInstr opt;

		if (fID != stlFuncs::stlFuncCount)
		{
			opt.meta = stlFuncMeta[(unsigned short)(fID)];
		}
		else
		{
			auto& funcs = getFuncTable();
			scislfuncMeta& mta = funcs.at(things[0]);

			opt.meta = mta;
		}

		if (opt.meta.expectedArgs != 0 && opt.meta.expectedArgs != argCount)
		{
			std::cout << "SCISL COMPILER ERROR: LINE: " << lineNum << '\t' << things[0] << " expects " << opt.meta.expectedArgs << " args, got " << (int)(argCount) << ".\n";
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

			//variables
			if (ctype.second == type::error)
			{
				std::string& next = things[i + 2];
				type rtype = strToType(next, vars).second;
				if (rtype == type::error)
				{
					std::cout << "SCISL COMPILER ERROR: line:" << lineNum << "\tInitializing variable with undeclared variable.\n";
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
		return { opt, true };
	}

	
	void simulate(std::map<std::string, value*>& vals, args& argz, scislFunc target)
	{
		program fakeP;
		std::vector<std::pair<std::string, value*>> virtualVars;
		args fakeArgs;
		fakeArgs.argCount = argz.argCount;
		fakeArgs.arguments = new arg[argz.argCount];

		for (auto& v : vals)
		{
			if (v.second != nullptr)
			{
				virtualVars.push_back({v.first, v.second});
			}
		}

		for (unsigned int i = 0; i < argz.argCount; i++)
		{
			arg& cur = argz.arguments[i];
			if (cur.argType == argType::variable)
			{
				fakeArgs.arguments[i].argType = argType::variable; 
				fakeArgs.arguments[i].val.val = virtualVars[findV(virtualVars, SCISL_CAST_STRING(cur.val.val))].second->val;
				fakeArgs.arguments[i].val.type = virtualVars[findV(virtualVars, SCISL_CAST_STRING(cur.val.val))].second->type;
				continue;
			}
			fakeArgs.arguments[i] = cur;
		}

		target(fakeP, fakeArgs);

		for (unsigned int i = 0; i < fakeArgs.argCount; i++)
		{
			if (fakeArgs.arguments[i].argType == argType::variable)
			{
				fakeArgs.arguments[i].val.val = nullptr;
			}
		}
		delete[] fakeArgs.arguments;
	}


	inline void invalidateVars(std::vector<precompInstr>& newProcess, std::map<std::string, value*>& evalVal)
	{
		for (std::pair<const std::string, value*>& var : evalVal)
		{
			if (var.second != nullptr)
			{
				newProcess.push_back(setInstr(var.first, var.second));
			}
			delete var.second;
			var.second = nullptr;
		}
	}

	inline void deleteArgsUF(args& argz)
	{
		for (unsigned int j = 0; j < argz.argCount; j++)
		{
			arg* c = &argz.arguments[j];
			if (c->argType == argType::variable)
			{
				delete (std::string*)(c->val.val);
			}
		}
		delete[] argz.arguments;
	}

	//done pretty early, essentially runs the program to figure out if things can be figured out ahead of time
	void evaluateConstants(std::vector<precompInstr>& process, std::vector<std::pair<std::string, type>>& vars)
	{
		std::vector<precompInstr> newProcess;
		newProcess.reserve(process.size());

		std::map<std::string, value*> evalVal;
		
		for (precompInstr& i : process)
		{
			if (isFunc(i.meta, stlFuncs::set)) //initialization of var
			{
				arg& cur = i.instr.arguments.arguments[0];
				if (cur.argType != argType::variable)
				{
					newProcess.push_back(std::move(i));
					continue;
				}

				arg& next = i.instr.arguments.arguments[1];
				if (evalVal.contains(SCISL_CAST_STRING(cur.val.val)))
				{
					if (next.argType == argType::interop)
					{
						delete evalVal.at(SCISL_CAST_STRING(cur.val.val));
						evalVal.at(SCISL_CAST_STRING(cur.val.val)) = nullptr;
						newProcess.push_back(std::move(i));
						continue;
					}

					if (next.argType == argType::variable)
					{
						value* v = evalVal.at(SCISL_CAST_STRING(next.val.val));
						if (v == nullptr)
						{
							delete evalVal.at(SCISL_CAST_STRING(cur.val.val));
							evalVal.at(SCISL_CAST_STRING(cur.val.val)) = nullptr;
							newProcess.push_back(std::move(i));
							continue;
						}

						*evalVal.at(SCISL_CAST_STRING(cur.val.val)) = v->val;
						continue;
					}

					*evalVal.at(SCISL_CAST_STRING(cur.val.val)) = next.val.val;
					continue;
				}

				value* n = new value();
				if (next.argType == argType::interop)
				{
					delete n;
					newProcess.push_back(std::move(i));
					evalVal.insert({ SCISL_CAST_STRING(cur.val.val), nullptr });
					continue;
				}
				
				if (next.argType == argType::variable)
				{
					value* v = evalVal.at(SCISL_CAST_STRING(next.val.val));
					if (v == nullptr)
					{
						delete n;
						newProcess.push_back(std::move(i));
						evalVal.insert({ SCISL_CAST_STRING(cur.val.val), nullptr });
						continue;
					}

					*n = *v;
				}
				else
				{
					*n = next.val;
				}
	
				evalVal.insert({ SCISL_CAST_STRING(cur.val.val), n });

				deleteArgsUF(i.instr.arguments);
				continue;
			}

			if ((i.meta.optimizerFlags & SCISL_OP_NO_JMP) == 0) //jumps are scary, invalidate everything once one is found
			{
				invalidateVars(newProcess, evalVal);
				newProcess.push_back(std::move(i));
				continue;
			}
			
			if ((i.meta.optimizerFlags & SCISL_OP_NO_MOD) > 0) //doesn't modify anything, good!
			{
				for (unsigned int j = 0; j < i.instr.arguments.argCount; j++)
				{
					arg& cur = i.instr.arguments.arguments[j];
					if (cur.argType == argType::variable)
					{
						if (evalVal.contains(SCISL_CAST_STRING(cur.val.val)))
						{
							value* val = evalVal.at(SCISL_CAST_STRING(cur.val.val));
							if (val != nullptr)
							{
								cur.argType = argType::constant;
								delete (std::string*)(cur.val.val);
								cur.val = *val;
							}
						}
						else
						{
							std::cout << "SCISL COMPILER ERROR: variable " << SCISL_CAST_STRING(cur.val.val) << " referenced before being defined by SET by " << i.meta.funcID << ".\n";
						}
					}
				}
				newProcess.push_back(std::move(i));
				continue;
			}
			
			//these can modify
			if (!isSTLfunc(i.meta.fnc)) //invalidate everything
			{
				invalidateVars(newProcess, evalVal);
				newProcess.push_back(std::move(i));
				continue;
			}

			bool valid = true;
			arg& modified = i.instr.arguments.arguments[0];
			if (modified.argType != argType::variable)
			{
				for (unsigned int j = 1; j < i.instr.arguments.argCount; j++)
				{
					arg& cur = i.instr.arguments.arguments[j];
					value* val = evalVal.at(SCISL_CAST_STRING(cur.val.val));
					if (val != nullptr && cur.argType == argType::variable)
					{
						cur.argType = argType::constant;
						delete (std::string*)(cur.val.val);
						cur.val = *val;
					}
				}
				newProcess.push_back(std::move(i));
				continue;
			}

			for (unsigned int j = 1; j < i.instr.arguments.argCount; j++)
			{
				arg& cur = i.instr.arguments.arguments[j];
				if (cur.argType != argType::variable) continue;

				value* val = evalVal.at(SCISL_CAST_STRING(cur.val.val));
				if (val == nullptr)
				{
					valid = false;
				}
			}

			value* v = evalVal.at(SCISL_CAST_STRING(modified.val.val));
			if (!valid && v != nullptr)
			{
				newProcess.push_back(setInstr(SCISL_CAST_STRING(modified.val.val), v));
				delete (std::string*)(v->val);
				delete v;
				v = nullptr;
				newProcess.push_back(std::move(i));
				continue;
			}

			if (valid)
			{
				switch (strToFuncID(i.meta.funcID))
				{
				case stlFuncs::add: case stlFuncs::adde:
				case stlFuncs::sub: case stlFuncs::sube:
				case stlFuncs::mult: case stlFuncs::multe:
				case stlFuncs::div: case stlFuncs::dive:
				case stlFuncs::less: case stlFuncs::great:
				case stlFuncs::equal: case stlFuncs::nequal:
				{
					simulate(evalVal, i.instr.arguments, i.instr.func);
					deleteArgsUF(i.instr.arguments);
					break;
				}
				default:
					newProcess.push_back(std::move(i));
					break;
				}
				continue;
			}

			newProcess.push_back(std::move(i));
		}

		process = newProcess;

		for (auto& i : evalVal)
		{
			if (i.second != nullptr)
			{
				delete (std::string*)(i.second->val);
			}
			delete i.second;
		}
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
					unsigned short loc = findV(remainingVars, SCISL_CAST_STRING(cur.val.val));
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

	void removeNOOP(std::vector<precompInstr>& instructions)
	{
		std::vector<precompInstr> remaining;
		remaining.reserve(instructions.size());
		for (precompInstr& i : instructions)
		{
			if (!isFunc(i.meta, stlFuncs::noop))
			{
				remaining.push_back(std::move(i));
			}
			else
			{
				delete[] i.instr.arguments.arguments;
			}
		}
		instructions = std::move(remaining);
	}

	/*
	void removeUnusedVars(std::vector<precompInstr>& instructions)
	{
		std::map<std::string, unsigned int> varCount;
		std::vector<precompInstr> remaining;
		remaining.reserve(instructions.size());

		for (precompInstr& i : instructions)
		{
			for (unsigned int j = 0; j < i.instr.arguments.argCount; j++)
			{
				arg& cur = i.instr.arguments.arguments[j];
				if (cur.argType == argType::variable)
				{
					if (varCount.contains(SCISL_CAST_STRING(cur.val)))
					{
						varCount.at(SCISL_CAST_STRING(cur.val)) += 1;
					}
					else
					{
						varCount.insert({ SCISL_CAST_STRING(cur.val), 1});
					}
				}
			}
		}

		for (precompInstr& i : instructions)
		{
			if (isFunc(i.meta, stlFuncs::set))
			{
				arg& cur = i.instr.arguments.arguments[0];
				if (cur.argType == argType::variable)
				{
					if (varCount.at(SCISL_CAST_STRING(cur.val)) == 1)
					{
						delete[] i.instr.arguments.arguments;
						continue;
					}
				}
			}
			remaining.push_back(i);
		}
		instructions = remaining;
		
	}
	*/

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
				auto o = parseInstr(line, vars);
				if (o.second)
				{
					instructions.push_back(o.first);
					continue;
				}
				file.close();
				return nullptr;
			}
			file.close();

			evaluateConstants(instructions, vars);
			removeNOOP(instructions);
			//removeUnusedVars(instructions);
			
			finalize(instructions);
			opt->instructions.reserve(instructions.size());
			for (precompInstr& i : instructions)
			{
				opt->instructions.push_back(std::move(i.instr));
			}

			return opt;
		}
		return nullptr;
	}
}