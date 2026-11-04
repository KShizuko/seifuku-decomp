#if !defined(AFX_MYGRID_H__891D9907_6DA2_4F0D_A12A_4C9620126950__INCLUDED_)
#define AFX_MYGRID_H__891D9907_6DA2_4F0D_A12A_4C9620126950__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
enum { ICON_TYPE_ITEM = 0,
	   ICON_TYPE_ACTION,
	   ICON_TYPE_SKILL,
	   ICON_TYPE_XP,
	   ICON_TYPE_SHOP,
	   ICON_TYPE_DEPOT,
	   ICON_TYPE_EQUIP,
	   ICON_TYPE_QUSE,
	   ICON_TYPE_TRADE,
	   ICON_TYPE_TASKITEM};

class CMyGrid : public CButton
{
public:
	CMyGrid();

private:
	struct MyGridParameter
	{
		CPoint		m_IntPoint ;
		CPoint		m_Point ;
		BYTE		m_btGridLine ;
		BYTE		m_btGridRow ;
		BYTE		m_btGridHeight ;
		BYTE		m_btGridWidth ;
		BOOL		m_bPickUp ;
		POINT		m_PickUpMousePoint ;
		CPoint		m_PickUpIconLandR ;
		BOOL		m_bMouseMove ;
		CPoint		m_MouseMoveIconLandR ;
		BYTE		m_btOwnerID ;
		CPoint		m_InsetLandR ;
		OBJID		m_IconTypeID[256] ;
		OBJID		m_IconRealID[256] ;
		BOOL		m_bGridAble[256] ;
		DWORD		m_IconIndex[256] ;
		OBJID		m_PickUpIconTypeID ;
		BYTE		m_btPickUpIconType ;
		DWORD		m_dwPickUpIconIndex ;
		BYTE		m_btPickUpIconActType ;
		DWORD		m_dwMouseMoveIconIndex ;
		DWORD		m_dwMouseMoveIconTypeID;
		int			m_nGridAbleNum ;
		BOOL		m_bEnable ;
		BOOL		m_bPickEnable ;
		CRect		m_ClientRect ;
		int			m_IconNum[256] ;
		BYTE		m_btIconType[256] ;
		BYTE		m_btIconActType[256] ;
		DWORD		m_dwIconActTime[256] ;
	}m_GriPar ;

public:

	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	int GetIconRealID ( int nIndex );
	void SetIconRealID( int nIndex, OBJID rID );
	BYTE GetPickUpActType( );
	BYTE GetIconType( int nIndex );
	BYTE GetPickUpType();
	void SetIconNum ( int nIndex, int nNum);
	DWORD GetMouseMoveTypeID();
	int GetIconTypeID ( int nIndex );
	DWORD GetMouseMoveIndex ();
	void SetPickEnable ( BOOL bEnable );
	void SetEnable ( BOOL bEnable );
	void ReSumePickUp ();
	int GetPickUpIndex ();
	void RemoveIcon ( int nLine, int nRow, BOOL bAll = false );
	int ChangeIcon ( CPoint IconLandR, OBJID ItemTypeID, DWORD dwItemIndex, BYTE btItemType = ICON_TYPE_ITEM, BYTE btActType = 0 );
	CPoint GetInsertLandR();
	void SetPickUp ( BOOL bPickUp );
	CPoint GetPickUpMousePnt ();
	void MouseMoveTest();
	int GetGridIndex ( CPoint lrPoint );
	BOOL LoadIcon ( CPoint lrPoint, OBJID ItemTypeID, DWORD dwItemIndex, BYTE btItemType, BYTE btActType );
	void SetGridAble ( CPoint lrPoint, BOOL bAble );
	int InsertIcon ( int nLine, int nRow, OBJID ItemTypeID, DWORD dwItemIndex, BYTE btItemType = ICON_TYPE_ITEM, BYTE btActType = 0, BOOL bAuto = true);
	BOOL GetGridAble ( CPoint lrPoint );
	CPoint GetLineAndRow ( CPoint MousePoint );
	void Show ( int x, int y );
	BOOL Init ( int xPos, int yPos, BYTE btLine, BYTE btRow, BYTE btOwnerID );
	virtual ~CMyGrid();
	OBJID GetPickUpTypeID ( ) ;
	BOOL GetPickUp () ;
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk (UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	friend struct MyGridProbe;
};

#endif
