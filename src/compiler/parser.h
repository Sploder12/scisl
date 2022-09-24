#ifndef _SCISL_PARSER_H
#define _SCISL_PARSER_H

#include "type.h"
#include "../runtime/stl.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace scisl {
	
	struct IntermediateArg {
		std::string value; // it's just the name
		ValType valType;
		ArgType argType;

		IntermediateArg(std::string_view str, ValType vtype, ArgType atype) :
			value(str), valType(vtype), argType(atype) {}
	};

	struct IntermediateInstr {
		std::string strName; //it's the function's string name
		
		std::vector<IntermediateArg> args;

		stlFunc func; //stl funcID, or count for interop

		std::string asString() const {
			std::string out = strName;

			for (auto& arg : args) {
				out += ' ' + arg.value;
			}

			return out;
		}
	};

	struct IntermediateVar {
		std::string value; // it's just the name
		ValType valType;
		std::vector<IntermediateInstr*> usedBy; // sorted vector of all usages
	};

	struct Intermediate {
		std::vector<IntermediateInstr> instrs;

		std::unordered_map<std::string, IntermediateVar> vars;
	};

	static const funcMeta& getMeta(const IntermediateInstr& instr) {
		if (instr.func != stlFunc::count) {
			return stlFuncMeta[(size_t)instr.func];
		}
		else {
			//@TODO replace with interop
			return stlFuncMeta[(size_t)stlFunc::jmp];
		}
	}

	Intermediate parse(std::string_view in);
}

#endif 