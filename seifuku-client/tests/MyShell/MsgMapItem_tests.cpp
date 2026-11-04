#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgMapItem.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgMapItemProbe : CMsgMapItem
{
    typedef CMsgMapItem::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgMapItemProbe, m_pInfo); }
};

static_assert(sizeof(CMsgMapItem) == 0x408, "");
static_assert(sizeof(MsgMapItemProbe::Info) == 0x14, "");
static_assert(offsetof(MsgMapItemProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgMapItemProbe::Info, id) == 0x4, "");
static_assert(offsetof(MsgMapItemProbe::Info, idType) == 0x8 && offsetof(MsgMapItemProbe::Info, usLook) == 0x8, "");
static_assert(offsetof(MsgMapItemProbe::Info, usPosX) == 0xC && offsetof(MsgMapItemProbe::Info, usPosY) == 0xE, "");
static_assert(offsetof(MsgMapItemProbe::Info, ucAction) == 0x10, "");
static_assert(offsetof(MsgMapItemProbe::Info, szBuf) == 0x11, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgMapItem::*)(OBJID, int, int, unsigned char)>(&CMsgMapItem::Create)), BOOL (CMsgMapItem::*)(OBJID, int, int, unsigned char)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgMapItem::*)(char*, DWORD)>(&CMsgMapItem::Create)), BOOL (CMsgMapItem::*)(char*, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMsgMapItem::Send), void (CMsgMapItem::*)(void)>::value, "");
static_assert(MSGMAPITEM_PICK == 3, "");

TEST_CLASS(ClientMsgMapItemHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgMapItemProbe::InfoPtr() == 0x404);
    }
};
