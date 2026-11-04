#pragma warning(disable:4786)
#include "Player.h"
#include "GamePlayerSet.h"
#include "3DGameMap.h"
#include "Hero.h"
#include "AllMsg.h"
#include "3DRoleData.h"
#include "BoothMask.h"
CGamePlayerSet	g_objPlayerSet;
const DWORD CREATE_3DROLE_INTERVAL = 300;

CGamePlayerSet::CGamePlayerSet()
{
	this->Init();
	m_dwTimeFor3DRole = ::TimeGet();
	m_idWhiteSyn = m_idBlackSyn = ID_NONE;
}

CGamePlayerSet::~CGamePlayerSet()
{
	this->Destroy();
	this->DestroySyndicateInfo();
}

void CGamePlayerSet::DestroySyndicateInfo()
{
	int nAmount = m_setSyndicateInfo.size();
	for(int i = nAmount-1; i >= 0; i --)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		SAFE_DELETE(pInfo);
	}
	m_setSyndicateInfo.clear();
}
void CGamePlayerSet::Init(void)
{
	m_dequePlayer.clear();
	m_pFocusPlayer	=NULL;
}

void CGamePlayerSet::Destroy(void)
{
	this->DelAllPlayer();

	m_pFocusPlayer	=NULL;
}

BOOL CGamePlayerSet::AddPlayer(CPlayer* pPlayer)
{
	if (!CPlayer::IsValidPt(pPlayer))
		return false;

	if (this->GetPlayer(pPlayer->GetID()))
		return false;
	switch(pPlayer->GetRoleType())
	{
	case _ROLE_BOOTH_NPC:
		{
			CMyPos posNpc;
			pPlayer->GetPos(posNpc);
			int nDir = pPlayer->GetDir()%8;
			for(int i = 0; i < 5; i ++)
			{
				CMyPos posMask;
				posMask.x = posNpc.x + BoothMaskOffset[nDir][i][0];
				posMask.y = posNpc.y + BoothMaskOffset[nDir][i][1];
				CellInfo* pInfo = g_objGameMap.GetCell(posMask.x, posMask.y);
				if(pInfo)
				{
					LayerInfo* pLayerInfo = g_objGameMap.GetLastLayerInfo(pInfo);
					pLayerInfo->usMask = 1;
				}
			}
		}
		break;
	case _ROLE_SHOPKEEPER_NPC:
	case _ROLE_TASK_NPC:
	case _ROLE_STORAGE_NPC:
	case _ROLE_TRUNCK_NPC:
	case _ROLE_FACE_NPC:
	case _ROLE_FORGE_NPC:
	case _ROLE_EMBED_NPC:
	case _ROLE_SYNFLAG_NPC:
		{
			CMyPos posNpc;
			pPlayer->GetPos(posNpc);
			for(int x = posNpc.x-1; x <= posNpc.x+1; x ++)
			{
				for(int y = posNpc.y-1; y <= posNpc.y+1; y ++)
				{
					CellInfo* pInfo = g_objGameMap.GetCell(x, y);
					if(pInfo)
					{
						LayerInfo* pLayerInfo = g_objGameMap.GetLastLayerInfo(pInfo);
						pLayerInfo->usMask = 1;
					}
				}
			}
		}
		break;
	case _ROLE_STATUARY_NPC:
	case _SYNTRANS_NPC:
	case _ROLE_BOOTH_FLAG_NPC:
	case _ROLE_WEAPONGOAL_NPC:
	case _ROLE_MAGICGOAL_NPC:
	case _ROLE_BOWGOAL_NPC:
		{
			CMyPos posNpc;
			pPlayer->GetPos(posNpc);
			CMyPos posMask;
			posMask.x = posNpc.x;
			posMask.y = posNpc.y;
			CellInfo* pInfo = g_objGameMap.GetCell(posMask.x, posMask.y);
			if(pInfo)
			{
				LayerInfo* pLayerInfo = g_objGameMap.GetLastLayerInfo(pInfo);
				pLayerInfo->usMask = 1;
			}
		}
		break;
	default:
		break;
	}
	m_dequePlayer.push_back(pPlayer);
	g_objGameMap.AddInteractiveObj(pPlayer);

	return true;
}

