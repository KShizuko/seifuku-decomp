#include "stdafx.h"
#include "MyShell.h"
#include "GameDataset.h"
#include "3droledata.h"
#include "BkWnd.h"
#include "BaseFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CMyShellApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

GemInfo CMyShellApp::m_GemInfo [24] ;
char	g_strControlAni[32] ;
char	g_strMagicAni[32] ;

CMyShellApp::CMyShellApp()
{
	strcpy ( g_strControlAni, "ani/Control.Ani" ) ;
	strcpy ( g_strMagicAni, "ani/Magic.Ani" ) ;

	m_GemInfo[0].btAmount = 0 ;
	m_GemInfo[0].btAtk = 0 ;
	m_GemInfo[0].btHit = 0 ;
	m_GemInfo[0].btMagicAtk = 5 ;
	m_GemInfo[0].btMagicExp = 0 ;
	m_GemInfo[0].btRoleExp = 0 ;
	m_GemInfo[0].btWeaponSkill = 0 ;
	m_GemInfo[0].strID = 10200 ;

	m_GemInfo[1].btAmount = 0 ;
	m_GemInfo[1].btAtk = 0 ;
	m_GemInfo[1].btHit = 0 ;
	m_GemInfo[1].btMagicAtk = 10 ;
	m_GemInfo[1].btMagicExp = 0 ;
	m_GemInfo[1].btRoleExp = 0 ;
	m_GemInfo[1].btWeaponSkill = 0 ;
	m_GemInfo[1].strID = 10201 ;

	m_GemInfo[2].btAmount = 0 ;
	m_GemInfo[2].btHit = 0 ;
	m_GemInfo[2].btMagicAtk = 15 ;
	m_GemInfo[2].btMagicExp = 0 ;
	m_GemInfo[2].btRoleExp = 0 ;
	m_GemInfo[2].btWeaponSkill = 0 ;
	m_GemInfo[2].strID = 10202 ;

	m_GemInfo[3].btAmount = 0 ;
	m_GemInfo[3].btAtk = 5 ;
	m_GemInfo[3].btHit = 0 ;
	m_GemInfo[3].btMagicAtk = 0 ;
	m_GemInfo[3].btMagicExp = 0 ;
	m_GemInfo[3].btRoleExp = 0 ;
	m_GemInfo[3].btWeaponSkill = 0 ;
	m_GemInfo[3].strID = 10203 ;

	m_GemInfo[4].btAmount = 0 ;
	m_GemInfo[4].btAtk = 10 ;
	m_GemInfo[4].btHit = 0 ;
	m_GemInfo[4].btMagicAtk = 0 ;
	m_GemInfo[4].btMagicExp = 0 ;
	m_GemInfo[4].btRoleExp = 0 ;
	m_GemInfo[4].btWeaponSkill = 0 ;
	m_GemInfo[4].strID = 10204 ;

	m_GemInfo[5].btAmount = 0 ;
	m_GemInfo[5].btAtk = 15 ;
	m_GemInfo[5].btHit = 0 ;
	m_GemInfo[5].btMagicAtk = 0 ;
	m_GemInfo[5].btMagicExp = 0 ;
	m_GemInfo[5].btRoleExp = 0 ;
	m_GemInfo[5].btWeaponSkill = 0 ;
	m_GemInfo[5].strID = 10205 ;

	m_GemInfo[6].btAmount = 0 ;
	m_GemInfo[6].btAtk = 0 ;
	m_GemInfo[6].btHit = 5 ;
	m_GemInfo[6].btMagicAtk = 0 ;
	m_GemInfo[6].btMagicExp = 0 ;
	m_GemInfo[6].btRoleExp = 0 ;
	m_GemInfo[6].btWeaponSkill = 0 ;
	m_GemInfo[6].strID = 10206 ;

	m_GemInfo[7].btAmount = 0 ;
	m_GemInfo[7].btAtk = 0 ;
	m_GemInfo[7].btHit = 10 ;
	m_GemInfo[7].btMagicAtk = 0 ;
	m_GemInfo[7].btMagicExp = 0 ;
	m_GemInfo[7].btRoleExp = 0 ;
	m_GemInfo[7].btWeaponSkill = 0 ;
	m_GemInfo[7].strID = 10207 ;

	m_GemInfo[8].btAmount = 0 ;
	m_GemInfo[8].btAtk = 0 ;
	m_GemInfo[8].btHit = 15 ;
	m_GemInfo[8].btMagicAtk = 0 ;
	m_GemInfo[8].btMagicExp = 0 ;
	m_GemInfo[8].btRoleExp = 0 ;
	m_GemInfo[8].btWeaponSkill = 0 ;
	m_GemInfo[8].strID = 10208 ;

	m_GemInfo[9].btAmount = 0 ;
	m_GemInfo[9].btAtk = 0 ;
	m_GemInfo[9].btHit = 0 ;
	m_GemInfo[9].btMagicAtk = 0 ;
	m_GemInfo[9].btMagicExp = 0 ;
	m_GemInfo[9].btRoleExp = 10 ;
	m_GemInfo[9].btWeaponSkill = 0 ;
	m_GemInfo[9].strID = 10209 ;

	m_GemInfo[10].btAmount = 0 ;
	m_GemInfo[10].btAtk = 0 ;
	m_GemInfo[10].btHit = 0 ;
	m_GemInfo[10].btMagicAtk = 0 ;
	m_GemInfo[10].btMagicExp = 0 ;
	m_GemInfo[10].btRoleExp = 15 ;
	m_GemInfo[10].btWeaponSkill = 0 ;
	m_GemInfo[10].strID = 10210 ;

	m_GemInfo[11].btAmount = 0 ;
	m_GemInfo[11].btAtk = 0 ;
	m_GemInfo[11].btHit = 0 ;
	m_GemInfo[11].btMagicAtk = 0 ;
	m_GemInfo[11].btMagicExp = 0 ;
	m_GemInfo[11].btRoleExp = 25 ;
	m_GemInfo[11].btWeaponSkill = 0 ;
	m_GemInfo[11].strID = 10211 ;

	m_GemInfo[12].btAmount = 50 ;
	m_GemInfo[12].btAtk = 0 ;
	m_GemInfo[12].btHit = 0 ;
	m_GemInfo[12].btMagicAtk = 0 ;
	m_GemInfo[12].btMagicExp = 0 ;
	m_GemInfo[12].btRoleExp = 0 ;
	m_GemInfo[12].btWeaponSkill = 0 ;
	m_GemInfo[12].strID = 10212 ;

	m_GemInfo[13].btAmount = 100 ;
	m_GemInfo[13].btAtk = 0 ;
	m_GemInfo[13].btHit = 0 ;
	m_GemInfo[13].btMagicAtk = 0 ;
	m_GemInfo[13].btMagicExp = 0 ;
	m_GemInfo[13].btRoleExp = 0 ;
	m_GemInfo[13].btWeaponSkill = 0 ;
	m_GemInfo[13].strID = 10213 ;

	m_GemInfo[14].btAmount = 200 ;
	m_GemInfo[14].btAtk = 0 ;
	m_GemInfo[14].btHit = 0 ;
	m_GemInfo[14].btMagicAtk = 0 ;
	m_GemInfo[14].btMagicExp = 0 ;
	m_GemInfo[14].btRoleExp = 0 ;
	m_GemInfo[14].btWeaponSkill = 0 ;
	m_GemInfo[14].strID = 10214 ;

	m_GemInfo[15].btAmount = 0 ;
	m_GemInfo[15].btAtk = 0 ;
	m_GemInfo[15].btHit = 0 ;
	m_GemInfo[15].btMagicAtk = 0 ;
	m_GemInfo[15].btMagicExp = 0 ;
	m_GemInfo[15].btRoleExp = 0 ;
	m_GemInfo[15].btWeaponSkill = 30 ;
	m_GemInfo[15].strID = 10215 ;

	m_GemInfo[16].btAmount = 0 ;
	m_GemInfo[16].btAtk = 0 ;
	m_GemInfo[16].btHit = 0 ;
	m_GemInfo[16].btMagicAtk = 0 ;
	m_GemInfo[16].btMagicExp = 0 ;
	m_GemInfo[16].btRoleExp = 0 ;
	m_GemInfo[16].btWeaponSkill = 50 ;
	m_GemInfo[16].strID = 10216 ;

	m_GemInfo[17].btAmount = 0 ;
	m_GemInfo[17].btAtk = 0 ;
	m_GemInfo[17].btHit = 0 ;
	m_GemInfo[17].btMagicAtk = 0 ;
	m_GemInfo[17].btMagicExp = 0 ;
	m_GemInfo[17].btRoleExp = 0 ;
	m_GemInfo[17].btWeaponSkill = 100 ;
	m_GemInfo[17].strID = 10217 ;

	m_GemInfo[18].btAmount = 0 ;
	m_GemInfo[18].btAtk = 0 ;
	m_GemInfo[18].btHit = 0 ;
	m_GemInfo[18].btMagicAtk = 0 ;
	m_GemInfo[18].btMagicExp = 15 ;
	m_GemInfo[18].btRoleExp = 0 ;
	m_GemInfo[18].btWeaponSkill = 0 ;
	m_GemInfo[18].strID = 10218 ;

	m_GemInfo[19].btAmount = 0 ;
	m_GemInfo[19].btAtk = 0 ;
	m_GemInfo[19].btHit = 0 ;
	m_GemInfo[19].btMagicAtk = 0 ;
	m_GemInfo[19].btMagicExp = 30 ;
	m_GemInfo[19].btRoleExp = 0 ;
	m_GemInfo[19].btWeaponSkill = 0 ;
	m_GemInfo[19].strID = 10219 ;

	m_GemInfo[20].btAmount = 0 ;
	m_GemInfo[20].btAtk = 0 ;
	m_GemInfo[20].btHit = 0 ;
	m_GemInfo[20].btMagicAtk = 0 ;
	m_GemInfo[20].btMagicExp = 50 ;
	m_GemInfo[20].btRoleExp = 0 ;
	m_GemInfo[20].btWeaponSkill = 0 ;
	m_GemInfo[20].strID = 10220 ;

	for ( int i = 0; i < 14; i++ )
		m_hCursor[i] = NULL ;
	m_GemInfo[21].btAmount = 0 ;
	m_GemInfo[21].btAtk = 0 ;
	m_GemInfo[21].btHit = 0 ;
	m_GemInfo[21].btMagicAtk = 0 ;
	m_GemInfo[21].btMagicExp = 0 ;
	m_GemInfo[21].btRoleExp = 0 ;
	m_GemInfo[21].btWeaponSkill = 0 ;
	m_GemInfo[21].strID = 10457 ;

	m_GemInfo[22].btAmount = 0 ;
	m_GemInfo[22].btAtk = 0 ;
	m_GemInfo[22].btHit = 0 ;
	m_GemInfo[22].btMagicAtk = 0 ;
	m_GemInfo[22].btMagicExp = 0 ;
	m_GemInfo[22].btRoleExp = 0 ;
	m_GemInfo[22].btWeaponSkill = 0 ;
	m_GemInfo[22].strID = 10458 ;

	m_GemInfo[23].btAmount = 0 ;
	m_GemInfo[23].btAtk = 0 ;
	m_GemInfo[23].btHit = 0 ;
	m_GemInfo[23].btMagicAtk = 0 ;
	m_GemInfo[23].btMagicExp = 0 ;
	m_GemInfo[23].btRoleExp = 0 ;
	m_GemInfo[23].btWeaponSkill = 0 ;
	m_GemInfo[23].strID = 10459 ;
}

