#include "stdafx.h"
#include "MyShell.h"
#include "DlgRoleCreate.h"
#include "BaseFunc.h"
#include "NetworkDef.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgRoleCreate>::value, "");
static_assert(std::is_polymorphic<CDlgRoleCreate>::value, "");
static_assert(CDlgRoleCreate::IDD == 151, "");
static_assert(ON_ROLECREATE_RETURN == 6 && ON_EXIT == 0, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_ROLECREATE == 2, "");
static_assert(_STATUS_REGISTER == 1004, "");
static_assert(_MSG_REGISTER == 1001, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgRoleCreateProbe
{
    static size_t Brush() { return offsetof(CDlgRoleCreate, m_Hbr); }
    static size_t EdtBrush() { return offsetof(CDlgRoleCreate, m_EdtHbr); }
    static size_t InitSuc() { return offsetof(CDlgRoleCreate, m_bInitSuc); }
    static size_t RoleType() { return offsetof(CDlgRoleCreate, m_btRoleType); }
    static size_t StrRoleType() { return offsetof(CDlgRoleCreate, m_strRoleType); }
    static size_t RoleName() { return offsetof(CDlgRoleCreate, m_strRoleName); }
    static size_t Account() { return offsetof(CDlgRoleCreate, m_strAccount); }
    static size_t Password() { return offsetof(CDlgRoleCreate, m_strPassword); }
    static size_t RoleA() { return offsetof(CDlgRoleCreate, m_ShowRoleA); }
    static size_t RoleB() { return offsetof(CDlgRoleCreate, m_ShowRoleB); }
    static size_t B1024() { return offsetof(CDlgRoleCreate, m_b1024); }
    static size_t SelIndex() { return offsetof(CDlgRoleCreate, m_nRoleSelIndex); }
};

TEST_CLASS(ClientDlgRoleCreateHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgRoleCreate, m_bShow) == b + 0x00);
        const size_t intro = RoundUp(b + 0x04, __alignof(CDlgProIntro));
        Assert::IsTrue(offsetof(CDlgRoleCreate, m_DlgProIntro) == intro);
        const size_t brush = RoundUp(intro + sizeof(CDlgProIntro), 4);
        Assert::IsTrue(DlgRoleCreateProbe::Brush() == brush);
        Assert::IsTrue(DlgRoleCreateProbe::EdtBrush() == brush + 0x04);
        Assert::IsTrue(DlgRoleCreateProbe::InitSuc() == brush + 0x08);
        Assert::IsTrue(DlgRoleCreateProbe::RoleType() == brush + 0x0C);
        Assert::IsTrue(DlgRoleCreateProbe::StrRoleType() == brush + 0x0D);
        Assert::IsTrue(DlgRoleCreateProbe::RoleName() == brush + 0x4D);
        Assert::IsTrue(DlgRoleCreateProbe::Account() == brush + 0x5D);
        Assert::IsTrue(DlgRoleCreateProbe::Password() == brush + 0x6D);
        const size_t roleA = RoundUp(brush + 0x7D, __alignof(C3DSimpleRole));
        const size_t role = RoundUp(sizeof(C3DSimpleRole), __alignof(C3DSimpleRole));
        Assert::IsTrue(DlgRoleCreateProbe::RoleA() == roleA);
        Assert::IsTrue(DlgRoleCreateProbe::RoleB() == roleA + role);
        Assert::IsTrue(DlgRoleCreateProbe::B1024() == roleA + 2 * role);
        Assert::IsTrue(DlgRoleCreateProbe::SelIndex() == roleA + 2 * role + 4);
        const size_t img2 = RoundUp(roleA + 2 * role + 8, __alignof(CMyImage));
        Assert::IsTrue(offsetof(CDlgRoleCreate, m_Img2) == img2);
        const size_t img1 = RoundUp(img2 + sizeof(CMyImage), __alignof(CMyImage));
        Assert::IsTrue(offsetof(CDlgRoleCreate, m_Img1) == img1);
        const size_t exitBtn = RoundUp(img1 + sizeof(CMyImage), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgRoleCreate, m_RoleCreateExitBtn) == exitBtn);
        const size_t sta = RoundUp(exitBtn + sizeof(CMyButton), __alignof(CMyStatic));
        Assert::IsTrue(offsetof(CDlgRoleCreate, m_RoleCreateDataSta) == sta);
        const size_t edt = RoundUp(sta + sizeof(CMyStatic), __alignof(CMyEdit));
        Assert::IsTrue(offsetof(CDlgRoleCreate, m_RoleCreateNameEdt) == edt);
        const size_t ok = RoundUp(edt + sizeof(CMyEdit), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgRoleCreate, m_RoleCreateOkBtn) == ok);
        const size_t ret = RoundUp(ok + sizeof(CMyButton), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgRoleCreate, m_RoleCreateReturnBtn) == ret);
        const size_t help = RoundUp(ret + sizeof(CMyButton), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgRoleCreate, m_HelpBtn) == help);
        Assert::IsTrue(sizeof(CDlgRoleCreate) == RoundUp(help + sizeof(CMyButton), __alignof(CDlgRoleCreate)));
    }
};
