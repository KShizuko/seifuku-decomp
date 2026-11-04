#include "stdafx.h"
#include "MyShell.h"
#include "DlgDepot.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgDepot>::value, "");
static_assert(std::is_polymorphic<CDlgDepot>::value, "");
static_assert(CDlgDepot::IDD == 161, "");
static_assert(DEPOT_PAGE_AMOUNT == 20, "");
static_assert(ON_NPCBUY_CLOSE == 22 && DLG_DEPOT == 17, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgDepotProbe
{
    static size_t Brush() { return offsetof(CDlgDepot, m_Hbr); }
    static size_t Black() { return offsetof(CDlgDepot, m_HbrBlack); }
    static size_t Money() { return offsetof(CDlgDepot, m_nMoney); }
    static size_t Change() { return offsetof(CDlgDepot, m_nChange); }
    static size_t InitSuc() { return offsetof(CDlgDepot, m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgDepot, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgDepot, m_nPntX); }
    static size_t LastShowTime() { return offsetof(CDlgDepot, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgDepot, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgDepot, m_nFrameMode); }
};

TEST_CLASS(ClientDlgDepotHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgDepot, m_bShow) == b + 0x00);
        Assert::IsTrue(offsetof(CDlgDepot, m_nCurPage) == b + 0x04);
        Assert::IsTrue(DlgDepotProbe::Brush() == b + 0x08);
        Assert::IsTrue(DlgDepotProbe::Black() == b + 0x0C);
        Assert::IsTrue(DlgDepotProbe::Money() == b + 0x10);
        Assert::IsTrue(DlgDepotProbe::Change() == b + 0x14);
        Assert::IsTrue(DlgDepotProbe::InitSuc() == b + 0x18);
        Assert::IsTrue(DlgDepotProbe::Pnt() == b + 0x1C);
        Assert::IsTrue(DlgDepotProbe::PntX() == b + 0x24);
        Assert::IsTrue(DlgDepotProbe::LastShowTime() == b + 0x28);
        Assert::IsTrue(DlgDepotProbe::FrameStep() == b + 0x2C);
        Assert::IsTrue(DlgDepotProbe::FrameMode() == b + 0x30);
        const size_t as = __alignof(CMyStatic);
        const size_t page = RoundUp(b + 0x34, as);
        Assert::IsTrue(offsetof(CDlgDepot, m_DepotStaPage) == page);
        Assert::IsTrue(offsetof(CDlgDepot, m_DepotStaAllPage) == page + RoundUp(sizeof(CMyStatic), as));
        const size_t ab = __alignof(CMyButton);
        const size_t up = RoundUp(page + 2 * RoundUp(sizeof(CMyStatic), as), ab);
        Assert::IsTrue(offsetof(CDlgDepot, m_DepotBtnU) == up);
        Assert::IsTrue(offsetof(CDlgDepot, m_DepotBtnD) == up + 1 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgDepot, m_DepotCloseB) == up + 2 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgDepot, m_DepotBtnAdd) == up + 3 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgDepot, m_DepotBtnGet) == up + 4 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgDepot, m_DepotClose) == up + 5 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgDepot, m_BtnHelp) == up + 6 * RoundUp(sizeof(CMyButton), ab));
        const size_t edit = RoundUp(up + 7 * RoundUp(sizeof(CMyButton), ab), __alignof(CMyEdit));
        Assert::IsTrue(offsetof(CDlgDepot, m_DepotEdtMoney) == edit);
        const size_t lay = RoundUp(edit + sizeof(CMyEdit), as);
        Assert::IsTrue(offsetof(CDlgDepot, m_DepotStaLay) == lay);
        const size_t grid = RoundUp(lay + sizeof(CMyStatic), __alignof(CMyGrid));
        Assert::IsTrue(offsetof(CDlgDepot, m_DepotGriLay) == grid);
        Assert::IsTrue(sizeof(CDlgDepot) == RoundUp(grid + sizeof(CMyGrid), __alignof(CDlgDepot)));
    }
};
