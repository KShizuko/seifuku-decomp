#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgTeamAward.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgTeamAwardProbe : CMsgTeamAward
{
    typedef CMsgTeamAward::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgTeamAwardProbe, m_pInfo); }
};

static_assert(sizeof(CMsgTeamAward) == 0x408, "");
static_assert(sizeof(MsgTeamAwardProbe::Info) == 0x10, "");
static_assert(offsetof(MsgTeamAwardProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgTeamAwardProbe::Info, dwTeamData) == 0x4, "");
static_assert(sizeof(MsgTeamAwardProbe::Info::dwTeamData) == 12, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTeamAward::*)(char*, DWORD)>(&CMsgTeamAward::Create)), BOOL (CMsgTeamAward::*)(char*, DWORD)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTeamAward::*)(DWORD, DWORD, DWORD)>(&CMsgTeamAward::Create)), BOOL (CMsgTeamAward::*)(DWORD, DWORD, DWORD)>::value, "");

TEST_CLASS(ClientMsgTeamAwardHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgTeamAwardProbe::InfoPtr() == 0x404);
    }
};
