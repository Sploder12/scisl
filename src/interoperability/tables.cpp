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
		const auto it = interopTable.find(id);
		if (it != interopTable.end())
		{
			registeredVar* v = (*it).second;
			v->type = type::integer;
			v->val = &var;
		}
	}

	void updateVar(std::string id, SCISL_FLOAT_PRECISION& var)
	{
		const auto it = interopTable.find(id);
		if (it != interopTable.end())
		{
			registeredVar* v = (*it).second;
			v->type = type::floating;
			v->val = &var;
		}
	}

	void updateVar(std::string id, std::string& var)
	{
		const auto it = interopTable.find(id);
		if (it != interopTable.end())
		{
			registeredVar* v = (*it).second;
			v->type = type::string;
			v->val = &var;
		}
	}

	void removeVar(std::string id)
	{
		const auto it = interopTable.find(id);
		if (it != interopTable.end())
		{
			registeredVar* v = (*it).second;
			v->val = nullptr;
			delete v;
			interopTable.erase(it);
		}
	}

	void removeAllVars()
	{
		for (auto& [id, val] : interopTable)
		{
			val->val = nullptr;
			delete val;
		}
		interopTable.clear();
	}

	void registerFunc(std::string id, scislFunc func, unsigned short argCount, unsigned short minArgs, std::string argTypes, type initializes, unsigned int flags, scislPeephole optimizer)
	{
		registeredFunc&& tmp = { stlFuncs::stlFuncCount, id, func, optimizer, argCount, minArgs, argTypes, initializes, flags};
		interopFuncTable.insert({ id, tmp });
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