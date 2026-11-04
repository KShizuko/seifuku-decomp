#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgTeam.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgTeamProbe : CMsgTeam
{
    typedef CMsgTeam::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgTeamProbe, m_pInfo); }
};

static_assert(sizeof(CMsgTeam) == 0x408, "");
static_assert(sizeof(MsgTeamProbe::Info) == 0xC, "");
static_assert(offsetof(MsgTeamProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgTeamProbe::Info, unAction) == 0x4, "");
static_assert(offsetof(MsgTeamProbe::Info, idPlayer) == 0x8, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTeam::*)(OBJID, int)>(&CMsgTeam::Create)), BOOL (CMsgTeam::*)(OBJID, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTeam::*)(char*, DWORD)>(&CMsgTeam::Create)), BOOL (CMsgTeam::*)(char*, DWORD)>::value, "");
static_assert(_MSGTEAM_CREATE == 0 && _MSGTEAM_APPLYJOIN == 1 && _MSGTEAM_LEAVE == 2 && _MSGTEAM_INVITE == 4 && _MSGTEAM_OPENTEAM == 9, "");
static_assert(_MSGTEAM_CLOSEMONEYACCESS == 10 && _MSGTEAM_OPENITEMACCESS == 13, "");

TEST_CLASS(ClientMsgTeamHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgTeamProbe::InfoPtr() == 0x404);
    }
};
