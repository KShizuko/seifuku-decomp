#if !defined(AFX_WORLDKERNEL_H__9A393F65_C167_402E_8A17_7EA70AC5879E__INCLUDED_)
#define AFX_WORLDKERNEL_H__9A393F65_C167_402E_8A17_7EA70AC5879E__INCLUDED_

#include <time.h>
#include <winsock2.h>
#include "I_mydb.h"
#include "NetMsg.h"
#include "I_Shell.h"
#include "T_Singlemap.h"
#include "ClientSocket.h"
#include "SynWorldManager.h"
#include "AutoPtr.h"

#if _MSC_VER > 1000
#pragma once
#endif

#include "protocol.h"
using namespace world_kernel;
#include "UserList.h"
#include "MapList.h"
#include "ConnectUser.h"

#define NEW_ROLE_STR		"NEW_ROLE"
const int	MAX_GM_ID			= 1000;

typedef	ISingleMap<CConnectUser>	IConnectSet;
typedef	CSingleMap<CConnectUser>	CConnectSet;

class ISynMessage
{
public:
	virtual bool	CreateSyndicate		(const CreateSyndicateInfo* pInfo)				PURE_VIRTUAL_FUNCTION_0
	virtual bool	DestroySyndicate	(OBJID idSyn)									PURE_VIRTUAL_FUNCTION_0
	virtual bool	CombineSyndicate	(OBJID idSyn, OBJID idTarget)					PURE_VIRTUAL_FUNCTION_0
	virtual bool	ChangeSyndicate		(const SynFuncInfo0* pFuncInfo)					PURE_VIRTUAL_FUNCTION_0
};

