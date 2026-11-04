#include "stdafx.h"
#include "MyShell.h"
#include "DlgTask.h"
#include "GameMsg.h"
#include "Hero.h"
#include "ND_Bitmap.h"
#include "GameDataSetProvisional.h"
#include "3DRoleData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgTask::CDlgTask(CWnd* pParent )
	: CDialog(CDlgTask::IDD, pParent)
{
	m_bShow = false ;
	m_bMouseMove = false ;
	m_Hbr = CreateSolidBrush ( 0x000000 ) ;
	m_Pnt = CPoint ( 0, 0 ) ;
	m_nCloseIndex = 0 ;
	m_nTextAmount = 0 ;
	m_nLinkAmount = 0 ;
	m_nEditAmount = 0 ;
	m_nAnswerIndex = 0 ;
	m_nEditIndex = 0 ;
	m_nDlgHeight = 30 ;
}


void CDlgTask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TASK_EDT_ANSWER, m_TaskEdtAnswer);
	DDX_Control(pDX, IDC_TASK_BTN_ANSWER, m_TaskBtnAnswer);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_CloseBtn);
}


BEGIN_MESSAGE_MAP(CDlgTask, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_TASK_BTN_ANSWER, OnTaskBtnAnswer)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
END_MESSAGE_MAP()

LRESULT CDlgTask::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_SHOWWINDOW:
		if ( wParam )
		{
			InitTaskDialog () ;
		}
		else
		{
			ResetTaskDialog() ;
		}
		m_bShow = wParam ;
		break ;
	case WM_COMMAND:
		if ( wParam == 2 )
		{
			return true ;
		}
		else if ( wParam == 1 )
		{
			return true ;
		}
		break ;
	case WM_DESTROY:
		::DeleteObject ( m_Hbr ) ;
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
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

HBRUSH CDlgTask::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode ( TRANSPARENT ) ;
	return m_Hbr ;
}

BOOL CDlgTask::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_objFontScale.Adjust ( this, IDD ) ;

	CRect rect ;
	CDlgTask::GetWindowRect ( rect ) ;

	m_TaskEdtAnswer.Init ( rect.left, rect.top ) ;

	m_TaskBtnAnswer.Init ( rect.left, rect.top, "Button230") ;
	m_CloseBtn.Init ( rect.left, rect.top, "Button5" ) ;

	return TRUE;
}

void CDlgTask::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_Pnt.x = x ;
	m_Pnt.y = y ;
}

