#include "optimizer.h"

namespace scisl {
	void setpeep(IntermediateInstr& instr) {
		if (instr.args[0].value == instr.args[1].value) {
			instr.func = stlFunc::noop;
		}
	}

	void removeIdentity(IntermediateInstr& instr, size_t start, SCISL_INT identityVal) {
		std::vector<IntermediateArg> remaining{};
		remaining.reserve(instr.args.size());

		for (size_t i = 0; i < start; ++i) {
			remaining.emplace_back(std::move(instr.args[i]));
		}

		for (size_t i = start; i < instr.args.size(); ++i) {
			auto& cur = instr.args[i];
			if (cur.argType == ArgType::constant) {
				switch (cur.valType) {
				case ValType::integer:
					if (std::stoi(cur.value) != identityVal) {
						remaining.emplace_back(std::move(cur));
					}
					break;
				case ValType::floating:
					if (std::stod(cur.value) != identityVal) {
						remaining.emplace_back(std::move(cur));
					}
					break;
				default:
					remaining.emplace_back(std::move(cur));
				}
			}
			else {
				remaining.emplace_back(std::move(cur));
			}
		}

		instr.args = std::move(remaining);
	}

	void addepeep(IntermediateInstr& instr) {
		removeIdentity(instr, 1, 0);

		if (instr.args.size() == 1) {
			instr.func = stlFunc::noop;
		}
	}

	void addpeep(IntermediateInstr& instr) {
		auto& args = instr.args;

		switch (args[0].valType)
		{
		case ValType::integer:
		case ValType::floating:
			removeIdentity(instr, 1, 0);

			//@TODO to adde
			break;
		case ValType::string: // strings do not follow commutative property

			// can be turned into adde
			if (args[0].value == args[1].value) {
				instr.func = stlFunc::adde;
				std::shift_left(args.begin(), args.end(), 1);
				instr.args.pop_back();
				addepeep(instr);
				return;
			}
			break;
		default:
			break;
		}

		if (instr.args.size() == 1) {
			instr.func = stlFunc::noop;
		}
		else if (instr.args.size() == 2) {
			instr.func = stlFunc::set;
			setpeep(instr);
			return;
		}
	}

	void subepeep(IntermediateInstr& instr) {
		removeIdentity(instr, 1, 0);

		if (instr.args.size() == 1) {
			instr.func = stlFunc::noop;
		}
	}

	void subpeep(IntermediateInstr& instr) {

		if (instr.args[1].value == instr.args[2].value) {
			instr.func = stlFunc::set;
			instr.args[1].value = "0";
			instr.args[1].argType = ArgType::constant;
			instr.args[1].valType = ValType::integer;
			instr.args.pop_back();
			setpeep(instr);
			return;
		}

		if (instr.args[0].value == instr.args[1].value) {
			instr.func = stlFunc::sube;
			instr.args[1] = instr.args[2];
			instr.args.pop_back();
			subepeep(instr);
			return;
		}

		removeIdentity(instr, 1, 0);

		if (instr.args.size() == 2) {
			instr.func = stlFunc::set;
			instr.args[1].value = "0";
			instr.args[1].argType = ArgType::constant;
			instr.args[1].valType = ValType::integer;
			instr.args.pop_back();
			setpeep(instr);
		}
		
	}

	void multepeep(IntermediateInstr& instr) {
		removeIdentity(instr, 1, 1);

		if (instr.args.size() == 1) {
			instr.func = stlFunc::noop;
		}
	}

	void multpeep(IntermediateInstr& instr) {

	}

	void divepeep(IntermediateInstr& instr) {
		if (instr.args[0].value == instr.args[1].value) {
			instr.func = stlFunc::set;
			instr.args[1].value = "1";
			instr.args[1].argType = ArgType::constant;
			instr.args[1].valType = ValType::integer;
			setpeep(instr);
			return;
		}

		removeIdentity(instr, 1, 1);

		if (instr.args.size() == 1) {
			instr.func = stlFunc::noop;
		}
	}

	void divpeep(IntermediateInstr& instr) {
		if (instr.args[1].value == instr.args[2].value) { // yes, 0/0 = 1
			instr.func = stlFunc::set;
			instr.args[1].value = "1";
			instr.args[1].argType = ArgType::constant;
			instr.args[1].valType = ValType::integer;
			instr.args.pop_back();
			setpeep(instr);
			return;
		}

		if (instr.args[0].value == instr.args[1].value) {
			instr.func = stlFunc::dive;
			instr.args[1] = instr.args[2];
			divepeep(instr);
			return;
		}

		removeIdentity(instr, 2, 1);

		if (instr.args.size() == 2) {
			instr.func = stlFunc::set;
			setpeep(instr);
			return;
		}
	}

	void printpeep(IntermediateInstr& instr) {
		if (instr.args.size() == 0) {
			instr.func = stlFunc::noop;
		}
	}


	void substrpeep(IntermediateInstr& instr) {

	}

	void strlenpeep(IntermediateInstr& instr) {

	}

	void chrsetpeep(IntermediateInstr& instr) {

	}

