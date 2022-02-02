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
		value cur = args.arguments[0].getValue(process);
		value to = args.arguments[1].getValue(process);

		cur = to.val;
	}

	//veriadic args
	void add(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value sum = createTemporary(cur.type); //defaults as empty

		for (unsigned char i = 1; i < args.argCount; i++)
		{
			value to = args.arguments[i].getValue(process);
			sum += to;
		}
		
		cur = sum.val;
	}

	//veriadic args
	void adde(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);

		for (unsigned char i = 1; i < args.argCount; i++)
		{
			value to = args.arguments[i].getValue(process);
			cur += to;
		}
	}

	//3 args
	void sub(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value first = args.arguments[1].getValue(process);
		value second = args.arguments[2].getValue(process);

		value diff = createTemporary(cur.type);
		diff = first.val;
		diff -= second;
		cur = diff.val;
	}

	//2 args
	void sube(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value first = args.arguments[1].getValue(process);

		cur -= first;
	}

	//veriadic args
	void mult(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value prod = createTemporary(cur.type);
		prod = args.arguments[1].getValue(process).val;

		for (unsigned char i = 2; i < args.argCount; i++)
		{
			value to = args.arguments[i].getValue(process);
			prod *= to;
		}

		cur = prod.val;
	}

	//veriadic args
	void multe(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);

		for (unsigned char i = 1; i < args.argCount; i++)
		{
			value to = args.arguments[i].getValue(process);
			cur *= to;
		}
	}

	//3 args
	void div(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value first = args.arguments[1].getValue(process);
		value second = args.arguments[2].getValue(process);

		value quo = createTemporary(cur.type);
		quo = first.val;
		quo /= second;
		cur = quo.val;
	}

	//2 args
	void dive(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value first = args.arguments[1].getValue(process);

		cur /= first;
	}

	//veriadic args
	void print(program& process, const args& args)
	{
		for (unsigned char i = 0; i < args.argCount; i++)
		{
			value cur = args.arguments[i].getValue(process);
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

	void less(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value first = args.arguments[1].getValue(process);
		value second = args.arguments[2].getValue(process);

		cur = createTemporary(cur.type, first < second).val;
	}

	void great(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value first = args.arguments[1].getValue(process);
		value second = args.arguments[2].getValue(process);

		cur = createTemporary(cur.type, first > second);
	}

	void equal(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value first = args.arguments[1].getValue(process);
		value second = args.arguments[2].getValue(process);

		cur = createTemporary(cur.type, first == second).val;
	}

	void nequal(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value first = args.arguments[1].getValue(process);
		value second = args.arguments[2].getValue(process);

		cur = createTemporary(cur.type, first != second).val;
	}

	void jmp(program& process, const args& args)
	{
		unsigned int line = *(unsigned int*)(args.arguments[0].val);
		process.curInstr = line;
	}

	void cjmp(program& process, const args& args)
	{
		value cur = args.arguments[1].getValue(process);
		if (SCISL_CAST_INT(cur.val) > 0)
		{
			unsigned int line = *(unsigned int*)(args.arguments[0].val);
			process.curInstr = line;
		}
	}

	inline void toNOOP(precompInstr& instruct)
	{
		instruct.instr.func = nullptr;
		delete[] instruct.instr.arguments.arguments;
		instruct.instr.arguments.arguments = nullptr;
		instruct.instr.arguments.argCount = 0;
		instruct.meta = stlFuncMeta[(unsigned short)(stlFuncs::noop)];
	}

	inline argType getAType(precompInstr& instruct, unsigned short argIdx)
	{
		return instruct.instr.arguments.arguments[argIdx].argType;
	}

	inline type getVType(precompInstr& instruct, unsigned short argIdx)
	{
		return instruct.instr.arguments.arguments[argIdx].type;
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

	void removeErrArgs(precompInstr& instruct)
	{
		std::vector<arg> remaining;

		for (unsigned int i = 0; i < instruct.instr.arguments.argCount; i++)
		{
			if (getAType(instruct, i) != argType::error)
			{
				remaining.push_back(std::move(instruct.instr.arguments.arguments[i]));
				continue;
			}
			instruct.instr.arguments.arguments[i].argType = argType::constant;
		}

		if (remaining.size() != instruct.instr.arguments.argCount)
		{
			delete[] instruct.instr.arguments.arguments;
			instruct.instr.arguments.arguments = new arg[remaining.size()];
			instruct.instr.arguments.argCount = remaining.size();
		}

		for (unsigned int i = 0; i < instruct.instr.arguments.argCount; i++)
		{
			instruct.instr.arguments.arguments[i] = std::move(remaining[i]);
		}
	}

	void combineConsts(precompInstr& instruct, unsigned short startIDX, void (*func)(value&, value&))
	{
		value firstStr;
		firstStr.type = type::string;
		value firstInt;
		firstInt.type = type::integer;
		value firstFloat;
		firstFloat.type = type::floating;
		for (unsigned int i = startIDX; i < instruct.instr.arguments.argCount; i++)
		{
			if (getAType(instruct, i) == argType::constant)
			{
				switch (getVType(instruct, i))
				{
				case type::string:
				{
					if (firstStr.val == nullptr)
					{
						firstStr.val = instruct.instr.arguments.arguments[i].val;
						break;
					}

					value tmp = createTemporary(type::string);
					tmp = instruct.instr.arguments.arguments[i].val;
					func(firstStr, tmp);
					instruct.instr.arguments.arguments[i].argType = argType::error; //mark deleted
					break;
				}
				case type::integer:
				{
					if (firstInt.val == nullptr)
					{
						firstInt.val = instruct.instr.arguments.arguments[i].val;
						break;
					}

					value tmp = createTemporary(type::integer);
					tmp = instruct.instr.arguments.arguments[i].val;
					func(firstInt, tmp);
					instruct.instr.arguments.arguments[i].argType = argType::error; //mark deleted
					break;
				}
				case type::floating:
				{
					if (firstFloat.val == nullptr)
					{
						firstFloat.val = instruct.instr.arguments.arguments[i].val;
						break;
					}

					value tmp = createTemporary(type::floating);
					tmp = instruct.instr.arguments.arguments[i].val;
					func(firstFloat, tmp);
					instruct.instr.arguments.arguments[i].argType = argType::error; //mark deleted
					break;
				}
				default:
					break;
				}
			}
		}
		removeErrArgs(instruct);
	}

	void removeIdentity(precompInstr& instruct, unsigned short startIDX, SCISL_INT_PRECISION identityVal)
	{

		for (unsigned int i = 0; i < instruct.instr.arguments.argCount; i++)
		{
			if (getAType(instruct, i) == argType::constant)
			{
				switch (getVType(instruct, i))
				{
				case type::string:
					break;
				case type::floating:
				{
					SCISL_FLOAT_PRECISION val = SCISL_CAST_FLOAT(instruct.instr.arguments.arguments[i].val);
					if (val == identityVal)
					{
						instruct.instr.arguments.arguments[i].argType = argType::error;
					}
					break;
				}
				case type::integer:
				{
					SCISL_INT_PRECISION val = SCISL_CAST_INT(instruct.instr.arguments.arguments[i].val);
					if (val == identityVal)
					{
						instruct.instr.arguments.arguments[i].argType = argType::error;
					}
					break;
				}
				}
			}
		}

		removeErrArgs(instruct);
	}

	inline bool settingConst(precompInstr& instruct)
	{
		if (getAType(instruct, 0) == argType::constant)
		{ //setting a constant
			toNOOP(instruct);
			return true;
		}
		return false;
	}

	void setPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
		
	}


	void addPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		combineConsts(instruct, 1, plusEq);

		removeIdentity(instruct, 1, 0);
	}

	void addePeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		combineConsts(instruct, 1, plusEq);

		removeIdentity(instruct, 1, 0);
	}

	void subPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

	}

	void subePeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void multPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		combineConsts(instruct, 1, multEq);

		removeIdentity(instruct, 1, 1);
	}

	void multePeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;

		combineConsts(instruct, 1, multEq);

		removeIdentity(instruct, 1, 1);
	}

	void divPeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void divePeep(precompInstr& instruct)
	{
		if (settingConst(instruct)) return;
	}

	void printPeep(precompInstr& instruct)
	{
		combineConsts(instruct, 0, plusEq);
	}

	void lessPeep(precompInstr& instruct)
	{

	}

	void greatPeep(precompInstr& instruct)
	{
	
	}

	void equalPeep(precompInstr& instruct)
	{

	}

	void nequalPeep(precompInstr& instruct)
	{

	}

	void cjmpPeep(precompInstr& instruct)
	{

	}
}
#pragma warning(pop)