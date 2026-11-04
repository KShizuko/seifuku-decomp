#include "MessagePort.h"
#pragma warning(disable:4786)
#include "AllMsg.h"
#include "MapGroup.h"
#pragma warning(disable:4786)
#include <algorithm>
#include "netmsg.h"
#include "protocol.h"
#include "PoliceWanted.h"
#include "Agent.h"

MYHEAP_IMPLEMENTATION(CUserManager,s_heap)

bool CUserManager::Create()
{
	m_pTeamSet		= CTeamSet::CreateNew(true);
	m_idLastTeam	= m_idProcess;
	return true;
}

void CUserManager::Release()
{
	LogoutAllUser();
	if(m_pTeamSet)
		m_pTeamSet->Release();
	delete this;
}

void CUserManager::OnTimer(time_t tCurr)
{
}

CUser* CUserManager::CreateUser(SOCKET_ID idSocket, OBJID idUser)
{
	if(idSocket == SOCKET_NONE || idUser == ID_NONE)
	{
		MYASSERT(!"CUserManager::LoginUser");
		return NULL;
	}
	if(idSocket < (SOCKET_ID)m_setUser.size() && m_setUser[idSocket])
	{
		MYASSERT(!"CUserManager::LoginUser 重复SOCKET_ID");
		DEBUG_TRY
		LogoutUser(idSocket);
		DEBUG_CATCH("LogoutUser(idSocket)")
		m_setUser[idSocket]	= NULL;
	}

	CUserPtr pOldUser = GetUser(idUser);
	if(pOldUser)
	{
		SOCKET_ID idOldSocket = pOldUser->GetSocketID();

		DEBUG_TRY
		LOGMSG("DEBUG: kicked duplicate login USER_ID[%u], SOCKET_ID[%d]", idUser, idOldSocket);
		KickOutSocket(idOldSocket, "Duplicate login");
		DEBUG_CATCH("LogoutUser(id)")
		return NULL;
	}

	CUser* pUser = CUser::CreateNew();
	if(!pUser)
		return NULL;

	if(!pUser->Create(m_idProcess, idSocket, idUser))
	{
		pUser->ReleaseByOwner();
		MapGroup(PID)->QueryIntraMsg()->CloseSocket(idSocket);
		return NULL;
	}

	return pUser;
}

CAgent* CUserManager::CreateAgent(OBJID idAgent)
{
	if(idAgent == ID_NONE)
	{
		MYASSERT(!"CUserManager::LoginAgent");
		return NULL;
	}

	CAgent* pAgent = CAgent::CreateNew();
	if(!pAgent)
		return NULL;

	if(!pAgent->Create(m_idProcess, idAgent))
	{
		pAgent->ReleaseByOwner();
		return NULL;
	}

	return pAgent;
}

