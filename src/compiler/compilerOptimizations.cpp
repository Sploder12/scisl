#include "compilerOptimizations.h"

#include <iostream>
#include <map>

#include "../runtime/stl.h"
#include "../runtime/stlOptimizations.h"

namespace scisl
{
	inline unsigned int findBlockEnd(std::vector<precompInstr>& instructions, unsigned int start)
	{
		unsigned int dl = 1;
		for (unsigned int i = start + 1; i < instructions.size(); i++)
		{
			precompInstr& cur = instructions[i];
			if (cur.meta.flags & SCISL_F_BLOCK)
			{
				dl++;
			}
			else if (isFunc(cur.meta, stlFuncs::blockend))
			{
				if (--dl == 0) return i;
			}
		}
		return (unsigned int)(instructions.size());
	}

	type inferType(const precompInstr& instr, type nextArgType)
	{
		if (instr.meta.flags & SCISL_F_INITIALIZES)
		{
			type rtype = instr.meta.initializes;
			if (rtype == type::error)
			{
				return nextArgType;
			}
			return rtype;
		}
		return type::error;
	}

	precompInstr setInstr(const std::string& varName, value* var)
	{
		precompInstr opt;
		opt.meta = stlFuncMeta[(unsigned short)(stlFuncs::set)];
		opt.instr.func = opt.meta.fnc;
		opt.instr.argCount = 2;
		opt.instr.arguments = new arg[2];
		opt.instr.arguments[0].argType = argType::variable;
		opt.instr.arguments[0].val.type = var->type;
		opt.instr.arguments[0].val.val = new std::string(varName);

		opt.instr.arguments[1].argType = argType::constant;
		opt.instr.arguments[1].val.type = var->type;
		switch (var->type)
		{
		case type::string:
			opt.instr.arguments[1].val.val = new std::string(SCISL_CAST_STRING(var->val));
			break;
		case type::integer:
			opt.instr.arguments[1].val.val = new SCISL_INT_PRECISION(SCISL_CAST_INT(var->val));
			break;
		case type::floating:
			opt.instr.arguments[1].val.val = new SCISL_FLOAT_PRECISION(SCISL_CAST_FLOAT(var->val));
			break;
		}
		return opt;
	}

	inline void simulate(std::map<std::string, value*>& vals, instruction& instr)
	{
		program fakeP;
		std::vector<std::pair<std::string, value*>> virtualVars;

		instruction fakeArgs;
		fakeArgs.argCount = instr.argCount;
		fakeArgs.arguments = new arg[instr.argCount];

		for (auto& [name, value] : vals)
		{
			if (value != nullptr)
			{
				virtualVars.push_back({ name, value });
			}
		}

		for (unsigned int i = 0; i < instr.argCount; i++)
		{
			arg& cur = instr.arguments[i];
			if (cur.argType == argType::variable)
			{
				auto& [id, value] = virtualVars[findV(virtualVars, SCISL_CAST_STRING(cur.val.val))];
				fakeArgs.arguments[i].argType = argType::variable;
				fakeArgs.arguments[i].finalized = true;
				fakeArgs.arguments[i].val.val = value->val;
				fakeArgs.arguments[i].val.type = value->type;
				continue;
			}
			fakeArgs.arguments[i] = cur;
		}

		scislFunc target = instr.func;
		target(fakeP, fakeArgs);

		delete[] fakeArgs.arguments;
	}


	inline void invalidateVars(std::vector<precompInstr>& newProcess, std::map<std::string, value*>& evalVal)
	{
		for (auto& [id, var] : evalVal)
		{
			if (var != nullptr)
			{
				newProcess.push_back(setInstr(id, var));
			}
			delete var;
			var = nullptr;
		}
	}

	inline bool isValid(const precompInstr& i, std::map<std::string, value*>& evalVal)
	{
		for (unsigned int j = 1; j < i.instr.argCount; j++)
		{
			arg& cur = i.instr.arguments[j];
			if (cur.argType == argType::constant) continue;

			if (cur.argType == argType::interop)
			{
				return false;
			}

			value* val = evalVal.at(SCISL_CAST_STRING(cur.val.val));
			if (val == nullptr)
			{
				return false;
			}
		}
		return true;
	}

	inline void handleNoMod(precompInstr& i, std::map<std::string, value*>& evalVal)
	{
		for (unsigned int j = 0; j < i.instr.argCount; j++)
		{
			arg& cur = i.instr.arguments[j];
			if (cur.argType == argType::variable)
			{
				value* val = evalVal.at(SCISL_CAST_STRING(cur.val.val));
				if (val != nullptr)
				{
					cur.argType = argType::constant;
					delete (std::string*)(cur.val.val);
					cur.val = *val;
				}
			}
		}
	}

