#include "stdafx.h"
#include "MyShell.h"
#include "DlgMagic.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgMagic>::value, "");
static_assert(std::is_polymorphic<CDlgMagic>::value, "");
static_assert(MAGIC_ICON_SIZE == 50, "");
static_assert(10 * MAGIC_ICON_SIZE + 18 == 518, "");
static_assert(DLG_MAIN_HEIGHT - 18 == 123, "");
static_assert(ON_MAGIC_CHOOSE == 62, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgMagicProbe
{
    static int Idd() { return CDlgMagic::IDD; }
    static size_t Brush() { return offsetof(CDlgMagic, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgMagic, m_Pnt); }
    static size_t IdType() { return offsetof(CDlgMagic, m_MagicIdType); }
    static size_t IdTypeSize() { return sizeof(((CDlgMagic*)0)->m_MagicIdType); }
    static size_t CurBase() { return offsetof(CDlgMagic, m_nCurBase); }
    static size_t BtnU() { return offsetof(CDlgMagic, m_MagicBtnU); }
    static size_t BtnD() { return offsetof(CDlgMagic, m_MagicBtnD); }
    static size_t Img10() { return offsetof(CDlgMagic, m_MagicImg10); }
    static size_t Img9() { return offsetof(CDlgMagic, m_MagicImg9); }
    static size_t Img5() { return offsetof(CDlgMagic, m_MagicImg5); }
    static size_t Img2() { return offsetof(CDlgMagic, m_MagicImg2); }
    static size_t Img1() { return offsetof(CDlgMagic, m_MagicImg1); }
};

TEST_CLASS(ClientDlgMagicHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgMagicProbe::Idd() == 188);
        Assert::IsTrue(offsetof(CDlgMagic, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgMagicProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgMagicProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgMagicProbe::IdType() == b + 0x10);
        Assert::IsTrue(DlgMagicProbe::IdTypeSize() == 50 * sizeof(DWORD));
        Assert::IsTrue(DlgMagicProbe::CurBase() == b + 0xD8);
        const size_t ab = __alignof(CMyButton);
        const size_t up = RoundUp(b + 0xDC, ab);
        Assert::IsTrue(DlgMagicProbe::BtnU() == up);
        Assert::IsTrue(DlgMagicProbe::BtnD() == up + RoundUp(sizeof(CMyButton), ab));
        const size_t ai = __alignof(CMyImage);
        const size_t img10 = RoundUp(up + 2 * RoundUp(sizeof(CMyButton), ab), ai);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        Assert::IsTrue(DlgMagicProbe::Img10() == img10);
        Assert::IsTrue(DlgMagicProbe::Img9() == img10 + 1 * img);
        Assert::IsTrue(DlgMagicProbe::Img5() == img10 + 5 * img);
        Assert::IsTrue(DlgMagicProbe::Img2() == img10 + 8 * img);
        Assert::IsTrue(DlgMagicProbe::Img1() == img10 + 9 * img);
        Assert::IsTrue(sizeof(CDlgMagic) == RoundUp(img10 + 10 * img, __alignof(CDlgMagic)));
    }
};
