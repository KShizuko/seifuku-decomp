#include "stdafx.h"
#include "MyShell.h"
#include "DlgConfirm.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgConfirm>::value, "");
static_assert(std::is_polymorphic<CDlgConfirm>::value, "");
static_assert(CDlgConfirm::IDD == 158, "");
static_assert(IDC_CONFIRM_EDT_DATA == 1666 && IDC_CONFIRM_BTN_OK == 1665 && IDC_CONFIRM_BTN_CANCEL == 1664, "");
static_assert(IDC_CONFIRM_BTN_EXERCISE == 0x5B7 && IDC_CONFIRM_STA_TEXT == 0x5B2 && IDC_CONFIRM_EDT_DATA2 == 0x683 && IDC_CONFIRM_STA_DATA2 == 0x5B1 && IDC_CONFIRM_STA_DATA == 0x5B0, "");
static_assert(TRADE_CONFIRM == 0 && MPCCOURT_CONFIRM == 14 && IMPTITEM_CONFIRM == 15 && DROPITEM_CONFIRM == 16 && RESOURCE_CONFIRM == 21, "");
static_assert(CONFIRM_TYPE_23 == 23 && CONFIRM_TYPE_24 == 24 && CONFIRM_TYPE_27 == 27 && CONFIRM_TYPE_28 == 28, "");
static_assert(ON_TRADE_ACCEPT == 28 && ON_TEAMJOIN_ACCEPT == 38 && ON_TEAMINVITE_ACCEPT == 39 && ON_FACTIONJOIN_ACCEPT == 49 && ON_FACTIONINVITE_ACCEPT == 48 && ON_MONEYDROP_ACCEPT == 50, "");
static_assert(ON_FRIENDDEL_ACCEPT == 40 && ON_TALKDEL_ACCEPT == 41 && ON_FACTIONOUT_ACCEPT == 51 && ON_FACTIONSUB_ACCEPT == 42 && ON_MPCBOOTHADD_ACCEPT == 73 && ON_MSG_76 == 76 && ON_ACTION_INI == 52, "");
static_assert(ON_BOOTHOPEN_ACCEPT == 72 && ON_MPCCOURT_ACCEPT == 82 && ON_IMPTITEM_ACCEPT == 85 && ON_DROPITEM_ACCEPT == 86 && ON_TACKOFF_ACCEPT == 87 && ON_JOINDICE_ACCEPT == 91 && ON_OUTDICE_ACCEPT == 92 && ON_ENEMYDEL_ACCEPT == 95 && ON_RESOURCE_ACCEPT == 101 && ON_MSG_1003 == 1003, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgConfirmProbe
{
    static size_t Brush() { return offsetof(CDlgConfirm, m_Hbr); }
    static size_t InitSuc() { return offsetof(CDlgConfirm, m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgConfirm, m_Pnt); }
    static size_t Text() { return offsetof(CDlgConfirm, m_strText); }
    static size_t TextSize() { return sizeof(CDlgConfirm::m_strText); }
    static size_t Line() { return offsetof(CDlgConfirm, m_nLine); }
    static size_t LineNum() { return offsetof(CDlgConfirm, m_nLineNum); }
    static size_t AniWidth() { return offsetof(CDlgConfirm, m_nAniWidth); }
    static size_t AniHeight() { return offsetof(CDlgConfirm, m_nAniHeight); }
    static size_t Width() { return offsetof(CDlgConfirm, m_nWidth); }
    static size_t Height() { return offsetof(CDlgConfirm, m_nHeight); }
    static size_t LeftLeft() { return offsetof(CDlgConfirm, m_nLeftLeft); }
    static size_t Type() { return offsetof(CDlgConfirm, m_uType); }
    static size_t EdtNum() { return offsetof(CDlgConfirm, m_nEdtNum); }
    static size_t EdtNum2() { return offsetof(CDlgConfirm, m_nEdtNum2); }
    static size_t Skip() { return offsetof(CDlgConfirm, m_nSkip); }
    static size_t Skip2() { return offsetof(CDlgConfirm, m_nSkip2); }
    static size_t TargetID() { return offsetof(CDlgConfirm, m_TargetID); }
    static size_t ShowTime() { return offsetof(CDlgConfirm, m_dwShowTime); }
};

TEST_CLASS(ClientDlgConfirmHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgConfirm, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgConfirmProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgConfirmProbe::InitSuc() == b + 0x08);
        Assert::IsTrue(DlgConfirmProbe::Pnt() == b + 0x0C);
        Assert::IsTrue(DlgConfirmProbe::Text() == b + 0x14);
        Assert::IsTrue(DlgConfirmProbe::TextSize() == 128);
        Assert::IsTrue(DlgConfirmProbe::Line() == b + 0x94);
        Assert::IsTrue(DlgConfirmProbe::LineNum() == b + 0x98);
        Assert::IsTrue(DlgConfirmProbe::AniWidth() == b + 0x9C);
        Assert::IsTrue(DlgConfirmProbe::AniHeight() == b + 0xA0);
        Assert::IsTrue(DlgConfirmProbe::Width() == b + 0xA4);
        Assert::IsTrue(DlgConfirmProbe::Height() == b + 0xA8);
        Assert::IsTrue(DlgConfirmProbe::LeftLeft() == b + 0xAC);
        Assert::IsTrue(DlgConfirmProbe::Type() == b + 0xB0);
        Assert::IsTrue(DlgConfirmProbe::EdtNum() == b + 0xB4);
        Assert::IsTrue(DlgConfirmProbe::EdtNum2() == b + 0xB8);
        Assert::IsTrue(DlgConfirmProbe::Skip() == b + 0xBC);
        Assert::IsTrue(DlgConfirmProbe::Skip2() == b + 0xC0);
        Assert::IsTrue(DlgConfirmProbe::TargetID() == b + 0xC4);
        Assert::IsTrue(DlgConfirmProbe::ShowTime() == b + 0xC8);
        const size_t ab = __alignof(CMyButton);
        const size_t as = __alignof(CMyStatic);
        const size_t ae = __alignof(CMyEdit);
        const size_t exercise = RoundUp(b + 0xCC, ab);
        Assert::IsTrue(offsetof(CDlgConfirm, m_ConfirmBtnExercise) == exercise);
        const size_t text = RoundUp(exercise + sizeof(CMyButton), as);
        Assert::IsTrue(offsetof(CDlgConfirm, m_ConfirmStaText) == text);
        const size_t edt2 = RoundUp(text + sizeof(CMyStatic), ae);
        Assert::IsTrue(offsetof(CDlgConfirm, m_ConfirmEdtData2) == edt2);
        const size_t data2 = RoundUp(edt2 + sizeof(CMyEdit), as);
        Assert::IsTrue(offsetof(CDlgConfirm, m_ConfirmStaData2) == data2);
        Assert::IsTrue(offsetof(CDlgConfirm, m_ConfirmStaData) == data2 + sizeof(CMyStatic));
        const size_t edt = RoundUp(data2 + 2 * sizeof(CMyStatic), ae);
        Assert::IsTrue(offsetof(CDlgConfirm, m_ConfirmEdtData) == edt);
        const size_t ok = RoundUp(edt + sizeof(CMyEdit), ab);
        Assert::IsTrue(offsetof(CDlgConfirm, m_ConfirmBtnOk) == ok);
        Assert::IsTrue(offsetof(CDlgConfirm, m_ConfirmBtnCancel) == ok + sizeof(CMyButton));
        Assert::IsTrue(sizeof(CDlgConfirm) == RoundUp(ok + 2 * sizeof(CMyButton), __alignof(CDlgConfirm)));
    }
};
