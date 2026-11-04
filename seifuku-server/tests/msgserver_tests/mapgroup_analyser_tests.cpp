#include "CppUnitTest.h"
#include <windows.h>
#include "../../src/MsgServer/MapGroupKernel/ActionAnalyser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace msgserver_tests
{
	TEST_CLASS(MapGroupActionAnalyserTests)
	{
	public:
		TEST_METHOD(RegularActionsFail)
		{
			CActionAnalyser obj(20, 500, 100, 1500);
			DWORD dwTime = 10000;
			for (int i = 0; i < 19; i++, dwTime += 600)
				Assert::IsTrue(obj.AddAction(dwTime));
			Assert::IsFalse(obj.AddAction(dwTime));
		}

		TEST_METHOD(IrregularSlowActionsPass)
		{
			CActionAnalyser obj(20, 500, 100, 1500);
			DWORD dwTime = 10000;
			for (int i = 0; i < 25; i++)
			{
				Assert::IsTrue(obj.AddAction(dwTime));
				dwTime += (i % 2) ? 900 : 400;
			}
		}

		TEST_METHOD(FastActionsFail)
		{
			CActionAnalyser obj(3, 500, 100, 1500);
			Assert::IsTrue(obj.AddAction(1000));
			Assert::IsTrue(obj.AddAction(1100));
			Assert::IsFalse(obj.AddAction(1400));
		}

		TEST_METHOD(PauseResets)
		{
			CActionAnalyser obj(3, 500, 100, 1500);
			Assert::IsTrue(obj.AddAction(1000));
			Assert::IsTrue(obj.AddAction(1100));
			Assert::IsTrue(obj.AddAction(2600));
			Assert::IsTrue(obj.AddAction(2700));
			Assert::IsTrue(obj.AddAction(2800));
			Assert::IsTrue(obj.AddAction(2800));
		}

		TEST_METHOD(AtLeastThree)
		{
			CActionAnalyser obj(1, 500, 100, 1500);
			Assert::IsTrue(obj.AddAction(1000));
			Assert::IsTrue(obj.AddAction(1100));
			Assert::IsFalse(obj.AddAction(1200));
		}
	};
}
