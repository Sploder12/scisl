#include "stl.h"

#include <iostream>

#pragma warning(push)
#pragma warning(disable : 4244)
namespace scisl
{
	bool isSTLfunc(scislFunc fnc)
	{
		for (unsigned int i = 0; i < (unsigned short)(stlFuncs::stlFuncCount); i++)
		{
			if (fnc == stlFuncMeta[i].fnc) return true;
		}

		return false;
	}

	bool isFunc(scislFunc func, stlFuncs fnc)
	{
		return func == stlFuncMeta[(unsigned short)(fnc)].fnc;
	}

	bool isFunc(scislfuncMeta meta, stlFuncs fnc)
	{
		return meta.funcID == stlFuncMeta[(unsigned short)(fnc)].funcID;
	}

	stlFuncs strToFuncID(const std::string& str)
	{
		for (unsigned short i = 0; i < (unsigned short)(stlFuncs::stlFuncCount); i++)
		{
			if (str == stlFuncMeta[i].funcID)
			{
				return (stlFuncs)(i);
			}
		}
		return stlFuncs::stlFuncCount;
	}

	void set(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& to = args.arguments[1].getValue();

		cur = to.val;
	}

	//veriadic args
	void add(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value sum = createTemporary(cur.type); //defaults as empty

		for (unsigned char i = 1; i < args.argCount; i++)
		{
			value& to = args.arguments[i].getValue();
			sum += to;
		}

		cur = sum.val;
	}

	//veriadic args
	void adde(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();

		for (unsigned char i = 1; i < args.argCount; i++)
		{
			value& to = args.arguments[i].getValue();
			cur += to;
		}
	}

	//3 args
	void sub(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();
		value& second = args.arguments[2].getValue();

		value diff = createTemporary(cur.type);
		diff = first.val;
		diff -= second;

		cur = diff.val;
	}

	//2 args
	void sube(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();

		cur -= first;
	}

	//veriadic args
	void mult(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value prod = createTemporary(cur.type);
		prod = args.arguments[1].getValue().val;

		for (unsigned char i = 2; i < args.argCount; i++)
		{
			value& to = args.arguments[i].getValue();
			prod *= to;
		}

		cur = prod.val;
	}

	//veriadic args
	void multe(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();

		for (unsigned char i = 1; i < args.argCount; i++)
		{
			value& to = args.arguments[i].getValue();
			cur *= to;
		}
	}

	//3 args
	void div(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();
		value& second = args.arguments[2].getValue();

		value quo = createTemporary(cur.type);
		quo = first.val;
		quo /= second;
		cur = quo.val;
	}

	//2 args
	void dive(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();

		cur /= first;
	}

	//veriadic args
	void print(program& process, const args& args)
	{
		for (unsigned char i = 0; i < args.argCount; i++)
		{
			value& cur = args.arguments[i].getValue();
			switch (cur.type)
			{
			case type::string:
				std::cout << SCISL_CAST_STRING(cur.val);
				break;
			case type::integer:
				std::cout << SCISL_CAST_INT(cur.val);
				break;
			case type::floating:
				std::cout << SCISL_CAST_FLOAT(cur.val);
				break;
			default:
				break;
			}
		}
		std::cout << '\n';
	}

	void substr(program& process, const args& args)
	{
		value& c = args.arguments[0].getValue();
		value& str = args.arguments[1].getValue();
		value& start = args.arguments[2].getValue();
		value& end = args.arguments[2].getValue();
		//thank type checking
		c = SCISL_CAST_STRING(str.val).substr(SCISL_CAST_INT(start.val), SCISL_CAST_INT(end.val) - SCISL_CAST_INT(start.val));
	}

	void sstrlen(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& str = args.arguments[1].getValue();

		cur = (SCISL_INT_PRECISION)(SCISL_CAST_STRING(str.val).size());
	}

	void chrset(program& process, const args& args)
	{
		value& str = args.arguments[0].getValue();
		value& c = args.arguments[1].getValue();
		value& idx = args.arguments[2].getValue();
		//type checking guarentees this has the right types
		((std::string*)(str.val))[SCISL_CAST_INT(idx.val)] = char(SCISL_CAST_INT(c.val));
	}

	void chrat(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& str = args.arguments[1].getValue();
		value& idx = args.arguments[2].getValue();
		//type checking guarentees this has the right types
		cur = SCISL_CAST_STRING(str.val)[SCISL_CAST_INT(idx.val)];
	}

	void less(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();
		value& second = args.arguments[2].getValue();

		cur = first < second;
	}

	void great(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();
		value& second = args.arguments[2].getValue();

		cur = first > second;
	}

	void equal(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();
		value& second = args.arguments[2].getValue();

		cur = first == second;
	}

	void nequal(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();
		value& second = args.arguments[2].getValue();

		cur = first != second;
	}

	void land(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();
		value& second = args.arguments[2].getValue();

		cur = first && second;
	}