void CDlgTask::InitTaskDialog()
{
	m_nPicIndex = -1 ;
	m_TaskEdtAnswer.EnableWindow ( false ) ;
	m_TaskBtnAnswer.EnableWindow ( false ) ;
	int nAmount = g_objHero.GetTaskDialogLineAmount () ;
	if ( nAmount > 0 )
	{
		m_nAnswerAmount = 0 ;
		for ( int j = 0; j < nAmount; j++ )
		{
			TaskDialogLine* pLine = g_objHero.GetTaskDialogLineByIndex ( j ) ;
			if ( pLine->nType == _TASKDIALOG_LINK || pLine->nType == _TASKDIALOG_EDIT )
				m_nAnswerAmount++ ;
		}
		for ( int i = 0; i < nAmount; i++ )
		{
			TaskDialogLine* pLine = g_objHero.GetTaskDialogLineByIndex ( i ) ;
			switch ( pLine->nType )
			{
			case _TASKDIALOG_TEXT:
				if ( pLine->szStr )
				{
					strcpy ( m_TaskText[m_nTextAmount].strText, pLine->szStr ) ;
					this->SetLineIndex ( 0 ) ;
					m_nTextAmount++ ;
				}
				break ;
			case _TASKDIALOG_LINK:
				if ( pLine->szStr )
				{
					if ( m_nAnswerIndex == 0 && m_nDlgHeight < 104 )
						m_nDlgHeight = 104 ;
					m_nAnswerIndex++ ;
					strcpy ( m_TaskLink[m_nLinkAmount].strText, pLine->szStr ) ;
					this->SetLineIndex ( 1 ) ;
					m_TaskLink[m_nLinkAmount].nIndex = pLine->nIndex ;
					m_nLinkAmount++ ;
				}
				break ;
			case _TASKDIALOG_EDIT:
				{
					if ( m_nAnswerIndex == 0 && m_nDlgHeight < 104 )
						m_nDlgHeight = 104 ;
					m_nAnswerIndex++ ;
					m_nEditIndex = m_nAnswerIndex ;
					m_TaskEdtAnswer.EnableWindow ( true ) ;
					m_TaskBtnAnswer.EnableWindow ( true ) ;
					strcpy ( m_TaskEdit.strTitle, pLine->szStr ) ;
					m_TaskEdit.nIndex = pLine->nIndex ;
					m_TaskEdit.dwData = pLine->dwData ;
					this->SetLineIndex ( 2 ) ;
					m_nEditAmount++ ;
				}
				break ;
			case _TASKDIALOG_PIC:
				m_nPicIndex = pLine->dwData ;
				break ;
			case _TASKDIALOG_DEFAULT:
				m_nCloseIndex = pLine->nIndex ;
				break ;
			}
		}
		m_nDlgHeight += 10 ;
		CDlgTask::MoveWindow ( ( CMyBitmap::GetScreenWidth () - 544 ) / 2, 0, 544, m_nDlgHeight, true ) ;
	}
}

void CDlgTask::ResetTaskDialog()
{
	m_nDlgHeight = 30 ;
	m_nCloseIndex = 0 ;
	m_nTextAmount = 0 ;
	m_nLinkAmount = 0 ;
	m_nEditAmount = 0 ;
	m_nAnswerIndex = 0 ;
	m_nEditIndex = 0 ;
	m_TaskEdtAnswer.SetWindowText ( NULL ) ;
}

