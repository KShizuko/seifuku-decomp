#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgFriend.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgFriendProbe : CMsgFriend
{
    typedef CMsgFriend::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgFriendProbe, m_pInfo); }
};

static_assert(sizeof(CMsgFriend) == 0x408, "");
static_assert(sizeof(MsgFriendProbe::Info) == 0x1C, "");
static_assert(offsetof(MsgFriendProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgFriendProbe::Info, idFriend) == 0x4, "");
static_assert(offsetof(MsgFriendProbe::Info, ucAction) == 0x8, "");
static_assert(offsetof(MsgFriendProbe::Info, ucOnlineFlag) == 0x9, "");
static_assert(offsetof(MsgFriendProbe::Info, ucLevel) == 0xA, "");
static_assert(offsetof(MsgFriendProbe::Info, szName) == 0xC && offsetof(MsgFriendProbe::Info, idData) == 0xC, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgFriend::*)(int, OBJID, BOOL, const char*)>(&CMsgFriend::Create)), BOOL (CMsgFriend::*)(int, OBJID, BOOL, const char*)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgFriend::*)(char*, DWORD)>(&CMsgFriend::Create)), BOOL (CMsgFriend::*)(char*, DWORD)>::value, "");
static_assert(_FRIEND_APPLY == 10 && _FRIEND_ONLINE == 12 && _FRIEND_BREAK == 14 && _ENEMY_ADD == 19, "");

TEST_CLASS(ClientMsgFriendHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgFriendProbe::InfoPtr() == 0x404);
    }
};
