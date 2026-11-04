#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgFriendInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgFriendInfoProbe : CMsgFriendInfo
{
    typedef CMsgFriendInfo::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgFriendInfoProbe, m_pInfo); }
};

static_assert(sizeof(CMsgFriendInfo) == 0x408, "");
static_assert(sizeof(MsgFriendInfoProbe::Info) == 0x28, "");
static_assert(offsetof(MsgFriendInfoProbe::Info, idFriend) == 0x4, "");
static_assert(offsetof(MsgFriendInfoProbe::Info, dwLook) == 0x8, "");
static_assert(offsetof(MsgFriendInfoProbe::Info, ucLevel) == 0xC, "");
static_assert(offsetof(MsgFriendInfoProbe::Info, ucProfession) == 0xD, "");
static_assert(offsetof(MsgFriendInfoProbe::Info, sPk) == 0xE, "");
static_assert(offsetof(MsgFriendInfoProbe::Info, dwSynID_Rank) == 0x10, "");
static_assert(offsetof(MsgFriendInfoProbe::Info, szMate) == 0x14, "");
static_assert(offsetof(MsgFriendInfoProbe::Info, ucAction) == 0x24, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgFriendInfo::*)(char*, DWORD)>(&CMsgFriendInfo::Create)), BOOL (CMsgFriendInfo::*)(char*, DWORD)>::value, "");
static_assert(_FRIEND_INFO == 0 && _ENEMY_INFO == 1, "");

TEST_CLASS(ClientMsgFriendInfoHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgFriendInfoProbe::InfoPtr() == 0x404);
    }
};
