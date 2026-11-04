#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgUserInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct UserInfoProbe : public CMsgUserInfo
	{
		typedef CMsgUserInfo::MSG_Info	Body;
		static size_t	Wire(size_t nBody)	{ return nBody + 4; }
	};
	typedef UserInfoProbe::Body	Body;
}

namespace mapgroup_userinfo_tests
{
	TEST_CLASS(MapGroupUserInfoTests)
	{
	public:
		TEST_METHOD(UserInfoWireLayout)
		{
			Assert::AreEqual((size_t)0x04, UserInfoProbe::Wire(offsetof(Body, id)));
			Assert::AreEqual((size_t)0x08, UserInfoProbe::Wire(offsetof(Body, dwLookFace)));
			Assert::AreEqual((size_t)0x0C, UserInfoProbe::Wire(offsetof(Body, usHair)));
			Assert::AreEqual((size_t)0x10, UserInfoProbe::Wire(offsetof(Body, dwMoney)));
			Assert::AreEqual((size_t)0x14, UserInfoProbe::Wire(offsetof(Body, dwEmoney)));
			Assert::AreEqual((size_t)0x18, UserInfoProbe::Wire(offsetof(Body, nExp)));
			Assert::AreEqual((size_t)0x1C, UserInfoProbe::Wire(offsetof(Body, nDeed)));
			Assert::AreEqual((size_t)0x20, UserInfoProbe::Wire(offsetof(Body, dwMedal)));
			Assert::AreEqual((size_t)0x24, UserInfoProbe::Wire(offsetof(Body, dwMedal_select)));
			Assert::AreEqual((size_t)0x28, UserInfoProbe::Wire(offsetof(Body, dwVirtue)));
			Assert::AreEqual((size_t)0x2C, UserInfoProbe::Wire(offsetof(Body, usForce)));
			Assert::AreEqual((size_t)0x2E, UserInfoProbe::Wire(offsetof(Body, usDexterity)));
			Assert::AreEqual((size_t)0x30, UserInfoProbe::Wire(offsetof(Body, usHealth)));
			Assert::AreEqual((size_t)0x32, UserInfoProbe::Wire(offsetof(Body, usSoul)));
			Assert::AreEqual((size_t)0x34, UserInfoProbe::Wire(offsetof(Body, usAdditionalPoint)));
			Assert::AreEqual((size_t)0x36, UserInfoProbe::Wire(offsetof(Body, usLife)));
			Assert::AreEqual((size_t)0x38, UserInfoProbe::Wire(offsetof(Body, usMana)));
			Assert::AreEqual((size_t)0x3A, UserInfoProbe::Wire(offsetof(Body, sPk)));
			Assert::AreEqual((size_t)0x3C, UserInfoProbe::Wire(offsetof(Body, ucLevel)));
			Assert::AreEqual((size_t)0x3D, UserInfoProbe::Wire(offsetof(Body, ucProfession)));
			Assert::AreEqual((size_t)0x3E, UserInfoProbe::Wire(offsetof(Body, ucNobility)));
			Assert::AreEqual((size_t)0x3F, UserInfoProbe::Wire(offsetof(Body, ucMetempsychosis)));
			Assert::AreEqual((size_t)0x40, UserInfoProbe::Wire(offsetof(Body, ucAutoAllot)));
			Assert::AreEqual((size_t)0x41, UserInfoProbe::Wire(offsetof(Body, szBuf)));
		}
		TEST_METHOD(UserInfoFieldSizes)
		{
			Assert::AreEqual((size_t)2, sizeof(((Body*)0)->usHair));
			Assert::AreEqual((size_t)4, sizeof(((Body*)0)->nExp));
			Assert::AreEqual((size_t)2, sizeof(((Body*)0)->sPk));
			Assert::AreEqual((size_t)1, sizeof(((Body*)0)->ucAutoAllot));
		}
	};
}
