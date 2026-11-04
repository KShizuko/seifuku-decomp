#include "stdafx.h"
#include "MyShell.h"
#include "DlgMsgReceive.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgMsgReceive>::value, "");
static_assert(std::is_polymorphic<CDlgMsgReceive>::value, "");
static_assert(ON_MSGREC_REVERT == 69, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_MSGRECEIVE == 27, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgMsgReceiveProbe
{
    static int Idd() { return CDlgMsgReceive::IDD; }
    static size_t Brush() { return offsetof(CDlgMsgReceive, m_Hbr); }
    static size_t Black() { return offsetof(CDlgMsgReceive, m_HbrBlack); }
    static size_t Pnt() { return offsetof(CDlgMsgReceive, m_Pnt); }
    static size_t CurIndex() { return offsetof(CDlgMsgReceive, m_nCurIndex); }
    static size_t Delall() { return offsetof(CDlgMsgReceive, m_MsgReceiveDelall); }
    static size_t Pre() { return offsetof(CDlgMsgReceive, m_MsgReceivePre); }
    static size_t Data() { return offsetof(CDlgMsgReceive, m_MsgReceiveData); }
    static size_t Name() { return offsetof(CDlgMsgReceive, m_MsgReceiveName); }
    static size_t Close() { return offsetof(CDlgMsgReceive, m_MsgReceiveClose); }
    static size_t Closes() { return offsetof(CDlgMsgReceive, m_MsgReceiveCloses); }
    static size_t Next() { return offsetof(CDlgMsgReceive, m_MsgReceiveNext); }
    static size_t Revert() { return offsetof(CDlgMsgReceive, m_MsgReceiveRevert); }
    static size_t Help() { return offsetof(CDlgMsgReceive, m_BtnHelp); }
};

TEST_CLASS(ClientDlgMsgReceiveHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(DlgMsgReceiveProbe::Idd() == 238);
        Assert::IsTrue(offsetof(CDlgMsgReceive, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgMsgReceiveProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgMsgReceiveProbe::Black() == b + 0x08);
        Assert::IsTrue(DlgMsgReceiveProbe::Pnt() == b + 0x0C);
        Assert::IsTrue(DlgMsgReceiveProbe::CurIndex() == b + 0x14);
        const size_t ab = __alignof(CMyButton);
        const size_t delall = RoundUp(b + 0x18, ab);
        Assert::IsTrue(DlgMsgReceiveProbe::Delall() == delall);
        const size_t pre = RoundUp(delall + sizeof(CMyButton), ab);
        Assert::IsTrue(DlgMsgReceiveProbe::Pre() == pre);
        const size_t data = RoundUp(pre + sizeof(CMyButton), __alignof(CMyEdit));
        Assert::IsTrue(DlgMsgReceiveProbe::Data() == data);
        const size_t name = RoundUp(data + sizeof(CMyEdit), __alignof(CMyStatic));
        Assert::IsTrue(DlgMsgReceiveProbe::Name() == name);
        const size_t close = RoundUp(name + sizeof(CMyStatic), ab);
        Assert::IsTrue(DlgMsgReceiveProbe::Close() == close);
        Assert::IsTrue(DlgMsgReceiveProbe::Closes() == close + 1 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(DlgMsgReceiveProbe::Next() == close + 2 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(DlgMsgReceiveProbe::Revert() == close + 3 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(DlgMsgReceiveProbe::Help() == close + 4 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(sizeof(CDlgMsgReceive) == RoundUp(DlgMsgReceiveProbe::Help() + sizeof(CMyButton), __alignof(CDlgMsgReceive)));
    }
};
