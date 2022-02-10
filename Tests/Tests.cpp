#include "CppUnitTest.h"

#include "../compiler.h"
#include "../tables.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Scisl
{
	TEST_CLASS(MATH)
	{
	public:
		
		TEST_METHOD(Math)
		{
			int first = 10, second = 10;
			scisl::registerVar("first", first);
			scisl::defineMacro("second", std::to_string(second));

			int a = 0, s = 0, m = 0, d = 0;
			scisl::registerVar("a", a);
			scisl::registerVar("s", s);
			scisl::registerVar("m", m);
			scisl::registerVar("d", d);

			scisl::program* prog = scisl::compile("../../Tests/Math.scisl");

			if (prog == nullptr) Assert::Fail();

			prog->decompile("../../Tests/DecompiledScisl/Math.scisl");
			prog->run();
			delete prog;

			Assert::AreEqual(a, first + second);
			Assert::AreEqual(s, first - second);
			Assert::AreEqual(m, first * second);
			Assert::AreEqual(d, first / second);
			
		}

		TEST_METHOD(Factorial12)
		{
			int opt = 0;
			scisl::registerVar("opt", opt);

			scisl::program* prog = scisl::compile("../../Tests/Factorial12.scisl");

			if (prog == nullptr) Assert::Fail();

			prog->run();
			prog->decompile("../../Tests/DecompiledScisl/Factorial12.scisl");
			delete prog;

			Assert::AreEqual(opt, 479001600);
		}

		TEST_METHOD(StringArray)
		{
			std::string name = "Bob";
			scisl::defineMacro("name", '"' + name + '"');
			scisl::program* prog = scisl::compile("../../Tests/StrAdd.scisl");

			if (prog == nullptr) Assert::Fail();

			int opt = prog->run();
			prog->decompile("../../Tests/DecompiledScisl/StrAdd.scisl");
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
