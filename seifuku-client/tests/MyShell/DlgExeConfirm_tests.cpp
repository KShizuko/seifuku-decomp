#include "stdafx.h"
#include "MyShell.h"
#include "DlgExeConfirm.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgExeConfirm>::value, "");
static_assert(std::is_polymorphic<CDlgExeConfirm>::value, "");
static_assert(CDlgExeConfirm::IDD == 285, "");
static_assert(IDC_EXECONFIRM_BTN_OK == 0x681 && IDC_EXECONFIRM_BTN_HELP == 0x5B8 && IDC_EXECONFIRM_BTN_CANCEL == 0x680, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_EXECONFIRM == 47, "");
static_assert(CMD_EXERCISE_CONFIRM == 1092, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgExeConfirmProbe
{
    static size_t Brush() { return offsetof(CDlgExeConfirm, m_Hbr); }
    static size_t InitSuc() { return offsetof(CDlgExeConfirm, m_bInitSuc); }
    static size_t InitSucSize() { return sizeof(CDlgExeConfirm::m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgExeConfirm, m_Pnt); }
    static size_t Time() { return offsetof(CDlgExeConfirm, m_nTime); }
};

TEST_CLASS(ClientDlgExeConfirmHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgExeConfirmProbe::Brush() == b + 0x00);
        Assert::IsTrue(DlgExeConfirmProbe::InitSuc() == b + 0x04);
        Assert::IsTrue(DlgExeConfirmProbe::InitSucSize() == 1);
        Assert::IsTrue(DlgExeConfirmProbe::Pnt() == b + 0x08);
        Assert::IsTrue(offsetof(CDlgExeConfirm, m_bShow) == b + 0x10);
        Assert::IsTrue(DlgExeConfirmProbe::Time() == b + 0x14);
        const size_t ab = __alignof(CMyButton);
        const size_t ok = RoundUp(b + 0x18, ab);
        Assert::IsTrue(offsetof(CDlgExeConfirm, m_BtnOk) == ok);
        Assert::IsTrue(offsetof(CDlgExeConfirm, m_BtnHelp) == ok + sizeof(CMyButton));
        Assert::IsTrue(offsetof(CDlgExeConfirm, m_BtnCancel) == ok + 2 * sizeof(CMyButton));
        Assert::IsTrue(sizeof(CDlgExeConfirm) == RoundUp(ok + 3 * sizeof(CMyButton), __alignof(CDlgExeConfirm)));
    }
};
