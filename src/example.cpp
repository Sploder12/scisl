#include "preprocessor/preprocess.h"
#include "preprocessor/macros.h"

#include "compiler/parser.h"
#include "compiler/compile.h"

#include "compiler/optimizer.h"

#include "interop/interop.h"

#include <iostream>
#include <fstream>

#include <chrono>
#define TIME(funntions)\
	auto s = std::chrono::system_clock::now();\
	funntions\
	auto e = std::chrono::system_clock::now();\
	std::cout << "Took " << (e - s).count() / 10000000.0 << "s \n"\

int main()
{
	
	int number = 5;

	scisl::registerVar("num", &number);

	scisl::defineMacro("macro!", "-1");

	scisl::defineMacro("hello", "\"Hello\"");
	scisl::defineMacro("world", "\"World\"");
	scisl::defineMacro("helloworld", "#hello #world");

	scisl::defineMacro("maybeAnInstruction", "= num1 13.37");

	std::ifstream file("src/example.scisl");
	std::string in{};
	std::string line;
	while (std::getline(file, line)) {
		in += line + '\n';
	}

	auto preprocessed = scisl::preprocess(in.substr(0, in.size() - 1));
	auto parsed = scisl::parse(preprocessed);
	optimize(parsed);
	auto prog = scisl::compile(parsed);

	std::cout << preprocessed << "\n\n";

	prog.decompile("DECOMPexample.scisl");

	return prog.run();
}