#ifndef SCISL_COMPILER_H
#define SCISL_COMPILER_H

#include "../runtime/program.h"

namespace scisl
{
	program* compile(const char* filename, bool nullOnError = true);
}

#endif