void CDlgTask::SetLineIndex(BYTE btType)
{
	switch ( btType )
	{
	case 0:
		{
			int	nSingle = 0;
			int nLineNum = 0 ;
			int nLineIndex = 1 ;
			int i ;
			m_TaskText[m_nTextAmount].nLineIndex [0] = 0 ;
			for ( i = 0; i < strlen ( m_TaskText[m_nTextAmount].strText ); i++ )
			{
				if ( CMyBitmap::IsDBCSLeadByte( m_TaskText[m_nTextAmount].strText, i ) )
				{
					nLineNum += 2;
					i++;
				}
				else
				{
					nLineNum++;
					nSingle++;
				}
				if ( nLineNum >= TEXT_LIMIT )
				{
					if ( strcmp( GameDataSetQuery()->GetVersionInfo (), "English" ) == 0 )
					{
						for ( int j = i; j >= 0; j-- )
						{
							if ( m_TaskText[m_nTextAmount].strText[j] == ' ' )
							{
								i -= i - j;
								break;
							}
						}
					}
					if ( nSingle % 2 == 0 )
					{
						nSingle = 0 ;
						nLineNum = 0 ;
						m_TaskText[m_nTextAmount].nLineIndex [nLineIndex] = i + 1 ;
						nLineIndex++ ;
					}
					else
					{
						i -= nLineNum - TEXT_LIMIT;
						m_TaskText[m_nTextAmount].nLineIndex [nLineIndex] = i + 1 - ( nLineNum - TEXT_LIMIT );
						nSingle= 0 ;
						nLineNum = 0 ;
						nLineIndex++ ;
					}
					m_nDlgHeight += 18 ;
				}
			}
			m_TaskText[m_nTextAmount].nLineNum = nLineIndex ;
			if ( nSingle % 2 == 0 )
			{
				m_TaskText[m_nTextAmount].nLineIndex [nLineIndex] = i + 1 ;
			}
			else
			{
				m_TaskText[m_nTextAmount].nLineIndex [nLineIndex] = i ;
			}
			m_nDlgHeight += 18 ;
		}
		break ;
	case 1:
		{
			int	nSingle = 0;
			int nLineNum = 0 ;
			int nLineIndex = 1 ;
			int i ;
			m_TaskLink[m_nLinkAmount].nLineIndex [0] = 0 ;
			int nLastHeight = m_nDlgHeight ;
			for ( i = 0; i < strlen ( m_TaskLink[m_nLinkAmount].strText ); i++ )
			{
				if ( CMyBitmap::IsDBCSLeadByte( m_TaskLink[m_nLinkAmount].strText, i ) )
				{
					nLineNum += 2;
					i++;
				}
				else
				{
					nLineNum++;
					nSingle++;
				}
				if ( nLineNum >= LINK_LIMIT )
				{
					if ( strcmp( GameDataSetQuery()->GetVersionInfo (), "English" ) == 0 )
					{
						for ( int j = i; j >= 0; j-- )
						{
							if ( m_TaskLink[m_nLinkAmount].strText[j] == ' ' )
							{
								i -= i - j;
								break;
							}
						}
					}
					if ( nLineNum != strlen ( m_TaskLink[m_nLinkAmount].strText ) )
						m_nDlgHeight += 18 ;
					if ( nSingle % 2 == 0 )
					{
						nSingle= 0 ;
						nLineNum = 0 ;
						m_TaskLink[m_nLinkAmount].nLineIndex [nLineIndex] = i + 1 ;
						nLineIndex++ ;
					}
					else
					{
						i -= nLineNum - TEXT_LIMIT;
						m_TaskLink[m_nLinkAmount].nLineIndex [nLineIndex] = i + 1 - ( nLineNum - TEXT_LIMIT );
						nSingle = 0 ;
						nLineNum = 0 ;
						nLineIndex++ ;
					}
				}
			}
			m_TaskLink[m_nLinkAmount].nLineNum = nLineIndex ;
			if ( nSingle % 2 == 0 )
			{
				m_TaskLink[m_nLinkAmount].nLineIndex [nLineIndex] = i + 1 ;
			}
			else
			{
				m_TaskLink[m_nLinkAmount].nLineIndex [nLineIndex] = i ;
			}
			m_TaskLink[m_nLinkAmount].nLineNum = nLineIndex ;
			if ( m_nAnswerIndex % 2 == 1 )
			{
				m_TaskLink[m_nLinkAmount].Rect = CRect( 20, nLastHeight, 20 + LINK_LIMIT * 7, m_nDlgHeight + 18 ) ;
				if ( m_nAnswerIndex == m_nAnswerAmount )
					m_nDlgHeight += 23 ;
				else
					m_nDlgHeight = nLastHeight ;
			}
			else
			{
				m_TaskLink[m_nLinkAmount].Rect = CRect( 282, nLastHeight, 282 + LINK_LIMIT * 7, m_nDlgHeight + 18 ) ;
				if ( m_nAnswerIndex == m_nAnswerAmount && m_nEditAmount > 0 && m_nEditIndex == m_nAnswerIndex - 1 )
					m_nDlgHeight += 44 ;
				else
					m_nDlgHeight += 23 ;
			}
		}
		break ;
	case 2:
		{
			CRect rect ;
			m_TaskEdtAnswer.GetWindowRect ( rect ) ;
			if ( m_nAnswerIndex % 2 == 1 )
			{
				m_TaskEdtAnswer.ReSize ( 40,
										 m_nDlgHeight + 18,
										 rect.Width (),
										 rect.Height (),
										 false ) ;
				m_TaskEdtAnswer.m_EditRect.MoveWindow ( 40, m_nDlgHeight + 18, 180, 18, false ) ;
				m_TaskBtnAnswer.MoveWindow ( rect.Width () + 20,
											 m_nDlgHeight + 18,
											 20,
											 20,
											 false ) ;
			}
			else
			{
				m_TaskEdtAnswer.ReSize ( 282,
										 m_nDlgHeight + 18,
										 rect.Width (),
										 rect.Height (),
										 false ) ;
				m_TaskEdtAnswer.m_EditRect.MoveWindow ( 282, m_nDlgHeight + 18, 180, 18, false ) ;
				m_TaskBtnAnswer.MoveWindow ( rect.Width () + 282,
											 m_nDlgHeight + 18,
											 20,
											 20,
											 false ) ;
			}
			CDlgTask::GetWindowRect ( rect ) ;
			m_TaskEdtAnswer.ReInit ( rect.left, rect.top ) ;
			m_TaskEdtAnswer.SetEditBkID ( "Edit230" ) ;
			m_TaskEdtAnswer.SetLimitText ( m_TaskEdit.dwData ) ;
			if ( m_TaskEdit.dwData & 0xFF00 )
			{
				m_TaskEdtAnswer.SetPasswordType ( true ) ;
				m_TaskEdtAnswer.SetPasswordChar ( '*' ) ;
				m_TaskEdtAnswer.SetLimitText ( m_TaskEdit.dwData & 0xFF ) ;
			}
			else
			{
				m_TaskEdtAnswer.SetPasswordType ( false ) ;
				m_TaskEdtAnswer.SetPasswordChar ( 0 ) ;
			}
			m_TaskBtnAnswer.Init ( rect.left, rect.top, "Button230" ) ;
			if ( m_nAnswerIndex % 2 == 0 )
				m_nDlgHeight += 44 ;
			else if ( m_nAnswerIndex == m_nAnswerAmount )
				m_nDlgHeight += 44 ;
		}
		break ;
	}
}

