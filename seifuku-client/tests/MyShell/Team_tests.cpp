#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Team.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct TeamProbe : CTeam
{
    static size_t Members() { return offsetof(TeamProbe, m_setMember); }
    static size_t TimeBegin() { return offsetof(TeamProbe, m_dwTimeBegin); }
    static size_t TimeFocusBegin() { return offsetof(TeamProbe, m_dwTimeFocusBegin); }
    static size_t LastApply() { return offsetof(TeamProbe, m_idPlayerLastApply); }
    static size_t LastApplyName() { return offsetof(TeamProbe, m_szNamePlayerLastApply); }
    static size_t Closed() { return offsetof(TeamProbe, m_bClosed); }
    static size_t FocusIndex() { return offsetof(TeamProbe, m_nFocusIndex); }
    static size_t FocusIndexFrame() { return offsetof(TeamProbe, m_nFocusIndexFrame); }
    static size_t Leader() { return offsetof(TeamProbe, m_posLeader); }
};

static_assert(sizeof(TeamMemberInfo) == 0x2C, "");
static_assert(offsetof(TeamMemberInfo, id) == 0x10 && offsetof(TeamMemberInfo, dwFace) == 0x14 && offsetof(TeamMemberInfo, dwMaxHp) == 0x18 && offsetof(TeamMemberInfo, dwHp) == 0x1C && offsetof(TeamMemberInfo, bActive) == 0x20 && offsetof(TeamMemberInfo, m_dwQueryBegin) == 0x24 && offsetof(TeamMemberInfo, bBeginSearch) == 0x28, "");
static_assert(sizeof(CTeam) == 4 + sizeof(DEQUE_TEAMMEMBER) + 0x30, "");
static_assert(std::has_virtual_destructor<CTeam>::value, "");
static_assert(std::is_same<decltype(&CTeam::CreateNewMemberInfo), TeamMemberInfo* (*)(char*, OBJID, DWORD, DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CTeam::Show), void (CTeam::*)(CMyPos)>::value, "");

TEST_CLASS(ClientTeamHeader)
{
public:
    TEST_METHOD(MembersFollowTheDequeInTheCtorOrder)
    {
        const size_t d = sizeof(DEQUE_TEAMMEMBER);
        Assert::IsTrue(TeamProbe::Members() == 0x4);
        Assert::IsTrue(TeamProbe::TimeBegin() == 0x4 + d);
        Assert::IsTrue(TeamProbe::TimeFocusBegin() == 0x4 + d + 0x4);
        Assert::IsTrue(TeamProbe::LastApply() == 0x4 + d + 0x8);
        Assert::IsTrue(TeamProbe::LastApplyName() == 0x4 + d + 0xC);
        Assert::IsTrue(TeamProbe::Closed() == 0x4 + d + 0x1C);
        Assert::IsTrue(TeamProbe::FocusIndex() == 0x4 + d + 0x20);
        Assert::IsTrue(TeamProbe::FocusIndexFrame() == 0x4 + d + 0x24);
        Assert::IsTrue(TeamProbe::Leader() == 0x4 + d + 0x28);
    }
};
