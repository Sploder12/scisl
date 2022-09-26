#ifndef _SCISL_OPTIMIZER_H
#define _SCISL_OPTIMIZER_H

#include "../runtime/program.h"
#include "../runtime/stl.h"

#include "parser.h"

namespace scisl {
	static size_t getBlockEnd(const std::vector<IntermediateInstr>& instrs, size_t offset) {
		size_t blocks = 1;
		for (size_t i = offset + 1; i < instrs.size(); ++i) {
			const auto& cur = instrs[i];
			const auto& meta = getMeta(cur);
			if (meta.flags & funcFlags::defines_block) {
				++blocks;
			}
			else if (cur.func == stlFunc::blockend) {
				if (--blocks == 0) return i;
			}
		}
		return instrs.size();
	}

	void optimize(Intermediate& program);
}

#endif
