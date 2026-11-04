#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgFuse.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgFuseProbe : CMsgFuse
{
    typedef CMsgFuse::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgFuseProbe, m_pInfo); }
};

static_assert(sizeof(CMsgFuse) == 0x408, "");
static_assert(sizeof(MsgFuseProbe::Info) == 0x10, "");
static_assert(offsetof(MsgFuseProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgFuseProbe::Info, idUser) == 0x4, "");
static_assert(offsetof(MsgFuseProbe::Info, usAction) == 0x8, "");
static_assert(offsetof(MsgFuseProbe::Info, usAmount) == 0xA, "");
static_assert(offsetof(MsgFuseProbe::Info, dwData) == 0xC, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgFuse::*)(OBJID, USHORT, DWORD)>(&CMsgFuse::Create)), BOOL (CMsgFuse::*)(OBJID, USHORT, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMsgFuse::Append), void (CMsgFuse::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgFuse::*)(char*, DWORD)>(&CMsgFuse::Create)), BOOL (CMsgFuse::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgFuseHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgFuseProbe::InfoPtr() == 0x404);
    }
};
