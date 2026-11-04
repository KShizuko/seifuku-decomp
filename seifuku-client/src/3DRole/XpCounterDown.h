#if !defined(AFX_XPCOUNTERDOWN_H__02F1EBA3_D554_4800_A33C_04592E8C4495__INCLUDED_)
#define AFX_XPCOUNTERDOWN_H__02F1EBA3_D554_4800_A33C_04592E8C4495__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
class CXpCounterDown
{
	friend struct XpCounterDownProbe;
public:
	CXpCounterDown();
	virtual ~CXpCounterDown();
private:
	DWORD m_dwTimeBegin;
	int	  m_nOldNum;
public:
	void Show(DWORD dwTimeLeft, int nHeight, DWORD dwMax, BOOL bLeftAlign = true);
	void Show(int nNum, CMyPos posShow);
};

#endif