void CDlgTask::Show()
{
	int ShowHeight = 30 ;
	int j = 0;
	int i ;

	IAni* pShowAni = GameDataSetQuery()->GetDataAni ( ( char * )g_strControlAni,
												   "Dialog23",
												   EXIGENCE_IMMEDIATE ) ;
	if ( pShowAni != NULL )
	{
			CRect DlgRect = CRect ( 0, 0, 544, 200 ) ;
			pShowAni->ShowEx( 0,
							  m_Pnt.x,
							  m_Pnt.y,
							  DlgRect,
							  DlgRect.Width(),
							  m_nDlgHeight ) ;
	}
	else
		return ;

	if ( m_nPicIndex != -1 )
	{
		IAni* FaceAni = RoleDataQuery()->GetNpcFaceIcon ( m_nPicIndex,
														  EXIGENCE_IMMEDIATE ) ;
		if ( FaceAni != NULL )
		{
			m_PicSize = FaceAni->GetSize ( 0 ) ;
			FaceAni->ShowEx ( 0,
							  m_Pnt.x + 10,
							  m_Pnt.y + 30,
							  CRect ( 0, 0, m_PicSize.iWidth, m_PicSize.iHeight ),
							  HEADPIC_WIDTH,
							  HEADPIC_HEIGHT ) ;
		}
	}

	for ( i = 0; i < m_nTextAmount; i++ )
	{
		for ( j = 0; j < m_TaskText[i].nLineNum; j++ )
		{
			char tempChar[TEXT_LIMIT + 1] = "";
			strncpy ( tempChar,
					  m_TaskText[i].strText + m_TaskText[i].nLineIndex[j],
					  m_TaskText[i].nLineIndex[j + 1] - m_TaskText[i].nLineIndex[j] ) ;
			if ( m_TaskText[i].nLineIndex[j + 1] - m_TaskText[i].nLineIndex[j] == TEXT_LIMIT )
				tempChar[TEXT_LIMIT] = NULL ;
			else
				tempChar[m_TaskText[i].nLineIndex[j + 1] - m_TaskText[i].nLineIndex[j]] = NULL ;
			CMyBitmap::ShowString ( m_Pnt.x + 84,
									m_Pnt.y + ShowHeight,
									0xffff00,
									tempChar ) ;
			ShowHeight += 18 ;
		}
	}

	for ( i = 0; i < m_nLinkAmount; i++ )
	{
		ShowHeight = 0 ;
		for ( j = 0; j < m_TaskLink[i].nLineNum; j++ )
		{
			char tempChar[LINK_LIMIT + 1] = "";
			strncpy ( tempChar,
					  m_TaskLink[i].strText + m_TaskLink[i].nLineIndex[j],
					  m_TaskLink[i].nLineIndex[j + 1] - m_TaskLink[i].nLineIndex[j] ) ;
			if ( m_TaskLink[i].nLineIndex[j + 1] - m_TaskLink[i].nLineIndex[j] == LINK_LIMIT )
				tempChar[LINK_LIMIT] = NULL ;
			else
				tempChar[m_TaskLink[i].nLineIndex[j + 1] - m_TaskLink[i].nLineIndex[j]] = NULL ;
			if ( m_btCurLink == i && m_bMouseMove )
				CMyBitmap::ShowString ( m_Pnt.x + m_TaskLink[i].Rect.left,
										m_Pnt.y + m_TaskLink[i].Rect.top + ShowHeight,
										0xff0000,
										tempChar ) ;
			else
				CMyBitmap::ShowString ( m_Pnt.x + m_TaskLink[i].Rect.left,
										m_Pnt.y + m_TaskLink[i].Rect.top + ShowHeight,
										0xffffff,
										tempChar ) ;
			ShowHeight += 18 * ( j + 1 ) ;
		}
	}

	if ( m_nEditAmount > 0 )
	{
		if ( m_TaskEdit.strTitle )
		{
			CRect rect, prect ;
			m_TaskEdtAnswer.GetWindowRect ( rect ) ;
			CDlgTask::GetWindowRect ( prect ) ;
			CMyBitmap::ShowString ( m_Pnt.x + ( rect.left - prect.left ),
									m_Pnt.y + ( rect.top - prect.top ) - 18,
									0xffff00,
									m_TaskEdit.strTitle ) ;
		}
		m_TaskEdtAnswer.Show ( m_Pnt.x, m_Pnt.y ) ;
		m_TaskBtnAnswer.Show ( m_Pnt.x, m_Pnt.y ) ;
	}

	m_CloseBtn.Show ( m_Pnt.x, m_Pnt.y ) ;
}