	void lor(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();
		value& second = args.arguments[2].getValue();

		cur = first || second;
	}

	void band(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();

		cur &= first;
	}

	void bor(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();

		cur |= first;
	}

	void bxor(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();

		cur ^= first;
	}

	void lshift(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();

		cur <<= first;
	}

	void rshift(program& process, const args& args)
	{
		value& cur = args.arguments[0].getValue();
		value& first = args.arguments[1].getValue();

		cur >>= first;
	}

	void jmp(program& process, const args& args)
	{
		unsigned int line = *(unsigned int*)(args.arguments[0].val.val);
		process.curInstr = line;
	}

	void cjmp(program& process, const args& args)
	{
		value& cur = args.arguments[1].getValue();
		switch (cur.type)
		{
		case type::integer:
			if (SCISL_CAST_INT(cur.val) > 0)
			{
				unsigned int line = *(unsigned int*)(args.arguments[0].val.val);
				process.curInstr = line;
			}
			break;
		case type::floating:
			if (SCISL_CAST_FLOAT(cur.val) > 0)
			{
				unsigned int line = *(unsigned int*)(args.arguments[0].val.val);
				process.curInstr = line;
			}
			break;
		}
	}

	void end(program& process, const args& args)
	{
		value& v = args.arguments[0].getValue();
		process.curInstr = process.instructions.size();
		process.retVal = SCISL_CAST_INT(v.val);
	}

	inline void toNOOP(precompInstr& instruct)
	{
		instruct.instr.func = nullptr;
		delete[] instruct.instr.arguments.arguments;
		instruct.instr.arguments.arguments = nullptr;
		instruct.instr.arguments.argCount = 0;
		instruct.meta = stlFuncMeta[(unsigned short)(stlFuncs::noop)];
	}

	//Expects the instruction to already have 2 args, 1st is the modified, 2nd is the value
	inline void toSET(precompInstr& instruct)
	{
		instruct.meta = stlFuncMeta[(unsigned short)(stlFuncs::set)];
		instruct.instr.func = instruct.meta.fnc;
		setPeep(instruct);
	}

	void plusEq(value& first, value& other)
	{
		first += other;
	}

	void minusEq(value& first, value& other)
	{
		first -= other;
	}

	void multEq(value& first, value& other)
	{
		first *= other;
	}

	void divEq(value& first, value& other)
	{
		first /= other;
	}

	void combineConsts(precompInstr& instruct, unsigned short startIDX, void (*func)(value&, value&))
	{
		value firstStr;
		firstStr.type = type::string;
		value firstInt;
		firstInt.type = type::integer;
		value firstFloat;
		firstFloat.type = type::floating;
		std::vector<arg> realArgs;
		for (unsigned int i = 0; i < startIDX; i++)
		{
			realArgs.push_back(std::move(instruct.instr.arguments.arguments[i]));
		}

		for (unsigned int i = startIDX; i < instruct.instr.arguments.argCount; i++)
		{
			arg& cur = instruct.instr.arguments.arguments[i];
			if (cur.argType == argType::constant)
			{
				switch (cur.val.type)
				{
				case type::string:
				{
					if (firstStr.val == nullptr)
					{
						firstStr.val = cur.val.val;
						realArgs.push_back(std::move(cur));
						break;
					}

					func(firstStr, cur.getValue());
					
					break;
				}
				case type::integer:
				{
					if (firstInt.val == nullptr)
					{
						firstInt.val = cur.val.val;
						realArgs.push_back(std::move(cur));
						break;
					}

					func(firstInt, cur.getValue());
					break;
				}
				case type::floating:
				{
					if (firstFloat.val == nullptr)
					{
						firstFloat.val = cur.val.val;
						realArgs.push_back(std::move(cur));
						break;
					}

					func(firstFloat, cur.getValue());
					break;
				}
				default:
					break;
				}
				continue;
			}
			realArgs.push_back(std::move(cur));
		}

		delete[] instruct.instr.arguments.arguments;
		instruct.instr.arguments.argCount = realArgs.size();
		instruct.instr.arguments.arguments = new arg[realArgs.size()];
		for (unsigned int i = 0; i < realArgs.size(); i++)
		{
			instruct.instr.arguments.arguments[i] = std::move(realArgs[i]);
		}
		firstStr.val = nullptr;
		firstInt.val = nullptr;
		firstFloat.val = nullptr;
	}

