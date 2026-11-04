#include "stdafx.h"
#include "MyShell.h"
#include "DlgImprove.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgImprove>::value, "");
static_assert(std::is_polymorphic<CDlgImprove>::value, "");
static_assert(CDlgImprove::IDD == 211, "");
static_assert(ON_IMPROVE_CLOSE == 34 && DLG_IMPROVE == 22, "");
static_assert(DLG_ALL == 48 && ON_ICON_DROP == 15, "");
static_assert(CMD_FLASHITEM == 1003, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgImproveProbe
{
    static size_t Brush() { return offsetof(CDlgImprove, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgImprove, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgImprove, m_nPntX); }
    static size_t PickUp() { return offsetof(CDlgImprove, m_bPickUp); }
    static size_t IconRealID() { return offsetof(CDlgImprove, m_nIconRealID); }
    static size_t GoodID() { return offsetof(CDlgImprove, m_GoodID); }
    static size_t GemID() { return offsetof(CDlgImprove, m_GemID); }
    static size_t GoodIndex() { return offsetof(CDlgImprove, m_nGoodIndex); }
    static size_t GemIndex() { return offsetof(CDlgImprove, m_nGemIndex); }
    static size_t ImproveType() { return offsetof(CDlgImprove, m_nImproveType); }
    static size_t LastShowTime() { return offsetof(CDlgImprove, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgImprove, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgImprove, m_nFrameMode); }
};

TEST_CLASS(ClientDlgImproveHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgImprove, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgImproveProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgImproveProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgImproveProbe::PntX() == b + 0x10);
        Assert::IsTrue(DlgImproveProbe::PickUp() == b + 0x14);
        Assert::IsTrue(DlgImproveProbe::IconRealID() == b + 0x18);
        Assert::IsTrue(DlgImproveProbe::GoodID() == b + 0x1C);
        Assert::IsTrue(DlgImproveProbe::GemID() == b + 0x20);
        Assert::IsTrue(DlgImproveProbe::GoodIndex() == b + 0x24);
        Assert::IsTrue(DlgImproveProbe::GemIndex() == b + 0x28);
        Assert::IsTrue(DlgImproveProbe::ImproveType() == b + 0x2C);
        Assert::IsTrue(DlgImproveProbe::LastShowTime() == b + 0x30);
        Assert::IsTrue(DlgImproveProbe::FrameStep() == b + 0x34);
        Assert::IsTrue(DlgImproveProbe::FrameMode() == b + 0x38);
        const size_t ab = __alignof(CMyButton);
        const size_t closeb = RoundUp(b + 0x3C, ab);
        Assert::IsTrue(offsetof(CDlgImprove, m_ImproveBtnCloseB) == closeb);
        Assert::IsTrue(offsetof(CDlgImprove, m_BtnHelp) == closeb + 1 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgImprove, m_ImproveBtnClose) == closeb + 2 * RoundUp(sizeof(CMyButton), ab));
        const size_t good = RoundUp(closeb + 3 * RoundUp(sizeof(CMyButton), ab), __alignof(CMyImage));
        Assert::IsTrue(offsetof(CDlgImprove, m_ImproveImaGood) == good);
        const size_t gem = RoundUp(good + sizeof(CMyImage), __alignof(CMyImage));
        Assert::IsTrue(offsetof(CDlgImprove, m_ImproveImaGem) == gem);
        const size_t imp = RoundUp(gem + sizeof(CMyImage), ab);
        Assert::IsTrue(offsetof(CDlgImprove, m_ImproveBtnImp) == imp);
        Assert::IsTrue(sizeof(CDlgImprove) == RoundUp(imp + sizeof(CMyButton), __alignof(CDlgImprove)));
    }
};
