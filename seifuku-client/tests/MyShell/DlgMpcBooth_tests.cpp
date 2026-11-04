#include "stdafx.h"
#include "MyShell.h"
#include "DlgMpcBooth.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgMpcBooth>::value, "");
static_assert(std::is_polymorphic<CDlgMpcBooth>::value, "");
static_assert(IDC_STA_PRICE1 == 1310 && IDC_STA_PRICE6 == 1315 && IDC_MPCBOOTH_IMA_GOOD1 == 1304 && IDC_MPCBOOTH_IMA_GOOD6 == 1309, "");
static_assert(IDC_MPCBOOTH_BTN_CHANGE1 == 1557 && IDC_MPCBOOTH_BTN_GETBACK1 == 1558 && IDC_MPCBOOTH_BTN_GETBACK6 == 1125, "");
static_assert(IDC_MPCBOOTH_CLOSEB == 1287 && IDC_MPCBOOTH_BTN_UP == 1288 && IDC_MPCBOOTH_BTN_HAWK == 1291 && IDC_MPCBOOTH_BTN_DOWN == 1289 && IDC_MPCBOOTH_BTN_CLOSES == 1286 && IDC_BTN_HELP == 1667, "");
static_assert(ON_MPCBOOTH_CLOSE == 70 && ON_MPCBOOTH_ADDITEM == 71 && ON_MPCBOOTH_HAWK == 78 && ON_HELPDLG_SHOW == 53 && ON_ICON_DROP == 15 && ON_MSG_1003 == 1003, "");
static_assert(DLG_MPCBOOTH == 28 && DLG_ALL == 48 , "");
static_assert(ITEM7_IMAGE == 7, "");
static_assert(IDC_MPCBOOTH_STA_MONEYTYPE1 == 1455 && IDC_MPCBOOTH_STA_MONEYTYPE6 == 1460, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgMpcBoothProbe
{
    static constexpr int Idd() { return CDlgMpcBooth::IDD; }
    static size_t Brush() { return offsetof(CDlgMpcBooth, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgMpcBooth, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgMpcBooth, m_nPntX); }
    static size_t PickUp() { return offsetof(CDlgMpcBooth, m_bPickUp); }
    static size_t IconRealID() { return offsetof(CDlgMpcBooth, m_nIconRealID); }
    static size_t CurBase() { return offsetof(CDlgMpcBooth, m_nCurBase); }
    static size_t LastShowTime() { return offsetof(CDlgMpcBooth, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgMpcBooth, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgMpcBooth, m_nFrameMode); }
    static size_t MoneyType6() { return offsetof(CDlgMpcBooth, m_StaMoneyType6); }
    static size_t MoneyType1() { return offsetof(CDlgMpcBooth, m_StaMoneyType1); }
    static size_t StaPrice1() { return offsetof(CDlgMpcBooth, m_StaPrice1); }
    static size_t StaPrice6() { return offsetof(CDlgMpcBooth, m_StaPrice6); }
    static size_t ImaGood1() { return offsetof(CDlgMpcBooth, m_MpcBoothImaGood1); }
    static size_t ImaGood6() { return offsetof(CDlgMpcBooth, m_MpcBoothImaGood6); }
    static size_t Change1() { return offsetof(CDlgMpcBooth, m_BtnChange1); }
    static size_t Change6() { return offsetof(CDlgMpcBooth, m_BtnChange6); }
    static size_t GetBack1() { return offsetof(CDlgMpcBooth, m_BtnGetBack1); }
    static size_t GetBack6() { return offsetof(CDlgMpcBooth, m_BtnGetBack6); }
    static size_t CloseB() { return offsetof(CDlgMpcBooth, m_MpcBoothBtnCloseB); }
    static size_t Up() { return offsetof(CDlgMpcBooth, m_MpcBoothBtnUp); }
    static size_t Hawk() { return offsetof(CDlgMpcBooth, m_MpcBoothBtnHawk); }
    static size_t Down() { return offsetof(CDlgMpcBooth, m_MpcBoothBtnDown); }
    static size_t CloseS() { return offsetof(CDlgMpcBooth, m_MpcBoothBtnCloseS); }
    static size_t Help() { return offsetof(CDlgMpcBooth, m_BtnHelp); }
};

static_assert(DlgMpcBoothProbe::Idd() == 233, "");

TEST_CLASS(ClientDlgMpcBoothHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgMpcBooth, m_bShow) == b + 0x00);
        Assert::IsTrue(offsetof(CDlgMpcBooth, m_nCurRealID) == b + 0x04);
        Assert::IsTrue(offsetof(CDlgMpcBooth, m_nCurChangeIndex) == b + 0x08);
        Assert::IsTrue(DlgMpcBoothProbe::Brush() == b + 0x0C);
        Assert::IsTrue(DlgMpcBoothProbe::Pnt() == b + 0x10);
        Assert::IsTrue(DlgMpcBoothProbe::PntX() == b + 0x18);
        Assert::IsTrue(DlgMpcBoothProbe::PickUp() == b + 0x1C);
        Assert::IsTrue(DlgMpcBoothProbe::IconRealID() == b + 0x20);
        Assert::IsTrue(DlgMpcBoothProbe::CurBase() == b + 0x24);
        Assert::IsTrue(DlgMpcBoothProbe::LastShowTime() == b + 0x28);
        Assert::IsTrue(DlgMpcBoothProbe::FrameStep() == b + 0x2C);
        Assert::IsTrue(DlgMpcBoothProbe::FrameMode() == b + 0x30);
        const size_t as = __alignof(CMyStatic);
        const size_t sta = RoundUp(sizeof(CMyStatic), as);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        size_t o = RoundUp(b + 0x34, as);
        Assert::IsTrue(DlgMpcBoothProbe::MoneyType6() == o);
        Assert::IsTrue(DlgMpcBoothProbe::MoneyType1() == o + 5 * sta);
        o += 6 * sta;
        Assert::IsTrue(DlgMpcBoothProbe::StaPrice1() == o);
        Assert::IsTrue(DlgMpcBoothProbe::StaPrice6() == o + 5 * sta);
        o = RoundUp(o + 6 * sta, ai);
        Assert::IsTrue(DlgMpcBoothProbe::ImaGood1() == o);
        Assert::IsTrue(DlgMpcBoothProbe::ImaGood6() == o + 5 * img);
        o = RoundUp(o + 6 * img, ab);
        Assert::IsTrue(DlgMpcBoothProbe::Change1() == o);
        Assert::IsTrue(DlgMpcBoothProbe::Change6() == o + 5 * btn);
        o += 6 * btn;
        Assert::IsTrue(DlgMpcBoothProbe::GetBack1() == o);
        Assert::IsTrue(DlgMpcBoothProbe::GetBack6() == o + 5 * btn);
        o += 6 * btn;
        Assert::IsTrue(DlgMpcBoothProbe::CloseB() == o);       o += btn;
        Assert::IsTrue(DlgMpcBoothProbe::Up() == o);           o += btn;
        Assert::IsTrue(DlgMpcBoothProbe::Hawk() == o);         o += btn;
        Assert::IsTrue(DlgMpcBoothProbe::Down() == o);         o += btn;
        Assert::IsTrue(DlgMpcBoothProbe::CloseS() == o);       o += btn;
        Assert::IsTrue(DlgMpcBoothProbe::Help() == o);         o += btn;
        Assert::IsTrue(sizeof(CDlgMpcBooth) == RoundUp(o, __alignof(CDlgMpcBooth)));
    }
};
