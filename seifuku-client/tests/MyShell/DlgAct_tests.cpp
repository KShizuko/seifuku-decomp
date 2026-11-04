#include "stdafx.h"
#include "MyShell.h"
#include "DlgAct.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgAct>::value, "");
static_assert(std::is_polymorphic<CDlgAct>::value, "");
static_assert(DLG_ACT == 20, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgActProbe
{
    static int Idd() { return CDlgAct::IDD; }
    static size_t Brush() { return offsetof(CDlgAct, m_Hbr); }
    static size_t Load() { return offsetof(CDlgAct, m_bLoad); }
    static size_t Pnt() { return offsetof(CDlgAct, m_Pnt); }
    static size_t Btn1() { return offsetof(CDlgAct, m_ActBtn1); }
    static size_t Btn6() { return offsetof(CDlgAct, m_ActBtn6); }
    static size_t Btn10() { return offsetof(CDlgAct, m_ActBtn10); }
    static size_t Close() { return offsetof(CDlgAct, m_CloseBtn); }
};

TEST_CLASS(ClientDlgActHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgActProbe::Idd() == 140);
        Assert::IsTrue(offsetof(CDlgAct, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgActProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgActProbe::Load() == b + 0x08);
        Assert::IsTrue(DlgActProbe::Pnt() == b + 0x0C);
        const size_t ab = __alignof(CMyButton);
        const size_t btn1 = RoundUp(b + 0x14, ab);
        Assert::IsTrue(DlgActProbe::Btn1() == btn1);
        Assert::IsTrue(DlgActProbe::Btn6() == btn1 + 5 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(DlgActProbe::Btn10() == btn1 + 9 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(DlgActProbe::Close() == btn1 + 10 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(sizeof(CDlgAct) == RoundUp(DlgActProbe::Close() + sizeof(CMyButton), __alignof(CDlgAct)));
    }
};
