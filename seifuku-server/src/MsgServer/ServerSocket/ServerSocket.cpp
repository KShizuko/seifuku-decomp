#define _WINSOCKAPI_
#define FD_SETSIZE      2048
#include <winsock2.h>
#include <windows.h>
#pragma	warning(disable:4786)
#include "define.h"
#include "protocol.h"
#include "EncryptServer.h"
#include "SSocket.h"
#include "I_Shell.h"
#include "ServerSocket.h"
#include "I_MessagePort.h"
#include "basefunc.h"
#include "IniFile.h"

#define		MAX_USERSIZE	(FD_SETSIZE - 48)

bool CSocketKernel::Create(IMessagePort* pPort)
{
	m_idNpcSocket = SOCKET_NONE;
	m_pMsgPort = pPort;
	m_pMsgPort->Open();
	m_pListen = m_cListen.GetInterface();

	if(!ServerSocketInit())
		return false;
	CIniFile	ini(CONFIG_FILENAME, "System");
	int			nPort = ini.GetInt("SERVE_PORT");
	if(nPort == 0)
		nPort = SERVER_LISTEN_PORT;
	if(!m_pListen->Listen(nPort, SOCKET_SENDBUFSIZE))
		return false;

	return true;
}

bool CSocketKernel::ProcessMsg(OBJID idPacket, void* buf, int nType, int nFrom)
{
	switch(idPacket)
	{
	case	SOCKET_CLOSESOCKET:
		{
			SOCKET_ID	idSocket	= *(int*)buf;
			if(idSocket >= (int)m_setSocket.size())
				break;
			LOGDEBUG("DEBUG: socket kernel got close-network message, SOCKET_ID[%u]", idSocket);

			if(m_setSocket[idSocket])
			{
				m_setSocket[idSocket]->Close(true);
				m_setSocket[idSocket]		= NULL;
				CServerSocket::Destroy(m_setServerSocket[idSocket]);
				m_setProcessID[idSocket]	= PROCESS_NONE;
				m_setNetBreakFlag[idSocket]	= false;
				if(m_idNpcSocket == idSocket)
				{
					LOGDUMP("m_idNpcSocket(%d) = SOCKET_NONE", m_idNpcSocket);
					m_idNpcSocket = SOCKET_NONE;

					char*	pText = "NPC server broken connect.";
					m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(pText), pText);
				}
			}
		}
		break;
	case	SOCKET_SETPROCESSID:
		{
			const CHANGE_USERDATA*	pPack = (CHANGE_USERDATA*)buf;
			if(pPack->idSocket >= 0 && pPack->idSocket < m_setSocket.size() 
					&& m_setSocket[pPack->idSocket]
					&& !IsNetBreakFlag(pPack->idSocket) )
				m_setProcessID[pPack->idSocket] = pPack->nData;
			else
				m_pMsgPort->Send(nFrom, KERNEL_CLOSEKERNEL, VARTYPE_INT, &pPack->idSocket);
		}
		break;
	case	SOCKET_SENDCLIENTMSG:
		{
			const SENDCLIENTMSG_PACKET0* pPack = (SENDCLIENTMSG_PACKET0*)buf;
			if(pPack->idSocket >= (int)m_setSocket.size())
				break;

			if(m_setSocket[pPack->idSocket])
			{
				char buf[MAX_PACKETSIZE];
				int	nNetPacketSize = UnitePacket(buf, MAX_PACKETSIZE, pPack->idPacket, pPack->buf, pPack->nSize);
				MYASSERT(nNetPacketSize);
				if(nNetPacketSize)
				{
#ifdef	PALED_DEBUG
					OBJID	idPacket = *((unsigned short*)buf + 1);
					char	szText[256];
					sprintf(szText, "VVV sent message %u", idPacket);
					m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(szText), szText);
#endif
					DWORD	clkDebug = timeGetTime();
					m_setSocket[pPack->idSocket]->SendPacket(buf, nNetPacketSize);
					{
						extern struct STAT_STRUCT	g_stat;
						InterlockedExchangeAdd(&g_stat.nSocketBytes, nNetPacketSize);

					}
				}
			}
		}
		break;
	case	SOCKET_SENDNPCMSG:
		{
			const SENDNPCMSG_PACKET0* pPack = (SENDNPCMSG_PACKET0*)buf;
			OBJID	idNpc = pPack->idNpc;
			if(m_idNpcSocket != SOCKET_NONE)
			{
				char buf[MAX_PACKETSIZE];
				int	nLen = UniteNpcPacket(buf, MAX_PACKETSIZE, pPack->idPacket, pPack->buf, pPack->nSize, idNpc);
				MYASSERT(nLen);
				if(nLen)
				{
#ifdef	DUMP_NPC_MSG
					OBJID	idPacket = *((unsigned short*)buf + 1);
					char	szText[256];
					sprintf(szText, "VVV sent NPC message %u", idPacket);
					m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(szText), szText);
#endif
					DWORD	clkDebug = timeGetTime();
					m_setSocket[m_idNpcSocket]->SendPacket(buf, nLen);
					{
						extern struct STAT_STRUCT	g_stat;
						InterlockedExchangeAdd(&g_stat.nNpcSocketBytes, nLen);

					}
				}
				break;
			}
		}
		break;
	case	SOCKET_SETNPCSOCKETID:
		{
			SOCKET_ID	idSocket = *(SOCKET_ID*)buf;

			if(idSocket != m_idNpcSocket && m_idNpcSocket != SOCKET_NONE && m_setSocket[m_idNpcSocket])
			{
				LOGDUMP("m_idNpcSocket(%d)->Close()", m_idNpcSocket);
				m_setSocket[m_idNpcSocket]->Close();
				LOGWARNING("NPC server logged in twice; the previous SOCKET is closed!");
			}

			LOGDUMP("m_idNpcSocket(%d) = idSocket(%d)", m_idNpcSocket, idSocket);
			m_idNpcSocket = idSocket;
			LOGMSG("NPC server SOCKET index is %u.", idSocket);
			if(m_idNpcSocket >= 0 && m_idNpcSocket < m_setSocket.size() && m_setSocket[m_idNpcSocket])
				m_setSocket[m_idNpcSocket]->SetBufSize(SOCKET_NPCBUFSIZE, SOCKET_NPCBUFSIZE);
		}
		break;
	case	SOCKET_SETNPCPROCESSID:
		{
			const CHANGE_NPCPROCESSID*	pPack = (CHANGE_NPCPROCESSID*)buf;
			if(pPack->idProcess == PROCESS_NONE)
				m_setNpcProcessID.erase(pPack->idNpc);
			else
				m_setNpcProcessID[pPack->idNpc] = pPack->idProcess;
		}
		break;
	case	MAPGROUP_DELALLMONSTER:
		{
			m_setNpcProcessID.clear();
			LOGDUMP("Del all monster in socket thread.");
		}
		break;
	case	SOCKET_CHANGECODE:
		{
			const CHANGE_USERDATA*	pPack = (CHANGE_USERDATA*)buf;
			SOCKET_ID idSocket	= pPack->idSocket;
			DWORD	dwData		= pPack->nData;
			CHECKF(0 <= idSocket && idSocket < (SOCKET_ID)m_setSocket.size());
			m_setSocket[idSocket]->ChangeCode(dwData);
		}
		break;
	case	SOCKET_BREAKCONNECT:
		{
			SOCKET_ID	idSocket = *(SOCKET_ID*)buf;
			CHECKF(0 <= idSocket && idSocket < (SOCKET_ID)m_setSocket.size());
			LOGDEBUG("DEBUG: network kernel closes SOCKET at kernel request, SOCKET_ID[%d]", idSocket);
			if(m_setSocket[idSocket] && !IsNetBreakFlag(idSocket))
				m_setSocket[idSocket]->Close(true);
			else
				SendCloseNotify(idSocket);
		}
		break;
	case	SOCKET_BREAKALLCONNECT:
		{
			SOCKET_ID	idGmSocket = *(SOCKET_ID*)buf;
			LOGDEBUG("DEBUG: network kernel closes all SOCKETs at GM request, SOCKET_ID[%d]", idGmSocket);
			extern long g_nServerClosed;
			g_nServerClosed = true;
			for(int i = m_setSocket.size()-1; i >= 0; i--)
			{

				if(i != idGmSocket && i != m_idNpcSocket && m_setSocket[i])
				{
					LOGDUMP("m_setSocket[%d]->Close(true);", i);
					m_setSocket[i]->Close(true);
				}
			}
		}
		break;
	case	SOCKET_QUERYIP:
		{
			const ST_QUERYIP*	pIP = (ST_QUERYIP*)buf;
			CHECKF(pIP);
			ST_SOCKETIP	info;
			memset(&info, 0, sizeof(info));
			info.idSocket	= pIP->idSocket;
			CHECKF(0 <= pIP->idSocket && pIP->idSocket < (int)m_setSocket.size());
			info.dwIP		= m_setSocket[pIP->idSocket]->GetPeerIP();
			m_pMsgPort->Send(pIP->idProcess, MAPGROUP_SOCKETIP, STRUCT_TYPE(info), &info);
		}
		break;
	default:
		LOGERROR("CSocketKernel::ProcessMsg()");
		return false;
	}

	return true;
}

