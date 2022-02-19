#include "tables.h"

#include "../runtime/stl.h"

namespace scisl
{
	varTable interopTable;
	funcTable interopFuncTable;
	macroTable macros;

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
		tvar->type = type::floating;
		tvar->val = &var;
		interopTable.insert({ id, tvar });
	}

	void registerVar(std::string id, std::string& var)
	{
		registeredVar* tvar = new registeredVar();
		tvar->type = type::string;
		tvar->val = &var;
		interopTable.insert({ id, tvar });
	}

	void updateVar(std::string id, SCISL_INT_PRECISION& var)
	{
		if (interopTable.contains(id))
		{
			registeredVar* v = interopTable.at(id);
			v->type = type::integer;
			v->val = &var;
		}
	}

	void updateVar(std::string id, SCISL_FLOAT_PRECISION& var)
	{
		if (interopTable.contains(id))
		{
			registeredVar* v = interopTable.at(id);
			v->type = type::floating;
			v->val = &var;
		}
	}

	void updateVar(std::string id, std::string& var)
	{
		if (interopTable.contains(id))
		{
			registeredVar* v = interopTable.at(id);
			v->type = type::string;
			v->val = &var;
		}
	}

	void removeVar(std::string id)
	{
		if (interopTable.contains(id))
		{
			registeredVar* v = interopTable.at(id);
			v->val = nullptr;
			delete v;
			interopTable.erase(id);
		}
	}

	void removeAllVars()
	{
		for (auto& p : interopTable)
		{
			p.second->val = nullptr;
			delete p.second;
		}
		interopTable.clear();
	}

	void registerFunc(std::string id, scislFunc func, unsigned short argCount, unsigned short minArgs, std::string argTypes, unsigned int optimizerFlags, scislPeephole optimizer)
	{
		id = '$' + id;
		registeredFunc tmp = { (unsigned short)(stlFuncs::stlFuncCount), id, func, optimizer, argCount, minArgs, argTypes, optimizerFlags};
		interopFuncTable.insert({ id, std::move(tmp) });
	}

	void defineMacro(std::string id, std::string value)
	{
		macros.insert({id, value});
	}

	varTable& getVarTable()
	{
		return interopTable;
	}

	funcTable& getFuncTable()
	{
		return interopFuncTable;
	}

	macroTable& getMacroTable()
	{
		return macros;
	}
}