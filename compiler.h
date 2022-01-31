#ifndef SCISL_COMPILER_H
#define SCISL_COMPILER_H

#include "common.h"
#include "program.h"
#include "stl.h"

namespace scisl
{
	program* compile(const char* filename);
}

#endif