#ifndef _SCISL_TYPE_H
#define _SCISL_TYPE_H

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
