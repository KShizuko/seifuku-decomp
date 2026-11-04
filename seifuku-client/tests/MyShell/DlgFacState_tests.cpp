#include "stdafx.h"
#include "MyShell.h"
#include "DlgFacState.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgFacState>::value, "");
static_assert(std::is_polymorphic<CDlgFacState>::value, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgFacStateProbe
{
    static int Idd() { return CDlgFacState::IDD; }
    static size_t Show() { return offsetof(CDlgFacState, m_bShow); }
    static size_t Brush() { return offsetof(CDlgFacState, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgFacState, m_Pnt); }
    static size_t Proffer() { return offsetof(CDlgFacState, m_StaProffer); }
    static size_t SynJob() { return offsetof(CDlgFacState, m_StaSynJob); }
    static size_t Level() { return offsetof(CDlgFacState, m_StaLevel); }
    static size_t Name() { return offsetof(CDlgFacState, m_StaName); }
    static size_t Close() { return offsetof(CDlgFacState, m_BtnClose); }
};

TEST_CLASS(ClientDlgFacStateHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        Assert::IsTrue(DlgFacStateProbe::Idd() == 257);
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgFacStateProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgFacStateProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgFacStateProbe::Pnt() == b + 0x08);
        const size_t a = __alignof(CMyStatic);
        const size_t proffer = RoundUp(b + 0x10, a);
        Assert::IsTrue(DlgFacStateProbe::Proffer() == proffer);
        Assert::IsTrue(DlgFacStateProbe::SynJob() == RoundUp(proffer + sizeof(CMyStatic), a));
        Assert::IsTrue(DlgFacStateProbe::Level() == RoundUp(DlgFacStateProbe::SynJob() + sizeof(CMyStatic), a));
        Assert::IsTrue(DlgFacStateProbe::Name() == RoundUp(DlgFacStateProbe::Level() + sizeof(CMyStatic), a));
        Assert::IsTrue(DlgFacStateProbe::Close() == RoundUp(DlgFacStateProbe::Name() + sizeof(CMyStatic), __alignof(CMyButton)));
        Assert::IsTrue(sizeof(CDlgFacState) == RoundUp(DlgFacStateProbe::Close() + sizeof(CMyButton), __alignof(CDlgFacState)));
    }
};
