#include "compile.h"

#include "../preprocessor/preprocess.h"
#include "parser.h"

#include "../interop/interop.h"

#include <fstream>
#include <sstream>

namespace scisl {

	constexpr size_t sizeOf(ValType type) {
		switch (type)
		{
		case scisl::ValType::integer:
			return sizeof(SCISL_INT);
		case scisl::ValType::floating:
			return sizeof(SCISL_FLOAT);
		case scisl::ValType::string:
			return sizeof(SCISL_STR);
		default:
			return 0;
		}
	}

	size_t getRequiredData(const Intermediate& program) {
		size_t out = 0;

		std::unordered_set<std::string> usedVars{};
		std::unordered_set<std::string> usedStrs{};

		for (const auto& instr : program.instrs) {
			for (const auto& arg : instr.args) {
				switch (arg.argType)
				{
				case ArgType::constant:

					if (arg.valType == ValType::string) {
						if (usedStrs.insert(arg.value).second) {
							out += sizeof(SCISL_STR);
						}
					}
					else {
						out += sizeOf(arg.valType);
					}
					
					break;
				case ArgType::variable:
					if (usedVars.insert(arg.value).second) {
						out += sizeOf(arg.valType);
					}
					break;
				default:
					break;
				}
			}
		}

		return out;
	}

	inline void* dataNew(uint8_t* buf, ValType type) {
		switch (type)
		{
		case scisl::ValType::integer:
			return new(buf)(SCISL_INT);
		case scisl::ValType::floating:
			return new(buf)(SCISL_FLOAT);
		case scisl::ValType::string:
			return new(buf)(SCISL_STR);
		default:
			return nullptr;
		}
	}

	size_t getBlockEnd(const std::vector<IntermediateInstr>& instrs, size_t offset) {
		size_t blocks = 1;
		for (size_t i = offset + 1; i < instrs.size(); ++i) {
			const auto& cur = instrs[i];
			const auto& meta = getMeta(cur);
			if (meta.flags & funcFlags::defines_block) {
				++blocks;
			}
			else if (cur.func == stlFunc::blockend) {
				if (--blocks == 0) return i;
			}
		}
		return instrs.size();
	}

	void setJumps(Intermediate& program) {
		std::unordered_map<std::string, size_t> labels;

		// get label info
		for (size_t i = 0; i < program.instrs.size(); ++i) {
			auto& cur = program.instrs[i];

			if (cur.func == stlFunc::label || cur.func == stlFunc::def) {
				labels.emplace(cur.args[0].value, i);

				if (cur.func == stlFunc::def) {
					cur.args[0].valType = ValType::integer;
					cur.args[0].argType = ArgType::constant;
					cur.args[0].value = std::to_string(getBlockEnd(program.instrs, i));
				}
			}
		}

		// set jumps
		for (auto& instr : program.instrs) {
			if (instr.func == stlFunc::jmp || instr.func == stlFunc::cjmp || instr.func == stlFunc::call) {
				auto& cur = instr.args[0];
				cur.valType = ValType::integer;
				cur.argType = ArgType::constant;
				cur.value = std::to_string(labels[cur.value]);
			}
		}
	}

	Program compile(Intermediate& program) {
		Program out{};



		// @TODO optimizations

		setJumps(program);

		out.dataSize = getRequiredData(program);
		out.data = new uint8_t[out.dataSize]();
		uint8_t* buf = out.data;

		std::unordered_map<std::string, void*> vars;
		std::unordered_map<std::string, SCISL_STR*> strConsts;

		for (const auto& [id, var] : program.vars) {

			void* addr = dataNew(buf, var.valType);
			buf += sizeOf(var.valType);
			
			vars.emplace(id, addr);
			if (var.valType == ValType::string) {
				out.strs.push_back((SCISL_STR*)addr);
			}
		}

		for (const auto& instr : program.instrs) {
			const auto& meta = getMeta(instr);
			Instruction cInstr;
			cInstr.func = meta.def;
			
			for (const auto& arg : instr.args) {
				Val v{};
				v.valtype = arg.valType;

				switch (arg.argType)
				{
				case ArgType::constant:
					if (arg.valType == ValType::string) {
						auto it = strConsts.find(arg.value);
						if (it != strConsts.end()) {
							v.data = it->second;
						}
						else {
							v.data = dataNew(buf, arg.valType);
							buf += sizeof(SCISL_STR);
							out.strs.push_back((SCISL_STR*)v.data);
							v = arg.value.substr(1, arg.value.size() - 2);

							strConsts.emplace(arg.value, (SCISL_STR*)v.data);
						}
					}
					else {
						v.data = dataNew(buf, arg.valType);
						buf += sizeOf(arg.valType);
						v = arg.value;
					}

					break;
				case ArgType::variable:
					v.data = vars[arg.value];
					break;
				case ArgType::interop:
					v.data = getVarTable()[arg.value].data;
					break;
				default:
					break;
				}

				cInstr.args.push_back(std::move(v));
			}

			out.instructions.push_back(std::move(cInstr));
		}
		

		return out;
	}

	Program compile(const char* filename) {
		std::ifstream file(filename);

		std::stringstream buf;
		buf << file.rdbuf();
		const std::string code = buf.str();

		const auto&& prepro = preprocess(code);
		auto&& parsed = parse(prepro);
		return compile(parsed);
	}
}