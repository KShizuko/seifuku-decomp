#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/UserData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_userdata_tests
{
	#define AT(field)	(offsetof(UserInfoStruct, field) + 8)

	TEST_CLASS(MapGroupUserDataTests)
	{
	public:
		TEST_METHOD(RecordSizeAndObject)
		{
			Assert::AreEqual((size_t)0x118, sizeof(UserInfoStruct));
			Assert::AreEqual((size_t)0x128, sizeof(CUserData));
		}
		TEST_METHOD(ColumnsWhereLoadInfoStoresThem)
		{
			Assert::AreEqual((size_t)0x08, AT(id));
			Assert::AreEqual((size_t)0x0C, AT(szName));
			Assert::AreEqual((size_t)0x1C, AT(szMate));
			Assert::AreEqual((size_t)0x2C, AT(dwLookface));
			Assert::AreEqual((size_t)0x30, AT(dwHair));
			Assert::AreEqual((size_t)0x34, AT(dwMoney));
			Assert::AreEqual((size_t)0x38, AT(dwMoney_saved));
			Assert::AreEqual((size_t)0x3C, AT(dwCoin_money));
			Assert::AreEqual((size_t)0x40, AT(ucLevel));
			Assert::AreEqual((size_t)0x48, AT(i64Exp));
			Assert::AreEqual((size_t)0x50, AT(usForce));
			Assert::AreEqual((size_t)0x52, AT(ucOldProf));
			Assert::AreEqual((size_t)0x54, AT(usSpeed));
			Assert::AreEqual((size_t)0x56, AT(usHealth));
			Assert::AreEqual((size_t)0x58, AT(usSoul));
			Assert::AreEqual((size_t)0x5A, AT(usAdditional_point));
			Assert::AreEqual((size_t)0x5C, AT(ucAuto_allot));
			Assert::AreEqual((size_t)0x5E, AT(usLife));
			Assert::AreEqual((size_t)0x60, AT(usMana));
			Assert::AreEqual((size_t)0x62, AT(ucProfession));
			Assert::AreEqual((size_t)0x64, AT(nDeed));
			Assert::AreEqual((size_t)0x68, AT(sPk));
			Assert::AreEqual((size_t)0x6A, AT(ucNobility));
			Assert::AreEqual((size_t)0x6C, AT(dwMedal));
			Assert::AreEqual((size_t)0x70, AT(dwMedal_select));
			Assert::AreEqual((size_t)0x74, AT(ucMetempsychosis));
			Assert::AreEqual((size_t)0x78, AT(idSyndicate));
			Assert::AreEqual((size_t)0x7C, AT(idRecordmap));
			Assert::AreEqual((size_t)0x80, AT(usRecordX));
			Assert::AreEqual((size_t)0x82, AT(usRecordY));
			Assert::AreEqual((size_t)0x84, AT(idAccount));
			Assert::AreEqual((size_t)0x88, AT(dwLast_login));
			Assert::AreEqual((size_t)0x8C, AT(dwTaskMask));
			Assert::AreEqual((size_t)0x90, AT(dwVirtue));
			Assert::AreEqual((size_t)0x94, AT(idHome));
			Assert::AreEqual((size_t)0x98, AT(dwLockKey));
			Assert::AreEqual((size_t)0x9C, AT(dwGodStatus));
			Assert::AreEqual((size_t)0xA0, AT(dwExpBallUsage));
			Assert::AreEqual((size_t)0xB8, AT(dwCurrLockKey));
			Assert::AreEqual((size_t)0xE4, AT(dwEmoney));
			Assert::AreEqual((size_t)0xE8, AT(dwChkSum));
			Assert::AreEqual((size_t)0xEC, AT(dwOnlineTime));
			Assert::AreEqual((size_t)0xF0, AT(dwAutoExercise));
			Assert::AreEqual((size_t)0xF4, AT(dwLastLogout));
			Assert::AreEqual((size_t)0xF8, AT(usBigPrizeFails));
			Assert::AreEqual((size_t)0xFA, AT(usSmallPrizeFails));
			Assert::AreEqual((size_t)0xFC, AT(dwMeteLev));
			Assert::AreEqual((size_t)0x100, AT(dwCurrentLayoutType));
			Assert::AreEqual((size_t)0x104, AT(dwOnlineTime2));
			Assert::AreEqual((size_t)0x108, AT(dwOfflineTime));
			Assert::AreEqual((size_t)0x10C, AT(dwLastLogout2));
			Assert::AreEqual((size_t)0x110, AT(i64Donation));
			Assert::AreEqual((size_t)0x118, AT(dwBusiness));
		}
		TEST_METHOD(FieldsCUserReadsFromTheRecord)
		{
			Assert::AreEqual((size_t)0x7B4, 0x718 + AT(dwGodStatus));
			Assert::AreEqual((size_t)0x780, 0x718 + AT(sPk));
			Assert::AreEqual((size_t)0x7BC, 0x718 + AT(dwEffect));
			Assert::AreEqual((size_t)0x7C4, 0x718 + AT(dwStatus));
			Assert::AreEqual((size_t)0x7C8, 0x718 + AT(ucPkMode));
			Assert::AreEqual((size_t)0x7CA, 0x718 + AT(usEnergy));
			Assert::AreEqual((size_t)0x7CC, 0x718 + AT(usMaxEnergy));
			Assert::AreEqual((size_t)0x7D4, 0x718 + AT(dwMonsterType));
			Assert::AreEqual((size_t)0x7EC, 0x718 + AT(infoWanted.nBonuty));
			Assert::AreEqual((size_t)0x7F0, 0x718 + AT(infoWanted.idModifyWanted));
			Assert::AreEqual((size_t)0x7F4, 0x718 + AT(infoWanted.idWanted));
			Assert::AreEqual((size_t)0x7F8, 0x718 + AT(idPoliceWanted));
			Assert::AreEqual((size_t)0x788, 0x718 + AT(dwMedal_select));
		}
	};
}
