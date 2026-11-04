#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define		MY_QUERY_ROLE		1
#define		MY_QUERY_WEAPON		2
#define		MY_QUERY_MISSION	3
#define		MY_QUERY_SKILL		4
#define		MY_QUERY_TITLE		5

#ifdef _SOUND_DISABLE
	#define DXPlaySound
#endif

CDlgQuery::CDlgQuery(CWnd* pParent )
	: CDialog(CDlgQuery::IDD, pParent)
{
	m_bLoad = false ;
	m_bShow = false ;
	m_Hbr = CreateSolidBrush ( 0x2132B ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_dwLastShowTime = 0 ;
	m_fFrameStep = 0.0f ;
	m_nFrameMode = 0 ;
	m_nPntX = 0 ;
	m_btCurChildDlgID = 1 ;
}


void CDlgQuery::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_QUERY_SKILL, m_ChkSkill);
	DDX_Control(pDX, IDC_QUERY_CLOSEB, m_QueryCloseBBtn);
	DDX_Control(pDX, IDC_QUERY_CLOSE, m_QueryCloseBtn);
	DDX_Control(pDX, IDC_QUERY_ROLE, m_ChkRole);
	DDX_Control(pDX, IDC_QUERY_WEAPON, m_ChkWeapon);
	DDX_Control(pDX, IDC_QUERY_MISSION, m_ChkMission);
	DDX_Control(pDX, IDC_BTN_HELP, m_BtnHelp);
}

BEGIN_MESSAGE_MAP(CDlgQuery, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_QUERY_CLOSE, OnQueryClose)
	ON_BN_CLICKED(IDC_QUERY_MISSION, OnChkMission)
	ON_BN_CLICKED(IDC_QUERY_WEAPON, OnChkWeapon)
	ON_BN_CLICKED(IDC_QUERY_FACTION, OnQueryFaction)
	ON_BN_CLICKED(IDC_QUERY_ROLE, OnChkRole)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_QUERY_SKILL, OnChkSkill)
	ON_BN_CLICKED(IDC_QUERY_CLOSEB, OnQueryCloseb)
END_MESSAGE_MAP()

void CDlgQuery::Show()
{
	if ( m_bLoad && m_bShow )
	{
		if ( m_nFrameMode == 1 )
		{
			if ( m_dwLastShowTime == 0 )
			{
				m_dwLastShowTime = TimeGet() ;
				m_Pnt.x = m_nPntX ;
			}
			else
			{
				if ( TimeGet() - m_dwLastShowTime <= 300 )
				{
					m_Pnt.x = m_nPntX ;
					m_Pnt.x += ( TimeGet() - m_dwLastShowTime ) / 15 * m_fFrameStep ;
				}
				else
				{
					m_Pnt.x = m_nPntX + m_Width ;
					m_nFrameMode = 2 ;
					CDlgQuery::WindowProc ( WM_SHOWWINDOW, true, NULL ) ;
				}
			}
		}
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
												  "Dialog6",
												  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			ShowAni->Show ( 0, m_Pnt.x, m_Pnt.y ) ;
		}
		else
			return ;

		m_QueryCloseBtn.Show( m_Pnt.x, m_Pnt.y ) ;
		m_QueryCloseBBtn.Show( m_Pnt.x, m_Pnt.y ) ;
		m_ChkRole.Show(m_Pnt.x, m_Pnt.y);
		m_ChkWeapon.Show(m_Pnt.x, m_Pnt.y + 1);
		m_ChkMission.Show(m_Pnt.x, m_Pnt.y);
		m_ChkSkill.Show(m_Pnt.x, m_Pnt.y);
		m_BtnHelp.Show( m_Pnt.x, m_Pnt.y ) ;

		ShowChildDlg () ;
	}
}