BOOL CGamePlayerSet::DelPlayer(OBJID id, BOOL bDelObj)
{
	int nPlayerAmount	=m_dequePlayer.size();

	CPlayer* pMyPlayer = this->GetPlayer(id);
	if(pMyPlayer && pMyPlayer->IsMyPet())
	{
		g_objHero.DelPet(id);
	}

	for (int i=nPlayerAmount-1; i>=0; i--)
	{
		CPlayer* pPlayer = m_dequePlayer[i];
		if(id == pPlayer->GetID())
		{
			if (m_pFocusPlayer == pPlayer)
				m_pFocusPlayer = NULL;

			switch(pPlayer->GetRoleType())
			{
			case _ROLE_BOOTH_NPC:
				{
					CMyPos posNpc;
					pPlayer->GetPos(posNpc);
					for(int i = 0; i < 5; i ++)
					{
						CMyPos posMask;
						int nDir = pPlayer->GetDir()%8;
						posMask.x = posNpc.x + BoothMaskOffset[nDir][i][0];
						posMask.y = posNpc.y + BoothMaskOffset[nDir][i][1];
						CellInfo* pInfo = g_objGameMap.GetCell(posMask.x, posMask.y);
						if(pInfo)
						{
							LayerInfo* pLayerInfo = g_objGameMap.GetLastLayerInfo(pInfo);
							pLayerInfo->usMask = 0;
						}
					}

					{
						CPlayer* pOwner = this->GetPlayer(pPlayer->GetID(), true);
						if (pOwner)
							pOwner->SetCryOut("");
					}
				}
				break;
			case _ROLE_SHOPKEEPER_NPC:
			case _ROLE_TASK_NPC:
			case _ROLE_STORAGE_NPC:
			case _ROLE_TRUNCK_NPC:
			case _ROLE_FACE_NPC:
			case _ROLE_FORGE_NPC:
			case _ROLE_EMBED_NPC:
			case _ROLE_SYNFLAG_NPC:
				{
					CMyPos posNpc;
					pPlayer->GetPos(posNpc);
					for(int x = posNpc.x-1; x <= posNpc.x+1; x ++)
					{
						for(int y = posNpc.y-1; y <= posNpc.y+1; y ++)
						{
							CellInfo* pInfo = g_objGameMap.GetCell(x, y);
							if(pInfo)
							{
								LayerInfo* pLayerInfo = g_objGameMap.GetLastLayerInfo(pInfo);
								pLayerInfo->usMask = 0;
							}
						}
					}
				}
				break;
			case _ROLE_STATUARY_NPC:
			case _SYNTRANS_NPC:
			case _ROLE_BOOTH_FLAG_NPC:
			case _ROLE_WEAPONGOAL_NPC:
			case _ROLE_MAGICGOAL_NPC:
			case _ROLE_BOWGOAL_NPC:
				{
					CMyPos posNpc;
					pPlayer->GetPos(posNpc);
					CellInfo* pInfo = g_objGameMap.GetCell(posNpc.x, posNpc.y);
					if(pInfo)
					{
						LayerInfo* pLayerInfo = g_objGameMap.GetLastLayerInfo(pInfo);
						pLayerInfo->usMask = 0;
					}
				}
				break;
			default:
				break;
			}
			if (bDelObj)
			{
				g_objGameMap.DelInteractiveObj(pPlayer);
				SAFE_DELETE (pPlayer);
			}

			m_dequePlayer.erase(m_dequePlayer.begin()+i);
			return true;
		}
	}

	return false;
}

void CGamePlayerSet::DelAllPlayer(BOOL bExcludeMyPet)
{
	if(bExcludeMyPet)
	{
		int nPlayerAmount	=m_dequePlayer.size();
		for (int i=nPlayerAmount-1; i>=0; i--)
		{
			CPlayer* pPlayer = m_dequePlayer[i];
			if(!pPlayer->IsMyPet())
			{
				if (CPlayer::IsValidPt(pPlayer))
				{
					g_objGameMap.DelInteractiveObj(pPlayer);
					SAFE_DELETE (pPlayer);
					m_dequePlayer.erase(m_dequePlayer.begin()+i);
				}
				else
				{
					g_objGameMap.DelInteractiveObj(pPlayer);
					m_dequePlayer.erase(m_dequePlayer.begin()+i);
				}
			}
		}
	}
	else
	{
		int nPlayerAmount	=m_dequePlayer.size();
		for (int i=nPlayerAmount-1; i>=0; i--)
		{
			CPlayer* pPlayer = m_dequePlayer[i];
			if (CPlayer::IsValidPt(pPlayer))
			{
				g_objGameMap.DelInteractiveObj(pPlayer);
				SAFE_DELETE (pPlayer);
			}
			else
			{
				g_objGameMap.DelInteractiveObj(pPlayer);
				m_dequePlayer.erase(m_dequePlayer.begin()+i);
			}
		}
		m_dequePlayer.clear();
	}
	m_pFocusPlayer = NULL;
}

