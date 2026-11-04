#include "stdafx.h"
#include "MyShell.h"
#include "DlgOther.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgOther>::value, "");
static_assert(std::is_polymorphic<CDlgOther>::value, "");
static_assert(DLG_OTHER == 35, "");
static_assert(ON_HELPDLG_SHOW == 53 && ON_OTHER_DELNPC == 83, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgOtherProbe
{
    static int Idd() { return CDlgOther::IDD; }
    static size_t Show() { return offsetof(CDlgOther, m_bShow); }
    static size_t Brush() { return offsetof(CDlgOther, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgOther, m_Pnt); }
    static size_t Lay() { return offsetof(CDlgOther, m_OtherBtnLay); }
    static size_t Del() { return offsetof(CDlgOther, m_OtherBtnDel); }
    static size_t CloseB() { return offsetof(CDlgOther, m_BtnCloseB); }
    static size_t Close() { return offsetof(CDlgOther, m_BtnClose); }
    static size_t Help() { return offsetof(CDlgOther, m_BtnHelp); }
};

TEST_CLASS(ClientDlgOtherHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        Assert::IsTrue(DlgOtherProbe::Idd() == 255);
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgOtherProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgOtherProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgOtherProbe::Pnt() == b + 0x08);
        const size_t a = __alignof(CMyButton);
        const size_t lay = RoundUp(b + 0x10, a);
        Assert::IsTrue(DlgOtherProbe::Lay() == lay);
        Assert::IsTrue(DlgOtherProbe::Del() == RoundUp(lay + sizeof(CMyButton), a));
        Assert::IsTrue(DlgOtherProbe::CloseB() == RoundUp(DlgOtherProbe::Del() + sizeof(CMyButton), a));
        Assert::IsTrue(DlgOtherProbe::Close() == RoundUp(DlgOtherProbe::CloseB() + sizeof(CMyButton), a));
        Assert::IsTrue(DlgOtherProbe::Help() == RoundUp(DlgOtherProbe::Close() + sizeof(CMyButton), a));
        Assert::IsTrue(sizeof(CDlgOther) == RoundUp(DlgOtherProbe::Help() + sizeof(CMyButton), __alignof(CDlgOther)));
    }
};