HBRUSH CDlgQuery::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgQuery::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgQuery::GetWindowRect ( rect ) ;
	m_Height = rect.Height() ;
	m_Width = rect.Width() ;
	m_fFrameStep = ( float ) rect.Width () / 20 ;

	if ( !m_QueryCloseBtn.Init ( rect.left, rect.top, "Button5" ) )
		return false ;
	if ( !m_QueryCloseBBtn.Init ( rect.left, rect.top, "Button3" ) )
		return false ;

	if ( !m_ChkRole.Init(rect.left,rect.top,"Check61",2) )
		return false ;
	if ( !m_ChkWeapon.Init(rect.left,rect.top,"Check63",2) )
		return false ;
	if ( !m_ChkMission.Init(rect.left,rect.top,"Check64",2) )
		return false ;
	if ( !m_ChkSkill.Init(rect.left,rect.top,"Check65",2) )
		return false ;

	if ( !m_BtnHelp.Init ( rect.left, rect.top, "Button4" ) )
		return false ;

	CRect childRect ;
	m_DlgRole.Create(IDD_DIALOG_QUERY_ROLE,this);
	m_DlgRole.GetWindowRect ( childRect ) ;
	m_DlgRole.MoveWindow ( m_Width - childRect.Width () - 16, 55, childRect.Width (), childRect.Height (), true ) ;

	m_DlgWeapon.Create(IDD_DIALOG_QUERY_WEAPON,this);
	m_DlgWeapon.GetWindowRect ( childRect ) ;
	m_DlgWeapon.MoveWindow ( m_Width - childRect.Width () - 16, 55, childRect.Width (), childRect.Height (), true ) ;

	m_DlgMission.Create(IDD_DIALOG_QUERY_MISSION,this);
	m_DlgMission.GetWindowRect ( childRect ) ;
	m_DlgMission.MoveWindow ( m_Width - childRect.Width () - 16, 55, childRect.Width (), childRect.Height (), true ) ;

	m_DlgSkill.Create(IDD_DIALOG_QUERY_SKILL,this);
	m_DlgSkill.GetWindowRect ( childRect ) ;
	m_DlgSkill.MoveWindow ( m_Width - childRect.Width () - 16, 55, childRect.Width (), childRect.Height (), true ) ;

	m_bLoad = true ;
	m_ChkRole.SetCheckValue ( 1 ) ;

	return TRUE;
}

void CDlgQuery::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_nPntX = x ;
	m_Pnt.y = y ;
}

void CDlgQuery::OnQueryClose()
{
	DXPlaySound( "Sound/BtnClick.wav" ) ;
	CDlgQuery::ShowWindow( SW_HIDE ) ;
}



LRESULT CDlgQuery::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		{
			m_bShow = wParam ;
			if ( wParam == false )
			{
				DXPlaySound( "Sound/DlgClose.wav" ) ;
				CDlgQuery::HideCurDlg( m_btCurChildDlgID ) ;
			}
			else
			{
				if ( m_nFrameMode == 0 )
				{
					DXPlaySound("Sound/DlgOpen.wav");
					m_dwLastShowTime = 0 ;
					m_nFrameMode = 1 ;
					CDlgQuery::MoveWindow ( -m_Width, m_Pnt.y, m_Width, m_Height, true ) ;
				}
				else if ( m_nFrameMode == 2 )
				{
					CDlgQuery::MoveWindow ( 0, m_Pnt.y, m_Width, m_Height, true ) ;
					m_nFrameMode = 0 ;
					CDlgQuery::ShowCurDlg( m_btCurChildDlgID ) ;
				}
			}
		}
		break ;
	case WM_DESTROY:
		DeleteObject ( m_Hbr ) ;
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
		{
			if ( wParam == 1 )
				return true ;
			else if ( wParam == 2 )
			{
				CDlgQuery::OnQueryClose () ;
				return true ;
			}
		}
		break ;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDlgQuery::ShowCurDlg(BYTE btCurDlgID)
{
	switch(btCurDlgID)
	{
	case 1:
		m_DlgRole.ShowWindow ( SW_SHOW ) ;
		break;
	case 2:
		m_DlgWeapon.ShowWindow ( SW_SHOW ) ;
		break;
	case 3:
		m_DlgMission.ShowWindow ( SW_SHOW ) ;
		break;
	case 4:
		m_DlgSkill.ShowWindow ( SW_SHOW ) ;
		break;
	default:
		break;
	}
}

