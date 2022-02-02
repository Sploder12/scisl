#include "common.h"

namespace scisl
{
	value::value(value&& moved) noexcept
	{
		std::swap(this->type, moved.type);
		std::swap(this->val, moved.val);
		std::swap(this->isTemporary, moved.isTemporary);
	}

	value& value::operator=(value&& moved) noexcept
	{
		std::swap(this->type, moved.type);
		std::swap(this->val, moved.val);
		std::swap(this->isTemporary, moved.isTemporary);
		return *this;
	}

	value& value::operator=(void* val)
	{
		switch (this->type)
		{
			case type::string:
				SCISL_CAST_STRING(this->val) = SCISL_CAST_STRING(val);
				break;
			case type::integer:
				SCISL_CAST_INT(this->val) = SCISL_CAST_INT(val);
				break;
			case type::floating:
				SCISL_CAST_FLOAT(this->val) = SCISL_CAST_FLOAT(val);
				break;
		}
		return *this;
	}

	value& value::operator+=(value& other)
	{
		switch (this->type)
		{
		case type::string:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_STRING(this->val) += SCISL_CAST_STRING(other.val);
				break;
			case type::integer:
				SCISL_CAST_STRING(this->val) += std::to_string(SCISL_CAST_INT(other.val));
				break;
			case type::floating:
				SCISL_CAST_STRING(this->val) += std::to_string(SCISL_CAST_FLOAT(other.val));
				break;
			default:
				break;
			}
			break;
		case type::integer:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_INT(this->val) += (SCISL_INT_PRECISION)std::stol(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_INT(this->val) += SCISL_CAST_INT(other.val);
				break;
			case type::floating:
				SCISL_CAST_INT(this->val) += SCISL_CAST_FLOAT(other.val);
				break;
			default:
				break;
			}
			break;
		case type::floating:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_FLOAT(this->val) += (SCISL_FLOAT_PRECISION)std::stod(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_FLOAT(this->val) += SCISL_CAST_INT(other.val);
				break;
			case type::floating:
				SCISL_CAST_FLOAT(this->val) += SCISL_CAST_FLOAT(other.val);
				break;
			default:
				break;
			}
			break; 
		default:
			break; 
		}
		return *this;
	}

	value& value::operator-=(value& other)
	{
		switch (this->type)
		{
		case type::integer:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_INT(this->val) -= (SCISL_INT_PRECISION)std::stol(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_INT(this->val) -= SCISL_CAST_INT(other.val);
				break;
			case type::floating:
				SCISL_CAST_INT(this->val) -= SCISL_CAST_FLOAT(other.val);
				break;
			default:
				break;
			}
			break;
		case type::floating:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_FLOAT(this->val) -= (SCISL_FLOAT_PRECISION)std::stod(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_FLOAT(this->val) -= SCISL_CAST_INT(other.val);
				break;
			case type::floating:
				SCISL_CAST_FLOAT(this->val) -= SCISL_CAST_FLOAT(other.val);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return *this;
	}

	value& value::operator*=(value& other)
	{
		switch (this->type)
		{
		case type::string:
			switch (other.type)
			{
			case type::integer:
			{
				const std::string tmp = SCISL_CAST_STRING(this->val);
				std::string tmpSum = "";
				for (int i = 0; i < SCISL_CAST_INT(other.val); i++)
				{
					tmpSum += tmp;
				}
				SCISL_CAST_STRING(this->val) = tmpSum;
				break;
			}
			default:
				break;
			}
			break;
		case type::integer:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_INT(this->val) *= (SCISL_INT_PRECISION)std::stol(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_INT(this->val) *= SCISL_CAST_INT(other.val);
				break;
			case type::floating:
				SCISL_CAST_INT(this->val) *= SCISL_CAST_FLOAT(other.val);
				break;
			default:
				break;
			}
			break;
		case type::floating:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_FLOAT(this->val) *= (SCISL_FLOAT_PRECISION)std::stod(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_FLOAT(this->val) *= SCISL_CAST_INT(other.val);
				break;
			case type::floating:
				SCISL_CAST_FLOAT(this->val) *= SCISL_CAST_FLOAT(other.val);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return *this;
	}

	value& value::operator/=(value& other)
	{
		switch (this->type)
		{
		case type::integer:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_INT(this->val) /= (SCISL_INT_PRECISION)std::stol(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_INT(this->val) /= SCISL_CAST_INT(other.val);
				break;
			case type::floating:
				SCISL_CAST_INT(this->val) /= SCISL_CAST_FLOAT(other.val);
				break;
			default:
				break;
			}
			break;
		case type::floating:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_FLOAT(this->val) /= (SCISL_FLOAT_PRECISION)std::stod(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_FLOAT(this->val) /= SCISL_CAST_INT(other.val);
				break;
			case type::floating:
				SCISL_CAST_FLOAT(this->val) /= SCISL_CAST_FLOAT(other.val);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return *this;
	}

	value& value::operator%=(value& other)
	{
		switch (this->type)
		{
		case type::integer:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_INT(this->val) %= (SCISL_INT_PRECISION)std::stol(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_INT(this->val) %= SCISL_CAST_INT(other.val);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return *this;
	}

	value& value::operator|=(value& other)
	{
		switch (this->type)
		{
		case type::integer:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_INT(this->val) |= (SCISL_INT_PRECISION)std::stol(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_INT(this->val) |= SCISL_CAST_INT(other.val);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return *this;
	}

	value& value::operator&=(value& other)
	{
		switch (this->type)
		{
		case type::integer:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_INT(this->val) &= (SCISL_INT_PRECISION)std::stol(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_INT(this->val) &= SCISL_CAST_INT(other.val);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return *this;
	}

	value& value::operator^=(value& other)
	{
		switch (this->type)
		{
		case type::integer:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_INT(this->val) ^= (SCISL_INT_PRECISION)std::stol(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_INT(this->val) ^= SCISL_CAST_INT(other.val);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return *this;
	}

	value& value::operator>>=(value& other)
	{
		switch (this->type)
		{
		case type::integer:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_INT(this->val) >>= (SCISL_INT_PRECISION)std::stol(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_INT(this->val) >>= SCISL_CAST_INT(other.val);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return *this;
	}

	value& value::operator<<=(value& other)
	{
		switch (this->type)
		{
		case type::integer:
			switch (other.type)
			{
			case type::string:
				SCISL_CAST_INT(this->val) <<= (SCISL_INT_PRECISION)std::stol(SCISL_CAST_STRING(other.val));
				break;
			case type::integer:
				SCISL_CAST_INT(this->val) <<= SCISL_CAST_INT(other.val);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return *this;
	}

	value::~value()
	{
		if (isTemporary)
		{
			switch (this->type)
			{
			case type::string:
				delete (std::string*)(this->val);
				break;
			case type::integer:
				delete (SCISL_INT_PRECISION*)(this->val);
				break;
			case type::floating:
				delete (SCISL_FLOAT_PRECISION*)(this->val);
				break;
			default:
				break;
			}
		}
	}

	value createTemporary(type tipe)
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
}