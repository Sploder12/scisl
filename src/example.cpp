﻿#include "preprocessor/preprocess.h"
#include "preprocessor/macros.h"

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
	scisl::defineMacro("macro!", "10");

	std::ifstream file("src/example.scisl");
	std::string in{};
	std::string line;
	while (std::getline(file, line)) {
		in += line + '\n';
	}

	auto preprocessed = scisl::preprocess(in.substr(0, in.size() - 1));

	std::cout << preprocessed;
	

	return 0;
}