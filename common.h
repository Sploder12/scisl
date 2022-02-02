#ifndef SCISL_COMMON_H
#define SCISL_COMMON_H

#ifndef SCISL_INT_PRECISION
#define SCISL_INT_PRECISION int
#endif

#ifndef SCISL_FLOAT_PRECISION
#define SCISL_FLOAT_PRECISION float
#endif

#include <string>

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
	//although similar to arg, it acts VERY differently
	struct value
	{
		type type;
		void* val;
		bool isTemporary = false;

		value() :
			type(type::error), val(nullptr) {}

		value(value& o) = default;
		value(value&& moved) noexcept;

		value& operator=(value& o) = default;
		value& operator=(value&& moved) noexcept;

		value& operator=(void* val);

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

		~value();
	};

	value createTemporary(type tipe);
}

#endif