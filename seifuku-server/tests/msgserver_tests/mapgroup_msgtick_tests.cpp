#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgTick.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct TickProbe : public CMsgTick
	{
		typedef CMsgTick::MSG_Info	Body;
		static size_t	Wire(size_t nBody)	{ return nBody + 4; }
	};
	typedef TickProbe::Body	Body;
}

namespace mapgroup_msgtick_tests
{
	TEST_CLASS(MapGroupMsgTickTests)
	{
	public:
		TEST_METHOD(TickLayout)
		{
			Assert::AreEqual((size_t)0x00, offsetof(Body, idPlayer));
			Assert::AreEqual((size_t)0x08, TickProbe::Wire(offsetof(Body, dwData)));
			Assert::AreEqual((size_t)0x08, offsetof(Body, dwChkData));
			Assert::AreEqual((size_t)0x0C, offsetof(Body, dwRandom));
			Assert::AreEqual((size_t)0x1C, TickProbe::Wire(offsetof(Body, dwNameChk)));
			Assert::AreEqual((size_t)0x1C, sizeof(Body));
		}
	};
}
