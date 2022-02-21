#include "CppUnitTest.h"

#include "../src/compiler/compiler.h"
#include "../src/interoperability/tables.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define DECOMP_PATH "../../Tests/DecompiledScisl/"

namespace Scisl
{
	TEST_CLASS(SCRIPTING)
	{
	public:
		#define SCRIPTING_PATH "../../Tests/scripting/"
		TEST_METHOD(TwoBalls)
		{
			scisl::removeAllVars();
			struct ball {
				int x;
				int y;
				unsigned int curInstr;
			};

			ball b1 = { 5, -5, 0 };
			ball b2 = { 0, 0, 0 };

			scisl::registerVar("x", b1.x);
			scisl::registerVar("y", b1.y);

			scisl::program* prog = scisl::compile(SCRIPTING_PATH "TwoBalls.scisl");

			if (prog == nullptr) Assert::Fail();

			prog->decompile(DECOMP_PATH "TwoBalls.scisl");

			prog->run();
			b1.curInstr = prog->curInstr;

			scisl::updateVar("x", b2.x);
			scisl::updateVar("y", b2.y);
			prog->curInstr = b2.curInstr;
			prog->run();
			b2.curInstr = prog->curInstr;

			scisl::updateVar("x", b1.x);
			scisl::updateVar("y", b1.y);
			prog->curInstr = b1.curInstr;
			prog->run();
			b1.curInstr = prog->curInstr;

			scisl::updateVar("x", b2.x);
			scisl::updateVar("y", b2.y);
			prog->curInstr = b2.curInstr;
			prog->run();
			b2.curInstr = prog->curInstr;

			scisl::updateVar("x", b1.x);
			scisl::updateVar("y", b1.y);
			prog->curInstr = b1.curInstr;
			prog->run();
			b1.curInstr = prog->curInstr;

			scisl::updateVar("x", b2.x);
			scisl::updateVar("y", b2.y);
			prog->curInstr = b2.curInstr;
			prog->run();
			b2.curInstr = prog->curInstr;

			scisl::updateVar("x", b1.x);
			scisl::updateVar("y", b1.y);
			prog->curInstr = b1.curInstr;
			int b1ret = prog->run();


			scisl::updateVar("x", b2.x);
			scisl::updateVar("y", b2.y);
			prog->curInstr = b2.curInstr;
			int b2ret = prog->run();

			delete prog;

			Assert::AreEqual(-15, b1ret);
			Assert::AreEqual(5, b2ret);
		}

		TEST_METHOD(UsingBreak)
		{
			scisl::removeAllVars();
			int ipt = 5;
			scisl::defineMacro("input", std::to_string(ipt));
			scisl::program* prog = scisl::compile(SCRIPTING_PATH "BasicBreaks.scisl");

			if (prog == nullptr) Assert::Fail();

			prog->decompile(DECOMP_PATH "BasicBreaks.scisl");
			int b1 = prog->run();
			Assert::IsTrue(prog->broke);

			int b2 = prog->run();
			Assert::IsTrue(prog->broke);

			int b3 = prog->run();
			Assert::IsTrue(prog->broke);

			int ret = prog->run();
			Assert::IsFalse(prog->broke);

			delete prog;

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
			scisl::program* prog = scisl::compile(SCRIPTING_PATH "MacroBranching.scisl");

			if (prog == nullptr) Assert::Fail();

			prog->decompile(DECOMP_PATH "MacroBranching.scisl");
			int r = prog->run();
			delete prog;

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
			scisl::registerVar("line", line);

			std::string first = "";
			std::string second = "";
			std::string third = "";
			std::string fourth = "";
			std::string fifth = "";
			std::string last = "";
			scisl::registerVar("first", first);
			scisl::registerVar("second", second);
			scisl::registerVar("third", third);
			scisl::registerVar("fourth", fourth);
			scisl::registerVar("fifth", fifth);
			scisl::registerVar("last", last);

			scisl::program* prog = scisl::compile(FUNCTION_PATH "ParseStr.scisl");

			if (prog == nullptr) Assert::Fail();

			prog->decompile(DECOMP_PATH "ParseStr.scisl");
			int o = prog->run();

			Assert::AreEqual(0, o);
			Assert::AreEqual(std::string("apple"), first);
			Assert::AreEqual(std::string("orange"), second);
			Assert::AreEqual(std::string("c++"), third);
			Assert::AreEqual(std::string("scisl"), fourth);
			Assert::AreEqual(std::string("1337"), fifth);
			Assert::AreEqual(std::string("Flood"), last);

			line = "1,2,3,4,5,6";
			o = prog->run();

			Assert::AreEqual(0, o);
			Assert::AreEqual(std::string("1"), first);
			Assert::AreEqual(std::string("2"), second);
			Assert::AreEqual(std::string("3"), third);
			Assert::AreEqual(std::string("4"), fourth);
			Assert::AreEqual(std::string("5"), fifth);
			Assert::AreEqual(std::string("6"), last);

			delete prog;
		}

		TEST_METHOD(ReverseString)
		{
			scisl::removeAllVars();
			std::string line = "pineapple";
			scisl::registerVar("ipt", line);

			scisl::program* prog = scisl::compile(FUNCTION_PATH "ReverseString.scisl");

			if (prog == nullptr) Assert::Fail();

			prog->decompile(DECOMP_PATH "ReverseString.scisl");
			int o = prog->run();

			Assert::AreEqual(0, o);
			Assert::AreEqual(std::string("elppaenip"), line);

			line = "12345";
			o = prog->run();

			Assert::AreEqual(0, o);
			Assert::AreEqual(std::string("54321"), line);

			delete prog;
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
			scisl::registerVar("first", first);
			scisl::defineMacro("second", std::to_string(second));

			int a = 0, s = 0, m = 0, d = 0;
			scisl::registerVar("a", a);
			scisl::registerVar("s", s);
			scisl::registerVar("m", m);
			scisl::registerVar("d", d);

			scisl::program* prog = scisl::compile(MATH_PATH "Math.scisl");

			if (prog == nullptr) Assert::Fail();

			prog->decompile(DECOMP_PATH "Math.scisl");
			prog->run();
			delete prog;

			Assert::AreEqual(a, first + second);
			Assert::AreEqual(s, first - second);
			Assert::AreEqual(m, first * second);
			Assert::AreEqual(d, first / second);
			
		}

		TEST_METHOD(Factorial12)
		{
			scisl::removeAllVars();
			int opt = 0;
			scisl::registerVar("opt", opt);

			scisl::program* prog = scisl::compile(MATH_PATH "Factorial12.scisl");

			if (prog == nullptr) Assert::Fail();

			prog->run();
			prog->decompile(DECOMP_PATH "Factorial12.scisl");
			delete prog;

			Assert::AreEqual(opt, 479001600);
		}

		TEST_METHOD(StringAdd)
		{
			scisl::removeAllVars();
			std::string name = "Bob";
			scisl::defineMacro("name", '"' + name + '"');
			scisl::program* prog = scisl::compile(MATH_PATH "StrAdd.scisl");

			if (prog == nullptr) Assert::Fail();

			int opt = prog->run();
			prog->decompile(DECOMP_PATH "StrAdd.scisl");
			delete prog;

			int expected = 0;
			std::string tmp = "hello " + name + '.';
			for (char c : tmp)
			{
				expected += c;
			}

			Assert::AreEqual(expected, opt);
		}
	};
}