	//done pretty early, essentially runs the program to figure out if things can be figured out ahead of time
	void evaluateConstants(std::vector<precompInstr>& process, std::vector<std::pair<std::string, type>>& vars)
	{
		std::vector<precompInstr> newProcess;
		newProcess.reserve(process.size());

		std::map<std::string, value*> evalVal;

		for (precompInstr& i : process)
		{
			if (isFunc(i.meta, stlFuncs::noop)) //removes NOOP
			{
				delete[] i.instr.arguments;
				continue;
			}

			if ((i.meta.flags & SCISL_F_NO_JMP) == 0) //jumps
			{
				if (isFunc(i.meta, stlFuncs::cjmp)) //there is a chance a cjmp can be reduced which makes way for HUGE optimizations later
				{
					arg& c = i.instr.arguments[1];
					if (c.argType == argType::variable)
					{
						if (evalVal.contains(SCISL_CAST_STRING(c.val.val)))
						{
							value* val = evalVal.at(SCISL_CAST_STRING(c.val.val));
							if (val != nullptr)
							{
								c.argType = argType::constant;
								delete (std::string*)(c.val.val);
								c.val = *val;
								if (c.val == 0)
								{
									newProcess.push_back(std::move(i));
									continue;
								}
							}
						}
					}
				}
				invalidateVars(newProcess, evalVal); //invalidate everything
				newProcess.push_back(std::move(i));
				continue;
			}

			if ((i.meta.flags & SCISL_F_NO_MOD) > 0) //doesn't modify anything, good!
			{
				handleNoMod(i, evalVal);
				newProcess.push_back(std::move(i));
				continue;
			}


			bool simulatable = (i.meta.flags & SCISL_F_SIMABLE);
			if (!simulatable && !isSTLfunc((stlFuncs)(i.meta.funcID))) //non simulatble registered funcs (that modify)
			{
				invalidateVars(newProcess, evalVal);
				newProcess.push_back(std::move(i));
				continue;
			}

			arg& modified = i.instr.arguments[0];
			if (modified.argType != argType::variable) //the modified var is registered
			{
				for (unsigned int j = 1; j < i.instr.argCount; j++) //apply our knowledge to the args
				{
					arg& cur = i.instr.arguments[j];

					if (cur.argType == argType::variable)
					{
						value* val = evalVal.at(SCISL_CAST_STRING(cur.val.val));
						if (val != nullptr)
						{
							cur.argType = argType::constant;
							delete (std::string*)(cur.val.val);
							cur.val = *val;
						}
					}
				}
				newProcess.push_back(std::move(i));
				continue;
			}

			if (!evalVal.contains(SCISL_CAST_STRING(modified.val.val))) //initialization
			{
				type t = inferType(i, i.instr.arguments[1].val.type);
				value* n = new value(createTemporary(t));
				evalVal.insert({ SCISL_CAST_STRING(modified.val.val), n });
			}

			// valid means that the modified variable can be determined at compile time
			bool valid = isValid(i, evalVal);

			value*& v = evalVal.at(SCISL_CAST_STRING(modified.val.val));
			if (valid)
			{
				if (v == nullptr)
				{
					type t = inferType(i, i.instr.arguments[1].val.type);
					if (t != type::error)
					{
						v = new value(createTemporary(t));
					}
					else
					{
						simulatable = false;
					}
				}

				if (simulatable)
				{
					simulate(evalVal, i.instr);
					delete[] i.instr.arguments;
					continue;
				}
			}
			else if (v != nullptr)
			{
				if (!simulatable)
				{
					newProcess.push_back(setInstr(SCISL_CAST_STRING(modified.val.val), v));
				}

				delete v;
				v = nullptr;
			}

			newProcess.push_back(std::move(i));
		}

		process = std::move(newProcess);
		for (precompInstr& i : process) //run peephole on remaining instructions
		{
			scislPeephole& peep = i.meta.peep;
			if (peep != nullptr) peep(i);
		}

		for (auto& [id, var] : evalVal) //clear eval memory
		{
			delete var;
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
				delete[] i.instr.arguments;
			}
		}
		instructions = std::move(remaining);
	}

	void removeUnusedVars(std::vector<precompInstr>& instructions)
	{
		std::map<std::string, unsigned int> varCount;
		std::vector<precompInstr> remaining;
		remaining.reserve(instructions.size());

		for (precompInstr& i : instructions)
		{
			for (unsigned int j = 0; j < i.instr.argCount; j++)
			{
				arg& cur = i.instr.arguments[j];
				if (cur.argType == argType::variable)
				{
					if (varCount.contains(SCISL_CAST_STRING(cur.val.val)))
					{
						varCount.at(SCISL_CAST_STRING(cur.val.val)) += 1;
					}
					else
					{
						varCount.insert({ SCISL_CAST_STRING(cur.val.val), 1 });
					}
				}
			}
		}

		for (precompInstr& i : instructions)
		{
			if (i.meta.flags & SCISL_F_NJIS)
			{
				arg& cur = i.instr.arguments[0];
				if (cur.argType == argType::variable)
				{
					if (varCount.at(SCISL_CAST_STRING(cur.val.val)) == 1)
					{
						delete[] i.instr.arguments;
						continue;
					}
				}
			}
			remaining.push_back(std::move(i));
		}
		instructions = std::move(remaining);
	}

	void removeUnusedLabels(std::vector<precompInstr>& instructions)
	{
		std::map<std::string, unsigned int> usedLabels;
		std::vector<precompInstr> remaining;
		remaining.reserve(instructions.size());

		for (unsigned int i = 0; i < instructions.size(); i++)
		{
			if (isFunc(instructions[i].meta, stlFuncs::jmp) || isFunc(instructions[i].meta, stlFuncs::cjmp))
			{
				arg& label = instructions[i].instr.arguments[0];
				if (!usedLabels.contains(SCISL_CAST_STRING(label.val.val)))
				{
					unsigned int loc = findLabel(instructions, SCISL_CAST_STRING(label.val.val), stlFuncs::label);

					if (i + 1 == loc)
					{
						toNOOP(instructions[i]);
					}
					else
					{
						usedLabels.insert({ SCISL_CAST_STRING(label.val.val), loc });
					}
				}
				else
				{
					unsigned int loc = usedLabels.at(SCISL_CAST_STRING(label.val.val));
					if (i + 1 == loc)
					{
						toNOOP(instructions[i]);
					}
				}
			}
		}

		for (precompInstr& i : instructions)
		{
			if (isFunc(i.meta, stlFuncs::label))
			{
				arg& label = i.instr.arguments[0];
				if (usedLabels.contains(SCISL_CAST_STRING(label.val.val)))
				{
					remaining.push_back(std::move(i));
				}
			}
			else
			{
				remaining.push_back(std::move(i));
			}
		}
		instructions = std::move(remaining);
	}

	template <typename T>
	inline bool vecContains(std::vector<T>& vec, T& obj)
	{
		for (T& o : vec)
		{
			if (obj == o)
			{
				return true;
			}
		}
		return false;
	}

	void exploreBranch(std::vector<precompInstr>& instructions, std::vector<unsigned int>& reachedInstructions, unsigned int branchIdx)
	{
		while (branchIdx < instructions.size())
		{
			if (!vecContains(reachedInstructions, branchIdx))
			{
				precompInstr& cur = instructions[branchIdx];
				reachedInstructions.push_back(branchIdx);
				switch (cur.meta.funcID)
				{
				case stlFuncs::jmp:
					{
					std::string& v = SCISL_CAST_STRING(cur.instr.arguments[0].val.val);
					const unsigned int idx = findLabel(instructions, v, stlFuncs::label);
					return exploreBranch(instructions, reachedInstructions, idx);
					}
				case stlFuncs::cjmp:
					{
					std::string& v = SCISL_CAST_STRING(cur.instr.arguments[0].val.val);
					const unsigned int idx = findLabel(instructions, v, stlFuncs::label);
					exploreBranch(instructions, reachedInstructions, idx);
					break;
					}
				case stlFuncs::call:
					{
					std::string& v = SCISL_CAST_STRING(cur.instr.arguments[0].val.val);
					const unsigned int idx = findLabel(instructions, v, stlFuncs::def);
					exploreBranch(instructions, reachedInstructions, idx + 1);
					break;
					}
				case stlFuncs::def:
					{
					const unsigned int idx = findBlockEnd(instructions, branchIdx);
					branchIdx = idx;
					break;
					}
				case stlFuncs::breakp:
					{
					const unsigned int block = findBlockEnd(instructions, branchIdx);
					if (block != instructions.size()) return; //return if in a block
					break;
					}
				case stlFuncs::exit:
				case stlFuncs::blockend:
					return;
				default:
					if (cur.meta.flags & SCISL_F_BLOCK)
					{
						exploreBranch(instructions, reachedInstructions, branchIdx);
						const unsigned int idx = findBlockEnd(instructions, branchIdx);
						branchIdx = idx;
					}
				}

				branchIdx++;
			}
			else
			{
				return;
			}
		}
	}

	void removeUnreachableCode(std::vector<precompInstr>& instructions)
	{
		std::vector<precompInstr> remaining;
		std::vector<unsigned int> reachedInstructions;
		reachedInstructions.reserve(instructions.size());

		exploreBranch(instructions, reachedInstructions, 0);

		remaining.reserve(reachedInstructions.size());
		for (unsigned int i = 0; i < instructions.size(); i++)
		{
			if (vecContains(reachedInstructions, i))
			{
				remaining.push_back(std::move(instructions[i]));
			}
		}

		instructions = std::move(remaining);
	}
}