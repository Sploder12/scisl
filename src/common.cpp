#include "common.h"

namespace scisl
{
	value::value(value&& moved) noexcept
	{
		std::swap(this->type, moved.type);
		std::swap(this->val, moved.val);
	}

	value& value::operator=(value& o)
	{
		this->type = o.type;
		switch (o.type)
		{
		case type::integer:
			this->val = new SCISL_INT_PRECISION(SCISL_CAST_INT(o.val));
			break;
		case type::floating:
			this->val = new SCISL_FLOAT_PRECISION(SCISL_CAST_FLOAT(o.val));
			break;
		case type::string:
			this->val = new std::string(SCISL_CAST_STRING(o.val));
			break;
		}
		return *this;
	}

	value& value::operator=(value&& moved) noexcept
	{
		std::swap(this->type, moved.type);
		std::swap(this->val, moved.val);
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

	value& value::operator=(SCISL_INT_PRECISION val)
	{
		switch (this->type)
		{
		case type::string:
			SCISL_CAST_STRING(this->val) = std::to_string(val);
			break;
		case type::integer:
			SCISL_CAST_INT(this->val) = val;
			break;
		case type::floating:
			SCISL_CAST_FLOAT(this->val) = SCISL_FLOAT_PRECISION(val);
			break;
		}
		return *this;
	}

	value& value::operator=(SCISL_FLOAT_PRECISION val)
	{
		switch (this->type)
		{
		case type::string:
			SCISL_CAST_STRING(this->val) = std::to_string(val);
			break;
		case type::integer:
			SCISL_CAST_INT(this->val) = SCISL_INT_PRECISION(val);
			break;
		case type::floating:
			SCISL_CAST_FLOAT(this->val) = val;
			break;
		}
		return *this;
	}

	value& value::operator=(std::string val)
	{
		switch (this->type)
		{
		case type::string:
			SCISL_CAST_STRING(this->val) = val;
			break;
		case type::integer:
			SCISL_CAST_INT(this->val) = (SCISL_INT_PRECISION)(std::stol(val));
			break;
		case type::floating:
			SCISL_CAST_FLOAT(this->val) = (SCISL_FLOAT_PRECISION)(std::stod(val));
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

	bool value::operator<(value& other)
	{
		switch (this->type)
		{
		case type::string:
			return false;
		case type::integer:
			switch (other.type)
			{
			case type::string:
				return false;
			case type::integer:
			{
				return SCISL_CAST_INT(this->val) < SCISL_CAST_INT(other.val);
			}
			case type::floating:
				return SCISL_CAST_INT(this->val) < SCISL_CAST_FLOAT(other.val);
			default:
				return false;
			}
		case type::floating:
			switch (other.type)
			{
			case type::string:
				return false;
			case type::integer:
				return SCISL_CAST_FLOAT(this->val) < SCISL_CAST_INT(other.val);
			case type::floating:
				return SCISL_CAST_FLOAT(this->val) > SCISL_CAST_FLOAT(other.val);
			default:
				return false;
			}
		default:
			return false;
		}
	}

	bool value::operator>(value& other)
	{
		switch (this->type)
		{
		case type::string:
			return false;
		case type::integer:
			switch (other.type)
			{
			case type::string:
				return false;
			case type::integer:
				return SCISL_CAST_INT(this->val) > SCISL_CAST_INT(other.val);
			case type::floating:
				return SCISL_CAST_INT(this->val) > SCISL_CAST_FLOAT(other.val);
			default:
				return false;
			}
		case type::floating:
			switch (other.type)
			{
			case type::string:
				return false;
			case type::integer:
				return SCISL_CAST_FLOAT(this->val) > SCISL_CAST_INT(other.val);
			case type::floating:
				return SCISL_CAST_FLOAT(this->val) > SCISL_CAST_FLOAT(other.val);
			default:
				return false;
			}
		default:
			return false;
		}
	}

	bool value::operator==(value& other)
	{
		switch (this->type)
		{
		case type::string:
			switch (other.type)
			{
			case type::string:
				return SCISL_CAST_STRING(this->val) == SCISL_CAST_STRING(other.val);
			default:
				return false;
			}
		case type::integer:
			switch (other.type)
			{
			case type::string:
				return false;
			case type::integer:
				return SCISL_CAST_INT(this->val) == SCISL_CAST_INT(other.val);
			case type::floating:
				return SCISL_CAST_INT(this->val) == SCISL_CAST_FLOAT(other.val);
			default:
				return false;
			}
		case type::floating:
			switch (other.type)
			{
			case type::string:
				return false;
			case type::integer:
				return SCISL_CAST_FLOAT(this->val) == SCISL_CAST_INT(other.val);
			case type::floating:
				return SCISL_CAST_FLOAT(this->val) == SCISL_CAST_FLOAT(other.val);
			default:
				return false;
			}
		default:
			return false;
		}
	}

	bool value::operator==(SCISL_INT_PRECISION other)
	{
		switch (this->type)
		{
		case type::string:
			return false;
		case type::integer:
			return SCISL_CAST_INT(this->val) == other;
		case type::floating:
			return SCISL_CAST_FLOAT(this->val) == other;
		default:
			return false;
		}
	}

	bool value::operator&&(value& other)
	{
		switch (this->type)
		{
		case type::string:
			return false;
		case type::integer:
			switch (other.type)
			{
			case type::string:
				return false;
			case type::integer:
				return SCISL_CAST_INT(this->val) && SCISL_CAST_INT(other.val);
			case type::floating:
				return SCISL_CAST_INT(this->val) && SCISL_CAST_FLOAT(other.val);
			default:
				return false;
			}
		case type::floating:
			switch (other.type)
			{
			case type::string:
				return false;
			case type::integer:
				return SCISL_CAST_FLOAT(this->val) && SCISL_CAST_INT(other.val);
			case type::floating:
				return SCISL_CAST_FLOAT(this->val) && SCISL_CAST_FLOAT(other.val);
			default:
				return false;
			}
		default:
			return false;
		}
	}

	bool value::operator||(value& other)
	{
		switch (this->type)
		{
		case type::string:
			return false;
		case type::integer:
			switch (other.type)
			{
			case type::string:
				return false;
			case type::integer:
				return SCISL_CAST_INT(this->val) || SCISL_CAST_INT(other.val);
			case type::floating:
				return SCISL_CAST_INT(this->val) || SCISL_CAST_FLOAT(other.val);
			default:
				return false;
			}
		case type::floating:
			switch (other.type)
			{
			case type::string:
				return false;
			case type::integer:
				return SCISL_CAST_FLOAT(this->val) || SCISL_CAST_INT(other.val);
			case type::floating:
				return SCISL_CAST_FLOAT(this->val) || SCISL_CAST_FLOAT(other.val);
			default:
				return false;
			}
		default:
			return false;
		}
	}

	value::~value()
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

	value createTemporary(type tipe)
	{
		value opt;
		opt.type = tipe;
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

	value createTemporary(type tipe, SCISL_INT_PRECISION val)
	{
		value opt;
		opt.type = tipe;
		switch (tipe)
		{
		case (type::string):
			opt.val = new std::string(std::to_string(val));
			break;
		case (type::integer):
			opt.val = new SCISL_INT_PRECISION((SCISL_INT_PRECISION)(val));
			break;
		case (type::floating):
			opt.val = new SCISL_FLOAT_PRECISION((SCISL_FLOAT_PRECISION)(val));
			break;
		default:
			opt.val = nullptr;
		}
		return opt;
	}

	value createTemporary(type tipe, SCISL_FLOAT_PRECISION val)
	{
		value opt;
		opt.type = tipe;
		switch (tipe)
		{
		case (type::string):
			opt.val = new std::string(std::to_string(val));
			break;
		case (type::integer):
			opt.val = new SCISL_INT_PRECISION((SCISL_INT_PRECISION)(val));
			break;
		case (type::floating):
			opt.val = new SCISL_FLOAT_PRECISION((SCISL_FLOAT_PRECISION)(val));
			break;
		default:
			opt.val = nullptr;
		}
		return opt;
	}

	value createTemporary(type tipe, std::string val)
	{
		value opt;
		opt.type = tipe;
		switch (tipe)
		{
		case (type::string):
			opt.val = new std::string(val);
			break;
		case (type::integer):
			opt.val = new SCISL_INT_PRECISION((SCISL_INT_PRECISION)(std::stol(val)));
			break;
		case (type::floating):
			opt.val = new SCISL_FLOAT_PRECISION((SCISL_FLOAT_PRECISION)(std::stod(val)));
			break;
		default:
			opt.val = nullptr;
		}
		return opt;
	}
}