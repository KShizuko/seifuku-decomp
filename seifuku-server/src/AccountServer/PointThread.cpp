#include "AllHeads.h"
#include "PointThread.h"
#include "MsgConnect.h"
#include "MsgAccount.h"
#include "MsgLogin.h"
#include "MsgFee.h"
#include "MsgQueryFee.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CPointThread::CPointThread(u_short nPort, int nSndBuf )
			: CThreadBase(), m_cListenSocket(nPort, nSndBuf)
{
	for(int i = 0; i < MAXGAMESERVERS; i++)
	{
		Clear(i, false);
	}

	GetServerAccount();
}

CPointThread::~CPointThread()
{
}

void	CPointThread::OnInit()
{
	LOCKTHREAD;
	try{

		m_cListenSocket.Open();
		LOGMSG("Point thread started");
	}catch(...) { LOGCATCH("Point thread init exception, exiting");}
}

bool	CPointThread::OnProcess()
{
	LOCKTHREAD;

	try{
		time_t	tStart = clock();
		fd_set	readmask;
		FD_ZERO(&readmask);
		if(!m_cListenSocket.IsOpen())
			m_cListenSocket.Rebuild();
		if(m_cListenSocket.IsOpen())
			FD_SET(m_cListenSocket.Socket(), &readmask);
		for(int i = 1; i < MAXGAMESERVERS; i++)
		{
			if(m_aServerSocket[i].IsOpen())
				FD_SET(m_aServerSocket[i].Socket(), &readmask);
		}

		struct timeval	timeout = {0,0};
		int ret = select(FD_SETSIZE, &readmask, (fd_set *) 0, (fd_set *) 0, &timeout);

		if(ret == -1)
		{
			m_cListenSocket.Close();
			LOGERROR("Point thread select error [%d]", WSAGetLastError());
			PrintText("Point thread error, socket closed; rebuilt automatically in %d seconds", REBUILDLISTENDELAYSEC);
		}
		else if(ret > 0 && FD_ISSET(m_cListenSocket.Socket(), &readmask))
		{
			FD_CLR(m_cListenSocket.Socket(), &readmask);
			ret--;
			u_long	nClientIP;
			SOCKET	sockNew = m_cListenSocket.Accept(nClientIP);
			if(sockNew != INVALID_SOCKET)
			{
				bool	bSuccess = false;
				int i;
				for(i =1; i < MAXGAMESERVERS; i++)
				{
					if(!m_aServerSocket[i].IsOpen())
					{
						bSuccess = true;
						break;
					}
				}

				struct in_addr	in;
				in.S_un.S_addr = nClientIP;
				char * pAddr = inet_ntoa(in);
				if(bSuccess)
				{
					m_aServerSocket[i].Open(sockNew, nClientIP);
					m_aHeartbeat[i] = clock();
					LOGMSG("New game server connected, server ID [%d], IP [%s]", i, pAddr);
					::SetServerState(i, c_flagSocket);
				}
				else
				{
					LOGWARNING("New connection [%s] received, but the connection table is full. Raise MAXGAMESERVERS and rebuild."
								, pAddr);
					struct linger ling;
					ling.l_onoff = 1;
					ling.l_linger = 0;
					setsockopt(sockNew, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));
					closesocket(sockNew);
					InterlockedDecrement(&s_nSocketCount);
				}
			}
		}

		for(int i = 1; i < MAXGAMESERVERS; i++)
		{
			if(ret <= 0)
				break;

			if(m_aServerSocket[i].IsOpen())
			{
				if(FD_ISSET(m_aServerSocket[i].Socket(), &readmask))
				{
					FD_CLR(m_aServerSocket[i].Socket(), &readmask);
					ret--;
					char	bufMsg[_MAX_MSGSIZE];
					int		nMsgLen = 0;
					if(m_aServerSocket[i].Recv(bufMsg, nMsgLen))
					{
						if(nMsgLen)
						{
							m_aHeartbeat[i] = clock();
							m_aHeartbeatLast[i] = 0;
							ProcessMsg(i, bufMsg, nMsgLen);
						}
					}
					else
					{
						LOGWARNING("Recv error, server [%s] closed.", m_aServerName[i]);
						LOGSERVER("Disconnected, server [%s] closed.", m_aServerName[i]);
						PrintText("Server \"%s\" closed.", m_aServerName[i]);
						Clear(i);
					}
				}
			}
		}
		long	nUsed = clock() - tStart;
		InterlockedExchangeAdd(&s_nAllTimeSum, nUsed);
		InterlockedIncrement(&s_nPointCount);

		long	nRemain = POINTLOOPDELAY - nUsed;
		if(nRemain < 0 || nRemain > POINTLOOPDELAY)
			nRemain = 0;
		if(nRemain)
			Sleep(nRemain);

		return true;

	}catch(...) { 
		LOGCATCH("Point thread main loop exception, still running");
		PrintText("Point thread main loop error, still running...");
		return true; 
	}
}

