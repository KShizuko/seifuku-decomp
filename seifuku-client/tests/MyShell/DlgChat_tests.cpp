#include "stdafx.h"
#include "MyShell.h"
#include "DlgChat.h"
#include "NetworkDef.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgChat>::value, "");
static_assert(std::is_polymorphic<CDlgChat>::value, "");
static_assert(CDlgChat::IDD == 139, "");
static_assert(IDC_CHAT_CHK_F == 1359 && IDC_CHAT_IMG_FC == 1360, "");
static_assert(IDC_CHAT_BTN_CLOSES == 1335 && IDC_BTN_HELP == 1667 && IDC_CHAT_CLOSEB == 1334, "");
static_assert(IDC_CHAT_IMG_CC1 == 1349 && IDC_CHAT_IMG_CC8 == 1356, "");
static_assert(_TXTATR_TALK == 2007 && _TXTATR_SYNDICATE == 2004 && _TXTATR_TEAM == 2003 && _TXTATR_FRIEND == 2009 && _TXTATR_ACTION == 2002 && _TXTATR_PRIVATE == 2001 && _TXTATR_SERVE == 2014, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_CHAT == 30 && ON_TALK_LOG == 32 && ON_CHAT_SIFT == 80, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgChatProbe
{
    static size_t Brush() { return offsetof(CDlgChat, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgChat, m_Pnt); }
    static size_t Channel() { return offsetof(CDlgChat, m_usCurChannel); }
    static size_t CurColor() { return offsetof(CDlgChat, m_dwCurColor); }
    static size_t ColorList() { return offsetof(CDlgChat, m_bColorList); }
};

TEST_CLASS(ClientDlgChatHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgChat, m_bShow) == b + 0x00);
        Assert::IsTrue(offsetof(CDlgChat, m_nColorLog) == b + 0x04);
        Assert::IsTrue(sizeof(CDlgChat::m_nColorLog) == 7 * sizeof(int));
        Assert::IsTrue(offsetof(CDlgChat, m_dwColor) == b + 0x20);
        Assert::IsTrue(sizeof(CDlgChat::m_dwColor) == 8 * sizeof(DWORD));
        Assert::IsTrue(DlgChatProbe::Brush() == b + 0x40);
        Assert::IsTrue(DlgChatProbe::Pnt() == b + 0x44);
        Assert::IsTrue(DlgChatProbe::Channel() == b + 0x4C);
        Assert::IsTrue(DlgChatProbe::CurColor() == b + 0x50);
        Assert::IsTrue(DlgChatProbe::ColorList() == b + 0x54);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        const size_t cc1 = RoundUp(b + 0x58, ai);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatImgCc1) == cc1);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatImgCc8) == cc1 + 7 * img);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatImgBc) == cc1 + 8 * img);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatImgDc) == cc1 + 9 * img);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatImgGc) == cc1 + 10 * img);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatImgSc) == cc1 + 11 * img);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatImgHc) == cc1 + 12 * img);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatImgZc) == cc1 + 13 * img);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatImgFc) == cc1 + 14 * img);
        const size_t ac = __alignof(CMyCheck);
        const size_t chk = RoundUp(sizeof(CMyCheck), ac);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t sf = RoundUp(cc1 + 15 * img, ac);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatChkSF) == sf);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatChkS) == sf + chk);
        const size_t closes = RoundUp(sf + 2 * chk, ab);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatBtnCloseS) == closes);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatBtnCloseB) == closes + btn);
        const size_t h = RoundUp(closes + 2 * btn, ac);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatCHkH) == h);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatCHkG) == h + 1 * chk);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatCHkD) == h + 2 * chk);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatChkB) == h + 3 * chk);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatChkZ) == h + 4 * chk);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatChkF) == h + 5 * chk);
        const size_t sift = RoundUp(h + 6 * chk, ab);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatBtnSift) == sift);
        Assert::IsTrue(offsetof(CDlgChat, m_ChatBtnRecord) == sift + btn);
        Assert::IsTrue(offsetof(CDlgChat, m_BtnHelp) == sift + 2 * btn);
        Assert::IsTrue(sizeof(CDlgChat) == RoundUp(sift + 3 * btn, __alignof(CDlgChat)));
    }
};
