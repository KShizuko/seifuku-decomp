#if !defined(AFX_LOGINTHREAD_H__60D5F481_EF6A_4D21_9D0A_ED44B0AB9F48__INCLUDED_)
#define AFX_LOGINTHREAD_H__60D5F481_EF6A_4D21_9D0A_ED44B0AB9F48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ThreadBase.h"
#include "RC5_321216.h"

class CBanIP
{
public:
	CBanIP() { Clear(); }
public:
	void	Create(DWORD nClientIP) { Clear(); m_nClientIP = nClientIP; IncError(); }
	DWORD	ClientIP() { ClearErrorTimeout(); ClearBanTimeout(); return m_nClientIP; }

	bool	IncError() {
		if(ClearErrorTimeout()) return false;
		if(m_nClientIP && ++m_nErrorTimes >= BANERRORS){ m_tStartBan = m_tLastError = clock(); return true; } 
		else { m_tLastError = clock(); return false; }
	}

	bool	IsBan() { 
		ClearBanTimeout();
		return (m_nClientIP && m_tStartBan);
	}
private:
	bool ClearErrorTimeout(){
		if(m_tLastError && m_nClientIP && !m_tStartBan && clock() >= m_tLastError + BANERRORSECS*CLOCKS_PER_SEC){
			Clear(); return true; 
		}
		return false;
	}
	bool ClearBanTimeout(){
		if(m_tStartBan && m_nClientIP && m_tStartBan && clock() >= m_tStartBan + BANSECS*CLOCKS_PER_SEC){
			Clear(); return true;
		}
		return false;
	}
	void Clear() { m_nClientIP = m_nErrorTimes = m_tStartBan = m_tLastError = 0; }
protected:
	DWORD	m_nClientIP;
	int		m_nErrorTimes;
	time_t	m_tStartBan;
	time_t	m_tLastError;
};

class CLoginThread : public CThreadBase  
{
public:
	CLoginThread(u_short nPort);
	virtual ~CLoginThread();
public:
protected:
	virtual	void	OnInit();
	virtual bool	OnProcess();
	virtual void	OnDestroy();
protected:
	CServerSocket<ACCOUNT_KEY1, ACCOUNT_KEY2>	m_aServerSocket[MAXCONNECTS];
private:
	bool	ProcessMsg(int nIndex, char * pBuf, int nLen);
	bool	ProcessMsgAccount(int nIndex, char * pBuf, int nLen);
	bool	ProcessMsgConnect(int nIndex, char * pBuf, int nLen);
	void	AddBan(DWORD nClientIP, LPCTSTR szClientIP, LPCTSTR szAccount);
	void	AllowLogin(int nIndex, OBJID idAccount, DWORD nAuthenID, LPCTSTR szServer);
	void	RefuseLogin(int nIndex, LPCTSTR szLoginName, int nType, LPCTSTR szText);
private:
	CListenSocket	m_cListenSocket;
	CBanIP *		m_pBanIPs;
	CRc5_321216		m_cRc5;
};

#endif
