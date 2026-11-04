#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgWalk.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgWalkProbe : CMsgWalk
{
    typedef CMsgWalk::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgWalkProbe, m_pInfo); }
};

static_assert(sizeof(CMsgWalk) == 0x408, "");
static_assert(sizeof(MsgWalkProbe::Info) == 0xC, "");
static_assert(offsetof(MsgWalkProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgWalkProbe::Info, idUser) == 0x4, "");
static_assert(offsetof(MsgWalkProbe::Info, ucDir) == 0x8, "");
static_assert(offsetof(MsgWalkProbe::Info, ucRun) == 0x9, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgWalk::*)(OBJID, int, int)>(&CMsgWalk::Create)), BOOL (CMsgWalk::*)(OBJID, int, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgWalk::*)(char*, DWORD)>(&CMsgWalk::Create)), BOOL (CMsgWalk::*)(char*, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMsgWalk::Send), void (CMsgWalk::*)(void)>::value, "");

TEST_CLASS(ClientMsgWalkHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgWalkProbe::InfoPtr() == 0x404);
    }
};
