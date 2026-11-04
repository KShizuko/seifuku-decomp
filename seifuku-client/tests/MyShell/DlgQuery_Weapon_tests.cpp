#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery_Weapon.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgQuery_Weapon>::value, "");
static_assert(std::is_polymorphic<CDlgQuery_Weapon>::value, "");
static_assert(CDlgQuery_Weapon::IDD == 134, "");
static_assert(IDC_WEAPON_BTN_S == 1519 && IDC_WEAPON_BTN_D == 1520 && IDC_WEAPON_BTN_O == 1521, "");
static_assert(IDC_WEAPON_IMA1 == 1522 && IDC_WEAPON_IMA6 == 1527 && IDC_WEAPON_IMA11 == 1528 && IDC_WEAPON_IMA66 == 1062, "");
static_assert(IDC_WEAPON_BTN_UP == 1063 && IDC_WEAPON_BTN_DOWN == 1064, "");
static_assert(CONTROL_IMAGE == 10, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgQuery_WeaponProbe
{
    static size_t Brush() { return offsetof(CDlgQuery_Weapon, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgQuery_Weapon, m_DlgPnt); }
    static size_t Class() { return offsetof(CDlgQuery_Weapon, m_nWeaponClass); }
    static size_t Skill() { return offsetof(CDlgQuery_Weapon, m_pCurSkill); }
    static size_t ShowInfo() { return offsetof(CDlgQuery_Weapon, m_bShowInfo); }
    static size_t Top() { return offsetof(CDlgQuery_Weapon, m_nTopIndex); }
};

TEST_CLASS(ClientDlgQuery_WeaponHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgQuery_WeaponProbe::Brush() == b + 0x00);
        Assert::IsTrue(DlgQuery_WeaponProbe::Pnt() == b + 0x04);
        Assert::IsTrue(DlgQuery_WeaponProbe::Class() == b + 0x0C);
        Assert::IsTrue(DlgQuery_WeaponProbe::Skill() == b + 0x10);
        Assert::IsTrue(DlgQuery_WeaponProbe::ShowInfo() == b + 0x14);
        Assert::IsTrue(DlgQuery_WeaponProbe::Top() == b + 0x18);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t up = RoundUp(b + 0x1C, ab);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_BtnUp) == up);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_BtnDown) == up + 1 * btn);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_BtnS) == up + 2 * btn);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_BtnD) == up + 3 * btn);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_BtnO) == up + 4 * btn);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        const size_t ima1 = RoundUp(up + 5 * btn, ai);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima1) == ima1);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima2) == ima1 + 1 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima3) == ima1 + 2 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima4) == ima1 + 3 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima5) == ima1 + 4 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima6) == ima1 + 5 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima11) == ima1 + 6 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima22) == ima1 + 7 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima33) == ima1 + 8 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima44) == ima1 + 9 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima55) == ima1 + 10 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Weapon, m_Ima66) == ima1 + 11 * img);
        Assert::IsTrue(sizeof(CDlgQuery_Weapon) == RoundUp(ima1 + 12 * img, __alignof(CDlgQuery_Weapon)));
    }
};
