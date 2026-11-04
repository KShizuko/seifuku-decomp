#define	_WINSOCKAPI_
#include "inifile.h"
#include "protocol.h"
#include "windows.h"
#include "Winuser.h"
#include "BaseFunc.h"
#include "I_MessagePort.h"
#include "AllMsg.h"
#include "NetMsg.h"
#include "MessageBoard.h"
#include "WorldKernel.h"
#include "ClientSocket.h"
#include "MsgAccount_A.h"
#include "MsgConnect_A.h"
#include "MsgLogin_A.h"
#include "MsgFee_A.h"
#include "MsgQueryFee_A.h"
#include "SynWorldManager.h"
#include "SupermanList.h"
#include "AutoPtr.h"

using namespace world_kernel;

const int	MAX_TOKENSIZE	= 32;
static char* SkipSpace(char* pStr)
{
	while((signed char)*pStr <= ' ')
	{
		if(*pStr == 0)
			return NULL;
		pStr++;
	}
	return pStr;
}

static char* GetToken(char** ppStr)
{
	static char	szToken[MAX_TOKENSIZE];
	szToken[0]	= 0;
	int	nLen	= 0;
	if(!*ppStr)
		return NULL;
	char* pStr = SkipSpace(*ppStr);
	if(!pStr)
		return NULL;

	do
	{
		if(nLen < MAX_TOKENSIZE)
			szToken[nLen++]	= *pStr;
	}
	while((signed char)*(++pStr) > ' ');

	if(nLen == MAX_TOKENSIZE)
	{
		LOGERROR("Token exceeded %i chars, discarded.\n", MAX_TOKENSIZE);
		nLen	= 0;
	}
	szToken[nLen]	= 0;
	*ppStr	= pStr;
	return szToken;
}

CWorldKernel*	CWorldKernel::m_pWorld = NULL;

CWorldKernel::CWorldKernel()
{
	m_idProcess		= 0;
	m_pDb			= NULL;
	m_pMsgPort		= NULL;
	m_pUserList		= NULL;
	m_pMapList		= NULL;
	m_pSynManager	= NULL;
	m_idNpcSocket	= SOCKET_NONE;

	m_setConnectClient	= NULL;
	m_setConnectAccount	= NULL;
	m_idServer		= ID_NONE;
	m_nState		= STATE_NONE;
	m_szServer[0]	= 0;
	m_szAccount[0]	= 0;
	m_szPassword[0]	= 0;
	m_nServePort	= 5816;
}

bool CWorldKernel::Create(IMessagePort* pPort)
{
	m_pMsgPort		= pPort;
	m_idProcess		= m_pMsgPort->GetID();
	m_pMsgPort->Open();
	MYASSERT(m_idProcess == MSGPORT_WORLD);
	m_idNpcSocket	= SOCKET_NONE;

	char	DB_IP	[DBSTR_SIZE] = "";
	char	DB_USER	[DBSTR_SIZE] = "";
	char	DB_PW	[DBSTR_SIZE] = "";
	char	DB_DB	[DBSTR_SIZE] = "";
	CIniFile	ini(CONFIG_FILENAME, "Database");
	ini.GetString(DB_IP,	"DB_IP",	DBSTR_SIZE);
	ini.GetString(DB_USER,	"DB_USER",	DBSTR_SIZE);
	ini.GetString(DB_PW,	"DB_PW",	DBSTR_SIZE);
	ini.GetString(DB_DB,	"DB_DB",	DBSTR_SIZE);
	int		DB_PORT	= ini.GetInt("DB_PORT");
	if(DB_PORT < 0)
		DB_PORT	= 0;
	m_pDb	= ::CreateDatabase(DB_IP, DB_USER, DB_PW, DB_DB, DB_PORT, true);
	if(!m_pDb)
	{
		LOGERROR("Database connection failed!");
		return false;
	}

	m_pUserList	= CUserList::CreateNew();
	if(!m_pUserList)
		return false;
	m_pUserList->Create(GetSocketInterface(), m_pDb);

	m_pMapList	= CMapList::CreateNew();
	if(!m_pMapList)
		return false;
	m_pMapList->Create(m_pDb);

	m_pWorld	= this;

	m_pSynManager	= new CSynWorldManager;
	CHECKF(m_pSynManager);
	m_pSynManager->Create(m_pDb);

	m_setConnectClient		= CConnectSet::CreateNew(true);
	m_setConnectAccount		= CConnectSet::CreateNew(true);

	ini.SetSection("System");
	int nServePort = ini.GetInt("SERVE_PORT");
	if(nServePort)
		m_nServePort	= nServePort;

	ini.SetSection("AccountServer");
	NAMESTR		ACCOUNT_IP;
	int			ACCOUNT_PORT;

	ini.GetString(ACCOUNT_IP, "ACCOUNT_IP", _MAX_NAMESIZE);
	ACCOUNT_PORT	= ini.GetInt("ACCOUNT_PORT");
	ini.GetString(m_szServer, "SERVERNAME", _MAX_NAMESIZE);
	ini.GetString(m_szAccount, "LOGINNAME", _MAX_NAMESIZE);
	ini.GetString(m_szPassword, "PASSWORD", _MAX_NAMESIZE);
	if(ACCOUNT_PORT == 0)
	{
		LOGERROR("INI configuration error!");
		return false;
	}
	if(m_sock.Open(ACCOUNT_IP, ACCOUNT_PORT, 0, SOCKET_ACCOUNTBUFSIZE, SOCKET_ACCOUNTBUFSIZE))
		m_nState = STATE_CONNECTOK;

	m_ptrTradeMsgBd		= CMessageBoard::CreateNew(_TXTATR_MSG_TRADE);
	m_ptrFriendMsgBd	= CMessageBoard::CreateNew(_TXTATR_MSG_FRIEND);
	m_ptrTeamMsgBd		= CMessageBoard::CreateNew(_TXTATR_MSG_TEAM);
	m_ptrOtherMsgBd		= CMessageBoard::CreateNew(_TXTATR_MSG_OTHER);
	m_ptrSystemMsgBd	= CMessageBoard::CreateNew(_TXTATR_MSG_SYSTEM);

	m_tUserAmount.Startup(30);

	LoadConfig(m_pDb);
	LoadForbidName(m_pDb);
	m_pUserList->LoadGmIP();

	return true;
}

