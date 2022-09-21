#include "preprocess.h"

#include "macros.h"
#include "aliases.h"

namespace scisl {
	
	enum class State : unsigned char {
		line_start,
		funct_name,
		macro_name,
		in_comment,
		in_quotes,
		inbetween,
		arg_name,

		state_count
	};

	constexpr bool isSpace(char c) {
		return (c == ' ' || c == '\t');
	}


	#ifndef SCISL_MACRO_DEPTH
	#define SCISL_MACRO_DEPTH 3
	#endif

	std::string resolveMacro(const std::string& name, unsigned char macrodepth) {
		auto macro = getMacro(name);
		if (macro != "") {
			auto tmp = preprocess(macro, macrodepth + 1);
			if (tmp != "" && tmp.back() == '\n' || isSpace(tmp.back())) {
				return tmp.substr(0, tmp.size() - 1);
			}
			else {
				return tmp;
			}
		}
		return "";
	}

	std::string_view resolveFunc(const std::string& name) {
		auto alias = nameFromAlias(name);
		if (alias == "") {
			return name;
		}
		else {
			return alias;
		}
	}

	std::string preprocess(std::string_view program, unsigned char macrodepth) {
		if (program == "" || macrodepth > SCISL_MACRO_DEPTH) // prevent infinite loop from recursive macros
			return "";

		std::string out{};

		State state = State::line_start;

		if (macrodepth > 0 && program.front() == '"') {
			state = State::inbetween;
		}

		std::string temp{};

		for (char c : program) {

			switch(state) {
			case State::line_start:
				if (!isSpace(c) && c != '\n') {
					if (c == ';') {
						state = State::in_comment;
					}
					else if (c == '#') {
						state = State::macro_name;
						temp = "";
					}
					else {
						state = State::funct_name;
						temp = c;
					}
				}
				break;

			case State::funct_name:
				if (isSpace(c) || c == '\n' || c == ';') {
					out += resolveFunc(temp);
					temp = "";

					if (c == '\n') {
						if (out.back() != '\n') out += '\n';
						state = State::line_start;
					}
					else if (c == ';') {
						state = State::in_comment;
					}
					else {
						out += ' ';
						state = State::inbetween;
					}
				}
				else {
					temp += c;
				}
				break;

			case State::macro_name:
				if (isSpace(c) || c == '\n' || c == ';') {
					out += resolveMacro(temp, macrodepth);
					temp = "";

					if (c == '\n') {
						if (out.back() != '\n') out += '\n';
						state = State::line_start;
					}
					else if (c == ';') {
						state = State::in_comment;
					}
					else {
						out += ' ';
						state = State::inbetween;
					}
				}
				else {
					temp += c;
				}
				break;

			case State::in_comment:
				if (c == '\n') {
					if (out.back() != '\n') out += '\n';
					state = State::line_start;
				}
				break;

			case State::in_quotes:
				out += c;
				if (c == '\n') {
					state = State::line_start;
				}
				else if (c == '"') {
					out += ' ';
					state = State::inbetween;
				}
				
				break;

			case State::inbetween:
				if (c == '\n') {
					if (out.back() != '\n') out += '\n';
					state = State::line_start;
				}
				else if (c == ';') {
					state = State::in_comment;
				}
				else if (c == '"') {
					state = State::in_quotes;
					out += '"';
				}
				else if (c == '#') {
					state = State::macro_name;
					temp = "";
				}
				else if (!isSpace(c)) {
					state = State::arg_name;
					out += c;
				}
				break;

			case State::arg_name:
				if (isSpace(c) || c == '\n' || c == ';') {
					if (c == '\n') {
						if (out.back() != '\n') out += '\n';
						state = State::line_start;
					}
					else if (c == ';') {
						state = State::in_comment;
					}
					else {
						out += ' ';
						state = State::inbetween;
					}
				}
				else {
					out += c;
				}
				break;

			default:
				break;
			}
		}

		if (temp != "") {
			if (state == State::macro_name) {
				out += resolveMacro(temp, macrodepth);
			}
			else {
				out += resolveFunc(temp);
			}
		}

		return out;
	}
}