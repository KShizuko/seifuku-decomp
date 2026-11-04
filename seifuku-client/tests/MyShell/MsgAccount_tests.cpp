#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgAccount.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgAccountProbe : CMsgAccount
{
    typedef CMsgAccount::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgAccountProbe, m_pInfo); }
};

static_assert(sizeof(CMsgAccount) == 0x408, "");
static_assert(sizeof(MsgAccountProbe::Info) == 0x34, "");
static_assert(offsetof(MsgAccountProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgAccountProbe::Info, szAccount) == 0x4, "");
static_assert(offsetof(MsgAccountProbe::Info, szPassword) == 0x14, "");
static_assert(offsetof(MsgAccountProbe::Info, szServer) == 0x24, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgAccount::*)(const char*, const char*, const char*)>(&CMsgAccount::Create)), BOOL (CMsgAccount::*)(const char*, const char*, const char*)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgAccount::*)(char*, DWORD)>(&CMsgAccount::Create)), BOOL (CMsgAccount::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgAccountHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgAccountProbe::InfoPtr() == 0x404);
    }
};
