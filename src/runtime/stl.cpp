#include "stl.h"

namespace scisl {
	stlFunc strToStlFunc(std::string_view str) {
		for (size_t i = 0; i < stlNames.size(); ++i) {
			if (str == stlNames[i]) {
				return (stlFunc)(i);
			}
		}

		return stlFunc::count;
	}
}