#if !defined(AFX_STRINGINFOVIEW_H__68FD8816_525E_4636_BBF7_21C699DDF018__INCLUDED_)
#define AFX_STRINGINFOVIEW_H__68FD8816_525E_4636_BBF7_21C699DDF018__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "IInfoView.h"
#include "BaseFunc.h"

class CStringInfoView : public IInfoView 
{
	friend struct StringInfoViewProbe;
public:
	CStringInfoView();
	virtual ~CStringInfoView();

private:
	char	m_szStr[_MAX_STRING];
	CMyPos	m_posShow;
	DWORD	m_dwColor;

public:
	virtual void Show();

public:
	static CStringInfoView* CreateNew(char* pszStr, CMyPos posShow, DWORD dwColor);
	BOOL Create(char* pszStr, CMyPos posShow, DWORD dwColor);
};

#endif
