#if !defined(AFX_SERVERADV_H__4294A0C1_0468_4440_8E90_054CE7400250__INCLUDED_)
#define AFX_SERVERADV_H__4294A0C1_0468_4440_8E90_054CE7400250__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include<vector>
using namespace std;

typedef struct
{
	char	szServerName[32];
	int		nPosX;
	int		nPosY;
	char	szFont[32];
	int		nFontSize;
	DWORD	dwColor;
	char	szText[256];
	char	szUrl[256];
}ServerAdvInfo;

class CServerAdv
{
public:
	CServerAdv();
	virtual ~CServerAdv();
private:
	vector<ServerAdvInfo> m_setAdv;
public:
	void Init(BOOL b1024);
	bool GetAdv(const char* pszServerName, vector<ServerAdvInfo>& setAdv);
	friend struct ServerAdvProbe;
};

#endif
