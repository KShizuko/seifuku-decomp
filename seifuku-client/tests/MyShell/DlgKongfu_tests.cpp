#include "stdafx.h"
#include "MyShell.h"
#include "DlgKongfu.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgKongfu>::value, "");
static_assert(std::is_polymorphic<CDlgKongfu>::value, "");
static_assert(DLG_MAIN_HEIGHT - 18 + 100 == 223, "");
static_assert(10 * 50 + 18 == 518, "");
static_assert(ON_MAGIC_CHOOSE == 62, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgKongfuProbe
{
    static int Idd() { return CDlgKongfu::IDD; }
    static size_t Brush() { return offsetof(CDlgKongfu, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgKongfu, m_Pnt); }
    static size_t IdType() { return offsetof(CDlgKongfu, m_KongfuIdType); }
    static size_t IdTypeSize() { return sizeof(((CDlgKongfu*)0)->m_KongfuIdType); }
    static size_t CurBase() { return offsetof(CDlgKongfu, m_nCurBase); }
    static size_t BtnU() { return offsetof(CDlgKongfu, m_KongfuBtnU); }
    static size_t BtnD() { return offsetof(CDlgKongfu, m_KongfuBtnD); }
    static size_t Img1() { return offsetof(CDlgKongfu, m_KongfuImg1); }
    static size_t Img2() { return offsetof(CDlgKongfu, m_KongfuImg2); }
    static size_t Img6() { return offsetof(CDlgKongfu, m_KongfuImg6); }
    static size_t Img9() { return offsetof(CDlgKongfu, m_KongfuImg9); }
    static size_t Img10() { return offsetof(CDlgKongfu, m_KongfuImg10); }
};

TEST_CLASS(ClientDlgKongfuHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgKongfuProbe::Idd() == 214);
        Assert::IsTrue(offsetof(CDlgKongfu, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgKongfuProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgKongfuProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgKongfuProbe::IdType() == b + 0x10);
        Assert::IsTrue(DlgKongfuProbe::IdTypeSize() == 50 * sizeof(DWORD));
        Assert::IsTrue(DlgKongfuProbe::CurBase() == b + 0xD8);
        const size_t ab = __alignof(CMyButton);
        const size_t up = RoundUp(b + 0xDC, ab);
        Assert::IsTrue(DlgKongfuProbe::BtnU() == up);
        Assert::IsTrue(DlgKongfuProbe::BtnD() == up + RoundUp(sizeof(CMyButton), ab));
        const size_t ai = __alignof(CMyImage);
        const size_t img1 = RoundUp(up + 2 * RoundUp(sizeof(CMyButton), ab), ai);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        Assert::IsTrue(DlgKongfuProbe::Img1() == img1);
        Assert::IsTrue(DlgKongfuProbe::Img2() == img1 + 1 * img);
        Assert::IsTrue(DlgKongfuProbe::Img6() == img1 + 5 * img);
        Assert::IsTrue(DlgKongfuProbe::Img9() == img1 + 8 * img);
        Assert::IsTrue(DlgKongfuProbe::Img10() == img1 + 9 * img);
        Assert::IsTrue(sizeof(CDlgKongfu) == RoundUp(img1 + 10 * img, __alignof(CDlgKongfu)));
    }
};
