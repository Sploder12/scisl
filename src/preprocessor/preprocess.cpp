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


	std::string preprocess(std::string_view program) {
		std::string out{};

		State state = State::line_start;
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
					auto alias = nameFromAlias(temp);
					if (alias == "") {
						out += temp;
					}
					else {
						out += alias;
					}

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
					auto macro = getMacro(temp);
					if (macro != "") {
						out += macro;
					}

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


		return out;
	}
}