void CDlgTask::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_bMouseMove )
	{
		if ( !m_TaskLink[m_btCurLink].Rect.PtInRect ( point ) )
			m_bMouseMove = false ;
	}
	else
	{
		for ( int i = 0; i < m_nLinkAmount; i++ )
		{
			if ( m_TaskLink[i].Rect.PtInRect ( point ) )
			{
				m_bMouseMove = true ;
				m_btCurLink = i ;
				return ;
			}
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CDlgTask::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ( m_bMouseMove )
	{
		g_objHero.AnswerTaskDialog ( m_TaskLink[m_btCurLink].nIndex, NULL ) ;
		CDlgTask::ShowWindow ( SW_HIDE ) ;
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgTask::OnTaskBtnAnswer()
{
	if ( m_TaskEdtAnswer.GetWindowTextLength () > 0 )
	{
		char strAnswer[32] ;
		m_TaskEdtAnswer.GetWindowText ( strAnswer, 32) ;
		g_objHero.AnswerTaskDialog ( m_TaskEdit.nIndex, strAnswer ) ;
		CDlgTask::ShowWindow ( SW_HIDE ) ;
	}
}

void CDlgTask::OnBtnClose()
{
	g_objHero.AnswerTaskDialog ( m_nCloseIndex, NULL ) ;
	CDlgTask::ShowWindow ( SW_HIDE ) ;
}
