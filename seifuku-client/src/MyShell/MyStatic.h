#if !defined(AFX_MYSTATIC_H__3ED7AB8A_9887_4F49_9ED6_3EBCB0568423__INCLUDED_)
#define AFX_MYSTATIC_H__3ED7AB8A_9887_4F49_9ED6_3EBCB0568423__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CMyStatic : public CStatic
{
public:
	CMyStatic();
	CPoint		m_IntPoint ;
private:
	int			m_nFontType;
	int			m_nStaticWidth;
	DWORD		m_dwFontColor ;
	int			m_nStaticPosType;
	BYTE		m_btType ;
	BOOL		m_bImageLoad ;
	char		m_strPictureID[64] ;
	int			m_nNum ;
	int			m_nLightType ;
	UINT		m_uFrame ;
	CPoint		m_ptBkOffset ;
	CPoint		m_ptBkSrc ;
	int			m_nBkWidth ;
	int			m_nBkHeight ;
	char		m_szBkAni[64] ;
	char		m_szFontName[32] ;
public:

	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void SetFontType(int nType )	{m_nFontType = nType;}
	void SetStaticPosType(int nType) {m_nStaticPosType=nType;}
	void SetCurFrame ( UINT uFrame );
	void SetLightType( int nType );
	void RemoveImage();
	void InsertImage ( );
	void SetFontColor ( DWORD dwColor );
	void Show ( int x, int y );
	BOOL Init ( int xPos, int yPos, BYTE nType = 0, char *PictureID = NULL, char *pszBkAni = NULL );
	void SetFontName ( int nType );
	void SetPictureID ( const char* pszID );
	virtual ~CMyStatic();

protected:

	DECLARE_MESSAGE_MAP()
	friend struct MyStaticProbe;
};

#endif
