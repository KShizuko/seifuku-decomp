#include "stdafx.h"
#include "MyShell.h"
#include "DlgTalk.h"

#include "GameMsg.h"
#include "Hero.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgTalk::CDlgTalk(CWnd* pParent )
	: CDialog(CDlgTalk::IDD, pParent)
{
	m_bShow = false ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_dwLastTime = 0 ;
	m_nListAmount = 0;
	m_nListIndex = 0 ;
	m_uTalkMode = G_CLICK ;
	m_uAddBtnMode = 0 ;
	m_uDelIndex = 0 ;
	m_nTargetAmount = 0 ;
	m_nCurTargetIndex = 0 ;
	for(int i=0;i<5;i++)
	{
		m_uTargetTalkMode[i] = 0 ;
		strcpy(m_strTargetName[i],"");
	}
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_HbrEdit = CreateSolidBrush ( 0 ) ;
	m_bInitSuc = false ;
	m_bShowActionBtn = true ;
	m_dwLastGmTime = 0 ;
}


void CDlgTalk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TALK_COB_DATA, m_TalkDataCob);
	DDX_Control(pDX, IDC_TALK_EDT_NAME, m_TalkNameEdt);
	DDX_Control(pDX, IDC_TALK_IMG_MODE, m_TalkModeImg);
	DDX_Control(pDX, IDC_TALK_BTN_SEND, m_TalkSendBtn);
	DDX_Control(pDX, IDC_TALK_BTN_ACTION, m_TalkActionBtn);
	DDX_Control(pDX, IDC_TALK_BTN_CLEAR, m_TalkClearBtn);
	DDX_Control(pDX, IDC_TALK_BTN_ADD, m_TalkAddBtn);
	DDX_Control(pDX, IDC_TALK_BTN_LIST, m_TalkListBtn);
	DDX_Control(pDX, IDC_TALK_BTN_LOG, m_TalkLogBtn);
}

BEGIN_MESSAGE_MAP(CDlgTalk, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_TALK_BTN_SEND, OnTalkBtnSend)
	ON_BN_CLICKED(IDC_TALK_BTN_CLEAR, OnTalkBtnClear)
	ON_BN_CLICKED(IDC_TALK_IMG_MODE, OnTalkImgMode)
	ON_WM_LBUTTONDOWN()
	ON_EN_UPDATE(IDC_TALK_EDT_NAME, OnUpdateTalkEdtName)
	ON_BN_CLICKED(IDC_TALK_BTN_LIST, OnTalkBtnList)
	ON_BN_CLICKED(IDC_TALK_BTN_ADD, OnTalkBtnAdd)
	ON_BN_CLICKED(IDC_TALK_BTN_LOG, OnTalkBtnLog)
	ON_BN_CLICKED(IDC_TALK_BTN_ACTION, OnTalkBtnAction)
END_MESSAGE_MAP()

