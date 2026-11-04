#include "MessagePort.h"
#include "common.h"
#include "NetMsg.h"
#include "I_mydb.h"
#include "protocol.h"
#include "inifile.h"
#include "MsgFee_A.h"
using namespace world_kernel;
#include "AllMsg.h"
#include "supermanlist.h"
#include "UserList.h"
#include "MessageBoard.h"
#include "WorldKernel.h"
#include "BaseFunc.h"
#include <mmsystem.h>

const int	SECS_PER_GAMEHOUR		= 300;
const int	NEWBIE_ATTR_POINT		= 10;
void CPlayer::OnTimer(time_t tCurr)
{
	if(IsAgent())
		return;

	if(m_nLevel < LOGIN_FREE_LEVEL)
	{
		m_tPoint	= time(NULL);
		if (m_tTick.ToNextTime(POINTFEE_SECS))
			GameWorld()->SendFee(m_idAccount, CMsgFee_A::FEE_TICK);
		return;
	}

	if(GameWorld()->IsAccountLoginOK() && time(NULL) > m_tPoint + POINTFEE_DELAY_SECS)
	{
		m_tPoint += POINTFEE_SECS;

		GameWorld()->SendFee(m_idAccount, CMsgFee_A::FEE_POINT);
	}
}

bool CPlayer::SendMsg(world_kernel::CNetMsg* pMsg)
{
	if(IsAgent())
		return GameWorld()->SendNpcMsg(GetID(), pMsg);
	else
		return GameWorld()->SendClientMsg(idSocket, pMsg);
}

MYHEAP_IMPLEMENTATION(CUserList,s_heap)
CUserList::CUserList()
{
	m_nPlayerAmount		= 0;
	m_nMaxPlayerAmount	= 0;
	m_nHour				= 6;
	m_setAgent			= AGENT_SET::CreateNew(true);
}

CUserList::~CUserList()
{

}

bool CUserList::Create(world_kernel::ISocket* pSocket, IDatabase* pDb)
{
	CHECKF(pSocket);
	m_pSocket	= pSocket;

	m_pSupermanSet = CSupermanList::CreateNew();
	CHECKF(m_pSupermanSet && m_pSupermanSet->Create(pDb));

	return true;
}

bool CUserList::CheckAccountLock(OBJID idAccount, const char* pMac, DWORD dwIP, DWORD dwResData)
{
	DWORD	dwNow	= timeGetTime();
	ACCOUNTLOCK_SET::iterator iter = m_setAccountLock.find(idAccount);
	if(iter == m_setAccountLock.end())
	{
		ACCOUNT_LOCK	info;
		info.nCount			= 1;
		info.tLast			= dwNow;
		info.tFirst			= dwNow;
		info.nLoginMinute	= ::TimeGetMinute();
		memcpy(info.bufMac, pMac, 4);
		info.dwIP			= dwIP;
		info.dwResData		= dwResData;
		m_setAccountLock[idAccount]	= info;
		return true;
	}

	ACCOUNT_LOCK&	info = iter->second;
	info.tLast			= dwNow;
	info.nLoginMinute	= ::TimeGetMinute();
	memcpy(info.bufMac, pMac, 4);
	info.nCount++;
	info.dwIP			= dwIP;
	info.dwResData		= dwResData;

	DWORD	dwAverage	= 0;
	if(info.nCount > 20)
	{
		dwAverage	= (info.tLast - info.tFirst) / (info.nCount - 1);
		if(dwAverage < 30*1000)
			return false;
		if(info.nCount >= 200)
			return false;
	}

	if(info.nCount % 10 == 0 && dwAverage >= 30*60*1000)
		m_setAccountLock.erase(idAccount);
	return true;
}

void CUserList::LockAccount(OBJID idAccount)
{
	ACCOUNTLOCK_SET::iterator iter = m_setAccountLock.find(idAccount);
	if(iter != m_setAccountLock.end())
		iter->second.nCount	= 200;
}

void CUserList::UnlockAccount(OBJID idAccount)
{
	m_setAccountLock.erase(idAccount);
}

