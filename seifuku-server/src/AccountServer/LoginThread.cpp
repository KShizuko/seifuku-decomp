#include "AllHeads.h"
#include "LoginThread.h"
#include "MsgAccount.h"
#include "MsgConnect.h"
#include "MsgLogin.h"
#include "MsgQueryFee.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#undef	LOCKTHREAD
#define	LOCKTHREAD

CLoginThread::CLoginThread(u_short nPort) 
		: CThreadBase(), m_cListenSocket(nPort)
{
	LOCKTHREAD;

	m_pBanIPs = new CBanIP[MAXBANIPS];
}

CLoginThread::~CLoginThread()
{
	LOCKTHREAD;

	delete m_pBanIPs;
}

void	CLoginThread::OnInit()
{
	LOCKTHREAD;

	try{
		m_cRc5.Rc5InitKey(RC5PASSWORD_KEY);

		m_cListenSocket.Open();
		LOGMSG("Login thread started");

	}catch(...) { LOGCATCH("Login thread init exception, exiting"); }
}

bool	CLoginThread::OnProcess()
{
	LOCKTHREAD;

	try{
		time_t	tStart = clock();
		long	nUsed = 0;

		if(g_bEnableLogin)
		{
			fd_set	readmask;
			FD_ZERO(&readmask);
			if(!m_cListenSocket.IsOpen())
				m_cListenSocket.Rebuild();
			if(m_cListenSocket.IsOpen())
				FD_SET(m_cListenSocket.Socket(), &readmask);
			for(int i = 0; i < MAXCONNECTS; i++)
			{
				if(m_aServerSocket[i].IsOpen())
					FD_SET(m_aServerSocket[i].Socket(), &readmask);
			}
			struct timeval	timeout = {0,0};
			int ret = select(FD_SETSIZE, &readmask, (fd_set *) 0, (fd_set *) 0, &timeout);

			if(ret == -1)
			{
				LOGERROR("Login thread select error [%d]", WSAGetLastError());
			}
			else if(ret > 0 && FD_ISSET(m_cListenSocket.Socket(), &readmask))
			{
				FD_CLR(m_cListenSocket.Socket(), &readmask);
				ret--;
				u_long	nClientIP;
				SOCKET sockNew = m_cListenSocket.Accept(nClientIP);
				if(sockNew != INVALID_SOCKET)
				{
					InterlockedIncrement(&s_nLoginAccept);

					bool	bBan = false;
					{
						for(int i = 0; i < MAXBANIPS; i++)
						{
							if(m_pBanIPs[i].ClientIP() == nClientIP && m_pBanIPs[i].IsBan())
							{
								bBan = true;
								break;
							}
						}
					}

					if(bBan)
					{
						struct linger ling;
						ling.l_onoff = 1;
						ling.l_linger = 0;
						setsockopt(sockNew, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));
						closesocket(sockNew);
						InterlockedDecrement(&s_nSocketCount);
					}
					else
					{
						bool	bSuccess = false;
						int i;
						for(i = 0; i < MAXCONNECTS; i++)
						{
							if(!m_aServerSocket[i].IsOpen())
							{
								bSuccess = true;
								break;
							}
						}

						if(bSuccess)
						{
							m_aServerSocket[i].Open(sockNew, nClientIP);
							m_aServerSocket[i].GetPeerIP();
						}
						else
						{
							srand((unsigned int) clock());
							int		nPos = rand() % MAXCONNECTS;
							char	bufTemp[IPSTRSIZE];
							ASSERT(m_aServerSocket[nPos].IsOpen());
							SafeCopy(bufTemp, m_aServerSocket[nPos].GetPeerIP(), IPSTRSIZE);
							if(FD_ISSET(m_aServerSocket[nPos].Socket(), &readmask))
							{
								FD_CLR(m_aServerSocket[nPos].Socket(), &readmask);
								ret--;
							}
							m_aServerSocket[nPos].Close(true);
							m_aServerSocket[nPos].Open(sockNew, nClientIP);
							LOGWARNING("Connection table full, MAXCONNECTS too small. [%s] took the slot of [%s].",
										m_aServerSocket[nPos].GetPeerIP(), bufTemp);
						}
					}
				}
			}

			for(int i = 0; i < MAXCONNECTS; i++)
			{
				if(ret <= 0)
					break;

				if(m_aServerSocket[i].IsOpen() && FD_ISSET(m_aServerSocket[i].Socket(), &readmask))
				{
					FD_CLR(m_aServerSocket[i].Socket(), &readmask);
					ret--;

					char	bufMsg[_MAX_MSGSIZE];
					int		nMsgLen = 0;
					if(m_aServerSocket[i].Recv(bufMsg, nMsgLen))
						if(nMsgLen)
							ProcessMsg(i, bufMsg, nMsgLen);
				}
			}
			nUsed = clock() - tStart;
			InterlockedExchangeAdd(&s_nAllTimeSum, nUsed);
			InterlockedIncrement(&s_nLoginCount);
			InterlockedExchangeAdd(&s_nAvgServerTime, nUsed);
			if(nUsed > InterlockedExchangeAdd(&s_nMaxServerTime, 0))
				InterlockedExchange(&s_nMaxServerTime, nUsed);
		}

		long	nRemain = LOGINLOOPDELAY - nUsed;
		if(nRemain < 0 || nRemain > LOGINLOOPDELAY)
			nRemain = 0;
		Sleep(nRemain);

		return true;

	}catch(...) { 
		LOGCATCH("Login thread main loop exception, still running.");
		PrintText("Login thread main loop error, still running...");
		return true; 
	}
}