LRESULT CDlgTalk::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		if ( wParam == false )
		{
			m_bShow = false ;
			if ( m_MenuTalk.m_bShow )
				m_MenuTalk.ShowWindow ( SW_HIDE ) ;
			CDialog::MoveWindow ( m_rect.top, m_rect.left, 0, 0, false ) ;
		}
		else
		{
			m_bShow = true ;
			m_TalkDataCob.SetFocus () ;
			CDialog::MoveWindow ( m_rect, false ) ;
		}
		break ;
	case WM_DESTROY:
		DeleteObject ( m_Hbr ) ;
		DeleteObject ( m_HbrEdit ) ;
		break ;
	case WM_ERASEBKGND:
		return true ;
	case WM_NCPAINT:
		return false ;
	case WM_PAINT:
		{
			PAINTSTRUCT ps ;
			::BeginPaint ( m_hWnd, &ps ) ;
			::EndPaint ( m_hWnd, &ps ) ;
		}
		return false ;
	case WM_COMMAND:
		if ( wParam == 2 )
		{
			return true ;
		}
		else if ( wParam == 1 )
		{
			CDlgTalk::OnTalkBtnSend ( ) ;
			return true ;
		}
		break ;
	case WM_VSCROLL:
		if ( wParam == SB_LINEUP )
		{
			if ( m_nListIndex > 0 )
			{
				m_nListIndex-- ;
				char strChar[1024] ;
				m_ListBox.GetText ( m_nListIndex, strChar ) ;
				m_TalkDataCob.SetWindowText ( strChar ) ;
				return true ;
			}
		}
		else if ( wParam == SB_LINEDOWN )
		{
			if ( m_nListIndex <= m_nListAmount - 1 )
			{
				char strChar[1024] ;
				m_ListBox.GetText ( m_nListIndex, strChar ) ;

				if ( 0 == m_nListIndex )
				{
					char strTemp[1024] ;
					m_TalkDataCob.GetWindowText ( strTemp, 1024 ) ;
					if ( !strcmp ( strTemp, strChar ) )
					{
						m_nListIndex++ ;
						m_ListBox.GetText ( m_nListIndex, strChar ) ;
					}
				}

				m_TalkDataCob.SetWindowText ( strChar ) ;
				if ( m_nListIndex < m_nListAmount - 1  )
					m_nListIndex++ ;
				return true ;
			}
		}
		break ;
	case WM_MY_MESSAGE:
		if ( wParam == MENU_TALK_CLICK )
		{
			CDlgTalk::SetTalkMode ( lParam ) ;
			return true ;
		}
		else if ( wParam == ACTION_CHOOSE )
		{
			CString strEmotion, strTemp ;
			if ( m_TalkDataCob.GetWindowTextLength () + 3 > 88 )
				return true ;
			{
				if ( lParam < 10 )
					sprintf ( strEmotion.GetBuffer ( 8 ), "#0%d", lParam ) ;
				else
					sprintf ( strEmotion.GetBuffer ( 8 ), "#%d", lParam ) ;
				m_TalkDataCob.GetWindowText ( strTemp ) ;
				int nStart, nEnd ;
				m_TalkDataCob.GetSel ( nStart, nEnd ) ;
				strTemp.Insert ( nStart, strEmotion ) ;
				m_TalkDataCob.SetWindowText ( strTemp ) ;
				m_TalkDataCob.SetSel ( nStart + 3, nStart + 3 ) ;
			}
		}
		break ;
	case CB_GETITEMDATA:
		{
			if ( wParam )
			{
				char strTemp[16] ;
				m_DlgListBox.m_ListBox.GetText ( lParam, strTemp ) ;
				m_TalkNameEdt.SetWindowText ( strTemp ) ;
				m_DlgListBox.ShowWindow ( SW_HIDE ) ;
				return true ;
			}
			else
			{
				char strTemp[16] ;
				m_DlgListBox.m_ListBox.GetText ( lParam, strTemp ) ;
				m_TalkNameEdt.SetWindowText ( strTemp ) ;
				return true ;
			}
		}
		break ;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDlgTalk::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkColor ( 0 ) ;
	pDC->SetBkMode ( TRANSPARENT ) ;
	if ( nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_LISTBOX )
		return m_HbrEdit ;
	return m_Hbr ;
}