bool CWorldKernel::ProcessMsg(OBJID idPacket, void* buf, int nType, int nFrom)
{
	switch(idPacket)
	{
	case	KERNEL_CLIENTMSG:
		{
			CLIENTMSG_PACKET0*	pMsg = (CLIENTMSG_PACKET0*)buf;
			SOCKET_ID	idSocket= pMsg->idSocket;
			OBJID	idMsg	= pMsg->idPacket;
			char*	pBuf	= pMsg->buf;
			int		nMsgLen	= pMsg->nSize;
			ProcessClientMsg(idSocket, idMsg, pBuf, nMsgLen);
		}
		break;
	case	WORLD_TRANSMITMSG:
		{
			TRANSMSG_PACKET0*	pMsg = (TRANSMSG_PACKET0*)buf;
			SOCKET_ID	idSocket= pMsg->idSocket;
			OBJID		idNpc	= pMsg->idNpc;
			OBJID	idMsg	= pMsg->idPacket;
			char*	pBuf	= pMsg->buf;
			int		nMsgLen	= pMsg->nSize;
			int		nTrans	= pMsg->nTrans;
			if(idSocket == SOCKET_NONE)
				ProcessNpcMsg(idNpc, idMsg, pBuf, nMsgLen, nTrans);
			else
				ProcessClientMsg(idSocket, idMsg, pBuf, nMsgLen, nTrans);
		}
		break;
	case	KERNEL_NPCMSG:
		{
			NPCMSG_PACKET0*	pMsg = (NPCMSG_PACKET0*)buf;
			OBJID	idNpc	= pMsg->idNpc;
			OBJID	idMsg	= pMsg->idPacket;
			char*	pBuf	= pMsg->buf;
			int		nMsgLen	= pMsg->nSize;
			ProcessNpcMsg(idNpc, idMsg, pBuf, nMsgLen);
		}
		break;
	case	WORLD_SHELLTALK:
		{
			char* pText = (char*)buf;
			m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(pText), pText);

			char szCmd[256] = "";
			if (sscanf(pText, "%s", szCmd) == 1)
			{
				if (0 == stricmp(szCmd, "setmaxplayer"))
				{
					int nMaxPlayers = 0;
					if (2 == sscanf(pText, "%s %d", szCmd, &nMaxPlayers))
					{
						extern DWORD g_dwMaxPlayer;
						InterlockedExchange((long*)&g_dwMaxPlayer, nMaxPlayers);
						{
							char szMsg[256] = "";
							sprintf(szMsg, "Set max player limit to %d", g_dwMaxPlayer);
							m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(szMsg), szMsg);
						}
					}
				}
				else if (0 == stricmp(szCmd, "kickoutcheat"))
				{
					int	nData = 0;
					if (2 == sscanf(pText, "%s %d", szCmd, &nData))
					{
						extern long	g_sKickoutCheat;
						long nOld = InterlockedExchange(&g_sKickoutCheat, nData);
					}
				}
				else if (0 == stricmp(szCmd, "kickoutall"))
				{
					OBJID idGmSocket = -1;
					m_pMsgPort->Send(MSGPORT_SOCKET, SOCKET_BREAKALLCONNECT, VARTYPE_INT, &idGmSocket);
				}
				else if (0 == stricmp(szCmd, "broadcast"))
				{
					char* pCmd = pText + strlen(szCmd);
					if(*pCmd == ' ')
						pCmd++;

					GameWorld()->QueryUserList()->BroadcastMsg(pCmd, NULL, NULL, 0x00FF0000, _TXTATR_GM);
				}
				else if (0 == stricmp(szCmd, "shutdown"))
				{
					GameWorld()->QueryUserList()->BroadcastMsg(STR_SHUTDOWN_NOTIFY, NULL, NULL, 0x00FF0000, _TXTATR_GM);

					char* pMsg = "Send shut down broadcast message.";
					m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(pMsg), pMsg);
				}
				else if (0 == stricmp(szCmd, "restartserver"))
				{
					extern long g_nServerClosed;
					if(g_nServerClosed)
					{
						extern long g_nRestart;
						g_nRestart = true;
					}
					else
					{
						LPCTSTR pMsg = "Please <kickoutall> before <restartserver>!";
						m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(pMsg), pMsg);
					}
				}
				else if (0 == stricmp(szCmd, "restartwindows"))
				{
					extern long g_nServerClosed;
					if(g_nServerClosed)
					{
						HANDLE hToken;
						TOKEN_PRIVILEGES tkp;

						if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
						{
							LOGERROR("OpenProcessToken");
							break;
						}

						LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
						tkp.PrivilegeCount = 1;
						tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

						AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

						if (GetLastError() != ERROR_SUCCESS)
						{
							LOGERROR("AdjustTokenPrivileges");
							break;
						}

#ifndef EWX_FORCEIFHUNG
#define EWX_FORCEIFHUNG      0x00000010
#endif
						if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG, 0))
						{
							LOGERROR("ExitWindowsEx");
							break;
						}

					}
					else
					{
						LPCTSTR pMsg = "Please <kickoutall> before <restartserver>!";
						m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(pMsg), pMsg);
					}
				}
			}
		}
		break;
	case	WORLD_NEWSOCKET:
		{
			SOCKET_ID	idSocket	= 0;
			DWORD		dwIP		= 0;
			if(sscanf((char*)buf, "%u %u", &idSocket, &dwIP) == 2)
				m_setSocketIP[idSocket]	= dwIP;
		}
		break;
	case	KERNEL_CLOSEKERNEL:
		{
			SOCKET_ID	idSocket = *(SOCKET_ID*)buf;
			LOGDEBUG("DEBUG: world kernel got close-kernel message, SOCKET_ID[%u]", idSocket);
			if(g_UserList.IsLoginMapGroup(idSocket))
			{
				for(int i = MSGPORT_MAPGROUP_FIRST; i < m_pMsgPort->GetSize(); i++)
					m_pMsgPort->Send(i, KERNEL_CLOSEKERNEL, VARTYPE_INT, &idSocket);
			}
			else
				m_pMsgPort->Send(MSGPORT_SOCKET, SOCKET_CLOSESOCKET, VARTYPE_INT, &idSocket);
		}
		break;
	case	WORLD_CLOSESOCKET:
		{
			SOCKET_ID	idSocket = *(SOCKET_ID*)buf;
			LOGDEBUG("DEBUG: world kernel got close-network message, SOCKET_ID[%u]", idSocket);
			g_UserList.LogoutUser(idSocket);
			m_pMsgPort->Send(MSGPORT_SOCKET, SOCKET_CLOSESOCKET, VARTYPE_INT, &idSocket);
		}
		break;
	case	WORLD_SETPROCESSID:
		{
			CHANGE_USERDATA* pPakcet = (CHANGE_USERDATA*)buf;
			g_UserList.ChangeProcessID(pPakcet->idSocket, pPakcet->nData);
		}
		break;
	case	WORLD_USERNAMEMSG:
		{
			USERNAMEMSG_PACKET0*	pMsg = (USERNAMEMSG_PACKET0*)buf;
			SOCKET_ID	idSocket= pMsg->idSocket;
			OBJID	idMsg	= pMsg->idPacket;
			char*	pBuf	= pMsg->buf;
			int		nMsgLen	= pMsg->nSize;
			CPlayer* pPlayer = UserList()->GetPlayer(pMsg->szName);
			if(pPlayer)
			{
				if(!pPlayer->IsAgent())
					SendMsg(pPlayer->idSocket, idMsg, pBuf, nMsgLen);
				else
					SendNpcMsg(pPlayer->GetID(), idMsg, pBuf, nMsgLen);
			}
			else if(idMsg == _MSG_TALK && idSocket != SOCKET_NONE)
			{
				CPlayer* pSender = UserList()->GetPlayerBySocket(idSocket);
				if(pSender)
				{
					CMsgTalk	msg;
					IF_OK(msg.Create(SYSTEM_NAME, pSender->szName, STR_TARGET_OFFLINE))
						pSender->SendMsg(&msg);
				}
			}
		}
		break;
	case	WORLD_USERIDMSG:
		{
			USERIDMSG_PACKET0*	pMsg = (USERIDMSG_PACKET0*)buf;
			SOCKET_ID	idSocket= pMsg->idSocket;
			OBJID	idMsg	= pMsg->idPacket;
			char*	pBuf	= pMsg->buf;
			int		nMsgLen	= pMsg->nSize;
			CPlayer* pPlayer = UserList()->GetPlayer(pMsg->idUser);
			if(pPlayer)
			{
				if(!pPlayer->IsAgent())
					SendMsg(pPlayer->idSocket, idMsg, pBuf, nMsgLen);
				else
					SendNpcMsg(pPlayer->GetID(), idMsg, pBuf, nMsgLen);
			}
		}
		break;
	case	KERNEL_CREATESYNDICATE:
		{
			CreateSyndicateInfo* pPacket = (CreateSyndicateInfo*)buf;
			GameWorld()->QuerySynManager()->QueryModify()->CreateSyndicate(pPacket);
		}
		break;
	case	KERNEL_DESTROYSYNDICATE:
		{
			OBJID idSyn		= *(OBJID*)buf;
			GameWorld()->QuerySynManager()->QueryModify()->DestroySyndicate(idSyn);
		}
		break;
	case	KERNEL_COMBINESYNDICATE:
		{
			OBJID* pId	= (OBJID*)buf;
			OBJID idSyn		= pId[0];
			OBJID idTarget	= pId[1];
			GameWorld()->QuerySynManager()->QueryModify()->CombineSyndicate(idSyn, idTarget);
		}
		break;
	case	KERNEL_CHANGESYNDICATE:
		{
			SynFuncInfo0* pPacket = (SynFuncInfo0*)buf;
			CSyndicateWorld* pSyn = GameWorld()->QuerySynManager()->QuerySyndicate(pPacket->idSyn);
			IF_OK(pSyn)
			{
				pSyn->ChangeSyndicate(pPacket);
			}
		}
		break;
	case	WORLD_LEVELUP:
		{
			OBJID idUser = *(OBJID*)buf;
			CPlayer* pPlayer = GameWorld()->QueryUserList()->GetPlayer(idUser);
			IF_OK(pPlayer)
				pPlayer->m_nLevel	= LOGIN_FREE_LEVEL;
		}
		break;
	case	WORLD_QUERYFEE:
		{
			OBJID idAccount = *(OBJID*)buf;
			if(IsAccountLoginOK())
				SendQueryFee(idAccount, 0, 0, 0);
		}
		break;
	case	WORLD_SENDTIME:
		{
			SOCKET_ID idSocket = *(SOCKET_ID*)buf;
			IF_OK(idSocket != SOCKET_NONE)
				UserList()->SendTimeToSocket(idSocket);
		}
		break;
	case	KERNEL_SUPERMANLIST:
		{
			VARIANT_SET0* pInfo  = (VARIANT_SET0*)buf;
			OBJID	idUser	= pInfo->IntParam(0);
			int		nNumber	= pInfo->IntParam(1);
			if(nNumber)
				UserList()->QuerySupermanList()->AddMember(idUser, nNumber);
			else
				UserList()->QuerySupermanList()->QueryMember(idUser);
		}
		break;
	case	KERNEL_QUERYSUPERMANLIST:
		{
			VARIANT_SET0* pInfo  = (VARIANT_SET0*)buf;
			OBJID	idUser	= pInfo->IntParam(0);
			int		nIndex	= pInfo->IntParam(1);
			OBJID	idAction	= pInfo->IntParam(2);
			int		nNumber	= pInfo->IntParam(3);
			UserList()->QuerySupermanList()->SendList(idUser, nIndex, nNumber, idAction);
		}
		break;
	default:
		MYASSERT(!"CWorldKernel::ProcessMsg()");
	}

	return true;
}

