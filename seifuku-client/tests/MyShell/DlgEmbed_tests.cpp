#include "stdafx.h"
#include "MyShell.h"
#include "DlgEmbed.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgEmbed>::value, "");
static_assert(std::is_polymorphic<CDlgEmbed>::value, "");
static_assert(ON_ICON_DROP == 15 && DLG_ALL == 48 && CMD_FLASHITEM == 1003, "");
static_assert(ON_EMBED_CLOSE == 66 && ON_EMBED_TACKOFF == 88 && ON_HELPDLG_SHOW == 53 && DLG_EMBED == 24, "");
static_assert(ITEM6_IMAGE == 6, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgEmbedProbe
{
    static int Idd() { return CDlgEmbed::IDD; }
    static size_t Brush() { return offsetof(CDlgEmbed, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgEmbed, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgEmbed, m_nPntX); }
    static size_t PickUp() { return offsetof(CDlgEmbed, m_bPickUp); }
    static size_t IconRealID() { return offsetof(CDlgEmbed, m_nIconRealID); }
    static size_t GoodID() { return offsetof(CDlgEmbed, m_GoodID); }
    static size_t Gem1ID() { return offsetof(CDlgEmbed, m_Gem1ID); }
    static size_t Gem2ID() { return offsetof(CDlgEmbed, m_Gem2ID); }
    static size_t Gem1Old() { return offsetof(CDlgEmbed, m_bGem1Old); }
    static size_t Gem2Old() { return offsetof(CDlgEmbed, m_bGem2Old); }
    static size_t GoodIndex() { return offsetof(CDlgEmbed, m_nGoodIndex); }
    static size_t Gem1Index() { return offsetof(CDlgEmbed, m_nGem1Index); }
    static size_t Gem2Index() { return offsetof(CDlgEmbed, m_nGem2Index); }
    static size_t CurGem() { return offsetof(CDlgEmbed, m_nCurGem); }
    static size_t TackOffWait() { return offsetof(CDlgEmbed, m_bTackOffWait); }
    static size_t TackOffGem() { return offsetof(CDlgEmbed, m_nTackOffGem); }
    static size_t LastShowTime() { return offsetof(CDlgEmbed, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgEmbed, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgEmbed, m_nFrameMode); }
    static size_t Closes() { return offsetof(CDlgEmbed, m_EmbedBtnCloses); }
    static size_t Good() { return offsetof(CDlgEmbed, m_EmbedImaGood); }
    static size_t Gem2() { return offsetof(CDlgEmbed, m_EmbedImaGem2); }
    static size_t Gem1() { return offsetof(CDlgEmbed, m_EmbedImaGem1); }
    static size_t Close() { return offsetof(CDlgEmbed, m_EmbedBtnClose); }
    static size_t TackOff() { return offsetof(CDlgEmbed, m_EmbedBtnTackOff); }
    static size_t Embed() { return offsetof(CDlgEmbed, m_EmbedBtnEmbed); }
    static size_t Help() { return offsetof(CDlgEmbed, m_BtnHelp); }
};

TEST_CLASS(ClientDlgEmbedHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgEmbedProbe::Idd() == 229);
        Assert::IsTrue(offsetof(CDlgEmbed, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgEmbedProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgEmbedProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgEmbedProbe::PntX() == b + 0x10);
        Assert::IsTrue(DlgEmbedProbe::PickUp() == b + 0x14);
        Assert::IsTrue(DlgEmbedProbe::IconRealID() == b + 0x18);
        Assert::IsTrue(DlgEmbedProbe::GoodID() == b + 0x1C);
        Assert::IsTrue(DlgEmbedProbe::Gem1ID() == b + 0x20);
        Assert::IsTrue(DlgEmbedProbe::Gem2ID() == b + 0x24);
        Assert::IsTrue(DlgEmbedProbe::Gem1Old() == b + 0x28);
        Assert::IsTrue(DlgEmbedProbe::Gem2Old() == b + 0x2C);
        Assert::IsTrue(DlgEmbedProbe::GoodIndex() == b + 0x30);
        Assert::IsTrue(DlgEmbedProbe::Gem1Index() == b + 0x34);
        Assert::IsTrue(DlgEmbedProbe::Gem2Index() == b + 0x38);
        Assert::IsTrue(DlgEmbedProbe::CurGem() == b + 0x3C);
        Assert::IsTrue(DlgEmbedProbe::TackOffWait() == b + 0x40);
        Assert::IsTrue(DlgEmbedProbe::TackOffGem() == b + 0x44);
        Assert::IsTrue(DlgEmbedProbe::LastShowTime() == b + 0x48);
        Assert::IsTrue(DlgEmbedProbe::FrameStep() == b + 0x4C);
        Assert::IsTrue(DlgEmbedProbe::FrameMode() == b + 0x50);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t closes = RoundUp(b + 0x54, ab);
        Assert::IsTrue(DlgEmbedProbe::Closes() == closes);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        const size_t good = RoundUp(closes + sizeof(CMyButton), ai);
        Assert::IsTrue(DlgEmbedProbe::Good() == good);
        Assert::IsTrue(DlgEmbedProbe::Gem2() == good + 1 * img);
        Assert::IsTrue(DlgEmbedProbe::Gem1() == good + 2 * img);
        const size_t close = RoundUp(good + 3 * img, ab);
        Assert::IsTrue(DlgEmbedProbe::Close() == close);
        Assert::IsTrue(DlgEmbedProbe::TackOff() == close + 1 * btn);
        Assert::IsTrue(DlgEmbedProbe::Embed() == close + 2 * btn);
        Assert::IsTrue(DlgEmbedProbe::Help() == close + 3 * btn);
        Assert::IsTrue(sizeof(CDlgEmbed) == RoundUp(close + 4 * btn, __alignof(CDlgEmbed)));
    }
};
