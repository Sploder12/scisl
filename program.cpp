#include "program.h"

namespace scisl
{
	void instruction::run(program& process)
	{
		func(process, arguments);
	}
}