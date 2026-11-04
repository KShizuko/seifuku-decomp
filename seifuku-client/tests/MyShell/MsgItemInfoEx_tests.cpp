#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgItemInfoEx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgItemInfoExProbe : CMsgItemInfoEx
{
    typedef CMsgItemInfoEx::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgItemInfoExProbe, m_pInfo); }
};

static_assert(sizeof(CMsgItemInfoEx) == 0x408, "");
static_assert(sizeof(MsgItemInfoExProbe::Info) == 0x2C, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, id) == 0x4, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, idOwner) == 0x8, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, dwPrice) == 0xC, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, dwType) == 0x10, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, usAmount) == 0x14 && offsetof(MsgItemInfoExProbe::Info, usAmountLimit) == 0x16, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, ucAction) == 0x18, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, ucStatus) == 0x19 && offsetof(MsgItemInfoExProbe::Info, ucPosition) == 0x1A, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, ucGem1) == 0x1B && offsetof(MsgItemInfoExProbe::Info, ucMagic3) == 0x1F, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, ucBless) == 0x20 && offsetof(MsgItemInfoExProbe::Info, ucEnchant) == 0x21, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, dwRestrain) == 0x24, "");
static_assert(offsetof(MsgItemInfoExProbe::Info, szBuf) == 0x28, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgItemInfoEx::*)(char*, DWORD)>(&CMsgItemInfoEx::Create)), BOOL (CMsgItemInfoEx::*)(char*, DWORD)>::value, "");
static_assert(ITEMINFOEX_BOOTH == 1 && ITEMINFOEX_EQUIPMENT == 2, "");
static_assert(ITEMINFOEX_BOOTH_EMONEY == 3, "");

TEST_CLASS(ClientMsgItemInfoExHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgItemInfoExProbe::InfoPtr() == 0x404);
    }
};
