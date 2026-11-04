#include "stdafx.h"
#include "MyShell.h"
#include "DlgTalkAct.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgTalkAct>::value, "");
static_assert(std::is_polymorphic<CDlgTalkAct>::value, "");
static_assert(ACTION_CHOOSE == 5006, "");
static_assert(DLG_TALKACT == 40, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgTalkActProbe
{
    static int Idd() { return CDlgTalkAct::IDD; }
    static size_t Show() { return offsetof(CDlgTalkAct, m_bShow); }
    static size_t Brush() { return offsetof(CDlgTalkAct, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgTalkAct, m_Pnt); }
    static size_t Space() { return offsetof(CDlgTalkAct, m_nSpace); }
    static size_t Index() { return offsetof(CDlgTalkAct, m_nCurIndex); }
    static size_t Amount() { return offsetof(CDlgTalkAct, m_nCurAmount); }
    static size_t Parent() { return offsetof(CDlgTalkAct, m_pParentCWnd); }
    static size_t PageUp() { return offsetof(CDlgTalkAct, m_TalkActBtnPageUp); }
    static size_t CloseB() { return offsetof(CDlgTalkAct, m_TalkActBtnCloseB); }
};

TEST_CLASS(ClientDlgTalkActHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        Assert::IsTrue(DlgTalkActProbe::Idd() == 274);
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgTalkActProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgTalkActProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgTalkActProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgTalkActProbe::Space() == b + 0x10);
        Assert::IsTrue(DlgTalkActProbe::Index() == b + 0x14);
        Assert::IsTrue(DlgTalkActProbe::Amount() == b + 0x18);
        Assert::IsTrue(DlgTalkActProbe::Parent() == b + 0x1C);
        const size_t a = __alignof(CMyButton);
        const size_t up = RoundUp(b + 0x20, a);
        Assert::IsTrue(DlgTalkActProbe::PageUp() == up);
        Assert::IsTrue(DlgTalkActProbe::CloseB() == up + 3 * RoundUp(sizeof(CMyButton), a));
        Assert::IsTrue(sizeof(CDlgTalkAct) == RoundUp(DlgTalkActProbe::CloseB() + sizeof(CMyButton), __alignof(CDlgTalkAct)));
    }
};
