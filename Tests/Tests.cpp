#include "CppUnitTest.h"

#include <string>

#include "../src/compiler/parser.cpp"

#include "../src/compiler/compile.h"
#include "../src/compiler/compile.cpp"
#include "../src/compiler/optimizer.cpp"

#include "../src/interop/interop.h"
#include "../src/interop/interop.cpp"

#include "../src/preprocessor/macros.h"
#include "../src/preprocessor/macros.cpp"

#include "../src/preprocessor/preprocess.h"
#include "../src/preprocessor/preprocess.cpp"
#include "../src/preprocessor/aliases.cpp"

#include "../src/runtime/stl.cpp"




using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define DECOMP_PATH "../../Tests/DecompiledScisl/"

namespace Scisl
{
	TEST_CLASS(SCRIPTING)
	{
	public:
		#define SCRIPTING_PATH "../../Tests/scripting/"

		TEST_METHOD(UsingBreak)
		{
			scisl::removeAllVars();
			int ipt = 5;
			scisl::defineMacro("input", std::to_string(ipt));
			scisl::Program prog = scisl::compile(SCRIPTING_PATH "BasicBreaks.scisl");

			
			int b1 = prog.run();
			Assert::IsTrue(prog.broke);

			int b2 = prog.run();
			Assert::IsTrue(prog.broke);

			int b3 = prog.run();
			Assert::IsTrue(prog.broke);

			int ret = prog.run();
			Assert::IsFalse(prog.broke);

			Assert::AreEqual(ipt, b1);
			Assert::AreEqual(ipt + 25 + 32 + 500, b2);
			Assert::AreEqual(ipt, b3);
			Assert::AreEqual(ipt * 10, ret);
		}

		TEST_METHOD(MacroBranching)
		{
			scisl::removeAllVars();
			std::string id = "2";
			scisl::defineMacro("id", id);
			scisl::Program prog = scisl::compile(SCRIPTING_PATH "MacroBranching.scisl");

			int r = prog.run();
			

			Assert::AreEqual(25, r);
		}	
	};

	TEST_CLASS(FUNCTION)
	{
	public:
		#define FUNCTION_PATH "../../Tests/function/"
		TEST_METHOD(ParseStr)
		{
			scisl::removeAllVars();
			std::string line = "apple,orange,c++,scisl,1337,Flood";
			scisl::registerVar("line", &line);

			std::string first = "";
			std::string second = "";
			std::string third = "";
			std::string fourth = "";
			std::string fifth = "";
			std::string last = "";
			scisl::registerVar("first", &first);
			scisl::registerVar("second", &second);
			scisl::registerVar("third", &third);
			scisl::registerVar("fourth", &fourth);
			scisl::registerVar("fifth", &fifth);
			scisl::registerVar("last", &last);

			scisl::Program prog = scisl::compile(FUNCTION_PATH "ParseStr.scisl");

			int o = prog.run();

			Assert::AreEqual(0, o);
			Assert::AreEqual(std::string("apple"), first);
			Assert::AreEqual(std::string("orange"), second);
			Assert::AreEqual(std::string("c++"), third);
			Assert::AreEqual(std::string("scisl"), fourth);
			Assert::AreEqual(std::string("1337"), fifth);
			Assert::AreEqual(std::string("Flood"), last);

			line = "1,2,3,4,5,6";
			o = prog.run();

			Assert::AreEqual(0, o);
			Assert::AreEqual(std::string("1"), first);
			Assert::AreEqual(std::string("2"), second);
			Assert::AreEqual(std::string("3"), third);
			Assert::AreEqual(std::string("4"), fourth);
			Assert::AreEqual(std::string("5"), fifth);
			Assert::AreEqual(std::string("6"), last);

			
		}

		TEST_METHOD(ParseStrFuncs)
		{
			scisl::removeAllVars();
			std::string line = "apple,orange,c++,scisl,1337,Flood";
			scisl::registerVar("line", &line);

			std::string first = "";
			std::string second = "";
			std::string third = "";
			std::string fourth = "";
			std::string fifth = "";
			std::string last = "";
			scisl::registerVar("first", &first);
			scisl::registerVar("second", &second);
			scisl::registerVar("third", &third);
			scisl::registerVar("fourth", &fourth);
			scisl::registerVar("fifth", &fifth);
			scisl::registerVar("last", &last);

			scisl::Program prog = scisl::compile(FUNCTION_PATH "ParseStrFuncs.scisl");

			int o = prog.run();

			Assert::AreEqual(0, o);
			Assert::AreEqual(std::string("apple"), first);
			Assert::AreEqual(std::string("orange"), second);
			Assert::AreEqual(std::string("c++"), third);
			Assert::AreEqual(std::string("scisl"), fourth);
			Assert::AreEqual(std::string("1337"), fifth);
			Assert::AreEqual(std::string("Flood"), last);

			line = "1,2,3,4,5,6";
			o = prog.run();

			Assert::AreEqual(0, o);
			Assert::AreEqual(std::string("1"), first);
			Assert::AreEqual(std::string("2"), second);
			Assert::AreEqual(std::string("3"), third);
			Assert::AreEqual(std::string("4"), fourth);
			Assert::AreEqual(std::string("5"), fifth);
			Assert::AreEqual(std::string("6"), last);

			
		}

		TEST_METHOD(ReverseString)
		{
			scisl::removeAllVars();
			std::string line = "pineapple";
			scisl::registerVar("ipt", &line);

			scisl::Program prog = scisl::compile(FUNCTION_PATH "ReverseString.scisl");

			

			int o = prog.run();

			Assert::AreEqual(0, o);
			Assert::AreEqual(std::string("elppaenip"), line);

			line = "12345";
			o = prog.run();

			Assert::AreEqual(0, o);
			Assert::AreEqual(std::string("54321"), line);

			
		}
	};

