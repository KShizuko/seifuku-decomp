#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgMagicInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgMagicInfoProbe : CMsgMagicInfo
{
    typedef CMsgMagicInfo::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgMagicInfoProbe, m_pInfo); }
};

static_assert(sizeof(CMsgMagicInfo) == 0x408, "");
static_assert(sizeof(MsgMagicInfoProbe::Info) == 0xC, "");
static_assert(offsetof(MsgMagicInfoProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgMagicInfoProbe::Info, dwExp) == 0x4, "");
static_assert(offsetof(MsgMagicInfoProbe::Info, usType) == 0x8, "");
static_assert(offsetof(MsgMagicInfoProbe::Info, usLevel) == 0xA, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgMagicInfo::*)(char*, DWORD)>(&CMsgMagicInfo::Create)), BOOL (CMsgMagicInfo::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgMagicInfoHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgMagicInfoProbe::InfoPtr() == 0x404);
    }
};
