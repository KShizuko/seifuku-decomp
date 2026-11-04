#include "allmsg.h"
#include "Team.h"
#include "mapgroup.h"

MYHEAP_IMPLEMENTATION(CTeam,s_heap)

CTeam::CTeam(PROCESS_ID idProcess)
{
	m_idProcess	= idProcess;
	m_bUnk28	= false;
}

CTeam::~CTeam()
{

}

CTeam* CTeam::CreateNew(PROCESS_ID idProcess, OBJID m_idTeam, OBJID idLeader)
{
	CTeam* pTeam	= new CTeam(idProcess);
	if (!pTeam)
		return NULL;

	if (!pTeam->Create(idLeader))
	{
		delete pTeam;
		return NULL;
	}

	pTeam->m_id = m_idTeam;
	return pTeam;
}

CTeam* CTeam::CreateNew(PROCESS_ID idProcess, TeamInfoStruct* pInfo)
{
	CTeam* pTeam	= new CTeam(idProcess);
	IF_NOT(pTeam)
		return NULL;

	IF_NOT(pTeam->Create(pInfo->idLeader))
	{
		delete pTeam;
		return NULL;
	}

	pTeam->m_id = pInfo->id;

	for(int i = 0; i < _MAX_TEAMAMOUNT; i++)
	{
		if(pInfo->setMember[i] != ID_NONE && pInfo->setMember[i] != pInfo->idLeader)
			pTeam->AddMemberOnly(pInfo->setMember[i]);
	}

	pTeam->m_bCloseMoney	= pInfo->bCloseMoney;
	pTeam->m_bCloseItem		= pInfo->bCloseItem;	pTeam->m_bUnk28 = pInfo->bUnk24;

	return pTeam;
}

BOOL CTeam::Create(OBJID idLeader)
{
	this->Init();
	m_idLeader	= idLeader;
	this->AddMemberOnly(idLeader);

	CUser* pPlayer	= UserManager()->GetUser(idLeader);
	if (pPlayer)
		pPlayer->SetStatus(STATUS_TEAMLEADER);

	return true;
}

BOOL CTeam::AddMember(OBJID idMember)
{
	if (m_fClosed)
		return false;

	int nOldAmount = this->GetMemberAmount();
	if (nOldAmount >= _MAX_TEAMAMOUNT)
		return false;

	if (this->IsTeamMember(idMember))
		return false;

	CUser* pMember = UserManager()->GetUser(idMember);
	if (!pMember)
		return false;

	int nSize = m_setIdUser.size();
	for(int i = 0; i < nSize; i++)
	{
		CUser* pPlayer = UserManager()->GetUser(m_setIdUser[i]);
		if(pPlayer)
		{
			IF_OK(pPlayer->GetTeam())
				pPlayer->GetTeam()->AddMemberOnly(idMember);
		}
		else
		{
			MapGroup(PID)->QueryIntraMsg()->ChangeTeam(CHANGETEAM_ADD, GetID(), idMember, 0);
		}
	}
	pMember->SetTeam(this->CloneNew());
	return true;
}

BOOL CTeam::DelMember(OBJID idMember)
{
	int nOldAmount = this->GetMemberAmount();
	int nSize = nOldAmount;
	for (int i=nSize-1; i>=0; i--)
	{
		CUser* pPlayer	= UserManager()->GetUser(m_setIdUser[i]);
		if (pPlayer)
		{
			IF_OK(pPlayer->GetTeam())
			{
				pPlayer->GetTeam()->DelMemberOnly(idMember);
			}
		}
		else
		{
			MapGroup(PID)->QueryIntraMsg()->ChangeTeam(CHANGETEAM_DEL, GetID(), idMember, 0);
		}
	}
	return true;
}

void CTeam::Dismiss(OBJID idLeader)
{
	int nSize = this->GetMemberAmount();
	for (int i=nSize-1; i>=0; i--)
	{
		CUser* pPlayer	= UserManager()->GetUser(m_setIdUser[i]);
		if (pPlayer)
		{
			IF_OK(pPlayer->GetTeam())
			{
				if(pPlayer->GetID() != idLeader)
					pPlayer->ClrTeam();
			}
		}
		else
		{
			MapGroup(PID)->QueryIntraMsg()->ChangeTeam(CHANGETEAM_DISMISS, GetID(), idLeader, 0);
		}
	}
	this->SendAllKillStatistic();
}

OBJID CTeam::GetMemberByIndex(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_setIdUser.size())
		return ID_NONE;

	return m_setIdUser[nIndex];
}

void CTeam::Init()
{
	m_setIdUser.clear();
	m_fClosed			= false;
	m_bCloseMoney		= false;
	m_bCloseItem		= true;
	m_idLeader			= ID_NONE;
}

