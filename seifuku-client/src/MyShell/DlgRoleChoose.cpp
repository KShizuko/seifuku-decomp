#include "stdafx.h"
#include "MyShell.h"
#include "DlgRoleChoose.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef _SOUND_DISABLE
#endif

CDlgRoleChoose::CDlgRoleChoose(CWnd* pParent )
	: CDialog(CDlgRoleChoose::IDD, pParent)
{
	m_bLoad = false ;
	m_bShow = false ;
	m_Hbr = ::CreateSolidBrush ( 0x2132B ) ;
	m_btRoleType = 0 ;
	m_b1024 = false ;
}


void CDlgRoleChoose::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROLECHOOSE_MIMG1, m_RoleChooseMImg1 ) ;
	DDX_Control(pDX, IDC_ROLECHOOSE_MIMG2, m_RoleChooseMImg2 ) ;
	DDX_Control(pDX, IDC_ROLECHOOSE_MIMG3, m_RoleChooseMImg3 ) ;
	DDX_Control(pDX, IDC_ROLECHOOSE_MIMG4, m_RoleChooseMImg4 ) ;
	DDX_Control(pDX, IDC_ROLECHOOSE_WIMG1, m_RoleChooseWImg1 ) ;
	DDX_Control(pDX, IDC_ROLECHOOSE_WIMG2, m_RoleChooseWImg2 ) ;
	DDX_Control(pDX, IDC_ROLECHOOSE_WIMG3, m_RoleChooseWImg3 ) ;
	DDX_Control(pDX, IDC_ROLECHOOSE_WIMG4, m_RoleChooseWImg4 ) ;
	DDX_Control(pDX, IDC_ROLECHOOSE_STA_DATA, m_RoleChooseDataSta ) ;
	DDX_Control(pDX, IDC_ROLECHOOSE_BTN_RETURN, m_RoleChooseReturnBtn);
	DDX_Control(pDX, IDC_ROLECHOOSE_BTN_OK, m_RoleChooseOkBtn);
	DDX_Control(pDX, IDC_BTN_HELP, m_HelpBtn);
}


BEGIN_MESSAGE_MAP(CDlgRoleChoose, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_ROLECHOOSE_BTN_RETURN, OnRolechooseBtnReturn)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBtnHelp)
	ON_BN_CLICKED(IDC_ROLECHOOSE_MIMG1, OnRolechooseMimg1)
	ON_BN_CLICKED(IDC_ROLECHOOSE_MIMG2, OnRolechooseMimg2)
	ON_BN_CLICKED(IDC_ROLECHOOSE_MIMG3, OnRolechooseMimg3)
	ON_BN_CLICKED(IDC_ROLECHOOSE_MIMG4, OnRolechooseMimg4)
	ON_BN_CLICKED(IDC_ROLECHOOSE_WIMG1, OnRolechooseWimg1)
	ON_BN_CLICKED(IDC_ROLECHOOSE_WIMG2, OnRolechooseWimg2)
	ON_BN_CLICKED(IDC_ROLECHOOSE_WIMG3, OnRolechooseWimg3)
	ON_BN_CLICKED(IDC_ROLECHOOSE_WIMG4, OnRolechooseWimg4)
	ON_BN_CLICKED(IDC_ROLECHOOSE_BTN_OK, OnRolechooseBtnOk)
END_MESSAGE_MAP()

LRESULT CDlgRoleChoose::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_SHOWWINDOW )
	{
		m_bShow = wParam ;
	}
	else if ( message == WM_COMMAND )
	{
		if ( wParam == 1 )
		{
			CDlgRoleChoose::OnRolechooseBtnOk () ;
			return true ;
		}
		else if ( wParam == 2 )
		{
			CDlgRoleChoose::OnRolechooseBtnReturn () ;
			return true ;
		}
	}
	else if ( message == WM_DESTROY )
	{
		::DeleteObject ( m_Hbr ) ;
	}
	else if ( message == WM_ERASEBKGND )
		return true ;
	else if ( message == WM_NCPAINT )
		return false ;
	else if ( message == WM_PAINT )
	{
		PAINTSTRUCT ps ;
		::BeginPaint ( m_hWnd, &ps ) ;
		::EndPaint ( m_hWnd, &ps ) ;
		return false ;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDlgRoleChoose::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgRoleChoose::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	if ( ! ( m_RoleChooseMImg1.Init ( 0, 0 ) ) )
		return false ;
	if ( ! ( m_RoleChooseMImg2.Init ( 0, 0 ) ) )
		return false ;
	if ( ! ( m_RoleChooseMImg3.Init ( 0, 0 ) ) )
		return false ;
	if ( ! ( m_RoleChooseMImg4.Init ( 0, 0 ) ) )
		return false ;
	if ( ! ( m_RoleChooseWImg1.Init ( 0, 0 ) ) )
		return false ;
	if ( ! ( m_RoleChooseWImg2.Init ( 0, 0 ) ) )
		return false ;
	if ( ! ( m_RoleChooseWImg3.Init ( 0, 0 ) ) )
		return false ;
	if ( ! ( m_RoleChooseWImg4.Init ( 0, 0 ) ) )
		return false ;

	if ( ! ( m_RoleChooseDataSta.Init ( 0, 0, 0, NULL, "NULL" ) ) )
		return false ;
	m_RoleChooseDataSta.SetFontColor ( 0xffffff ) ;

	if ( ! ( m_RoleChooseOkBtn.Init ( 0, 0, "Button20" ) ) )
		return false ;
	if ( ! ( m_RoleChooseReturnBtn.Init ( 0, 0, "Button14" ) ) )
		return false ;
	if ( ! ( m_HelpBtn.Init ( 0, 0, "Button15" ) ) )
		return false ;

	m_bLoad = true ;
	return TRUE;
}