void	CLoginThread::OnDestroy()
{
	LOCKTHREAD;

	try{
		m_cListenSocket.Close();
		for(int i = 0; i < MAXCONNECTS; i++)
		{
				m_aServerSocket[i].Close();
		}

		LOGMSG("Login thread closed");

	}catch(...) { LOGCATCH("Login thread close exception, exiting"); }
}

bool	CLoginThread::ProcessMsg(int nIndex, char * pBuf, int nLen)
{
	unsigned short unMsgType = CNetMsg::GetType(pBuf, nLen);
	int	nMsgLen = CNetMsg::GetSize(pBuf, nLen);

	if(unMsgType == _MSG_ACCOUNT && nMsgLen == nLen )
	{
		return ProcessMsgAccount(nIndex, pBuf, nLen);
	}
	else if(unMsgType == _MSG_CONNECT && nMsgLen == nLen)
	{
		if(STAT_SERIAL)
			return ProcessMsgConnect(nIndex, pBuf, nLen);
		return true;
	}
	else
	{
		char *	pClientIP	= m_aServerSocket[nIndex].GetPeerIP();
		unsigned long nClientIP	= inet_addr(pClientIP);
		AddBan(nClientIP, pClientIP, "未知");
		if(nLen >= 20)
		{
			pBuf[19] = 0;
			LOGWARNING("A client [%s] sent a bad login message, header [%d][%d], length [%d], maybe [%s]",
								pClientIP, nMsgLen, unMsgType, nLen, (pBuf+4));
		}
		else
			LOGWARNING("A client [%s] sent a bad login message, header [%d][%d], length [%d]",
							pClientIP, nMsgLen, unMsgType, nLen);
		m_aServerSocket[nIndex].Close(true);
		return false;
	}
}

