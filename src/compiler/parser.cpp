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

		if (cur < in.size() - 1) {
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

	const funcMeta& getMeta(stlFunc func) {
		if (func != stlFunc::count) {
			return stlFuncMeta[(size_t)func];
		}
		else {
			//@TODO replace with interop
			return stlFuncMeta[(size_t)stlFunc::jmp];
		}
	}

	void setTypes(Intermediate& program) {
		for (auto& instr : program.instrs) {

			const funcMeta& meta = getMeta(instr.func);
			

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
					
					//@TODO

					break;
				}

				if (cur.valType == ValType::err) {
					// error, could not determine type
				}
			}
		}
	}

	Intermediate parse(std::string_view in) {
		Intermediate out{};

		auto lines = split(in, '\n');
		out.instrs.reserve(lines.size());

		for (auto line : lines) {
			auto&& intermediate = parseLine(line);
			if (intermediate.func != stlFunc::noop && intermediate.strName != "") {
				out.instrs.push_back(intermediate); // initial collection of all operations
			}
		}

		// set valtypes
		setTypes(out);

		return out;
	}
}