CMyShellApp theApp;

CBkWnd *g_pBkWnd = NULL ;

BOOL CMyShellApp::InitInstance()
{
	AfxEnableControlContainer();

	GameDataSetQuery()->InitStrRes () ;
	LoadCursors () ;

	char szCmd[256] = "" ;
	char szAccount[256] = "" ;
	char szPassword[256] = "" ;
	sscanf ( m_lpCmdLine, "%s %s %s", szCmd, szAccount, szPassword ) ;
	int nWords = sscanf ( m_lpCmdLine, "%s %s %s", szCmd, szAccount, szPassword ) ;
	if ( nWords < 1 )
	{
		::MessageBox ( NULL, GameDataSetQuery()->GetStr ( 10047 ), "Error", MB_ICONHAND ) ;
		return FALSE ;
	}
	if ( _stricmp ( szCmd, "blacknull" ) != 0 )
	{
		::MessageBox ( NULL, GameDataSetQuery()->GetStr ( 10047 ), "Error", MB_ICONHAND ) ;
		return FALSE ;
	}

#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	g_pBkWnd = new CBkWnd ;
	m_pMainWnd = g_pBkWnd ;

	g_pBkWnd->Create() ;
	g_pBkWnd->ShowWindow( SW_SHOW ) ;

	if ( strlen ( szAccount ) != 0 && strlen ( szPassword ) != 0 )
		g_pBkWnd->m_MyShellDlg.m_DlgLogin.SetAccount ( szAccount, szPassword ) ;

	return TRUE;
}

