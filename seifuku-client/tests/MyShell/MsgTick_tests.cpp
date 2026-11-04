#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgTick.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgTickProbe : CMsgTick
{
    typedef CMsgTick::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgTickProbe, m_pInfo); }
};

static_assert(sizeof(CMsgTick) == 0x408, "");
static_assert(sizeof(MsgTickProbe::Info) == 0x20, "");
static_assert(offsetof(MsgTickProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgTickProbe::Info, idPlayer) == 0x4, "");
static_assert(offsetof(MsgTickProbe::Info, dwData) == 0x8, "");
static_assert(offsetof(MsgTickProbe::Info, dwRandom) == 0xC && sizeof(MsgTickProbe::Info::dwRandom) == 16, "");
static_assert(offsetof(MsgTickProbe::Info, dwChkData) == 0x1C, "");
static_assert(std::is_same<decltype(&CMsgTick::Create), BOOL (CMsgTick::*)(char*, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMsgTick::Send), void (CMsgTick::*)(void)>::value, "");

TEST_CLASS(ClientMsgTickHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgTickProbe::InfoPtr() == 0x404);
    }
};
