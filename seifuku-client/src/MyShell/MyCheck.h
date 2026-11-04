#if !defined(AFX_MYCHECK_H__F0C0A6D1_8E7B_4E6A_9F1B_2A6D0F8C1E77__INCLUDED_)
#define AFX_MYCHECK_H__F0C0A6D1_8E7B_4E6A_9F1B_2A6D0F8C1E77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CMyCheck : public CButton
{
public:
	CMyCheck();

private:
	struct MyCheckParameter
	{
		UINT		m_uFrame ;
		char		m_strCheckID[64] ;
		CPoint		m_IntPoint ;
		BYTE		m_btType ;
		BYTE		m_btCurrent ;
		CRect m_ShowRect ;
		int m_nWidth ;
		int m_nHeight ;
	} m_ChkPar ;

public:

	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void SetCheckValue( BYTE btValue );
	BYTE GetCheckValue();
	void Show ( int x, int y);
	BOOL Init ( int xPos, int yPos, char *CheckID, BYTE btType, BYTE btCurrent = 0 );
	virtual ~CMyCheck();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()
	friend struct MyCheckProbe;
};

#endif