int CUserList::LoginUser(OBJID idAccount, SOCKET_ID idSocket, LPCTSTR szInfo)
{
	int	nRet = LOGIN_NEW;
	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id, name, account_id, recordmap_id, level FROM %s WHERE account_id='%u'", _TBL_USER, idAccount);
	IRecordset*	pRes = GameWorld()->GetDatabase()->CreateNewRecordset(szSQL);
	if(pRes)
	{
		OBJID	idUser	= pRes->LoadDWord("id");
		NAMESTR	szName;
		strncpy(szName, NOMATE_NAME, _MAX_NAMESIZE-1);
		szName[_MAX_NAMESIZE-1]	= 0;
		pRes->LoadString(szName, "name", _MAX_NAMESIZE);
		OBJID	idAccount	= pRes->LoadDWord("account_id");
		OBJID	idMap	= pRes->LoadDWord("recordmap_id");
		PROCESS_ID	idProcess	= MapList()->GetMapProcessID(idMap);
		int		nLevel	= pRes->LoadInt("level");

		if(m_setGmIP.size() > 0 && (strstr(szName, "PM") || strstr(szName, "GM")))
		{
			ACCOUNTLOCK_SET::iterator iter = m_setAccountLock.find(idAccount);
			if(iter == m_setAccountLock.end())
				return LOGIN_GMIP;
			DWORD	dwIP	= iter->second.dwIP;
			vector<DWORD>::iterator it;
			for(it = m_setGmIP.begin(); it != m_setGmIP.end(); it++)
			{
				if(*it == dwIP)
					break;
			}
			if(it == m_setGmIP.end())
				return LOGIN_GMIP;
		}

		if(!(strcmp(szInfo, FLAG_NO_POINT) == 0 && nLevel >= LOGIN_FREE_LEVEL)
				&& CreateUser(idProcess, idUser, szName, idAccount, idSocket, nLevel, szInfo))
		{
			nRet = LOGIN_OK;
			GameWorld()->SendFee(idAccount, CMsgFee_A::FEE_BEGIN);
			GameWorld()->SendQueryFee(idAccount, 0, 0, 100);
		}
		else
		{
			nRet = LOGIN_BAN;
			GameWorld()->SendFee(idAccount, CMsgFee_A::FEE_OFFLINE);
		}
		pRes->Release();

	}

	return nRet;
}

bool CUserList::LoginAgent(OBJID idAgent)
{
	int	nRet = LOGIN_NEW;
	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id, name, recordmap_id, level FROM %s WHERE id=%u && account_id=0", _TBL_USER, idAgent);
	IRecordset*	pRes = GameWorld()->GetDatabase()->CreateNewRecordset(szSQL);
	if(pRes)
	{
		OBJID	idUser	= pRes->LoadDWord("id");
		NAMESTR	szName;
		strncpy(szName, NOMATE_NAME, _MAX_NAMESIZE-1);
		szName[_MAX_NAMESIZE-1]	= 0;
		pRes->LoadString(szName, "name", _MAX_NAMESIZE);
		OBJID	idAccount	= ID_NONE;
		SOCKET_ID	idSocket	= SOCKET_NONE;
		OBJID	idMap	= pRes->LoadDWord("recordmap_id");
		PROCESS_ID	idProcess	= MapList()->GetMapProcessID(idMap);
		int		nLevel	= pRes->LoadInt("level");

		pRes->Release();

		if(CreateAgent(idProcess, idAgent, szName))
			return true;
	}

	return false;
}

bool CUserList::LogoutUser(SOCKET_ID idSocket)
{
	if(!( idSocket != SOCKET_NONE && idSocket < (int)m_setUser.size() && m_setUser[idSocket] ))
		return false;

	CPlayer* pPlayer = m_setUser[idSocket];
	CHECKF(pPlayer);
	GameWorld()->SendFee(pPlayer->m_idAccount, CMsgFee_A::FEE_END);
	int	tCurr = time(NULL);
	if(tCurr > pPlayer->m_tPoint + POINTFEE_SECS)
	{
		LOGWARNING("Player [%s][%u] logged out with [%d] points not uploaded!",
				pPlayer->szName, pPlayer->GetID(), (tCurr - pPlayer->m_tPoint) / POINTFEE_SECS);
	}

	DWORD	dwNow	= timeGetTime();
	ACCOUNTLOCK_SET::iterator iter = m_setAccountLock.find(pPlayer->m_idAccount);
	if(iter != m_setAccountLock.end())
	{
		ACCOUNT_LOCK&	info = iter->second;
		char	szMac[16]	= "";
		sprintf(szMac, "%x%x%x%x%x%x", info.bufMac[0], info.bufMac[1], info.bufMac[2], info.bufMac[3], info.bufMac[4], info.bufMac[5]);
		SQLBUF	szSQL	= "";
		sprintf(szSQL, "INSERT INTO login_rcd SET account_id=%u,login_time=%u,online_second=%u,mac_adr='%s',ip_adr=%u,res_src=%u",
					pPlayer->m_idAccount, info.nLoginMinute, (dwNow - info.tLast) / 1000, szMac, info.dwIP, info.dwResData);
		GameWorld()->GetDatabase()->ExecuteAsyncSQL(szSQL);
	}
	else
	{
		LOGWARNING("Player [%s][%u] logged out but has no record in the user table!", pPlayer->szName, pPlayer->GetID());
	}

	{
		m_nPlayerAmount--;
		extern struct STAT_STRUCT	g_stat;
		InterlockedExchange(&g_stat.nAllPlayers, m_nPlayerAmount);
		InterlockedIncrement(&g_stat.nLogoutPlayers);
	}
	{
		SAFE_DELETE(m_setUser[idSocket]);
		m_setUser[idSocket] = NULL;
		return true;
	}
}