void	CPointThread::OnDestroy()
{
	LOCKTHREAD;

	try{
		m_cListenSocket.Close();
		for(int i = 1; i < MAXGAMESERVERS; i++)
		{
			if(m_aServerSocket[i].IsOpen())
				m_aServerSocket[i].Close();
		}
		LOGMSG("Point thread exited");

	}catch(...) { LOGCATCH("Point thread exit exception"); }

}

int	CPointThread::NewLogin(OBJID idAccount, DWORD nAuthenID, const char * szClientIP, const char * pGameServer)
{
	LOCKTHREAD;

	CMsgConnect	cMsg;
	ASSERT(IPSTRSIZE == _MAX_NAMESIZE);
	cMsg.Create(idAccount, nAuthenID, (char *)szClientIP);
	OBJID		nIndex = GetServerIndex_0(pGameServer);
	if(nIndex && m_aServerSocket[nIndex].IsOpen())
	{
		if(m_aState[nIndex] == STATE_NORMAL || idAccount <= MAX_GM_ID)
		{
			m_aServerSocket[nIndex].Send(cMsg.GetBuf(), cMsg.GetSize());
			return ERR_NONE;
		}
		else if(m_aState[nIndex] == STATE_BUSY)
			return ERR_BUSY;
		else if(m_aState[nIndex] == STATE_FULL)
			return ERR_FULL;
	}

	return ERR_NO_SERVER;
}

bool	CPointThread::GetServerIP(char * bufIP, const char * pServerName)
{
	LOCKTHREAD;

	int i;
	for(i = 1; i < MAXGAMESERVERS; i++)
	{
		if(strcmp(m_aServerName[i], pServerName) == 0)
			break;
	}

	if(bufIP && i < MAXGAMESERVERS)
	{
		if(m_aServerSocket[i].IsOpen())
		{
			strcpy(bufIP, m_aServerSocket[i].GetPeerIP());
			return true;
		}
		else
		{
			if(m_aServerName[i][0])
			{
				LOGWARNING("Getting IP: game server [%s] already closed", m_aServerName[i]);
				LOGSERVER("Server [%s] has closed", m_aServerName[i]);
				PrintText("Server \"%s\" has closed", m_aServerName[i]);
			}
			Clear(i);
		}
	}

	bufIP[0] = 0;
	return false;
}

int	CPointThread::GetServerState(LPCTSTR szServerName)
{
	LOCKTHREAD;

	int	nIndex = GetServerIndex_0(szServerName);
	if(nIndex)
		return m_aState[nIndex];
	else
		return STATE_OFFLINE;
}

int	CPointThread::GetServerCount()
{
	LOCKTHREAD;

	bool	nCount = 0;
	for(int i = 1; i < MAXGAMESERVERS; i++)
	{
		if(m_aState[i] != STATE_OFFLINE)
			nCount++;
	}

	return nCount;
}