bool CUserManager::LoginUser(SOCKET_ID idSocket, CUser* pUser)
{
	CHECKF(pUser);

	if(idSocket >= (SOCKET_ID)m_setUser.size())
		m_setUser.resize(idSocket + 1, NULL);

	m_setUser[idSocket]	= pUser;
	RoleManager()->QuerySet()->AddObj(pUser->QueryRole());

	CLevupexpData* pLevupexp	= LevupexpSet()->GetObj(pUser->GetLev());
	if(pLevupexp)
	{
		__int64	i64LevupExp	= pLevupexp->GetInt64(LEVUPEXPDATA_EXP);
		__int64	i64Exp		= pUser->GetExp();
		if(i64Exp > i64LevupExp)
		{
			pUser->SetExp(i64LevupExp - 1, false);
			::MyLogSave("gmlog/expcheater1", "%u\t%u\t%s\t%u\t%I64d\t%I64d\t",
						pUser->GetAccountID(), pUser->GetID(), pUser->GetName(), pUser->GetLev(), i64Exp, i64LevupExp);
		}
	}

	CMsgUserInfo	msg;
	if(msg.Create(pUser->GetID(), pUser->GetInfo()))
		MapGroup(PID)->QueryIntraMsg()->SendClientMsg(idSocket, &msg);

	{
		CMsgUserAttrib	msg;
		if(msg.Create(pUser->GetID(), _USERATTRIB_KEEPEFFECT, pUser->GetEffect(0)))
			MapGroup(PID)->QueryIntraMsg()->SendClientMsg(idSocket, &msg);
	}

	if (pUser->GetPk() >= PKVALUE_BADGUY)
	{
		if (MAP_PRISON != pUser->GetRecordMapID())
			PoliceWanted().AddWanted(pUser);
	}

	if (pUser->IsMarried())
	{
		CUser* pMate = UserManager()->GetUser(pUser->GetMate());
		if (pMate)
		{
			const char* pszMsg = MALE_MATE_LOGIN;
			if (pMate->GetSex() == SEX_MAN)
				pszMsg = FEMALE_MATE_LOGIN;

			pMate->SendSysMsg(_TXTATR_NORMAL, pszMsg);
		}
	}

	DWORD dwUserAmount = this->GetUserAmount();
	if (dwUserAmount >= m_dwMaxUser)
	{
		m_dwMaxUser = dwUserAmount;
	}

	pUser->LoadStatistic();

	const int ACTION_LOGINPROMPT = 1000000;
	GameAction()->ProcessAction(ACTION_LOGINPROMPT, pUser);

	if (pUser->IsGM())
		m_setGm.push_back(pUser->GetID());

	MapGroup(PID)->QueryIntraMsg()->QuerySocketIP(PID, pUser->GetSocketID());
	return true;
}

bool CUserManager::LoginAgent(CAgent* pAgent)
{
	CHECKF(pAgent);

	RoleManager()->QuerySet()->AddObj(pAgent->QueryRole());

	CMsgUserInfo	msg;
	if(msg.Create(pAgent->GetID(), pAgent->GetInfo()))
		pAgent->SendMsg(&msg);

	{
		CMsgUserAttrib	msg;
		if(msg.Create(pAgent->GetID(), _USERATTRIB_KEEPEFFECT, pAgent->GetEffect(0)))
			pAgent->SendMsg(&msg);
	}

	if (pAgent->GetPk() >= PKVALUE_BADGUY)
	{
		if (MAP_PRISON != pAgent->GetRecordMapID())
			PoliceWanted().AddWanted(pAgent);
	}

	if (pAgent->IsMarried())
	{
		CUser* pMate = UserManager()->GetUser(pAgent->GetMate());
		if (pMate)
		{
			const char* pszMsg = MALE_MATE_LOGIN;
			if (pMate->GetSex() == SEX_MAN)
				pszMsg = FEMALE_MATE_LOGIN;

			pMate->SendSysMsg(_TXTATR_NORMAL, pszMsg);
		}
	}

	return true;
}

bool CUserManager::LogoutUser(SOCKET_ID idSocket, BOOL bOffline )
{
	if(idSocket >= (SOCKET_ID)m_setUser.size() || !m_setUser[idSocket])
		return false;

	CUserPtr pUser = GetUserBySocketID(idSocket);
	if(pUser)
	{
		DEBUG_TRY

			pUser->SaveInfo();

		if(bOffline)
		{
			if(pUser->IsAlive())
			{
				if(pUser->GetMap()->IsStandEnable(pUser->GetPosX(), pUser->GetPosY()) && !pUser->GetMap()->IsRecordDisable())
					pUser->SetRecordPos(pUser->GetMapID(), pUser->GetPosX(), pUser->GetPosY());
			}

			for(int i = 0; i < pUser->GetFriendAmount(); i++)
			{
				CFriend* pFriend = pUser->GetFriendByIndex(i);
				if(pFriend)
				{
					OBJID	idFriend	= pFriend->GetFriendID();
					CMsgFriend	msg;
					IF_OK(msg.Create(_FRIEND_OFFLINE, pUser->GetID(), pUser->GetPeerageRank(), pUser->GetSex(), pUser->GetName()))
						MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(&msg, idFriend);
				}
			}
			PoliceWanted().DelWanted(pUser->GetID());

			if (pUser->IsGM())
			{
				vector<OBJID>::iterator iter;
				for (iter = m_setGm.begin(); iter != m_setGm.end(); iter++)
				{
					if (pUser->GetID() == *iter)
					{
						m_setGm.erase(iter);
						break;
					}
				}
			}
		}
		pUser->OnLogout();
		DEBUG_CATCH2("logout user [%d]", idSocket)

		m_setUser[idSocket] = NULL;
		RoleManager()->QuerySet()->DelObj(pUser->GetID());
	}

	return true;
}

