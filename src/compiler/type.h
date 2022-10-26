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
}

#endif
