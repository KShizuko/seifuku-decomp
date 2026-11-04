#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgNpc.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgNpcProbe : CMsgNpc
{
    typedef CMsgNpc::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgNpcProbe, m_pInfo); }
};

static_assert(sizeof(CMsgNpc) == 0x408, "");
static_assert(sizeof(MsgNpcProbe::Info) == 0x10, "");
static_assert(offsetof(MsgNpcProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgNpcProbe::Info, id) == 0x4, "");
static_assert(offsetof(MsgNpcProbe::Info, dwData) == 0x8 && offsetof(MsgNpcProbe::Info, usPosX) == 0x8 && offsetof(MsgNpcProbe::Info, usPosY) == 0xA, "");
static_assert(offsetof(MsgNpcProbe::Info, usAction) == 0xC, "");
static_assert(offsetof(MsgNpcProbe::Info, usType) == 0xE, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgNpc::*)(int, OBJID, DWORD, USHORT)>(&CMsgNpc::Create)), BOOL (CMsgNpc::*)(int, OBJID, DWORD, USHORT)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgNpc::*)(char*, DWORD)>(&CMsgNpc::Create)), BOOL (CMsgNpc::*)(char*, DWORD)>::value, "");
static_assert(EVENT_BEACTIVED == 0 && EVENT_LEAVEMAP == 2 && EVENT_DELNPC == 3 && EVENT_LAYNPC == 5, "");

TEST_CLASS(ClientMsgNpcHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgNpcProbe::InfoPtr() == 0x404);
    }
};
