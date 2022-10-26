#ifndef _SCISL_TYPE_H
#define _SCISL_TYPE_H

#define SCISL_INT int
#define SCISL_FLOAT float
#define SCISL_STR std::string

#include <variant>
#include <string>

namespace scisl {

	typedef std::variant<SCISL_INT*, SCISL_FLOAT*, SCISL_STR*> ScislType;
	enum class ValType {
		integer,
		floating,
		string,

		err = -1
	};

	enum class ArgType {
		constant,
		variable,
		interop
	};

	template <typename To_t, typename From_t>
	To_t ScislCast(const From_t& from) {
		if constexpr (std::is_same_v<To_t, From_t>)
			return from;
		else if constexpr (std::is_same_v<SCISL_INT, To_t>) {
			if constexpr (std::is_same_v<SCISL_STR, From_t>)
				return (SCISL_INT)std::stol(from);
			else
				return (SCISL_INT)from;
		}
		else if constexpr (std::is_same_v<SCISL_FLOAT, To_t>) {
			if constexpr (std::is_same_v<SCISL_STR, From_t>)
				return (SCISL_FLOAT)std::stod(from);
			else
				return (SCISL_FLOAT)from;
		}
		else if constexpr (std::is_same_v<SCISL_STR, To_t>)
			return std::to_string(from);
		else
			return from; // you're using weird types, pray for implicit conversion
	}
}

#endif
