#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgMapInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgMapInfoProbe : CMsgMapInfo
{
    typedef CMsgMapInfo::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgMapInfoProbe, m_pInfo); }
};

static_assert(sizeof(CMsgMapInfo) == 0x408, "");
static_assert(sizeof(MsgMapInfoProbe::Info) == 0x10, "");
static_assert(offsetof(MsgMapInfoProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgMapInfoProbe::Info, idMap) == 0x4, "");
static_assert(offsetof(MsgMapInfoProbe::Info, idDoc) == 0x8, "");
static_assert(offsetof(MsgMapInfoProbe::Info, dwType) == 0xC, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgMapInfo::*)(char*, DWORD)>(&CMsgMapInfo::Create)), BOOL (CMsgMapInfo::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgMapInfoHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgMapInfoProbe::InfoPtr() == 0x404);
    }
};