bool CTeam::IsUnk28()
{
	return m_bUnk28;
}

int CTeam::GetMemberAmount()
{
	return __min(_MAX_TEAMAMOUNT, m_setIdUser.size());
}

void CTeam::BroadcastTeamMsg(CNetMsg* pMsg, CUser* pSender )
{
	if (!pMsg)
		return;

	for (int i=0; i<this->GetMemberAmount(); i++)
	{
		CUser* pUser	= NULL;
		OBJID idUser	= this->GetMemberByIndex(i);
		pUser			= UserManager()->GetUser(idUser);
		if (pUser)
		{
			if (pUser != pSender)
				pUser->SendMsg(pMsg);
		}
		else
		{
			MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(pMsg, idUser);
		}
	}
}

void CTeam::Open()
{
	m_fClosed	= false;
}

void CTeam::Close()
{
	m_fClosed	= true;
	if (m_bUnk28)
	{
		m_bUnk28	= false;
		for (USERIDSET::iterator it = m_setIdUser.begin(); it != m_setIdUser.end(); it++)
		{
			CUser* pUser = UserManager()->GetUser(*it);
			if (pUser && pUser->GetTeam())
				pUser->GetTeam()->m_bUnk28	= m_bUnk28;
		}
	}
}

BOOL CTeam::IsClosed()
{
	return m_fClosed;
}

void CTeam::OpenMoney()
{
	m_bCloseMoney	= false;

	for (int i=0; i<this->GetMemberAmount(); i++)
	{
		OBJID idTarget = this->GetMemberByIndex(i);
		CUser* pTarget = UserManager()->GetUser(idTarget);
		if (pTarget)
		{
			CTeam* pTeam = pTarget->GetTeam();
			IF_OK(pTeam)
			{
				pTeam->SetCloseMoney(false);
			}
		}
		else
		{
			char	buf[1024];
			REMOTE_CALL0* pInfo = (REMOTE_CALL0*)&buf;
			pInfo->idUser		= idTarget;
			pInfo->ucFuncType	= REMOTE_CALL_TEAM_CLOSE_MONEY;
			pInfo->IntParam[0]	= false;
			pInfo->nSize		= sizeof(REMOTE_CALL0) + sizeof(int);
			MapGroup(PID)->QueryIntraMsg()->RemoteCall(pInfo);
		}
	}
}

void CTeam::CloseMoney()
{
	m_bCloseMoney	= true;

	for (int i=0; i<this->GetMemberAmount(); i++)
	{
		OBJID idTarget = this->GetMemberByIndex(i);
		CUser* pTarget = UserManager()->GetUser(idTarget);
		if (pTarget)
		{
			CTeam* pTeam = pTarget->GetTeam();
			IF_OK(pTeam)
			{
				pTeam->SetCloseMoney(true);
			}
		}
		else
		{
			char	buf[1024];
			REMOTE_CALL0* pInfo = (REMOTE_CALL0*)&buf;
			pInfo->idUser		= idTarget;
			pInfo->ucFuncType	= REMOTE_CALL_TEAM_CLOSE_MONEY;
			pInfo->IntParam[0]	= true;
			pInfo->nSize		= sizeof(REMOTE_CALL0) + sizeof(int);
			MapGroup(PID)->QueryIntraMsg()->RemoteCall(pInfo);
		}
	}
}

void CTeam::OpenItem()
{
	m_bCloseItem	= false;

	for (int i=0; i<this->GetMemberAmount(); i++)
	{
		OBJID idTarget = this->GetMemberByIndex(i);
		CUser* pTarget = UserManager()->GetUser(idTarget);
		if (pTarget)
		{
			CTeam* pTeam = pTarget->GetTeam();
			IF_OK(pTeam)
			{
				pTeam->SetCloseItem(false);
			}
		}
		else
		{
			char	buf[1024];
			REMOTE_CALL0* pInfo = (REMOTE_CALL0*)&buf;
			pInfo->idUser		= idTarget;
			pInfo->ucFuncType	= REMOTE_CALL_TEAM_CLOSE_ITEM;
			pInfo->IntParam[0]	= false;
			pInfo->nSize		= sizeof(REMOTE_CALL0) + sizeof(int);
			MapGroup(PID)->QueryIntraMsg()->RemoteCall(pInfo);
		}
	}
}

