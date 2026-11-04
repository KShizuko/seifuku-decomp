#include "stdafx.h"
#include "MyShell.h"
#include "DlgLevWord.h"
#include "NetworkDef.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgLevWord>::value, "");
static_assert(std::is_polymorphic<CDlgLevWord>::value, "");
static_assert(CDlgLevWord::IDD == 240, "");
static_assert(_TXTATR_MSG_TRADE == 2201 && _TXTATR_MSG_FRIEND == 2202 && _TXTATR_MSG_TEAM == 2203 && _TXTATR_MSG_SYN == 2204 && _TXTATR_MSG_OTHER == 2205, "");
static_assert(ON_LEVWORD_CHAT == 103 && ON_HELPDLG_SHOW == 53 && DLG_LEVWORD == 32, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgLevWordProbe
{
    static size_t Brush() { return offsetof(CDlgLevWord, m_Hbr); }
    static size_t Black() { return offsetof(CDlgLevWord, m_HbrBlack); }
    static size_t Pnt() { return offsetof(CDlgLevWord, m_Pnt); }
    static size_t ShowMode() { return offsetof(CDlgLevWord, m_btShowMode); }
    static size_t CurChannel() { return offsetof(CDlgLevWord, m_usCurChannel); }
    static size_t CurIndex() { return offsetof(CDlgLevWord, m_nCurIndex); }
};

TEST_CLASS(ClientDlgLevWordHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgLevWord, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgLevWordProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgLevWordProbe::Black() == b + 0x08);
        Assert::IsTrue(DlgLevWordProbe::Pnt() == b + 0x0C);
        Assert::IsTrue(DlgLevWordProbe::ShowMode() == b + 0x14);
        Assert::IsTrue(DlgLevWordProbe::CurChannel() == b + 0x16);
        Assert::IsTrue(DlgLevWordProbe::CurIndex() == b + 0x18);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t ok = RoundUp(b + 0x1C, ab);
        Assert::IsTrue(offsetof(CDlgLevWord, m_BtnOk) == ok);
        Assert::IsTrue(offsetof(CDlgLevWord, m_BtnCancel) == ok + btn);
        const size_t edit = RoundUp(ok + 2 * btn, __alignof(CMyEdit));
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordEdtData) == edit);
        const size_t lst = RoundUp(edit + sizeof(CMyEdit), __alignof(CMyList));
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordLstList) == lst);
        const size_t closeb = RoundUp(lst + sizeof(CMyList), ab);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordCloseB) == closeb);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnWatch) == closeb + 1 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnUp) == closeb + 2 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnTrade) == closeb + 3 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnSend) == closeb + 4 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnOther) == closeb + 5 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnGroup) == closeb + 6 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnFriend) == closeb + 7 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnFlash) == closeb + 8 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnFaction) == closeb + 9 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnDown) == closeb + 10 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnDel) == closeb + 11 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_LevWordBtnCloseS) == closeb + 12 * btn);
        Assert::IsTrue(offsetof(CDlgLevWord, m_BtnHelp) == closeb + 13 * btn);
        Assert::IsTrue(sizeof(CDlgLevWord) == RoundUp(closeb + 14 * btn, __alignof(CDlgLevWord)));
    }
};