BOOL CDlgTalk::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgTalk::GetWindowRect ( rect ) ;
	m_ListBox.Create ( WS_CHILD|WS_HSCROLL, CRect ( 0, 0, 0, 0 ), this, 0 ) ;

	if ( !m_TalkDataCob.Init ( rect.left, rect.top, NULL ) )
		return false ;
	m_TalkDataCob.SetLimitText ( 88 ) ;

	if ( !m_TalkNameEdt.Init ( rect.left, rect.top, NULL, 0x00ffff ) )
		return false ;
	m_TalkNameEdt.SetLimitText ( 15 ) ;
	m_TalkNameEdt.SetOffset ( CPoint ( 4, 2 ) ) ;

	if ( !m_TalkModeImg.Init ( rect.left, rect.top, "Button50", CONTROL_IMAGE ) )
		return false ;

	if ( !m_TalkClearBtn.Init ( rect.left, rect.top, "Button514" ) )
		return false ;
	if ( !m_TalkAddBtn.Init ( rect.left, rect.top, "Button57" ) )
		return false ;
	if ( !m_TalkListBtn.Init ( rect.left, rect.top, "Button510" ) )
		return false ;
	if ( !m_TalkSendBtn.Init ( rect.left, rect.top, "Button511" ) )
		return false ;
	if ( !m_TalkLogBtn.Init ( rect.left, rect.top, "Button512" ) )
		return false ;
	m_TalkActionBtn.Init ( rect.left, rect.top, "Button513" ) ;

	if ( !m_MenuTalk.Create ( IDD_MENU_TALK, this->GetParent () ) )
		return false ;
	m_MenuTalk.SetParentWnd ( this ) ;

	m_DlgListBox.Create ( IDD_DIALOG_LISTBOX, this->GetParent () ) ;
	m_DlgListBox.m_ListBox.SetParentCob ( this ) ;
	m_DlgListBox.GetWindowRect ( rect ) ;
	m_DlgListBox.MoveWindow ( 117, CMyBitmap::GetScreenHeight () - rect.Height () - 64, rect.Width (), rect.Height (), true ) ;

	m_DlgTalkAct.Create ( IDD_DIALOG_TALKACT, this->GetParent () ) ;
	m_DlgTalkAct.SetParentCWnd ( this ) ;
	m_DlgTalkAct.GetWindowRect ( rect ) ;
	m_DlgTalkAct.MoveWindow ( 800 - rect.Width (), CMyBitmap::GetScreenHeight () - rect.Height () - 64, rect.Width (), rect.Height (), true ) ;

	m_TalkModeImg.SetCursorChange ( ( BOOL ) ( INT_PTR ) ( ( CMyShellApp* ) AfxGetApp () )->m_hCursor[11] ) ;
	m_bInitSuc = true ;
	return TRUE;
}

void CDlgTalk::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;

	if ( m_bInitSuc )
	{
		CRect menuRect, rect, imgRect ;
		m_MenuTalk.GetWindowRect ( menuRect ) ;
		CDlgTalk::GetWindowRect ( rect ) ;
		m_TalkModeImg.GetWindowRect ( imgRect ) ;
		m_MenuTalk.MoveWindow ( m_Pnt.x + imgRect.left - rect.left,
								m_Pnt.y + imgRect.top - rect.top - menuRect.Height () - 4,
								imgRect.Width (),
								menuRect.Height (),
								true ) ;
	}
}

void CDlgTalk::Show()
{
	if ( m_bInitSuc && m_bShow )
	{
		IAni* pShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													   "Dialog5",
													   EXIGENCE_IMMEDIATE ) ;
		if ( pShowAni != NULL )
		{
			pShowAni->Show ( 0, m_Pnt.x, m_Pnt.y - 2 ) ;
			pShowAni->Show ( 1, m_Pnt.x + 256, m_Pnt.y - 2 ) ;
			pShowAni->Show ( 2, m_Pnt.x + 512, m_Pnt.y - 2 ) ;
			pShowAni->Show ( 3, m_Pnt.x + 768, m_Pnt.y - 2 ) ;
		}
		else
			return ;
		m_TalkDataCob.Show ( m_Pnt.x, m_Pnt.y ) ;

		m_TalkNameEdt.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkModeImg.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkClearBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkAddBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkListBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkSendBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		if ( m_bShowActionBtn )
			m_TalkActionBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TalkLogBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_MenuTalk.Show () ;

		m_DlgListBox.Show () ;
		m_DlgTalkAct.Show () ;
	}
}

