#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgSyndicate.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgSyndicateProbe : CMsgSyndicate
{
    typedef CMsgSyndicate::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgSyndicateProbe, m_pInfo); }
};

static_assert(sizeof(CMsgSyndicate) == 0x408, "");
static_assert(sizeof(MsgSyndicateProbe::Info) == 0xC, "");
static_assert(offsetof(MsgSyndicateProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgSyndicateProbe::Info, usAction) == 0x4, "");
static_assert(offsetof(MsgSyndicateProbe::Info, idTarget) == 0x8 && offsetof(MsgSyndicateProbe::Info, dwData) == 0x8, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgSyndicate::*)(unsigned short, OBJID)>(&CMsgSyndicate::Create)), BOOL (CMsgSyndicate::*)(unsigned short, OBJID)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgSyndicate::*)(char*, DWORD)>(&CMsgSyndicate::Create)), BOOL (CMsgSyndicate::*)(char*, DWORD)>::value, "");
static_assert(APPLY_JOIN == 1 && INVITE_JOIN == 2 && LEAVE_SYN == 3 && QUERY_SYN_NAME == 6 && DONATE_MONEY == 11 && QUERY_SYNATTR == 12, "");
static_assert(ALLY_APPLY == 7 && CLEAR_ALLY == 8 && ANTAGONIZE == 9 && CLEAR_ANTAGONIZE == 10 && SET_SYN == 14, "");
static_assert(UNITE_SUBSYN == 15 && UNITE_SYN == 16 && SET_WHITE_SYN == 17 && SET_BLACK_SYN == 18 && DESTROY_SYN == 19, "");

TEST_CLASS(ClientMsgSyndicateHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgSyndicateProbe::InfoPtr() == 0x404);
    }
};
