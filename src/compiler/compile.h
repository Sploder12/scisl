#ifndef _SCISL_COMPILE_H
#define _SCISL_COMPILE_H

#include "../runtime/program.h"
#include "parser.h"

namespace scisl {

	Program compile(Intermediate& program);
}

#endif