void CDlgTalk::OnTalkBtnSend()
{
	char Char[1024] = "";
	memset(Char, 0L, sizeof(Char));

	m_TalkDataCob.GetLine ( 0, Char, 1024 ) ;
	if ( Char[0] != NULL  && m_bShow )
	{
		m_TalkDataCob.SetWindowText ( NULL ) ;
		m_TalkDataCob.SetFocus () ;

		if (Char[0] == '/')
		{
			char szCmd[256] = "";
			if (!sscanf(Char+1, "%s", szCmd))
				return;

			if (0 == stricmp(szCmd, "3deffect"))
			{
				char szIndex[256] = "";
				if (2 == sscanf(Char+1, "%s %s", szCmd, szIndex))
				{
					if (g_objHero.m_objEffect)
						g_objHero.m_objEffect->Add(szIndex);
				}

				return;
			}
		}

		if (::TimeGet()-m_dwLastTime <= 1000/_MAX_WORDS_PS)
		{
			g_objGameMsg.AddMsg( ( char* )GameDataSetQuery()->GetStr ( 10044 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
			return;
		}

		m_dwLastTime	=::TimeGet();

		char strName[16] ;
		switch ( m_uTalkMode )
		{
		case G_CLICK:
			if ( m_TalkNameEdt.GetWindowTextLength () > 0 )
			{
				m_TalkNameEdt.GetWindowText ( strName, 16 ) ;
				g_objHero.Talk(	strName,
								Char,
								NULL,
								0xffffff,
								_TXTATR_NORMAL);
			}
			else
				g_objHero.Talk(	( CPlayer* )NULL,
								Char,
								NULL,
								0xffffff,
								_TXTATR_NORMAL);
			break ;
		case S_CLICK:
			{
				m_TalkNameEdt.GetWindowText ( strName, 16 ) ;
				if ( strName )
					g_objHero.Talk(	strName,
									Char,
									NULL,
									0xffffff,
									_TXTATR_PRIVATE);
			}
			break ;
		case D_CLICK:
			{
				if ( m_TalkNameEdt.GetWindowTextLength () > 0 )
				{
					m_TalkNameEdt.GetWindowText ( strName, 16 ) ;
					g_objHero.Talk(	strName,
									Char,
									NULL,
									0xffffff,
									_TXTATR_TEAM);
				}
				else
					g_objHero.Talk(	( CPlayer* )NULL,
					Char,
					NULL,
					0xffffff,
					_TXTATR_TEAM);
			}
			break ;
		case B_CLICK:
			{
				if ( m_TalkNameEdt.GetWindowTextLength () > 0 )
				{
					m_TalkNameEdt.GetWindowText ( strName, 16 ) ;
					g_objHero.Talk(	strName,
									Char,
									NULL,
									0xffffff,
									_TXTATR_SYNDICATE);
				}
				else
					g_objHero.Talk(	( CPlayer* )NULL,
					Char,
					NULL,
					0xffffff,
					_TXTATR_SYNDICATE);
			}
			break ;
		case H_CLICK:
			if ( !m_DlgListBox.m_bShow )
				m_DlgListBox.ShowWindow ( SW_HIDE ) ;
			m_TalkNameEdt.GetWindowText ( strName, 16 ) ;
			g_objHero.Talk(	strName,
							Char,
							NULL,
							0xffffff,
							_TXTATR_FRIEND);
			break ;
		case GM_CLICK:
			if ( !m_DlgListBox.m_bShow )
				m_DlgListBox.ShowWindow ( SW_HIDE ) ;
			if ( ::TimeGet () - m_dwLastGmTime > 20000 )
			{
				m_TalkNameEdt.GetWindowText ( strName, 16 ) ;
				g_objHero.Talk(	strName,
								Char,
								NULL,
								0xffffff,
								_TXTATR_SERVE);
				m_dwLastGmTime = ::TimeGet () ;
			}
			else
				g_objGameMsg.AddMsg ( ( char* )GameDataSetQuery()->GetStr ( 10444 ), _TXTATR_SYSTEM, 0xFF0000, 0, false ) ;
			break ;
		}
		CDlgTalk::SetTalkTargetList () ;

		for ( int i = 0; i < m_nListAmount; i++ )
		{
			char tempChar[1024];
			m_ListBox.GetText ( i, tempChar ) ;
			if ( strcmp ( tempChar, Char ) == 0 )
			{
				m_ListBox.DeleteString ( i ) ;
				m_ListBox.InsertString ( 0, Char ) ;
				m_nListIndex = 0 ;
				return ;
			}
		}
		m_ListBox.InsertString ( 0, Char ) ;
		m_nListIndex = 0 ;
		m_nListAmount++ ;
		if ( m_nListAmount > 20 )
		{
			m_nListAmount-- ;
			m_ListBox.DeleteString ( 20 ) ;
		}
	}
	else
		CDlgTalk::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_TALK_CLOSE ) ;
}

void CDlgTalk::OnTalkBtnClear()
{
	g_objGameMsg.ClearMsg();
}

void CDlgTalk::OnTalkImgMode()
{
	if ( !m_MenuTalk.m_bShow )
	{
		m_MenuTalk.ShowWindow ( SW_SHOW ) ;
	}
	else
	{
		m_MenuTalk.ShowWindow ( SW_HIDE ) ;
	}
}

void CDlgTalk::SetTalkTarget(CPlayer *pTalkTarget)
{
	m_TalkPlayerID = pTalkTarget->GetID() ;
	if ( pTalkTarget->GetName() )
	{
		m_TalkNameEdt.SetWindowText ( pTalkTarget->GetName () ) ;
	}
}

void CDlgTalk::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_TalkDataCob.SetFocus () ;

	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgTalk::OnUpdateTalkEdtName()
{
	char strName[16] ;
	m_TalkNameEdt.GetWindowText ( strName, 16 ) ;
	if ( strName[0] != NULL )
	{
		char strTemp[16] ;
		int nCount = m_DlgListBox.m_ListBox.GetCount () ;
		if ( nCount > 0 )
		{
			for ( int i = 0; i < nCount; i++ )
			{
				m_DlgListBox.m_ListBox.GetText ( i, strTemp ) ;
				if ( strcmp ( strName, strTemp ) == 0 )
				{
					m_TalkAddBtn.ChangeImage ( "Button59" ) ;
					m_TalkNameEdt.SetFontColor ( 0xff0000 ) ;
					m_uDelIndex = i ;
					m_uAddBtnMode = 2 ;
					return ;
				}
			}
			if ( nCount < 18 )
			{
				m_TalkAddBtn.ChangeImage ( "Button58" ) ;
				m_TalkNameEdt.SetFontColor ( 0xffffff ) ;
				m_uAddBtnMode = 1 ;
			}
			else
			{
				m_TalkAddBtn.ChangeImage ( "Button57" ) ;
				m_TalkNameEdt.SetFontColor ( 0xffffff ) ;
				m_uAddBtnMode = 0 ;
			}
		}
		else
		{
			m_TalkAddBtn.ChangeImage ( "Button58" ) ;
			m_TalkNameEdt.SetFontColor ( 0xffffff ) ;
			m_uAddBtnMode = 1 ;
		}
	}
	else
	{
		m_TalkAddBtn.ChangeImage ( "Button57" ) ;
		m_TalkNameEdt.SetFontColor ( 0xffffff ) ;
		m_uAddBtnMode = 0 ;
	}
}

void CDlgTalk::OnTalkBtnList()
{
	if ( !m_DlgListBox.m_bShow )
	{
		CRect rect ;
		m_DlgListBox.GetWindowRect ( rect ) ;
		m_DlgListBox.MoveWindow ( 117, CMyBitmap::GetScreenHeight () - rect.Height () - 64, rect.Width (), rect.Height (), true ) ;
		m_DlgListBox.ShowWindow ( SW_SHOW ) ;
	}
	else
		m_DlgListBox.ShowWindow ( SW_HIDE ) ;
}

void CDlgTalk::OnTalkBtnAdd()
{
	switch ( m_uAddBtnMode )
	{
	case 1:
		{
			char strName[16] ;
			m_TalkNameEdt.GetWindowText ( strName, 16 ) ;
			m_DlgListBox.m_ListBox.AddString ( strName ) ;
			m_TalkNameEdt.SetFontColor ( 0xff0000 ) ;
			m_TalkNameEdt.SetFocus () ;
			m_TalkAddBtn.ChangeImage ( "Button57" ) ;
			m_uAddBtnMode = 0 ;
		}
		break ;
	case 2:
		CDlgTalk::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_TALK_DEL ) ;
		break ;
	}
}