bool CPointThread::CheckHeartbeatAll()
{
	LOCKTHREAD;

	bool	ret = false;
	for(int i = 1; i < MAXGAMESERVERS; i++)
	{
		if(m_aServerSocket[i].IsOpen())
		{
			CheckHeartbeat(i);
			ret = true;
		}
	}
	return ret;
}

void	CPointThread::LogCount()
{
	LOCKTHREAD;

	char	bufTime[20];
	DateTime(bufTime, time(NULL));
	char	bufLine[4096]="";
	int		nAmount = g_pOnlineTable->GetPlayerCount();
	for(int i = 1; i < g_nServerAccount; i++)
	{
		int	nCount = g_pOnlineTable->GetPlayerCount(g_aServerAccount[i].m_szServerName);
		int	nState = g_pPointThread->GetServerState(g_aServerAccount[i].m_szServerName);
		if(nState == CPointThread::STATE_OFFLINE)
		{
			nAmount	-= nCount;
			nCount	= 0;
		}
		char	buf[256];
		sprintf(buf, " | %12d", nCount);
		strcat(bufLine, buf);
	}
	strcat(bufLine, "\n");

	char	buf[4096];
	sprintf(buf, "%s | %12d", bufTime, nAmount);
	strcat(buf, bufLine);
	log_SaveFile("count", buf, g_szCountHeadLine);
}

void	CPointThread::LogSerialCount()
{
	LOCKTHREAD;

	char	bufTime[20];
	DateTime(bufTime, time(NULL));
	char	bufLine[4096]="";
	int		nAmount = 0;
	int		nCount = 0;
	for(COnlineTable::SERIAL_INDEX::iterator iter = g_pOnlineTable->GetSerialSet()->Begin();
			iter != g_pOnlineTable->GetSerialSet()->End();
			iter++, nCount++)
	{
		char	buf[256];
		sprintf(buf, " | %4d:%4d", iter->first, iter->second);
		strcat(bufLine, buf);
		nAmount	+= iter->second;
		if(strlen(bufLine) > 4000)
			break;
		if(nCount > 50000)
		{
			LOGERROR("★LogSerialCount() stuck★");
			break;
		}
	}
	strcat(bufLine, "\n");

	char	buf[4196];
	sprintf(buf, "%s | %8d", bufTime, nAmount);
	strcat(buf, bufLine);
	log_SaveFile("serial", buf, "Time                |  Total   | Serial:Count\n");
}

void	CPointThread::LogServer()
{
	LOCKTHREAD;

	char	bufTime[20];
	DateTime(bufTime, time(NULL));
	char	bufLine[4096];
	sprintf(bufLine, "%s | %12d", bufTime, GetServerCount());
	for(int i = 1; i < g_nServerAccount; i++)
	{
		BOOL	bState = GetServerState(g_aServerAccount[i].m_szServerName);
		char	buf[256];
		if(bState != STATE_OFFLINE)
			sprintf(buf, " | %12s", "√");
		else
			sprintf(buf, " | %12s", "╳");
		strcat(bufLine, buf);
	}
	strcat(bufLine, "\n");

	log_SaveFile("server", bufLine, g_szCountHeadLine);
}

bool	CPointThread::Kickout(OBJID idAccount)
{
	LOCKTHREAD;

	char	bufServer[256];
	if(g_pOnlineTable->GetServerName(idAccount, bufServer))
	{
		return Kickout_0(idAccount, bufServer);
	}
	return false;
}

bool	CPointThread::Kickout_0(OBJID idAccount, LPCTSTR szServerName)
{
	int nIndex = GetServerIndex_0(szServerName);

	if(nIndex != 0 )
	{
		CMsgFee	cMsg;
		cMsg.Create(idAccount, CMsgFee::FEE_KICKOUT);
		m_aServerSocket[nIndex].Send(cMsg.GetBuf(), cMsg.GetSize());
		return true;
	}

	return false;
}