void CTeam::CloseItem()
{
	m_bCloseItem	= true;

	for (int i=0; i<this->GetMemberAmount(); i++)
	{
		OBJID idTarget = this->GetMemberByIndex(i);
		CUser* pTarget = UserManager()->GetUser(idTarget);
		if (pTarget)
		{
			CTeam* pTeam = pTarget->GetTeam();
			IF_OK(pTeam)
			{
				pTeam->SetCloseItem(true);
			}
		}
		else
		{
			char	buf[1024];
			REMOTE_CALL0* pInfo = (REMOTE_CALL0*)&buf;
			pInfo->idUser		= idTarget;
			pInfo->ucFuncType	= REMOTE_CALL_TEAM_CLOSE_ITEM;
			pInfo->IntParam[0]	= true;
			pInfo->nSize		= sizeof(REMOTE_CALL0) + sizeof(int);
			MapGroup(PID)->QueryIntraMsg()->RemoteCall(pInfo);
		}
	}
}

BOOL CTeam::IsTeamWithNewbie(OBJID idKiller, IRole* pTarget)
{
	CHECKF (pTarget);

	int nAmount = this->GetMemberAmount();
	if (nAmount <= 1) return false;

	BOOL bNewbieBonus = false;
	int nMonsterLev = pTarget->GetLev();
	for (int i=0; i<nAmount; i++)
	{
		CUser* pUser = UserManager()->GetUser(this->GetMemberByIndex(i));
		if (pUser)
		{
			if (!pUser->IsAlive())
				continue;

			if (pUser->GetMapID() != pTarget->GetMap()->GetID())
				continue;

			if (abs(pUser->GetPosX()-pTarget->GetPosX()) > _RANGE_EXPSHARE ||
					abs(pUser->GetPosY()-pTarget->GetPosY()) > _RANGE_EXPSHARE)
				continue;

			if ((int)pUser->GetLev()+20 < nMonsterLev)
			{
				bNewbieBonus = true;
				break;
			}
		}
	}

	return bNewbieBonus;
}

bool CTeam::IsAwardVirtueEnable(CUser* pQueryUser)
{
	IF_NOT(pQueryUser)
		return false;
	OBJID idQuerier = pQueryUser->GetID();
	IF_NOT(this->IsTeamMember(idQuerier))
		return false;

	OBJID idLeader = this->GetLeader();
	if (idQuerier == idLeader)
		return false;
	CUser* pLeader = UserManager()->GetUser(idLeader);
	IF_NOT(pLeader)
		return false;

	if ((DWORD)pLeader->GetLev() < 70 || (DWORD)pLeader->GetLev() < (DWORD)pQueryUser->GetLev() + 20)
		return false;
	if (pLeader->GetMapID() != pQueryUser->GetMapID())
		return false;
	return pLeader->GetDistance(pQueryUser->GetPosX(), pQueryUser->GetPosY()) <= _RANGE_EXPSHARE;
}

void CTeam::AwardMemberExp(OBJID idKiller, IRole* pTarget, int nExp)
{
	CHECK (pTarget);

	CUser*	pKiller	= UserManager()->GetUser(idKiller);
	IF_NOT (pKiller)
		return;

	BOOL bNewbie	= this->IsTeamWithNewbie(idKiller, pTarget);
	int nMonsterLev = pTarget->GetLev();
	for (int i=0; i<this->GetMemberAmount(); i++)
	{
		CUser* pUser	= NULL;
		OBJID idUser	= this->GetMemberByIndex(i);
		DEBUG_TRY
		pUser	= UserManager()->GetUser(idUser);
		DEBUG_CATCH("[Error in CTeam::AwardMemberExp Get user pt.]")
		if (!pUser)
			continue;

		if (!pUser->IsAlive() || pUser->GetID() == idKiller)
			continue;

		if (pUser->GetMapID() != pTarget->GetMap()->GetID())
			continue;

		if (abs(pUser->GetPosX()-pTarget->GetPosX()) > _RANGE_EXPSHARE ||
				abs(pUser->GetPosY()-pTarget->GetPosY()) > _RANGE_EXPSHARE)
			continue;

		if (pUser->IsTimeCardStatus())
			continue;

		int nAddExp = 0;
		if ((int)pUser->GetLev()+20 > nMonsterLev)
		{
			nAddExp = nExp;
			int nDiff = (int)pUser->GetLev() - nMonsterLev;
			if (nDiff < -10)
				nAddExp = nExp*130/100;
			else if (nDiff < -5)
				nAddExp = nExp*120/100;
			if (nAddExp < 1)
				nAddExp = 1;

			if (pUser->GetProfession() >= 133 && pUser->GetProfession() < 140)
				nAddExp = nAddExp*250/100;

			if (bNewbie)
				nAddExp *= 2;
			if (pUser->IsMate(pKiller))
				nAddExp *= 2;

			nAddExp += pUser->GetGemExpEffect()*nAddExp/100;
			nAddExp = (int)pUser->AwardBattleExp(nAddExp, true, 0);

			if (bNewbie)
				pUser->SendSysMsg(STR_TEAM_EXPEX, nAddExp);
			else
				pUser->SendSysMsg(STR_TEAM_EXPERIENCE, nAddExp);
		}
		else
		{
			nAddExp = pUser->GetLev()*30;
			if (pUser->IsMate(pKiller))
				nAddExp = pUser->GetLev()*36;

			nAddExp += pUser->GetGemExpEffect()*nAddExp/100;
			nAddExp = (int)pUser->AwardBattleExp(nAddExp, true, 0);

			pUser->SendSysMsg(STR_TEAM_EXPERIENCE, nAddExp);
		}
	}
}