const char* CMyShellApp::StringChange(const char *strText, BYTE btType)
{
	CString strTemp;
	strTemp = strText ;
	switch ( btType )
	{
	case 0:
		strTemp.Replace ( "\\t", "\t" ) ;
		strTemp.Replace ( "\\n", "\n" ) ;
		strcpy( ( char* )strText, strTemp ) ;
		break ;
	case 1:
		if ( -1 == strTemp.Find ( "~" ) )
			return strText ;
		strTemp.Replace ( "~", " " ) ;
		strcpy ( ( char* ) strText, strTemp ) ;
		break ;
	case 2:
		{
			int nLen = strlen ( strText ) ;
			if ( nLen > 3 && nLen < 7 )
					strTemp.Insert ( nLen - 3, ',' ) ;
			else if ( nLen >= 7 )
			{
				strTemp.Insert ( nLen - 6, ',' ) ;
				strTemp.Insert ( nLen - 2, ',' ) ;
			}
			strcpy ( ( char* ) strText, strTemp ) ;
		}
		break ;
	}

	return strText ;
}

BOOL CMyShellApp::OnIdle(LONG lCount)
{
	static DWORD timeLoop	=0;
	DWORD timeNow	=::TimeGet();
	if (timeNow < timeLoop+25)
	{
		::Sleep( timeLoop + 25 - timeNow ) ;
	}
	else
	{
		timeLoop	=timeNow;
		g_pBkWnd->m_MyShellDlg.OnTimer( 1 ) ;
	}
	return TRUE ;
}

