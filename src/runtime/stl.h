#ifndef _SCISL_STL_H
#define _SCISL_STL_H

#include "../preprocessor/aliases.h"
#include "../compiler/type.h"

#include <functional>
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

	enum funcFlags : unsigned char {
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
		// instruction pointer
		// peep pointer
		int args; // negative indicates variadic, -1 is 0 or more args, 1 is 1 arg, 0 is no args
		const char* typeStr; // types of the args, variadics use last for outer
		ValType initializes; // type if instruction can initialize a variable

		funcFlags flags; // flags to help out the compiler 
	};

	constexpr std::array<funcMeta, (size_t)(stlFunc::count)> stlFuncMeta {
		funcMeta{ stlFunc::set, 2, "aa", ValType::err, funcFlags::creates },
		{ stlFunc::add, -3, "a", ValType::err, funcFlags::creates },
		{ stlFunc::adde, -3, "a", ValType::err, funcFlags::modifies },
		{ stlFunc::sub, -3, "an", ValType::err, funcFlags::creates },
		{ stlFunc::sube, -3, "an", ValType::err, funcFlags::modifies },
		{ stlFunc::mult, -3, "an", ValType::err, funcFlags::creates },
		{ stlFunc::multe, -3, "an", ValType::err, funcFlags::modifies },
		{ stlFunc::div, 3, "ann", ValType::err, funcFlags::creates },
		{ stlFunc::dive, 2, "nn", ValType::err, funcFlags::modifies },
		{ stlFunc::print, -1, "a", ValType::err, funcFlags::outputs },

		{ stlFunc::substr, 4, "ssii", ValType::string, funcFlags::creates },
		{ stlFunc::strlen, 2, "ns", ValType::integer, funcFlags::creates },
		{ stlFunc::chrset, 3, "sii", ValType::err, funcFlags::modifies },
		{ stlFunc::chrat, 3, "nsi", ValType::integer, funcFlags::creates },

		{ stlFunc::less, 3, "nnn", ValType::integer, funcFlags::creates },
		{ stlFunc::great, 3, "nnn", ValType::integer, funcFlags::creates },
		{ stlFunc::equal, -4, "naa", ValType::integer, funcFlags::creates },
		{ stlFunc::nequal, 3, "naa", ValType::integer, funcFlags::creates },

		{ stlFunc::land, -3, "nn", ValType::integer, funcFlags::creates },
		{ stlFunc::lor, -3, "nn", ValType::integer, funcFlags::creates },

		{ stlFunc::band, 2, "ii", ValType::err, funcFlags::modifies },
		{ stlFunc::bor, 2, "ii", ValType::err, funcFlags::modifies },
		{ stlFunc::bxor, 2, "ii", ValType::err, funcFlags::modifies },
		{ stlFunc::lshift, 2, "ii", ValType::err, funcFlags::modifies },
		{ stlFunc::rshift, 2, "ii", ValType::err, funcFlags::modifies },

		{ stlFunc::label, 1, "a", ValType::err, funcFlags::is_const },
		{ stlFunc::jmp, 1, "a", ValType::err, funcFlags::is_const },
		{ stlFunc::cjmp, 2, "an", ValType::err, funcFlags::is_const },

		{ stlFunc::def, 1, "a", ValType::err, funcFlags::def },
		{ stlFunc::blockend, -1, "", ValType::err, funcFlags::is_const },
		{ stlFunc::call, 1, "a", ValType::err, funcFlags::is_const },

		{ stlFunc::exit, 1, "i", ValType::err, funcFlags::outputs },
		{ stlFunc::breakp, 1, "i", ValType::err, funcFlags::outputs },
		{ stlFunc::noop, -1, "", ValType::err, funcFlags::outputs }
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