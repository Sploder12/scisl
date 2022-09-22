#include "aliases.h"

namespace scisl {

#ifndef SCISL_NO_ALIASES
	std::string_view nameFromAlias(std::string alias) {
		std::transform(std::execution::unseq, alias.begin(), alias.end(), alias.begin(), toupper);

		for (size_t i = 0; i < stlAliases.size(); ++i) {
			if (stlAliases[i].contains(alias)) {
				return stlNames[i];
			}
		}

		return "";
	}
#else
	std::string_view nameFromAlias(std::string alias) {
		std::transform(std::execution::unseq, alias.begin(), alias.end(), alias.begin(), toupper);

		for (size_t i = 0; i < stlNames.size(); ++i) {
			if (stlNames[i] == alias) {
				return stlNames[i];
			}
		}

		return "";
	}
#endif
}