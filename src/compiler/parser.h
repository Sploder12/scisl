#ifndef _SCISL_PARSER_H
#define _SCISL_PARSER_H

#include "type.h"
#include "../runtime/stl.h"

#include "../interop/interop.h"

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

		SCISL_INT toInt() const {
			switch (valType)
			{
			case scisl::ValType::integer:
				return (SCISL_INT)std::stol(value);
			case scisl::ValType::floating:
				return (SCISL_INT)std::stod(value);
			case scisl::ValType::string:
				return (SCISL_INT)std::stol(value.substr(1, value.size()-2));
			default:
				return 0;
			}
		}

		SCISL_FLOAT toFloat() const {
			switch (valType)
			{
			case scisl::ValType::integer:
				return (SCISL_FLOAT)std::stol(value);
			case scisl::ValType::floating:
				return (SCISL_FLOAT)std::stod(value);
			case scisl::ValType::string:
				return (SCISL_FLOAT)std::stod(value.substr(1, value.size() - 2));
			default:
				return 0.0;
			}
		}

		SCISL_STR toString() const {
			switch (valType)
			{
			case scisl::ValType::integer:
			case scisl::ValType::floating:
				return value;
			case scisl::ValType::string:
				return value.substr(1, value.size() - 2);
			default:
				return "";
			}
		}
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
			return getFuncTable()[instr.strName];
		}
	}

	Intermediate parse(std::string_view in);
}

#endif 