class CWorldKernel : IWorld, world_kernel::ISocket, ISynMessage
{
public:
	CWorldKernel();
	virtual ~CWorldKernel() {}

public:
	IWorld*	GetInterface()				{ return (IWorld*)this; }
	world_kernel::ISocket*	GetSocketInterface()	{ return (world_kernel::ISocket*)this; }
	CUserList*	QueryUserList()			{ ASSERT(m_pUserList); return m_pUserList; }
	CMapList*	QueryMapList()			{ ASSERT(m_pMapList); return m_pMapList; }
	ISynMessage*	QuerySynMessage()	{ return (ISynMessage*)this; }
	CSynWorldManager*	QuerySynManager()	{ ASSERT(m_pSynManager); return m_pSynManager; }

protected:
	virtual bool	Create(IMessagePort* pPort);
	virtual bool	ProcessMsg(OBJID idPacket, void* buf, int nType, int nFrom);
	virtual bool	OnTimer(time_t tCurr);
	virtual bool	Release();

protected:
	virtual bool SendMsg			(world_kernel::CNetMsg* pNetMsg);
	virtual bool SendMsg			(SOCKET_ID idSocket, OBJID idMsg, const char* pBuf, int nMsgLen);
	virtual bool SendNpcMsg			(world_kernel::CNetMsg* pNetMsg);
	virtual bool SendNpcMsg			(OBJID idNpc, OBJID idMsg, const char* pBuf, int nMsgLen);
	virtual bool CloseSocket		(SOCKET_ID idSocket);

public:
	virtual bool SendClientMsg		(SOCKET_ID idSocket, world_kernel::CNetMsg* pNetMsg);
	virtual bool SendNpcMsg			(OBJID idNpc, world_kernel::CNetMsg* pNetMsg);
	virtual bool PrintText			(LPCTSTR szText);
	virtual bool ChangeCode			(SOCKET_ID idSocket, DWORD dwData);

public:
	bool IsAccountLoginOK()						{ return m_nState == STATE_NORMAL; }
	bool SendAccount(LPCTSTR szAccount, LPCTSTR szPassword, LPCTSTR szServer, int nServePort);
	bool SendLogin(OBJID idAccount, DWORD dwCheckData, int usVersion, LPCTSTR szUserName);
	bool SendFee(OBJID idAccount, int nType);
	bool SendQueryFee(OBJID idAccount, DWORD dwData, int nTime, int ucType);
	void ProcessAccountNetMsg();
	void ProcessLogin(OBJID idAccount, DWORD dwCheckData, int usVersion, LPCTSTR szUserName);
	void ProcessFee(OBJID idAccount, int ucType);
	void ProcessQueryFee(OBJID idAccount, DWORD dwData, int nTime, int ucType);
	bool ProcessConnect(OBJID idAccount, DWORD dwData, LPCTSTR szInfo);
	bool ClientConnect(OBJID idAccount, DWORD dwData, LPCTSTR szInfo, SOCKET_ID idSocket);
	void ProcessConnectInfo(OBJID idAccount, DWORD dwData, LPCTSTR szInfo);

protected:
	bool ProcessNpcMsg		(OBJID idNpc, OBJID idMsg, const char* pBuf, int nMsgLen, int nTrans=0);
	bool ProcessClientMsg	(SOCKET_ID idSocket, OBJID idMsg, const char* pBuf, int nMsgLen, int nTrans=0);
	PROCESS_ID	GetProcessID() { return m_idProcess; }

public:
	IMessagePort*	GetMsgPort()							{ return m_pMsgPort; }
	IDatabase*		GetDatabase()							{ return m_pDb; }
	SOCKET_ID		GetNpcSocketID()						{ return m_idNpcSocket; }
	void			SetNpcSocketID(SOCKET_ID idNpcSocket)	{ m_idNpcSocket = idNpcSocket; }
	bool			LoadConfig(IDatabase* pDb);
	bool			LoadForbidName(IDatabase* pDb);
	bool			IsNameForbidden(LPCTSTR szName);
	DWORD			GetSocketIP(SOCKET_ID idSocket);
public:
	CMessageBoard*	QueryTradeMsgBd()						{ CHECKF(m_ptrTradeMsgBd); return m_ptrTradeMsgBd; }
	CMessageBoard*	QueryFriendMsgBd()						{ CHECKF(m_ptrFriendMsgBd); return m_ptrFriendMsgBd; }
	CMessageBoard*	QueryTeamMsgBd()						{ CHECKF(m_ptrTeamMsgBd); return m_ptrTeamMsgBd; }
	CMessageBoard*	QueryOtherMsgBd()						{ CHECKF(m_ptrOtherMsgBd); return m_ptrOtherMsgBd; }
	CMessageBoard*	QuerySystemMsgBd()						{ CHECKF(m_ptrSystemMsgBd); return m_ptrSystemMsgBd; }
protected:
	typedef	CAutoPtr<CMessageBoard>	MSGBD_PTR;
	MSGBD_PTR		m_ptrTradeMsgBd;
	MSGBD_PTR		m_ptrFriendMsgBd;
	MSGBD_PTR		m_ptrTeamMsgBd;
	MSGBD_PTR		m_ptrOtherMsgBd;
	MSGBD_PTR		m_ptrSystemMsgBd;
	typedef	map<SOCKET_ID, DWORD>	SOCKETIP_SET;
	SOCKETIP_SET	m_setSocketIP;

protected:
	virtual bool	CreateSyndicate		(const CreateSyndicateInfo* pInfo);
	virtual bool	DestroySyndicate	(OBJID idSyn);
	virtual bool	CombineSyndicate	(OBJID idSyn, OBJID idTarget);
	virtual bool	ChangeSyndicate		(const SynFuncInfo0* pFuncInfo);

protected:
	PROCESS_ID		m_idProcess;
	IDatabase*		m_pDb;
	IMessagePort*	m_pMsgPort;
	CUserList*		m_pUserList;
	CMapList*		m_pMapList;
	CSynWorldManager*	m_pSynManager;

	SOCKET_ID		m_idNpcSocket;
	CTimeOut		m_tUserAmount;

protected:
	CClientSocket	m_sock;
	IConnectSet*		m_setConnectClient;
	IConnectSet*		m_setConnectAccount;
	OBJID		m_idServer;
	enum { STATE_NONE, STATE_CONNECTOK, STATE_ACCOUNTOK, STATE_NORMAL };
	int			m_nState;
	NAMESTR		m_szServer;
	NAMESTR		m_szAccount;
	NAMESTR		m_szPassword;
	int			m_nServePort;
	vector<String>	m_setForbidName;

protected:
	static CWorldKernel*	m_pWorld;
	friend CWorldKernel* GameWorld();
};

inline CWorldKernel* GameWorld()		{ ASSERT(CWorldKernel::m_pWorld); return CWorldKernel::m_pWorld; }
inline CUserList*	UserList()			{ return GameWorld()->QueryUserList(); }
inline CMapList*	MapList()			{ return GameWorld()->QueryMapList(); }
inline CSynWorldManager*	SynWorldManager()	{ return GameWorld()->QuerySynManager(); }

#define	g_UserList	(*UserList())

#endif
