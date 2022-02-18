#include "instruction.h"

namespace scisl
{
	void instruction::run(program& process)
	{
		if (func == nullptr) return;
		func(process, arguments);
	}
}