bool CWorldKernel::OnTimer(time_t tCurr)
{
	g_UserList.OnTimer(tCurr);
	if(!m_sock.IsOpen())
	{
		m_nState = STATE_NONE;
		GameWorld()->PrintText("Connect to account server again...");
		if(m_sock.Open(0))
			m_nState = STATE_CONNECTOK;
	}

	if(m_sock.IsOpen())
	{
		if(m_nState == STATE_CONNECTOK)
		{
			SendAccount(m_szAccount, m_szPassword, m_szServer, m_nServePort);
			m_nState = STATE_ACCOUNTOK;
		}

		ProcessAccountNetMsg();
	}

	if(m_tUserAmount.ToNextTime())
	{
		SendQueryFee(ID_NONE, QueryUserList()->GetUserAmount(), 0, 103);

		extern DWORD g_dwMaxPlayer;
		if(QueryUserList()->GetUserAmount() >= (int)g_dwMaxPlayer)
			GameWorld()->SendFee(ID_NONE, CMsgFee_A::SERVER_FULL);
	}

	return true;
}

bool CWorldKernel::Release()
{
	m_setForbidName.clear();

	if(m_pUserList)
	{
		m_pUserList->LogoutAllUser();
		m_pUserList->Release();
	}
	if(m_pMapList)
		m_pMapList->Release();
	if(m_pSynManager)
		m_pSynManager->Release();

	m_pMsgPort->Close();

	if(GameWorld())
		GameWorld()->SendFee(ID_NONE, CMsgFee_A::SERVER_CLOSE);

	m_sock.Close();

	if(m_setConnectClient)
		m_setConnectClient->Release();
	if(m_setConnectAccount)
		m_setConnectAccount->Release();

	SAFE_RELEASE (m_pDb);
	delete this;
	return true;
}

