#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgItemInfo.h"
#include "Item.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgItemInfoProbe : CMsgItemInfo
{
    typedef CMsgItemInfo::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgItemInfoProbe, m_pInfo); }
};

static_assert(sizeof(CMsgItemInfo) == 0x408, "");
static_assert(sizeof(MsgItemInfoProbe::Info) == 0x20, "");
static_assert(offsetof(MsgItemInfoProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgItemInfoProbe::Info, id) == 0x4, "");
static_assert(offsetof(MsgItemInfoProbe::Info, dwType) == 0x8, "");
static_assert(offsetof(MsgItemInfoProbe::Info, usAmount) == 0xC, "");
static_assert(offsetof(MsgItemInfoProbe::Info, usAmountLimit) == 0xE, "");
static_assert(offsetof(MsgItemInfoProbe::Info, ucAction) == 0x10, "");
static_assert(offsetof(MsgItemInfoProbe::Info, ucStatus) == 0x11, "");
static_assert(offsetof(MsgItemInfoProbe::Info, ucPosition) == 0x12, "");
static_assert(offsetof(MsgItemInfoProbe::Info, ucGem1) == 0x13 && offsetof(MsgItemInfoProbe::Info, ucGem2) == 0x14, "");
static_assert(offsetof(MsgItemInfoProbe::Info, ucMagic1) == 0x15 && offsetof(MsgItemInfoProbe::Info, ucMagic2) == 0x16 && offsetof(MsgItemInfoProbe::Info, ucMagic3) == 0x17, "");
static_assert(offsetof(MsgItemInfoProbe::Info, ucBless) == 0x18 && offsetof(MsgItemInfoProbe::Info, ucEnchant) == 0x19, "");
static_assert(offsetof(MsgItemInfoProbe::Info, dwRestrain) == 0x1C, "");
static_assert(ITEMINFO_NONE == 0 && ITEMINFO_ADDITEM == 1 && ITEMINFO_TRADE == 2 && ITEMINFO_UPDATE == 3 && ITEMINFO_OTHERPLAYER_EQUIPMENT == 4, "");
static_assert(std::is_same<decltype(&CMsgItemInfo::Create), BOOL (CMsgItemInfo::*)(char*, DWORD)>::value, "");

static_assert(ITEMPOSITION_BACKPACK == 0, "");

TEST_CLASS(ClientMsgItemInfoHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgItemInfoProbe::InfoPtr() == 0x404);
    }
};
