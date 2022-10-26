#include "interop.h"

namespace scisl {
	std::unordered_map<std::string, Val> varTable;
	std::unordered_map<std::string, funcMeta> funcTable;

	void registerVar(std::string_view id, SCISL_INT* var) {
		varTable['$' + (std::string)id] = Val{ var };
	}

	void registerVar(std::string_view id, SCISL_FLOAT* var) {
		varTable['$' + (std::string)id] = Val{ var };
	}

	void registerVar(std::string_view id, SCISL_STR* var) {
		varTable['$' + (std::string)id] = Val{ var };
	}

	//DO NOT USE THIS AFTER COMPILATION
	void removeVar(std::string_view id) {
		varTable.erase('$' + (std::string)id);
	}

	void removeAllVars() {
		varTable.clear();
	}

	void registerFunc(std::string_view id, scislFunc func, int args, const char* argTypes, ValType initializes, funcFlags flags) {
		funcTable['$' + (std::string)id] = funcMeta{stlFunc::count, func, args, argTypes, initializes, flags};
	}

	std::unordered_map<std::string, Val>& getVarTable() { return varTable; }
	std::unordered_map<std::string, funcMeta>& getFuncTable() { return funcTable; }
}