#include "CppUnitTest.h"
#include <windows.h>
#include "../../src/MsgServer/MapGroupKernel/DeadLoop.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

DEADLOOP_SET		g_setDeadLoop(8);
CMyCriticalSection	CDeadLoop::s_xDump;
CTimeOut			CDeadLoop::s_tDump;
int					CDeadLoop::s_nDumpCount	= 0;
int					CDeadLoop::s_nDumpLeft	= 0;
CMyCriticalSection	CDeadLoop::s_xCtrl;

TEST_CLASS(DeadLoopTests)
{
public:
	TEST_METHOD(GuardIsEightBytes)
	{
		Assert::AreEqual((size_t)8, sizeof(AUTO_POP));
	}

	TEST_METHOD(CheckPushesAndPopsItsText)
	{
		const int	PID = 3;
		Assert::AreEqual(0, DeadLoopSet()[PID].size());
		{
			DEADLOOP_CHECK(PID, "KERNEL_CLIENTMSG: ")
			Assert::AreEqual(1, DeadLoopSet()[PID].size());
		}
		Assert::AreEqual(0, DeadLoopSet()[PID].size());
	}

	TEST_METHOD(NestedChecksUnwindInOrder)
	{
		const int	PID = 4;
		{
			DEADLOOP_CHECK(PID, "outer")
			{
				DEADLOOP_CHECK(PID, "inner")
				Assert::AreEqual(2, DeadLoopSet()[PID].size());
			}
			Assert::AreEqual(1, DeadLoopSet()[PID].size());
			Assert::AreEqual("outer", DeadLoopSet()[PID].pop());
			DeadLoopSet()[PID].push("outer");
		}
		Assert::AreEqual(0, DeadLoopSet()[PID].size());
	}

	TEST_METHOD(ProcessesKeepSeparateStacks)
	{
		{
			DEADLOOP_CHECK(5, "five")
			Assert::AreEqual(1, DeadLoopSet()[5].size());
			Assert::AreEqual(0, DeadLoopSet()[6].size());
		}
		Assert::AreEqual(0, DeadLoopSet()[5].size());
	}
};
