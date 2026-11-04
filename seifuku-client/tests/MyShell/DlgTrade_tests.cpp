#include "stdafx.h"
#include "MyShell.h"
#include "DlgTrade.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgTrade>::value, "");
static_assert(std::is_polymorphic<CDlgTrade>::value, "");
static_assert(CDlgTrade::IDD == 137, "");
static_assert(TRADE_PAGE_AMOUNT == 6, "");
static_assert(DLG_TRADENPC == 15 && DLG_TRADEMPC == 16, "");
static_assert(ON_TRADE_CLOSE == 29 && ON_HELPDLG_SHOW == 53 && DLG_TRADE == 14, "");
static_assert(ICON_TYPE_ITEM == 0, "");
static_assert(IDC_TRADE_STA_NPCEMONEY == 1150 && IDC_TRADE_EDT_MPCEMONEY == 1573, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgTradeProbe
{
    static size_t NpcPage() { return offsetof(CDlgTrade, m_nNpcPage); }
    static size_t MpcPage() { return offsetof(CDlgTrade, m_nMpcPage); }
    static size_t Brush() { return offsetof(CDlgTrade, m_Hbr); }
    static size_t BrushStatic() { return offsetof(CDlgTrade, m_HbrStatic); }
    static size_t Money() { return offsetof(CDlgTrade, m_nMoney); }
    static size_t Emoney() { return offsetof(CDlgTrade, m_nEmoney); }
    static size_t Change() { return offsetof(CDlgTrade, m_nChange); }
    static size_t EmoneyChange() { return offsetof(CDlgTrade, m_nEmoneyChange); }
    static size_t InitSuc() { return offsetof(CDlgTrade, m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgTrade, m_Pnt); }
    static size_t MoneyAdd() { return offsetof(CDlgTrade, m_bMoneyAdd); }
    static size_t NpcNameLen() { return offsetof(CDlgTrade, m_nNpcNameLen); }
};

TEST_CLASS(ClientDlgTradeHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgTrade, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgTradeProbe::NpcPage() == b + 0x04);
        Assert::IsTrue(DlgTradeProbe::MpcPage() == b + 0x08);
        Assert::IsTrue(DlgTradeProbe::Brush() == b + 0x0C);
        Assert::IsTrue(DlgTradeProbe::BrushStatic() == b + 0x10);
        Assert::IsTrue(DlgTradeProbe::Money() == b + 0x14);
        Assert::IsTrue(DlgTradeProbe::Emoney() == b + 0x18);
        Assert::IsTrue(DlgTradeProbe::Change() == b + 0x1C);
        Assert::IsTrue(DlgTradeProbe::EmoneyChange() == b + 0x20);
        Assert::IsTrue(DlgTradeProbe::InitSuc() == b + 0x24);
        Assert::IsTrue(DlgTradeProbe::Pnt() == b + 0x28);
        Assert::IsTrue(DlgTradeProbe::MoneyAdd() == b + 0x30);
        Assert::IsTrue(DlgTradeProbe::NpcNameLen() == b + 0x34);
        const size_t as = __alignof(CMyStatic);
        const size_t sta = RoundUp(sizeof(CMyStatic), as);
        const size_t emoney = RoundUp(b + 0x38, as);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeStaNpcEmoney) == emoney);
        const size_t eedit = RoundUp(emoney + sta, __alignof(CMyEdit));
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeEdtMpcEmoney) == eedit);
        const size_t level = RoundUp(eedit + sizeof(CMyEdit), as);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeStaNpcLevel) == level);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeStaNpcAllPage) == level + 1 * sta);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeStaMpcAllPage) == level + 2 * sta);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t npcu = RoundUp(level + 3 * sta, ab);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeBtnNpcU) == npcu);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeBtnNpcD) == npcu + 1 * btn);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeBtnMpcU) == npcu + 2 * btn);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeBtnMpcD) == npcu + 3 * btn);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeCloseB) == npcu + 4 * btn);
        const size_t ag = __alignof(CMyGrid);
        const size_t grid = RoundUp(sizeof(CMyGrid), ag);
        const size_t npc = RoundUp(npcu + 5 * btn, ag);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeGriNpc) == npc);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeGriMpc) == npc + grid);
        const size_t name = RoundUp(npc + 2 * grid, as);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeStaNpcName) == name);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeStaNpcMoney) == name + 1 * sta);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeStaMpcPage) == name + 2 * sta);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeStaNpcPage) == name + 3 * sta);
        const size_t edit = RoundUp(name + 4 * sta, __alignof(CMyEdit));
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeEdtMpcMoney) == edit);
        const size_t agree = RoundUp(edit + sizeof(CMyEdit), as);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeStaNpcAgree) == agree);
        const size_t mpcagree = RoundUp(agree + sizeof(CMyStatic), ab);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeBtnMpcAgree) == mpcagree);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeBtnMoneyMpcOk) == mpcagree + 1 * btn);
        Assert::IsTrue(offsetof(CDlgTrade, m_TradeBtnClose) == mpcagree + 2 * btn);
        Assert::IsTrue(offsetof(CDlgTrade, m_BtnHelp) == mpcagree + 3 * btn);
        Assert::IsTrue(sizeof(CDlgTrade) == RoundUp(mpcagree + 4 * btn, __alignof(CDlgTrade)));
    }
};
