#include "macros.h"

#include <unordered_map>

namespace scisl {

	std::unordered_map<std::string, std::string> macros;

	bool defineMacro(std::string_view name, std::string_view replacement) {
		if (name.find(' ') != std::string_view::npos ||
			name.find(';') != std::string_view::npos ||
			name.find('\t') != std::string_view::npos ||
			name.find('\n') != std::string_view::npos)
			return false;

		return macros.emplace(name, replacement).second;
	}

	bool removeMacro(const std::string& name) {
		return macros.erase(name);
	}

	std::string_view getMacro(const std::string& input) {
		const auto it = macros.find(input);
		if (it != macros.end()) {
			return it->second;
		}
		
		return "";
	}
}