bool	CPointThread::ProcessMsg(int nServerIndex,  char * pBuf, int nLen)
{
	unsigned short unMsgType = CNetMsg::GetType(pBuf, nLen);
	switch(unMsgType)
	{
	case _MSG_ACCOUNT:
	case _MSG_ACCOUNT_A:
		{
			CMsgAccount	cMsg;
			cMsg.Create(pBuf, nLen);
			FormatStr(cMsg.m_pInfo->szAccount, _MAX_NAMESIZE);
			FormatStr(cMsg.m_pInfo->szPassword, _MAX_NAMESIZE);
			FormatStr(cMsg.m_pInfo->szServer, _MAX_NAMESIZE);
			LOGMSG("Got ACCOUNT message from game server [%s]. Account [%s], password [***]."
							, cMsg.m_pInfo->szServer
							, cMsg.m_pInfo->szAccount
							);

			if(strlen(cMsg.m_pInfo->szAccount) == 0 
						|| strlen(cMsg.m_pInfo->szPassword) == 0 
						|| strlen(cMsg.m_pInfo->szServer) == 0)
			{
				LOGERROR("Bad login message [%s][%s][%s]",
						cMsg.m_pInfo->szAccount, cMsg.m_pInfo->szPassword, cMsg.m_pInfo->szServer);
				return false;
			}

			GetServerAccount();

			int		nAccount = 0;
			for(int i = 1; i < g_nServerAccount; i++)
			{
				if(!g_aServerAccount[i].m_b91U
						&& strcmp(g_aServerAccount[i].m_szServerName, cMsg.m_pInfo->szServer) == 0)
				{
					nAccount = i;
					break;
				}
			}

			CMsgLogin	cRetMsg;
			if(nAccount
						&& strcmp(g_aServerAccount[nAccount].m_szLoginName, cMsg.m_pInfo->szAccount) == 0
						&& strcmp(g_aServerAccount[nAccount].m_szPassword, cMsg.m_pInfo->szPassword) == 0)
			{
				for(int i = 1; i < MAXGAMESERVERS; i++)
				{
					if(strcmp(g_aServerAccount[nAccount].m_szServerName, m_aServerName[i]) == 0)
					{
						if(m_aServerSocket[i].IsOpen())
						{
							LOGMSG("Game server [%s] ACCOUNT login again; the previous connection [%d] is closed.", m_aServerName[i], i);
							m_aServerSocket[i].Close();
							Clear(i);
						}
						m_aServerName[i][0] = 0;
					}
				}

				strcpy(m_aServerName[nServerIndex], g_aServerAccount[nAccount].m_szServerName);
				PrintText("Game server #%d \"%s\" logged in", nServerIndex, m_aServerName[nServerIndex]);
				LOGSERVER("Game server #%d \"%s\" logged in", nServerIndex, m_aServerName[nServerIndex]);
				cRetMsg.Create(nServerIndex, 0, "", ACCOUNTVERSION);
				m_aServerSocket[nServerIndex].Send(cRetMsg.GetBuf(), cRetMsg.GetSize());
				LOGMSG("New game server [%s] ACCOUNT accepted. Server ID [%d], game version [%04X]",
											m_aServerName[nServerIndex], nServerIndex, ACCOUNTVERSION);
				LOGMSG("New game server [%s] ACCOUNT logging in......", m_aServerName[nServerIndex]);
				::SetServerState(nServerIndex, c_flagAccount);

			}
			else
			{
				cRetMsg.Create(0, 0, "", ACCOUNTVERSION);
				m_aServerSocket[nServerIndex].Send(cRetMsg.GetBuf(), cRetMsg.GetSize());
				LOGMSG("New game server [%s] login failed", cMsg.m_pInfo->szServer);
			}
		}
		break;
	case _MSG_LOGIN:
		{
			if(g_aServerAccount[nServerIndex].m_b91U)
			{
				LOGERROR("★Unexpected _MSG_LOGIN message★");
				return false;
			}

			m_aState[nServerIndex] = STATE_NORMAL;

			LOGMSG("......game server [%s] LOGIN OK", m_aServerName[nServerIndex]);
			LOGMSG("Got login message. New game server [%s] LOGIN OK", m_aServerName[nServerIndex]);
			::SetServerState(nServerIndex, c_flagNormal);
		}
		break;
	case _MSG_FEE:
		{
			CMsgFee		cMsg;
			cMsg.Create(pBuf, nLen);
			OBJID	idAccount = cMsg.m_pInfo->idAccount;

			if(g_aServerAccount[nServerIndex].m_b91U && cMsg.m_pInfo->ucType != CMsgFee::HEARTBEAT)
			{
				LOGERROR("★Unexpected _MSG_FEE message★");
				return false;
			}

			switch(cMsg.m_pInfo->ucType)
			{
			case CMsgFee::FEE_BEGIN:
				{
					if(idAccount == ID_NONE)
					{
						LOGERROR("Bad FEE_BEGIN message [%d]★", idAccount);
						return false;
					}

					if(!g_pOnlineTable->IsOnline(idAccount))
					{
						DWORD	nAuthenID = 0xCCCCCCCC;
						::WaitForSingleObject(g_xDatabase, INFINITE);
						if(g_cDatabase.Create(idAccount))
						{
							char* pName = g_cDatabase.GetName();
							if(!g_pOnlineTable->AppendNew(idAccount, nAuthenID, "StartFee", pName, "", 
											c_typeNone, m_aServerName[nServerIndex], idAccount, pName))
								Kickout_0(idAccount, m_aServerName[nServerIndex]);
							else
								LOGERROR("Backfilled FEE_BEGIN, server [%s] account [%s][%d]. ISP or point mode will no longer deduct points★",
																m_aServerName[nServerIndex], pName, idAccount);
							g_cDatabase.Destroy();
						}
						else
						{
							LOGERROR("Game server [%s] sent FEE_BEGIN for unknown account [%d], not backfilled",
										m_aServerName[nServerIndex], idAccount);
						}
						::ReleaseMutex(g_xDatabase);
					}

					int nRet = g_pOnlineTable->StartFee(idAccount, m_aServerName[nServerIndex]);
					if(nRet < 0)
					{
						LOGERROR("...double login, extra START_FEE. Account [%d] kicked.", idAccount);
						Kickout_0(idAccount, m_aServerName[nServerIndex]);
					}

					if(m_aState[nServerIndex] == STATE_NORMAL)
						::SetServerState(nServerIndex, c_flagBegin);
					else
						::SetServerState(nServerIndex, c_flagBegin2);
				}
				break;
			case CMsgFee::FEE_END:
			case CMsgFee::FEE_OFFLINE:
				{
					if(idAccount == ID_NONE)
					{
						LOGERROR("Bad FEE_END message [%d]★", idAccount);
						return false;
					}

					g_pOnlineTable->EndFee(idAccount, m_aServerName[nServerIndex], 
											cMsg.m_pInfo->ucType == CMsgFee::FEE_OFFLINE);

					if(m_aState[nServerIndex] == STATE_NORMAL)
						::SetServerState(nServerIndex, c_flagEnd);
					else
						::SetServerState(nServerIndex, c_flagEnd2);
				}
				break;
			case CMsgFee::FEE_POINT:
			case CMsgFee::FEE_TICK:
				{
					if(idAccount == ID_NONE)
					{
						LOGERROR("Bad FEE_POINT message [%d]★", idAccount);
						return false;
					}

					if(!g_pOnlineTable->IsOnline(idAccount))
					{
						DWORD	nAuthenID = 0xCCCCCCCC;
						::WaitForSingleObject(g_xDatabase, INFINITE);
						if(g_cDatabase.Create(idAccount))
						{
							char* pName = g_cDatabase.GetName();
							if(!g_pOnlineTable->AppendNew(idAccount, nAuthenID, "PointFee", pName, "", 
											c_typeNone, m_aServerName[nServerIndex], idAccount, pName))
								Kickout_0(idAccount, m_aServerName[nServerIndex]);
							else
								LOGERROR("Backfilled FEE_POINT, server [%s] account [%s][%d]. ISP or point mode will no longer deduct points★",
												m_aServerName[nServerIndex], pName, idAccount);
							g_cDatabase.Destroy();
						}
						else
						{
							LOGERROR("Game server [%s] sent FEE_POINT for unknown account [%d], not backfilled",
										m_aServerName[nServerIndex], idAccount);
						}
						::ReleaseMutex(g_xDatabase);
					}

					int		nFeeType		= c_typeNone;
					char	szServerName[_MAX_NAMESIZE] = "(未知)";
					OBJID	idFeeAccount	= ID_NONE;
					char	szFeeAccount[_MAX_NAMESIZE] = "(未知)";
					int		nPoint			= 0;
					if(g_pOnlineTable->GetAttr2(idAccount, nFeeType, szServerName, idFeeAccount, szFeeAccount))
					{
						if((nFeeType == c_typePoint || nFeeType == c_typeNetBarPoint 
										|| nFeeType == c_typeISP) && cMsg.m_pInfo->ucType != CMsgFee::FEE_TICK)
						{
							if(strcmp(szServerName, m_aServerName[nServerIndex]) == 0)
							{
								bool	bOk		= false;
								::WaitForSingleObject(g_xDatabase, INFINITE);
								if(g_cDatabase.Create(idFeeAccount))
								{
									bOk		= true;
									nPoint	= g_cDatabase.GetPoint();
									g_cDatabase.Destroy();
								}
								::ReleaseMutex(g_xDatabase);
								
								if(bOk)
								{
									if(nPoint > 0)
									{
										if(g_cDatabase.DecPoint(idFeeAccount))
										{
											InterlockedIncrement(&s_nPointFee);
											InterlockedIncrement(&s_nPointSum);
										}
										else
										{
											LOGERROR("Database error, account ID [%d] cannot be charged.★", idFeeAccount);
											LOGPOINT("Database error, account ID [%d] cannot be charged.", idFeeAccount);
										}
									}
									else if(KICKOUT_USER_WHEN_NO_POINT)
									{
										LOGPOINT("Server [%s] player [%d] kicked: account [%s][%d] out of points.",
											m_aServerName[nServerIndex], idAccount, szFeeAccount, idFeeAccount);
										Kickout_0(idAccount, m_aServerName[nServerIndex]);
									}
								}
								else
								{
									LOGERROR("Game server [%s] sent a point message for unknown account [%d]. Points not recorded★",
										m_aServerName[nServerIndex], idFeeAccount);
									LOGPOINT("Game server [%s] sent a point message for unknown account [%d]. Points not recorded",
										m_aServerName[nServerIndex], idFeeAccount);
								}
							}
							else
							{
								LOGERROR("Double login. Player ID [%d] with account [%s][%d] logged in to [%s], but [%s] sent the point message. Player kicked★",
									idAccount, szFeeAccount, idFeeAccount, szServerName, m_aServerName[nServerIndex]);
								Kickout_0(idAccount, m_aServerName[nServerIndex]);
							}
						}

						int	nPointCount = g_pOnlineTable->PointFee(idAccount, m_aServerName[nServerIndex]);
						{
						}
						
						if(m_aState[nServerIndex] == STATE_NORMAL)
							::SetServerState(nServerIndex, c_flagPoint);
						else
							::SetServerState(nServerIndex, c_flagPoint2);
					}
					else
					{
						LOGERROR("Sharing conflict, FEE_POINT backfill login failed");
					}
				}
				break;
			case CMsgFee::HEARTBEAT:
				{

					LOGMSG("Got heartbeat from game server [%s].", m_aServerName[nServerIndex]);
					::SetServerState(nServerIndex, c_flagHeartbeat);
				}
				break;
			case CMsgFee::SERVER_BUSY:
				{
					SetServerBusy(nServerIndex);
				}
				break;
			case CMsgFee::SERVER_FULL:
				{
					SetServerFull(nServerIndex);
				}
				break;
			case CMsgFee::SERVER_CLOSE:
				{
					PrintText("Received the close message of server [%s].", m_aServerName[nServerIndex]);
				}
				break;
			default:
				LOGERROR("Game server [%s] bad fee message subtype [%d]", m_aServerName[nServerIndex], cMsg.m_pInfo->ucType);
			}
		}
		break;
	case _MSG_QUERYFEE:
		{
			if(g_aServerAccount[nServerIndex].m_b91U)
			{
				LOGERROR("★Unexpected _MSG_QUERYFEE message★");
				return false;
			}

			CMsgQueryFee	cMsg;
			cMsg.Create(pBuf, nLen);

			if(cMsg.m_pInfo->ucType == 103)
				return true;

			if(cMsg.m_pInfo->idAccount == ID_NONE)
			{
				LOGERROR("Bad _MSG_QUERYFEE message [%d]★", cMsg.m_pInfo->idAccount);
				return false;
			}

			int		nFeeType	= c_typeNone;
			DWORD	dwData		= 0;
			int		nTime		= 0;
			char	szServerName[_MAX_NAMESIZE];
			OBJID	idFeeAccount;
			char	szFeeAccount[_MAX_NAMESIZE];
			if(g_pOnlineTable->GetAttr2(cMsg.m_pInfo->idAccount, nFeeType, szServerName, idFeeAccount, szFeeAccount))
			{
				switch(nFeeType)
				{
				case	c_typePoint:
					::WaitForSingleObject(g_xDatabase, INFINITE);
					if(g_cDatabase.Create(cMsg.m_pInfo->idAccount))
					{
						dwData	= g_cDatabase.GetPoint();
						g_cDatabase.Destroy();
					}
					else
					{
						LOGERROR("Game server [%s] queried fee data of unknown account [%d]", m_aServerName[nServerIndex], cMsg.m_pInfo->idAccount);
					}
					::ReleaseMutex(g_xDatabase);
					break;
				case	c_typeTime:
					::WaitForSingleObject(g_xDatabase, INFINITE);
					if(g_cDatabase.Create(cMsg.m_pInfo->idAccount))
					{
						nTime	= g_cDatabase.GetPointTime();
						g_cDatabase.Destroy();
					}
					else
					{
						LOGERROR("Game server [%s] queried fee data of unknown account [%d]", m_aServerName[nServerIndex], cMsg.m_pInfo->idAccount);
					}
					::ReleaseMutex(g_xDatabase);
					break;
				case	c_typeNetBarTime:
				case	c_typeNetBarPoint:
				case	c_typeISP:
				case	c_typeFree:
					break;
				}
			}
			if(nFeeType == c_typeFree && (cMsg.m_pInfo->ucType == 100 || cMsg.m_pInfo->ucType == 0))
				nFeeType	= 100;
			cMsg.m_pInfo->ucType	= nFeeType;
			cMsg.m_pInfo->dwData	= dwData;
			cMsg.m_pInfo->nTime		= nTime;
			m_aServerSocket[nServerIndex].Send(cMsg.GetBuf(), cMsg.GetSize());
		}
		break;
	default:
		LOGERROR("Game server sent unknown message type [%d]", unMsgType);
		LOGPOINT("Got unknown message from game server [%s].", m_aServerName[nServerIndex], unMsgType);
	}

	return true;
}