void CTeam::AddKillStatistic(OBJID idKiller, IRole* pTarget, int idEvent, int idType)
{
	for (int i=0; i<this->GetMemberAmount(); i++)
	{
		CUser* pUser	= NULL;
		OBJID idUser	= this->GetMemberByIndex(i);
		DEBUG_TRY
		pUser	= UserManager()->GetUser(idUser);
		DEBUG_CATCH("[Error in CTeam::AwardMemberStatistic Get user pt.]")
		if (!pUser || idUser == idKiller)
			continue;

		if (!pUser->IsAlive() || pUser->GetID() == idKiller)
			continue;

		if (pUser->GetMapID() != pTarget->GetMap()->GetID())
			continue;

		if (abs(pUser->GetPosX()-pTarget->GetPosX()) > _RANGE_EXPSHARE ||
				abs(pUser->GetPosY()-pTarget->GetPosY()) > _RANGE_EXPSHARE)
			continue;

		int nData = pUser->GetStatistic(idEvent, idType) + 1;
		pUser->SetStatistic(idEvent, idType, nData, true, (nData % 200) == 0, false);
		pUser->SetTeamKillCount(idKiller, pUser->GetTeamKillCount(idKiller) + 1);
		pUser->SetKillStatistic(idType, pUser->GetKillStatistic(idType) + 1);
		if (pUser->SumKillStatistic() >= 50)
			pUser->SendKillStatistic(true);
	}
}

void CTeam::SendAllKillStatistic()
{
	for (int i=0; i<this->GetMemberAmount(); i++)
	{
		CUser* pUser = UserManager()->GetUser(this->GetMemberByIndex(i));
		if (pUser)
			pUser->SendKillStatistic(false);
	}
}

void CTeam::BroadcastMemberLife(CUser* pMember, bool bMaxLife)
{
	if (!pMember)
		return;

	for (int i=0; i<this->GetMemberAmount(); i++)
	{
		CUser* pUser	= NULL;
		OBJID idUser	= this->GetMemberByIndex(i);
		DEBUG_TRY
		pUser	= UserManager()->GetUser(idUser);
		DEBUG_CATCH("Team broadcast member life get user by id crash.")
		if (pUser && pUser->GetID() != pMember->GetID())
		{
			CMsgUserAttrib msg;
			if (msg.Create(pMember->GetID(), _USERATTRIB_LIFE, pMember->GetLife()))
			{
				msg.Append(_USERATTRIB_MAXLIFE, pMember->GetMaxLife());

				pUser->SendMsg(&msg);
			}
		}
	}
}

BOOL CTeam::IsTeamMember(OBJID idMember)
{
	for(int i = 0; i < GetMemberAmount(); i++)
	{
		if(GetMemberByIndex(i) == idMember)
			return TRUE;
	}

	return FALSE;
}

CTeam* CTeam::CloneNew()
{
	CTeam* pTeam	= new CTeam(m_idProcess);
	CHECKF(pTeam);

	*pTeam = *this;

	return pTeam;
}

void CTeam::GetInfo(TeamInfoStruct* pInfo)
{
	memset(pInfo, 0, sizeof(TeamInfoStruct));
	pInfo->id		= m_id;
	pInfo->idLeader	= m_idLeader;
	for(int i = 0; i < (int)m_setIdUser.size() && i < _MAX_TEAMAMOUNT; i++)
	{
		pInfo->setMember[i]	= m_setIdUser[i];
	}
	pInfo->bCloseMoney	= m_bCloseMoney;
	pInfo->bCloseItem	= m_bCloseItem;
	pInfo->bUnk24		= m_bUnk28;
}

void CTeam::AddMemberOnly		(OBJID idMember)
{
	m_setIdUser.push_back(idMember);
}

void CTeam::DelMemberOnly		(OBJID idMember)
{
	USERIDSET::iterator iter = find(m_setIdUser.begin(), m_setIdUser.end(), idMember);
	if(iter != m_setIdUser.end())
		m_setIdUser.erase(iter);
}
