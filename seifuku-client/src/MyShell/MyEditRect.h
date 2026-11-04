#if !defined(AFX_MYEDITRECT_H__DBB46217_5B5F_49AA_9DAF_8560DAD443BA__INCLUDED_)
#define AFX_MYEDITRECT_H__DBB46217_5B5F_49AA_9DAF_8560DAD443BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CMyEditRect : public CButton
{
public:
	CMyEditRect();

private:
	CWnd* m_pParentWnd;
public:

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void Init( CWnd* pParentWnd);
	virtual ~CMyEditRect();

protected:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()

	friend struct MyEditRectProbe;
};

#endif
