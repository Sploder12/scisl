#include "stl.h"

//this is horribly lazy of me and very dangerous but makes me waste less time
#define DO_OP(first, second, op)\
	switch (first.type)\
	{\
		case type::string:\
			switch (second.type)\
			{\
			case type::string:\
				SCISL_CAST_STRING(first.val) op SCISL_CAST_STRING(second.val);\
				break;\
			case type::integer:\
				SCISL_CAST_STRING(first.val) op std::to_string(SCISL_CAST_INT(second.val));\
				break;\
			case type::floating:\
				SCISL_CAST_STRING(first.val) op std::to_string(SCISL_CAST_FLOAT(second.val));\
				break;\
			default:\
				break;\
			}\
			break;\
		case type::integer:\
			switch (second.type)\
			{\
			case type::string:\
				SCISL_CAST_INT(first.val) op (SCISL_INT_PRECISION)std::stol(SCISL_CAST_STRING(second.val));\
				break;\
			case type::integer:\
				SCISL_CAST_INT(first.val) op SCISL_CAST_INT(second.val);\
				break;\
			case type::floating:\
				SCISL_CAST_INT(first.val) op SCISL_CAST_FLOAT(second.val);\
				break;\
			default:\
				break;\
			}\
			break;\
		case type::floating:\
			switch (second.type)\
			{\
			case type::string:\
				SCISL_CAST_FLOAT(first.val) op (SCISL_FLOAT_PRECISION)std::stod(SCISL_CAST_STRING(second.val));\
				break;\
			case type::integer:\
				SCISL_CAST_FLOAT(first.val) op SCISL_CAST_INT(second.val);\
				break;\
			case type::floating:\
				SCISL_CAST_FLOAT(first.val) op SCISL_CAST_FLOAT(second.val);\
				break;\
			default:\
				break;\
			}\
			break;\
		default:\
			break;\
	}\

#define DO_OP_NSTR(first, second, op)\
switch (first.type)\
{\
case type::string:\
	break;\
case type::integer:\
		switch (second.type)\
		{\
		case type::string:\
			break; \
		case type::integer:\
			SCISL_CAST_INT(first.val) op SCISL_CAST_INT(second.val); \
			break; \
		case type::floating:\
			SCISL_CAST_INT(first.val) op SCISL_CAST_FLOAT(second.val); \
			break; \
		default:\
			break; \
		}\
			break; \
case type::floating:\
			switch (second.type)\
			{\
			case type::string:\
				break; \
			case type::integer:\
				SCISL_CAST_FLOAT(first.val) op SCISL_CAST_INT(second.val); \
				break; \
			case type::floating:\
				SCISL_CAST_FLOAT(first.val) op SCISL_CAST_FLOAT(second.val); \
				break; \
			default:\
				break; \
			}\
				break; \
default:\
	break; \
}\

#include <iostream>

#pragma warning(push)
#pragma warning(disable : 4244)
namespace scisl
{
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
		
		DO_OP(cur, to, =)
	}

	//veriadic args
	void add(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value sum = args.arguments[1].getValue(process);
		for (unsigned char i = 2; i < args.argCount; i++)
		{
			value to = args.arguments[i].getValue(process);
			DO_OP(sum, to, +=)
		}
	}

	//3 args
	void sub(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value first = args.arguments[1].getValue(process);
		value second = args.arguments[2].getValue(process);

		DO_OP_NSTR(first, second, -=);
		DO_OP(cur, first, =);
	}

	//veriadic args
	void mult(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);

		for (unsigned char i = 1; i < args.argCount; i++)
		{
			value to = args.arguments[i].getValue(process);
			DO_OP_NSTR(cur, to, *=)
		}
	}

	//3 args
	void div(program& process, const args& args)
	{
		value cur = args.arguments[0].getValue(process);
		value first = args.arguments[1].getValue(process);
		value second = args.arguments[2].getValue(process);

		DO_OP_NSTR(first, second, /=);
		DO_OP(cur, first, =);
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