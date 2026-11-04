#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#pragma include_alias("AiNpc.h", "../../src/MsgServer/MapGroupKernel/AINpc.h")
#define AFX_AINPC_H__52682938_5D82_4101_B888_6A4A3F30EB51__INCLUDED_
class IRole;
#include "../../src/MsgServer/MapGroupKernel/Network/MsgPlayer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct PlayerProbe : public CMsgPlayer
	{
		typedef CMsgPlayer::MSG_Info	Body;
		static size_t	Wire(size_t nBody)	{ return nBody + 4; }
	};
	typedef PlayerProbe::Body	Body;
}

namespace mapgroup_player_tests
{
	TEST_CLASS(MapGroupPlayerTests)
	{
	public:
		TEST_METHOD(PlayerWireLayout)
		{
			Assert::AreEqual((size_t)0x04, PlayerProbe::Wire(offsetof(Body, id)));
			Assert::AreEqual((size_t)0x08, PlayerProbe::Wire(offsetof(Body, dwLookFace)));
			Assert::AreEqual((size_t)0x0C, PlayerProbe::Wire(offsetof(Body, dwStatus)));
			Assert::AreEqual((size_t)0x14, PlayerProbe::Wire(offsetof(Body, dwSynID_Rank)));
			Assert::AreEqual((size_t)0x18, PlayerProbe::Wire(offsetof(Body, dwMountType)));
			Assert::AreEqual((size_t)0x1C, PlayerProbe::Wire(offsetof(Body, dwHelmetType)));
			Assert::AreEqual((size_t)0x20, PlayerProbe::Wire(offsetof(Body, dwArmorType)));
			Assert::AreEqual((size_t)0x24, PlayerProbe::Wire(offsetof(Body, dwWeaponRType)));
			Assert::AreEqual((size_t)0x28, PlayerProbe::Wire(offsetof(Body, dwWeaponLType)));
			Assert::AreEqual((size_t)0x2C, PlayerProbe::Wire(offsetof(Body, usWeaponRAntiMonster)));
			Assert::AreEqual((size_t)0x2E, PlayerProbe::Wire(offsetof(Body, usWeaponLAntiMonster)));
			Assert::AreEqual((size_t)0x30, PlayerProbe::Wire(offsetof(Body, dwMantleType)));
			Assert::AreEqual((size_t)0x30, PlayerProbe::Wire(offsetof(Body, usLife)));
			Assert::AreEqual((size_t)0x32, PlayerProbe::Wire(offsetof(Body, usLevel)));
			Assert::AreEqual((size_t)0x34, PlayerProbe::Wire(offsetof(Body, usPosX)));
			Assert::AreEqual((size_t)0x36, PlayerProbe::Wire(offsetof(Body, usPosY)));
			Assert::AreEqual((size_t)0x38, PlayerProbe::Wire(offsetof(Body, usHair)));
			Assert::AreEqual((size_t)0x3A, PlayerProbe::Wire(offsetof(Body, ucDir)));
			Assert::AreEqual((size_t)0x3B, PlayerProbe::Wire(offsetof(Body, ucPose)));
			Assert::AreEqual((size_t)0x3C, PlayerProbe::Wire(offsetof(Body, ucMetempsychosis)));
			Assert::AreEqual((size_t)0x3E, PlayerProbe::Wire(offsetof(Body, szBuf)));
		}
		TEST_METHOD(PlayerFieldSizes)
		{
			Assert::AreEqual((size_t)8, sizeof(((Body*)0)->dwStatus));
			Assert::AreEqual((size_t)2, sizeof(((Body*)0)->usWeaponRAntiMonster));
			Assert::AreEqual((size_t)4, sizeof(((Body*)0)->dwMantleType));
			Assert::AreEqual((size_t)1, sizeof(((Body*)0)->ucMetempsychosis));
		}
	};
}
