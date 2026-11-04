#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgLogin.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgLoginProbe : CMsgLogin
{
    typedef CMsgLogin::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgLoginProbe, m_pInfo); }
};

static_assert(sizeof(CMsgLogin) == 0x408, "");
static_assert(sizeof(MsgLoginProbe::Info) == 0x28, "");
static_assert(offsetof(MsgLoginProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgLoginProbe::Info, dwVersion) == 0x4 && offsetof(MsgLoginProbe::Info, szAccount) == 0x8 && offsetof(MsgLoginProbe::Info, szPassword) == 0x18, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgLogin::*)(char*, DWORD)>(&CMsgLogin::Create)), BOOL (CMsgLogin::*)(char*, DWORD)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgLogin::*)(DWORD, const char*, const char*)>(&CMsgLogin::Create)), BOOL (CMsgLogin::*)(DWORD, const char*, const char*)>::value, "");

TEST_CLASS(ClientMsgLoginHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgLoginProbe::InfoPtr() == 0x404);
    }
};
