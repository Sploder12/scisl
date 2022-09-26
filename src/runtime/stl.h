#ifndef _SCISL_STL_H
#define _SCISL_STL_H

#include "../preprocessor/aliases.h"
#include "../compiler/type.h"
#include "../runtime/Program.h"

#include <array>

namespace scisl {

	enum class stlFunc : unsigned short {
		set,
		add,
		adde,
		sub,
		sube,
		mult,
		multe,
		div,
		dive,
		print,

		substr,
		strlen,
		chrset,
		chrat,

		less,
		great,
		equal,
		nequal,

		land,
		lor,

		band,
		bor,
		bxor,
		lshift,
		rshift,

		label,
		jmp,
		cjmp,

		def,
		blockend,
		call,

		exit,
		breakp,
		noop,

		count
	};

	void set(Program& process, std::vector<Val>& args);
	void add(Program& process, std::vector<Val>& instr);
	void adde(Program& process, std::vector<Val>& instr);
	void sub(Program& process, std::vector<Val>& instr);
	void sube(Program& process, std::vector<Val>& instr);
	void mult(Program& process, std::vector<Val>& instr);
	void multe(Program& process, std::vector<Val>& instr);
	void div(Program& process, std::vector<Val>& instr);
	void dive(Program& process, std::vector<Val>& instr);
	void print(Program& process, std::vector<Val>& instr);

	void substr(Program& process, std::vector<Val>& instr);
	void strlen(Program& process, std::vector<Val>& instr);
	void chrset(Program& process, std::vector<Val>& instr);
	void chrat(Program& process, std::vector<Val>& instr);

	void less(Program& process, std::vector<Val>& instr);
	void great(Program& process, std::vector<Val>& instr);
	void equal(Program& process, std::vector<Val>& instr);
	void nequal(Program& process, std::vector<Val>& instr);

	void land(Program& process, std::vector<Val>& instr);
	void lor(Program& process, std::vector<Val>& instr);

	void band(Program& process, std::vector<Val>& instr);
	void bor(Program& process, std::vector<Val>& instr);
	void bxor(Program& process, std::vector<Val>& instr);
	void lshift(Program& process, std::vector<Val>& instr);
	void rshift(Program& process, std::vector<Val>& instr);

	void jmp(Program& process, std::vector<Val>& instr);
	void cjmp(Program& process, std::vector<Val>& instr);

	void defb(Program& process, std::vector<Val>& instr);
	void blockend(Program& process, std::vector<Val>& instr);
	void call(Program& process, std::vector<Val>& instr);

	void exit(Program& process, std::vector<Val>& instr);
	void breakp(Program& process, std::vector<Val>& instr);

	void noop(Program& process, std::vector<Val>& instr);

	enum funcFlags : unsigned char {
		none = 0,
		is_const = 1,
		initializes = 2,
		no_jump = 4,
		defines_block = 8,

		is_simable = 16, // only used for optimization, set if the optimizer can simulate the function

		// combinations
		creates = initializes | no_jump | is_simable,
		modifies = no_jump | is_simable,
		outputs = is_const | no_jump,
		def = is_const | defines_block
	};

	struct funcMeta {
		stlFunc func; // stfFunc::count if interop
		void (*def)(Program&, std::vector<Val>&);
		// peep pointer
		int args; // negative indicates variadic, -1 is 0 or more args, 1 is 1 arg, 0 is no args
		const char* typeStr; // types of the args, variadics use last for outer
		ValType initializes; // type if instruction can initialize a variable

		funcFlags flags; // flags to help out the compiler 
	};