bool CWorldKernel::ProcessClientMsg(SOCKET_ID idSocket, OBJID idMsg, const char *pbufMsg, int nSize, int nTrans)
{
	if(idMsg == _MSG_NONE || !pbufMsg || nSize <= 0 || nSize >_MAX_MSGSIZE)
        return false;

	world_kernel::CNetMsg* pMsg	=world_kernel::CNetMsg::CreateClientMsg(m_idProcess, idSocket, idMsg, pbufMsg, nSize, nTrans);
	if(pMsg)
	{
		try {
			pMsg->Process(this);
		}
		catch(...)
		{
			MYASSERT(!"pMsg->Process(this);");
			::LogSave("exception catch at CGameSocket::ProcessMsg()! MsgType:%d, SocketID:%u", idMsg, idSocket);
		}
		delete pMsg;
	}

	return true;
}

bool CWorldKernel::SendMsg(world_kernel::CNetMsg* pNetMsg)
{
	MYASSERT(pNetMsg);
	if(!pNetMsg)
		return false;

	if(!pNetMsg->IsNpcMsg())
		return SendMsg(pNetMsg->GetSocketID(), pNetMsg->GetType(), pNetMsg->GetBuf(), pNetMsg->GetSize());
	else
		return SendNpcMsg(pNetMsg->GetNpcID(), pNetMsg->GetType(), pNetMsg->GetBuf(), pNetMsg->GetSize());
}

bool CWorldKernel::SendMsg(SOCKET_ID idSocket, OBJID idMsg, const char *pBuf, int nMsgLen)
{
	if (!pBuf)
	{
		::LogSave("Error: null msg point found in CGameSocket::SendMsg.");
		return false;
	}

	if (_MSG_NONE == idMsg)
	{
		::LogSave("Error: invalid msg type in CGameSocket::SendMsg().");
		return false;
	}

	try {
		MESSAGESTR	bufPacket;
		SENDCLIENTMSG_PACKET0* pPacket = (SENDCLIENTMSG_PACKET0*)bufPacket;
		pPacket->idSocket	= idSocket;
		pPacket->idPacket	= idMsg;
		pPacket->nSize		= nMsgLen;
		memcpy(pPacket->buf, pBuf, nMsgLen);
		m_pMsgPort->Send(MSGPORT_SOCKET, SOCKET_SENDCLIENTMSG, STRUCT_TYPE(bufPacket), &bufPacket);
		return true;
	}
	catch(...)
	{
		MYASSERT(!"catch");
		::LogSave("Error: exception catched in CGameSocket::SendMsg().");
		return false;
	}
	return true;
}

