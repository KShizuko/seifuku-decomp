#include "stdafx.h"
#include "MyShell.h"
#include "DlgFuse.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

constexpr size_t RoundUpC(size_t n, size_t a) { return (n + a - 1) / a * a; }

static_assert(std::is_base_of<CDialog, CDlgFuse>::value, "");
static_assert(std::is_polymorphic<CDlgFuse>::value, "");
static_assert(CDlgFuse::IDD == 278, "");
static_assert(FUSE_ITEM_AMOUNT == 20, "");
static_assert(DLG_FUSE == 43 && ON_HELPDLG_SHOW == 53, "");
static_assert(ON_FUSE_CLOSE == 104, "");
static_assert(sizeof(FUSE_INFO) == RoundUpC(1 + sizeof(list<OBJID>), 4), "");
static_assert(offsetof(FUSE_INFO, setItem) == 4, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgFuseProbe
{
    static size_t Brush() { return offsetof(CDlgFuse, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgFuse, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgFuse, m_nPntX); }
    static size_t LastShowTime() { return offsetof(CDlgFuse, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgFuse, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgFuse, m_nFrameMode); }
    static size_t FuseInfo() { return offsetof(CDlgFuse, m_FuseInfo); }
    static size_t Image() { return offsetof(CDlgFuse, m_btImage); }
    static size_t Item() { return offsetof(CDlgFuse, m_idItem); }
};

TEST_CLASS(ClientDlgFuseHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgFuse, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgFuseProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgFuseProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgFuseProbe::PntX() == b + 0x10);
        Assert::IsTrue(DlgFuseProbe::LastShowTime() == b + 0x14);
        Assert::IsTrue(DlgFuseProbe::FrameStep() == b + 0x18);
        Assert::IsTrue(DlgFuseProbe::FrameMode() == b + 0x1C);
        Assert::IsTrue(DlgFuseProbe::FuseInfo() == b + 0x20);
        const size_t rec = b + 0x20 + sizeof(FUSE_INFO);
        Assert::IsTrue(DlgFuseProbe::Image() == rec);
        Assert::IsTrue(DlgFuseProbe::Item() == rec + 4);
        const size_t as = __alignof(CMyStatic);
        const size_t odds = RoundUp(rec + 8, as);
        Assert::IsTrue(offsetof(CDlgFuse, m_StaOdds) == odds);
        Assert::IsTrue(offsetof(CDlgFuse, m_StaLevel) == odds + RoundUp(sizeof(CMyStatic), as));
        const size_t grid = RoundUp(odds + 2 * RoundUp(sizeof(CMyStatic), as), __alignof(CMyGrid));
        Assert::IsTrue(offsetof(CDlgFuse, m_GriLay) == grid);
        const size_t ab = __alignof(CMyButton);
        const size_t fuse = RoundUp(grid + sizeof(CMyGrid), ab);
        Assert::IsTrue(offsetof(CDlgFuse, m_BtnFuse) == fuse);
        const size_t chk = RoundUp(fuse + sizeof(CMyButton), __alignof(CMyCheck));
        Assert::IsTrue(offsetof(CDlgFuse, m_ChkOk) == chk);
        const size_t help = RoundUp(chk + sizeof(CMyCheck), ab);
        Assert::IsTrue(offsetof(CDlgFuse, m_BtnHelp) == help);
        Assert::IsTrue(offsetof(CDlgFuse, m_BtnCloseC) == help + 1 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgFuse, m_BtnClose) == help + 2 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(sizeof(CDlgFuse) == RoundUp(help + 3 * RoundUp(sizeof(CMyButton), ab), __alignof(CDlgFuse)));
    }
};
