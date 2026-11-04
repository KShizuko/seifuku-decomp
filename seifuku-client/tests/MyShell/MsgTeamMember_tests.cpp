#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgTeamMember.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgTeamMemberProbe : CMsgTeamMember
{
    typedef CMsgTeamMember::MSG_Info Info;
    typedef CMsgTeamMember::MsgTeamMemberInfo Member;
    static size_t InfoPtr() { return offsetof(MsgTeamMemberProbe, m_pInfo); }
};

static_assert(sizeof(CMsgTeamMember) == 0x408, "");
static_assert(sizeof(MsgTeamMemberProbe::Member) == 0x1C, "");
static_assert(offsetof(MsgTeamMemberProbe::Member, szName) == 0x0, "");
static_assert(offsetof(MsgTeamMemberProbe::Member, id) == 0x10, "");
static_assert(offsetof(MsgTeamMemberProbe::Member, dwFace) == 0x14, "");
static_assert(offsetof(MsgTeamMemberProbe::Member, usMaxHp) == 0x18, "");
static_assert(offsetof(MsgTeamMemberProbe::Member, usHp) == 0x1A, "");
static_assert(sizeof(MsgTeamMemberProbe::Info) == 0x94, "");
static_assert(offsetof(MsgTeamMemberProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgTeamMemberProbe::Info, ucAction) == 0x4, "");
static_assert(offsetof(MsgTeamMemberProbe::Info, ucAmount) == 0x5, "");
static_assert(offsetof(MsgTeamMemberProbe::Info, setMember) == 0x8, "");
static_assert(_MAX_TEAMMEMBER == 5, "");
static_assert(_MSG_TEAMMEMBER_ADDMEMBER == 0 && _MSG_TEAMMEMBER_DROP_ADDMEMBER == 1, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTeamMember::*)(char*, DWORD)>(&CMsgTeamMember::Create)), BOOL (CMsgTeamMember::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgTeamMemberHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgTeamMemberProbe::InfoPtr() == 0x404);
    }
};
