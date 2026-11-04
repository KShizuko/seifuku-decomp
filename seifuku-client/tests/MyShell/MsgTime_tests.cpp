#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgTime.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgTimeProbe : CMsgTime
{
    typedef CMsgTime::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgTimeProbe, m_pInfo); }
};

static_assert(sizeof(CMsgTime) == 0x408, "");
static_assert(sizeof(MsgTimeProbe::Info) == 0x6, "");
static_assert(offsetof(MsgTimeProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgTimeProbe::Info, ucHour) == 0x4, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTime::*)(char*, DWORD)>(&CMsgTime::Create)), BOOL (CMsgTime::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgTimeHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgTimeProbe::InfoPtr() == 0x404);
    }
};