void CMyShellApp::LoadCursors()
{
	m_hCursor[0] = ::LoadCursorFromFile ( "Data/Cursor/Faction.ani" ) ;
	m_hCursor[1] = ::LoadCursorFromFile ( "Data/Cursor/Fix.cur" ) ;
	m_hCursor[2] = ::LoadCursorFromFile ( "Data/Cursor/Friend.cur" ) ;
	m_hCursor[3] = ::LoadCursorFromFile ( "Data/Cursor/pickup.ani" ) ;
	m_hCursor[4] = ::LoadCursorFromFile ( "Data/Cursor/Marriage.ani" ) ;
	m_hCursor[5] = ::LoadCursorFromFile ( "Data/Cursor/Descyr.cur" ) ;
	m_hCursor[6] = ::LoadCursorFromFile ( "Data/Cursor/pk.ani" ) ;
	m_hCursor[7] = ::LoadCursorFromFile ( "Data/Cursor/Normal.ani" ) ;
	m_hCursor[8] = ::LoadCursorFromFile ( "Data/Cursor/Invite.ani" ) ;
	m_hCursor[9] = ::LoadCursorFromFile ( "Data/Cursor/Invite.ani" ) ;
	m_hCursor[10] = ::LoadCursorFromFile ( "Data/Cursor/Trade.ani" ) ;
	m_hCursor[11] = ::LoadCursorFromFile ( "Data/Cursor/Button.cur" ) ;
	m_hCursor[12] = ::LoadCursorFromFile ( "Data/Cursor/Npc.ani" ) ;
	m_hCursor[13] = ::LoadCursorFromFile ( "Data/Cursor/Delete.ani" ) ;
}

int CMyShellApp::ExitInstance()
{
	if ( g_pBkWnd )
	{
		g_pBkWnd->DestroyWindow () ;
		delete g_pBkWnd ;
	}
	for ( int i = 0; i < 14; i++ )
		::DestroyCursor ( m_hCursor[i] ) ;
	return CWinApp::ExitInstance () ;
}
