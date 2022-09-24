#include "compile.h"

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

	Program compile(Intermediate& program) {
		Program out{};



		// @TODO optimizations


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
					// @TODO
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
}