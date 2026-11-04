#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgFlushExp.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgFlushExpProbe : CMsgFlushExp
{
    typedef CMsgFlushExp::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgFlushExpProbe, m_pInfo); }
};

static_assert(sizeof(CMsgFlushExp) == 0x408, "");
static_assert(sizeof(MsgFlushExpProbe::Info) == 0xC, "");
static_assert(offsetof(MsgFlushExpProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgFlushExpProbe::Info, dwExp) == 0x4, "");
static_assert(offsetof(MsgFlushExpProbe::Info, usType) == 0x8, "");
static_assert(offsetof(MsgFlushExpProbe::Info, ucAction) == 0xA, "");
static_assert(MSGFLUSHEXP_WEAPONSKILL == 0 && MSGFLUSHEXP_MAGIC == 1, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgFlushExp::*)(char*, DWORD)>(&CMsgFlushExp::Create)), BOOL (CMsgFlushExp::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgFlushExpHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgFlushExpProbe::InfoPtr() == 0x404);
    }
};
