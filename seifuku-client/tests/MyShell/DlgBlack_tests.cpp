#include "stdafx.h"
#include "MyShell.h"
#include "DlgBlack.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgBlack>::value, "");
static_assert(std::is_polymorphic<CDlgBlack>::value, "");
static_assert(CDlgBlack::IDD == 239, "");
static_assert(DLG_BLACK == 31 && ON_BLACK_CLOSE == 98, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgBlackProbe
{
    static size_t Show() { return offsetof(CDlgBlack, m_bShow); }
    static size_t Brush() { return offsetof(CDlgBlack, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgBlack, m_Pnt); }
};

TEST_CLASS(ClientDlgBlackHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgBlackProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgBlackProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgBlackProbe::Pnt() == b + 0x08);
        const size_t edit = RoundUp(b + 0x10, __alignof(CMyEdit));
        Assert::IsTrue(offsetof(CDlgBlack, m_BlackEdtName) == edit);
        const size_t help = RoundUp(edit + sizeof(CMyEdit), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgBlack, m_BtnHelp) == help);
        const size_t list = RoundUp(help + sizeof(CMyButton), __alignof(CMyList));
        Assert::IsTrue(offsetof(CDlgBlack, m_BlackLstData) == list);
        const size_t a = __alignof(CMyButton);
        const size_t closeb = RoundUp(list + sizeof(CMyList), a);
        Assert::IsTrue(offsetof(CDlgBlack, m_BlackCloseb) == closeb);
        Assert::IsTrue(offsetof(CDlgBlack, m_BlackBtnCloses) == closeb + 3 * RoundUp(sizeof(CMyButton), a));
        Assert::IsTrue(sizeof(CDlgBlack) == RoundUp(offsetof(CDlgBlack, m_BlackBtnCloses) + sizeof(CMyButton), __alignof(CDlgBlack)));
    }
};
