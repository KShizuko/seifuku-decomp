#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgTrade.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgTradeProbe : CMsgTrade
{
    typedef CMsgTrade::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgTradeProbe, m_pInfo); }
};

static_assert(sizeof(CMsgTrade) == 0x408, "");
static_assert(sizeof(MsgTradeProbe::Info) == 0xC, "");
static_assert(offsetof(MsgTradeProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgTradeProbe::Info, id) == 0x4 && offsetof(MsgTradeProbe::Info, dwData) == 0x4, "");
static_assert(offsetof(MsgTradeProbe::Info, usAction) == 0x8, "");
static_assert(_TRADE_PLAYERTOTALEMONEY == 12 && _TRADE_ADDEMONEY == 13 && _TRADE_HEROTOTALEMONEY == 14, "");
static_assert(_TRADE_APPLY == 1 && _TRADE_QUIT == 2 && _TRADE_OPEN == 3 && _TRADE_SUCCESS == 4 && _TRADE_FALSE == 5 && _TRADE_ADDITEM == 6 && _TRADE_ADDMONEY == 7 && _TRADE_PLAYERTOTALMONEY == 8 && _TRADE_HEROTOTALMONEY == 9 && _TRADE_OK == 10 && _TRADE_ADDITEM_FALSE == 11, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTrade::*)(OBJID, unsigned short)>(&CMsgTrade::Create)), BOOL (CMsgTrade::*)(OBJID, unsigned short)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTrade::*)(char*, DWORD)>(&CMsgTrade::Create)), BOOL (CMsgTrade::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgTradeHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgTradeProbe::InfoPtr() == 0x404);
    }
};