bool CWorldKernel::ProcessNpcMsg(OBJID idNpc, OBJID idMsg, const char *pbufMsg, int nSize, int nTrans)
{
	if(idMsg == _MSG_NONE || !pbufMsg || nSize <= 0 || nSize >_MAX_MSGSIZE)
        return false;

	world_kernel::CNetMsg* pMsg	=world_kernel::CNetMsg::CreateNpcMsg(m_idProcess, idNpc, idMsg, pbufMsg, nSize, nTrans);
	if(pMsg)
	{
		try {
			pMsg->Process(this);
		}
		catch(...)
		{
			MYASSERT(!"pMsg->Process(this);");
			::LogSave("exception catch at CGameSocket::ProcessMsg()! MsgType:%d, NPCID:%u", idMsg, idNpc);
		}
		delete pMsg;
	}

	return true;
}

bool CWorldKernel::SendNpcMsg(world_kernel::CNetMsg* pNetMsg)
{
	MYASSERT(pNetMsg);
	if(!pNetMsg)
		return false;

	return SendNpcMsg(pNetMsg->GetNpcID(), pNetMsg->GetType(), pNetMsg->GetBuf(), pNetMsg->GetSize());
}

bool CWorldKernel::SendNpcMsg(OBJID idNpc, OBJID idMsg, const char *pBuf, int nMsgLen)
{
	if (!pBuf)
	{
		::LogSave("Error: null msg point found in CGameSocket::SendMsg.");
		return false;
	}

	if (_MSG_NONE == idMsg)
	{
		::LogSave("Error: invalid msg type in CGameSocket::SendMsg().");
		return false;
	}

	try {
		MESSAGESTR	bufPacket;
		SENDNPCMSG_PACKET0* pPacket = (SENDNPCMSG_PACKET0*)bufPacket;
		pPacket->idNpc		= idNpc;
		pPacket->idPacket	= idMsg;
		pPacket->nSize		= nMsgLen;
		memcpy(pPacket->buf, pBuf, nMsgLen);
		m_pMsgPort->Send(MSGPORT_SOCKET, SOCKET_SENDNPCMSG, STRUCT_TYPE(bufPacket), &bufPacket);
		return true;
	}
	catch(...)
	{
		MYASSERT(!"catch");
		::LogSave("Error: exception catched in CGameSocket::SendMsg().");
		return false;
	}

	return true;

	return true;
}

bool CWorldKernel::CloseSocket(SOCKET_ID idSocket)
{
	m_pMsgPort->Send(MSGPORT_SOCKET, SOCKET_BREAKCONNECT, VARTYPE_INT, &idSocket);

	if(idSocket == m_idNpcSocket)
	{
		LOGMSG("CWorldKernel::CloseSocket break npc connect!", idSocket);
		m_idNpcSocket	= SOCKET_NONE;
	}

	return true;
}

bool CWorldKernel::SendClientMsg(SOCKET_ID idSocket, world_kernel::CNetMsg* pMsg)
{
#ifdef _MYDEBUG
	::LogMsg("SendMsg:%d, size:%d", m_unMsgType, m_unMsgSize);

#endif

	pMsg->AppendInfo(MSGPORT_WORLD, idSocket, ID_NONE);
    return SendMsg(pMsg);
}

bool CWorldKernel::SendNpcMsg(OBJID idNpc, world_kernel::CNetMsg* pMsg)
{
#ifdef _MYDEBUG
	::LogMsg("SendMsg:%d, size:%d", m_unMsgType, m_unMsgSize);

#endif

	pMsg->AppendInfo(MSGPORT_WORLD, SOCKET_NONE, idNpc);
    return SendNpcMsg(pMsg);
}

bool CWorldKernel::PrintText			(LPCTSTR szText)
{
	return m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(szText), szText);
}

bool CWorldKernel::ChangeCode(SOCKET_ID idSocket, DWORD dwData)
{
	CHANGE_USERDATA	st;
	st.idSocket		= idSocket;
	st.nData		= dwData;
	return m_pMsgPort->Send(MSGPORT_SOCKET, SOCKET_CHANGECODE, STRUCT_TYPE(CHANGE_USERDATA), &st);
}

bool CWorldKernel::SendAccount(LPCTSTR szAccount, LPCTSTR szPassword, LPCTSTR szServer, int nServePort)
{
	CMsgAccount_A	msg;
	msg.Create(szAccount, szPassword, szServer, nServePort);

	return m_sock.SendPacket((LPCTSTR)msg.m_pInfo, msg.m_pInfo->unMsgSize, true);
}

bool CWorldKernel::SendLogin(OBJID idAccount, DWORD dwCheckData, int usVersion, LPCTSTR szUserName)
{
	CMsgLogin_A::MSG_Info	msg;
	msg.unMsgSize	= sizeof(msg);
	msg.unMsgType	= _MSG_LOGIN_A;

	msg.idAccount	= idAccount;
	msg.dwCheckData	= dwCheckData;
	msg.usVersion	= usVersion;
	SafeCopy(msg.szUserName, szUserName);

	return m_sock.SendPacket((LPCTSTR)&msg, msg.unMsgSize, true);
}

bool CWorldKernel::SendFee(OBJID idAccount, int nType)
{
	CMsgFee_A::MSG_Info	msg;
	msg.unMsgSize	= sizeof(msg);
	msg.unMsgType	= _MSG_FEE_A;

	msg.idAccount	= idAccount;
	msg.ucType		= nType;

	return m_sock.SendPacket((LPCTSTR)&msg, msg.unMsgSize, true);
}

bool CWorldKernel::SendQueryFee(OBJID idAccount, DWORD dwData, int nTime, int ucType)
{
	CMsgQueryFee_A::MSG_Info	msg;
	msg.unMsgSize	= sizeof(msg);
	msg.unMsgType	= _MSG_QUERYFEE_A;

	msg.idAccount	= idAccount;
	msg.dwData		= dwData;
	msg.nTime		= nTime;
	msg.ucType		= ucType;

	return m_sock.SendPacket((LPCTSTR)&msg, msg.unMsgSize, true);
}

