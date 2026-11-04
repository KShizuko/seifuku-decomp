#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include <string.h>
#include <new>
#include "../../src/MsgServer/MapGroupKernel/UserWeaponSkill.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_weaponskill_tests
{
	TEST_CLASS(MapGroupWeaponSkillTests)
	{
	public:
		TEST_METHOD(WeaponSkillRecordLayout)
		{
			Assert::AreEqual((size_t)0x18, sizeof(UserWeaponSkillInfoStruct));
			Assert::AreEqual((size_t)0x00, offsetof(UserWeaponSkillInfoStruct, id));
			Assert::AreEqual((size_t)0x04, offsetof(UserWeaponSkillInfoStruct, idowner));
			Assert::AreEqual((size_t)0x08, offsetof(UserWeaponSkillInfoStruct, nType));
			Assert::AreEqual((size_t)0x0C, offsetof(UserWeaponSkillInfoStruct, ucLevel));
			Assert::AreEqual((size_t)0x10, offsetof(UserWeaponSkillInfoStruct, nExp));
			Assert::AreEqual((size_t)0x14, offsetof(UserWeaponSkillInfoStruct, ucOldLev));
			Assert::AreEqual((size_t)0x15, offsetof(UserWeaponSkillInfoStruct, ucUnlearn));
		}

		TEST_METHOD(WeaponSkillRecordConstructorZeroes)
		{
			unsigned char buf[sizeof(UserWeaponSkillInfoStruct)];
			memset(buf, 0xCD, sizeof(buf));
#pragma push_macro("new")
#undef new
			UserWeaponSkillInfoStruct* pInfo = new(buf) UserWeaponSkillInfoStruct;
#pragma pop_macro("new")
			Assert::AreEqual((OBJID)0, pInfo->id);
			Assert::AreEqual((OBJID)0, pInfo->idowner);
			Assert::AreEqual(0, pInfo->nType);
			Assert::AreEqual((int)0, (int)pInfo->ucLevel);
			Assert::AreEqual(0, pInfo->nExp);
			Assert::AreEqual((int)0, (int)pInfo->ucOldLev);
			Assert::AreEqual((int)0, (int)pInfo->ucUnlearn);
		}
	};
}
