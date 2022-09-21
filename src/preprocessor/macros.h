#ifndef _SCISL_MACROS_H
#define _SCISL_MACROS_H

#include <string>

namespace scisl {
	bool defineMacro(std::string_view name, std::string_view replacement);
	bool removeMacro(const std::string& name);

	std::string_view getMacro(const std::string& input);
}

#endif