#include "stdafx.h"
#include "MyShell.h"
#include "DlgListBox.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgListBox>::value, "");
static_assert(std::is_polymorphic<CDlgListBox>::value, "");
static_assert(CDlgListBox::IDD == 148, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgListBoxProbe
{
    static size_t Show() { return offsetof(CDlgListBox, m_bShow); }
    static size_t Brush() { return offsetof(CDlgListBox, m_Hbr); }
    static size_t Load() { return offsetof(CDlgListBox, m_bLoad); }
    static size_t Pnt() { return offsetof(CDlgListBox, m_Pnt); }
    static size_t List() { return offsetof(CDlgListBox, m_ListBox); }
};

TEST_CLASS(ClientDlgListBoxHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgListBoxProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgListBoxProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgListBoxProbe::Load() == b + 0x08);
        Assert::IsTrue(DlgListBoxProbe::Pnt() == b + 0x0C);
        const size_t list = RoundUp(b + 0x14, __alignof(CMyList));
        Assert::IsTrue(DlgListBoxProbe::List() == list);
        Assert::IsTrue(sizeof(CDlgListBox) == RoundUp(list + sizeof(CMyList), __alignof(CDlgListBox)));
    }
};