	void chratpeep(IntermediateInstr& instr) {

	}


	void lesspeep(IntermediateInstr& instr) {
		if (instr.args[1].value == instr.args[2].value) {
			instr.func = stlFunc::set;
			instr.args[1].value = "0";
			instr.args[1].valType = ValType::integer;
			instr.args[1].argType = ArgType::constant;
			instr.args.pop_back();
			setpeep(instr);
			return;
		}
	}

	void greatpeep(IntermediateInstr& instr) {
		if (instr.args[1].value == instr.args[2].value) {
			instr.func = stlFunc::set;
			instr.args[1].value = "0";
			instr.args[1].valType = ValType::integer;
			instr.args[1].argType = ArgType::constant;
			instr.args.pop_back();
			setpeep(instr);
			return;
		}
	}

	void equalpeep(IntermediateInstr& instr) {

	}

	void nequalpeep(IntermediateInstr& instr) {
		if (instr.args[1].value == instr.args[2].value) {
			instr.func = stlFunc::set;
			instr.args[1].value = "0";
			instr.args[1].valType = ValType::integer;
			instr.args[1].argType = ArgType::constant;
			instr.args.pop_back();
			setpeep(instr);
			return;
		}
	}


	void landpeep(IntermediateInstr& instr) {

	}

	void lorpeep(IntermediateInstr& instr) {

	}


	void bandpeep(IntermediateInstr& instr) {
		if (instr.args[0].value == instr.args[1].value) {
			instr.func = stlFunc::noop;
			return;
		}
	}
	
	void borpeep(IntermediateInstr& instr) {
		if (instr.args[0].value == instr.args[1].value) {
			instr.func = stlFunc::noop;
			return;
		}

		removeIdentity(instr, 1, 0);

		if (instr.args.size() == 1) {
			instr.func = stlFunc::noop;
		}
	}
	
	void bxorpeep(IntermediateInstr& instr) {
		if (instr.args[0].value == instr.args[1].value) {
			instr.func = stlFunc::set;
			instr.args[1].valType = ValType::integer;
			instr.args[1].argType = ArgType::constant;
			instr.args[1].value = "0";
			setpeep(instr);
			return;
		}

		removeIdentity(instr, 1, 0);

		if (instr.args.size() == 1) {
			instr.func = stlFunc::noop;
		}
	}
	
	void lshiftpeep(IntermediateInstr& instr) {
		removeIdentity(instr, 1, 0);

		if (instr.args.size() == 1) {
			instr.func = stlFunc::noop;
		}
	}
	
	void rshiftpeep(IntermediateInstr& instr) {
		removeIdentity(instr, 1, 0);

		if (instr.args.size() == 1) {
			instr.func = stlFunc::noop;
		}
	}

	void cjmppeep(IntermediateInstr& instr) {
		if (instr.args[1].argType == ArgType::constant) {
			switch (instr.args[1].valType)
			{
			case ValType::integer:
				if (std::stol(instr.args[1].value) > 0) {
					instr.func = stlFunc::jmp;
					instr.args.pop_back();
				} else {
					instr.func = stlFunc::noop;
				}
				break;
			case ValType::floating:
				if (std::stod(instr.args[1].value) > 0) {
					instr.func = stlFunc::jmp;
					instr.args.pop_back();
				}
				else {
					instr.func = stlFunc::noop;
				}
				break;
			default:
				break;
			}
		}
	}


	constexpr std::array<void(*)(IntermediateInstr&), (size_t)stlFunc::count> stlPeepOpts{
		setpeep,
		addpeep,
		addepeep,
		subpeep,
		subepeep,
		multpeep,
		multepeep,
		divpeep,
		divepeep,
		printpeep,

		substrpeep,
		strlenpeep,
		chrsetpeep,
		chratpeep,

		lesspeep,
		greatpeep,
		equalpeep,
		nequalpeep,

		landpeep,
		lorpeep,

		bandpeep,
		borpeep,
		bxorpeep,
		lshiftpeep,
		rshiftpeep,

		nullptr,
		nullptr,
		cjmppeep,

		nullptr,
		nullptr,
		nullptr,

		nullptr,
		nullptr,
		nullptr
	};

	void removeNoop(Intermediate& program) {
		std::vector<IntermediateInstr> remaining{};
		remaining.reserve(program.instrs.size());

		for (auto& instr : program.instrs) {
			if (instr.func != stlFunc::noop) {
				remaining.emplace_back(std::move(instr));
			}
		}

		program.instrs = std::move(remaining);
	}

	void applyPeep(Intermediate& program) {
		std::for_each(std::execution::par_unseq, program.instrs.begin(), program.instrs.end(),
			[](IntermediateInstr& instr) {

			if (instr.func != stlFunc::count) {
				auto peep = stlPeepOpts[(size_t)instr.func];

				if (peep != nullptr) {
					peep(instr);
				}
			}
		});
	}

	void foldConstants(Intermediate& program) {
		
	}

	void optimize(Intermediate& program) {
		applyPeep(program);
		removeNoop(program);


	}
}