bool CUserManager::LogoutAgent(CUser* pUser, BOOL bOffline )
{
	MYASSERT(pUser && pUser->IsAgent());

	{
		DEBUG_TRY

		pUser->SaveInfo();

		if(bOffline)
		{
			for(int i = 0; i < pUser->GetFriendAmount(); i++)
			{
				CFriend* pFriend = pUser->GetFriendByIndex(i);
				if(pFriend)
				{
					OBJID	idFriend	= pFriend->GetFriendID();
					CMsgFriend	msg;
					IF_OK(msg.Create(_FRIEND_OFFLINE, pUser->GetID(), pUser->GetPeerageRank(), pUser->GetSex(), pUser->GetName()))
						MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(&msg, idFriend);
				}
			}
			PoliceWanted().DelWanted(pUser->GetID());
		}
		DEBUG_CATCH("logout agent")

		RoleManager()->QuerySet()->DelObj(pUser->GetID());
	}

	return true;
}

bool CUserManager::KickOutSocket(SOCKET_ID idSocket, LPCTSTR szReason )
{
	CHECKF(idSocket >= 0 && idSocket < (SOCKET_ID)m_setUser.size());

	CUserPtr pUser = GetUserBySocketID(idSocket);
	if (pUser)
	{
		if (pUser->IsKickOut())
			return true;
		pUser->SetKickOut();

		pUser->SaveInfo();
		if(szReason)
			LOGWARNING("KickOutSocket() kicked SOCKET_ID[%d], player [%s]. Reason: %s", idSocket, pUser->GetName(), szReason);
	}
	else
	{
		if(szReason)
			LOGWARNING("KickOutSocket() kicked SOCKET_ID[%d]. Reason: %s", idSocket, szReason);
	}

	MapGroup(PID)->QueryIntraMsg()->CloseSocket(idSocket);
	return true;
}

void CUserManager::LogoutAllUser()
{
	CUserManager::Iterator pUser = UserManager()->NewEnum();
	while(pUser.Next())
	{
		if(!pUser->IsAgent())
			LogoutUser(pUser->GetSocketID());
		else
			LogoutAgent(pUser);
	}
}

CUser* CUserManager::GetUser(OBJID idUser)
{
	if(idUser == ID_NONE)
		return NULL;

	IRole* pRole = RoleManager()->QueryRole(idUser);
	CUser* pUser;
	if(pRole && pRole->QueryObj(OBJ_USER, IPP_OF(pUser)))
		return pUser;

	return NULL;
}

CUser* CUserManager::GetUser(LPCTSTR szName)
{
	MYASSERT(szName);

	CUserManager::Iterator pUser = this->NewEnum();
	while(pUser.Next())
	{
		if(strcmp(pUser->GetName(), szName) == 0)
			return pUser;
	}

	return NULL;
}

CUser* CUserManager::GetUser(CNetMsg* pMsg)
{
	MYASSERT(pMsg);

	if(pMsg->IsNpcMsg())
	{
		IRole* pRole = RoleManager()->QuerySet()->GetObj(pMsg->GetNpcID());
		if(!pRole)
			return NULL;
		CUser* pUser = NULL;
		pRole->QueryObj(OBJ_USER, IPP_OF(pUser));
		return pUser;
	}
	else
		return GetUserBySocketID(pMsg->GetSocketID());
}

