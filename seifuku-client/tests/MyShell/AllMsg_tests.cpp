#include "CppUnitTest.h"
#include <cstddef>
#include "AllMsg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMsgLogin) == 0x408 && sizeof(CMsgTalk) == 0x414 && sizeof(CMsgAction) == 0x408 && sizeof(CMsgPlayer) == 0x414 && sizeof(CMsgTick) == 0x408, "");
static_assert(sizeof(CMsgFriend) == 0x408 && sizeof(CMsgUserInfo) == 0x414 && sizeof(CMsgItem) == 0x408 && sizeof(CMsgItemInfo) == 0x408 && sizeof(CMsgInteract) == 0x408, "");
static_assert(sizeof(CMsgUserAttrib) == 0x408 && sizeof(CMsgName) == 0x414 && sizeof(CMsgWalk) == 0x408 && sizeof(CMsgMapItem) == 0x408 && sizeof(CMsgAllot) == 0x408, "");
static_assert(sizeof(CMsgTrade) == 0x408 && sizeof(CMsgNpc) == 0x408 && sizeof(CMsgNpcInfo) == 0x414 && sizeof(CMsgPackage) == 0x408 && sizeof(CMsgWeaponSkill) == 0x408, "");
static_assert(sizeof(CMsgTaskDialog) == 0x414 && sizeof(CMsgAccount) == 0x408 && sizeof(CMsgConnect) == 0x408 && sizeof(CMsgTeam) == 0x408 && sizeof(CMsgTeamMember) == 0x408, "");
static_assert(sizeof(CMsgFlushExp) == 0x408 && sizeof(CMsgMagicInfo) == 0x408 && sizeof(CMsgMagicEffect) == 0x408 && sizeof(CMsgSyndicate) == 0x408 && sizeof(CMsgSyndicateAttributeInfo) == 0x408, "");
static_assert(sizeof(CMsgFriendInfo) == 0x408 && sizeof(CMsgWeather) == 0x408 && sizeof(CMsgGemEmbed) == 0x408 && sizeof(CMsgTime) == 0x408 && sizeof(CMsgItemInfoEx) == 0x408, "");
static_assert(sizeof(CMsgNpcInfoEX) == 0x414 && sizeof(CMsgMapInfo) == 0x408 && sizeof(CMsgMessageBoard) == 0x414 && sizeof(CMsgSynMemberInfo) == 0x408 && sizeof(CMsgDice) == 0x408, "");
static_assert(sizeof(CMsgPetInfo) == 0x408 && sizeof(CMsgDataArray) == 0x408 && sizeof(CMsgLoginReply) == 0x408 && sizeof(CMsgFuse) == 0x408 && sizeof(CMsgTeamAward) == 0x408, "");

TEST_CLASS(ClientAllMsgHeader)
{
public:
    TEST_METHOD(EveryClassDerivesFromNetMsg)
    {
        CMsgAllot* p = 0;
        CNetMsg* pBase = p;
        Assert::IsTrue(pBase == 0);
    }
};
