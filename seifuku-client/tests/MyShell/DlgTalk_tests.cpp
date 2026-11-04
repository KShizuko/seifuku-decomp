#include "stdafx.h"
#include "MyShell.h"
#include "DlgTalk.h"
#include "NetworkDef.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgTalk>::value, "");
static_assert(std::is_polymorphic<CDlgTalk>::value, "");
static_assert(IDC_TALK_COB_DATA == 1542 && IDC_TALK_EDT_NAME == 1544 && IDC_TALK_IMG_MODE == 1094 && IDC_TALK_BTN_SEND == 1543, "");
static_assert(IDC_TALK_BTN_ACTION == 1096 && IDC_TALK_BTN_CLEAR == 1092 && IDC_TALK_BTN_ADD == 1545 && IDC_TALK_BTN_LIST == 1185 && IDC_TALK_BTN_LOG == 1095, "");
static_assert(IDD_MENU_TALK == 174 && IDD_DIALOG_LISTBOX == 148 && IDD_DIALOG_TALKACT == 274, "");
static_assert(MENU_TALK_CLICK == 5000 && G_CLICK == 5001 && S_CLICK == 5002 && D_CLICK == 5003 && B_CLICK == 5004 && H_CLICK == 5005 && GM_CLICK == 5006, "");
static_assert(ACTION_CHOOSE == 5006, "");
static_assert(ON_TALK_CLOSE == 7 && ON_TALK_DEL == 31 && ON_TALK_CHAT == 33 && ON_TALKACT_SHOW == 102, "");
static_assert(_TXTATR_NORMAL == 2000 && _TXTATR_PRIVATE == 2001 && _TXTATR_TEAM == 2003 && _TXTATR_SYNDICATE == 2004 && _TXTATR_FRIEND == 2009 && _TXTATR_SERVE == 2014, "");
static_assert(_MAX_WORDS_PS == 2, "");
static_assert(CONTROL_IMAGE == 10, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgTalkProbe
{
    static constexpr int Idd() { return CDlgTalk::IDD; }
    static size_t Brush() { return offsetof(CDlgTalk, m_Hbr); }
    static size_t BrushEdit() { return offsetof(CDlgTalk, m_HbrEdit); }
    static size_t InitSuc() { return offsetof(CDlgTalk, m_bInitSuc); }
    static size_t Bitmap() { return offsetof(CDlgTalk, m_Bitmap); }
    static size_t Rect() { return offsetof(CDlgTalk, m_rect); }
    static size_t Pnt() { return offsetof(CDlgTalk, m_Pnt); }
    static size_t ListBox() { return offsetof(CDlgTalk, m_ListBox); }
    static size_t LastTime() { return offsetof(CDlgTalk, m_dwLastTime); }
    static size_t ListAmount() { return offsetof(CDlgTalk, m_nListAmount); }
    static size_t ListIndex() { return offsetof(CDlgTalk, m_nListIndex); }
    static size_t TalkPlayerID() { return offsetof(CDlgTalk, m_TalkPlayerID); }
    static size_t AddBtnMode() { return offsetof(CDlgTalk, m_uAddBtnMode); }
    static size_t DelIndex() { return offsetof(CDlgTalk, m_uDelIndex); }
    static size_t LastGmTime() { return offsetof(CDlgTalk, m_dwLastGmTime); }
};

static_assert(DlgTalkProbe::Idd() == 145, "");

TEST_CLASS(ClientDlgTalkHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgTalk, m_bShow) == b + 0x00);
        Assert::IsTrue(offsetof(CDlgTalk, m_uTalkMode) == b + 0x04);
        const size_t al = __alignof(CDlgListBox);
        const size_t at = __alignof(CDlgTalkAct);
        size_t o = RoundUp(b + 0x08, al);
        Assert::IsTrue(offsetof(CDlgTalk, m_DlgListBox) == o);
        o = RoundUp(o + sizeof(CDlgListBox), at);
        Assert::IsTrue(offsetof(CDlgTalk, m_DlgTalkAct) == o);
        o += sizeof(CDlgTalkAct);
        Assert::IsTrue(offsetof(CDlgTalk, m_strTargetName) == o);
        Assert::IsTrue(sizeof(CDlgTalk::m_strTargetName) == 5 * 16);
        o = RoundUp(o + 5 * 16, 4);
        Assert::IsTrue(offsetof(CDlgTalk, m_uTargetTalkMode) == o);
        Assert::IsTrue(sizeof(CDlgTalk::m_uTargetTalkMode) == 5 * sizeof(UINT));
        o += 5 * sizeof(UINT);
        Assert::IsTrue(offsetof(CDlgTalk, m_nTargetAmount) == o);       o += 4;
        Assert::IsTrue(offsetof(CDlgTalk, m_nCurTargetIndex) == o);     o += 4;
        Assert::IsTrue(DlgTalkProbe::Brush() == o);                     o += 4;
        Assert::IsTrue(DlgTalkProbe::BrushEdit() == o);                 o += 4;
        Assert::IsTrue(DlgTalkProbe::InitSuc() == o);                   o += 4;
        Assert::IsTrue(DlgTalkProbe::Bitmap() == o);                    o += sizeof(CBitmap);
        Assert::IsTrue(sizeof(CBitmap) == 8);
        Assert::IsTrue(DlgTalkProbe::Pnt() == o);                       o += 8;
        const size_t ab = __alignof(CListBox);
        o = RoundUp(o, ab);
        Assert::IsTrue(DlgTalkProbe::ListBox() == o);
        o = RoundUp(o + sizeof(CListBox), 4);
        Assert::IsTrue(DlgTalkProbe::LastTime() == o);                  o += 4;
        Assert::IsTrue(DlgTalkProbe::ListAmount() == o);                o += 4;
        Assert::IsTrue(DlgTalkProbe::ListIndex() == o);                 o += 4;
        const size_t am = __alignof(CMenuTalk);
        o = RoundUp(o, am);
        Assert::IsTrue(offsetof(CDlgTalk, m_MenuTalk) == o);
        o = RoundUp(o + sizeof(CMenuTalk), 4);
        Assert::IsTrue(DlgTalkProbe::TalkPlayerID() == o);              o += 4;
        Assert::IsTrue(DlgTalkProbe::AddBtnMode() == o);                o += 4;
        Assert::IsTrue(DlgTalkProbe::DelIndex() == o);                  o += 4;
        Assert::IsTrue(offsetof(CDlgTalk, m_bShowActionBtn) == o);      o += 4;
        Assert::IsTrue(DlgTalkProbe::LastGmTime() == o);                o += 4;
        Assert::IsTrue(DlgTalkProbe::Rect() == o);                      o += sizeof(CRect);
        const size_t ac = __alignof(CMyCombo);
        const size_t ae = __alignof(CMyEdit);
        const size_t ai = __alignof(CMyImage);
        const size_t an = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), an);
        o = RoundUp(o, ac);
        Assert::IsTrue(offsetof(CDlgTalk, m_TalkDataCob) == o);      o = RoundUp(o + sizeof(CMyCombo), ae);
        Assert::IsTrue(offsetof(CDlgTalk, m_TalkNameEdt) == o);      o = RoundUp(o + sizeof(CMyEdit), ai);
        Assert::IsTrue(offsetof(CDlgTalk, m_TalkModeImg) == o);      o = RoundUp(o + sizeof(CMyImage), an);
        Assert::IsTrue(offsetof(CDlgTalk, m_TalkSendBtn) == o);      o += btn;
        Assert::IsTrue(offsetof(CDlgTalk, m_TalkActionBtn) == o);    o += btn;
        Assert::IsTrue(offsetof(CDlgTalk, m_TalkClearBtn) == o);     o += btn;
        Assert::IsTrue(offsetof(CDlgTalk, m_TalkAddBtn) == o);       o += btn;
        Assert::IsTrue(offsetof(CDlgTalk, m_TalkListBtn) == o);      o += btn;
        Assert::IsTrue(offsetof(CDlgTalk, m_TalkLogBtn) == o);       o += btn;
        Assert::IsTrue(sizeof(CDlgTalk) == RoundUp(o, __alignof(CDlgTalk)));
    }
};
