#include "args.h"

#include "common.h"

#include "tables.h"
#include "program.h"

namespace scisl
{
	arg::arg(arg& other)
	{
		this->val = nullptr;
		this->argType = other.argType;
		this->type = other.type;
		switch (argType)
		{
		case argType::constant:
		{
			switch (type)
			{
			case type::integer:
				this->val = new SCISL_INT_PRECISION(*(SCISL_INT_PRECISION*)(other.val));
				break;
			case type::floating:
				this->val = new SCISL_FLOAT_PRECISION(*(SCISL_FLOAT_PRECISION*)(other.val));
				break;
			case type::string:
				this->val = new std::string(*(std::string*)other.val);
				break;
			}
			break;
		}
		case argType::variable:
		case argType::interop: //copy the name
			this->val = new std::string(*(std::string*)other.val);
			break;
		default:
			break;
			//error
		}
	}

	arg::arg(arg&& cmove) noexcept :
		argType(std::move(cmove.argType)),
		type(std::move(cmove.type)),
		val(std::move(cmove.val))
	{
		cmove.val = nullptr;
		cmove.type = type::error;
		cmove.argType = argType::error;
	}

	arg& arg::operator=(arg& other)
	{
		this->argType = other.argType;
		this->type = other.type;
		switch (argType)
		{
		case argType::constant:
		{
			switch (type)
			{
			case type::integer:
				this->val = new SCISL_INT_PRECISION(*(SCISL_INT_PRECISION*)(other.val));
				break;
			case type::floating:
				this->val = new SCISL_FLOAT_PRECISION(*(SCISL_FLOAT_PRECISION*)(other.val));
				break;
			case type::string:
				this->val = new std::string(*(std::string*)other.val);
				break;
			}
			break;
		}
		case argType::variable:
		case argType::interop: //copy the name
			this->val = new std::string(*(std::string*)other.val);
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
		std::swap(this->type, cmove.type);
		std::swap(this->val, cmove.val);

		return *this;
	}

	value arg::getValue(program& process)
	{
		value opt;
		
		switch (argType)
		{
		case argType::constant:
			opt.type = this->type;
			opt.val = this->val;
			break;
		case argType::variable:
			return process.memory[*(unsigned short*)this->val];
			break;
		case argType::interop:
		{
			auto& vtable = getVarTable();
			registeredVar& v = vtable.at(*(std::string*)this->val);
			opt.type = v.type;
			opt.val = v.adr;
			break;
		}
		default:
			opt.type = type::error;
			opt.val = nullptr;
			break;
		}
		return opt;
	}

	arg::~arg()
	{
		switch (argType)
		{
		case argType::interop:
			delete (std::string*)(val);
			break;
		case argType::variable:
			delete (unsigned short*)(val);
			break;
		case argType::constant:
			switch (type)
			{
			case type::integer:
				delete (SCISL_INT_PRECISION*)(val);
				break;
			case type::floating:
				delete (SCISL_FLOAT_PRECISION*)(val);
				break;
			case type::string:
				delete (std::string*)(val);
				break;
			default:
				break;
				//error type (nullptr)
			}
			break;
		}
	}
}