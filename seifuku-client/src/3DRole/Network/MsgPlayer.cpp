#include "AllMsg.h"
#include "Player.h"
#include "GamePlayerSet.h"
#include "Hero.h"
#include "3droledata.H"
#include "3DGameMap.h"
#include "GameMsg.h"

CMsgPlayer::CMsgPlayer()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
}

CMsgPlayer::~CMsgPlayer()
{

}

BOOL CMsgPlayer::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_PLAYER != this->GetType())
		return false;

	return true;
}

void CMsgPlayer::Process(void* pInfo)
{
#ifdef _MSGDEBUG
	::LogMsg("Process CMsgPlayer, id:%u, pos(%u,%u)", m_pInfo->id, m_pInfo->usPosX, m_pInfo->usPosY);
#endif
#ifdef _DEBUG
	static int nIndex = 0;
	char szDebug[128] ="";
	sprintf(szDebug, "CMsgPlayer: %d\n", nIndex++);
	::OutputDebugString(szDebug);
#endif

	if (m_pInfo->id == g_objHero.GetID())
	{
		::LogMsg("Got a player with hero id! %u", m_pInfo->id);
		return;
	}

	char szName[_MAX_NAMESIZE]		="";
	m_StrPacker.GetString(0, szName, _MAX_NAMESIZE);

	CPlayer* pPlayer	=g_objPlayerSet.GetPlayer(m_pInfo->id);
	if (!pPlayer)
	{
		pPlayer	=new CPlayer;
		MYASSERT (pPlayer);

		pPlayer->SetID(m_pInfo->id);
		pPlayer->SetName(szName);
		if(CRole::GetIDType(m_pInfo->id) == _PLAYER_ID_DYNNPC)
		{
			pPlayer->SetRoleType(_ROLE_STATUARY_NPC);
			pPlayer->Create(m_pInfo->dwLookFace%1000, _ROLE_VIEW_STATUARY);
		}
		else
		{
			pPlayer->SetRoleType(_ROLE_PLAYER);
			pPlayer->Create(m_pInfo->dwLookFace%1000);

			if (0 == strcmp(g_objHero.m_szWanted, szName))
			{
				CMsgName msg;
				if (msg.Create(NAMEACT_QUERY_WANTED, ""))
					msg.Send();
			}

			if (0 == strcmp(g_objHero.m_szPoliceWanted, szName))
			{
				CMsgName msg;
				if (msg.Create(NAMEACT_QUERY_POLICEWANTED, ""))
					msg.Send();
			}
		}

		MYASSERT (g_objPlayerSet.AddPlayer(pPlayer));
	}
	if (pPlayer->IsNpc())
	{
		pPlayer->SetLev(m_pInfo->usLevel);
		pPlayer->SetData(CPlayer::_PLAYER_LIFE, m_pInfo->usLife);

		const CMonsterInfo* pInfo = RoleDataQuery()->GetMonsterInfo(szName);
		if (pInfo)
		{
			pPlayer->SetMaxLife(pInfo->nMaxLife);
			pPlayer->SetAddSize(pInfo->nAddSize);
			pPlayer->SetZoomPercent(pInfo->nZoomPercent);
			pPlayer->SetArmet(pInfo->idArmet);
			pPlayer->SetRWeapon(pInfo->idRWeapon);
			pPlayer->SetLWeapon(pInfo->idLWeapon);
			pPlayer->SetMount(pInfo->idMount);
			pPlayer->SetMisc(pInfo->idMisc);
		}
	}
	else
	{
		pPlayer->SetWeaponRestrain(m_pInfo->usRWeaponRestrain, m_pInfo->usLWeaponRestrain);
		pPlayer->SetPotential(m_pInfo->nPotential);
	}

	pPlayer->ReplaceStatus(m_pInfo->dwStatus[0]);
	pPlayer->ReplaceStatus2(m_pInfo->dwStatus[1]);
	pPlayer->SetName(szName);

	pPlayer->SetDir(m_pInfo->ucDir);
	pPlayer->m_pIRoleView->SetVariable(_VAR_DIR, m_pInfo->ucDir);
	pPlayer->SetPose(m_pInfo->ucPose);

	pPlayer->SetFace((m_pInfo->dwLookFace/10000)%1000);
	pPlayer->SetSex((m_pInfo->dwLookFace/1000)%10);

	pPlayer->SetLook(m_pInfo->dwLookFace%1000);
	if (!pPlayer->IsNpc() || CRole::GetIDType(m_pInfo->id) == _PLAYER_ID_DYNNPC)
	{
		pPlayer->SetHair(m_pInfo->usHair % 1000);
		pPlayer->SetHead(m_pInfo->usHair / 1000);
		pPlayer->SetArmor(m_pInfo->dwArmorType);
		pPlayer->SetRWeapon(m_pInfo->dwWeaponRType);
		pPlayer->SetLWeapon(m_pInfo->dwWeaponLType);
		pPlayer->SetArmet(m_pInfo->dwArmetType);
		pPlayer->SetMantle(m_pInfo->dwGarmentType);
	}
	pPlayer->Transform(m_pInfo->dwLookFace/10000000);
	pPlayer->SetMetempsychosis(m_pInfo->ucMetempsychosis);

	OBJID idSyndicate = m_pInfo->dwSynID_Rank & 0x00ffffff;
	DWORD dwRank = m_pInfo->dwSynID_Rank >> 24;
	pPlayer->SetSyndicateID(idSyndicate);
	pPlayer->SetSyndicateRank(dwRank);
	if (pPlayer->IsDead() && !pPlayer->TestStatus(USERSTATUS_GHOST))
	{
		CCommand cmd;
		cmd.iType		=_COMMAND_POSE;
		cmd.iStatus		=_CMDSTATUS_BEGIN;
		cmd.nData		=_ACTION_BODY0;
		cmd.nDir		=m_pInfo->ucDir;

		pPlayer->SetCommand(&cmd);
	}
	else
	{
		CCommand cmd;
		cmd.iType		=_COMMAND_EMOTION;
		cmd.iStatus		=_CMDSTATUS_BEGIN;
		cmd.nData		=m_pInfo->ucPose;
		cmd.nDir		=m_pInfo->ucDir;

		pPlayer->SetCommand(&cmd);
	}

	int nPosX = m_pInfo->usPosX, nPosY = m_pInfo->usPosY;
	pPlayer->SetPos(nPosX, nPosY);
	if(CRole::GetIDType(m_pInfo->id) == _PLAYER_ID_DYNNPC)
	{
		pPlayer->m_pIRoleView->SetVariable(_VAR_FRAME, m_pInfo->usLevel);
		if(dwRank == RANK_LEADER)
		{
			pPlayer->SetMaxLife(RoleDataQuery()->GetLeaderMaxLife());
			pPlayer->SetZoomPercent(RoleDataQuery()->GetLeaderScale());
		}
		else if(dwRank == RANK_ASSISTANT)
		{
			pPlayer->SetMaxLife(RoleDataQuery()->GetAssistantMaxLife());
			pPlayer->SetZoomPercent(RoleDataQuery()->GetAssistantScale());
		}
		else if(dwRank == RANK_SUBLEADER)
		{
			pPlayer->SetMaxLife(RoleDataQuery()->GetSubLeaderMaxLife());
			pPlayer->SetZoomPercent(RoleDataQuery()->GetSubLeaderScale());
		}
		else
			return;
		pPlayer->SetData(CPlayer::_PLAYER_LIFE, m_pInfo->usLife*100);
	}
}