void CDlgRoleChoose::Show()
{
	if ( m_bLoad && m_bShow )
	{
		IAni* ShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
													  "Dialog2",
													  EXIGENCE_IMMEDIATE ) ;
		if ( ShowAni != NULL )
		{
			if ( !m_b1024 )
				ShowAni->Show ( 0, 0, 0 );
			else
				ShowAni->Show ( 1, 0, 0 );
		}
		else
			return ;

		CMyPos pos ;
		if ( !m_b1024 )
		{
			pos.x = 400 ;
			pos.y = 350 ;
		}
		else
		{
			pos.x = 512 ;
			pos.y = 420 ;
		}
		m_ShowRole.Show ( m_btRoleType, pos, 160 ) ;

		m_RoleChooseMImg1.Show ( 0, 0 ) ;
		m_RoleChooseMImg2.Show ( 0, 0 ) ;
		m_RoleChooseMImg3.Show ( 0, 0 ) ;
		m_RoleChooseMImg4.Show ( 0, 0 ) ;
		m_RoleChooseWImg1.Show ( 0, 0 ) ;
		m_RoleChooseWImg2.Show ( 0, 0 ) ;
		m_RoleChooseWImg3.Show ( 0, 0 ) ;
		m_RoleChooseWImg4.Show ( 0, 0 ) ;

		switch ( m_btRoleType % 4 )
		{
		case 1:
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10022 ) ) ;
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y + 20,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10023 ) ) ;
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y + 40,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10024 ) ) ;
			break ;
		case 0:
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10025 ) ) ;
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y + 20,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10026 ) ) ;
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y + 40,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10027 ) ) ;
			break ;
		case 2:
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10028 ) ) ;
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y + 20,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10029 ) ) ;
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y + 40,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10030 ) ) ;
			break ;
		case 3:
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10031 ) ) ;
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y + 20,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10032 ) ) ;
			CMyBitmap::ShowString ( m_RoleChooseDataSta.m_IntPoint.x,
									m_RoleChooseDataSta.m_IntPoint.y + 40,
									0xffffff,
									GameDataSetQuery()->GetStr ( 10033 ) ) ;
			break ;
		}

		m_RoleChooseOkBtn.Show ( 0, 0 ) ;
		m_RoleChooseReturnBtn.Show ( 0, 0 ) ;
		m_HelpBtn.Show ( 0, 0 ) ;
	}
}

void CDlgRoleChoose::OnRolechooseBtnReturn()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;

	CDlgRoleChoose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_EXIT ) ;
}

void CDlgRoleChoose::CreateRole(BYTE btRoleType)
{
	LPARAM lParam = btRoleType ;
	CDlgRoleChoose::ShowWindow (SW_HIDE ) ;
	CDlgRoleChoose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_ROLECHOOSE_CHOOSE, lParam ) ;
}

void CDlgRoleChoose::OnRolechooseBtnOk()
{
	DXPlaySound ( "Sound/BtnClick.wav" ) ;

	CreateRole ( m_btRoleType ) ;
	return ;
}

void CDlgRoleChoose::OnBtnHelp()
{
	CDlgRoleChoose::GetParent ()->PostMessage ( WM_MY_MESSAGE, ON_HELPDLG_SHOW, DLG_ROLECHOOSE ) ;
}

void CDlgRoleChoose::OnRolechooseMimg1()
{
	m_ShowRole.Blaze () ;
	m_btRoleType = 0 ;
}

void CDlgRoleChoose::OnRolechooseMimg2()
{
	m_ShowRole.Blaze () ;
	m_btRoleType = 1 ;
}

void CDlgRoleChoose::OnRolechooseMimg3()
{
	m_ShowRole.Blaze () ;
	m_btRoleType = 2 ;
}

void CDlgRoleChoose::OnRolechooseMimg4()
{
	m_ShowRole.Blaze () ;
	m_btRoleType = 3 ;
}

void CDlgRoleChoose::OnRolechooseWimg1()
{
	m_ShowRole.Blaze () ;
	m_btRoleType = 4 ;
}

void CDlgRoleChoose::OnRolechooseWimg2()
{
	m_ShowRole.Blaze () ;
	m_btRoleType = 5 ;
}

void CDlgRoleChoose::OnRolechooseWimg3()
{
	m_ShowRole.Blaze () ;
	m_btRoleType = 6 ;
}

void CDlgRoleChoose::OnRolechooseWimg4()
{
	m_ShowRole.Blaze () ;
	m_btRoleType = 7 ;
}

void CDlgRoleChoose::ReInit()
{
	m_RoleChooseMImg1.Init ( 0, 0 ) ;
	m_RoleChooseMImg2.Init ( 0, 0 ) ;
	m_RoleChooseMImg3.Init ( 0, 0 ) ;
	m_RoleChooseMImg4.Init ( 0, 0 ) ;
	m_RoleChooseWImg1.Init ( 0, 0 ) ;
	m_RoleChooseWImg2.Init ( 0, 0 ) ;
	m_RoleChooseWImg3.Init ( 0, 0 ) ;
	m_RoleChooseWImg4.Init ( 0, 0 ) ;
	m_RoleChooseDataSta.Init ( 0, 0, 0, NULL, "NULL" ) ;
	m_RoleChooseOkBtn.Init ( 0, 0, "Button20" ) ;
	m_RoleChooseReturnBtn.Init ( 0, 0, "Button14" ) ;
	m_HelpBtn.Init ( 0, 0, "Button15" ) ;
	m_b1024 = true ;
}