bool CSocketKernel::ProcessSocket()
{
	fd_set	readmask;
	FD_ZERO(&readmask);
	FD_SET(m_pListen->Socket(), &readmask);
	int i;	for(i = 0; i < m_setSocket.size(); i++)
	{
		if(!m_setSocket[i])
			continue;

		FD_SET(m_setSocket[i]->Socket(), &readmask);
	}

	int	nBlockMilli = 0;
	struct timeval	timeout;
	timeout.tv_sec	= (nBlockMilli*1000) / 1000000;
	timeout.tv_usec = (nBlockMilli*1000) % 1000000;
	int ret = select(FD_SETSIZE, &readmask, (fd_set *) 0, (fd_set *) 0, &timeout);

	extern long g_nMessagePortErrorCount;
	if(g_nMessagePortErrorCount < 5000 && FD_ISSET(m_pListen->Socket(), &readmask))
	{
		AcceptNewSocket();
	}

	for( i = m_setSocket.size()-1; i >= 0; i--)
	{
		if(!m_setSocket[i])
			continue;

		int		nBufLen = 0;
		BOOL	bHaveNetData = FD_ISSET(m_setSocket[i]->Socket(), &readmask);
		const char* pBuf = m_setSocket[i]->GetPacket(&nBufLen, bHaveNetData);

		if(!m_setSocket[i]->IsOpen())
		{
			if(!IsNetBreakFlag(i))
			{
				SendCloseNotify(i);
				SetNetBreakFlag(i);
				
				if(m_idNpcSocket == i)
				{
					LOGDUMP("m_setSocket(%d) = SOCKET_NONE", m_idNpcSocket);
					m_idNpcSocket = SOCKET_NONE;
					
					char*	pText = "NPC server broken connect!";
					m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(pText), pText);
				}
			}
			
			pBuf	= NULL;
			nBufLen = 0;
		}

		if(pBuf && nBufLen > 0)
		{
#ifdef	DUMP_DEBUG_MSG
			OBJID	idPacket = *(((short*)pBuf)+1);
			if(idPacket == 1002)
				LOGDUMP("LOGIN pocket(%u), idSocket(%d), m_idNpcSocket(%d)", idPacket, i, m_idNpcSocket);
#endif
			if(i != m_idNpcSocket)
			{
				OBJID	idPacket;
				char*	pMsg;
				int		nMsgSize;
				int		nNetPacketSize;
				bool	bSealed = false;
				if(( nNetPacketSize = SplitPacket(pBuf, nBufLen, &idPacket, &pMsg, &nMsgSize, bSealed) ) > 0)
				{
					char	bufPacket[MAX_MESSAGESIZE];
					CLIENTMSG_PACKET0* pPacket = (CLIENTMSG_PACKET0*)bufPacket;
					pPacket->idSocket	= i;
					pPacket->idPacket	= idPacket;
					pPacket->nSize		= nMsgSize;
					memcpy(pPacket->buf, pMsg, nMsgSize);
					m_pMsgPort->Send(m_setProcessID[i], KERNEL_CLIENTMSG, STRUCT_TYPE(bufPacket), bufPacket);

#ifdef	_DEBUGx
					char	szText[256];
					sprintf(szText, "AAA received message %u", idPacket);
					m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(szText), szText);
#endif
					m_setSocket[i]->ClearPacket(bSealed ? nNetPacketSize + PROTOCOL_MSG_SEALSIZE : nNetPacketSize);

					extern struct STAT_STRUCT	g_stat;
					InterlockedIncrement(&g_stat.nSocketPackets);
				}
				else if(nNetPacketSize < 0)
					m_setSocket[i]->Close();
			}
			else
			{
				do
				{
					OBJID	idNpc;
					OBJID	idPacket;
					char*	pMsg;
					int		nMsgSize;
					if(SplitNpcPacket(pBuf, nBufLen, &idPacket, &pMsg, &nMsgSize, &idNpc))
					{
						PROCESS_ID	idProcess = MSGPORT_WORLD;
						NPCPROCESSID_MAP::iterator iter = m_setNpcProcessID.find(idNpc);
						if(iter != m_setNpcProcessID.end())
							idProcess = iter->second;
#ifdef	DUMP_DEBUG_MSG
						if(idPacket == 1002)
							LOGDUMP("LOGIN AGENT %u in process id %d", idNpc, idProcess);
#endif

						char	bufPacket[MAX_MESSAGESIZE];
						NPCMSG_PACKET0* pPacket = (NPCMSG_PACKET0*)bufPacket;
						pPacket->idNpc		= idNpc;
						pPacket->idPacket	= idPacket;
						pPacket->nSize		= nMsgSize;
						memcpy(pPacket->buf, pMsg, nMsgSize);
						m_pMsgPort->Send(idProcess, KERNEL_NPCMSG, STRUCT_TYPE(bufPacket), bufPacket);

						int	nCurrMsgSize = PROTOCOL_NPCMSG_HEADSIZE+nMsgSize;
#ifdef	DUMP_NPC_MSG
						char	szText[256];
						sprintf(szText, "AAA received NPC message %u", idPacket);
						m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(szText), szText);
#endif
						IF_NOT(m_setSocket[i]->ClearPacket(nCurrMsgSize))
							break;

						extern struct STAT_STRUCT	g_stat;
						InterlockedIncrement(&g_stat.nNpcSocketPackets);
					}
					else
						break;

					bHaveNetData = false;
					pBuf = m_setSocket[i]->GetPacket(&nBufLen, bHaveNetData);
				}while(pBuf && nBufLen > 0);
			}
		}
	}

	return true;
}

