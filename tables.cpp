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

	void registerFunc(std::string id, scislFunc func, unsigned short argCount, scislPeephole optimizer)
	{
		registeredFunc tmp;
		tmp.func = func;
		tmp.argCount = argCount;
		tmp.optimizer = optimizer;
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