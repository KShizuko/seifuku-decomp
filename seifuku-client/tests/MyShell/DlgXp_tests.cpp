#include "stdafx.h"
#include "MyShell.h"
#include "DlgXp.h"
#include "Role.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgXp>::value, "");
static_assert(std::is_polymorphic<CDlgXp>::value, "");
static_assert(ON_MSGDLG_HIDE == 9 && ON_XP_CHOOSE == 63 && ON_MPCBOOTH_CLOSE == 70, "");
static_assert(XP_IMAGE == 4, "");
static_assert(USERSTATUS_GHOST == 0x400, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgXpProbe
{
    static constexpr int Idd() { return CDlgXp::IDD; }
    static size_t Brush() { return offsetof(CDlgXp, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgXp, m_Pnt); }
    static size_t IdTypes() { return offsetof(CDlgXp, m_XpIdType); }
    static size_t IdTypesSize() { return sizeof(CDlgXp::m_XpIdType); }
    static size_t CurBase() { return offsetof(CDlgXp, m_nCurBase); }
    static size_t BtnUp() { return offsetof(CDlgXp, m_XpBtnUp); }
    static size_t BtnDown() { return offsetof(CDlgXp, m_XpBtnDown); }
    static size_t Btn() { return offsetof(CDlgXp, m_XpBtn); }
    static size_t Img10() { return offsetof(CDlgXp, m_XpImg10); }
    static size_t Img9() { return offsetof(CDlgXp, m_XpImg9); }
    static size_t Img8() { return offsetof(CDlgXp, m_XpImg8); }
    static size_t Img7() { return offsetof(CDlgXp, m_XpImg7); }
    static size_t Img6() { return offsetof(CDlgXp, m_XpImg6); }
    static size_t Img5() { return offsetof(CDlgXp, m_XpImg5); }
    static size_t Img4() { return offsetof(CDlgXp, m_XpImg4); }
    static size_t Img3() { return offsetof(CDlgXp, m_XpImg3); }
    static size_t Img2() { return offsetof(CDlgXp, m_XpImg2); }
    static size_t Img1() { return offsetof(CDlgXp, m_XpImg1); }
};

static_assert(DlgXpProbe::Idd() == 189, "");

TEST_CLASS(ClientDlgXpHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgXp, m_bShow) == b + 0x00);
        Assert::IsTrue(offsetof(CDlgXp, m_btBoothType) == b + 0x04);
        Assert::IsTrue(offsetof(CDlgXp, m_bDie) == b + 0x08);
        Assert::IsTrue(offsetof(CDlgXp, m_bTransform) == b + 0x0C);
        Assert::IsTrue(offsetof(CDlgXp, m_bFly) == b + 0x10);
        Assert::IsTrue(DlgXpProbe::Brush() == b + 0x14);
        Assert::IsTrue(DlgXpProbe::Pnt() == b + 0x18);
        Assert::IsTrue(DlgXpProbe::IdTypes() == b + 0x20);
        Assert::IsTrue(DlgXpProbe::IdTypesSize() == 50 * sizeof(DWORD));
        Assert::IsTrue(DlgXpProbe::CurBase() == b + 0xE8);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t up = RoundUp(b + 0xEC, ab);
        Assert::IsTrue(DlgXpProbe::BtnUp() == up);
        Assert::IsTrue(DlgXpProbe::BtnDown() == up + btn);
        const size_t plain = RoundUp(up + 2 * btn, __alignof(CButton));
        Assert::IsTrue(DlgXpProbe::Btn() == plain);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        const size_t img10 = RoundUp(plain + sizeof(CButton), ai);
        Assert::IsTrue(DlgXpProbe::Img10() == img10);
        Assert::IsTrue(DlgXpProbe::Img9() == img10 + 1 * img);
        Assert::IsTrue(DlgXpProbe::Img8() == img10 + 2 * img);
        Assert::IsTrue(DlgXpProbe::Img7() == img10 + 3 * img);
        Assert::IsTrue(DlgXpProbe::Img6() == img10 + 4 * img);
        Assert::IsTrue(DlgXpProbe::Img5() == img10 + 5 * img);
        Assert::IsTrue(DlgXpProbe::Img4() == img10 + 6 * img);
        Assert::IsTrue(DlgXpProbe::Img3() == img10 + 7 * img);
        Assert::IsTrue(DlgXpProbe::Img2() == img10 + 8 * img);
        Assert::IsTrue(DlgXpProbe::Img1() == img10 + 9 * img);
        Assert::IsTrue(sizeof(CDlgXp) == RoundUp(img10 + 10 * img, __alignof(CDlgXp)));
    }
};
