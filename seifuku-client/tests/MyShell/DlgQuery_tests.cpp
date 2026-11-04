#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgQuery>::value, "");
static_assert(std::is_polymorphic<CDlgQuery>::value, "");
static_assert(IDC_QUERY_SKILL == 1028 && IDC_QUERY_CLOSEB == 1029 && IDC_QUERY_CLOSE == 1027 && IDC_QUERY_ROLE == 1504 && IDC_QUERY_WEAPON == 1505 && IDC_QUERY_MISSION == 1506 && IDC_BTN_HELP == 1667 && IDC_QUERY_FACTION == 2001, "");
static_assert(IDD_DIALOG_QUERY_ROLE == 132 && IDD_DIALOG_QUERY_WEAPON == 134 && IDD_DIALOG_QUERY_MISSION == 135 && IDD_DIALOG_QUERY_SKILL == 163, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_QUERY_ROLE == 7 && DLG_QUERY_WEAPON == 11 && DLG_QUERY_MISSION == 9 && DLG_QUERY_SKILL == 10, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgQueryProbe
{
    static size_t Brush() { return offsetof(CDlgQuery, m_Hbr); }
};

static_assert(CDlgQuery::IDD == 131, "");

TEST_CLASS(ClientDlgQueryHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgQuery, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgQueryProbe::Brush() == b + 0x04);
        Assert::IsTrue(offsetof(CDlgQuery, m_bLoad) == b + 0x08);
        Assert::IsTrue(offsetof(CDlgQuery, m_nPntX) == b + 0x0C);
        Assert::IsTrue(offsetof(CDlgQuery, m_Pnt) == b + 0x10);
        Assert::IsTrue(offsetof(CDlgQuery, m_Height) == b + 0x18);
        Assert::IsTrue(offsetof(CDlgQuery, m_Width) == b + 0x1C);
        const size_t ar = __alignof(CDlgQuery_Role);
        size_t o = RoundUp(b + 0x20, ar);
        Assert::IsTrue(offsetof(CDlgQuery, m_DlgRole) == o);
        o = RoundUp(o + sizeof(CDlgQuery_Role), __alignof(CDlgQuery_Weapon));
        Assert::IsTrue(offsetof(CDlgQuery, m_DlgWeapon) == o);
        o = RoundUp(o + sizeof(CDlgQuery_Weapon), __alignof(CDlgQuery_Mission));
        Assert::IsTrue(offsetof(CDlgQuery, m_DlgMission) == o);
        o = RoundUp(o + sizeof(CDlgQuery_Mission), __alignof(CDlgQuery_Skill));
        Assert::IsTrue(offsetof(CDlgQuery, m_DlgSkill) == o);
        o += sizeof(CDlgQuery_Skill);
        Assert::IsTrue(offsetof(CDlgQuery, m_btCurChildDlgID) == o);
        o = RoundUp(o + 1, 4);
        Assert::IsTrue(offsetof(CDlgQuery, m_dwLastShowTime) == o);   o += 4;
        Assert::IsTrue(offsetof(CDlgQuery, m_fFrameStep) == o);       o += 4;
        Assert::IsTrue(offsetof(CDlgQuery, m_nFrameMode) == o);       o += 4;
        const size_t ac = __alignof(CMyCheck);
        const size_t chk = RoundUp(sizeof(CMyCheck), ac);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        o = RoundUp(o, ac);
        Assert::IsTrue(offsetof(CDlgQuery, m_ChkSkill) == o);         o = RoundUp(o + chk, ab);
        Assert::IsTrue(offsetof(CDlgQuery, m_QueryCloseBBtn) == o);   o += btn;
        Assert::IsTrue(offsetof(CDlgQuery, m_QueryCloseBtn) == o);    o = RoundUp(o + btn, ac);
        Assert::IsTrue(offsetof(CDlgQuery, m_ChkRole) == o);          o += chk;
        Assert::IsTrue(offsetof(CDlgQuery, m_ChkWeapon) == o);        o += chk;
        Assert::IsTrue(offsetof(CDlgQuery, m_ChkMission) == o);       o = RoundUp(o + chk, ab);
        Assert::IsTrue(offsetof(CDlgQuery, m_BtnHelp) == o);          o += btn;
        Assert::IsTrue(sizeof(CDlgQuery) == RoundUp(o, __alignof(CDlgQuery)));
    }
};