void CDlgQuery::HideCurDlg(BYTE btCurDlgID)
{
	switch(btCurDlgID)
	{
	case 1:
		m_DlgRole.ShowWindow ( SW_HIDE ) ;
		break;
	case 2:
		m_DlgWeapon.ShowWindow ( SW_HIDE ) ;
		break;
	case 3:
		m_DlgMission.ShowWindow ( SW_HIDE ) ;
		break;
	case 4:
		m_DlgSkill.ShowWindow ( SW_HIDE ) ;
		break;
	default:
		break;
	}
}

void CDlgQuery::ShowChildDlg()
{
	switch( m_btCurChildDlgID )
	{
	case 1:
		m_DlgRole.Show(m_Pnt.x,m_Pnt.y);
		break;
	case 2:
		m_DlgWeapon.Show(m_Pnt.x,m_Pnt.y);
		break;
	case 3:
		m_DlgMission.Show(m_Pnt.x,m_Pnt.y);
		break;
	case 4:
		m_DlgSkill.Show(m_Pnt.x,m_Pnt.y);
		break;
	default:
		break;
	}
}

void CDlgQuery::PopupCurChk(BYTE btCurDlgID)
{
	switch(btCurDlgID)
	{
	case 1:
		m_ChkRole.SetCheckValue(0);
		break;
	case 2:
		m_ChkWeapon.SetCheckValue(0);
		break;
	case 3:
		m_ChkMission.SetCheckValue(0);
		break;
	case 4:
		m_ChkSkill.SetCheckValue(0);
		break;
	default:
		break;
	}
}

void CDlgQuery::OnChkMission()
{
	DXPlaySound( "Sound/BtnClick.wav" ) ;
	if ( m_btCurChildDlgID != MY_QUERY_MISSION )
	{
		PopupCurChk(m_btCurChildDlgID);
		HideCurDlg(m_btCurChildDlgID);
		m_btCurChildDlgID = MY_QUERY_MISSION;
		ShowCurDlg(m_btCurChildDlgID);
	}
	else
		m_ChkMission.SetCheckValue(1);
}

void CDlgQuery::OnChkWeapon()
{
	DXPlaySound( "Sound/BtnClick.wav" ) ;
	if ( m_btCurChildDlgID != MY_QUERY_WEAPON )
	{
		PopupCurChk(m_btCurChildDlgID);
		HideCurDlg(m_btCurChildDlgID);
		m_btCurChildDlgID = MY_QUERY_WEAPON;
		ShowCurDlg(m_btCurChildDlgID);
	}
	else
		m_ChkWeapon.SetCheckValue(1);
}

void CDlgQuery::OnQueryFaction()
{
}

void CDlgQuery::OnChkRole()
{
	DXPlaySound( "Sound/BtnClick.wav" ) ;
	if ( m_btCurChildDlgID != MY_QUERY_ROLE )
	{
		PopupCurChk(m_btCurChildDlgID);
		HideCurDlg(m_btCurChildDlgID);
		m_btCurChildDlgID = MY_QUERY_ROLE;
		ShowCurDlg(m_btCurChildDlgID);
	}
	else
		m_ChkRole.SetCheckValue(1);
}

void CDlgQuery::OnBtnHelp()
{
	switch( m_btCurChildDlgID )
	{
	case 1:
		CDlgQuery::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_QUERY_ROLE ) ;
		break;
	case 2:
		CDlgQuery::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_QUERY_WEAPON ) ;
		break;
	case 3:
		CDlgQuery::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_QUERY_MISSION ) ;
		break;
	case 4:
		CDlgQuery::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_QUERY_SKILL ) ;
		break;
	default:
		break;
	}
}

void CDlgQuery::OnChkSkill()
{
	DXPlaySound( "Sound/BtnClick.wav" ) ;
	if ( m_btCurChildDlgID != MY_QUERY_SKILL )
	{
		PopupCurChk(m_btCurChildDlgID);
		HideCurDlg(m_btCurChildDlgID);
		m_btCurChildDlgID = MY_QUERY_SKILL;
		ShowCurDlg(m_btCurChildDlgID);
	}
	else
		m_ChkSkill.SetCheckValue(1);
}

void CDlgQuery::OnQueryCloseb()
{
	DXPlaySound( "Sound/BtnClick.wav" ) ;
	CDlgQuery::ShowWindow( SW_HIDE ) ;
}
