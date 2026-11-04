#if !defined(AFX_MYIMAGE_H__A0276D1F_52DC_4D7C_83DB_FB3D265B03F9__INCLUDED_)
#define AFX_MYIMAGE_H__A0276D1F_52DC_4D7C_83DB_FB3D265B03F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

const int NORMAL_IMAGE  = 0 ;
const int ITEM_IMAGE	= 1 ;
const int FACE_IMAGE	= 2 ;
const int SKILL_IMAGE	= 3 ;
const int XP_IMAGE		= 4 ;
const int ITEM5_IMAGE	= 5 ;
const int ITEM6_IMAGE	= 6 ;
const int ITEM7_IMAGE	= 7 ;
const int BOOTHITEM_IMAGE = 8 ;
const int IMAGE_TYPE_9	= 9 ;
const int CONTROL_IMAGE = 10 ;
const int SERVER_IMAGE	= 11 ;
const int ITEM12_IMAGE	= 12 ;
const int ITEM13_IMAGE	= 13 ;

class CMyImage : public CButton
{
public:
	CMyImage();
	BOOL		m_bMouseMove ;
private:
	CPoint		m_IntPoint ;
	CPoint		m_Point ;
	BOOL		m_bImageLoad ;
	UINT		m_uFrame ;
	char		m_strImageID[64] ;
	DWORD		m_dwImageID ;
	int			m_nWidth ;
	int			m_nHeight ;
	CRect		m_ShowRect ;
	BOOL		m_btItemType ;
	char		m_strAni[63] ;
	int			m_nItemPos ;
	int			m_nImageTypeID ;
	BYTE		m_btActType ;
	DWORD		m_dwActTime ;
	BOOL		m_bImgZoom ;
	int			m_nCurMoney ;
	BYTE		m_btMilNum ;
	BYTE		m_btThoNum ;
	BYTE		m_btHunNum ;
	int			m_nWeaponLevel ;
	BYTE		m_btGoldStar ;
	BYTE		m_btSillerStar ;
	BYTE		m_btCopperStar ;
	BOOL		m_bSetCursor ;
public:

	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void SetCurFrame(UINT uFrame);
	char * GetImageID ();
	void SetCursorChange ( BOOL bChange ) { m_bSetCursor = bChange ; }
	void ShowWeaponLevel ( int x, int y );
	void SetWeaponLevel ( int nLevel );
	void ShowMoney ( int x, int y );
	void SetMoney ( int nMoney );
	void SetActType ( BYTE btActType );
	void ShowBk( const char* strBkID );
	void ShowFocus();
	BOOL Init ( int xPos, int yPos, char *ImageID = NULL, BYTE btItemType = 0, int nItemPos = -1 );
	void MouseMoveText();
	OBJID GetImageRealID() {return m_dwImageID;}
	BOOL GetImageLoadState ();
	void Show ( int x, int y );
	void RemoveImage ();
	BOOL InsertImage ( char *ImageID, int nImageTypeID = 0, BOOL bImgZoom = false );
	virtual ~CMyImage();

protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()
	friend struct MyImageProbe;
};

#endif
