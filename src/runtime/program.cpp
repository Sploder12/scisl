#include "program.h"

#include "../interop/interop.h"
#include "stl.h"

#include <fstream>
#include <string>


namespace scisl {
	void Program::decompile(const char* filename) {
		std::ofstream file(filename);

		const auto& fTable = getFuncTable();
		const auto& vTable = getVarTable();

		for (const auto& instr : instructions) {
			
			bool stl = false;
			for (const auto& meta : stlFuncMeta) {
				if (instr.func == meta.def) {
					file << stlNames[(size_t)meta.func];
					stl = true;
					break;
				}
			}

			if (!stl) {
				for (const auto& meta : fTable) {
					if (instr.func == meta.second.def) {
						file << meta.first;
						break;
					}
				}
			}

			for (const auto& arg : instr.args) {
				file << ' ';

				
				bool interop = false;
				for (const auto& v : vTable) {
					if (arg.data == v.second.data) {
						file << v.first;
						interop = true;
						break;
					}
				}

				// there is no way to know if something is a variable or constant
				if (!interop) {
					file << "v" << std::to_string((size_t)arg.data);
				}

			}
			file << '\n';
		}

		file.close();
	}
}