void CWorldKernel::ProcessAccountNetMsg()
{
	bool	bGetData = true;
	while(true)
	{
		int	nNetSize = 0;
		LPCTSTR pMsg = m_sock.GetPacket(&nNetSize, bGetData);
		if(!pMsg || nNetSize == 0)
			break;
		int nMsgSize = *((unsigned short*)pMsg);
		if(nNetSize < nMsgSize)
			break;

		CMsgLogin_A::MSG_Info*	pMsgLogin = (CMsgLogin_A::MSG_Info*)pMsg;
		if(pMsgLogin->unMsgType == _MSG_LOGIN_A)
		{
			CHECK(pMsgLogin->unMsgSize == sizeof(CMsgLogin_A::MSG_Info));
			ProcessLogin(pMsgLogin->idAccount, pMsgLogin->dwCheckData, pMsgLogin->usVersion, pMsgLogin->szUserName);
			m_sock.ClearPacket(nMsgSize);
			continue;
		}

		CMsgConnect_A::MSG_Info*	pMsgConnect = (CMsgConnect_A::MSG_Info*)pMsg;
		if(pMsgConnect->unMsgType == _MSG_CONNECT)
		{
			ProcessConnectInfo(pMsgConnect->idAccount, pMsgConnect->dwData, pMsgConnect->szInfo);
			CHECK(pMsgConnect->unMsgSize == sizeof(CMsgConnect_A::MSG_Info));
			if(ProcessConnect(pMsgConnect->idAccount, pMsgConnect->dwData, pMsgConnect->szInfo))
			{
				CPlayer* pPlayer	= g_UserList.GetPlayerByAccountID(pMsgConnect->idAccount);
				MYASSERT(pPlayer);
				if(pPlayer)
					UserList()->LoginToMapGroup(pPlayer);
			}
			m_sock.ClearPacket(nMsgSize);
			continue;
		}

		CMsgFee_A::MSG_Info*	pMsgFee = (CMsgFee_A::MSG_Info*)pMsg;
		if(pMsgFee->unMsgType == _MSG_FEE_A)
		{
			CHECK(pMsgFee->unMsgSize == sizeof(CMsgFee_A::MSG_Info));
			ProcessFee(pMsgFee->idAccount, pMsgFee->ucType);
			m_sock.ClearPacket(nMsgSize);
			continue;
		}

		CMsgQueryFee_A::MSG_Info*	pMsgQFee = (CMsgQueryFee_A::MSG_Info*)pMsg;
		if(pMsgQFee->unMsgType == _MSG_QUERYFEE_A)
		{
			CHECK(pMsgQFee->unMsgSize == sizeof(CMsgQueryFee_A::MSG_Info));
			ProcessQueryFee(pMsgQFee->idAccount, pMsgQFee->dwData, pMsgQFee->nTime, pMsgQFee->ucType);
			m_sock.ClearPacket(nMsgSize);
			continue;
		}

		bGetData = false;
	}
}

void CWorldKernel::ProcessLogin(OBJID idAccount, DWORD dwCheckData, int usVersion, LPCTSTR szUserName)
{
	if(idAccount == ID_NONE)
	{
		PrintText("Account server login failed!");
		return;
	}

	m_idServer	= idAccount;
	PrintText("Account server login OK.");
	SendLogin(idAccount, dwCheckData, usVersion, szUserName);
	m_nState = STATE_NORMAL;
}

bool CWorldKernel::ProcessConnect(OBJID idAccount, DWORD dwData, LPCTSTR szInfo)
{
	extern DWORD g_dwMaxPlayer;

	if(GameWorld()->QueryUserList()->GetUserAmount() > (int)g_dwMaxPlayer && (int)idAccount > MAX_GM_ID)
	{
		GameWorld()->SendFee(idAccount, CMsgFee_A::SERVER_FULL);
		return false;
	}

	CConnectUser* pConnect = m_setConnectClient->GetObj(idAccount);
	if(pConnect)
	{
		if(pConnect->IsValid() && pConnect->CheckData(dwData))
		{
			SOCKET_ID	idSocket = pConnect->GetSocketID();

			m_setConnectClient->DelObj(idAccount);

			if(UserList()->GetPlayerByAccountID(idAccount))
			{
				UserList()->KickOutAccount(idAccount, STR_LOGIN_AGAIN);
				return false;
			}

			int ret = UserList()->LoginUser(idAccount, idSocket, szInfo);
			if (ret == LOGIN_OK)
				return true;
			else if(ret == LOGIN_BAN)
			{
				CMsgTalk msg;
				if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, "错误：余额不足，请及时充值!", NULL, _COLOR_WHITE, _TXTATR_ENTRANCE))
					SendClientMsg(idSocket, &msg);
				return false;
			}
			else if(ret == LOGIN_NEW)
			{
				CMsgTalk msg;
				if (msg.Create(SYSTEM_NAME, ALLUSERS_NAME, NEW_ROLE_STR, NULL, _COLOR_WHITE, _TXTATR_ENTRANCE))
				{
					SendClientMsg(idSocket, &msg);
				}
			}
			else
				return false;
		}
		else
		{
			m_setConnectClient->DelObj(idAccount);
		}
	}

	pConnect = CConnectUser::CreateNew();
	CHECKF(pConnect->Create(idAccount, dwData, szInfo));
	m_setConnectAccount->DelObj(idAccount);
	m_setConnectAccount->AddObj(pConnect);

	return false;
}

