#ifndef SCISL_TABLES_H
#define SCISL_TABLES_H

#include "common.h"
#include "program.h"

#include <map>

namespace scisl
{
	typedef value registeredVar;
	typedef scislfuncMeta registeredFunc;

	typedef std::map<std::string, registeredVar*> varTable;
	typedef std::map<std::string, registeredFunc> funcTable;
	typedef std::map<std::string, std::string> macroTable;

	void registerVar(std::string id, SCISL_INT_PRECISION& var);
	void registerVar(std::string id, SCISL_FLOAT_PRECISION& var);
	void registerVar(std::string id, std::string& var);

	void registerFunc(std::string id, scislFunc func, unsigned short argCount, std::string argTypes = "", unsigned int optimizerFlags = 0, scislPeephole optimizer = nullptr);

	void defineMacro(std::string id, std::string value);

	varTable& getVarTable();

	funcTable& getFuncTable();

	macroTable& getMacroTable();
}

#endif