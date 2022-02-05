#ifndef SCISL_COMMON_H
#define SCISL_COMMON_H

#ifndef SCISL_INT_PRECISION
#define SCISL_INT_PRECISION int
#endif

#ifndef SCISL_FLOAT_PRECISION
#define SCISL_FLOAT_PRECISION float
#endif

#include <string>
#include <vector>

#define SCISL_CAST_INT(var) (*(SCISL_INT_PRECISION*)(var))
#define SCISL_CAST_FLOAT(var) (*(SCISL_FLOAT_PRECISION*)(var))
#define SCISL_CAST_STRING(var) (*(std::string*)(var))

namespace scisl
{
	enum class type : unsigned char
	{
		integer,
		floating,
		string,

		error
	};

	//these are vals, used during runtime
	struct value
	{
		type type;
		void* val;

		value() :
			type(type::error), val(nullptr) {}

		value(value& o) = default;
		value(value&& moved) noexcept;

		value& operator=(value& o) = default;
		value& operator=(value&& moved) noexcept;

		value& operator=(void* val);
		value& operator=(SCISL_INT_PRECISION val);
		value& operator=(SCISL_FLOAT_PRECISION val);
		value& operator=(std::string val);

		value& operator+=(value& other);
		value& operator-=(value& other);
		value& operator*=(value& other);
		value& operator/=(value& other);
		value& operator%=(value& other);
		value& operator|=(value& other);
		value& operator&=(value& other);
		value& operator^=(value& other);
		value& operator>>=(value& other);
		value& operator<<=(value& other);

		bool operator<(value& other);
		bool operator>(value& other);
		bool operator==(value& other);
		bool operator!=(value& other);

		~value();
	};

	value createTemporary(type tipe);
	value createTemporary(type tipe, SCISL_INT_PRECISION val);
	value createTemporary(type tipe, SCISL_FLOAT_PRECISION val);
	value createTemporary(type tipe, std::string val);

	template <typename T>
	inline size_t findV(std::vector<std::pair<std::string, T>>& vars, std::string& cur)
	{
		for (unsigned short i = 0; i < vars.size(); i++)
		{
			if (vars[i].first == cur)
			{
				return i;
			}
		}
		return vars.size();
	}
}

#endif