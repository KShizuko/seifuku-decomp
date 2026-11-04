#if !defined(AFX_FLOATINGSTR_H__6762C612_CC11_4162_990D_42C63710E792__INCLUDED_)
#define AFX_FLOATINGSTR_H__6762C612_CC11_4162_990D_42C63710E792__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"

typedef struct
{
	char	szStr[_MAX_NAMESIZE];
	int		nMaxHeight;
	int		nHeight;
	int		nStepHeight;
	DWORD	dwColor; 
}FStrInfo;

#include <deque>  
using namespace std;
typedef deque<FStrInfo*> DEQUE_FSTRINFO;

class CFloatingStr  
{
public:
	CFloatingStr();
	virtual ~CFloatingStr();

public:
	void AddStr(char* pszStr, int nMaxHeight, DWORD dwColor, int nStepHeight);
	void Show(CMyPos posScr);
	void Process();
private:
	DEQUE_FSTRINFO m_setStr;
	
private:
	void Destory();
};

#endif
