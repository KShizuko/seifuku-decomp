#if !defined(AFX_IINFOVIEW_H__7633ED1E_45B2_4A1D_B36A_72BFD70F9669__INCLUDED_)
#define AFX_IINFOVIEW_H__7633ED1E_45B2_4A1D_B36A_72BFD70F9669__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class IInfoView  
{
public:
	IInfoView();
	virtual ~IInfoView();
public:
	virtual void Show(){;}
};

#endif
