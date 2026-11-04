#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgAllot.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgAllotProbe : CMsgAllot
{
    typedef CMsgAllot::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgAllotProbe, m_pInfo); }
};

static_assert(sizeof(CMsgAllot) == 0x408, "");
static_assert(sizeof(MsgAllotProbe::Info) == 0x8, "");
static_assert(offsetof(MsgAllotProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgAllotProbe::Info, ucForce) == 0x4, "");
static_assert(offsetof(MsgAllotProbe::Info, ucSpeed) == 0x5, "");
static_assert(offsetof(MsgAllotProbe::Info, ucHealth) == 0x6, "");
static_assert(offsetof(MsgAllotProbe::Info, ucSoul) == 0x7, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgAllot::*)(unsigned char, unsigned char, unsigned char, unsigned char)>(&CMsgAllot::Create)), BOOL (CMsgAllot::*)(unsigned char, unsigned char, unsigned char, unsigned char)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgAllot::*)(char*, DWORD)>(&CMsgAllot::Create)), BOOL (CMsgAllot::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgAllotHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgAllotProbe::InfoPtr() == 0x404);
    }
};
