#include "stdafx.h"
#include "MyShell.h"
#include "DlgRoleChoose.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgRoleChoose>::value, "");
static_assert(std::is_polymorphic<CDlgRoleChoose>::value, "");
static_assert(CDlgRoleChoose::IDD == 150, "");
static_assert(ON_ROLECHOOSE_CHOOSE == 5 && ON_EXIT == 0, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_ROLECHOOSE == 1, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgRoleChooseProbe
{
    static size_t Brush() { return offsetof(CDlgRoleChoose, m_Hbr); }
    static size_t Load() { return offsetof(CDlgRoleChoose, m_bLoad); }
    static size_t RoleType() { return offsetof(CDlgRoleChoose, m_btRoleType); }
    static size_t Role() { return offsetof(CDlgRoleChoose, m_ShowRole); }
    static size_t B1024() { return offsetof(CDlgRoleChoose, m_b1024); }
};

TEST_CLASS(ClientDlgRoleChooseHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgRoleChoose, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgRoleChooseProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgRoleChooseProbe::Load() == b + 0x08);
        Assert::IsTrue(DlgRoleChooseProbe::RoleType() == b + 0x0C);
        const size_t role = RoundUp(b + 0x0D, __alignof(C3DSimpleRole));
        Assert::IsTrue(DlgRoleChooseProbe::Role() == role);
        const size_t b1024 = RoundUp(role + sizeof(C3DSimpleRole), __alignof(BOOL));
        Assert::IsTrue(DlgRoleChooseProbe::B1024() == b1024);
        const size_t ai = __alignof(CMyImage);
        const size_t img1 = RoundUp(b1024 + sizeof(BOOL), ai);
        Assert::IsTrue(offsetof(CDlgRoleChoose, m_RoleChooseMImg1) == img1);
        Assert::IsTrue(offsetof(CDlgRoleChoose, m_RoleChooseWImg1) == img1 + 4 * RoundUp(sizeof(CMyImage), ai));
        Assert::IsTrue(offsetof(CDlgRoleChoose, m_RoleChooseWImg4) == img1 + 7 * RoundUp(sizeof(CMyImage), ai));
        const size_t sta = RoundUp(img1 + 8 * RoundUp(sizeof(CMyImage), ai), __alignof(CMyStatic));
        Assert::IsTrue(offsetof(CDlgRoleChoose, m_RoleChooseDataSta) == sta);
        const size_t ab = __alignof(CMyButton);
        const size_t ret = RoundUp(sta + sizeof(CMyStatic), ab);
        Assert::IsTrue(offsetof(CDlgRoleChoose, m_RoleChooseReturnBtn) == ret);
        Assert::IsTrue(offsetof(CDlgRoleChoose, m_RoleChooseOkBtn) == ret + 1 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgRoleChoose, m_HelpBtn) == ret + 2 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(sizeof(CDlgRoleChoose) == RoundUp(offsetof(CDlgRoleChoose, m_HelpBtn) + sizeof(CMyButton), __alignof(CDlgRoleChoose)));
    }
};
