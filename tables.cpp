#include "tables.h"

namespace scisl
{
	varTable interopTable;
	funcTable interopFuncTable;

	void registerVar(std::string id, SCISL_INT_PRECISION& var)
	{
		registeredVar* tvar = new registeredVar();
		tvar->type = type::integer;
		tvar->val = &var;
		interopTable.insert({ id, tvar });
	}

	void registerVar(std::string id, SCISL_FLOAT_PRECISION& var)
	{
		registeredVar* tvar = new registeredVar();
		tvar->type = type::integer;
		tvar->val = &var;
		interopTable.insert({ id, tvar });
	}

	void registerVar(std::string id, std::string& var)
	{
		registeredVar* tvar = new registeredVar();
		tvar->type = type::integer;
		tvar->val = &var;
		interopTable.insert({ id, tvar });
	}

	void registerFunc(std::string id, scislFunc func, unsigned short argCount, std::string argTypes, unsigned int optimizerFlags, scislPeephole optimizer)
	{
		registeredFunc tmp = { id, func, optimizer, argCount, argTypes, optimizerFlags };
		interopFuncTable.insert({ id, std::move(tmp) });
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