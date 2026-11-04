#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <type_traits>
#include "MsgLoginAccountEx.h"
#include "NetworkDef.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgLoginAccountExProbe : CMsgLoginAccountEx
{
    typedef CMsgLoginAccountEx::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgLoginAccountExProbe, m_pInfo); }
};

static_assert(sizeof(CMsgLoginAccountEx) == 0x408, "");
static_assert(sizeof(MsgLoginAccountExProbe::Info) == 0x94, "");
static_assert(offsetof(MsgLoginAccountExProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgLoginAccountExProbe::Info, szAccount) == 0x4, "");
static_assert(offsetof(MsgLoginAccountExProbe::Info, szPassword) == 0x44, "");
static_assert(offsetof(MsgLoginAccountExProbe::Info, szServer) == 0x84, "");
static_assert(_MSG_LOGINACCOUNTEX == 1060, "");
static_assert(std::is_same<decltype(&CMsgLoginAccountEx::Create), void (CMsgLoginAccountEx::*)(const char*, const char*, const char*)>::value, "");

TEST_CLASS(ClientMsgLoginAccountEx)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgLoginAccountExProbe::InfoPtr() == 0x404);
    }
};
