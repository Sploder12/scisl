#ifndef SCISL_TABLES_H
#define SCISL_TABLES_H

#include "../common.h"
#include "../runtime/program.h"

#include <unordered_map>

namespace scisl
{
	typedef value registeredVar;
	typedef scislfuncMeta registeredFunc;

	typedef std::unordered_map<std::string, registeredVar*> varTable;
	typedef std::unordered_map<std::string, registeredFunc> funcTable;
	typedef std::unordered_map<std::string, std::string> macroTable;

	void registerVar(std::string id, SCISL_INT_PRECISION& var);
	void registerVar(std::string id, SCISL_FLOAT_PRECISION& var);
	void registerVar(std::string id, std::string& var);

	//you can update type, but dont do that after compilation, it's dangerous.
	void updateVar(std::string id, SCISL_INT_PRECISION& var);
	void updateVar(std::string id, SCISL_FLOAT_PRECISION& var);
	void updateVar(std::string id, std::string& var);

	//DO NOT USE THIS AFTER COMPILATION
	void removeVar(std::string id);

	void removeAllVars();

	void registerFunc(std::string id, scislFunc func, unsigned short argCount, unsigned short minArgs = 0,std::string argTypes = "", type initializes = type::error, unsigned int flags = 0, scislPeephole optimizer = nullptr);

	void defineMacro(std::string id, std::string value);

	varTable& getVarTable();

	funcTable& getFuncTable();

	macroTable& getMacroTable();
}

#endif