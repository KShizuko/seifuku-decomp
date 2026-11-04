#if !defined(AFX_MYEDIT_H__F0A4A4E3_5C62_4B1A_9D57_3E5B8C2D6A19__INCLUDED_)
#define AFX_MYEDIT_H__F0A4A4E3_5C62_4B1A_9D57_3E5B8C2D6A19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MyEditRect.h"

class CMyEdit : public CEdit
{
public:
	CMyEdit();

public:
	CMyEditRect		m_EditRect;
private:
	struct MyEditParameter
	{
		CPoint m_Pnt ;
		CPoint m_IntPnt ;
		int m_nWidth ;
		int m_nHeight ;
		int m_nSelNum ;
		BYTE m_btRightLeft ;
		BYTE m_btEndCharPos ;
		DWORD m_dwSelColor ;
		UINT m_uFrame ;
		char m_strEditID[64] ;
		BOOL	m_bMultiLine ;
		BOOL	m_bChinese;
		int				m_nLastSelEnd;
	} m_EdtPar ;
	struct MyEditFont
	{
		BOOL m_bPassword ;
		BYTE m_btFontWidth ;
		int m_nCharNum ;
		CPoint m_StartPoint ;
		CPoint m_EndPoint ;
		CPoint m_OffsetPoint ;
		DWORD m_dwFontColor ;
		DWORD m_dwBackColor ;
	} m_EdtFnt ;
	public:

		protected:
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	public:
		char* GetEditBkID();
		void SetEditBkID(char* EditID);
		void ReInit(int xPos, int yPos);
		void ReSize ( int xPos, int yPos, int nWidth, int nHeight, BOOL bUnused = false );
		void SetMultiLine ( BOOL bMultiLine );
		void GoNextLine ();
		void SetPasswordType( BOOL bPassword = true );
		void SetOffset ( CPoint offsetPnt );
		void SetFontColor (DWORD dwColor );
		BOOL Init ( int xPos, int yPos, char *EditID = NULL, DWORD dwBkColor = 0x000000, DWORD dwFontColor = 0xffffff, BOOL bMultiLine = false );
		void ShowChar();
		void Show( int x, int y );
		void EnableChinese( BOOL bChinese );
		virtual ~CMyEdit();

	protected:
		void ShowMultiLine();
		afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnEnable(BOOL bEnable);
		afx_msg void OnKillFocus(CWnd* pNewWnd);
		afx_msg void OnSetFocus(CWnd* pOldWnd);

		DECLARE_MESSAGE_MAP()
		friend struct MyEditProbe;
};

#endif