bool	CLoginThread::ProcessMsgAccount(int nIndex, char * pBuf, int nLen)
{
	CMsgAccount	cMsg;
	cMsg.Create(pBuf, nLen);
#ifdef	RC5ENCRYPT
	m_cRc5.Rc5Decrypt(cMsg.m_pInfo->szPassword, _MAX_NAMESIZE);
#endif
	FormatStr(cMsg.m_pInfo->szAccount, _MAX_NAMESIZE);
	FormatStr(cMsg.m_pInfo->szPassword, _MAX_NAMESIZE);
	FormatStr(cMsg.m_pInfo->szServer, _MAX_NAMESIZE);

	if(cMsg.m_pInfo->szAccount[0] == 0 || cMsg.m_pInfo->szPassword[0] == 0 || cMsg.m_pInfo->szServer[0] == 0)
		return false;

	char *	pClientIP						= m_aServerSocket[nIndex].GetPeerIP();
	unsigned long nClientIP					= inet_addr(pClientIP);
	DWORD	nAuthenID						= NewAuthenID(nClientIP);
	char *	pErrMsg							= "登录失败";
	char	szNotifyMsg[_MAX_NAMESIZE]		= "";
	OBJID	idAccount						= ID_NONE;
	OBJID	idFeeAccount					= ID_NONE;
	char	szFeeAccount[_MAX_NAMESIZE]		= "";
	int		nFeeType						= c_typeNone;
	int		nPoint							= 0;
	DWORD	nPointTime						= 0;
	int		nBlock							= false;
	bool	bRejoin							= false;
	char	szOldServerName[SERVERNAMESIZE]	= "";
	char	szOldFeeAccount[_MAX_NAMESIZE]	= "";
	char 	szPassword[_MAX_NAMESIZE]	= "";
	OBJID	idOldFeeAccount				= ID_NONE;
	char	szOldIP[_MAX_NAMESIZE]		= "";
	DWORD	nOldAuthenID				= 0;

	if(g_pOnlineTable->GetAttr(cMsg.m_pInfo->szAccount, idAccount, szPassword, nOldAuthenID,
						szOldServerName, szOldFeeAccount, idOldFeeAccount, szOldIP))
	{
		bRejoin	= true;
	}

	if(!GetAccount(cMsg.m_pInfo->szAccount, cMsg.m_pInfo->szPassword, 
						idAccount, nFeeType, nPoint, nPointTime, nBlock))
	{
		AddBan(nClientIP, pClientIP, cMsg.m_pInfo->szAccount);
		RefuseLogin(nIndex, cMsg.m_pInfo->szAccount, c_errPassword, "帐号名或口令错");
		LOGACCOUNT("Player [%s] password [%s] login. Wrong account or password, IP [%s]",
					cMsg.m_pInfo->szAccount, cMsg.m_pInfo->szPassword, pClientIP);
		return false;
	}

	if(nBlock)
	{
		RefuseLogin(nIndex, cMsg.m_pInfo->szAccount, c_errBan, "该帐号被封号");
		return false;
	}

	if(nFeeType == c_typeNetBarPoint || nFeeType == c_typeNetBarTime || nFeeType == c_typeISP)
	{
		RefuseLogin(nIndex, cMsg.m_pInfo->szAccount, c_errBarPassword, "该帐号不能登录");
		return false;
	}

	if(IsFreeServer(cMsg.m_pInfo->szServer))
	{
		nFeeType		= c_typeFree;
	}
	else if(CheckISP(idAccount, nClientIP, pClientIP, 
						idFeeAccount, szFeeAccount, nPoint, nPointTime, szNotifyMsg))
	{
		nFeeType		= c_typeISP;
	}
	else
	{
		idFeeAccount	= idAccount;
		SafeCopy(szFeeAccount, cMsg.m_pInfo->szAccount, _MAX_NAMESIZE);
	}

	if(bRejoin)
	{
		if(strcmp(cMsg.m_pInfo->szServer, szOldServerName) != 0 
										|| strcmp(szFeeAccount, szOldFeeAccount) != 0)
		{
			if(!g_pOnlineTable->IsTimeWait(idAccount))
				g_pPointThread->Kickout(idAccount);

			RefuseLogin(nIndex, cMsg.m_pInfo->szAccount, c_errOnline, "请稍后重新登录");
			return false;
		}
	}

	switch(nFeeType)
	{
	case c_typePoint:
	case c_typeISP:
		if(nPoint <= 0)
		{
			if(ENABLE_LOGIN_NO_POINT)
			{
				SafeCopy(szNotifyMsg, FLAG_NO_POINT, _MAX_NAMESIZE);
			}
			else
			{
				RefuseLogin(nIndex, cMsg.m_pInfo->szAccount, c_errNotPoint, "小时数已用尽");
				return false;
			}
		}
		break;
	case c_typeTime:
		{
			time_t	tCurr = time(NULL);
			tm *	pTm = localtime(&tCurr);
			DWORD	nCurrTime = (pTm->tm_year+1900)*10000 + (pTm->tm_mon+1)*100 + pTm->tm_mday;
			if(nCurrTime > nPointTime)
			{
				if(MONTH_CARD_TO_POINT_CARD && nPoint > 0)
				{
					nFeeType	= c_typePoint;
				}
				else if(MONTH_CARD_TO_POINT_CARD && ENABLE_LOGIN_NO_POINT)
				{
					nFeeType	= c_typePoint;
					SafeCopy(szNotifyMsg, FLAG_NO_POINT, _MAX_NAMESIZE);
				}
				else
				{
					RefuseLogin(nIndex, cMsg.m_pInfo->szAccount, c_errNotTime, "帐号已过期");
					return false;
				}
			}
			else
			{
				if(szNotifyMsg[0] == 0)
				{
					Check7DaysNodify(nPointTime, szNotifyMsg);
				}
			}
		}
		break;
	case c_typeFree:
		break;
	default:
		RefuseLogin(nIndex, cMsg.m_pInfo->szAccount, c_errUnknown, "数据库错误");
		LOGERROR("Player [%s] bad fee type [%d]. ★★★★★★★★★", cMsg.m_pInfo->szAccount, nFeeType);
		return false;
	}

	ASSERT(idAccount);

	int nRet = g_pPointThread->NewLogin(idAccount, nAuthenID, szNotifyMsg, cMsg.m_pInfo->szServer);
	switch(nRet)
	{
	case CPointThread::ERR_NO_SERVER:
		RefuseLogin(nIndex, cMsg.m_pInfo->szAccount, c_errUnknowServer, "服务器未启动");
		return false;
	case CPointThread::ERR_BUSY:
		RefuseLogin(nIndex, cMsg.m_pInfo->szAccount, c_errServerBusy, "服务器忙请稍候");
		return false;
	case CPointThread::ERR_FULL:
		RefuseLogin(nIndex, cMsg.m_pInfo->szAccount, c_errServerFull, "服务器人数已满");
		return false;
	case CPointThread::ERR_NONE:
		{
			if(bRejoin)
			{
				g_pOnlineTable->Rejoin(idAccount, nAuthenID, pClientIP, 
						cMsg.m_pInfo->szAccount, cMsg.m_pInfo->szPassword, 
						nFeeType, cMsg.m_pInfo->szServer, idFeeAccount, szFeeAccount);
				AllowLogin(nIndex, idAccount, nAuthenID, cMsg.m_pInfo->szServer);
				LOGACCOUNT("Player [%s][%d] allowed to relogin as type [%d] on [%s]. Auth ID [%08X], IP [%s], fee account [%s], notice [%s]",
								cMsg.m_pInfo->szAccount, idAccount, nFeeType, cMsg.m_pInfo->szServer, 
								nAuthenID, pClientIP, szFeeAccount, szNotifyMsg);
			}
			else
			{
				g_pOnlineTable->AddNew(idAccount, nAuthenID, pClientIP, 
						cMsg.m_pInfo->szAccount, cMsg.m_pInfo->szPassword, 
						nFeeType, cMsg.m_pInfo->szServer, idFeeAccount, szFeeAccount);
				AllowLogin(nIndex, idAccount, nAuthenID, cMsg.m_pInfo->szServer);
				LOGACCOUNT("Player [%s][%d] allowed to login as type [%d] on [%s]. Auth ID [%08X], IP [%s], fee account [%s], notice [%s]",
								cMsg.m_pInfo->szAccount, idAccount, nFeeType, cMsg.m_pInfo->szServer, 
								nAuthenID, pClientIP, szFeeAccount, szNotifyMsg);
			}

			char	szStamp[255];
			time_t	tCurr = time(NULL);
			tm*	pTm = localtime(&tCurr);
			sprintf(szStamp, "%04d%02d%02d%02d%02d%02d", pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
			char szSQL[1024];
			sprintf(szSQL, "INSERT DELAYED logon VALUES ('%s', '%s',%d,%d,'%s',   %u,'%s','%s','%s',%d);",
								szStamp, cMsg.m_pInfo->szAccount, idAccount, nFeeType, cMsg.m_pInfo->szServer, 
								nAuthenID, pClientIP, szFeeAccount, szNotifyMsg, bRejoin);
			g_db.ExecuteSQL(szSQL);
			return true;
		}
		break;
	default:
		ASSERT(!"NewLogin");
		return false;
	}
}

void CLoginThread::AllowLogin(int nIndex, OBJID idAccount, DWORD nAuthenID, LPCTSTR szServer)
{
	CMsgConnect cRetMsg;
	char	szServerIP[IPSTRSIZE] = "";
	g_pPointThread->GetServerIP(szServerIP, szServer);
	cRetMsg.Create(idAccount, nAuthenID, szServerIP);
	m_aServerSocket[nIndex].Send(cRetMsg.GetBuf(), cRetMsg.GetSize());
	if(!STAT_SERIAL)
		m_aServerSocket[nIndex].ShutDown();
}

void CLoginThread::RefuseLogin(int nIndex, LPCTSTR szLoginName, int nType, LPCTSTR szText)
{
	CMsgConnect cRetMsg;
	cRetMsg.Create(ID_NONE, nType, (char*)szText);
	m_aServerSocket[nIndex].Send(cRetMsg.GetBuf(), cRetMsg.GetSize());
	m_aServerSocket[nIndex].ShutDown();
	LOGACCOUNT("Login refused [%s]. Reason [%s][%d]", szLoginName, szText, nType);
}

void CLoginThread::AddBan(DWORD nClientIP, LPCTSTR szClientIP, LPCTSTR szAccount)
{
	bool	bFoundBan = false;
	int		nFreeSlot = -1;
	int		nBanCount = 0;
	for(int i = 0; i < MAXBANIPS; i++)
	{
		if(m_pBanIPs[i].ClientIP() == 0)
		{
			if(nFreeSlot == -1)
				nFreeSlot = i;
		}
		else
		{
			nBanCount++;
		}

		if(m_pBanIPs[i].ClientIP() == nClientIP)
		{
			bFoundBan = true;
			m_pBanIPs[i].IncError();
			if(m_pBanIPs[i].IsBan())
			{
				LOGERROR("A client failed login [%d] times; the account server may be under attack. IP [%s]", BANERRORS, szClientIP);
				LOGACCOUNT("Player [%s] failed login to the account server [%d] times in a row. IP [%s] banned for [%d] seconds",
							szAccount, BANERRORS, szClientIP, BANSECS);
				PrintText("IP address \"%s\" failed \"%d\" times in a row, no login for \"%d\" seconds",
							szClientIP, BANERRORS, BANSECS);
			}
		}
	}
	if(!bFoundBan)
	{
		if(nFreeSlot != -1)
		{
			m_pBanIPs[nFreeSlot].Create(nClientIP);
			if(nBanCount*100/MAXBANIPS > 78)
				LOGWARNING("Added to BAN table [%d/%d], IP [%s]", nBanCount+1, MAXBANIPS, szClientIP);
		}
		else
		{
			LOGERROR("BAN table too small, IP [%s] not added. Raise MAXBANIPS", szClientIP);
		}
	}
}

bool	CLoginThread::ProcessMsgConnect(int nIndex, char * pBuf, int nLen)
{
	CMsgConnect	cMsg;
	cMsg.Create(pBuf, nLen);
	cMsg.m_pInfo->szInfo[_MAX_NAMESIZE-1] = 0;

	g_pOnlineTable->SetSerial(cMsg.m_pInfo->idAccount, cMsg.m_pInfo->dwData);
	m_aServerSocket[nIndex].ShutDown();

	char	szStamp[255];
	time_t	tCurr = time(NULL);
	tm*	pTm = localtime(&tCurr);
	sprintf(szStamp, "%04d%02d%02d%02d%02d%02d", pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
	char	szSerial[255];
	::InsertBackslash(szSerial, cMsg.m_pInfo->szInfo);
	char szSQL[1024];
	sprintf(szSQL, "INSERT DELAYED serial VALUES ('%s',%d,%d,'%s');",
						szStamp, cMsg.m_pInfo->idAccount, cMsg.m_pInfo->dwData, szSerial);
	g_db.ExecuteSQL(szSQL);
	return true;
}
