#include "stdafx.h"
#include "MyShell.h"
#include "DlgDiceList.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgDiceList>::value, "");
static_assert(std::is_polymorphic<CDlgDiceList>::value, "");
static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgDiceListProbe
{
    static int Idd() { return CDlgDiceList::IDD; }
    static size_t Show() { return offsetof(CDlgDiceList, m_bShow); }
    static size_t Brush() { return offsetof(CDlgDiceList, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgDiceList, m_Pnt); }
    static size_t List() { return offsetof(CDlgDiceList, m_DiceListList); }
};

TEST_CLASS(ClientDlgDiceListHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        Assert::IsTrue(DlgDiceListProbe::Idd() == 267);
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgDiceListProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgDiceListProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgDiceListProbe::Pnt() == b + 0x08);
        const size_t list = RoundUp(b + 0x10, __alignof(CMyList));
        Assert::IsTrue(DlgDiceListProbe::List() == list);
        Assert::IsTrue(sizeof(CDlgDiceList) == RoundUp(list + sizeof(CMyList), __alignof(CDlgDiceList)));
    }
};