bool CUserList::KickOutAccount(OBJID idAccount, LPCTSTR szText)
{
	CHECKF(szText);

	CPlayer* pPlayer = GetPlayerByAccountID(idAccount);
	if(!pPlayer)
		return false;
	LOGDEBUG("DEBUG: world kernel kicked account [%u], SOCKET_ID[%d]", idAccount, pPlayer->idSocket);

	{
		CMsgTalk	msg;
		if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, (char*)szText, NULL, _COLOR_WHITE, _TXTATR_REGISTER))
			pPlayer->SendMsg(&msg);
	}
	return GameWorld()->GetSocketInterface()->CloseSocket(pPlayer->idSocket);
}

bool CUserList::KickOutSocket(SOCKET_ID idSocket)
{
	LOGDEBUG("DEBUG: world kernel kicked SOCKET_ID[%d]", idSocket);
	return GameWorld()->GetSocketInterface()->CloseSocket(idSocket);
}

void CUserList::OnTimer(time_t tCurr)
{
	for(int i = 0; i < (int)m_setUser.size(); i++)
	{
		if(m_setUser[i])
			m_setUser[i]->OnTimer(tCurr);
	}
	if(m_tHour.ToNextTick(SECS_PER_GAMEHOUR))
	{
		m_nHour = (m_nHour + 1) % 24;
	}
}

void CUserList::SendTimeToSocket(SOCKET_ID idSocket) {}	void CUserList::LogoutAllUser() {}	void CUserList::SetFeeData(OBJID idUser, DWORD dwData) {}
bool CUserList::CreateUser(PROCESS_ID idProcess, OBJID idUser, LPCTSTR szName, OBJID idAccount, SOCKET_ID idSocket, int nLevel, LPCTSTR szNotify)
{
	if(idSocket >= (int)m_setUser.size())
		m_setUser.resize(idSocket + 1, NULL);

	IF_NOT(!m_setUser[idSocket])
	{
		LOGERROR("Players logged in on the same SOCKET, new player [%s][%u], SOCKET[%d], old player [%s][%d]",
					szName, idUser, idSocket, m_setUser[idSocket]->szName, m_setUser[idSocket]->idUser);
		KickOutSocket(idSocket);
		return false;
	}

	m_setUser[idSocket] = new CPlayer;
	if(!m_setUser[idSocket])
		return false;

	m_setUser[idSocket]->idUser		= idUser;
	SafeCopy(m_setUser[idSocket]->szName, szName, _MAX_NAMESIZE);
	m_setUser[idSocket]->idProcess	= idProcess;
	SafeCopy(m_setUser[idSocket]->szAccount, szNotify, _MAX_NAMESIZE);
	m_setUser[idSocket]->idSocket	= idSocket;
	m_setUser[idSocket]->m_idAccount	= idAccount;
	m_setUser[idSocket]->m_nLevel		= nLevel;
	SafeCopy(m_setUser[idSocket]->szNotify, szNotify, _MAX_NAMESIZE);

	m_setUser[idSocket]->m_tPoint	= time(NULL);

	{
		m_nPlayerAmount++;
		if(m_nMaxPlayerAmount < m_nPlayerAmount)
			m_nMaxPlayerAmount = m_nPlayerAmount;
		extern struct STAT_STRUCT	g_stat;
		InterlockedExchange(&g_stat.nAllPlayers, m_nPlayerAmount);
		InterlockedExchange(&g_stat.nMaxPlayers, m_nMaxPlayerAmount);
		InterlockedIncrement(&g_stat.nLoginPlayers);
	}
	return true;
}

