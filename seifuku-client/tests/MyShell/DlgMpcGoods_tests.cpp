#include "stdafx.h"
#include "MyShell.h"
#include "DlgMpcGoods.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgMpcGoods>::value, "");
static_assert(std::is_polymorphic<CDlgMpcGoods>::value, "");
static_assert(CDlgMpcGoods::IDD == 153, "");
static_assert(ON_MPCGOODS_CLOSE == 24 && ON_MPCGOODS_MONEYDROP == 25, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_MPCGOODS == 13, "");
static_assert(IDC_MPCGOODS_BTN_EMONEY == 0x61A && IDC_MPCGOODS_STA_EMONEY == 0x469, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgMpcGoodsProbe
{
    static size_t Brush() { return offsetof(CDlgMpcGoods, m_Hbr); }
    static size_t InitSuc() { return offsetof(CDlgMpcGoods, m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgMpcGoods, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgMpcGoods, m_nPntX); }
    static size_t LastShowTime() { return offsetof(CDlgMpcGoods, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgMpcGoods, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgMpcGoods, m_nFrameMode); }
};

TEST_CLASS(ClientDlgMpcGoodsHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgMpcGoods, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgMpcGoodsProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgMpcGoodsProbe::InitSuc() == b + 0x08);
        Assert::IsTrue(DlgMpcGoodsProbe::Pnt() == b + 0x0C);
        Assert::IsTrue(DlgMpcGoodsProbe::PntX() == b + 0x14);
        Assert::IsTrue(DlgMpcGoodsProbe::LastShowTime() == b + 0x18);
        Assert::IsTrue(DlgMpcGoodsProbe::FrameStep() == b + 0x1C);
        Assert::IsTrue(DlgMpcGoodsProbe::FrameMode() == b + 0x20);
        const size_t ebtn = RoundUp(b + 0x24, __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgMpcGoods, m_MpcGoodsEmoneyBtn) == ebtn);
        const size_t esta = RoundUp(ebtn + sizeof(CMyButton), __alignof(CMyStatic));
        Assert::IsTrue(offsetof(CDlgMpcGoods, m_MpcGoodsEmoneySta) == esta);
        const size_t grid = RoundUp(esta + sizeof(CMyStatic), __alignof(CMyGrid));
        Assert::IsTrue(offsetof(CDlgMpcGoods, m_MpcGoodsGri) == grid);
        const size_t money = RoundUp(grid + sizeof(CMyGrid), __alignof(CMyStatic));
        Assert::IsTrue(offsetof(CDlgMpcGoods, m_MpcGoodsMoneySta) == money);
        const size_t close = RoundUp(money + sizeof(CMyStatic), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgMpcGoods, m_MpcGoodsCloseBtn) == close);
        Assert::IsTrue(offsetof(CDlgMpcGoods, m_MpcGoodsMoneyBtn) == RoundUp(close + sizeof(CMyButton), __alignof(CMyButton)));
        Assert::IsTrue(offsetof(CDlgMpcGoods, m_BtnHelp) == RoundUp(close + 2 * sizeof(CMyButton), __alignof(CMyButton)));
        Assert::IsTrue(sizeof(CDlgMpcGoods) == RoundUp(offsetof(CDlgMpcGoods, m_BtnHelp) + sizeof(CMyButton), __alignof(CDlgMpcGoods)));
    }
};