	constexpr std::array<funcMeta, (size_t)(stlFunc::count)> stlFuncMeta {
		funcMeta{ stlFunc::set, set, 2, "aa", ValType::err, funcFlags::creates },
		{ stlFunc::add, add, -4, "a", ValType::err, funcFlags::creates },
		{ stlFunc::adde, adde, -3, "a", ValType::err, funcFlags::modifies },
		{ stlFunc::sub, sub, 3, "an", ValType::err, funcFlags::creates },
		{ stlFunc::sube, sube, 2, "an", ValType::err, funcFlags::modifies },
		{ stlFunc::mult, mult, -4, "an", ValType::err, funcFlags::creates },
		{ stlFunc::multe, multe, -3, "an", ValType::err, funcFlags::modifies },
		{ stlFunc::div, div, 3, "ann", ValType::err, funcFlags::creates },
		{ stlFunc::dive, dive, 2, "nn", ValType::err, funcFlags::modifies },
		{ stlFunc::print, print, -1, "a", ValType::err, funcFlags::outputs },

		{ stlFunc::substr, substr, 4, "ssii", ValType::string, funcFlags::creates },
		{ stlFunc::strlen, strlen, 2, "ns", ValType::integer, funcFlags::creates },
		{ stlFunc::chrset, chrset, 3, "sii", ValType::err, funcFlags::modifies },
		{ stlFunc::chrat, chrat, 3, "nsi", ValType::integer, funcFlags::creates },

		{ stlFunc::less, less, 3, "nnn", ValType::integer, funcFlags::creates },
		{ stlFunc::great, great, 3, "nnn", ValType::integer, funcFlags::creates },
		{ stlFunc::equal, equal, -4, "naa", ValType::integer, funcFlags::creates },
		{ stlFunc::nequal, nequal, 3, "naa", ValType::integer, funcFlags::creates },

		{ stlFunc::land, land, -3, "nn", ValType::integer, funcFlags::creates },
		{ stlFunc::lor, lor, -3, "nn", ValType::integer, funcFlags::creates },

		{ stlFunc::band, band, 2, "ii", ValType::err, funcFlags::modifies },
		{ stlFunc::bor, bor, 2, "ii", ValType::err, funcFlags::modifies },
		{ stlFunc::bxor, bxor, 2, "ii", ValType::err, funcFlags::modifies },
		{ stlFunc::lshift, lshift, 2, "ii", ValType::err, funcFlags::modifies },
		{ stlFunc::rshift, rshift, 2, "ii", ValType::err, funcFlags::modifies },

		{ stlFunc::label, noop, 1, "a", ValType::err, funcFlags::is_const },
		{ stlFunc::jmp, jmp, 1, "a", ValType::err, funcFlags::is_const },
		{ stlFunc::cjmp, cjmp, 2, "an", ValType::err, funcFlags::is_const },

		{ stlFunc::def, defb, 1, "a", ValType::err, funcFlags::def },
		{ stlFunc::blockend, blockend, -1, "", ValType::err, funcFlags::is_const },
		{ stlFunc::call, call, 1, "a", ValType::err, funcFlags::is_const },

		{ stlFunc::exit, exit, 1, "i", ValType::err, funcFlags::outputs },
		{ stlFunc::breakp, breakp, 1, "i", ValType::err, funcFlags::outputs },
		{ stlFunc::noop, noop, -1, "", ValType::err, funcFlags::outputs }
	};

	constexpr bool metaIsValid(funcMeta meta) {
		if ((meta.flags & funcFlags::is_const) && (meta.flags & funcFlags::initializes)) {
			return false;
		}

		const char* argTypes = meta.typeStr;
		while (*argTypes != '\0') {
			char c = *argTypes;

			if (c != 'i' && c != 'n' && c != 's' && c != 'f' && c != 'a') {
				return false;
			}

			++argTypes;
		}

		return true;
	}

	constexpr bool verifyStlMeta(std::array<funcMeta, (size_t)(stlFunc::count)> meta) {
		for (size_t i = 0; i < meta.size(); ++i) {
			const auto& cur = meta[i];

			if (cur.func != stlFunc(i)) {
				return false;
			}

			if (!metaIsValid(cur)) {
				return false;
			}

		}
		return true;
	}

	static_assert(verifyStlMeta(stlFuncMeta), "Scisl STL Metadata is malformed.");

	stlFunc strToStlFunc(std::string_view str);
}

#endif