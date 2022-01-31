#include "tables.h"

namespace scisl
{
	varTable interopTable;

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

	varTable& getVarTable()
	{
		return interopTable;
	}
}