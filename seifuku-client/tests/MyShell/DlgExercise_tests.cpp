#include "stdafx.h"
#include "MyShell.h"
#include "DlgExercise.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgExercise>::value, "");
static_assert(std::is_polymorphic<CDlgExercise>::value, "");
static_assert(CDlgExercise::IDD == 284, "");
static_assert(IDC_EXERCISE_STA_SETTLE2 == 0x5CB && IDC_EXERCISE_STA_BLESSCOST == 0x5C9 && IDC_EXERCISE_STA_BLESS == 0x5C8 && IDC_EXERCISE_STA_BLESSEMONEY == 0x5CA && IDC_EXERCISE_STA_SETTLE1 == 0x5C3 && IDC_EXERCISE_STA_BALANCE == 0x5BC && IDC_EXERCISE_STA_COST == 0x5C0, "");
static_assert(IDC_EXERCISE_STA_COSTBG == 0x5BF && IDC_EXERCISE_STA_EMONEY == 0x5BD && IDC_EXERCISE_STA_EMONEY2 == 0x5C1 && IDC_EXERCISE_STA_TIME == 0x5BA && IDC_EXERCISE_STA_EXP == 0x5C5 && IDC_EXERCISE_STA_EXPTITLE == 0x5C4 && IDC_EXERCISE_STA_CLOCK == 0x5BB, "");
static_assert(IDC_EXERCISE_BTN_CONTINUE == 0x5C7 && IDC_EXERCISE_BTN_HELP == 0x683 && IDC_EXERCISE_BTN_OK == 0x5B9 && IDC_EXERCISE_BTN_CLOSE == 0x49C && IDC_EXERCISE_BTN_PAY == 0x5C6, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_EXERCISE == 46, "");
static_assert(std::is_same<decltype(&CDlgExercise::SetTime), void (CDlgExercise::*)(int, int, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CDlgExercise::FlashExp), void (CDlgExercise::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CDlgExercise::GetTotalTime), int (CDlgExercise::*)()>::value, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgExerciseProbe
{
    static size_t Brush() { return offsetof(CDlgExercise, m_Hbr); }
    static size_t InitSuc() { return offsetof(CDlgExercise, m_bInitSuc); }
    static size_t InitSucSize() { return sizeof(CDlgExercise::m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgExercise, m_Pnt); }
    static size_t Time() { return offsetof(CDlgExercise, m_nTime); }
    static size_t PayTime() { return offsetof(CDlgExercise, m_nPayTime); }
    static size_t BlessTime() { return offsetof(CDlgExercise, m_nBlessTime); }
};

TEST_CLASS(ClientDlgExerciseHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgExerciseProbe::Brush() == b + 0x00);
        Assert::IsTrue(DlgExerciseProbe::InitSuc() == b + 0x04);
        Assert::IsTrue(DlgExerciseProbe::InitSucSize() == 1);
        Assert::IsTrue(DlgExerciseProbe::Pnt() == b + 0x08);
        Assert::IsTrue(offsetof(CDlgExercise, m_bShow) == b + 0x10);
        Assert::IsTrue(DlgExerciseProbe::Time() == b + 0x14);
        Assert::IsTrue(DlgExerciseProbe::PayTime() == b + 0x18);
        Assert::IsTrue(DlgExerciseProbe::BlessTime() == b + 0x1C);
        const size_t as = __alignof(CMyStatic);
        const size_t ab = __alignof(CMyButton);
        const size_t sta = RoundUp(b + 0x20, as);
        Assert::IsTrue(offsetof(CDlgExercise, m_StaSettle2) == sta);
        Assert::IsTrue(offsetof(CDlgExercise, m_StaSpare) == sta + 4 * sizeof(CMyStatic));
        Assert::IsTrue(offsetof(CDlgExercise, m_StaClock) == sta + 14 * sizeof(CMyStatic));
        const size_t btn = RoundUp(sta + 15 * sizeof(CMyStatic), ab);
        Assert::IsTrue(offsetof(CDlgExercise, m_BtnSpare) == btn);
        Assert::IsTrue(offsetof(CDlgExercise, m_BtnContinue) == btn + sizeof(CMyButton));
        Assert::IsTrue(offsetof(CDlgExercise, m_BtnPay) == btn + 5 * sizeof(CMyButton));
        Assert::IsTrue(sizeof(CDlgExercise) == RoundUp(btn + 6 * sizeof(CMyButton), __alignof(CDlgExercise)));
    }
};
