#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgItem.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgItemProbe : CMsgItem
{
    typedef CMsgItem::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgItemProbe, m_pInfo); }
};

static_assert(sizeof(CMsgItem) == 0x408, "");
static_assert(sizeof(MsgItemProbe::Info) == 0x14, "");
static_assert(offsetof(MsgItemProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgItemProbe::Info, id) == 0x4, "");
static_assert(offsetof(MsgItemProbe::Info, dwData) == 0x8, "");
static_assert(offsetof(MsgItemProbe::Info, usPosY) == 0x8 && offsetof(MsgItemProbe::Info, usPosX) == 0xA, "");
static_assert(offsetof(MsgItemProbe::Info, usAction) == 0xC, "");
static_assert(offsetof(MsgItemProbe::Info, dwTimeStamp) == 0x10, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgItem::*)(OBJID, DWORD, int)>(&CMsgItem::Create)), BOOL (CMsgItem::*)(OBJID, DWORD, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgItem::*)(OBJID, USHORT, USHORT, int)>(&CMsgItem::Create)), BOOL (CMsgItem::*)(OBJID, USHORT, USHORT, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgItem::*)(char*, DWORD)>(&CMsgItem::Create)), BOOL (CMsgItem::*)(char*, DWORD)>::value, "");
static_assert(ITEMACT_BUY == 1 && ITEMACT_SELL == 2 && ITEMACT_DROP == 3 && ITEMACT_USE == 4 && ITEMACT_UNEQUIP == 6 && ITEMACT_COMBINEITEM == 8, "");
static_assert(ITEMACT_QUERYMONEYSAVED == 9 && ITEMACT_SAVEMONEY == 10 && ITEMACT_DRAWMONEY == 11 && ITEMACT_DROPMONEY == 12 && ITEMACT_REPAIR == 14, "");
static_assert(ITEMACT_IMPROVE == 19 && ITEMACT_UPLEV == 20 && ITEMACT_BOOTH_QUERY == 21 && ITEMACT_BOOTH_ADD == 22 && ITEMACT_BOOTH_DEL == 23 && ITEMACT_BOOTH_BUY == 24, "");
static_assert(ITEMACT_EQUIP == 5 && ITEMACT_DURABILITY == 17 && ITEMACT_DROPEQUIPMENT == 18 && ITEMACT_SYNCHRO_AMOUNT == 25 && ITEMACT_FIREWORKS == 26 && ITEMACT_PING == 27, "");
static_assert(ITEMACT_ENCHANT == 28, "");
static_assert(ITEMACT_BOOTH_ADD_EMONEY == 29, "");

TEST_CLASS(ClientMsgItemHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgItemProbe::InfoPtr() == 0x404);
    }
};
