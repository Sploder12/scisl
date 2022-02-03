#include "args.h"

#include "common.h"

#include "tables.h"
#include "program.h"

namespace scisl
{
	arg::arg(arg& other)
	{
		this->val = nullptr;
		this->val.type = other.val.type;
		switch (argType)
		{
		case argType::constant:
		{
			switch (other.val.type)
			{
			case type::integer:
				this->val = new SCISL_INT_PRECISION(*(SCISL_INT_PRECISION*)(other.val.val));
				break;
			case type::floating:
				this->val = new SCISL_FLOAT_PRECISION(*(SCISL_FLOAT_PRECISION*)(other.val.val));
				break;
			case type::string:
				this->val = new std::string(*(std::string*)other.val.val);
				break;
			}
			break;
		}
		case argType::variable:
		case argType::interop: //copy the name
			this->val = new std::string(*(std::string*)other.val.val);
			break;
		default:
			break;
			//error
		}
	}

	arg::arg(arg&& cmove) noexcept :
		argType(std::move(cmove.argType)),
		val(std::move(cmove.val))
	{
		cmove.val.val = nullptr;
		cmove.val.type = type::error;
		cmove.argType = argType::error;
	}

	arg& arg::operator=(arg& other)
	{
		this->argType = other.argType;
		this->val.type = other.val.type;
		switch (argType)
		{
		case argType::constant:
		{
			switch (other.val.type)
			{
			case type::integer:
				this->val = new SCISL_INT_PRECISION(*(SCISL_INT_PRECISION*)(other.val.val));
				break;
			case type::floating:
				this->val = new SCISL_FLOAT_PRECISION(*(SCISL_FLOAT_PRECISION*)(other.val.val));
				break;
			case type::string:
				this->val = new std::string(*(std::string*)other.val.val);
				break;
			}
			break;
		}
		case argType::variable:
		case argType::interop: //copy the name
			this->val = new std::string(*(std::string*)other.val.val);
			break;
		default:
			break;
			//error
		}
		return *this;
	}

	arg& arg::operator=(arg&& cmove) noexcept
	{
		std::swap(this->argType, cmove.argType);
		std::swap(this->val, cmove.val);

		return *this;
	}

	value& arg::getValue(program& process)
	{

		switch (argType)
		{
		case argType::interop:
		{
			auto& vtable = getVarTable();
			return *vtable.at(*(std::string*)this->val.val);
		}
		default:
			return val;
		}
	}

	std::string arg::toString()
	{
		std::string opt = "";

		switch (argType)
		{
		case argType::constant:
			switch (val.type)
			{
			case type::string:
				opt += '"' + SCISL_CAST_STRING(val.val) + '"';
				break;
			case type::floating:
			{
				SCISL_FLOAT_PRECISION v = SCISL_CAST_FLOAT(val.val);
				if (v == round(v))
				{
					opt += std::to_string(v) + ".0";
				}
				else
				{
					opt += std::to_string(v);
				}
				break;
			}
			case type::integer:
				opt += std::to_string(SCISL_CAST_INT(val.val));
				break;
			default:
				break;
			}
			break;
		case argType::variable:
			opt += 'v' + std::to_string((unsigned long long)(val.val));
			break;
		case argType::interop:
			opt += "$" + SCISL_CAST_STRING(val.val);
			break;
		default:
			break;
		}

		return opt;
	}

	arg::~arg()
	{
		switch (argType)
		{
		case argType::interop:
			delete (std::string*)(val.val);
			val.type = type::error;
			break;
		default:
			break;
				//error type (nullptr)
		}
	}
}