bool CSocketKernel::OnTimer(time_t tCurr)
{

	return true;
}

bool CSocketKernel::Release()
{
	m_pListen->Close();
	for(int i = m_setSocket.size()-1; i >= 0; i--)
	{
		if(m_setSocket[i])
		{
			m_setSocket[i]->Close();
			CServerSocket::Destroy(m_setServerSocket[i]);
		}
	}

	ServerSocketFinal();
	m_pMsgPort->Close();

	delete this;
	return true;
}

void CSocketKernel::AcceptNewSocket()
{
	SOCKET socket = m_pListen->Accept();

	extern long g_nServerClosed;
	if(g_nServerClosed || m_setSocket.size() >= MAX_USERSIZE)
		return ;

	if(socket != INVALID_SOCKET)
	{
		CServerSocket* pSocket = CServerSocket::CreateNew(socket);
		if(pSocket)
		{
			BOOL	bInsertOk = false;
			int		i;
			for(i = 0; i < m_setSocket.size(); i++)
			{
				if(!m_setSocket[i])
				{
					m_setServerSocket[i]	= pSocket;
					m_setSocket[i]			= pSocket->GetInterface();
					m_setProcessID[i]		= MSGPORT_WORLD;
					m_setNetBreakFlag[i]	= false;
					bInsertOk	= true;
					break;
				}
			}
			if(!bInsertOk)
			{
				m_setServerSocket.push_back(pSocket);
				m_setSocket.push_back(pSocket->GetInterface());
				m_setProcessID.push_back(MSGPORT_WORLD);
				m_setNetBreakFlag.push_back(false);
				if(m_setSocket.size() >= MAX_USERSIZE)
					LOGERROR("Player SOCKET count reached [%d]★", MAX_USERSIZE);
				i = m_setSocket.size() - 1;
			}

			char	szText[32] = "";
			sprintf(szText, "%u %u", i, pSocket->GetInterface()->GetPeerIP());
			m_pMsgPort->Send(MSGPORT_WORLD, WORLD_NEWSOCKET, STRING_TYPE(szText), szText);
		}
	}
}

bool CSocketKernel::SendCloseNotify(SOCKET_ID idSocket)
{
	LOGDEBUG("DEBUG: network kernel SOCKET closed, kick SOCKET_ID[%d]", idSocket);
#ifdef	NEW_LOGOUT
	m_pMsgPort->Send(m_pMsgPort->GetSize()-1, KERNEL_CLOSEKERNEL, VARTYPE_INT, &idSocket);
#else
	if(m_setProcessID[idSocket] == MSGPORT_WORLD)
		m_pMsgPort->Send(MSGPORT_WORLD, KERNEL_CLOSEKERNEL, VARTYPE_INT, &idSocket);
	else
	{
		for(int j = MSGPORT_MAPGROUP_FIRST; j < m_pMsgPort->GetSize(); j++)
			m_pMsgPort->Send(j, KERNEL_CLOSEKERNEL, VARTYPE_INT, &idSocket);
	}
#endif

	return true;
}

ISocketKernel* ISocketKernel::CreateNew()
{
	return (new CSocketKernel)->GetInterface();
}
