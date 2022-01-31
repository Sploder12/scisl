#ifndef SCISL_TABLES_H
#define SCISL_TABLES_H

#include "common.h"

#include <map>

namespace scisl
{
	struct registeredVar
	{
		type type;
		void* adr;
	};

	typedef std::map<std::string, registeredVar> varTable;

	void registerVar(std::string id, SCISL_INT_PRECISION& var);
	void registerVar(std::string id, SCISL_FLOAT_PRECISION& var);
	void registerVar(std::string id, std::string& var);

	varTable& getVarTable();
}

#endif