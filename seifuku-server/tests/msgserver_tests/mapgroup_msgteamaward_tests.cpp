#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgTeamAward.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct TeamAwardProbe : public CMsgTeamAward
	{
		typedef CMsgTeamAward::MSG_Info	Body;
		static size_t	Wire(size_t nBody)	{ return nBody + 4; }
	};
	typedef TeamAwardProbe::Body	Body;
}

namespace mapgroup_msgteamaward_tests
{
	TEST_CLASS(MapGroupMsgTeamAwardTests)
	{
	public:
		TEST_METHOD(TeamAwardLayout)
		{
			Assert::AreEqual((size_t)0x00, offsetof(Body, dwAward0));
			Assert::AreEqual((size_t)0x04, offsetof(Body, dwMoney));
			Assert::AreEqual((size_t)0x08, offsetof(Body, dwVirtue));
			Assert::AreEqual((size_t)0x0C, sizeof(Body));
			Assert::AreEqual((size_t)0x04, TeamAwardProbe::Wire(offsetof(Body, dwAward0)));
			Assert::AreEqual((size_t)0x08, TeamAwardProbe::Wire(offsetof(Body, dwMoney)));
			Assert::AreEqual((size_t)0x0C, TeamAwardProbe::Wire(offsetof(Body, dwVirtue)));
			Assert::AreEqual((size_t)0x10, TeamAwardProbe::Wire(sizeof(Body)));
		}
	};
}
