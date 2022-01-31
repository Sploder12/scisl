#ifndef SCISL_TABLES_H
#define SCISL_TABLES_H

#include "common.h"

#include "program.h"

#include <map>

namespace scisl
{
	struct registeredVar
	{
		type type;
		void* adr;
	};

	typedef void (*scislPeephole)(instruction&);

	struct registeredFunc
	{
		scislFunc func;
		unsigned short argCount;
		scislPeephole optimizer = nullptr;
	};

	typedef std::map<std::string, registeredVar> varTable;
	typedef std::map<std::string, registeredFunc> funcTable;

	void registerVar(std::string id, SCISL_INT_PRECISION& var);
	void registerVar(std::string id, SCISL_FLOAT_PRECISION& var);
	void registerVar(std::string id, std::string& var);

	void registerFunc(std::string id, scislFunc func, unsigned short argCount, scislPeephole optimizer = nullptr);

	varTable& getVarTable();

	funcTable& getFuncTable();
}

#endif