void CDlgTalk::SetTalkMode( UINT uTalkMode )
{
	m_uTalkMode = uTalkMode ;
	switch ( m_uTalkMode )
	{
	case G_CLICK:
		m_TalkModeImg.InsertImage ( "Button50" ) ;
		break ;
	case S_CLICK:
		m_TalkModeImg.InsertImage ( "Button51" ) ;
		break ;
	case D_CLICK:
		m_TalkModeImg.InsertImage ( "Button52" ) ;
		if ( m_DlgListBox.m_bShow )
			m_DlgListBox.ShowWindow ( SW_HIDE ) ;
		break ;
	case B_CLICK:
		m_TalkModeImg.InsertImage ( "Button53" ) ;
		if ( m_DlgListBox.m_bShow )
			m_DlgListBox.ShowWindow ( SW_HIDE ) ;
		break ;
	case H_CLICK:
		m_TalkModeImg.InsertImage ( "Button54" ) ;
		if ( m_DlgListBox.m_bShow )
			m_DlgListBox.ShowWindow ( SW_HIDE ) ;
		break ;
	case GM_CLICK:
		m_TalkModeImg.InsertImage ( "Button55" ) ;
		if ( m_DlgListBox.m_bShow )
			m_DlgListBox.ShowWindow ( SW_HIDE ) ;
		break ;
	}
}

