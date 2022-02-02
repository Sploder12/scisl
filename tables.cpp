#include "tables.h"

namespace scisl
{
	varTable interopTable;
	funcTable interopFuncTable;

	void registerVar(std::string id, SCISL_INT_PRECISION& var)
	{
		registeredVar tvar;
		tvar.type = type::integer;
		tvar.adr = &var;
		interopTable.insert({ id, tvar });
	}

	void registerVar(std::string id, SCISL_FLOAT_PRECISION& var)
	{
		registeredVar tvar;
		tvar.type = type::floating;
		tvar.adr = &var;
		interopTable.insert({ id, tvar });
	}

	void registerVar(std::string id, std::string& var)
	{
		registeredVar tvar;
		tvar.type = type::string;
		tvar.adr = &var;
		interopTable.insert({ id, tvar });
	}

	void registerFunc(std::string id, scislFunc func, unsigned short argCount, std::string argTypes, unsigned int optimizerFlags, scislPeephole optimizer)
	{
		registeredFunc tmp = { id, func, optimizer, argCount, argTypes, optimizerFlags };
		interopFuncTable.insert({ id, tmp });
	}

	varTable& getVarTable()
	{
		return interopTable;
	}

	funcTable& getFuncTable()
	{
		return interopFuncTable;
	}
}