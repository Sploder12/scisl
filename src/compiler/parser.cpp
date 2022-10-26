#include "parser.h"

#include "../runtime/stl.h"

#include <iostream>

namespace scisl {

	constexpr bool isNumeric(char c) {
		return (c >= '0' && c <= '9');
	}

	std::vector<std::string_view> split(std::string_view in, char delim) {
		std::vector<std::string_view> out{};

		bool inQuote = false;
		size_t cur = 0;
		for (size_t i = 0; i < in.size(); ++i) {
			char c = in[i];

			if (c == delim && !inQuote) {
				out.emplace_back(in.data() + cur, i - cur);
				cur = i + 1;
			}
			else if (c == '"') {
				inQuote = !inQuote;
			}
		}

		if (cur < in.size()) {
			out.emplace_back(in.data() + cur, in.size() - cur);
		}

		return out;
	}

	IntermediateInstr parseLine(std::string_view line) {
		IntermediateInstr out{};

		auto tokens = split(line, ' ');

		if (tokens.size() == 0) return out;

		auto func = strToStlFunc(tokens[0]);
		
		out.func = func;
		out.strName = tokens[0];

		for (size_t i = 1; i < tokens.size(); ++i) {
			ArgType argType = ArgType::variable;
			auto cur = tokens[i];
			if (cur == "") continue;

			// this is kinda unsafe, but we will verify it later
			if (cur[0] == '$') {
				argType = ArgType::interop;
			} else if (cur[0] == '"' || cur[0] == '-' || cur[0] == '.' || isNumeric(cur[0])) {
				argType = ArgType::constant;
			}

			out.args.emplace_back(tokens[i], ValType::err, argType);
		}

		if (out.args.size() > 0) {
			const auto& meta = getMeta(out);

			// setting of constant is replaced with noop
			if (out.args[0].argType == ArgType::constant &&
				((meta.flags & funcFlags::creates) == funcFlags::creates ||
				(meta.flags & funcFlags::modifies) == funcFlags::modifies)) {

				out.func = stlFunc::noop;
			}
		}

		return out;
	}

	constexpr ValType getConstType(std::string_view value) {
		if (value.size() == 0) return ValType::err;

		if (value[0] == '"') {
			return ValType::string;
		}
		else if (value[0] == '.') {
			return ValType::floating;
		}
		else if (isNumeric(value[0]) || value[0] == '-') {
			if (value.find('.') != std::string_view::npos) {
				return ValType::floating;
			}
			else {
				return ValType::integer;
			}
		}
		else {
			return ValType::err;
		}
	}

	void setTypes(Intermediate& program) {
		for (auto& instr : program.instrs) {

			const funcMeta& meta = getMeta(instr);
			

			for (size_t i = 0; i < instr.args.size(); ++i) {
				auto& cur = instr.args[instr.args.size() - 1 - i];
				

				switch (cur.argType) {
				case (ArgType::constant):
					cur.valType = getConstType(cur.value);
					break;
				case (ArgType::variable): {

						auto it = program.vars.find(cur.value);
						if (it != program.vars.end()) { // var already defined
							cur.valType = it->second.valType;
							if (it->second.usedBy.back() != &instr) {
								it->second.usedBy.push_back(&instr);
							}
						}
						else if (instr.args.size() - 1 - i != 0) { // arg isn't first arg
							// error, variable used before initialization
						}
						else if (meta.flags & funcFlags::initializes) {
							 
							cur.valType = meta.initializes; // try to get from meta
							if (cur.valType == ValType::err && instr.args.size() > 1) {
								const auto& next = instr.args[1]; // get from next arg
								cur.valType = next.valType;
							}

							program.vars.emplace(cur.value, IntermediateVar{ cur.value, cur.valType, {&instr} });
						}
						else {
							// error, variable used before initialization
						}

						break;
					}
				case (ArgType::interop):
					
					auto& var = getVarTable()[cur.value];

					std::visit([&](auto&& inter) {
						using T = std::decay_t<decltype(inter)>;
						if constexpr (std::is_same_v<T, SCISL_INT*>)
							cur.valType = ValType::integer;
						else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
							cur.valType = ValType::floating;
						else if constexpr (std::is_same_v<T, SCISL_STR*>)
							cur.valType = ValType::string;
						else
							cur.valType = ValType::err;
					}, var.data);

					break;
				}

				if (cur.valType == ValType::err) {
					// error, could not determine type
				}
			}
		}
	}

	constexpr bool matchesTypeChr(char typeChr, ValType type) {
		switch (typeChr)
		{
		case 'a':
			return true;
		case 'n':
			return (type == ValType::integer || type == ValType::floating);
		case 'i':
			return type == ValType::integer;
		case 'f':
			return type == ValType::floating;
		case 's':
			return type == ValType::string;
		default:
			return false;
		}
	}

	bool checkArgs(const Intermediate& program) {
		return std::all_of(std::execution::par_unseq, program.instrs.begin(), program.instrs.end(), 
			[](const IntermediateInstr& instr) {

			const funcMeta& meta = getMeta(instr);

			//arg count
			if (meta.args >= 0 && meta.args != instr.args.size()) {
				// err
				return false;
			}
			else if (meta.args < 0 && (meta.args - 1) < instr.args.size()) {
				// err
				return false;
			}

			// check if args follow typeStr
			bool typeStrDone = false;
			char typeChr = 'a';

			if (meta.typeStr == nullptr || meta.typeStr[0] == '\0') {
				typeStrDone = true;
			}

			for (size_t i = 0; i < instr.args.size(); ++i) {
				auto& cur = instr.args[i];

				if (!typeStrDone) {
					typeChr = meta.typeStr[i];
					if (meta.typeStr[i + 1] == '\0') {
						typeStrDone = true;
					}
				}

				
				if (!matchesTypeChr(typeChr, cur.valType)) {
					// err bad arg type
					return false;
				}
			}

			return true;
		});
	}

	Intermediate parse(std::string_view in) {
		Intermediate out{};

		auto lines = split(in, '\n');
		out.instrs.reserve(lines.size());

		for (auto line : lines) {
			auto&& intermediate = parseLine(line);
			if (intermediate.func != stlFunc::noop && intermediate.strName != "") {
				out.instrs.push_back(std::move(intermediate)); // initial collection of all operations
			}
		}

		// set valtypes
		setTypes(out);

		if (!checkArgs(out)) {
			out = {};
		}

		return out;
	}
}