CPlayer* CGamePlayerSet::GetPlayerByIndex(DWORD dwIndex) const
{
	if(dwIndex >= m_dequePlayer.size())
		return NULL;

	return m_dequePlayer[dwIndex];
}

CPlayer* CGamePlayerSet::GetPlayer	(const char* pszName) const
{
	if (!pszName)
		return NULL;

	int nPlayerAmount	=m_dequePlayer.size();
	for (int i=nPlayerAmount-1; i>=0; i--)
	{
		CPlayer* pPlayer = m_dequePlayer[i];
		if (!pPlayer)
			continue;

		if (!pPlayer->IsNpc() &&
				0 == strcmp(pPlayer->GetName(), pszName))
		{
			return pPlayer;
		}
	}

	return NULL;
}

CPlayer* CGamePlayerSet::GetPlayer	(OBJID id, BOOL bIncludeSelf) const
{
	if(bIncludeSelf && g_objHero.GetID() == id)
		return &g_objHero;
	int nPlayerAmount	=m_dequePlayer.size();
	for (int i=nPlayerAmount-1; i>=0; i--)
	{
		CPlayer* pPlayer	=m_dequePlayer[i];
		if (!pPlayer)
			continue;

		if(id == pPlayer->GetID())
			return pPlayer;
	}

	return NULL;
}

CPlayer* CGamePlayerSet::GetPlayer	(int nMapX, int nMapY, int nRoleType) const
{
	int nPlayerAmount	=m_dequePlayer.size();
	for (int i=nPlayerAmount-1; i>=0; i--)
	{
		CPlayer* pPlayer	=m_dequePlayer[i];
		if (!pPlayer)
			continue;

		CMyPos posPlayer;
		pPlayer->GetPos(posPlayer);

		if(nMapX == posPlayer.x && nMapY == posPlayer.y)
		{
			if(nRoleType == _ROLE_NPC_NONE)
				return pPlayer;
			else if(pPlayer->GetRoleType() == nRoleType)
				return pPlayer;
		}
	}

	return NULL;
}

CPlayer* CGamePlayerSet::GetFocusPlayer(DWORD dwFlag )
{
	if (PLAYER_NORMAL == dwFlag)
	{
		if (CPlayer::IsValidPt(m_pFocusPlayer))
			return m_pFocusPlayer;
		else
		{
			m_pFocusPlayer = NULL;
			return NULL;
		}
	}

	CPlayer* pFocusPlayer = NULL;
	if ((dwFlag&PLAYER_INCLUDESELF) && g_objHero.IsMouseFocus())
		pFocusPlayer = &g_objHero;

	int iPlayerAmount = this->GetPlayerAmount();
	for(int i=iPlayerAmount-1; i>=0; i--)
	{
		CPlayer* pPlayer = m_dequePlayer[i];
		if (!CPlayer::IsValidPt(pPlayer))
			continue;
		if (!pPlayer->IsVisible())
			continue;
		if (pPlayer->GetID() == MOUSE_PLAYER_ID)
			continue;
		if (pPlayer->IsDead())
		{
			if (!(dwFlag&PLAYER_INCLUDEDEAD))
				continue;
		}
		if (pPlayer->IsMouseFocus())
		{
			if (pFocusPlayer)
			{
				CMyPos posFocusPlayer;
				pFocusPlayer->GetPos(posFocusPlayer);

				CMyPos posPlayer;
				pPlayer->GetPos(posPlayer);

				if (g_objHero.GetPkMode() == PKMODE_MONSTERONLY && !pFocusPlayer->IsNpc() && pPlayer->IsNpc())
				{
					pFocusPlayer = pPlayer;
				}
				if ((posPlayer.x+posPlayer.y >= posFocusPlayer.x+posFocusPlayer.y) ||
					(g_objHero.GetPkMode() == PKMODE_ARRESTMENT && !pFocusPlayer->IsBaddy() && pPlayer->IsBaddy()))
				{
					if (dwFlag&PLAYER_MONSTERPRI)
					{
						if (pFocusPlayer->IsNpc())
						{
							if (pPlayer->IsNpc())
								pFocusPlayer = pPlayer;
						}
						else
						{
							if (pPlayer->IsNpc())
								pFocusPlayer = pPlayer;
							else
							{
								if (g_objHero.GetFriend(pFocusPlayer->GetID())
										|| g_objHero.GetTeamMemberInfoByID(pFocusPlayer->GetID())
										|| g_objHero.GetSydicateID() == pFocusPlayer->GetSydicateID())
								{
									pFocusPlayer = pPlayer;
								}
								else
								{
									if (g_objHero.GetFriend(pFocusPlayer->GetID())
											|| g_objHero.GetTeamMemberInfoByID(pFocusPlayer->GetID())
											|| g_objHero.GetSydicateID() == pFocusPlayer->GetSydicateID())
									{
									}
									else
										pFocusPlayer = pPlayer;
								}
							}
						}
					}
					else if (dwFlag&PLAYER_PLAYERPRI)
					{
						if (!pFocusPlayer->IsNpc())
						{
							if (!pPlayer->IsNpc())
								pFocusPlayer = pPlayer;
						}
						else
						{
							pFocusPlayer = pPlayer;
						}
					}
					else
					{
						pFocusPlayer = pPlayer;
					}
				}
				if (g_objGameMap.GetType())
				{
					if (pPlayer->GetObjType() == 16 || pPlayer->GetObjType() == 14)
						pFocusPlayer = pPlayer;
				}
			}
			else
				pFocusPlayer = pPlayer;
		}
	}

	return pFocusPlayer;
}