	TEST_CLASS(MATH)
	{
	public:
		#define MATH_PATH "../../Tests/math/"
		TEST_METHOD(Math)
		{
			scisl::removeAllVars();
			int first = 10, second = 10;
			scisl::registerVar("first", &first);
			scisl::defineMacro("second", std::to_string(second));

			int a = 0, s = 0, m = 0, d = 0;
			scisl::registerVar("a", &a);
			scisl::registerVar("s", &s);
			scisl::registerVar("m", &m);
			scisl::registerVar("d", &d);

			scisl::Program prog = scisl::compile(MATH_PATH "Math.scisl");

			
			prog.run();
			

			Assert::AreEqual(a, first + second);
			Assert::AreEqual(s, first - second);
			Assert::AreEqual(m, first * second);
			Assert::AreEqual(d, first / second);
		}

		TEST_METHOD(Factorial12)
		{
			scisl::removeAllVars();
			int opt = 0;
			scisl::registerVar("opt", &opt);

			scisl::Program prog = scisl::compile(MATH_PATH "Factorial12.scisl");

			

			prog.run();
			

			Assert::AreEqual(479001600, opt);
		}

		TEST_METHOD(StringAdd)
		{
			scisl::removeAllVars();
			std::string name = "Bob";
			scisl::defineMacro("name", '"' + name + '"');
			scisl::Program prog = scisl::compile(MATH_PATH "StrAdd.scisl");

			

			int opt = prog.run();
			

			int expected = 0;
			std::string tmp = "hello " + name + '.';
			for (char c : tmp)
			{
				expected += c;
			}

			Assert::AreEqual(expected, opt);
		}
	};

	TEST_CLASS(ERRORS)
	{
	public:
		#define ERRORS_PATH "../../Tests/errors/"
		TEST_METHOD(FuncBeforeDef)
		{
			scisl::removeAllVars();
			
			scisl::Program prog = scisl::compile(ERRORS_PATH "funcBeforeDef.scisl");

			
			
		}

		TEST_METHOD(JumpOutBlock)
		{
			scisl::removeAllVars();

			scisl::Program prog = scisl::compile(ERRORS_PATH "jumpOutBlock.scisl");

			
			
		}

		TEST_METHOD(MalformedDef)
		{
			scisl::removeAllVars();

			scisl::Program prog = scisl::compile(ERRORS_PATH "malformedDef.scisl");

			
			
		}

		TEST_METHOD(UndefinedFunc)
		{
			scisl::removeAllVars();

			scisl::Program prog = scisl::compile(ERRORS_PATH "undefinedFunc.scisl");

			
			
		}

		TEST_METHOD(UndefinedLabel)
		{
			scisl::removeAllVars();

			scisl::Program prog = scisl::compile(ERRORS_PATH "undefinedLabel.scisl");

			
			
		}

		TEST_METHOD(UndefinedVar)
		{
			scisl::removeAllVars();

			scisl::Program prog = scisl::compile(ERRORS_PATH "undefinedVar.scisl");

			
			
		}
	};
}