#include "stdafx.h"
#include "MyShell.h"
#include "DlgLog.h"
#include "NetworkDef.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgLog>::value, "");
static_assert(std::is_polymorphic<CDlgLog>::value, "");
static_assert(CDlgLog::IDD == 218, "");
static_assert(_TXTATR_TALK == 2007 && _TXTATR_PRIVATE == 2001 && _TXTATR_SYNDICATE == 2004 && _TXTATR_FRIEND == 2009 && _TXTATR_TEAM == 2003, "");
static_assert(RGB(52, 37, 14) == 0x0E2534, "");
static_assert(RGB(214, 213, 211) == 0xD3D5D6, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

TEST_CLASS(ClientDlgLogHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgLog, m_Hbr) == b + 0x00);
        Assert::IsTrue(offsetof(CDlgLog, m_EdtHbr) == b + 0x04);
        Assert::IsTrue(offsetof(CDlgLog, m_bFirst) == b + 0x08);
        Assert::IsTrue(offsetof(CDlgLog, m_nLastLine) == b + 0x0C);
        Assert::IsTrue(offsetof(CDlgLog, m_nLogChannel) == b + 0x10);
        const size_t ab = __alignof(CButton);
        const size_t btn = RoundUp(sizeof(CButton), ab);
        const size_t team = RoundUp(b + 0x14, ab);
        Assert::IsTrue(offsetof(CDlgLog, m_LogTeam) == team);
        Assert::IsTrue(offsetof(CDlgLog, m_LogS) == team + 1 * btn);
        Assert::IsTrue(offsetof(CDlgLog, m_LogG) == team + 2 * btn);
        Assert::IsTrue(offsetof(CDlgLog, m_LogFrd) == team + 3 * btn);
        Assert::IsTrue(offsetof(CDlgLog, m_LogFac) == team + 4 * btn);
        Assert::IsTrue(offsetof(CDlgLog, m_LogAll) == team + 5 * btn);
        Assert::IsTrue(offsetof(CDlgLog, m_LogClose) == team + 6 * btn);
        Assert::IsTrue(offsetof(CDlgLog, m_LogDelAll) == team + 7 * btn);
        Assert::IsTrue(offsetof(CDlgLog, m_LogDel) == team + 8 * btn);
        Assert::IsTrue(offsetof(CDlgLog, m_LogRecord) == team + 9 * btn);
        const size_t edit = RoundUp(team + 10 * btn, __alignof(CEdit));
        Assert::IsTrue(offsetof(CDlgLog, m_LogData) == edit);
        const size_t combo = RoundUp(edit + sizeof(CEdit), __alignof(CComboBox));
        Assert::IsTrue(offsetof(CDlgLog, m_LogFile) == combo);
        Assert::IsTrue(sizeof(CDlgLog) == RoundUp(combo + sizeof(CComboBox), __alignof(CDlgLog)));
    }
};