bool CUserManager::FindNextSynMember(OBJID idSyn, int& nIter)
{
	CHECKF(idSyn != ID_NONE);

	OBJID	idLastUser	= nIter;
	CUser*	pTarget		= NULL;
	bool	bFindNext	= true;
	CUserManager::Iterator pUser = UserManager()->NewEnum();
	while(pUser.Next())
	{
		if(pUser && pUser->GetSynID() == idSyn)
		{
			if(bFindNext)
			{
				pTarget		= pUser;
				bFindNext	= false;
			}

			if(pUser->GetID() == idLastUser)
			{
				pTarget		= NULL;
				bFindNext	= true;
				continue;
			}
		}
	}

	if(pTarget)
	{
		nIter = pTarget->GetID();
		return true;
	}
	return false;
}

void CUserManager::BroadcastMsg(CNetMsg* pMsg, CUser* pExcludeSender )
{
	CHECK(pMsg);

	BroadcastMapGroupMsg(pMsg, pExcludeSender);

	MapGroup(PID)->QueryIntraMsg()->SendBroadcastMsg(pMsg);
}

void CUserManager::BroadcastMsg(LPCTSTR szMsg, CUser* pExcludeSender , LPCTSTR pszEmotion, DWORD dwColor, int nAttribute)
{
	CHECK(szMsg);

	CMsgTalk	msg;
	if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szMsg, pszEmotion, dwColor, nAttribute))
		BroadcastMsg(&msg, pExcludeSender);
}

void CUserManager::BroadcastMapMsg(OBJID idMap, CNetMsg* pMsg, CUser* pExcludeSender)
{
	MYASSERT(pMsg);

	bool	bSendAiServer = true;
	CUserManager::Iterator pUser = UserManager()->NewEnum();
	while(pUser.Next())
	{
		if(!pUser->IsAgent())
		{
			if(pUser->GetMap()->GetID() == idMap && (!pExcludeSender || pExcludeSender->GetID() != pUser->GetID()))
			{
				MapGroup(PID)->QueryIntraMsg()->SendClientMsg(pUser->GetSocketID(), pMsg);
			}
		}
		else if(bSendAiServer)
		{
			if(pUser->GetMap()->GetID() == idMap && (!pExcludeSender || pExcludeSender->GetID() != pUser->GetID()))
			{
				MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(pUser->GetID(), pMsg);
				bSendAiServer = false;
			}
		}
	}
}

void CUserManager::BroadcastMapGroupMsg(CNetMsg* pMsg, CUser* pExcludeSender)
{
	MYASSERT(pMsg);

	bool	bSendAiServer = true;
	CUserManager::Iterator pUser = UserManager()->NewEnum();
	while(pUser.Next())
	{
		if(!pUser->IsAgent())
		{
			if(!pExcludeSender || pExcludeSender->GetID() != pUser->GetID())
			{
				MapGroup(PID)->QueryIntraMsg()->SendClientMsg(pUser->GetSocketID(), pMsg);
			}
		}
		else if(bSendAiServer)
		{
			if(!pExcludeSender || pExcludeSender->GetSocketID() != pUser->GetID())
			{
				MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(pUser->GetID(), pMsg);
				bSendAiServer = false;
			}
		}
	}
}

void CUserManager::CheckAllClient(int nSecs)
{
	CUserManager::Iterator pUser = UserManager()->NewEnum();
	while(pUser.Next())
	{
		if(!pUser->IsAgent() && !pUser->IsGM())
			pUser->SendVerifyCode(nSecs);
	}
}

