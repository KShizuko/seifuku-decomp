#if !defined(AFX_MYPROGRESS_H__5EB25FBE_E467_490D_8D85_C86A9AA0B826__INCLUDED_)
#define AFX_MYPROGRESS_H__5EB25FBE_E467_490D_8D85_C86A9AA0B826__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

const int		MY_PROGRESS_V		=	0;
const int		MY_PROGRESS_HLEFT	=	10;
const int		MY_PROGRESS_HRIGHT	=	11;
const int		MY_PROGRESS_BLOCK   =	26;

class CMyProgress : public CProgressCtrl
{
public:
	CMyProgress();

private:
	struct MyProgressParameter
	{
		char		m_strProgressID[64] ;
		BOOL		m_bBorder ;
		CPoint		m_BorderOffset ;
		CPoint		m_IntPoint ;
		int			m_nWidth ;
		int			m_nHWidth ;
		int			m_nHeight ;
		float		m_nStep ;
		float		m_fAddStep ;
		BYTE		m_btType ;
		int			m_nMax ;
		int			m_nUpper ;
		int			m_nLower ;
		BYTE		m_btShowMode ;
	} m_PrgPar ;

public:

	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void SetWidthAndHeight( int nWidth, int nHeight, int nHWidth );
	void SetShowMode ( BYTE btShowMode );
	void GetRange ( int &nUpper, int &nLower );
	void SetRange( int nUpper, int nLower );
	void ResetValue( int nLower, int nUpper );
	BOOL Init ( int xPos, int yPos, char *ProgressID, int nLower, int nUpper, BYTE btType, BOOL bBorder = false, CPoint BorderPnt = CPoint ( 0, 0 ) );
	void Show ( int x, int y, int nPos, int nCurpos );
	virtual ~CMyProgress();

protected:

	DECLARE_MESSAGE_MAP()
	friend struct MyProgressProbe;
};

#endif
