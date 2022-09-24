#include "stl.h"

#include <iostream>
#include <algorithm>
#include <execution>

namespace scisl {
	stlFunc strToStlFunc(std::string_view str) {
		for (size_t i = 0; i < stlNames.size(); ++i) {
			if (str == stlNames[i]) {
				return (stlFunc)(i);
			}
		}

		return stlFunc::count;
	}

	void set(Program& process, std::vector<Val>& args) {
		args[0] = args[1];
	}

	void add(Program& process, std::vector<Val>& args) {
		Val& out = args[0];
		out = args[1]; // initial set
		for (size_t i = 2; i < args.size(); ++i) {
			out += args[i];
		}
	}

	void adde(Program& process, std::vector<Val>& args) {
		Val& out = args[0];
		for (size_t i = 1; i < args.size(); ++i) {
			out += args[i];
		}
	}

	void sub(Program& process, std::vector<Val>& args) {
		Val& out = args[0];
		out = args[1];
		out -= args[2];
	}

	void sube(Program& process, std::vector<Val>& args) {
		args[0] -= args[1];
	}

	void mult(Program& process, std::vector<Val>& args) {
		Val& out = args[0];
		out = args[1]; // initial set
		std::for_each(std::execution::unseq, args.begin() + 2, args.end(),
			[&out](const Val& v) {
			out *= v;
		});
	}

	void multe(Program& process, std::vector<Val>& args) {
		Val& out = args[0];
		std::for_each(std::execution::unseq, args.begin() + 1, args.end(),
			[&out](const Val& v) {
			out *= v;
		});
	}

	void div(Program& process, std::vector<Val>& args) {
		Val& out = args[0];
		out = args[1];
		out /= args[2];
	}

	void dive(Program& process, std::vector<Val>& args) {
		args[0] /= args[1];
	}

	void print(Program& process, std::vector<Val>& args) {
		for (const auto& arg : args) {
			std::cout << arg.asStr();
		}
		std::cout << '\n';
	}


	void substr(Program& process, std::vector<Val>& args) {
		Val& out = args[0];

		const SCISL_STR& str = *cast<SCISL_STR>(args[1]);
		size_t start = *cast<SCISL_INT>(args[2]);
		size_t end = *cast<SCISL_INT>(args[3]);

		out = str.substr(start, end - start);
	}

	void strlen(Program& process, std::vector<Val>& args) {
		args[0] = (SCISL_INT)cast<SCISL_STR>(args[1])->size();
	}

	void chrset(Program& process, std::vector<Val>& args) {
		SCISL_STR& str = *cast<SCISL_STR>(args[0]);
		size_t idx = *cast<SCISL_INT>(args[1]);
		SCISL_INT chr = *cast<SCISL_INT>(args[2]);

		str[idx] = chr;
	}

	void chrat(Program& process, std::vector<Val>& args) {
		const SCISL_STR& str = *cast<SCISL_STR>(args[1]);
		size_t idx = *cast<SCISL_INT>(args[2]);

		args[0] = str[idx];
	}

	
	void less(Program& process, std::vector<Val>& args) {
		args[0] = args[1] < args[2];
	}

	void great(Program& process, std::vector<Val>& args) {
		args[0] = args[1] > args[2];
	}

	void equal(Program& process, std::vector<Val>& args) {

		const Val& first = args[1];

		args[0] = std::all_of(std::execution::unseq, args.begin() + 2, args.end(), 
		[&first](const Val& v) {
			return first == v;
		});
	}

	void nequal(Program& process, std::vector<Val>& args) {
		args[0] = args[1] != args[2];
	}


	void land(Program& process, std::vector<Val>& args) {
		Val& out = args[0];
		for (size_t i = 1; i < args.size(); ++i) {
			bool res = out && args[i];

			out = res;

			if (!res) return; // short circuit eval
		}
	}

	void lor(Program& process, std::vector<Val>& args) {
		Val& out = args[0];
		for (size_t i = 1; i < args.size(); ++i) {
			bool res = out || args[i];

			out = res;

			if (res) return; // short circuit eval
		}
	}

	void band(Program& process, std::vector<Val>& args) {
		args[0] &= args[1];
	}

	void bor(Program& process, std::vector<Val>& args) {
		args[0] |= args[1];
	}

	void bxor(Program& process, std::vector<Val>& args) {
		args[0] ^= args[1];
	}

	void lshift(Program& process, std::vector<Val>& args) {
		args[0] <<= args[1];
	}

	void rshift(Program& process, std::vector<Val>& args) {
		args[0] >>= args[1];
	}


	void jmp(Program& process, std::vector<Val>& args) {
		process.rip = (size_t)*cast<SCISL_INT>(args[0]);
	}

	void cjmp(Program& process, std::vector<Val>& args) {
		if (args[1].asFloat() > 0) {
			process.rip = (size_t)*cast<SCISL_INT>(args[0]);
		}
	}

	void defb(Program& process, std::vector<Val>& args) {
		process.rip = (size_t)*cast<SCISL_INT>(args[0]); //behaves as a jmp
	}

	void blockend(Program& process, std::vector<Val>& args) {
		if (!process.callstack.empty()) {
			process.rip = process.callstack.top();
			process.callstack.pop();
		}
	}

	void call(Program& process, std::vector<Val>& args) {
		process.callstack.emplace(process.rip);
		process.rip = (size_t)*cast<SCISL_INT>(args[0]) + 1; 
	}

	void exit(Program& process, std::vector<Val>& args) {
		process.returnVal = *cast<SCISL_INT>(args[0]);
		process.rip = process.instructions.size();
		process.callstack = {};
	}

	void breakp(Program& process, std::vector<Val>& args) {
		if (process.callstack.empty()) {
			process.broke = true;
			process.returnVal = *cast<SCISL_INT>(args[0]);
		}
		else { // inside block
			process.rip = process.callstack.top();
			process.callstack.pop();
		}
	}

	void noop(Program& process, std::vector<Val>& instr){}
}