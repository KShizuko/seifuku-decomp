#if !defined(AFX_MYLIST_H__A7857E9D_70A8_4A65_86E6_93923883A587__INCLUDED_)
#define AFX_MYLIST_H__A7857E9D_70A8_4A65_86E6_93923883A587__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
#include "MyScrollButton.h"
const int SIMPLE_LIST = 1 ;
const int COMPLEX_LIST = 2 ;

class CMyList : public CListBox
{
public:
	CMyList();

private:
	struct MyListParameter
	{
		BYTE		m_btType ;
		BYTE		m_btListCount ;
		BYTE		m_btListLineNum ;
		BYTE		m_btShowLineNum ;
		int			m_nWidth ;
		int			m_nHeight ;
		CPoint		m_ListPoint ;
		CPoint		m_IntPoint ;
		CRect		m_SelRect ;
		CRect		m_ItemRect ;
		int			m_nTopIndex ;
		int			m_nCurIndex ;
		CPoint		m_OffsetPoint ;
		BOOL		m_bVShow ;
		CWnd*		m_ParentCob ;
		char		m_strListID[64] ;
		int			m_nLastClkIndex ;
		int			m_nListRealIndex[64] ;
	} m_LstPar;
	struct MyListFont
	{
		BYTE		m_btFontHeight ;
		DWORD		m_dwFontColor ;
		BYTE		m_btCharNum ;
	} m_LstFnt;
	struct MyScrParameter
	{
		CRect		m_ScrUpRect ;
		CRect		m_ScrDoRect ;
		CMyScrollButton m_ScrUpBtn ;
		CMyScrollButton m_ScrDoBtn ;
	} m_LstScr;
public:

	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	void ShowVScroll ();
	void SetOffset ( CPoint offsetPnt );
	int GetListRealIndex ( int nIndex );
	void SetListRealIndex ( int nIndex, int nRealIndex );
	void SetParentCob( CWnd *ParentCob ) ;
	void SetParameter ();
	BOOL Init ( int xPos, int yPos, char *ListID = NULL, BYTE btType = 0 );
	void SetFontColor ( DWORD dwColor );
	void ShowChar ();
	void ShowSel ();
	void Show ( int x, int y);
	virtual ~CMyList();

protected:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	friend struct MyListProbe;
};

#endif
