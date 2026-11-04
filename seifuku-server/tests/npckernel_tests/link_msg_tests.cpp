#include <cstddef>
#include "CppUnitTest.h"
#include "windows.h"
#include "define.h"
#include "NetMsg.h"
#define private public
#define protected public
#include "NpcWorld.h"
#include "AllMsg.h"
#undef protected
#undef private

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(CMsgLogin::MSG_Info) == 0x20, "");
static_assert(sizeof(CMsgAiNpcInfo::MSG_Info) == 0x20, "");
static_assert(sizeof(CMsgInteract::MSG_Info) == 0x1C, "");
static_assert(sizeof(CMsgPlayer::MSG_Info) == 0x54, "");
static_assert(sizeof(UserAttribStruct) == 8, "");
static_assert(sizeof(EffectRoleStruct) == 8, "");
static_assert(sizeof(CMsgPlayer) == 0x420, "");
static_assert(sizeof(CMsgItemInfo) == 0x414 && sizeof(CMsgLogin) == 0x414 && sizeof(CMsgSyndicate) == 0x414, "");
static_assert(sizeof(CNpcWorld) == 0x34, "");

TEST_CLASS(LinkMsgLayoutTests)
{
public:
	TEST_METHOD(AiNpcInfoAsMsgServerReadsIt)
	{
		Assert::AreEqual((size_t)0x04, offsetof(CMsgAiNpcInfo::MSG_Info, idMap));
		Assert::AreEqual((size_t)0x08, offsetof(CMsgAiNpcInfo::MSG_Info, idGen));
		Assert::AreEqual((size_t)0x0C, offsetof(CMsgAiNpcInfo::MSG_Info, idOwner));
		Assert::AreEqual((size_t)0x10, offsetof(CMsgAiNpcInfo::MSG_Info, nData));
		Assert::AreEqual((size_t)0x14, offsetof(CMsgAiNpcInfo::MSG_Info, usPosX));
		Assert::AreEqual((size_t)0x16, offsetof(CMsgAiNpcInfo::MSG_Info, usPosY));
		Assert::AreEqual((size_t)0x18, offsetof(CMsgAiNpcInfo::MSG_Info, usAction));
		Assert::AreEqual((size_t)0x1A, offsetof(CMsgAiNpcInfo::MSG_Info, usType));
		Assert::AreEqual((size_t)0x1C, offsetof(CMsgAiNpcInfo::MSG_Info, ucOwnerType));
		Assert::AreEqual((size_t)0x10, offsetof(CMsgLogin::MSG_Info, szPassword));
		Assert::AreEqual((size_t)0x08, offsetof(CMsgNpc::MSG_Info, usAction));
	}
	TEST_METHOD(PlayerBodyOf2008)
	{
		Assert::AreEqual((size_t)0x08, offsetof(CMsgPlayer::MSG_Info, dwEffect));
		Assert::AreEqual((size_t)0x10, offsetof(CMsgPlayer::MSG_Info, dwSynID_Rank));
		Assert::AreEqual((size_t)0x18, offsetof(CMsgPlayer::MSG_Info, dwHelmetType));
		Assert::AreEqual((size_t)0x28, offsetof(CMsgPlayer::MSG_Info, dwMountType));
		Assert::AreEqual((size_t)0x2C, offsetof(CMsgPlayer::MSG_Info, dwMedalSelect));
		Assert::AreEqual((size_t)0x30, offsetof(CMsgPlayer::MSG_Info, usPosX));
		Assert::AreEqual((size_t)0x34, offsetof(CMsgPlayer::MSG_Info, usHair));
		Assert::AreEqual((size_t)0x36, offsetof(CMsgPlayer::MSG_Info, ucDir));
		Assert::AreEqual((size_t)0x37, offsetof(CMsgPlayer::MSG_Info, ucPose));
		Assert::AreEqual((size_t)0x50, offsetof(CMsgPlayer::MSG_Info, szBuf));
	}
	TEST_METHOD(EntriesAndInteract)
	{
		Assert::AreEqual((size_t)0x08, offsetof(CMsgUserAttrib::MSG_Info, setUserAttrib));
		Assert::AreEqual((size_t)0x0C, offsetof(CMsgMagicEffect::MSG_Info, ucEffectNum));
		Assert::AreEqual((size_t)0x10, offsetof(CMsgMagicEffect::MSG_Info, setEffectRole));
		Assert::AreEqual((size_t)0x08, offsetof(CMsgMagicEffect::MSG_Info, usType));
		Assert::AreEqual((size_t)0x04, offsetof(CMsgInteract::MSG_Info, idSender));
		Assert::AreEqual((size_t)0x0C, offsetof(CMsgInteract::MSG_Info, unPosX));
		Assert::AreEqual((size_t)0x10, offsetof(CMsgInteract::MSG_Info, unType));
		Assert::AreEqual((size_t)0x14, offsetof(CMsgInteract::MSG_Info, dwData));
	}
	TEST_METHOD(ItemInfoSyndicateTeamMember)
	{
		Assert::AreEqual((size_t)0x0C, offsetof(CMsgItemInfo::MSG_Info, ucAction));
		Assert::AreEqual((size_t)0x0E, offsetof(CMsgItemInfo::MSG_Info, ucPosition));
		Assert::AreEqual((size_t)0x18, offsetof(CMsgItemInfo::MSG_Info, nData));
		Assert::AreEqual((size_t)0x0F, offsetof(CMsgItemInfo::MSG_Info, ucGem1));
		Assert::AreEqual((size_t)0x13, offsetof(CMsgItemInfo::MSG_Info, ucMagic3));
		Assert::AreEqual((size_t)0x1C, sizeof(CMsgItemInfo::MSG_Info));
		Assert::AreEqual((size_t)0x04, offsetof(CMsgSyndicate::MSG_Info, idTarget));
		Assert::AreEqual((size_t)0x01, offsetof(CMsgTeamMember::MSG_Info, ucAmount));
		Assert::AreEqual((size_t)0x04, offsetof(CMsgTeamMember::MSG_Info, setMember));
	}
	TEST_METHOD(NpcWorldInTheReferencesOrder)
	{
		Assert::AreEqual((size_t)0x04, offsetof(CNpcWorld, m_nLoginState));
		Assert::AreEqual((size_t)0x08, offsetof(CNpcWorld, m_timeoutLogin));
		Assert::AreEqual((size_t)0x10, offsetof(CNpcWorld, m_tUpdateStateWin));
		Assert::AreEqual((size_t)0x18, offsetof(CNpcWorld, m_pShell));
		Assert::AreEqual((size_t)0x1C, offsetof(CNpcWorld, m_pMapManager));
		Assert::AreEqual((size_t)0x24, offsetof(CNpcWorld, m_pNpcManager));
		Assert::AreEqual((size_t)0x2C, offsetof(CNpcWorld, m_pItemType));
		Assert::AreEqual((size_t)0x30, offsetof(CNpcWorld, m_pDatabase));
		Assert::IsTrue(IsGmName("[GM]Kate") && IsGmName("x[PM]") && !IsGmName("GM"));
	}
};
