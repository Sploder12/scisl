#include "stl.h"

#include <iostream>

#pragma warning(push)
#pragma warning(disable : 4244)
namespace scisl
{
	bool isSTLfunc(stlFuncs fnc)
	{
		return fnc != stlFuncs::stlFuncCount;
	}

	bool isFunc(scislFunc func, stlFuncs fnc)
	{
		return func == stlFuncMeta[(unsigned short)(fnc)].fnc;
	}

	stlFuncs strToFuncID(const std::string& str)
	{
		for (unsigned short i = 0; i < (unsigned short)(stlFuncs::stlFuncCount); i++)
		{
			if (str == stlFuncMeta[i].funcName)
			{
				return (stlFuncs)(i);
			}
		}

		for (unsigned short i = 0; i < (unsigned short)(stlFuncs::stlFuncCount); i++)
		{
			const std::vector<std::string>& aliases = funcAliases[i];
			for (const std::string& s : aliases)
			{
				if (str == s)
				{
					return (stlFuncs)(i);
				}
			}
		}
		return stlFuncs::stlFuncCount;
	}

	void set(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& to = instr.arguments[1].getValue();
		cur = to.val;
	}

	//veriadic args
	void add(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value sum = createTemporary(cur.type); //defaults as empty

		for (unsigned char i = 1; i < instr.argCount; i++)
		{
			value& to = instr.arguments[i].getValue();
			sum += to;
		}

		cur = sum.val;
	}

	//veriadic args
	void adde(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();

		for (unsigned char i = 1; i < instr.argCount; i++)
		{
			value& to = instr.arguments[i].getValue();
			cur += to;
		}
	}

	//3 args
	void sub(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();
		value& second = instr.arguments[2].getValue();

		value diff = createTemporary(cur.type);
		diff = first.val;
		diff -= second;

		cur = diff.val;
	}

	//2 args
	void sube(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();

		cur -= first;
	}

	//veriadic args
	void mult(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value prod = createTemporary(cur.type);
		prod = instr.arguments[1].getValue().val;

		for (unsigned char i = 2; i < instr.argCount; i++)
		{
			value& to = instr.arguments[i].getValue();
			prod *= to;
		}

		cur = prod.val;
	}

	//veriadic args
	void multe(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();

		for (unsigned char i = 1; i < instr.argCount; i++)
		{
			value& to = instr.arguments[i].getValue();
			cur *= to;
		}
	}

	//3 args
	void div(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();
		value& second = instr.arguments[2].getValue();

		value quo = createTemporary(cur.type);
		quo = first.val;
		quo /= second;
		cur = quo.val;
	}

	//2 args
	void dive(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();

		cur /= first;
	}

	//veriadic args
	void print(program& process, const instruction& instr)
	{
		for (unsigned char i = 0; i < instr.argCount; i++)
		{
			value& cur = instr.arguments[i].getValue();
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

	void substr(program& process, const instruction& instr)
	{
		value& c = instr.arguments[0].getValue();
		value& str = instr.arguments[1].getValue();
		value& start = instr.arguments[2].getValue();
		value& end = instr.arguments[2].getValue();
		//thank type checking
		c = SCISL_CAST_STRING(str.val).substr(SCISL_CAST_INT(start.val), SCISL_CAST_INT(end.val) - SCISL_CAST_INT(start.val));
	}

	void sstrlen(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& str = instr.arguments[1].getValue();

		cur = (SCISL_INT_PRECISION)(SCISL_CAST_STRING(str.val).size());
	}

	void chrset(program& process, const instruction& instr)
	{
		value& str = instr.arguments[0].getValue();
		value& c = instr.arguments[1].getValue();
		value& idx = instr.arguments[2].getValue();
		//type checking guarentees this has the right types
		SCISL_CAST_STRING(str.val)[SCISL_CAST_INT(idx.val)] = char(SCISL_CAST_INT(c.val));
	}

	void chrat(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& str = instr.arguments[1].getValue();
		value& idx = instr.arguments[2].getValue();
		//type checking guarentees this has the right types
		cur = SCISL_CAST_STRING(str.val)[SCISL_CAST_INT(idx.val)];
	}

	void less(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();
		value& second = instr.arguments[2].getValue();

		cur = first < second;
	}

	void great(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();
		value& second = instr.arguments[2].getValue();

		cur = first > second;
	}

	void equal(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();
		for (unsigned char i = 2; i < instr.argCount; i++)
		{
			value& next = instr.arguments[i].getValue();
			if (first != next)
			{
				cur = false;
				return;
			}
		}

		cur = true;
	}

	void nequal(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();
		value& second = instr.arguments[2].getValue();

		cur = first != second;
	}

	void land(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();
		value& second = instr.arguments[2].getValue();

		cur = first && second;
	}

	void lor(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();
		value& second = instr.arguments[2].getValue();

		cur = first || second;
	}

	void band(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();

		cur &= first;
	}

	void bor(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();

		cur |= first;
	}

	void bxor(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();

		cur ^= first;
	}

	void lshift(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();

		cur <<= first;
	}

	void rshift(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[0].getValue();
		value& first = instr.arguments[1].getValue();

		cur >>= first;
	}

	void jmp(program& process, const instruction& instr)
	{
		unsigned int line = *(unsigned int*)(instr.arguments[0].val.val);
		process.curInstr = line;
	}

	void cjmp(program& process, const instruction& instr)
	{
		value& cur = instr.arguments[1].getValue();
		switch (cur.type)
		{
		case type::integer:
			if (SCISL_CAST_INT(cur.val) > 0)
			{
				unsigned int line = *(unsigned int*)(instr.arguments[0].val.val);
				process.curInstr = line;
			}
			break;
		case type::floating:
			if (SCISL_CAST_FLOAT(cur.val) > 0)
			{
				unsigned int line = *(unsigned int*)(instr.arguments[0].val.val);
				process.curInstr = line;
			}
			break;
		}
	}
	
	void blockend(program& process, const instruction& instr)
	{
		if (!process.callStack.empty())
		{
			process.curInstr = process.callStack.top();
			process.callStack.pop();
		}
	}

	void def(program& process, const instruction& instr) //acts like a label but jumps to the end if not called
	{
		unsigned int line = *(unsigned int*)(instr.arguments[0].val.val);
		process.curInstr = line;
	}

	void call(program& process, const instruction& instr)
	{
		process.callStack.push(process.curInstr);
		unsigned int line = *(unsigned int*)(instr.arguments[0].val.val);
		process.curInstr = line + 1;
	}

	void end(program& process, const instruction& instr)
	{
		value& v = instr.arguments[0].getValue();
		process.curInstr = (unsigned int)(process.instructions.size());
		process.retVal = SCISL_CAST_INT(v.val);
		process.callStack = {};
	}

	void breakp(program& process, const instruction& instr)
	{
		if (process.callStack.empty())
		{
			value& v = instr.arguments[0].getValue();
			process.broke = true;
			process.retVal = SCISL_CAST_INT(v.val);
		}
		else //we're inside a function
		{
			process.curInstr = process.callStack.top();
			process.callStack.pop();
		}
	}
}
#pragma warning(pop)