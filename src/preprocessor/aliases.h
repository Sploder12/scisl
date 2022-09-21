#ifndef _SCISL_ALIASES_H
#define _SCISL_ALIASES_H

#include <string>
#include <unordered_set>
#include <array>

#include <algorithm>
#include <execution>

namespace scisl {
	constexpr char toupper(char chr) {
		if (chr >= 'a' && chr <= 'z') {
			return chr + ('A' - 'a');
		}
		return chr;
	}

	constexpr std::array<std::string_view, 34> stlNames{
		"SET",
		"ADD",
		"ADDE",
		"SUB",
		"SUBE",
		"MULT",
		"MULTE",
		"DIV",
		"DIVE",
		"PRINT",

		"SUBSTR",
		"STRLEN",
		"CHRSET",
		"CHRAT",

		"LESS",
		"GREAT",
		"EQUAL",
		"NEQUAL",

		"LAND",
		"LOR",

		"BAND",
		"BOR",
		"BXOR",
		"LSHIFT",
		"RSHIFT",

		"LABEL",
		"JMP",
		"CJMP",

		"DEF",
		"BLOCKEND",
		"CALL",

		"EXIT",
		"BREAK",
		"NOOP"
	};

	typedef std::unordered_set<std::string> alias_set;

#ifndef SCISL_NO_ALIASES
	const std::array<alias_set, 34> stlAliases = {
		alias_set{ "SET", "VAR", "LET", "=" },
		alias_set{ "ADD", "PLUS", "+" },
		alias_set{ "ADDE", "PLUSE", "PLUSEQUALS", "ADDEQUALS", "+=", "PLUS_EQUALS" },
		alias_set{ "SUB", "MINUS", "SUBTRACT", "-" },
		alias_set{ "SUBE", "MINUSE", "MINUSEQUALS", "SUBEQUALS", "SUBTRACTEQUALS", "-=", "MINUS_EQUALS" },
		alias_set{ "MULT", "TIMES", "MULTIPLY", "*" },
		alias_set{ "MULTE", "TIMESE", "TIMESEQUALS", "MULTEQUALS", "MULTIPLYEQUALS", "*=", "TIMES_EQUALS" },
		alias_set{ "DIV", "DIVIDE", "/" },
		alias_set{ "DIVE", "DIVEQUALS", "DIVIDEEQUALS", "/=", "DIVIDE_EQUALS" },
		alias_set{ "PRINT", "COUT", "PRINTLN", "PRINTLINE", "PRINT_LINE" },
		alias_set{ "SUBSTR", "SUBSTRING", "SUB_STRING" },
		alias_set{ "STRLEN", "STRINGLEN", "STRLENGTH", "STRINGLENGTH", "LENSTR", "STRING_LENGTH" },
		alias_set{ "CHRSET", "CHARSET", "SETCHAR", "SETCHR", "SET_CHAR" },
		alias_set{ "CHRAT", "CHARAT", "CHAR_AT"},
		alias_set{ "LESS", "LESSER", "<", "LESS_THAN" },
		alias_set{ "GREAT", "GREATER", ">", "GREATER_THAN" },
		alias_set{ "EQUAL", "EQUALS", "==", "EQUAL_TO", "SAME?"},
		alias_set{ "NEQUAL", "NEQUALS", "NOTEQUAL", "NOTEQUALS", "!=", "NOT_EQUALS"},
		alias_set{ "LAND", "LOGICALAND", "AND", "&&", "LOGICAL_AND" },
		alias_set{ "LOR", "LOGICALOR", "OR", "||", "LOGICAL_OR" },
		alias_set{ "BAND", "BITWISEAND", "&", "BITWISE_AND" },
		alias_set{ "BOR", "BITWISEOR", "|", "BITWISE_OR" },
		alias_set{ "BXOR", "BITWISEXOR", "XOR", "^", "BITWISE_XOR" },
		alias_set{ "LSHIFT", "LEFTSHIFT", "<<", "LEFT_SHIFT" },
		alias_set{ "RSHIFT", "RIGHTSHIFT", ">>", "RIGHT_SHIFT" },
		alias_set{ "LABEL", "MARK" },
		alias_set{ "JMP", "JUMP", "GOTO" },
		alias_set{ "CJMP", "CJUMP", "CONDITIONALJMP", "CONDITIONALJUMP", "CGOTO", "CONDITIONALGOTO", "CONDITIONAL_JUMP" },
		alias_set{ "DEF", "DEFINE", "FNC", "FUNC", "FUNCTION", "{[]"},
		alias_set{ "BLOCKEND", "BLOCK_END", "CLOSEBLOCK", "CLOSE_BLOCK", "ENDBLOCK", "END_BLOCK", "EB", "[]}" },
		alias_set{ "CALL", "RUN" },
		alias_set{ "EXIT", "FINISH", "END" },
		alias_set{ "BREAK", "HALT" },
		alias_set{ "NOOP", "NOP", "NOTHING" }
	};

	std::string_view nameFromAlias(std::string alias) {
		std::transform(std::execution::unseq, alias.begin(), alias.end(), alias.begin(), toupper);

		for (size_t i = 0; i < stlAliases.size(); ++i) {
			if (stlAliases[i].contains(alias)) {
				return stlNames[i];
			}
		}

		return "";
	}
#else
	std::string_view nameFromAlias(std::string alias) {
		std::transform(std::execution::unseq, alias.begin(), alias.end(), alias.begin(), toupper);

		for (size_t i = 0; i < stlNames.size(); ++i) {
			if (stlNames[i] == alias) {
				return stlNames[i];
			}
		}

		return "";
	}
#endif

	
	
}

#endif