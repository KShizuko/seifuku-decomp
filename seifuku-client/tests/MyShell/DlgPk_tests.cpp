#include "stdafx.h"
#include "MyShell.h"
#include "DlgPk.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgPk>::value, "");
static_assert(std::is_polymorphic<CDlgPk>::value, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgPkProbe
{
    static int Idd() { return CDlgPk::IDD; }
    static size_t Show() { return offsetof(CDlgPk, m_bShow); }
    static size_t Brush() { return offsetof(CDlgPk, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgPk, m_Pnt); }
    static size_t Arre() { return offsetof(CDlgPk, m_PkArre); }
    static size_t Free() { return offsetof(CDlgPk, m_PkFree); }
    static size_t Group() { return offsetof(CDlgPk, m_PkGroup); }
    static size_t Safe() { return offsetof(CDlgPk, m_PkSafe); }
};

TEST_CLASS(ClientDlgPkHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        Assert::IsTrue(DlgPkProbe::Idd() == 191);
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgPkProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgPkProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgPkProbe::Pnt() == b + 0x08);
        const size_t a = __alignof(CMyButton);
        const size_t arre = RoundUp(b + 0x10, a);
        Assert::IsTrue(DlgPkProbe::Arre() == arre);
        Assert::IsTrue(DlgPkProbe::Free() == RoundUp(arre + sizeof(CMyButton), a));
        Assert::IsTrue(DlgPkProbe::Group() == RoundUp(DlgPkProbe::Free() + sizeof(CMyButton), a));
        Assert::IsTrue(DlgPkProbe::Safe() == RoundUp(DlgPkProbe::Group() + sizeof(CMyButton), a));
        Assert::IsTrue(sizeof(CDlgPk) == RoundUp(DlgPkProbe::Safe() + sizeof(CMyButton), __alignof(CDlgPk)));
    }
};
