#include "stl.h"

#include <iostream>

#pragma warning(push)
#pragma warning(disable : 4244)
namespace scisl
{
	inline value createTemporary(type tipe)
	{
		value opt;
		opt.type = tipe;
		opt.isTemporary = true;
		switch (tipe)
		{
		case (type::string):
			opt.val = new std::string("");
			break;
		case (type::integer):
			opt.val = new SCISL_INT_PRECISION(0);
			break;
		case (type::floating):
			opt.val = new SCISL_FLOAT_PRECISION(0);
			break;
		default:
			opt.val = nullptr;
		}
		return opt;
	}

	stlFuncs strToFuncID(const std::string& str)
	{
		for (unsigned short i = 0; i < (unsigned short)(stlFuncs::stlFuncCount); i++)
		{
			if (str == stlFuncIDs[i])
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

	//veriadic args
	void mult(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value prod = createTemporary(cur.type);
		prod = args.arguments[1].getValue(process);

		for (unsigned char i = 2; i < args.argCount; i++)
		{
			value to = args.arguments[i].getValue(process);
			prod *= to;
		}

		cur = prod.val;
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
	}
}
#pragma warning(pop)