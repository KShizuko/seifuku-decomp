#include "StringInfoView.h"
#include "ND_BITMAP.h"

CStringInfoView::CStringInfoView()
{
	
}

CStringInfoView::~CStringInfoView()
{
	
}

CStringInfoView* CStringInfoView::CreateNew(char* pszStr, CMyPos posShow, DWORD dwColor)
{
	CStringInfoView* pView = new CStringInfoView;
	MYASSERT(pView);
	if(pView->Create(pszStr, posShow, dwColor))
		return pView;
	SAFE_DELETE(pView);
	return NULL;
}
void CStringInfoView::Show()
{
	CMyBitmap::ShowString(m_posShow.x, m_posShow.y, m_dwColor, m_szStr);
}

BOOL CStringInfoView::Create(char* pszStr, CMyPos posShow, DWORD dwColor)
{
	if(!pszStr || strlen(pszStr) >= _MAX_STRING)
		return false;
	strcpy(m_szStr, pszStr);
	m_posShow.x	= 	posShow.x;
	m_posShow.y	= 	posShow.y;
	m_dwColor = dwColor;
	return true;
}