void CGamePlayerSet::ShowX(void)
{
	int nPlayerAmount = m_dequePlayer.size();
	for (int i = nPlayerAmount-1; i >= 0; i--)
	{
		CPlayer* pPlayer = m_dequePlayer[i];
		if (CPlayer::IsValidPt(pPlayer))
			pPlayer->ShowScrEffect();
	}
	g_objHero.ShowScrEffect();
	if (CPlayer::IsValidPt(m_pFocusPlayer))
	{
		m_pFocusPlayer->ShowInfo();
	}
}

void CGamePlayerSet::ProcessManager(int nHeroX, int nHeroY)
{
	int iPlayerAmount = this->GetPlayerAmount();
	for(int i=iPlayerAmount-1; i>=0; i--)
	{
		CPlayer* pPlayer	=m_dequePlayer[i];
		if (CPlayer::IsValidPt(pPlayer) && _ROLE_CALL_PET != pPlayer->GetRoleType() && MOUSE_PLAYER_ID != pPlayer->GetID())
		{
			if (nHeroX > 0 && nHeroY > 0)
			{
				int nPosX=0, nPosY=0;
				CMyPos posHero, posPlayer;
				posHero.x = nHeroX;
				posHero.y = nHeroY;
				pPlayer->GetPos(posPlayer);

				if(g_objGameMap.Outof9Block(posHero, posPlayer))
				{
					if (m_pFocusPlayer == pPlayer)
						m_pFocusPlayer = NULL;
					g_objGameMap.DelInteractiveObj(pPlayer);

					m_dequePlayer.erase(m_dequePlayer.begin()+i);
					SAFE_DELETE(pPlayer);
				}
			}
		}
	}
}

void CGamePlayerSet::Process(int nHeroX, int nHeroY)
{
	CMyPos posAlign = g_objHero.GetAlignPos();
	this->ProcessManager(posAlign.x, posAlign.y);

	m_pFocusPlayer	=NULL;
	DWORD dwDistance = 999;

	int iPlayerAmount = this->GetPlayerAmount();
	for(int i=iPlayerAmount-1; i>=0; i--)
	{
		CPlayer* pPlayer	=m_dequePlayer[i];
		if (CPlayer::IsValidPt(pPlayer))
		{
			if (!pPlayer->IsDead() && pPlayer->IsMouseFocus())
			{
				if (m_pFocusPlayer)
				{
					CMyPos posFocusPlayer;
					m_pFocusPlayer->GetPos(posFocusPlayer);

					CMyPos posPlayer;
					pPlayer->GetPos(posPlayer);

					if (posPlayer.x+posPlayer.y >= posFocusPlayer.x+posFocusPlayer.y)
						m_pFocusPlayer = pPlayer;
					if (g_objHero.GetPkMode() == PKMODE_MONSTERONLY && !m_pFocusPlayer->IsNpc() && pPlayer->IsNpc())
						m_pFocusPlayer = pPlayer;
				}
				else
					m_pFocusPlayer = pPlayer;
			}

		}
		else
		{
			if (m_pFocusPlayer == pPlayer)
				m_pFocusPlayer = NULL;

			g_objGameMap.DelInteractiveObj(pPlayer);
			m_dequePlayer.erase(m_dequePlayer.begin()+i);
		}
	}
}

