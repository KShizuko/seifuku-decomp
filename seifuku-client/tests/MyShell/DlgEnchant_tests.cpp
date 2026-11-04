#include "stdafx.h"
#include "MyShell.h"
#include "DlgEnchant.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgEnchant>::value, "");
static_assert(std::is_polymorphic<CDlgEnchant>::value, "");
static_assert(CDlgEnchant::IDD == 105, "");
static_assert(IDC_ENCHANT_IMA_GEM == 1444 && IDC_ENCHANT_IMA_GOOD == 1445 && IDC_ENCHANT_BTN_OK == 1446 && IDC_ENCHANT_BTN_CLOSE == 1447 && IDC_ENCHANT_BTN_CLOSES == 1448, "");
static_assert(ON_ENCHANT_CLOSE == 107 && ON_HELPDLG_SHOW == 53 && DLG_ENCHANT == 44, "");
static_assert(ON_ICON_DROP == 15 && DLG_ALL == 48 && CMD_FLASHITEM == 1003, "");
static_assert(ITEM13_IMAGE == 13, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgEnchantProbe
{
    static size_t Brush() { return offsetof(CDlgEnchant, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgEnchant, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgEnchant, m_nPntX); }
    static size_t PickUp() { return offsetof(CDlgEnchant, m_bPickUp); }
    static size_t IconRealID() { return offsetof(CDlgEnchant, m_nIconRealID); }
    static size_t GoodID() { return offsetof(CDlgEnchant, m_GoodID); }
    static size_t GemID() { return offsetof(CDlgEnchant, m_GemID); }
    static size_t GoodIndex() { return offsetof(CDlgEnchant, m_nGoodIndex); }
    static size_t GemIndex() { return offsetof(CDlgEnchant, m_nGemIndex); }
    static size_t LastShowTime() { return offsetof(CDlgEnchant, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgEnchant, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgEnchant, m_nFrameMode); }
};

TEST_CLASS(ClientDlgEnchantHeader)
{
public:
    TEST_METHOD(LayoutFollowsTheCtor)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgEnchant, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgEnchantProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgEnchantProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgEnchantProbe::PntX() == b + 0x10);
        Assert::IsTrue(DlgEnchantProbe::PickUp() == b + 0x14);
        Assert::IsTrue(DlgEnchantProbe::IconRealID() == b + 0x18);
        Assert::IsTrue(DlgEnchantProbe::GoodID() == b + 0x1C);
        Assert::IsTrue(DlgEnchantProbe::GemID() == b + 0x20);
        Assert::IsTrue(DlgEnchantProbe::GoodIndex() == b + 0x24);
        Assert::IsTrue(DlgEnchantProbe::GemIndex() == b + 0x28);
        Assert::IsTrue(DlgEnchantProbe::LastShowTime() == b + 0x2C);
        Assert::IsTrue(DlgEnchantProbe::FrameStep() == b + 0x30);
        Assert::IsTrue(DlgEnchantProbe::FrameMode() == b + 0x34);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t help = RoundUp(b + 0x38, ab);
        Assert::IsTrue(offsetof(CDlgEnchant, m_BtnHelp) == help);
        const size_t ai = __alignof(CMyImage);
        const size_t good = RoundUp(help + btn, ai);
        Assert::IsTrue(offsetof(CDlgEnchant, m_EnchantImaGood) == good);
        Assert::IsTrue(offsetof(CDlgEnchant, m_EnchantImaGem) == good + RoundUp(sizeof(CMyImage), ai));
        const size_t ok = RoundUp(good + 2 * RoundUp(sizeof(CMyImage), ai), ab);
        Assert::IsTrue(offsetof(CDlgEnchant, m_EnchantBtnOk) == ok);
        Assert::IsTrue(offsetof(CDlgEnchant, m_EnchantBtnClose) == ok + btn);
        Assert::IsTrue(offsetof(CDlgEnchant, m_EnchantBtnCloses) == ok + 2 * btn);
        Assert::IsTrue(sizeof(CDlgEnchant) == RoundUp(ok + 3 * btn, __alignof(CDlgEnchant)));
    }
};
