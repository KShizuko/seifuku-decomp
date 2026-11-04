#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery_Role.h"
#include "Item.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgQuery_Role>::value, "");
static_assert(std::is_polymorphic<CDlgQuery_Role>::value, "");
static_assert(CDlgQuery_Role::IDD == 132, "");
static_assert(_MAX_EQUIPMENT == 9 && ITEMPOSITION_TREASURE == 7, "");
static_assert(sizeof(GemInfo) == 12 && offsetof(GemInfo, btAtk) == 4 && offsetof(GemInfo, btMagicAtk) == 5, "");
static_assert(sizeof(CMyShellApp::m_GemInfo) == 24 * 12, "");
static_assert(offsetof(ItemAdditionInfo, nAttackMax) == 2 && offsetof(ItemAdditionInfo, nDefense) == 6 && offsetof(ItemAdditionInfo, nMAttack) == 8 && offsetof(ItemAdditionInfo, nMDef) == 10, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgQuery_RoleProbe
{
    static size_t Brush() { return offsetof(CDlgQuery_Role, m_Hbr); }
    static size_t InitSuc() { return offsetof(CDlgQuery_Role, m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgQuery_Role, m_DlgPnt); }
    static size_t ShowType() { return offsetof(CDlgQuery_Role, m_nShowType); }
};

TEST_CLASS(ClientDlgQuery_RoleHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgQuery_RoleProbe::Brush() == b + 0x00);
        Assert::IsTrue(DlgQuery_RoleProbe::InitSuc() == b + 0x04);
        Assert::IsTrue(DlgQuery_RoleProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgQuery_RoleProbe::ShowType() == b + 0x10);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t more = RoundUp(b + 0x14, ab);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_BtnMore) == more);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_BtnBase) == more + btn);
        const size_t as = __alignof(CMyStatic);
        const size_t sta = RoundUp(sizeof(CMyStatic), as);
        const size_t name = RoundUp(more + 2 * btn, as);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaName) == name);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaLev) == name + 1 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaFamor) == name + 2 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaPk) == name + 3 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaFere) == name + 4 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaSyn) == name + 5 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaTitle) == name + 6 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaStrong) == name + 7 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaSmart) == name + 8 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaHa) == name + 9 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaEnergy) == name + 10 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaPotential) == name + 11 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaUnused) == name + 12 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaAtk) == name + 13 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaDef) == name + 14 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaMagicAtk) == name + 15 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_StaMagicDef) == name + 16 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_Sta18) == name + 17 * sta);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_Sta19) == name + 18 * sta);
        const size_t add = RoundUp(name + 19 * sta, ab);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_BtnAddStrong) == add);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_BtnAddSmart) == add + 1 * btn);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_BtnAddHa) == add + 2 * btn);
        Assert::IsTrue(offsetof(CDlgQuery_Role, m_BtnAddEnergy) == add + 3 * btn);
        Assert::IsTrue(sizeof(CDlgQuery_Role) == RoundUp(add + 4 * btn, __alignof(CDlgQuery_Role)));
    }
};
