#include "stdafx.h"
#include "MyShell.h"
#include "DlgDelRole.h"
#include "GameDataSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgDelRole::CDlgDelRole(CWnd* pParent )
	: CDialog(CDlgDelRole::IDD, pParent)
{
}


void CDlgDelRole::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DELROLE_STA_TEXT, m_StaText);
	DDX_Control(pDX, IDC_DELROLE_STA_TITLE, m_StaTitle);
	DDX_Control(pDX, IDC_DELROLE_EDT, m_Edt);
}


BEGIN_MESSAGE_MAP(CDlgDelRole, CDialog)
	ON_BN_CLICKED(IDC_DELROLE_BTN_CANCEL, OnDelroleBtnCancel)
	ON_BN_CLICKED(IDC_DELROLE_BTN_OK, OnDelroleBtnOk)
END_MESSAGE_MAP()

BOOL CDlgDelRole::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_StaTitle.SetWindowText ( GameDataSetQuery()->GetStr ( 10043 ) ) ;
	m_StaText.SetWindowText ( GameDataSetQuery()->GetStr ( 10476 ) ) ;

	return TRUE;
}

void CDlgDelRole::OnDelroleBtnOk()
{
	m_Edt.GetWindowText ( m_strCode ) ;
	if ( ! m_strCode.IsEmpty () )
		EndDialog ( IDOK ) ;
}

void CDlgDelRole::OnDelroleBtnCancel()
{
	EndDialog ( 0 ) ;
}