void CUserManager::JoinMapGroup(SOCKET_ID idSocket, UserInfoStruct* pInfo)
{
	CHECK(idSocket != SOCKET_NONE && pInfo);

	if(idSocket >= (SOCKET_ID)m_setUser.size())
		m_setUser.resize(idSocket + 1, NULL);
	CHECK(m_setUser[idSocket] == NULL);

	CUser* pUser = CUser::CreateNewUser(m_idProcess, idSocket, pInfo);
	if(!pUser)
		return;

	MYASSERT(!m_setUser[idSocket]);
	m_setUser[idSocket]	= pUser;
	RoleManager()->QuerySet()->AddObj(pUser->QueryRole());
}

DWORD CUserManager::GetUserAmount()
{
	DWORD dwUsers = 0;
	for(int i = 0; i < (int)m_setUser.size(); i++)
	{
		if(m_setUser[i])
			dwUsers++;
	}
	return dwUsers;
}

DWORD CUserManager::GetMapUserAmount(OBJID idMap)
{
	DWORD dwUsers = 0;
	for(int i = 0; i < (int)m_setUser.size(); i++)
	{
		CUser* pUser = m_setUser[i];
		if (pUser && pUser->GetMapID() == idMap)
			dwUsers++;
	}
	return dwUsers;
}

int CUserManager::GetTeamAmount()
{
	CHECKF(m_pTeamSet);

	return m_pTeamSet->GetAmount();
}

CTeam* CUserManager::GetTeamByIndex(int nIndex)
{
	CHECKF(m_pTeamSet);

	CTeam* pTeam	= m_pTeamSet->GetObjByIndex(nIndex);
	return pTeam;
}

CTeam* CUserManager::GetTeamByLeaderId(OBJID idLeader)
{
	for (int i=0; i<this->GetTeamAmount(); i++)
	{
		CTeam* pTeam	= this->GetTeamByIndex(i);
		if (pTeam && pTeam->GetLeader() == idLeader)
			return pTeam;
	}

	return NULL;
}

BOOL CUserManager::IsValidTeam(CTeam* pTeam)
{
	if (!pTeam)
		return false;

	for (int i=0; i<this->GetTeamAmount(); i++)
	{
		CTeam* pNowTeam	= this->GetTeamByIndex(i);
		if (pNowTeam && pNowTeam->GetLeader() == pTeam->GetLeader() &&
			pNowTeam->GetMemberAmount() == pTeam->GetMemberAmount())
			return true;
	}

	return false;
}

BOOL CUserManager::AddTeam(CTeam* pTeam)
{
	if (!pTeam)
		return false;

	m_pTeamSet->AddObj(pTeam);
	return true;
}

void CUserManager::DelTeam(CTeam* pTeam)
{
	if (!pTeam)
		return;

	for (int i=0; i<this->GetTeamAmount(); i++)
	{
		CTeam* pNowTeam	= this->GetTeamByIndex(i);
		if (pNowTeam && pNowTeam->GetID() == pTeam->GetID())
		{
			m_pTeamSet->DelObj(pTeam->GetID());
			return;
		}
	}
}

OBJID CUserManager::SpawnNewTeamID()
{
	m_idLastTeam += MapGroup(PID)->GetMapGroupAmount();
	MYASSERT(m_idLastTeam < 4012345678);
	return m_idLastTeam;
}

CUserManager::Iterator CUserManager::NewEnum()						{ return Iterator(RoleManager()->QuerySet()); }

bool CUserManager::AddCheat(OBJID idUser, int *pnAmount, int *pnTimes)
{
	CHECKF(pnAmount && pnTimes);

	int	nAmount = m_setCheatIDTimes.Size();
	int	nTimes = 1;
	if(m_setCheatIDTimes.IsIn(idUser))
		nTimes = m_setCheatIDTimes.At(idUser) + 1;
	else
		nAmount++;

	m_setCheatIDTimes.Add(idUser, nTimes);

	*pnAmount	= nAmount;
	*pnTimes	= nTimes;
	return true;
}
