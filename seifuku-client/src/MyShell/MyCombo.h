#if !defined(AFX_MYCOMBO_H__055278BE_217C_49C2_947E_54B6D33E54A7__INCLUDED_)
#define AFX_MYCOMBO_H__055278BE_217C_49C2_947E_54B6D33E54A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "DlgListBox.h"
#include "MyEditRect.h"

class CMyCombo : public CEdit
{
public:
	CMyCombo();

public:
	CPoint m_IntPnt ;
	CMyBitmap* m_pBmp ;
	int m_nWidth ;
	int m_nHeight ;
	BOOL m_bChinese;
	int	m_nStartChar;
	CPoint m_OffsetPoint ;
	int m_nFontWidth ;
	DWORD  m_dwFontColor ;
	DWORD  m_dwBackColor ;
	int		m_nCharNum;
	CDlgListBox	*m_CobLstBox;
	CMyEditRect		m_EditRect;
public:

	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void InitListBox ( CDlgListBox *ListBox );
	BOOL Init ( int xPos, int yPos, char *FileName = NULL, DWORD dwFontColor = 0xffffff, DWORD dwBackColor = 0x000000 );
	void ShowChar( int x, int y );
	void Show ( int x, int y );
	virtual ~CMyCombo();
	void EnableChinese( BOOL bChinese );
	void ReInit( int xPos, int yPos );

protected:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};

#endif
