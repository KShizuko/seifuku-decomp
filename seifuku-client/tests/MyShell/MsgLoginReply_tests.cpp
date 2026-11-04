#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgLoginReply.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgLoginReplyProbe : CMsgLoginReply
{
    typedef CMsgLoginReply::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgLoginReplyProbe, m_pInfo); }
};

static_assert(sizeof(CMsgLoginReply) == 0x408, "");
static_assert(sizeof(MsgLoginReplyProbe::Info) == 0x20, "");
static_assert(offsetof(MsgLoginReplyProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgLoginReplyProbe::Info, idAccount) == 0x4, "");
static_assert(offsetof(MsgLoginReplyProbe::Info, dwData) == 0x8, "");
static_assert(offsetof(MsgLoginReplyProbe::Info, szInfo) == 0xC, "");
static_assert(sizeof(MsgLoginReplyProbe::Info::szInfo) == 16, "");
static_assert(offsetof(MsgLoginReplyProbe::Info, nServePort) == 0x1C, "");
static_assert(std::is_same<decltype(&CMsgLoginReply::Create), BOOL (CMsgLoginReply::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgLoginReplyHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgLoginReplyProbe::InfoPtr() == 0x404);
    }
};