bool CUserList::CreateAgent(PROCESS_ID idProcess, OBJID idAgent, LPCTSTR szName)
{
	CPlayer* pAgent = new CPlayer;
	if(!pAgent)
		return false;

	pAgent->idUser			= idAgent;
	SafeCopy(pAgent->szName, szName, _MAX_NAMESIZE);
	pAgent->idProcess		= idProcess;
	pAgent->szAccount[0]	= 0;
	pAgent->idSocket		= SOCKET_NONE;
	pAgent->m_idAccount		= ID_NONE;
	pAgent->m_nLevel		= 0;
	pAgent->szNotify[0]		= 0;

	m_setAgent->AddObj(pAgent);

	return true;
}

CPlayer* CUserList::GetPlayer(LPCTSTR szName)
{
	for(int i = 0; i < (int)m_setUser.size(); i++)
	{
		if(m_setUser[i])
		{
			if(strcmp(m_setUser[i]->szName, szName) == 0)
				return m_setUser[i];
		}
	}

	AGENT_SET::Iterator pAgent = m_setAgent->NewEnum();
	while(pAgent.Next())
	{
		if(strcmp(pAgent->szName, szName) == 0)
			return pAgent;
	}

	return NULL;
}

CPlayer* CUserList::GetPlayer(OBJID idUser)
{
	for(int i = 0; i < (int)m_setUser.size(); i++)
	{
		if(m_setUser[i])
		{
			if(m_setUser[i]->idUser == idUser)
				return m_setUser[i];
		}
	}

	AGENT_SET::Iterator pAgent = m_setAgent->NewEnum();
	while(pAgent.Next())
	{
		if(pAgent->GetID() == idUser)
			return pAgent;
	}

	return NULL;
}

CPlayer* CUserList::GetPlayerByAccountID(OBJID idAccount)
{
	for(int i = 0; i < (int)m_setUser.size(); i++)
	{
		if(m_setUser[i])
		{
			if(m_setUser[i]->m_idAccount == idAccount)
				return m_setUser[i];
		}
	}

	return NULL;
}

bool CUserList::CreateNewPlayer(LPCTSTR szAccount, LPCTSTR szName, LPCTSTR szPassword,
						int unLook, int nData, OBJID idAccount)
{
	int idMap	= DEFAULT_LOGIN_MAPID;
	CMapSt* pMap = MapList()->GetMap(idMap);
	if(!pMap)
		return false;

	if(!(unLook >= 1003 && unLook <= 1004 || unLook >= 2001 && unLook <= 2002))
		return false;

	int nProfession = nData;
	if(nProfession == 0)
		nProfession = 20;

	int nForce	= 0;
	int nDex	= 0;
	int nHealth	= 0;
	int nSoul	= 0;
	if (nProfession == 100)
	{
		nForce	= 0;
		nDex	= 2;
		nHealth	= 3;
		nSoul	= 5;
	}
	else if (nProfession == 40)
	{
		nForce	= 2;
		nDex	= 7;
		nHealth	= 1;
		nSoul	= 0;
	}
	else
	{
		if (nProfession != 10)
			nProfession = 20;
		nForce	= 5;
		nDex	= 2;
		nHealth	= 3;
		nSoul	= 0;
	}

	int nMaxLife = (nForce + nDex + nSoul + nHealth * 8) * 3;
	int nMaxMana = nSoul * 5;
	int nDefaultMoney = 100;

	int nDefaultHair = 310 + ::RandGet(7) * 100;
	int nSex = (unLook/1000)%10;
	int nLookFace;
	if (nSex == 1)
		nLookFace = unLook % 10000 + 10000;
	else if (nSex == 2)
		nLookFace = unLook % 10000 + 2010000;
	else
		nLookFace = (unLook % 1000 + 1000) % 10000 + 10000;

	SQLBUF	szSQL;
	sprintf(szSQL, "INSERT %s SET account_id=%u,name='%s',lookface=%d,hair=%d,life=%d,mana=%d,money=%u,recordmap_id=%u, recordx=%u, recordy=%u,strength=%d,speed=%d,health=%d,soul=%d,profession=%d",
					_TBL_USER,
					idAccount, szName, nLookFace, nDefaultHair,
					nMaxLife, nMaxMana, nDefaultMoney,
					idMap, pMap->nPortalX, pMap->nPortalY,
					nForce, nDex, nHealth, nSoul, nProfession);
	return GameWorld()->GetDatabase()->ExecuteSQL(szSQL);
}

void CUserList::ChangeProcessID(SOCKET_ID idSocket, PROCESS_ID idProcess)
{
	CHECK(idProcess >= MSGPORT_MAPGROUP_FIRST && (int)idProcess < GameWorld()->GetMsgPort()->GetSize());
	CHECK(0 <= idSocket && idSocket < (int)m_setUser.size());

	m_setUser[idSocket]->idProcess = idProcess;
}

