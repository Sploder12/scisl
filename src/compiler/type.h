#ifndef _SCISL_TYPE_H
#define _SCISL_TYPE_H

#define SCISL_INT int
#define SCISL_FLOAT float
#define SCISL_STR std::string

namespace scisl {
	enum class ValType {
		integer,
		floating,
		string,

		err
	};

	enum class ArgType {
		constant,
		variable,
		interop
	};
}

#endif
