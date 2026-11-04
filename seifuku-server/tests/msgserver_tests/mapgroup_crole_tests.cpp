#include "CppUnitTest.h"
#include <windows.h>
#include "../../src/MsgServer/MapGroupKernel/Role.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(CRole) == 8, "");

TEST_CLASS(MapGroupCRoleTests)
{
public:
	TEST_METHOD(IMapThingFollowsIRole)
	{
		CRole role;
		IRole* pRole = &role;
		IMapThing* pThing = &role;
		Assert::AreEqual((const void*)&role, (const void*)pRole);
		Assert::AreEqual(4, (int)((const char*)pThing - (const char*)&role));
	}

	TEST_METHOD(StatusLists)
	{
		Assert::IsTrue(CRole::IsWellStatus(STATUS_ATTACK));
		Assert::IsTrue(CRole::IsWellStatus(0x00001000));
		Assert::IsTrue(CRole::IsWellStatus(0x00100000));
		Assert::IsFalse(CRole::IsWellStatus(0x00000004));
		Assert::IsTrue(CRole::IsBadlyStatus(0x00000004));
		Assert::IsTrue(CRole::IsBadlyStatus(STATUS_SYNCRIME));
		Assert::IsFalse(CRole::IsBadlyStatus(STATUS_ATTACK));
	}
};
