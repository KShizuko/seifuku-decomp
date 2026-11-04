#include "stdafx.h"
#include "MyShell.h"
#include "DlgFriend.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgFriend>::value, "");
static_assert(std::is_polymorphic<CDlgFriend>::value, "");
static_assert(CDlgFriend::IDD == 160, "");
static_assert(ON_FRIEND_ADD == 54 && ON_FRIEND_CHAT == 55 && ON_FRIEND_LEAVE == 56 && ON_FRIEND_RESUME == 58, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_FRIEND == 5, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgFriendProbe
{
    static size_t Brush() { return offsetof(CDlgFriend, m_Hbr); }
    static size_t InitSuc() { return offsetof(CDlgFriend, m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgFriend, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgFriend, m_nPntX); }
    static size_t LastShowTime() { return offsetof(CDlgFriend, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgFriend, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgFriend, m_nFrameMode); }
};

TEST_CLASS(ClientDlgFriendHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgFriend, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgFriendProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgFriendProbe::InitSuc() == b + 0x08);
        Assert::IsTrue(DlgFriendProbe::Pnt() == b + 0x0C);
        Assert::IsTrue(DlgFriendProbe::PntX() == b + 0x14);
        Assert::IsTrue(DlgFriendProbe::LastShowTime() == b + 0x18);
        Assert::IsTrue(DlgFriendProbe::FrameStep() == b + 0x1C);
        Assert::IsTrue(DlgFriendProbe::FrameMode() == b + 0x20);
        const size_t card = RoundUp(b + 0x24, __alignof(CDlgFrdState));
        Assert::IsTrue(offsetof(CDlgFriend, m_DlgFrdState) == card);
        const size_t cur = RoundUp(card + sizeof(CDlgFrdState), __alignof(int));
        Assert::IsTrue(offsetof(CDlgFriend, m_nCurIndex) == cur);
        const size_t ab = __alignof(CMyButton);
        const size_t resume = RoundUp(cur + sizeof(int), ab);
        Assert::IsTrue(offsetof(CDlgFriend, m_FriendBtnResume) == resume);
        Assert::IsTrue(offsetof(CDlgFriend, m_FriendBtnAdd) == resume + 1 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgFriend, m_FriendBtnClose) == resume + 2 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgFriend, m_BtnHelp) == resume + 3 * RoundUp(sizeof(CMyButton), ab));
        const size_t list = RoundUp(offsetof(CDlgFriend, m_BtnHelp) + sizeof(CMyButton), __alignof(CMyList));
        Assert::IsTrue(offsetof(CDlgFriend, m_FriendLstData) == list);
        Assert::IsTrue(sizeof(CDlgFriend) == RoundUp(list + sizeof(CMyList), __alignof(CDlgFriend)));
    }
};
