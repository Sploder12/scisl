#include "stlOptimizations.h"

#include "stl.h"

namespace scisl
{
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
		instruct.instr.arguments.argCount = (unsigned char)(realArgs.size());
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
		instruct.instr.arguments.argCount = (unsigned char)(realArgs.size());
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
					t.arguments[0] = std::move(instruct.instr.arguments.arguments[0]);
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
					t.arguments[0] = std::move(instruct.instr.arguments.arguments[0]);
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