bool CWorldKernel::ClientConnect(OBJID idAccount, DWORD dwData, LPCTSTR szInfo, SOCKET_ID idSocket)
{
	CHECKF(idSocket != SOCKET_NONE);

	CConnectUser* pConnect = m_setConnectAccount->GetObj(idAccount);
	if(pConnect)
	{
		NAMESTR	info;
		SafeCopy(info, pConnect->GetInfo(), _MAX_NAMESIZE);

		if(pConnect->IsValid() && pConnect->CheckData(dwData))
		{
			m_setConnectAccount->DelObj(idAccount);

			if(UserList()->GetPlayerByAccountID(idAccount))
			{
				UserList()->KickOutAccount(idAccount, STR_LOGIN_AGAIN);
				return false;
			}

			int ret = UserList()->LoginUser(idAccount, idSocket, info);
			if (ret == LOGIN_OK)
				return true;
			else if(ret == LOGIN_BAN)
			{
				CMsgTalk msg;
				if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, "错误：余额不足，请及时充值!", NULL, _COLOR_WHITE, _TXTATR_ENTRANCE))
					SendClientMsg(idSocket, &msg);
				return false;
			}
			else
			{
				CMsgTalk msg;
				if (msg.Create(SYSTEM_NAME, ALLUSERS_NAME, NEW_ROLE_STR, NULL, _COLOR_WHITE, _TXTATR_ENTRANCE))
				{
					SendClientMsg(idSocket, &msg);
				}
			}
		}
		else
		{
			m_setConnectAccount->DelObj(idAccount);
		}
	}

	pConnect = CConnectUser::CreateNew();
	CHECKF(pConnect->Create(idAccount, dwData, szInfo, idSocket));
	m_setConnectClient->DelObj(idAccount);
	m_setConnectClient->AddObj(pConnect);

	return false;
}

void CWorldKernel::ProcessFee(OBJID idAccount, int nType)
{
	switch(nType)
	{
	case	CMsgFee_A::HEARTBEAT:
		SendFee(idAccount, CMsgFee_A::HEARTBEAT);
		break;
	case	CMsgFee_A::FEE_KICKOUT:
		LOGMSG("DEBUG: account server kicked account [%u]", idAccount);
		if(!UserList()->KickOutAccount(idAccount, STR_LOGIN_ANOTHER))
			SendFee(idAccount, CMsgFee_A::FEE_OFFLINE);
		break;
	default:
		break;
	}

}

enum { c_typeNone = 0, c_typePoint, c_typeTime, c_typeNetBarPoint, c_typeNetBarTime, c_typeISP, c_typeFree, c_typeAll };
void CWorldKernel::ProcessQueryFee(OBJID idAccount, DWORD dwData, int nTime, int ucType)
{
	if(ucType >= 101 && ucType <= 103)
		return;

	CPlayer*  pUser	=UserList()->GetPlayerByAccountID(idAccount);
	if(pUser)
	{
		char szMsg[1024];
		strncpy(szMsg, STR_FEETYPE_KNOWN, 1024);
		szMsg[1023]	= 0;
		switch(ucType)
		{
		case	c_typeNone:
			UserList()->KickOutAccount(idAccount, FLAG_ISP_TO_NORMAL);
			return;
		case	c_typePoint:
			sprintf(szMsg, STR_FEETYPE_HOUR, dwData/20, (dwData/2)%10);
			break;
		case	c_typeTime:
			sprintf(szMsg, STR_FEETYPE_MONTH, nTime/10000, (nTime/100) % 100, nTime % 100);
			break;
		case	c_typeNetBarPoint:
			sprintf(szMsg, STR_FEETYPE_BARHOUR, dwData);
			break;
		case	c_typeNetBarTime:
			sprintf(szMsg, STR_FEETYPE_BARMONTH, dwData);
			break;
		case	c_typeISP:
			sprintf(szMsg, STR_FEETYPE_ISP, dwData);
			break;
		case	c_typeFree:
			sprintf(szMsg, STR_FEETYPE_FREE, dwData);
			break;
		case	100:
			UserList()->SetFeeData(pUser->GetID(), dwData);
			return;
		case	104:
		case	105:
		case	106:
		case	107:
			{
				ST_FEEINFO	st;
				st.idSocket	= pUser->idSocket;
				st.idUser	= pUser->GetID();
				st.nType	= ucType;
				st.dwData	= dwData;
				st.nTime	= nTime;
				m_pMsgPort->Send(pUser->idProcess, MAPGROUP_FEEINFO, STRUCT_TYPE(ST_FEEINFO), &st);
			}
			return;
		case	108:
			{
				char szText[1024] = "";
				switch(nTime)
				{
				case	0:
					_snprintf(szText, 1024, STR_KICKOUT_ELSE);
					break;
				case	1:
					_snprintf(szText, 1024, STR_KICKOUT_SAMEACCOUNT, inet_ntoa(*(in_addr*)&dwData));
					break;
				case	2:
					_snprintf(szText, 1024, STR_KICKOUT_SAMEIDCARD);
					break;
				case	3:
					_snprintf(szText, 1024, STR_KICKOUT_WALLOWSYSTEM);
					break;
				default:
					return;
				}
				CMsgTalk	msg;
				if(msg.Create(SYSTEM_NAME, pUser->szName, szText))
					pUser->SendMsg(&msg);
				UserList()->KickOutAccount(idAccount, szText);
			}
			return;
		default:
			break;
		}

		CMsgTalk	msg;
		IF_OK(msg.Create(SYSTEM_NAME, pUser->szName, szMsg))
			pUser->SendMsg(&msg);

		char szWord1[256] = "";
		char szWord2[256] = "";
		char szWord3[256] = "";
		szMsg[0]	= 0;
		if(sscanf(pUser->szNotify, "%s %s %s", szWord1, szWord2, szWord3) < 1)
		{
			LOGERROR("Account server returned an error! (idAccount=%u, name=%s, data=%u, time=%d, type=%u)", idAccount, pUser->szName, dwData, nTime, ucType);
			return;
		}
		if(stricmp(szWord3, FLAG_ISP_TO_NORMAL) == 0)
			SafeCopy(szMsg, STR_LOGIN_ISP_FAIL);
		else if(stricmp(szWord3, "999999") == 0)
			SafeCopy(szMsg, STR_ACCOUNT_NOFEE);
		else if(szWord3[0] >= '0' && szWord3[0] <= '9')
			sprintf(szMsg, STR_FEW_FEE_NOTIFY, szWord3);
		else
			SafeCopy(szMsg, szWord3, _MAX_NAMESIZE);
		if(strlen(szMsg) > 0)
		{
			CMsgTalk	msg;
			IF_OK(msg.Create(SYSTEM_NAME, pUser->szName, szMsg))
				pUser->SendMsg(&msg);
		}
	}
	else
		LOGWARNING("Account server sent QueryFee for a player not logged in (idAccount=%u, data=%u, time=%d, type=%u)", idAccount, dwData, nTime, ucType);
}