char*	CGamePlayerSet::GetSyndicateLeaderName(OBJID idSyndaicate)
{
	const OBJID idStrNone = 100021;
	static InfoSyndicate infoNoSyndicate = {ID_NONE, "NONE"};
	strcpy(infoNoSyndicate.szSyndicateName, GameDataSetQuery()->GetStr(idStrNone));
	if(idSyndaicate == ID_NONE)
		return infoNoSyndicate.szSyndicateName;

	int nAmount = m_setSyndicateInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		if(pInfo && pInfo->idSyndicate == idSyndaicate)
			return pInfo->szSyndicateLeaderName;
	}
	return "";
}
char* CGamePlayerSet::GetSyndicateName(OBJID idSyndaicate)
{
	const OBJID idStrNone = 100021;
	static InfoSyndicate infoNoSyndicate = {ID_NONE, "NONE"};
	strcpy(infoNoSyndicate.szSyndicateName, GameDataSetQuery()->GetStr(idStrNone));
	if(idSyndaicate == ID_NONE)
		return infoNoSyndicate.szSyndicateName;

	int nAmount = m_setSyndicateInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		if(pInfo && pInfo->idSyndicate == idSyndaicate)
			return pInfo->szSyndicateName;
	}

	CMsgSyndicate msg;
	if(msg.Create(QUERY_SYN_NAME, idSyndaicate))
		msg.Send();

	InfoSyndicate* pInfo = new InfoSyndicate;

	MYASSERT(pInfo);
	pInfo->idSyndicate = idSyndaicate;
	strcpy(pInfo->szSyndicateName, "");
	strcpy(pInfo->szSyndicateLeaderName, "");
	pInfo->dwStatus	= SYNDICATE_NORMAL;
	m_setSyndicateInfo.push_back(pInfo);
	return pInfo->szSyndicateName;
}
void CGamePlayerSet::SetSyndicateName(OBJID idSyndaicate, char* pszName, char* pszLeaderName)
{
	if(!pszName || !pszLeaderName)
		return;

	int nAmount = m_setSyndicateInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		if(pInfo && pInfo->idSyndicate == idSyndaicate)
		{
			strcpy(pInfo->szSyndicateName, pszName);
			strcpy(pInfo->szSyndicateLeaderName, pszLeaderName);
			return;
		}
	}

	return;
}
void  CGamePlayerSet::SetSyndicateStatus(OBJID idSyndaicate, DWORD dwStatus)
{
	CGamePlayerSet::GetSyndicateName(idSyndaicate);
	int nAmount = m_setSyndicateInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		if(pInfo && pInfo->idSyndicate == idSyndaicate)
		{
			pInfo->dwStatus = dwStatus;
			return;
		}
	}
}
void CGamePlayerSet::ClearAllSyndicateStatus()
{
	int nAmount = m_setSyndicateInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		if(pInfo)
		{
			pInfo->dwStatus = SYNDICATE_NORMAL;
		}
	}
}
DWORD	CGamePlayerSet::GetSyndicateStatus(OBJID idSyndaicate)
{
	int nAmount = m_setSyndicateInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		if(pInfo && pInfo->idSyndicate == idSyndaicate)
		{
			return pInfo->dwStatus;
		}
	}
	return SYNDICATE_NORMAL;
}
DWORD	CGamePlayerSet::GetAllySyndicateAmount()
{
	int nAmount = m_setSyndicateInfo.size();
	int nAllySyndicateAmount = 0;
	for(int i = 0; i < nAmount; i ++)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		if(pInfo->dwStatus == SYNDICATE_ALLY)
		{
			nAllySyndicateAmount ++;
		}
	}
	return nAllySyndicateAmount;
}
char*	CGamePlayerSet::GetAllySyndicateNameByIndex(int nIndex)
{
	int nAmount = m_setSyndicateInfo.size();
	int nAllySyndicateAmount = 0;
	for(int i = 0; i < nAmount; i ++)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		if(pInfo->dwStatus == SYNDICATE_ALLY)
		{
			if(nAllySyndicateAmount == nIndex)
				return pInfo->szSyndicateName;
			nAllySyndicateAmount ++;
		}
	}
	return NULL;
}
DWORD	CGamePlayerSet::GetAntagonizeSyndicateAmount()
{
	int nAmount = m_setSyndicateInfo.size();
	int nAntagonizeSyndicateAmount = 0;
	for(int i = 0; i < nAmount; i ++)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		if(pInfo->dwStatus == SYNDICATE_ANTAGONIZE)
		{
			nAntagonizeSyndicateAmount ++;
		}
	}
	return nAntagonizeSyndicateAmount;
}
char*	CGamePlayerSet::GetAntagonizeNameByIndex(int nIndex)
{
	int nAmount = m_setSyndicateInfo.size();
	int nAntagonizeSyndicateAmount = 0;
	for(int i = 0; i < nAmount; i ++)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		if(pInfo->dwStatus == SYNDICATE_ANTAGONIZE)
		{
			if(nAntagonizeSyndicateAmount == nIndex)
				return pInfo->szSyndicateName;
			nAntagonizeSyndicateAmount ++;
		}
	}
	return NULL;
}
void	CGamePlayerSet::UpdateSyndicateName(OBJID idSyndaicate)
{
	int nAmount = m_setSyndicateInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		InfoSyndicate* pInfo = m_setSyndicateInfo[i];
		if(pInfo && pInfo->idSyndicate == idSyndaicate)
			SAFE_DELETE(pInfo);
		m_setSyndicateInfo.erase(m_setSyndicateInfo.begin()+i);
		return;
	}
}
void	CGamePlayerSet::SetWhiteSynId(OBJID idWhiteSyn)
{
}
OBJID	CGamePlayerSet::GetWhiteSynId()
{
	return m_idWhiteSyn;
}
void	CGamePlayerSet::SetBlackSynId(OBJID idBlackSyn)
{
}
OBJID	CGamePlayerSet::GetBlackSynId()
{
	return m_idBlackSyn;
}
void	CGamePlayerSet::DestroySyndicate(OBJID idSyndicate)
{
	if(g_objHero.GetSydicateID() == idSyndicate)
	{
		g_objHero.SetSyndicateAnnounce("");
		g_objHero.SetSyndicateFund(0);
		g_objHero.SetSyndicateLeaderName("");
		g_objHero.SetSyndicatePopulation(0);
		g_objHero.SetSyndicateProffer(0);
		g_objHero.SetSyndicateRank(RANK_NONE);
		g_objHero.SetSyndicateID(ID_NONE);
	}

	int nAmount = this->GetPlayerAmount();
	for(int i = 0; i < nAmount; i ++)
	{
		CPlayer* pPlayer = this->GetPlayerByIndex(i);
		if(pPlayer && pPlayer->GetSydicateID() == idSyndicate)
		{
			pPlayer->SetSyndicateID(ID_NONE);
			pPlayer->SetSyndicateFund(0);
			pPlayer->SetSyndicateLeaderName("");
			pPlayer->SetSyndicatePopulation(0);
			pPlayer->SetSyndicateProffer(0);
			pPlayer->SetSyndicateRank(ID_NONE);
		}
	}
}

