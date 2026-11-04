#include "stdafx.h"
#include "MyShell.h"
#include "MyShellDlg.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CMyShellDlg>::value, "");
static_assert(std::is_polymorphic<CMyShellDlg>::value, "");
static_assert(IDD_MYSHELL_DIALOG == 102 && IDC_SLI_LINE == 1198 && IDC_BTN_SYSHIDE == 1199 && IDC_SHELL_BTN_MSGHIDE == 1200, "");
static_assert(IDD_DIALOG_LOGO == 273 && IDD_DIALOG_HELP == 215 && IDD_DIALOG_LOG == 218 && IDD_DIALOG_TITLEBAR == 195 && IDD_DIALOG_ROLECHOOSE == 150 && IDD_DIALOG_PROINTRO == 271 && IDD_DIALOG_ROLECREATE == 151 && IDD_DIALOG_LOGIN == 129, "");
static_assert(IDD_DIALOG_TASK == 173 && IDD_DIALOG_CONFIRM == 158 && IDD_MAIN_DIALOG == 130, "");
static_assert(sizeof(((CMyShellApp*)0)->m_hCursor) == 14 * sizeof(HCURSOR), "");
static_assert(CMyShellDlg::CMD_FRDMAKE == 4 && CMyShellDlg::CMD_TEMJOIN == 6 && CMyShellDlg::CMD_TEMINVITE == 7 && CMyShellDlg::CMD_FACJOIN == 8 && CMyShellDlg::CMD_FACINVITE == 9 && CMyShellDlg::CMD_TRADE == 14 && CMyShellDlg::CMD_LAY == 15 && CMyShellDlg::CMD_MPCCOURT == 16 && CMyShellDlg::CMD_DELNPC == 17 && CMyShellDlg::CMD_NPCEQUIP == 18, "");
static_assert(_STATUS_NONE == 1000 && _STATUS_ACCOUNT == 1001 && _STATUS_LOGIN == 1003 && _STATUS_REGISTER == 1004 && _STATUS_WAITING == 1008 && _STATUS_NORMAL == 1010 && _STATUS_EXIT == 1100, "");
static_assert(ON_LOGIN_NEW == 3 && ON_ROLECHOOSE_RETURN == 4 && ON_ROLECHOOSE_CHOOSE == 5 && ON_ROLECREATE_RETURN == 6 && ON_TALK_CHAT == 33 && ON_ACTION_INI == 52 && ON_IMPTITEM_ACCEPT == 85 && ON_FUSE_CLOSE == 104 && ON_SETUP_KOSHOW == 105 && ON_FRIENDADD_ACCEPT == 106 && ON_ENCHANT_CLOSE == 107 , "");
static_assert(CMD_FLASHITEM == 1003 && CMD_SETPKMODE == 1025 && CMD_LEVELUP == 1026 && CMD_ACTION_OPENDIALOG == 1090 && CMD_FRIEND_APPLY == 1052 && CMD_ACTION_TEACHAPPLY == 1091 , "");
static_assert(HOTKEY_PARENT_AMOUNT == 6 && HOTKEY_CHILD_AMOUNT == 10, "");
static_assert(DLG_MAIN == 3 && DLG_QUERY_MISSION == 9 && DLG_NPCBUY == 12 && DLG_MPCGOODS == 13 && DLG_TRADENPC == 15 && DLG_TRADEMPC == 16 && DLG_DEPOT == 17 && DLG_ACT == 20 && DLG_MAGIC == 21 && DLG_IMPROVE == 22 && DLG_NPCFACE == 23 && DLG_EMBED == 24 && DLG_MPCBOOTH == 28 && DLG_NPCBOOTH == 29 && DLG_NPCEQUIP == 36 && DLG_XP == 37 && DLG_BAG == 42 && DLG_FUSE == 43 && DLG_ENCHANT == 44 && DLG_NPCBUY_EMONEY == 45 && DLG_EXERCISE == 46 && DLG_EXECONFIRM == 47 && DLG_ALL == 48 , "");
static_assert(sizeof(DEVMODE) >= 148, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct MyShellDlgProbe
{
    static size_t Icon() { return offsetof(CMyShellDlg, m_hIcon); }
};

#define SFK_NEXT_MEMBER(m, T) o = RoundUp(o, __alignof(T)); Assert::IsTrue(offsetof(CMyShellDlg, m) == o); o += sizeof(T);

TEST_CLASS(MyShellDlgTests)
{
public:
    TEST_METHOD(LayoutFollowsTheBinary)
    {
        size_t o = sizeof(CDialog);
        SFK_NEXT_MEMBER(m_DlgLogin, CDlgLogin);
        SFK_NEXT_MEMBER(m_DlgRoleChoose, CDlgRoleChoose);
        SFK_NEXT_MEMBER(m_DlgRoleCreate, CDlgRoleCreate);
        SFK_NEXT_MEMBER(m_DlgMain, CDlgMain);
        SFK_NEXT_MEMBER(m_DlgConfirm, CDlgConfirm);
        SFK_NEXT_MEMBER(m_DlgTask, CDlgTask);
        SFK_NEXT_MEMBER(m_DlgTitleBar, CDlgTitleBar);
        SFK_NEXT_MEMBER(m_DlgHelp, CDlgHelp);
        SFK_NEXT_MEMBER(m_DlgLog, CDlgLog);
        SFK_NEXT_MEMBER(m_bLogin, BOOL);
        SFK_NEXT_MEMBER(m_bGameStarted, BOOL);
        SFK_NEXT_MEMBER(m_bWait, BOOL);
        SFK_NEXT_MEMBER(m_bRoleCreate, BOOL);
        SFK_NEXT_MEMBER(m_bMsgDlg, BOOL);
        SFK_NEXT_MEMBER(m_nMsgDlgWidth, int);
        SFK_NEXT_MEMBER(m_MsgDlgPnt, CPoint);
        SFK_NEXT_MEMBER(m_StaticPoint, CPoint[9]);
        SFK_NEXT_MEMBER(m_uMsgDlgType, UINT);
        SFK_NEXT_MEMBER(m_dwMsgDlgOwner, DWORD);
        SFK_NEXT_MEMBER(m_btStaticID, BYTE);
        SFK_NEXT_MEMBER(m_bIconFix, BOOL);
        SFK_NEXT_MEMBER(m_InitDlgRect, CRect);
        SFK_NEXT_MEMBER(m_bDlgMove, BOOL);
        SFK_NEXT_MEMBER(m_nScreenMode, int);
        SFK_NEXT_MEMBER(m_OldDeviceMode, DEVMODE);
        SFK_NEXT_MEMBER(m_bPickUp, BOOL);
        SFK_NEXT_MEMBER(m_IconMousePnt, CPoint);
        SFK_NEXT_MEMBER(m_IconShowPnt, CPoint);
        SFK_NEXT_MEMBER(m_IconTypeID, OBJID);
        SFK_NEXT_MEMBER(m_IconRealID, OBJID);
        SFK_NEXT_MEMBER(m_IconIndex, DWORD);
        SFK_NEXT_MEMBER(m_PickUpDlgID, UINT);
        SFK_NEXT_MEMBER(m_btIconType, BYTE);
        SFK_NEXT_MEMBER(m_btIconActType, BYTE);
        SFK_NEXT_MEMBER(m_MagicID, OBJID);
        SFK_NEXT_MEMBER(m_DisMagicID, OBJID);
        SFK_NEXT_MEMBER(m_XpID, OBJID);
        SFK_NEXT_MEMBER(m_uCurSkill, UINT);
        SFK_NEXT_MEMBER(m_pGameFont, CFont*);
        SFK_NEXT_MEMBER(m_strOwnerIni, char[128]);
        SFK_NEXT_MEMBER(m_dwScale, DWORD);
        SFK_NEXT_MEMBER(m_nFacWordValue, int);
        SFK_NEXT_MEMBER(m_nImptIndex, int);
        SFK_NEXT_MEMBER(m_nDropIndex, int);
        SFK_NEXT_MEMBER(m_DropPnt, CPoint);
        SFK_NEXT_MEMBER(m_bExpShow, BOOL);
        SFK_NEXT_MEMBER(m_bExit, BOOL);
        SFK_NEXT_MEMBER(m_bInitSuc, BOOL);
        SFK_NEXT_MEMBER(m_idActiveNpc, OBJID);
        SFK_NEXT_MEMBER(m_bMouseInShell, BOOL);
        SFK_NEXT_MEMBER(m_dwGameLoop, DWORD);
        SFK_NEXT_MEMBER(m_uCmd, UINT);
        SFK_NEXT_MEMBER(m_hCursorH, HCURSOR);
        SFK_NEXT_MEMBER(m_bCursorNpc, BOOL);
        SFK_NEXT_MEMBER(m_bCursorPk, BOOL);
        SFK_NEXT_MEMBER(m_bCursorPick, BOOL);
        SFK_NEXT_MEMBER(m_Hbr, HBRUSH);
        SFK_NEXT_MEMBER(m_pMutexGuard, CMutexGuard*);
        SFK_NEXT_MEMBER(m_BtnSysHide, CMyButton);
        SFK_NEXT_MEMBER(m_BtnMsgHide, CMyButton);
        SFK_NEXT_MEMBER(m_SliLine, CMySlider);
        o = RoundUp(o, __alignof(HICON));
        Assert::IsTrue(MyShellDlgProbe::Icon() == o);
        o += sizeof(HICON);
        Assert::IsTrue(sizeof(CMyShellDlg) == RoundUp(o, __alignof(CMyShellDlg)));

        Assert::IsTrue(offsetof(CMyShellDlg, m_bPickUp) - offsetof(CMyShellDlg, m_OldDeviceMode) == sizeof(DEVMODE));
    }
};
