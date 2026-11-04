#include "stdafx.h"
#include "MyShell.h"
#include "DlgEffigy.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgEffigy>::value, "");
static_assert(std::is_polymorphic<CDlgEffigy>::value, "");
static_assert(CDlgEffigy::IDD == 241, "");
static_assert(ON_EFFIGY_CLOSE == 67 && DLG_EFFIGY == 33, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgEffigyProbe
{
    static size_t Brush() { return offsetof(CDlgEffigy, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgEffigy, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgEffigy, m_nPntX); }
    static size_t Play() { return offsetof(CDlgEffigy, m_bPlay); }
    static size_t Dir() { return offsetof(CDlgEffigy, m_nDir); }
    static size_t LastShowTime() { return offsetof(CDlgEffigy, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgEffigy, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgEffigy, m_nFrameMode); }
};

TEST_CLASS(ClientDlgEffigyHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgEffigy, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgEffigyProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgEffigyProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgEffigyProbe::PntX() == b + 0x10);
        Assert::IsTrue(DlgEffigyProbe::Play() == b + 0x14);
        Assert::IsTrue(DlgEffigyProbe::Dir() == b + 0x18);
        Assert::IsTrue(DlgEffigyProbe::LastShowTime() == b + 0x1C);
        Assert::IsTrue(DlgEffigyProbe::FrameStep() == b + 0x20);
        Assert::IsTrue(DlgEffigyProbe::FrameMode() == b + 0x24);
        const size_t ab = __alignof(CMyButton);
        const size_t right = RoundUp(b + 0x28, ab);
        Assert::IsTrue(offsetof(CDlgEffigy, m_EffigyRight) == right);
        Assert::IsTrue(offsetof(CDlgEffigy, m_EffigyPandP) == right + 1 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgEffigy, m_EffigyOk) == right + 2 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgEffigy, m_EffigyLeft) == right + 3 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgEffigy, m_Effigy_CLoseS) == right + 4 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgEffigy, m_Effigy_Close) == right + 5 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgEffigy, m_BtnHelp) == right + 6 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgEffigy, m_ActBtn1) == right + 7 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgEffigy, m_ActBtn8) == right + 14 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(sizeof(CDlgEffigy) == RoundUp(offsetof(CDlgEffigy, m_ActBtn8) + sizeof(CMyButton), __alignof(CDlgEffigy)));
    }
};