int CGamePlayerSet::GetRestrainMultiple(CPlayer* pSender, CPlayer* pTarget)
{
	if (!pSender || !pTarget)
		return 1;

	int nMultiple = 1;
	if (!pSender->IsNpc() && pTarget->IsMonster())
	{
		CONST CMonsterInfo* pInfo = RoleDataQuery()->GetMonsterInfo(pTarget->GetName());
		if (pInfo
				&& ((pSender->GetRWeaponRestrain() != 0 && pSender->GetRWeaponRestrain() == (USHORT)pInfo->dwAntiType)
					|| (pSender->GetLWeaponRestrain() != 0 && pSender->GetLWeaponRestrain() == (USHORT)pInfo->dwAntiType)))
		{
			CONST RestrainInfo* pRestrain = RoleDataQuery()->GetRestrainInfo(pInfo->dwAntiType);
			if (pRestrain)
				nMultiple = pRestrain->dwMultiple;
			else
				nMultiple = 2;
		}
	}
	return nMultiple;
}
int CGamePlayerSet::GetRestrainMultiple(OBJID idSender, OBJID idTarget)
{
	CPlayer* pSender = this->GetPlayer(idSender, true);
	CPlayer* pTarget = this->GetPlayer(idTarget, true);
	if (pSender && pTarget)
		return this->GetRestrainMultiple(pSender, pTarget);
	return 1;
}
