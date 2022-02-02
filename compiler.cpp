#include "compiler.h"

#include "common.h"
#include "tables.h"
#include "stl.h"

#include <fstream>
#include <iostream>

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
		case '-':
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

	size_t lineNum = 0;
	std::pair<precompInstr, bool> parseInstr(std::string& line, std::vector<std::pair<std::string, type>>& vars)
	{
		std::vector<std::string> things = splitLine(line);
		unsigned char argCount = things.size() - 1;
		if (line == "" || line == "\t" || line[0] == ';')
		{
			return {noopInstr(), true};
		}

		stlFuncs fID = strToFuncID(things[0]);

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

		if (opt.meta.expectedArgs != 0)
		{
			if (opt.meta.expectedArgs != argCount)
			{
				std::cout << "SCISL COMPILER ERROR: LINE: " << lineNum << '\t' << things[0] << " expects " << opt.meta.expectedArgs << " args, got " << (int)(argCount) << ".\n";
				return { opt, false }; //error
			}
		}

		opt.instr.arguments.arguments = new arg[argCount];
		opt.instr.arguments.argCount = argCount;

		for (unsigned char i = 0; i < argCount; i++)
		{
			std::string& cur = things[i + 1];
			arg* carg = &opt.instr.arguments.arguments[i];

			if (i == 0 && (fID == stlFuncs::label || fID == stlFuncs::jmp || fID == stlFuncs::cjmp))
			{
				carg->argType = argType::constant;
				carg->type = type::integer;
				carg->val = new std::string(cur);
				continue;
			}

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
				carg->val = new std::string(vars[loc].first);
				continue;
			}

			std::string& next = things[i + 2];
			if (isNumeric(next[0]))
			{
				if (next.find('.') != std::string::npos)
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
			else if (next[0] == '$')
			{
				carg->type = type::integer;
				std::cout << "SCISL COMPILER WARNING: line:" << lineNum << "\tInitializing variable with interoperable, assuming INT.\n";
			}
			else
			{
				const unsigned int loc = findV(vars, next);
				if (loc >= vars.size())
				{
					std::cout << "SCISL COMPILER ERROR: line:" << lineNum << "\tInitializing variable with undeclared variable.\n";
				}
				else
				{
					carg->type = vars[loc].second;
				}
			}

			carg->val = new std::string(cur);
			vars.push_back({ cur, carg->type });
		}

		opt.instr.func = opt.meta.fnc;
		return { opt, true };
	}

	void finalize(std::vector<precompInstr>& instructions, std::vector<std::pair<std::string, type>>& vars)
	{
		std::vector<std::pair<std::string, type>> remainingVars;
		std::map<std::string, unsigned int> labels;
		for (unsigned int i = 0; i < instructions.size(); i++)
		{
			precompInstr& cur = instructions[i];
			
			scislPeephole peep = cur.meta.peep;
			if (peep != nullptr) peep(cur);

			if (isFunc(cur.meta, stlFuncs::label))
			{
				labels.insert({ SCISL_CAST_STRING(cur.instr.arguments.arguments[0].val) , i });
				delete (std::string*)(cur.instr.arguments.arguments[0].val);
				cur.instr.arguments.arguments[0].val = new SCISL_INT_PRECISION(i);
				continue;
			}

			for (unsigned int j = 0; j < instructions[i].instr.arguments.argCount; j++)
			{
				arg& cur = instructions[i].instr.arguments.arguments[j];
				if (cur.argType == argType::variable)
				{
					if (findV(remainingVars, SCISL_CAST_STRING(cur.val)) == remainingVars.size())
					{
						remainingVars.push_back({ SCISL_CAST_STRING(cur.val), cur.type });
					}

					unsigned short loc = findV(vars, SCISL_CAST_STRING(cur.val));
					delete (std::string*)(cur.val);
					cur.val = new unsigned short(loc);
					cur.finalized = true;
				}
			}
		}

		for (unsigned int i = 0; i < instructions.size(); i++)
		{
			precompInstr& cur = instructions[i];
			if (isFunc(cur.meta, stlFuncs::jmp) || isFunc(cur.meta, stlFuncs::cjmp))
			{
				unsigned int loc = labels[SCISL_CAST_STRING(cur.instr.arguments.arguments[0].val)];
				delete (std::string*)(cur.instr.arguments.arguments[0].val);
				cur.instr.arguments.arguments[0].val = new SCISL_INT_PRECISION(loc);
			}
		}
		vars = remainingVars;
	}

	value getVal(arg& a, std::map<std::string, value>& evalVals)
	{
		if (a.argType == argType::interop)
		{
			return createTemporary(type::error);
		}

		if (a.argType == argType::constant)
		{
			value v = createTemporary(a.type);
			v = a.val;
			return v;
		}
		
		if (a.argType == argType::variable)
		{
			value& t = evalVals.at(SCISL_CAST_STRING(a.val));
			value v = createTemporary(t.type);
			if (v.type != type::error)
			{
				v = t.val;
			}
			return v;
		}

		return createTemporary(type::error);
	}

	void simulate(std::map<std::string, value>& vals, args& argz, scislFunc target)
	{
		program fakeP;
		std::vector<std::pair<std::string, value&>> virtualVars;
		args fakeArgs;
		fakeArgs.argCount = argz.argCount;
		fakeArgs.arguments = new arg[argz.argCount];

		for (auto& v : vals)
		{
			if (v.second.type != type::error)
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
				fakeArgs.arguments[i].finalized = true;
				fakeArgs.arguments[i].type = cur.type;
				fakeArgs.arguments[i].val = new unsigned short(findV(virtualVars, SCISL_CAST_STRING(cur.val)));
				continue;
			}
			fakeArgs.arguments[i] = cur;
		}

		fakeP.memsize = virtualVars.size();
		fakeP.memory = new value[fakeP.memsize];
		for (unsigned int i = 0; i < fakeP.memsize; i++)
		{
			fakeP.memory[i] = createTemporary(virtualVars[i].second.type);
			fakeP.memory[i].isTemporary = false;
			fakeP.memory[i] = virtualVars[i].second.val;
		}

		target(fakeP, fakeArgs);

		for (unsigned int i = 0; i < fakeP.memsize; i++)
		{
			virtualVars[i].second = fakeP.memory[i].val;
		}

		delete[] fakeArgs.arguments;
	}

	//done pretty early, essentially runs the program to figure out if things can be figured out ahead of time
	void evaluateConstants(std::vector<precompInstr>& process, std::vector<std::pair<std::string, type>>& vars)
	{
		std::vector<precompInstr> newProcess;
		newProcess.reserve(process.size());

		std::map<std::string, value> evalVal;
		
		for (precompInstr& i : process)
		{
			if (isFunc(i.meta, stlFuncs::set)) //initialization of var
			{
				arg& cur = i.instr.arguments.arguments[0];
				if (cur.argType != argType::variable)
				{
					newProcess.push_back(i);
					continue;
				}

				value val = getVal(i.instr.arguments.arguments[1], evalVal);
				value tmp = createTemporary(val.type);
				tmp.isTemporary = false;
				evalVal.insert({ SCISL_CAST_STRING(cur.val), tmp });
				evalVal.at(SCISL_CAST_STRING(cur.val)) = val.val;

				if (val.type == type::error)
				{
					newProcess.push_back(i);
					continue;
				}

				delete[] i.instr.arguments.arguments;
			}
			else if ((i.meta.optimizerFlags & SCISL_OP_NO_JMP) == 0) //jumps are scary, invalidate everything once one is found
			{
				for (auto& p : evalVal)
				{
					if (p.second.type != type::error)
					{
						precompInstr t;
						t.meta = stlFuncMeta[(unsigned short)(stlFuncs::set)];
						t.instr.func = t.meta.fnc;
						t.instr.arguments.argCount = 2;
						t.instr.arguments.arguments = new arg[2];
						t.instr.arguments.arguments[0].argType = argType::variable;
						t.instr.arguments.arguments[0].type = p.second.type;
						t.instr.arguments.arguments[0].val = new std::string(p.first);

						t.instr.arguments.arguments[1].argType = argType::constant;
						t.instr.arguments.arguments[1].type = p.second.type;
						switch (p.second.type)
						{
						case type::string:
							t.instr.arguments.arguments[1].val = new std::string(SCISL_CAST_STRING(p.second.val));
							break;
						case type::integer:
							t.instr.arguments.arguments[1].val = new SCISL_INT_PRECISION(SCISL_CAST_INT(p.second.val));
							break;
						case type::floating:
							t.instr.arguments.arguments[1].val = new SCISL_FLOAT_PRECISION(SCISL_CAST_FLOAT(p.second.val));
							break;
						}
						newProcess.push_back(t);
					}
					p.second.isTemporary = true;
					p.second = createTemporary(type::error);
				}
				newProcess.push_back(i);
			}
			else if ((i.meta.optimizerFlags & SCISL_OP_NO_MOD) > 0) //doesn't modify anything, good!
			{
				for (unsigned int j = 0; j < i.instr.arguments.argCount; j++)
				{
					arg& cur = i.instr.arguments.arguments[j];
					if (cur.argType == argType::variable)
					{
						if (evalVal.contains(SCISL_CAST_STRING(cur.val)))
						{
							value val = evalVal.at(SCISL_CAST_STRING(cur.val));
							if (val.type != type::error)
							{
								cur.argType = argType::constant;
								cur.type = val.type;
								delete (std::string*)(cur.val);
								switch (cur.type)
								{
								case type::string:
									cur.val = new std::string(SCISL_CAST_STRING(val.val));
									break;
								case type::integer:
									cur.val = new SCISL_INT_PRECISION(SCISL_CAST_INT(val.val));
									break;
								case type::floating:
									cur.val = new SCISL_FLOAT_PRECISION(SCISL_CAST_FLOAT(val.val));
									break;
								}
							}
						}
						else
						{
							std::cout << "SCISL COMPILER ERROR: variable " << SCISL_CAST_STRING(cur.val) << " referenced before being defined by SET by " << i.meta.funcID << ".\n";
						}
					}
				}
				newProcess.push_back(i);
			}
			else //these can modify
			{
				if (!isSTLfunc(i.meta.fnc)) //invalidate everything
				{
					for (auto& p : evalVal)
					{
						if (p.second.type != type::error)
						{
							precompInstr t;
							t.meta = stlFuncMeta[(unsigned short)(stlFuncs::set)];
							t.instr.func = t.meta.fnc;
							t.instr.arguments.argCount = 2;
							t.instr.arguments.arguments = new arg[2];
							t.instr.arguments.arguments[0].argType = argType::variable;
							t.instr.arguments.arguments[0].type = p.second.type;
							t.instr.arguments.arguments[0].val = new std::string(p.first);

							t.instr.arguments.arguments[1].argType = argType::constant;
							t.instr.arguments.arguments[1].type = p.second.type;
							switch (p.second.type)
							{
							case type::string:
								t.instr.arguments.arguments[1].val = new std::string(SCISL_CAST_STRING(p.second.val));
								break;
							case type::integer:
								t.instr.arguments.arguments[1].val = new SCISL_INT_PRECISION(SCISL_CAST_INT(p.second.val));
								break;
							case type::floating:
								t.instr.arguments.arguments[1].val = new SCISL_FLOAT_PRECISION(SCISL_CAST_FLOAT(p.second.val));
								break;
							}
							newProcess.push_back(t);
						}
						p.second.isTemporary = true;
						p.second = createTemporary(type::error);
					}
					newProcess.push_back(i);
					continue;
				}

				bool valid = true;
				arg& modified = i.instr.arguments.arguments[0];
				if (modified.argType != argType::variable)
				{
					for (unsigned int j = 1; j < i.instr.arguments.argCount; j++)
					{
						arg& cur = i.instr.arguments.arguments[j];
						value val = getVal(cur, evalVal);
						if (val.type != type::error)
						{

							if (cur.argType == argType::variable)
							{
								delete (std::string*)cur.val;

								cur.argType = argType::constant;
								cur.type = val.type;
								switch (cur.type)
								{
								case type::string:
									cur.val = new std::string(SCISL_CAST_STRING(val.val));
									break;
								case type::integer:
									cur.val = new SCISL_INT_PRECISION(SCISL_CAST_INT(val.val));
									break;
								case type::floating:
									cur.val = new SCISL_FLOAT_PRECISION(SCISL_CAST_FLOAT(val.val));
									break;
								}
							}
						}
					}
					newProcess.push_back(i);
					continue;
				}
				for (unsigned int j = 1; j < i.instr.arguments.argCount; j++)
				{
					arg& cur = i.instr.arguments.arguments[j];
					value val = getVal(cur, evalVal);
					if (val.type == type::error)
					{
						valid = false;
					}
				}

				value v = getVal(modified, evalVal);
				if (!valid && v.type != type::error)
				{
					precompInstr t;
					t.meta = stlFuncMeta[(unsigned short)(stlFuncs::set)];
					t.instr.func = t.meta.fnc;
					t.instr.arguments.argCount = 2;
					t.instr.arguments.arguments = new arg[2];
					t.instr.arguments.arguments[0].argType = argType::variable;
					t.instr.arguments.arguments[0].type = v.type;
					t.instr.arguments.arguments[0].val = new std::string(SCISL_CAST_STRING(modified.val));

					t.instr.arguments.arguments[1].argType = argType::constant;
					t.instr.arguments.arguments[1].type = v.type;
					switch (v.type)
					{
					case type::string:
						t.instr.arguments.arguments[1].val = new std::string(SCISL_CAST_STRING(v.val));
						break;
					case type::integer:
						t.instr.arguments.arguments[1].val = new SCISL_INT_PRECISION(SCISL_CAST_INT(v.val));
						break;
					case type::floating:
						t.instr.arguments.arguments[1].val = new SCISL_FLOAT_PRECISION(SCISL_CAST_FLOAT(v.val));
						break;
					}
					newProcess.push_back(t);
					evalVal.at(SCISL_CAST_STRING(modified.val)).isTemporary = true;
					evalVal.at(SCISL_CAST_STRING(modified.val)) = createTemporary(type::error);
					newProcess.push_back(i);
					continue;
				}
				
				if (valid)
				{ 
					switch (strToFuncID(i.meta.funcID))
					{
					case stlFuncs::add:
					case stlFuncs::adde:
					case stlFuncs::sub:
					case stlFuncs::sube:
					case stlFuncs::mult:
					case stlFuncs::multe:
					case stlFuncs::div:
					case stlFuncs::dive:
					case stlFuncs::less:
					case stlFuncs::great:
					case stlFuncs::equal:
					case stlFuncs::nequal:
					{
						simulate(evalVal, i.instr.arguments, i.instr.func);
						delete[] i.instr.arguments.arguments;
						break;
					}
					default:
						newProcess.push_back(i);
						break;
					}
				}
				else
				{
					newProcess.push_back(i);
				}
			}
		}

		for (auto& i : evalVal)
		{
			for (auto& p : vars)
			{
				if (p.first == i.first)
				{
					i.second.type = p.second;
					break;
				}
			}
		}

		process = newProcess;
	}

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

	program* compile(const char* filename)
	{
		std::ifstream file(filename);
		if (file.is_open())
		{
			program* opt = new program();
			opt->memory = nullptr;
			
			std::string line;
			std::vector<std::pair<std::string, type>> vars = {};

			std::vector<precompInstr> instructions;

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
				return nullptr;
			}

			evaluateConstants(instructions, vars);
			removeNOOP(instructions);
			removeUnusedVars(instructions);
			
			finalize(instructions, vars);
			opt->instructions.reserve(instructions.size());
			for (precompInstr& i : instructions)
			{
				opt->instructions.push_back(i.instr);
			}
			
			opt->memory = new value[vars.size()];
			opt->memsize = vars.size();
			for (unsigned int i = 0; i < vars.size(); i++)
			{
				opt->memory[i].type = vars[i].second;
				switch (vars[i].second)
				{
				case type::integer:
					opt->memory[i].val = new SCISL_INT_PRECISION(0);
					break;
				case type::floating:
					opt->memory[i].val = new SCISL_FLOAT_PRECISION(0);
					break;
				case type::string:
					opt->memory[i].val = new std::string("");
					break;
				}
			}

			file.close();
			return opt;
		}
		return nullptr;
	}
}