	void removeIdentity(precompInstr& instruct, unsigned short startIDX, SCISL_INT_PRECISION identityVal)
	{
		std::vector<arg> realArgs;
		for (unsigned int i = 0; i < startIDX; i++)
		{
			realArgs.push_back(std::move(instruct.instr.arguments.arguments[i]));
		}

		for (unsigned int i = startIDX; i < instruct.instr.arguments.argCount; i++)
		{
			arg& cur = instruct.instr.arguments.arguments[i];
			if (cur.argType == argType::constant)
			{
				switch (cur.val.type)
				{
				case type::string:
					realArgs.push_back(std::move(cur));
					break;
				case type::floating:
				{
					SCISL_FLOAT_PRECISION val = SCISL_CAST_FLOAT(instruct.instr.arguments.arguments[i].val.val);
					if (val != identityVal)
					{
						realArgs.push_back(std::move(cur));
					}
					break;
				}
				case type::integer:
				{
					SCISL_INT_PRECISION val = SCISL_CAST_INT(instruct.instr.arguments.arguments[i].val.val);
					if (val != identityVal)
					{
						realArgs.push_back(std::move(cur));
					}
					break;
				}
				}
				continue;
			}
			realArgs.push_back(std::move(cur));
		}

		delete[] instruct.instr.arguments.arguments;
		instruct.instr.arguments.argCount = realArgs.size();
		instruct.instr.arguments.arguments = new arg[realArgs.size()];
		for (unsigned int i = 0; i < realArgs.size(); i++)
		{
			instruct.instr.arguments.arguments[i] = std::move(realArgs[i]);
		}
	}

	inline bool settingConst(precompInstr& instruct)
	{
		if (instruct.instr.arguments.arguments[0].argType == argType::constant)
		{ //setting a constant
			toNOOP(instruct);
			return true;
		}
		return false;
	}

	void setPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
		
		arg& first = instruct.instr.arguments.arguments[0];
		arg& second = instruct.instr.arguments.arguments[1];
		if (first.argType == argType::variable && second.argType == argType::variable)
		{
			if (SCISL_CAST_STRING(first.val.val) == SCISL_CAST_STRING(second.val.val))
			{
				toNOOP(instruct);
			}
		}
	}


	void addPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		combineConsts(instruct, 1, plusEq);

		removeIdentity(instruct, 1, 0);

		if (instruct.instr.arguments.argCount == 2)
		{
			toSET(instruct);
			return;
		}
	}

	void addePeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		combineConsts(instruct, 1, plusEq);

		removeIdentity(instruct, 1, 0);

		if (instruct.instr.arguments.argCount == 1)
		{
			toNOOP(instruct);
			return;
		}
	}

	void subPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		removeIdentity(instruct, 2, 0);

		if (instruct.instr.arguments.argCount == 2)
		{
			toSET(instruct);
			return;
		}
	}

	void subePeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		removeIdentity(instruct, 1, 0);

		if (instruct.instr.arguments.argCount == 1)
		{
			toNOOP(instruct);
			return;
		}
	}

	void multPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		combineConsts(instruct, 1, multEq);

		removeIdentity(instruct, 1, 1);

		if (instruct.instr.arguments.argCount == 2)
		{
			toSET(instruct);
			return;
		}
	}

	void multePeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		combineConsts(instruct, 1, multEq);

		removeIdentity(instruct, 1, 1);

		if (instruct.instr.arguments.argCount == 1)
		{
			toNOOP(instruct);
			return;
		}
	}

	void divPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		removeIdentity(instruct, 2, 1);

		if (instruct.instr.arguments.argCount == 2)
		{
			toSET(instruct);
			return;
		}
	}

	void divePeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		removeIdentity(instruct, 1, 1);

		if (instruct.instr.arguments.argCount == 1)
		{
			toNOOP(instruct);
			return;
		}
	}

	void printPeep(precompInstr& instruct)
	{
		combineConsts(instruct, 0, plusEq);
	}

	void substrPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void strlenPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void chrsetPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void chratPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void lessPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void greatPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void equalPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void nequalPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void landPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void lorPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void bandPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void borPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void bxorPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void lshiftPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void rshiftPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void cjmpPeep(precompInstr& instruct)
	{
		arg& cond = instruct.instr.arguments.arguments[1];
		if (cond.argType == argType::constant)
		{
			value& v = cond.val;
			switch (v.type)
			{
			case type::integer:
				if (SCISL_CAST_INT(v.val) > 0)
				{
					args t;
					t.argCount = 1;
					t.arguments = new arg[1];
					t.arguments[0] = instruct.instr.arguments.arguments[0];
					instruct.meta = stlFuncMeta[(unsigned short)(stlFuncs::jmp)];
					instruct.instr.func = instruct.meta.fnc;
					delete[] instruct.instr.arguments.arguments;
					instruct.instr.arguments = t;
				}
				else
				{
					toNOOP(instruct);
				}
				break;
			case type::floating:
				if (SCISL_CAST_FLOAT(v.val) > 0)
				{
					args t;
					t.argCount = 1;
					t.arguments = new arg[1];
					t.arguments[0] = instruct.instr.arguments.arguments[0];
					instruct.meta = stlFuncMeta[(unsigned short)(stlFuncs::jmp)];
					instruct.instr.func = instruct.meta.fnc;
					delete[] instruct.instr.arguments.arguments;
					instruct.instr.arguments = t;
				}
				else
				{
					toNOOP(instruct);
				}
				break;
			}
		}
	}
}
#pragma warning(pop)