void CUserList::LoadGmIP()
{
	IRecordset* pRes = GameWorld()->GetDatabase()->CreateNewRecordset("SELECT * FROM cq_config WHERE type = 11009");
	if(pRes)
	{
		for(int i = 0; i < (int)pRes->RecordCount(); i++)
		{
			unsigned char	bufIP[4];
			bufIP[0]	= (unsigned char)pRes->LoadInt("data1");
			bufIP[1]	= (unsigned char)pRes->LoadInt("data2");
			bufIP[2]	= (unsigned char)pRes->LoadInt("data3");
			bufIP[3]	= (unsigned char)pRes->LoadInt("data4");
			m_setGmIP.push_back(*(DWORD*)bufIP);
			pRes->MoveNext();
		}
		pRes->Release();
	}
}

void CUserList::LoginToMapGroup(CPlayer* pPlayer)
{
	CHECK(pPlayer);

	IMessagePort* pPort = GameWorld()->GetMsgPort();
	ST_LOGIN	cLogin;
	cLogin.idSocket	= pPlayer->idSocket;
	cLogin.idUser	= pPlayer->GetID();
	cLogin.nMinorFlag = 0; cLogin.nClientVersion = 0;
	cLogin.nMinorFlag		= GameWorld()->QueryUserList()->GetMinorFlag(pPlayer->m_idAccount);
	cLogin.nClientVersion	= GameWorld()->QueryUserList()->GetClientVersion(pPlayer->m_idAccount);
	GameWorld()->QueryUserList()->DelMinorFlag(pPlayer->m_idAccount);
	GameWorld()->QueryUserList()->DelClientVersion(pPlayer->m_idAccount);
	PROCESS_ID	idMapGroupProcess = pPlayer->GetProcessID();
	pPort->Send(idMapGroupProcess, MAPGROUP_LOGIN, STRUCT_TYPE(ST_LOGIN), &cLogin);
	pPlayer->idProcess	= idMapGroupProcess;

	if(!pPlayer->IsAgent())
	{
		CMsgTalk	msg;
		if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, REPLAY_OK_STR, NULL, _COLOR_WHITE, _TXTATR_ENTRANCE))
			pPlayer->SendMsg(&msg);
	}
}

void CUserList::BroadcastMsg(world_kernel::CNetMsg* pMsg, CPlayer* pExcludeSender )
{
	CHECK(pMsg);

	for(int i = 0; i < (int)m_setUser.size(); i++)
	{
		CPlayer* pPlayer = m_setUser[i];
		if(pPlayer && (!pExcludeSender || pExcludeSender->GetID() != pPlayer->GetID()))
			pPlayer->SendMsg(pMsg);
	}
	AGENT_SET::Iterator pAgent = m_setAgent->NewEnum();
	while(pAgent.Next())
	{
		if(pAgent && (!pExcludeSender || pExcludeSender->GetID() != pAgent->GetID()))
		{
			pAgent->SendMsg(pMsg);
			return;
		}
	}
}

void CUserList::BroadcastMsg(LPCTSTR szMsg, CPlayer* pExcludeSender , LPCTSTR pszEmotion, DWORD dwColor, int nAttribute)
{
	CHECK(szMsg);

	CMsgTalk	msg;
	if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, (char*)szMsg, (char*)pszEmotion, dwColor, nAttribute))
		BroadcastMsg(&msg, pExcludeSender);
}

void CUserList::DelAllAgent()
{
	AGENT_SET::Iterator pAgent	= m_setAgent->NewEnum();
	while(pAgent.Next())
		m_setAgent->DelObj(pAgent);
}

void CUserList::SetClientVersion(OBJID idAccount, int nVersion)
{
	m_setClientVersion[idAccount]	= nVersion;
}

int CUserList::GetClientVersion(OBJID idAccount)
{
	map<OBJID, int>::iterator iter = m_setClientVersion.find(idAccount);
	if(iter != m_setClientVersion.end())
		return iter->second;
	return 0;
}

void CUserList::DelClientVersion(OBJID idAccount)
{
	m_setClientVersion.erase(idAccount);
}

void CUserList::SetMinorFlag(OBJID idAccount, int nMinor)
{
	m_setMinorFlag[idAccount]	= nMinor;
}

int CUserList::GetMinorFlag(OBJID idAccount)
{
	map<OBJID, int>::iterator iter = m_setMinorFlag.find(idAccount);
	if(iter != m_setMinorFlag.end())
		return iter->second;
	return 0;
}

void CUserList::DelMinorFlag(OBJID idAccount)
{
	m_setMinorFlag.erase(idAccount);
}
