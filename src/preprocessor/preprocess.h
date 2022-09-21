#ifndef _SCISL_PREPROCESS_H
#define _SCISL_PREPROCESS_H

#include <string>

namespace scisl {
	std::string preprocess(std::string_view program, unsigned char macrodepth = 0);
}

#endif