void CDlgTalk::DeleteTalkTarget()
{
	m_DlgListBox.m_ListBox.DeleteString ( m_uDelIndex ) ;
	m_TalkNameEdt.SetWindowText ( NULL ) ;
	m_TalkNameEdt.SetFocus () ;
}

void CDlgTalk::OnTalkBtnLog()
{
	CDlgTalk::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_TALK_CHAT ) ;
}

void CDlgTalk::SetTalkTargetList()
{
	int i ;
	if ( m_TalkNameEdt.GetWindowTextLength () > 0 &&
		 ( m_uTalkMode == G_CLICK || m_uTalkMode == S_CLICK || m_uTalkMode == H_CLICK || m_uTalkMode == GM_CLICK ) )
	{
		char strName[16] ;
		m_TalkNameEdt.GetWindowText ( strName, 16 ) ;
		for ( i = 0; i < 5; i++ )
		{
			if ( m_uTargetTalkMode[i] == 0 )
				break ;
			else if ( m_uTargetTalkMode[i] == m_uTalkMode && strcmp ( strName, m_strTargetName[i] ) == 0 )
				return ;
		}

		if ( i == 5 )
		{
			for ( i = 0; i < 4; i++ )
			{
				m_uTargetTalkMode[i] = m_uTargetTalkMode[i + 1] ;
				strcpy ( m_strTargetName[i], m_strTargetName[i + 1] ) ;
			}
			m_uTargetTalkMode[i] = m_uTalkMode ;
			strcpy ( m_strTargetName[i], strName ) ;
			m_nTargetAmount = 5 ;
		}
		else
		{
			m_uTargetTalkMode[i] = m_uTalkMode ;
			strcpy ( m_strTargetName[i], strName ) ;
			m_nTargetAmount = i + 1 ;
		}
	}
	else
	{
		for ( i = 0; i < 5; i++ )
		{
			if ( m_uTargetTalkMode[i] == 0 )
				break ;
			else if ( m_uTargetTalkMode[i] == m_uTalkMode )
				return ;
		}

		if ( i == 5 )
		{
			for ( i = 0; i < 4; i++ )
			{
				m_uTargetTalkMode[i] = m_uTargetTalkMode[i + 1] ;
				strcpy ( m_strTargetName[i], m_strTargetName[i + 1] ) ;
			}
			m_uTargetTalkMode[i] = m_uTalkMode ;
			m_strTargetName[i][0] = NULL ;
			m_nTargetAmount = 5 ;
		}
		else
		{
			m_uTargetTalkMode[i] = m_uTalkMode ;
			m_strTargetName[i][0] = NULL ;
			m_nTargetAmount = i + 1 ;
		}
	}
}

