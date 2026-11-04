#include "stdafx.h"
#include "MyShell.h"
#include "CppUnitTest.h"
#include <cstddef>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(WM_MY_MESSAGE == 0x464, "");
static_assert(ON_EXIT == 0 && ON_EXIT_LOGIN == 1 && ON_MIN == 2 && ON_LOGIN_NEW == 3, "");
static_assert(ON_MSGDLG_GOODS == 8 && ON_MSGDLG_FACE == 13, "");
static_assert(ON_ICON_PICKUP == 14 && ON_ICON_DROP == 15 && ON_ICON_ID == 20 && ON_ICON_PNT == 21, "");
static_assert(ON_HELPDLG_SHOW == 53 && ON_SETUP_MODECHANGE == 59 && ON_MAGIC_CHOOSE == 62 && ON_XP_CHOOSE == 63, "");
static_assert(ON_EMBED_TACKOFF == 88 && ON_ENMSTATE_DEL == 94 && ON_RESOURCE_ACCEPT == 101 && ON_LEVWORD_CHAT == 103 && ON_FUSE_CLOSE == 104, "");
static_assert(DLG_ACT == 20, "");
static_assert(IDD_DIALOG_LOGIN == 129 && IDD_MAIN_DIALOG == 130 && IDD_DIALOG_CHAT == 139 && IDD_DIALOG_DICE == 262 && IDD_MYSHELL_DIALOG == 102, "");
static_assert(sizeof(GemInfo) == 12, "");
static_assert(offsetof(CMyShellApp, m_hCursor) == offsetof(CMyShellApp, m_dwReserved) + 4, "");
static_assert(sizeof(CMyShellApp::m_hCursor) == 14 * sizeof(HCURSOR), "");
static_assert(offsetof(CMyShellApp, m_nOnButton) == offsetof(CMyShellApp, m_dwReserved) + 0x3C, "");

TEST_CLASS(ClientMyShellHeaders)
{
public:
    TEST_METHOD(TheShellConstantsAreTheBinarys)
    {
        Assert::IsTrue(_T("ini/GameSetup.Ini") == CString(g_strSetupIni));
        Assert::AreEqual(1003, (int) ON_MSG_1003);
    }
};
