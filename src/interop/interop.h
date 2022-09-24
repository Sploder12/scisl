#ifndef _SCISL_INTEROP_H
#define _SCISL_INTEROP_H

#include "../runtime/program.h"
#include "../runtime/stl.h"
#include "../compiler/type.h"

#include <string>
#include <unordered_map>

namespace scisl {
	typedef void(*scislFunc)(Program&, std::vector<Val>&);

	void registerVar(std::string_view id, SCISL_INT* var);
	void registerVar(std::string_view id, SCISL_FLOAT* var);
	void registerVar(std::string_view id, SCISL_STR* var);

	//DO NOT USE THIS AFTER COMPILATION
	void removeVar(std::string_view id);

	void removeAllVars();

	void registerFunc(std::string_view id, scislFunc func, size_t args, const char* argTypes = "", ValType initializes = ValType::err, funcFlags flags = funcFlags::none);


	std::unordered_map<std::string, Val>& getVarTable();
	std::unordered_map<std::string, funcMeta>& getFuncTable();

}

#endif