void CDlgTalk::CycleTalkTarget( int nDir )
{
	if ( nDir == SB_LEFT )
	{
		if ( m_nTargetAmount > 0 )
		{
			if ( m_nCurTargetIndex == 0 )
				m_nCurTargetIndex = m_nTargetAmount - 1;
			else
				m_nCurTargetIndex-- ;

			CDlgTalk::SetTalkMode ( m_uTargetTalkMode[m_nCurTargetIndex] ) ;
			if ( m_strTargetName[m_nCurTargetIndex][0] != NULL )
				m_TalkNameEdt.SetWindowText ( m_strTargetName[m_nCurTargetIndex] ) ;
			else
				m_TalkNameEdt.SetWindowText ( NULL ) ;
		}
	}
	else if ( nDir == SB_RIGHT )
	{
		if ( m_nTargetAmount > 0 )
		{
			if ( m_nCurTargetIndex == m_nTargetAmount - 1)
				m_nCurTargetIndex = 0 ;
			else
				m_nCurTargetIndex++ ;

			CDlgTalk::SetTalkMode ( m_uTargetTalkMode[m_nCurTargetIndex] ) ;
			if ( m_strTargetName[m_nCurTargetIndex][0] != NULL )
				m_TalkNameEdt.SetWindowText ( m_strTargetName[m_nCurTargetIndex] ) ;
			else
				m_TalkNameEdt.SetWindowText ( NULL ) ;
		}
	}
}

void CDlgTalk::CycleTalkChannel(int nDir)
{
	if ( nDir == SB_TOP )
	{
		UINT uMode = m_uTalkMode;
		if ( m_uTalkMode == G_CLICK )
			uMode = GM_CLICK ;
		else
			uMode-- ;
		CDlgTalk::SetTalkMode ( uMode ) ;
	}
	else if ( nDir == SB_BOTTOM )
	{
		UINT uMode = m_uTalkMode;
		if ( m_uTalkMode == GM_CLICK )
			uMode = G_CLICK ;
		else
			uMode++ ;
		CDlgTalk::SetTalkMode ( uMode ) ;
	}
}

void CDlgTalk::OnTalkBtnAction()
{
	if ( m_DlgTalkAct.m_bShow )
		m_DlgTalkAct.ShowWindow ( SW_HIDE ) ;
	else
	{
		m_DlgTalkAct.ShowWindow ( SW_SHOW ) ;
		CDlgTalk::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_TALKACT_SHOW ) ;
	}
}

void CDlgTalk::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint)
{
	m_rect.SetRect ( x, y, x + nWidth, y + nHeight ) ;
	CDialog::MoveWindow ( x, y, nWidth, nHeight, bRepaint ) ;
}
