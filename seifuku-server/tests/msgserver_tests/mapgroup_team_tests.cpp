#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Team.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_team_tests
{
	TEST_CLASS(MapGroupTeamTests)
	{
	public:
		TEST_METHOD(TeamInfoLayout)
		{
			Assert::AreEqual((size_t)0x28, sizeof(TeamInfoStruct));
			Assert::AreEqual((size_t)0x00, offsetof(TeamInfoStruct, id));
			Assert::AreEqual((size_t)0x04, offsetof(TeamInfoStruct, idLeader));
			Assert::AreEqual((size_t)0x08, offsetof(TeamInfoStruct, setMember));
			Assert::AreEqual((size_t)0x1C, offsetof(TeamInfoStruct, bCloseMoney));
			Assert::AreEqual((size_t)0x20, offsetof(TeamInfoStruct, bCloseItem));
			Assert::AreEqual((size_t)0x24, offsetof(TeamInfoStruct, bUnk24));
		}
		TEST_METHOD(TeamConstants)
		{
			Assert::AreEqual(5, _MAX_TEAMAMOUNT);
			Assert::AreEqual(32, _RANGE_EXPSHARE);
			Assert::AreEqual(1, (int)CHANGETEAM_ADD);
			Assert::AreEqual(2, (int)CHANGETEAM_DEL);
			Assert::AreEqual(3, (int)CHANGETEAM_DISMISS);
		}
	};
}
