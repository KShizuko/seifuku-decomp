#include "stdafx.h"
#include "MyShell.h"
#include "DlgNpcBuy.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgNpcBuy>::value, "");
static_assert(std::is_polymorphic<CDlgNpcBuy>::value, "");
static_assert(CDlgNpcBuy::IDD == 152, "");
static_assert(SHOP_AMOUNT == 20, "");
static_assert(ON_NPCBUY_CLOSE == 22 && ON_NPCBUY_FIX == 23, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_NPCBUY == 12 && DLG_NPCBUY_EMONEY == 45, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgNpcBuyProbe
{
    static size_t Brush() { return offsetof(CDlgNpcBuy, m_Hbr); }
    static size_t InitSuc() { return offsetof(CDlgNpcBuy, m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgNpcBuy, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgNpcBuy, m_nPntX); }
    static size_t CurrentAmount() { return offsetof(CDlgNpcBuy, m_nCurrentAmount); }
    static size_t LastShowTime() { return offsetof(CDlgNpcBuy, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgNpcBuy, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgNpcBuy, m_nFrameMode); }
};

TEST_CLASS(ClientDlgNpcBuyHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgNpcBuy, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgNpcBuyProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgNpcBuyProbe::InitSuc() == b + 0x08);
        Assert::IsTrue(DlgNpcBuyProbe::Pnt() == b + 0x0C);
        Assert::IsTrue(DlgNpcBuyProbe::PntX() == b + 0x14);
        Assert::IsTrue(DlgNpcBuyProbe::CurrentAmount() == b + 0x18);
        Assert::IsTrue(DlgNpcBuyProbe::LastShowTime() == b + 0x1C);
        Assert::IsTrue(DlgNpcBuyProbe::FrameStep() == b + 0x20);
        Assert::IsTrue(DlgNpcBuyProbe::FrameMode() == b + 0x24);
        const size_t grid = RoundUp(b + 0x28, __alignof(CMyGrid));
        Assert::IsTrue(offsetof(CDlgNpcBuy, m_NpcBuySaleGri) == grid);
        const size_t ab = __alignof(CMyButton);
        const size_t close = RoundUp(grid + sizeof(CMyGrid), ab);
        Assert::IsTrue(offsetof(CDlgNpcBuy, m_NpcBuyCloseBtn) == close);
        Assert::IsTrue(offsetof(CDlgNpcBuy, m_NpcBuyFixBtn) == close + 1 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgNpcBuy, m_BtnHelp) == close + 2 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgNpcBuy, m_NpcButUpBtn) == close + 3 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgNpcBuy, m_NpcButDownBtn) == close + 4 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(sizeof(CDlgNpcBuy) == RoundUp(offsetof(CDlgNpcBuy, m_NpcButDownBtn) + sizeof(CMyButton), __alignof(CDlgNpcBuy)));
    }
};
