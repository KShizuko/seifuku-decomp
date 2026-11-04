#include "stdafx.h"
#include "MyShell.h"
#include "DlgNpcBooth.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgNpcBooth>::value, "");
static_assert(std::is_polymorphic<CDlgNpcBooth>::value, "");
static_assert(IDC_NPCBOOTH_BTN_CLOSE == 1605 && IDC_NPCBOOTH_BTN_CLOSEB == 1606 && IDC_BTN_HELP == 1667, "");
static_assert(IDC_NPCBOOTH_BTN_UP == 1603 && IDC_NPCBOOTH_BTN_DOWN == 1604 && IDC_NPCBOOTH_BTN_BUY1 == 1597 && IDC_NPCBOOTH_BTN_BUY6 == 1602, "");
static_assert(IDC_NPCBOOTH_IMA_GOOD1 == 1607 && IDC_NPCBOOTH_IMA_GOOD6 == 1612 && IDC_STA_PRICE11 == 1322 && IDC_STA_PRICE22 == 1317 && IDC_STA_PRICE66 == 1321, "");
static_assert(ON_NPCBOOTH_CLOSE == 79 && ON_HELPDLG_SHOW == 53 && DLG_NPCBOOTH == 29, "");
static_assert(BOOTHITEM_IMAGE == 8, "");
static_assert(IDC_NPCBOOTH_STA_MONEYTYPE1 == 1457 && IDC_NPCBOOTH_STA_MONEYTYPE6 == 1462, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgNpcBoothProbe
{
    static constexpr int Idd() { return CDlgNpcBooth::IDD; }
    static size_t Brush() { return offsetof(CDlgNpcBooth, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgNpcBooth, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgNpcBooth, m_nPntX); }
    static size_t CurBase() { return offsetof(CDlgNpcBooth, m_nCurBase); }
    static size_t LastShowTime() { return offsetof(CDlgNpcBooth, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgNpcBooth, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgNpcBooth, m_nFrameMode); }
    static size_t MoneyType6() { return offsetof(CDlgNpcBooth, m_StaMoneyType6); }
    static size_t MoneyType1() { return offsetof(CDlgNpcBooth, m_StaMoneyType1); }
    static size_t Price1() { return offsetof(CDlgNpcBooth, m_StaPrice1); }
    static size_t Price6() { return offsetof(CDlgNpcBooth, m_StaPrice6); }
    static size_t Good1() { return offsetof(CDlgNpcBooth, m_NpcBoothImaGood1); }
    static size_t Good6() { return offsetof(CDlgNpcBooth, m_NpcBoothImaGood6); }
    static size_t CloseB() { return offsetof(CDlgNpcBooth, m_NpcBoothBtnCloseB); }
    static size_t Close() { return offsetof(CDlgNpcBooth, m_NpcBoothBtnClose); }
    static size_t Up() { return offsetof(CDlgNpcBooth, m_NpcBoothBtnUp); }
    static size_t Down() { return offsetof(CDlgNpcBooth, m_NpcBoothBtnDown); }
    static size_t Buy1() { return offsetof(CDlgNpcBooth, m_BtnBuy1); }
    static size_t Buy6() { return offsetof(CDlgNpcBooth, m_BtnBuy6); }
    static size_t Help() { return offsetof(CDlgNpcBooth, m_BtnHelp); }
};

static_assert(DlgNpcBoothProbe::Idd() == 234, "");

TEST_CLASS(ClientDlgNpcBoothHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgNpcBooth, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgNpcBoothProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgNpcBoothProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgNpcBoothProbe::PntX() == b + 0x10);
        Assert::IsTrue(DlgNpcBoothProbe::CurBase() == b + 0x14);
        Assert::IsTrue(DlgNpcBoothProbe::LastShowTime() == b + 0x18);
        Assert::IsTrue(DlgNpcBoothProbe::FrameStep() == b + 0x1C);
        Assert::IsTrue(DlgNpcBoothProbe::FrameMode() == b + 0x20);
        const size_t as = __alignof(CMyStatic);
        const size_t sta = RoundUp(sizeof(CMyStatic), as);
        const size_t m6 = RoundUp(b + 0x24, as);
        Assert::IsTrue(DlgNpcBoothProbe::MoneyType6() == m6);
        Assert::IsTrue(DlgNpcBoothProbe::MoneyType1() == m6 + 5 * sta);
        const size_t p1 = m6 + 6 * sta;
        Assert::IsTrue(DlgNpcBoothProbe::Price1() == p1);
        Assert::IsTrue(DlgNpcBoothProbe::Price6() == p1 + 5 * sta);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        const size_t g1 = RoundUp(p1 + 6 * sta, ai);
        Assert::IsTrue(DlgNpcBoothProbe::Good1() == g1);
        Assert::IsTrue(DlgNpcBoothProbe::Good6() == g1 + 5 * img);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t closeb = RoundUp(g1 + 6 * img, ab);
        Assert::IsTrue(DlgNpcBoothProbe::CloseB() == closeb);
        Assert::IsTrue(DlgNpcBoothProbe::Close() == closeb + 1 * btn);
        Assert::IsTrue(DlgNpcBoothProbe::Up() == closeb + 2 * btn);
        Assert::IsTrue(DlgNpcBoothProbe::Down() == closeb + 3 * btn);
        Assert::IsTrue(DlgNpcBoothProbe::Buy1() == closeb + 4 * btn);
        Assert::IsTrue(DlgNpcBoothProbe::Buy6() == closeb + 9 * btn);
        Assert::IsTrue(DlgNpcBoothProbe::Help() == closeb + 10 * btn);
        Assert::IsTrue(sizeof(CDlgNpcBooth) == RoundUp(closeb + 11 * btn, __alignof(CDlgNpcBooth)));
    }
};