OBJID	CPointThread::GetServerIndex_0(const char * pServerName)
{
	int i;
	for(i = 1; i < MAXGAMESERVERS; i++)
	{
		if(strcmp(m_aServerName[i], pServerName) == 0)
			break;
	}

	if(i < MAXGAMESERVERS)
	{
		if(m_aServerSocket[i].IsOpen())
		{
			return i;
		}
		else
		{
			if(m_aServerName[i][0])
			{
				LOGWARNING("Getting server ID: game server [%s] already closed", m_aServerName[i]);
				LOGSERVER("Server [%s] has closed.", m_aServerName[i]);
				PrintText("Server \"%s\" has closed.", m_aServerName[i]);
			}
			Clear(i);
		}
	}

	return 0;
}

bool	CPointThread::CheckHeartbeat(int nIndex)
{
	if((m_aState[nIndex] == STATE_BUSY || m_aState[nIndex] == STATE_FULL) && clock() > m_aServerDelay[nIndex])
	{
		m_aState[nIndex] = STATE_NORMAL;
		::SetServerState(nIndex, c_flagNormal);
	}

	bool	ret = false;
	if(m_aHeartbeatLast[nIndex] && (clock() - m_aHeartbeat[nIndex]) / CLOCKS_PER_SEC > HEARTBEATKICKSECS)
	{
		LOGWARNING("Game server [%s] timed out after [%d] seconds, connection dropped",
						m_aServerName[nIndex], (clock() - m_aHeartbeat[nIndex])/CLOCKS_PER_SEC);
		LOGSERVER("Timeout, server [%s] disconnected!", m_aServerName[nIndex]);
		PrintText("Response timed out, server \"%s\" disconnected!", m_aServerName[nIndex]);
		m_aServerSocket[nIndex].Close();
		Clear(nIndex);
		ret = true;
	}
	else if((clock() - m_aHeartbeat[nIndex]) / CLOCKS_PER_SEC > HEARTBEATINTERVALSECS
				&& (clock() - m_aHeartbeatLast[nIndex]) / CLOCKS_PER_SEC > HEARTBEATINTERVALSECS)
	{
		CMsgFee	cMsg;
		cMsg.Create(666666, CMsgFee::HEARTBEAT);
		if(m_aServerSocket[nIndex].Send(cMsg.GetBuf(), cMsg.GetSize()))
		{
			LOGERROR("Game server [%s] no response for [%d] seconds, heartbeat sent",
						m_aServerName[nIndex], (clock() - m_aHeartbeat[nIndex])/CLOCKS_PER_SEC);
		}
		else
		{
			LOGWARNING("Game server [%s] connection closed, cannot SEND heartbeat", m_aServerName[nIndex]);
			LOGSERVER("Heartbeat send failed, server [%s] disconnected!", m_aServerName[nIndex]);
			PrintText("Heartbeat failed, server \"%s\" disconnected!", m_aServerName[nIndex]);
			Clear(nIndex);
		}
		m_aHeartbeatLast[nIndex] = clock();
	}

	return ret;
}

void CPointThread::Clear(int nIndex, bool flag )
{ 
	m_aState[nIndex] = STATE_OFFLINE;
	m_aServerName[nIndex][0] = m_aHeartbeat[nIndex] = m_aHeartbeatLast[nIndex] = m_aServerDelay[nIndex] = 0; 
	if(flag)
		::SetServerState(nIndex, c_flagOffline);
}

void CPointThread::SetServerBusy(int nServerIndex) { 
	m_aServerDelay[nServerIndex] = clock() + SERVER_BUSY_DELAY_SECS*CLOCKS_PER_SEC;
	m_aState[nServerIndex] = STATE_BUSY;
	::SetServerState(nServerIndex, c_flagStop);
}

void CPointThread::SetServerFull(int nServerIndex) {
	m_aServerDelay[nServerIndex] = clock() + SERVER_FULL_DELAY_SECS*CLOCKS_PER_SEC;
	m_aState[nServerIndex] = STATE_FULL;
	::SetServerState(nServerIndex, c_flagStop);
}