IWorld* IWorld::CreateNew()
{
	return (IWorld*)new CWorldKernel;
}

bool CWorldKernel::CreateSyndicate		(const CreateSyndicateInfo* pInfo)
{
	for(int i = MSGPORT_MAPGROUP_FIRST; i < GameWorld()->GetMsgPort()->GetSize(); i++)
	{
		m_pMsgPort->Send(i, KERNEL_CREATESYNDICATE, STRUCT_TYPE(CreateSyndicateInfo), pInfo);
	}
	return true;
}

bool CWorldKernel::DestroySyndicate	(OBJID idSyn)
{
	for(int i = MSGPORT_MAPGROUP_FIRST; i < GameWorld()->GetMsgPort()->GetSize(); i++)
	{
		m_pMsgPort->Send(i, KERNEL_DESTROYSYNDICATE, VARTYPE_ULONG, &idSyn);
	}
	return true;
}

bool CWorldKernel::CombineSyndicate(OBJID idSyn, OBJID idTarget)
{
	OBJID	setID[2];
	setID[0]	= idSyn;
	setID[1]	= idTarget;
	for(int i = MSGPORT_MAPGROUP_FIRST; i < GameWorld()->GetMsgPort()->GetSize(); i++)
	{
		m_pMsgPort->Send(i, KERNEL_COMBINESYNDICATE, STRUCT_TYPE(setID), &setID);
	}
	return true;
}

bool CWorldKernel::ChangeSyndicate		(const SynFuncInfo0* pFuncInfo)
{
	for(int i = MSGPORT_MAPGROUP_FIRST; i < GameWorld()->GetMsgPort()->GetSize(); i++)
	{
		m_pMsgPort->Send(i, KERNEL_CHANGESYNDICATE, BUFFER_TYPE(pFuncInfo->nSize), pFuncInfo);
	}
	return true;
}

enum { CONFIGDATA_NAME=1, CONFIGDATA_DATA, CONFIGDATA_STRING };
bool CWorldKernel::LoadConfig(IDatabase *pDb)
{
	extern long	g_sKickoutCheat;

	CAutoPtr<IRecordset> pRes = pDb->CreateNewRecordset("SELECT * FROM cq_config", false);
	CHECKF(pRes);
	for(int i = 0; i < (int)pRes->RecordCount(); i++, pRes->MoveNext())
	{
		if(stricmp(pRes->GetStr(CONFIGDATA_NAME), "kickoutcheat") == 0)
			InterlockedExchange(&g_sKickoutCheat, pRes->GetInt(CONFIGDATA_DATA));
	}

	return true;
}

bool CWorldKernel::LoadForbidName(IDatabase *pDb)
{
	IRecordset* pRes = pDb->CreateNewRecordset("select check from cq_forbidname", false);
	CHECKF(pRes);
	for(UINT i = 0; i < pRes->RecordCount(); i++, pRes->MoveNext())
	{
		m_setForbidName.push_back(String(pRes->GetStr(0)));
	}
	pRes->Release();

	return true;
}

bool CWorldKernel::IsNameForbidden(LPCTSTR szName)
{
	for(vector<String>::iterator iter = m_setForbidName.begin(); iter != m_setForbidName.end(); iter++)
	{
		if(!iter->QueryString().empty() && strstr(szName, *iter))
			return true;
	}
	return false;
}

DWORD CWorldKernel::GetSocketIP(SOCKET_ID idSocket)
{
	SOCKETIP_SET::iterator iter = m_setSocketIP.find(idSocket);
	if(iter != m_setSocketIP.end())
		return iter->second;
	return 0;
}

void CWorldKernel::ProcessConnectInfo(OBJID idAccount, DWORD dwData, LPCTSTR szInfo)
{
	int	nMinor	= 0;
	if(stricmp("", szInfo) != 0)
	{
		char*	pInfo	= (char*)szInfo;
		char*	pToken	= GetToken(&pInfo);
		if(*pToken != 0)
		{
			if(stricmp("vip", pToken) == 0)
			{
				char* pData = GetToken(&pInfo);
				if(pData && *pData != 0)
					atoi(pData);
			}
			else
			{
				atoi(pToken);
				char* pBirth = GetToken(&pInfo);
				if(pBirth && *pBirth != 0)
				{
					int	nBirth	= atoi(pBirth) / 100;
					nMinor	= (DWORD)(TimeGet(TIME_DAY) / 10000 - nBirth + 1900) < 18;
				}
			}
		}
	}

	bool	bChinese	= (stricmp(CStringManager::Instance()->GetStr("Language"), "中文") == 0);
	if(bChinese)
		UserList()->SetMinorFlag(idAccount, nMinor);
	else
		UserList()->SetMinorFlag(idAccount, 0);
}
