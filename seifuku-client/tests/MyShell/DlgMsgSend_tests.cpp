#include "stdafx.h"
#include "MyShell.h"
#include "DlgMsgSend.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgMsgSend>::value, "");
static_assert(std::is_polymorphic<CDlgMsgSend>::value, "");
static_assert(CDlgMsgSend::IDD == 232, "");
static_assert(DLG_MSGSEND == 26 && ON_MSGSEND_VIEW == 68, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgMsgSendProbe
{
    static size_t Show() { return offsetof(CDlgMsgSend, m_bShow); }
    static size_t Brush() { return offsetof(CDlgMsgSend, m_Hbr); }
    static size_t Black() { return offsetof(CDlgMsgSend, m_HbrBlack); }
    static size_t Pnt() { return offsetof(CDlgMsgSend, m_Pnt); }
    static size_t Friend() { return offsetof(CDlgMsgSend, m_FriendID); }
};

TEST_CLASS(ClientDlgMsgSendHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgMsgSendProbe::Show() == b + 0x00);
        Assert::IsTrue(DlgMsgSendProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgMsgSendProbe::Black() == b + 0x08);
        Assert::IsTrue(DlgMsgSendProbe::Pnt() == b + 0x0C);
        Assert::IsTrue(DlgMsgSendProbe::Friend() == b + 0x14);
        const size_t edit = RoundUp(b + 0x18, __alignof(CMyEdit));
        Assert::IsTrue(offsetof(CDlgMsgSend, m_MsgSendData) == edit);
        const size_t send = RoundUp(edit + sizeof(CMyEdit), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgMsgSend, m_MsgSendSend) == send);
        const size_t name = RoundUp(send + sizeof(CMyButton), __alignof(CMyStatic));
        Assert::IsTrue(offsetof(CDlgMsgSend, m_MsgSendName) == name);
        const size_t a = __alignof(CMyButton);
        const size_t close = RoundUp(name + sizeof(CMyStatic), a);
        Assert::IsTrue(offsetof(CDlgMsgSend, m_MsgSendClose) == close);
        Assert::IsTrue(offsetof(CDlgMsgSend, m_BtnHelp) == close + 3 * RoundUp(sizeof(CMyButton), a));
        Assert::IsTrue(sizeof(CDlgMsgSend) == RoundUp(offsetof(CDlgMsgSend, m_BtnHelp) + sizeof(CMyButton), __alignof(CDlgMsgSend)));
    }
};
