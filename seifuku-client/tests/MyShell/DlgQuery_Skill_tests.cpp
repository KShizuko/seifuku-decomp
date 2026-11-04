#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery_Skill.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgQuery_Skill>::value, "");
static_assert(std::is_polymorphic<CDlgQuery_Skill>::value, "");
static_assert(CDlgQuery_Skill::IDD == 163, "");
static_assert(IDC_SKILL_BTN_Z == 1172 && IDC_SKILL_BTN_M == 1173 && IDC_SKILL_BTN_J == 1174, "");
static_assert(IDC_SKILL_IMA1 == 1160 && IDC_SKILL_IMA6 == 1165 && IDC_SKILL_IMA11 == 1166 && IDC_SKILL_IMA66 == 1171, "");
static_assert(IDC_SKILL_BTN_UP == 1058 && IDC_SKILL_BTN_DOWN == 1059, "");
static_assert(SKILL_IMAGE == 3, "");
static_assert(CONTROL_IMAGE == 10, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgQuery_SkillProbe
{
    static size_t Brush() { return offsetof(CDlgQuery_Skill, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgQuery_Skill, m_DlgPnt); }
    static size_t Class() { return offsetof(CDlgQuery_Skill, m_nSkillClass); }
    static size_t Skill() { return offsetof(CDlgQuery_Skill, m_pCurSkill); }
    static size_t ShowInfo() { return offsetof(CDlgQuery_Skill, m_bShowInfo); }
    static size_t Top() { return offsetof(CDlgQuery_Skill, m_nTopIndex); }
};

TEST_CLASS(ClientDlgQuery_SkillHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgQuery_SkillProbe::Brush() == b + 0x00);
        Assert::IsTrue(DlgQuery_SkillProbe::Pnt() == b + 0x04);
        Assert::IsTrue(DlgQuery_SkillProbe::Class() == b + 0x0C);
        Assert::IsTrue(DlgQuery_SkillProbe::Skill() == b + 0x10);
        Assert::IsTrue(DlgQuery_SkillProbe::ShowInfo() == b + 0x14);
        Assert::IsTrue(DlgQuery_SkillProbe::Top() == b + 0x18);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t z = RoundUp(b + 0x1C, ab);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_BtnZ) == z);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_BtnM) == z + 1 * btn);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_BtnJ) == z + 2 * btn);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_BtnUp) == z + 3 * btn);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_BtnDown) == z + 4 * btn);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        const size_t ima1 = RoundUp(z + 5 * btn, ai);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima1) == ima1);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima2) == ima1 + 1 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima3) == ima1 + 2 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima4) == ima1 + 3 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima5) == ima1 + 4 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima6) == ima1 + 5 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima11) == ima1 + 6 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima22) == ima1 + 7 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima33) == ima1 + 8 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima44) == ima1 + 9 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima55) == ima1 + 10 * img);
        Assert::IsTrue(offsetof(CDlgQuery_Skill, m_Ima66) == ima1 + 11 * img);
        Assert::IsTrue(sizeof(CDlgQuery_Skill) == RoundUp(ima1 + 12 * img, __alignof(CDlgQuery_Skill)));
    }
};
