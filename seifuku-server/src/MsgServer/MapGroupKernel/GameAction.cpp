#include <ctype.h>
#include "define.h"
#include "windows.h"
#include "I_Role.h"
#include "User.h"
#include "Item.h"
#include "GameAction.h"
#include "MapGroup.h"
#include "AllMsg.h"
#include "ActionDefine.h"
#include "WantedList.h"
#include "PoliceWanted.h"
#include "NpcLink.h"
#include "MapTrap.h"
#include "DeadLoop.h"
#include "string.h"
#include "EventPack.h"

const int	PRESENTMONEY_LIMIT				= 10000;
const int	ENTER_HOME_TEAM_DISTANCE		= 5;
const int	MAX_UNLEARN_NUMBER				= 20;
const int	SUBSYN_NAMESIZE					= 16;
const int	MAX_ACCEPTSTR					= 4;

MYHEAP_IMPLEMENTATION(CGameAction,s_heap)
CGameAction::CGameAction(PROCESS_ID idProcess)
{
	m_idProcess	= idProcess;
	m_pUser		= NULL;
	m_pRole		= NULL;
	m_pItem		= NULL;
	memset(&m_infoPkBonus, 0, sizeof(m_infoPkBonus));
}

void CGameAction::ReplaceAttrStr(char* pszTarget, const char* pszSource, LPCTSTR pszAccept, CUser* pUser, IRole* pRole, CItem* pItem)
{
	if(!pszTarget || !pszSource)
	{
		MYASSERT(!"CJobNpc::ReplaceAttrStr");
		return;
	}

	if(strchr(pszSource, '%') == NULL)
	{
		SafeCopy(pszTarget, pszSource);
		return;
	}

	CNpc* pNpc = NULL;
	if(pRole)
		pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc));

	const char*	ptr = pszSource;
	char*	ptr2 = pszTarget;
	while(*ptr)
	{
		if(*ptr == '%')
		{
			ptr++;

			int nWidth	= 0;
			if(isdigit(*ptr))
				nWidth	= nWidth*10 + (*(ptr++))-'0';
			if(isdigit(*ptr))
				nWidth	= nWidth*10 + (*(ptr++))-'0';
			const char* pNext = ptr2 + nWidth;

			if(*(ptr) == '%')
			{
				*ptr2	= '%';

				ptr	+= 2;
				ptr2++;
			}
			if(pUser)
			{
				if (strnicmp(ptr, PARA_ITER_EMONEY, sizeof(PARA_ITER_EMONEY)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%u", pUser->GetEmoney());

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_EMONEY)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_ITER_BLESSTIME, sizeof(PARA_ITER_BLESSTIME)-1) == 0)
				{
					char szString[256] = "--";
					pUser->GetBlessTimeStr(szString);

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_BLESSTIME)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_EMONEY_CARD1, sizeof(PARA_EMONEY_CARD1)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%d", pUser->GetEmoneyCard1());

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_EMONEY_CARD1)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_EMONEY_CARD2, sizeof(PARA_EMONEY_CARD2)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%d", pUser->GetEmoneyCard2());

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_EMONEY_CARD2)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_DRAGON_TYPE1, sizeof(PARA_DRAGON_TYPE1)-1) == 0)
				{
					char szString[256] = "";
					_snprintf(szString, 256, "%d", pUser->GetDragonType(1));

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_DRAGON_TYPE1)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_DRAGON_TYPE2, sizeof(PARA_DRAGON_TYPE2)-1) == 0)
				{
					char szString[256] = "";
					_snprintf(szString, 256, "%d", pUser->GetDragonType(2));

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_DRAGON_TYPE2)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_BUSINESSMAN_DAYS, sizeof(PARA_BUSINESSMAN_DAYS)-1) == 0)
				{
					char szString[256] = "";
					_snprintf(szString, 256, "%d", pUser->GetBusinessmanDays());

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_BUSINESSMAN_DAYS)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_ITER_TIME, sizeof(PARA_ITER_TIME)-1) == 0)
				{
					time_t	tCurr = time(NULL);
					tm*	pTm = localtime(&tCurr);
					DWORD	dwTime = ((((pTm->tm_year%100)*100 + pTm->tm_mon+1)*100 + pTm->tm_mday)*100 + pTm->tm_hour)*100 + pTm->tm_min;
					char szString[256] = "--";
					sprintf(szString, "%u", dwTime);

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_TIME)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_USER_LEV, sizeof(PARA_USER_LEV)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%d", pUser->GetLev());

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_USER_LEV)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_USER_SEX, sizeof(PARA_USER_SEX)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%d", pUser->GetSex());

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_USER_SEX)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_USER_TEAM, sizeof(PARA_USER_TEAM)-1) == 0)
				{
					OBJID idTeam = pUser->GetTeam() ? pUser->GetTeam()->GetID() : ID_NONE;
					char szString[256] = "--";
					sprintf(szString, "%d", idTeam);

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_USER_TEAM)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_USER_METO, sizeof(PARA_USER_METO)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%d", pUser->GetMetempsychosis());

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_USER_METO)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_USER_PRO, sizeof(PARA_USER_PRO)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%d", pUser->GetProfession());

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_USER_PRO)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_ITER_RAND, sizeof(PARA_ITER_RAND)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%d", rand());

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_RAND)-1;
					ptr2 += strlen(szString);
				}
				else if(strnicmp(ptr, PARA_ITER_VAR_DATA_, sizeof(PARA_ITER_VAR_DATA_)-1) == 0
							&& *(ptr+sizeof(PARA_ITER_VAR_DATA_)-1) >= '0' && *(ptr+sizeof(PARA_ITER_VAR_DATA_)-1) < '8')
				{
					char szString[256] = "";
					sprintf(szString, "%I64d", pUser->GetVarData(*(ptr+sizeof(PARA_ITER_VAR_DATA_)-1) - '0'));

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_VAR_DATA_)-1 + 1;
					ptr2 += strlen(szString);
				}
				else if(strnicmp(ptr, PARA_ITER_VAR_STR_, sizeof(PARA_ITER_VAR_STR_)-1) == 0
							&& *(ptr+sizeof(PARA_ITER_VAR_STR_)-1) >= '0' && *(ptr+sizeof(PARA_ITER_VAR_STR_)-1) < '8')
				{
					char szString[256] = "";
					sprintf(szString, "%s", pUser->GetVarStr(*(ptr+sizeof(PARA_ITER_VAR_STR_)-1) - '0'));

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_VAR_STR_)-1 + 1;
					ptr2 += strlen(szString);
				}
				else if(strnicmp(ptr, ACCEPT_STR_, sizeof(ACCEPT_STR_)-1) == 0
							&& *(ptr+sizeof(ACCEPT_STR_)-1) >= '0' && *(ptr+sizeof(ACCEPT_STR_)-1) < '0'+ MAX_ACCEPTSTR)
				{
					char	szStr[4][256] = { "", "", "", "" };
					if(pszAccept)
						sscanf(pszAccept, "%s %s %s %s", szStr[0], szStr[1], szStr[2], szStr[3]);
					char	szNum[256] = "";
					sprintf(szNum, "%s", szStr[*(ptr+sizeof(ACCEPT_STR_)-1) - '0']);
					strcpy(ptr2, szNum);

					ptr	+= sizeof(ACCEPT_STR_)-1 + 1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, ACCEPT_DATA_, sizeof(ACCEPT_DATA_)-1) == 0
							&& *(ptr+sizeof(ACCEPT_DATA_)-1) >= '0' && *(ptr+sizeof(ACCEPT_DATA_)-1) < '0'+ MAX_ACCEPTSTR)
				{
					int		nData[4] = { 0 };
					if(pszAccept)
						sscanf(pszAccept, "%d %d %d %d", &nData[0], &nData[1], &nData[2], &nData[3]);
					char	szNum[256] = "";
					sprintf(szNum, "%d", nData[*(ptr+sizeof(ACCEPT_DATA_)-1) - '0']);
					strcpy(ptr2, szNum);

					ptr	+= sizeof(ACCEPT_DATA_)-1 + 1;
					ptr2		+= strlen(szNum);
				}
				else if (strnicmp(ptr, PARA_ITER_GAME_CARD4, sizeof(PARA_ITER_GAME_CARD4)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%d", pUser->CountCardByType(780003));

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_GAME_CARD4)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_ITER_GAME_CARD3, sizeof(PARA_ITER_GAME_CARD3)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%d", pUser->CountCardByType(780002));

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_GAME_CARD3)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_ITER_GAME_CARD2, sizeof(PARA_ITER_GAME_CARD2)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%d", pUser->CountCard2());

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_GAME_CARD2)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_ITER_GAME_CARD, sizeof(PARA_ITER_GAME_CARD)-1) == 0)
				{
					char szString[256] = "--";
					sprintf(szString, "%d", pUser->CountCard());

					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_GAME_CARD)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_ITER_COST_DURRECOVER, sizeof(PARA_ITER_COST_DURRECOVER)-1) == 0)
				{
					char szString[256] = "--";
					if (pUser->TaskIterator() != 0)
					{
						CItem* pItem = pUser->GetEquipItemByPos(pUser->TaskIterator());
						if (pItem)
							sprintf(szString, "%d", pItem->GetRecoverDurCost());
					}
					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_COST_DURRECOVER)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_ITER_GEMSUPQUALITY, sizeof(PARA_ITER_GEMSUPQUALITY)-1) == 0)
				{
					char szString[256] = "--";
					if (pUser->TaskIterator() != 0)
					{
						int nChance = pUser->GetChanceUpEpQuality(pUser->TaskIterator(), 0);

						int nGemCost = 0;
						if (nChance >= 0)
							nGemCost = 100/__max(1, nChance) + 1;

						sprintf(szString, "%d", nGemCost);
					}
					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_GEMSUPQUALITY)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_ITER_GEMSUPLEVEL, sizeof(PARA_ITER_GEMSUPLEVEL)-1) == 0)
				{
					char szString[256] = "--";
					if (pUser->TaskIterator() != 0)
					{
						int nChance = pUser->GetChanceUpEpLevel(pUser->TaskIterator(), 0);

						int nGemCost = 0;
						if (nChance >= 0)
							nGemCost = (100/__max(1, nChance) + 1)*12/10;

						sprintf(szString, "%d", nGemCost);
					}
					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_GEMSUPLEVEL)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_ITER_POLICEWANTED, sizeof(PARA_ITER_POLICEWANTED)-1) == 0)
				{
					char szString[256] = "--";
					{
						PoliceWantedStruct* pWanted = PoliceWanted().GetWantedByIndex(pUser->TaskIterator());
						if (pWanted)
						{
							sprintf(szString, "%03u %-15s %-15s %03d %d",
									pUser->TaskIterator(),
									pWanted->strName.c_str(),
									pWanted->strSynName.c_str(),
									pWanted->nLev,
									pWanted->nPk);
						}
					}
					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_POLICEWANTED)-1;
					ptr2 += strlen(szString);
				}
				else if (strnicmp(ptr, PARA_ITER_WANTED, sizeof(PARA_ITER_WANTED)-1) == 0)
				{
					char szString[256] = "--";
					{
						CWantedData* pWanted = CWantedList::s_WantedList.GetWantedByIndex(pUser->TaskIterator());
						if (pWanted)
						{
							sprintf(szString, "%06u %-15s %u", pWanted->GetInt(DATA_ID), pWanted->GetStr(DATA_TARGET_NAME), pWanted->GetInt(DATA_BOUNTY));
						}
					}
					strcpy(ptr2, szString);

					ptr += sizeof(PARA_ITER_WANTED)-1;
					ptr2 += strlen(szString);
				}
				else if(strnicmp(ptr, PARA_ENEMY_SYN_, sizeof(PARA_ENEMY_SYN_)-1) == 0
							&& *(ptr+sizeof(PARA_ENEMY_SYN_)-1) >= '0' && *(ptr+sizeof(PARA_ENEMY_SYN_)-1) < '0'+ MAX_SYNENEMYSIZE)
				{
					char	szNum[256] = "--";
					int		idx = *(ptr+sizeof(PARA_ENEMY_SYN_)-1) - '0';
					if(idx >= 0 && idx < MAX_SYNENEMYSIZE)
					{
						CSynPtr pSyn = SynManager()->QuerySyndicate(pUser->GetSynID());
						if(pSyn)
						{
							CSynPtr pTarget = SynManager()->QuerySyndicate(pSyn->GetInt((SYNDATA)(SYNDATA_ENEMY0+idx)));
							if(pTarget)
								sprintf(szNum, "%s", pTarget->GetStr(SYNDATA_NAME));
						}
					}
					strcpy(ptr2, szNum);

					ptr	+= sizeof(PARA_ENEMY_SYN_)-1 + 1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_ALLY_SYN_, sizeof(PARA_ALLY_SYN_)-1) == 0
							&& *(ptr+sizeof(PARA_ALLY_SYN_)-1) >= '0' && *(ptr+sizeof(PARA_ALLY_SYN_)-1) < '0'+ MAX_SYNALLYSIZE)
				{
					char	szNum[256] = "--";
					int		idx = *(ptr+sizeof(PARA_ALLY_SYN_)-1) - '0';
					if(idx >= 0 && idx < MAX_SYNALLYSIZE)
					{
						CSynPtr pSyn = SynManager()->QuerySyndicate(pUser->GetSynID());
						if(pSyn)
						{
							CSynPtr pTarget = SynManager()->QuerySyndicate(pSyn->GetInt((SYNDATA)(SYNDATA_ALLY0+idx)));
							if(pTarget)
								sprintf(szNum, "%s", pTarget->GetStr(SYNDATA_NAME));
						}
					}
					strcpy(ptr2, szNum);

					ptr	+= sizeof(PARA_ALLY_SYN_)-1 + 1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_ACCOUNT_ID, sizeof(PARA_ACCOUNT_ID)-1) == 0)
				{
					char	szNum[256] = "";
					sprintf(szNum, "%u", pUser->GetAccountID());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_ACCOUNT_ID)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_USER_ID, sizeof(PARA_USER_ID)-1) == 0)
				{
					char	szNum[256] = "";
					sprintf(szNum, "%u", pUser->GetID());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_USER_ID)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_USER_MAPID, sizeof(PARA_USER_MAPID)-1) == 0)
				{
					char	szNum[256] = "";
					sprintf(szNum, "%u", pUser->GetMapID());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_USER_MAPID)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_USER_MAPX, sizeof(PARA_USER_MAPX)-1) == 0)
				{
					char	szNum[256] = "";
					sprintf(szNum, "%u", pUser->GetPosX());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_USER_MAPX)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_USER_MAPY, sizeof(PARA_USER_MAPY)-1) == 0)
				{
					char	szNum[256] = "";
					sprintf(szNum, "%u", pUser->GetPosY());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_USER_MAPY)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_USER_VIRTUE, sizeof(PARA_USER_VIRTUE)-1) == 0)
				{
					char	szNum[256] = "";
					sprintf(szNum, "%u", pUser->GetVirtue());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_USER_VIRTUE)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_USER_HOME, sizeof(PARA_USER_HOME)-1) == 0)
				{
					char	szNum[256] = "";
					sprintf(szNum, "%u", pUser->GetHomeID());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_USER_HOME)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_SYN_ID, sizeof(PARA_SYN_ID)-1) == 0)
				{
					char	szNum[256] = "";
					sprintf(szNum, "%u", pUser->GetMasterSynID());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_SYN_ID)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_SYN_NAME, sizeof(PARA_SYN_NAME)-1) == 0)
				{
					const char*	pName = SYNNAME_NONE;
					if(pUser->GetSynID() != ID_NONE)
					{
						CSynPtr pSyn = SynManager()->QuerySyndicate(pUser->GetSynID());
						if(pSyn)
							pName = pSyn->GetStr(SYNDATA_NAME);
					}
					strcpy(ptr2, pName);

					ptr += sizeof(PARA_SYN_NAME)-1;
					ptr2		+= strlen(pName);
				}
				else if(strnicmp(ptr, PARA_USER_NAME, sizeof(PARA_USER_NAME)-1) == 0)
				{
					strcpy(ptr2, pUser->GetName());

					ptr += sizeof(PARA_USER_NAME)-1;
					ptr2		+= strlen(pUser->GetName());
				}
				else if(strnicmp(ptr, PARA_MATE_NAME, sizeof(PARA_MATE_NAME)-1) == 0)
				{
					strcpy(ptr2, pUser->GetMate());

					ptr += sizeof(PARA_MATE_NAME)-1;
					ptr2		+= strlen(pUser->GetMate());
				}
				else if(strnicmp(ptr, PARA_MAP_OWNER_ID, sizeof(PARA_MAP_OWNER_ID)-1) == 0)
				{
					char	szNum[256] = "0";
					sprintf(szNum, "%u", pUser->GetMap()->GetOwnerID());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_MAP_OWNER_ID)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_MAP_OWNER_TYPE, sizeof(PARA_MAP_OWNER_TYPE)-1) == 0)
				{
					char	szNum[256] = "0";
					sprintf(szNum, "%u", pUser->GetMap()->GetOwnerType());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_MAP_OWNER_TYPE)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_ITER_VALUE, sizeof(PARA_ITER_VALUE)-1) == 0)
				{
					char	szNum[256] = "0";
					sprintf(szNum, "%u", pUser->TaskIterator());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_ITER_VALUE)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_ITER_SYN_NAME, sizeof(PARA_ITER_SYN_NAME)-1) == 0)
				{
					const char*	pName = "--";
					if(pUser->TaskIterator() != ID_NONE)
					{
						CSynPtr pSyn = SynManager()->QuerySyndicate((OBJID)pUser->TaskIterator());
						if(pSyn)
							pName = pSyn->GetStr(SYNDATA_NAME);
					}
					strcpy(ptr2, pName);

					ptr += sizeof(PARA_ITER_SYN_NAME)-1;
					ptr2		+= strlen(pName);
				}
				else if(strnicmp(ptr, PARA_ITER_SYN_LEADER, sizeof(PARA_ITER_SYN_LEADER)-1) == 0)
				{
					const char*	pName = "--";
					if(pUser->TaskIterator() != ID_NONE)
					{
						CSynPtr pSyn = SynManager()->QuerySyndicate((OBJID)pUser->TaskIterator());
						if(pSyn)
							pName = pSyn->GetStr(SYNDATA_LEADERNAME);
					}
					strcpy(ptr2, pName);

					ptr += sizeof(PARA_ITER_SYN_LEADER)-1;
					ptr2		+= strlen(pName);
				}
				else if(strnicmp(ptr, PARA_ITER_SYN_MONEY, sizeof(PARA_ITER_SYN_MONEY)-1) == 0)
				{
					char	szNum[256] = "--";
					if(pUser->TaskIterator() != ID_NONE)
					{
						CSynPtr pSyn = SynManager()->QuerySyndicate((OBJID)pUser->TaskIterator());
						if(pSyn)
							sprintf(szNum, "%d", pSyn->GetInt(SYNDATA_MONEY));
					}
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_ITER_SYN_MONEY)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_ITER_SYN_AMOUNT, sizeof(PARA_ITER_SYN_AMOUNT)-1) == 0)
				{
					char	szNum[256] = "--";
					if(pUser->TaskIterator() != ID_NONE)
					{
						CSynPtr pSyn = SynManager()->QuerySyndicate((OBJID)pUser->TaskIterator());
						if(pSyn)
							sprintf(szNum, "%u", pSyn->GetMemberAmount());
					}
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_ITER_SYN_AMOUNT)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_ITER_SYN_FEALTY, sizeof(PARA_ITER_SYN_FEALTY)-1) == 0)
				{
					char	szNum[256] = "--";
					if(pUser->TaskIterator() != ID_NONE)
					{
						CSynPtr pSyn = SynManager()->QuerySyndicate((OBJID)pUser->TaskIterator());
						if(pSyn)
							sprintf(szNum, "%u", pSyn->GetFealtyName());
					}
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_ITER_SYN_FEALTY)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_ITER_MEMBER_RANK, sizeof(PARA_ITER_MEMBER_RANK)-1) == 0)
				{
					const char*	pName = "--";
					if(pUser->TaskIterator() != ID_NONE)
					{
						CUser* pTheUser = UserManager()->GetUser((OBJID)pUser->TaskIterator());
						if(pTheUser && pTheUser->GetSynID())
							pName = pTheUser->QuerySynAttr()->GetRankTitle();
					}
					strcpy(ptr2, pName);

					ptr += sizeof(PARA_ITER_MEMBER_RANK)-1;
					ptr2		+= strlen(pName);
				}
				else if(strnicmp(ptr, PARA_ITER_MEMBER_NAME, sizeof(PARA_ITER_MEMBER_NAME)-1) == 0)
				{
					const char*	pName = "--";
					if(pUser->TaskIterator() != ID_NONE)
					{
						CUser* pTheUser = UserManager()->GetUser((OBJID)pUser->TaskIterator());
						if(pTheUser)
							pName = pTheUser->GetName();
					}
					strcpy(ptr2, pName);

					ptr += sizeof(PARA_ITER_MEMBER_NAME)-1;
					ptr2		+= strlen(pName);
				}
				else if(strnicmp(ptr, PARA_ITER_MEMBER_PROFFER, sizeof(PARA_ITER_MEMBER_PROFFER)-1) == 0)
				{
					char	szNum[256] = "--";
					if(pUser->TaskIterator() != ID_NONE)
					{
						CUser* pTheUser = UserManager()->GetUser((OBJID)pUser->TaskIterator());
						if(pTheUser && pTheUser->GetSynID() != ID_NONE)
							sprintf(szNum, "%d", pTheUser->QuerySynAttr()->GetInt(SYNATTRDATA_PROFFER));
					}
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_ITER_MEMBER_PROFFER)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_LAST_ADD_ITEM_ID, sizeof(PARA_LAST_ADD_ITEM_ID)-1) == 0)
				{
					char	szNum[256] = "--";
					CItem*	pLastItem = pUser->GetLastAddItem();
					sprintf(szNum, "%u", pLastItem ? pLastItem->GetID() : ID_NONE);
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_LAST_ADD_ITEM_ID)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_LAST_ADD_ITEM_TYPE, sizeof(PARA_LAST_ADD_ITEM_TYPE)-1) == 0)
				{
					char	szNum[256] = "--";
					CItem*	pLastItem = pUser->GetLastAddItem();
					sprintf(szNum, "%u", pLastItem ? pLastItem->GetInt(ITEMDATA_TYPE) : ID_NONE);
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_LAST_ADD_ITEM_TYPE)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_LAST_DEL_ITEM_ID, sizeof(PARA_LAST_DEL_ITEM_ID)-1) == 0)
				{
					char	szNum[256] = "--";
					sprintf(szNum, "%u", pUser->GetLastDelItemID());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_LAST_DEL_ITEM_ID)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_PK_BONUS_TARGET_NAME, sizeof(PARA_PK_BONUS_TARGET_NAME)-1) == 0)
				{
					strcpy(ptr2, m_infoPkBonus.szTargetName);

					ptr += sizeof(PARA_PK_BONUS_TARGET_NAME)-1;
					ptr2		+= strlen(m_infoPkBonus.szTargetName);
				}
				else if(strnicmp(ptr, PARA_PK_BONUS_HUNTER_NAME, sizeof(PARA_PK_BONUS_HUNTER_NAME)-1) == 0)
				{
					strcpy(ptr2, m_infoPkBonus.szHunterName);

					ptr += sizeof(PARA_PK_BONUS_HUNTER_NAME)-1;
					ptr2		+= strlen(m_infoPkBonus.szHunterName);
				}
				else if(strnicmp(ptr, PARA_PK_BONUS_BONUS, sizeof(PARA_PK_BONUS_BONUS)-1) == 0)
				{
					char	szNum[256] = "0";
					_snprintf(szNum, 256, "%u", m_infoPkBonus.dwBonus);
					szNum[255]	= 0;
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_PK_BONUS_BONUS)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_PK_BONUS_NUM, sizeof(PARA_PK_BONUS_NUM)-1) == 0)
				{
					char	szNum[256] = "0";
					_snprintf(szNum, 256, "%u", m_infoPkBonus.dwNum);
					szNum[255]	= 0;
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_PK_BONUS_NUM)-1;
					ptr2		+= strlen(szNum);
				}
			}
			if(pNpc)
			{
				if (strnicmp(ptr, PARA_DATASTR, sizeof(PARA_DATASTR)-1) == 0)
				{
					strcpy(ptr2, pNpc->GetStr(NPCDATA_DATASTR));

					ptr += sizeof(PARA_DATASTR)-1;
					ptr2		+= strlen(pNpc->GetStr(NPCDATA_DATASTR));
				}
				else if(strnicmp(ptr, PARA_DATA_, sizeof(PARA_DATA_)-1) == 0
							&& *(ptr+sizeof(PARA_DATA_)-1) >= '0' && *(ptr+sizeof(PARA_DATA_)-1) < '0'+ MAX_NPCDATA)
				{
					char	szNum[256] = "";
					int		idx = *(ptr+sizeof(PARA_DATA_)-1) - '0';
					if(idx >= 0 && idx < MAX_NPCDATA)
						sprintf(szNum, "%d", pNpc->GetData(idx));
					strcpy(ptr2, szNum);

					ptr	+= sizeof(PARA_DATA_)-1 + 1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_NAME, sizeof(PARA_NAME)-1) == 0)
				{
					strcpy(ptr2, pNpc->GetName());

					ptr += sizeof(PARA_NAME)-1;
					ptr2		+= strlen(pNpc->GetStr(NPCDATA_NAME));
				}
				else if(strnicmp(ptr, PARA_NPC_ID, sizeof(PARA_NPC_ID)-1) == 0)
				{
					char	szNum[256] = "0";
					sprintf(szNum, "%u", pNpc->GetRecordID());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_NPC_ID)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_NPC_X, sizeof(PARA_NPC_X)-1) == 0)
				{
					char	szNum[256] = "0";
					sprintf(szNum, "%u", pNpc->GetPosX());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_NPC_X)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_NPC_Y, sizeof(PARA_NPC_Y)-1) == 0)
				{
					char	szNum[256] = "0";
					sprintf(szNum, "%u", pNpc->GetPosY());
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_NPC_Y)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_NPC_OWNERID, sizeof(PARA_NPC_OWNERID)-1) == 0)
				{
					char	szNum[256] = "0";
					sprintf(szNum, "%u", pNpc->GetInt(NPCDATA_OWNERID));
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_NPC_OWNERID)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_NPC_LIFE, sizeof(PARA_NPC_LIFE)-1) == 0)
				{
					char	szNum[256] = "0";
					sprintf(szNum, "%u", pNpc->GetInt(NPCDATA_LIFE));
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_NPC_LIFE)-1;
					ptr2		+= strlen(szNum);
				}
			}
			if(pItem)
			{
				if(strnicmp(ptr, PARA_ITEM_TYPE, sizeof(PARA_ITEM_TYPE)-1) == 0)
				{
					char	szNum[256] = "";
					sprintf(szNum, "%u", pItem->GetInt(ITEMDATA_TYPE));
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_ITEM_TYPE)-1;
					ptr2		+= strlen(szNum);
				}
				else if(strnicmp(ptr, PARA_ITEM_DATA, sizeof(PARA_ITEM_DATA)-1) == 0)
				{
					char	szNum[256] = "";
					sprintf(szNum, "%u", pItem->GetInt(ITEMDATA_DATA));
					strcpy(ptr2, szNum);

					ptr += sizeof(PARA_ITEM_DATA)-1;
					ptr2		+= strlen(szNum);
				}
			}

			if(nWidth && ptr2 < pNext)
			{
				memset(ptr2, ' ', pNext - ptr2);
				ptr2	+= pNext - ptr2;
			}
		}
		else
		{
			*ptr2	= *ptr;

			ptr++;
			ptr2++;
		}
	}
	*ptr2	= 0;
}

bool CGameAction::ProcessAction(OBJID idAction, CUser* pUser, IRole* pRole, CItem* pItem, LPCTSTR pszAccept)
{
	if (idAction == ID_NONE)
		return false;

	m_pUser	= pUser;
	m_pRole	= pRole;
	m_pItem	= pItem;

	try
	{
		const int _MAX_ACTIONPRC	=32;
		DWORD dwActionCount	=0;
		while (idAction != ID_NONE)
		{
			DEADLOOP_CHECK(PID, "idAction: ")

			CActionData* pAction	= ActionSet()->GetObj(idAction);
			if (!pAction)
			{
				::LogSave("Error: game action %u not found.", idAction);
				break;
			}

			char	szParam[1024] = "";
			ReplaceAttrStr(szParam, pAction->GetStr(ACTIONDATA_PARAM), pszAccept, m_pUser, m_pRole, NULL);
			szParam[255]	= 0;

			if(m_pUser && m_pUser->IsPM())
			{
				char	szMsg[1024];
				sprintf(szMsg, "%d: [%d,%d]. type[%d], data[%d], param:[%s].",
						pAction->GetID(),
						pAction->GetInt(ACTIONDATA_IDNEXT),
						pAction->GetInt(ACTIONDATA_IDNEXTFAIL),
						pAction->GetInt(ACTIONDATA_TYPE),
						pAction->GetInt(ACTIONDATA_DATA),
						szParam);
				szMsg[255]	= 0;
				CMsgTalk msg;
				if (msg.Create(SYSTEM_NAME, m_pUser->GetName(), szMsg, NULL, 0x00ffff00, _TXTATR_NORMAL, 0))
					m_pUser->SendMsg(&msg);
			}

			bool	bRet = false;
			int nActionType = pAction->GetInt(ACTIONDATA_TYPE);
			if(nActionType > ACTION_SYS_FIRST && nActionType < ACTION_SYS_LIMIT)
				bRet	= ProcessActionSys	(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_NPC_FIRST && nActionType < ACTION_NPC_LIMIT)
				bRet	= ProcessActionNpc	(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_MAP_FIRST && nActionType < ACTION_MAP_LIMIT)
				bRet	= ProcessActionMap	(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_ITEMONLY_FIRST && nActionType < ACTION_ITEMONLY_LIMIT)
				bRet	= ProcessActionItemOnly	(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_ITEM_FIRST && nActionType < ACTION_ITEM_LIMIT)
				bRet	= ProcessActionItem	(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_NPCONLY_FIRST && nActionType < ACTION_NPCONLY_LIMIT)
				bRet	= ProcessActionNpcOnly	(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_SYN_FIRST && nActionType < ACTION_SYN_LIMIT)
				bRet	= ProcessActionSyn	(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_USER_FIRST && nActionType < ACTION_USER_LIMIT)
				bRet	= ProcessActionUser	(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_EVENT_FIRST && nActionType < ACTION_EVENT_LIMIT)
				bRet	= ProcessActionEvent	(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_TRAP_FIRST && nActionType < ACTION_TRAP_LIMIT)
				bRet	= ProcessActionEvent	(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_WANTED_FIRST && nActionType < ACTION_WANTED_LIMIT)
				bRet	= ProcessActionWanted	(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_MST_FIRST && nActionType < ACTION_MST_LIMIT)
				bRet	= ProcessActionMonster (pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else if(nActionType > ACTION_PKBONUS_FIRST && nActionType < ACTION_PKBONUS_LIMIT)
				bRet	= ProcessActionPkBonus(pAction, szParam, m_pUser, m_pRole, m_pItem, pszAccept);
			else
			{
				MYASSERT(!"Error action type!");
			}

			if (bRet)
				idAction	= pAction->GetInt(ACTIONDATA_IDNEXT);
			else
				idAction	= pAction->GetInt(ACTIONDATA_IDNEXTFAIL);

			if (dwActionCount++ >= _MAX_ACTIONPRC)
			{
				::LogSave("Error: too many game action, last action:%u.", idAction);
				break;
			}
		}
	}
	catch(...)
	{
		MYASSERT(!"catch");
		::LogSave("Error: exception catched in CGameAction::ProcessAction(%u)", idAction);
	}

	return true;
}

bool CGameAction::ProcessActionMonster(CActionData* pAction, LPCTSTR szParam, CUser* pUser, IRole* pRole, CItem* pItem, LPCTSTR pszAccept)
{
	IF_NOT (pAction)
		return false;

	CMonster* pMonster = NULL;
	IF_NOT (pRole->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
		return false;

	switch(pAction->GetInt(ACTIONDATA_TYPE))
	{
	case	ACTION_MST_DROPITEM:
		{

			char szCmd[MAX_PARAMSIZE] = "";
			if (1 != sscanf(szParam, "%s", szCmd))
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			if (0 == stricmp(szCmd, "dropitem"))
			{
				DWORD dwData = 0;
				if (2 == sscanf(szParam, "%s %u", szCmd, &dwData))
				{
					if (0 == stricmp(szCmd, "dropitem"))
					{
						OBJID idUser = ID_NONE;
						if (pUser)
							idUser = pUser->GetID();

						if (pMonster->DropItem(dwData, idUser))
							return true;
					}
				}
				else
				{
					LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
					break;
				}
			}
			else if (0 == stricmp(szCmd, "dropmoney"))
			{
				DWORD dwData=0, dwLowPercent=0;
				int rval = sscanf(szParam, "%s %u %u", szCmd, &dwData, &dwLowPercent);
				if (rval == 2)
					dwLowPercent = 100;
				else if (rval == 3)
					dwLowPercent %= 100;
				else
				{
					LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
					break;
				}

				DWORD dwMoneyDrop = dwData*(dwLowPercent+::RandGet(100-dwLowPercent))/100;
				if (dwMoneyDrop <= 0)
					return false;

				OBJID idUser = ID_NONE;
				if (pUser)
					idUser = pUser->GetID();

				if (pMonster->DropMoney(dwMoneyDrop, idUser))
					return true;
			}
		}
		break;
	default:
		LOGERROR("Bad Action Type", pAction->GetInt(ACTIONDATA_TYPE));
		break;
	}

	return false;
}

bool CGameAction::ProcessActionSys(CActionData* pAction, LPCTSTR szParam, CUser* pUser, IRole* pRole, CItem* pItem, LPCTSTR pszAccept)
{
	CHECKF(pAction);

	switch(pAction->GetInt(ACTIONDATA_TYPE))
	{
	case	ACTION_BROCASTMSG:
		{
			CMsgTalk msg;
			if (msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szParam, NULL, _COLOR_WHITE, pAction->GetInt(ACTIONDATA_DATA), 0))
				RoleManager()->BroadcastMsg(&msg, NULL);
		}
		return true;

	case	ACTION_SQL_EXECUTE:
		{
			return Database()->ExecuteAsyncSQL((char*)szParam);
		}
		break;

	case	ACTION_RAND:
		{
			int		nData1, nData2;
			if(sscanf(szParam, "%u %u", &nData1, &nData2) < 2)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			if(::RandGet(nData2) < nData1)
				return true;
			else
				return false;
		}
		break;

	case	ACTION_POSTCMD:
		{
			if (pUser)
			{
				CMsgAction msg;
				IF_OK (msg.Create( pUser->GetID(), pUser->GetPosX(), pUser->GetPosY(), pUser->GetDir(), actionPostCmd, pAction->GetInt(ACTIONDATA_DATA) ))
					pUser->SendMsg(&msg);

				return true;
			}
		}
		break;

	case	ACTION_CHKTIME:
		{
			if (CMapGroup::CheckTime(pAction->GetInt(ACTIONDATA_DATA), szParam))
				return true;
		}
		break;

	case	ACTION_MENUTEXT:
		{
			if(!pUser)
			{
				LOGERROR("Can't find user object when action[%d].", pAction->GetID());
				return false;
			}

			CMsgDialog	msg;
			CHECKF(msg.Create(MSGDIALOG_TEXT, szParam, INDEX_NONE, pAction->GetInt(ACTIONDATA_DATA)));
			pUser->SendMsg(&msg);
			return true;
		}
		break;
	case	ACTION_MENULINK:
		{
			CHECKF(pUser);

			PARAMBUF	szText;
			OBJID		idTask;
			int			nAlign = 0;
			if(sscanf(szParam, "%s %u %d", szText, &idTask, &nAlign) < 2)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			CMsgDialog	msg;
			CHECKF(msg.Create(MSGDIALOG_LINK, szText, pUser->PushTaskID(idTask), nAlign));
			pUser->SendMsg(&msg);
			return true;
		}
		break;
	case	ACTION_MENUEDIT:
		{
			CHECKF(pUser);

			int			nAcceptLen;
			OBJID		idTask;
			PARAMBUF	szText = "";
			int			nFlag = 0;
			if(sscanf(szParam, "%d %u %s %d", &nAcceptLen, &idTask, szText, &nFlag) < 2)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}
			if(nFlag)
				nAcceptLen	+= 256;

			CMsgDialog	msg;
			CHECKF(msg.Create(MSGDIALOG_EDIT, szText, pUser->PushTaskID(idTask), nAcceptLen));
			pUser->SendMsg(&msg);
			return true;
		}
		break;
	case	ACTION_MENUPIC:
		{
			CHECKF(pUser);

			int			x;
			int			y;
			OBJID		idPic;
			OBJID		idTask = ID_NONE;
			if(sscanf(szParam, "%d %d %u %u", &x, &y, &idPic, &idTask) < 3)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			CMsgDialog	msg;
			CHECKF(msg.Create(MSGDIALOG_PIC, x, y, idPic, pUser->PushTaskID(idTask)));
			pUser->SendMsg(&msg);
			return true;
		}
		break;
	case	ACTION_MENUCREATE:
		{
			CHECKF(pUser);

			OBJID		idTask = ID_NONE;
			sscanf(szParam, "%u", &idTask);

			CMsgDialog	msg;
			CHECKF(msg.Create(MSGDIALOG_CREATE, pUser->PushTaskID(idTask)));
			pUser->SendMsg(&msg);
			return true;
		}
		break;

	case ACTION_RANDACTION:
		{
			OBJID idAction[8];
			if (8 == sscanf(szParam, "%u %u %u %u %u %u %u %u",
				&idAction[0], &idAction[1], &idAction[2], &idAction[3],
				&idAction[4], &idAction[5], &idAction[6], &idAction[7]))
			{
				ProcessAction(idAction[::RandGet(8)], pUser, pRole, pItem, pszAccept);
				return true;
			}
			else
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
		}
		break;

	default:
		LOGERROR("Unsupported ACTION[%u] type [%u]", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE));
	}

	return false;
}

bool CGameAction::ProcessActionNpc(CActionData* pAction, LPCTSTR szParam, CUser* pUser, IRole* pRole, CItem* pItem, LPCTSTR pszAccept)
{
	CHECKF(pAction);

	switch(pAction->GetInt(ACTIONDATA_TYPE))
	{
	case	ACTION_NPC_ATTR:
		{
			PARAMBUF	szAttr;
			PARAMBUF	szOpt;
			PARAMBUF	szData;
			OBJID		idTargetNpc = ID_NONE;
			if(sscanf(szParam, "%s %s %s %u", szAttr, szOpt, szData, &idTargetNpc) < 3)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}
			int		nData = atoi(szData);
			bool	bUpdate = UPDATE_FALSE;

			CNpc* pNpc=NULL;
			if(idTargetNpc != ID_NONE)
			{
				pNpc = RoleManager()->QueryDynNpcByRecordID(idTargetNpc);
			}
			else
			{
				CHECKF(pRole);
				pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc));
			}
			IF_NOT(pNpc)
			{
				LOGERROR("Can't find npc_id[%u] in action[%u].", idTargetNpc, pAction->GetID());
				return false;
			}

			if(stricmp(szAttr, "ownerid") == 0)
			{
				if(strcmp(szOpt, "=") == 0)
				{
					pNpc->SetInt(NPCDATA_OWNERID, nData, bUpdate);
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					if(pNpc->GetInt(NPCDATA_OWNERID) == nData)
						return true;
				}
			}
			else if(stricmp(szAttr, "ownertype") == 0)
			{
				if(strcmp(szOpt, "=") == 0)
				{
					pNpc->SetInt(NPCDATA_OWNERTYPE, nData, bUpdate);
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					if(pNpc->GetInt(NPCDATA_OWNERTYPE) == nData)
						return true;
				}
			}
			else if(stricmp(szAttr, "lookface") == 0)
			{
				if(strcmp(szOpt, "=") == 0)
				{
					pNpc->SetLookFace(nData, true);
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					if(pNpc->GetInt(NPCDATA_LOOKFACE) == nData)
						return true;
				}
			}
			else if(stricmp(szAttr, "datastr") == 0)
			{
				if(strcmp(szOpt, "=") == 0)
				{
					pNpc->SetInt(NPCDATA_DATASTR, nData, bUpdate);
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					if(pNpc->GetInt(NPCDATA_DATASTR) == nData)
						return true;
				}
			}
			else if(strnicmp(szAttr, "data", 4) == 0)
			{
				if(stricmp(szData, "accept") == 0)
				{
					nData	= atol(pszAccept);
				}

				int idx = (*(szAttr+4) - '0');
				if(idx < 0 || idx >= MAX_NPCDATA)
					return false;
				if(strcmp(szOpt, "=") == 0)
				{
					pNpc->SetData(idx, nData, bUpdate);
					return true;
				}
				else if(strcmp(szOpt, "+=") == 0)
				{
					pNpc->SetData(idx, pNpc->GetData(idx) + nData, bUpdate);
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					if(pNpc->GetData(idx) == nData)
						return true;
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					if(pNpc->GetData(idx) < nData)
						return true;
				}
				else if(strcmp(szOpt, "<=") == 0)
				{
					if(pNpc->GetData(idx) <= nData)
						return true;
				}
				else if(strcmp(szOpt, ">") == 0)
				{
					if(pNpc->GetData(idx) > nData)
						return true;
				}
				else if(strcmp(szOpt, ">=") == 0)
				{
					if(pNpc->GetData(idx) >= nData)
						return true;
				}
			}
			else if(stricmp(szAttr, "life") == 0)
			{
				if(strcmp(szOpt, "=") == 0)
				{
					pNpc->SetInt(NPCDATA_LIFE, nData, bUpdate);

					CMsgUserAttrib	msg;
					IF_OK(msg.Create(pNpc->GetID(), _USERATTRIB_LIFE, nData))
						pNpc->BroadcastRoomMsg(&msg, false);

					return true;
				}
			}
			else if(stricmp(szAttr, "maxlife") == 0)
			{
				if(strcmp(szOpt, "=") == 0)
				{
					pNpc->SetInt(NPCDATA_MAXLIFE, nData, bUpdate);

					CMsgUserAttrib	msg;
					IF_OK(msg.Create(pNpc->GetID(), _USERATTRIB_MAXLIFE, nData))
						pNpc->BroadcastRoomMsg(&msg, false);

					return true;
				}
			}
			else
			{
				LOGERROR("Unsupported ACTION[%u] attribute [%s]", pAction->GetKey(), szAttr);
			}

			return false;
		}
		break;
	case	ACTION_NPC_ERASE:
		{
			CHECKF(pAction && pRole);

			CNpc* pNpc;
			IF_NOT(pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
				return false;

			if(pNpc->IsDeleted())
				return false;

			int	nType = pAction->GetInt(ACTIONDATA_DATA);
			if(nType == 0)
			{
				m_pRole	= NULL;
				return pNpc->DelNpc();
			}
			else
			{
				for(IRoleSet::Iter i = RoleManager()->QuerySet()->Begin(); i != RoleManager()->QuerySet()->End(); )
				{
					IRole* pRole = RoleManager()->QuerySet()->GetObjByIter(i);
					i++;

					CNpc* pNpc;
					if(pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->GetType() == nType)
					{
						if(!pNpc->DelNpc())
							return false;
					}
				}
				return true;
			}
		}
		break;
	case	ACTION_NPC_RESETSYNOWNER:
		{
			CHECKF(pAction && pRole);

			CNpc* pNpc;
			IF_NOT(pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
				return false;

			CHECKF(pNpc->IsSynFlag());
			int		nMaxScore	= 0;
			CSynPtr pMaxSyn		= NULL;
			for(int i = 0; i < SynManager()->QuerySynSet()->GetAmount(); i++)
			{
				CSyndicate* pSyn = SynManager()->QuerySynSet()->GetObjByIndex(i);
				if(pSyn)
				{
					int nScore = pSyn->GetSynWarScore(pNpc->GetID());
					pSyn->ClearSynWarScore(pNpc->GetID());
					if(nMaxScore < nScore)
					{
						nMaxScore	= nScore;
						pMaxSyn		= pSyn;
					}
				}
			}

			if(!pMaxSyn || pMaxSyn->GetID() == ID_NONE)
				return false;

			if(pNpc->GetSynID() != ID_NONE)
			{
				CSyndicate* pOldSyn = SynManager()->QuerySyndicate(pNpc->GetSynID());
				if(pOldSyn && pOldSyn->GetInt(SYNDATA_MONEY) > 0)
				{
					int nMoney = MulDiv(pOldSyn->GetInt(SYNDATA_MONEY), SYNWAR_GAINMONEY_PERCENT, 100);
					if(nMoney > 0)
					{
						pOldSyn->QueryModify()->AddData(SYNDATA_MONEY, -1*nMoney, true);
						pMaxSyn->QueryModify()->AddData(SYNDATA_MONEY, nMoney, true);
					}
				}
			}

			pNpc->SetSynOwnerID(pMaxSyn->GetID(), false);
			pNpc->GetMap()->SetSynID(pMaxSyn->GetID(), true);
			pNpc->SendSelfToBlock();

			MSGBUF	szMsg;
			sprintf(szMsg, STR_GOT_WIN_s, pMaxSyn->GetStr(SYNDATA_NAME));

			CMsgTalk msg;
			IF_OK (msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szMsg, NULL, _COLOR_WHITE, _TXTATR_GM))
				UserManager()->BroadcastMsg(&msg, NULL);

			CRoleSet::Iterator pRole = RoleManager()->QuerySet()->NewEnum();
			while(pRole.Next())
			{
				if(pRole && pRole->GetMap()->GetID() == pNpc->GetMapID())
					pRole->ClrAttackTarget();
			}

			CGameMap* pMap = pNpc->GetMap();
			if(pMap->GetID() == WHITE_SYN_MAP_ID)
			{
				CMsgSyndicate	msg;
				IF_OK(msg.Create(SET_WHITE_SYN, pMap->GetOwnerID()))
					UserManager()->BroadcastMsg(&msg, NULL);
			}
			else if(pMap->GetID() == BLACK_SYN_MAP_ID)
			{
				CMsgSyndicate	msg;
				IF_OK(msg.Create(SET_BLACK_SYN, pMap->GetOwnerID()))
					UserManager()->BroadcastMsg(&msg, NULL);
			}
			return true;
		}
		break;
	case	ACTION_NPC_SYNCHRO:
		{
			DWORD	dwData1, dwData2;
			int		nData3, nData4;
			if(sscanf(szParam, "%u %u %d %d", &dwData1, &dwData2, &nData3, &nData4) < 4)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			CNpc* pNpc;
			IF_NOT(pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
				return false;
			CHECKF(pUser);
			MYASSERT(pNpc->GetInt(NPCDATA_OWNERID) == 0);
			MapGroup(PID)->QueryIntraMsg()->SynchroNpc(pNpc->GetID(), pUser->GetSynID(), dwData1, dwData2, nData3, nData4, true);
		}
		break;
	default:
		LOGERROR("Unsupported ACTION[%u] type [%u]", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE));
	}

	return false;
}

bool CGameAction::ProcessActionMap(CActionData* pAction, LPCTSTR szParam, CUser* pUser, IRole* pRole, CItem* pItem_no_use, LPCTSTR pszAccept)
{
	CHECKF(pAction);

	switch(pAction->GetInt(ACTIONDATA_TYPE))
	{
	case ACTION_MAP_FIREWORKS:
		{
			IF_OK (pUser)
			{
				CMsgItem msg;
				IF_OK (msg.Create(pUser->GetID(), ITEMACT_FIREWORKS))
					pUser->BroadcastRoomMsg(&msg, true);

				return true;
			}
		}
		break;

	case	ACTION_MAP_FIREWORKS2:
		{
			IF_OK (pUser)
			{
				char	szMsg[8 + 1] = "";
				strncpy(szMsg, szParam, 8);

				CMsgName	msg;
				IF_OK (msg.Create(NAMEACT_FIREWORKS, szMsg))
					pUser->BroadcastRoomMsg(&msg, true);

				return true;
			}
		}
		break;

	case ACTION_MAP_DROPITEM:
		{
			OBJID idItemType = ID_NONE, idMap = ID_NONE;
			POINT pos;
			if (4 == sscanf(szParam, "%u %u %d %d", &idItemType, &idMap, &pos.x, &pos.y))
			{
				CGameMap* pMap = MapManager()->QueryMap(idMap);
				if (pMap && pMap->IsLayItemEnable(pos.x, pos.y))
				{
					CMapItem* pMapItem = CMapItem::CreateNew();
					if (pMapItem)
					{
						if(pMapItem->Create(MapManager()->SpawnMapItemID(), pMap, pos, idItemType, ID_NONE))
						{
							MapManager()->QueryMapItemSet()->AddObj(pMapItem);
							return true;
						}
						else
							pMapItem->ReleaseByOwner();
					}
				}
			}
			else
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
		}
		break;

	case	ACTION_MAP_MAPEFFECT:
		{
			OBJID idMap = ID_NONE;
			int nPosX = 0, nPosY = 0;
			char szEffect[256] = "";

			if (4 == sscanf(szParam, "%u %d %d %s", &idMap, &nPosX, &nPosY, szEffect))
			{
				CGameMap* pMap = MapManager()->QueryMap(idMap);
				if (pMap)
				{
					CMsgName msg;
					IF_OK (msg.Create(NAMEACT_MAPEFFECT, szEffect, nPosX, nPosY))
						pMap->BroadcastBlockMsg(nPosX, nPosY, &msg);

					return true;
				}
			}
			else
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
		}
		break;

	case	ACTION_MAP_MOVENPC:
		{
			CNpc* pNpc;
			IRole* pRole = RoleManager()->QuerySet()->GetObj(pAction->GetInt(ACTIONDATA_DATA));
			if (pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
			{
				IF_NOT(!pNpc->IsDynaNpc())
				{
					LOGERROR("ACTION_MAP_MOVENPC is for fixed NPCs only [%d]", pNpc->GetID());
					return false;
				}

				OBJID idMap = ID_NONE;
				int nPosX = 0, nPosY = 0;

				if (3 == sscanf(szParam, "%u %d %d", &idMap, &nPosX, &nPosY))
				{
					if (pNpc->ChangePos(idMap, nPosX, nPosY))
						return true;
				}
				else
					LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
			}
		}
		break;

	case	ACTION_MAP_MAPUSER:
		{
			char szCmd[256] = "", szOpt[256] = "";
			int nData = 0;
			if (3 == sscanf(szParam, "%s %s %d", szCmd, szOpt, &nData))
			{
				OBJID idMap = pAction->GetInt(ACTIONDATA_DATA);
				vector<CUser*> setUser;

				if (0 == stricmp("map_user", szCmd))
				{
					CUserManager::Iterator pUser = UserManager()->NewEnum();
					while(pUser.Next())
					{
						if (pUser && pUser->GetMapID() == idMap)
							setUser.push_back(pUser);
					}
				}
				else if (0 == stricmp("alive_user", szCmd))
				{
					CUserManager::Iterator pUser = UserManager()->NewEnum();
					while(pUser.Next())
					{
						if (pUser && pUser->GetMapID() == idMap && pUser->IsAlive())
							setUser.push_back(pUser);
					}
				}
				else
				{
					LOGERROR("ACTION %u: bad param", pAction->GetID());
					return false;
				}

				if (0 == stricmp(szOpt, "=="))
				{
					if ((int)setUser.size() == nData)
						return true;
				}
				else if (0 == stricmp(szOpt, "<="))
				{
					if ((int)setUser.size() <= nData)
						return true;
				}
				else if (0 == stricmp(szOpt, ">="))
				{
					if ((int)setUser.size() >= nData)
						return true;
				}
			}
			else
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
		}
		break;

	case	ACTION_MAP_BROCASTMSG:
		{
			CMsgTalk msg;
			IF_OK (msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szParam, NULL, 0x0ffffff, _TXTATR_SYSTEM))
				UserManager()->BroadcastMapMsg(pAction->GetInt(ACTIONDATA_DATA), &msg);

			return true;
		}
		break;

	case	ACTION_MAP_ATTRIB:
		{
			PARAMBUF	szField;
			PARAMBUF	szOpt;
			int			nData;
			OBJID		idMap = ID_NONE;
			if (sscanf(szParam, "%s %s %d %u", szField, szOpt, &nData, &idMap) < 3)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				return false;
			}

			CGameMap*	pTaskMap = NULL;
			if(idMap != ID_NONE)
			{
				pTaskMap = MapManager()->QueryMap(idMap);
			}
			else
			{
				CNpc* pNpc = NULL;
				if(pUser)
					pTaskMap = pUser->GetMap();
				else if(pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
					pTaskMap = pNpc->GetMap();
			}
			CHECKF(pTaskMap);

			if(stricmp(szField, "synid") == 0)
			{
				if(strcmp(szOpt, "==") == 0)
				{
					if(pTaskMap->GetSynID() == nData)
						return true;
				}
				else if(strcmp(szOpt, "=") == 0)
				{
					pTaskMap->SetSynID(nData, true);
					return true;
				}
			}
			else if(stricmp(szField, "status") == 0)
			{
				if(strcmp(szOpt, "test") == 0)
				{
					if((pTaskMap->GetStatus() & nData) != 0)
						return true;
				}
				else if(strcmp(szOpt, "set") == 0)
				{
					pTaskMap->SetStatus(nData, true);
					return true;
				}
				else if(strcmp(szOpt, "reset") == 0)
				{
					pTaskMap->SetStatus(nData, false);
					return true;
				}
			}
			else if(stricmp(szField, "type") == 0)
			{
				if(strcmp(szOpt, "test") == 0)
				{
					if((pTaskMap->GetType() & nData) != 0)
						return true;
				}
			}
			else if(stricmp(szField, "res_lev") == 0)
			{
				if(strcmp(szOpt, "==") == 0)
				{
					if(pTaskMap->GetResLev() == nData)
						return true;
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					if(pTaskMap->GetResLev() < nData)
						return true;
				}
				else if(strcmp(szOpt, "=") == 0)
				{
					pTaskMap->SetResLev(nData, true);
					return true;
				}
			}
			else if(stricmp(szField, "mapdoc") == 0)
			{
				if(strcmp(szOpt, "==") == 0)
				{
					if(pTaskMap->GetDocID() == nData)
						return true;
				}
				else if(strcmp(szOpt, "=") == 0)
				{
					pTaskMap->ChangeMapDoc(nData);
					return true;
				}
			}
			else if(stricmp(szField, "portal0_x") == 0)
			{
				if(strcmp(szOpt, "=") == 0)
				{
					pTaskMap->SetPortal0X(nData, UPDATE_TRUE);
					return true;
				}
			}
			else if(stricmp(szField, "portal0_y") == 0)
			{
				if(strcmp(szOpt, "=") == 0)
				{
					pTaskMap->SetPortal0Y(nData, UPDATE_TRUE);
					return true;
				}
			}
		}
		break;
	default:
		LOGERROR("Unsupported ACTION[%u] type [%u]", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE));
	}

	return false;
}

bool CGameAction::ProcessActionItemOnly(CActionData* pAction, LPCTSTR szParam, CUser* pUser, IRole* pRole, CItem* pItem, LPCTSTR pszAccept)
{
	CHECKF(pAction);
	if(!pUser)
	{
		LOGERROR("CGameAction::ProcessActionItemOnly() ActionID=%u, ActionType=%u at %s, %d", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE),
				__FILE__, __LINE__);
		return false;
	}

	switch(pAction->GetInt(ACTIONDATA_TYPE))
	{
	case	ACTION_ITEM_REQUESTLAYNPC:
		{
			OBJID	idNextTask;
			int		nType, nSort, nLookFace, nRegion=0;
			if(sscanf(szParam, "%u %d %d %d %d %d", &idNextTask, &nType, &nSort, &nLookFace, &nRegion) < 4)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			CHECKF(idNextTask != ID_NONE);
			pUser->SetTaskItemActionID(idNextTask);

			CMsgNpc		msg;
			IF_OK(msg.Create(EVENT_LAYNPC, nRegion, nType, nLookFace))
				pUser->SendMsg(&msg);
			return true;
		}
		break;
	case	ACTION_ITEM_COUNTNPC:
		{
			PARAMBUF	szField;
			PARAMBUF	szData;
			PARAMBUF	szOpt;
			int			nNum;
			int ret = sscanf(szParam, "%s %s %s %d", szField, szData, szOpt, &nNum);
			IF_NOT_(ret == 4)
				return false;

			int nCount = 0;
			if(stricmp(szField, "all") == 0)
			{
				nCount = RoleManager()->CountAllNpc(pUser->GetMapID());
			}
			else if(stricmp(szField, "furniture") == 0)
			{
				nCount = RoleManager()->CountFurnitureNpc(pUser->GetMapID());
			}
			else if(stricmp(szField, "name") == 0)
			{
				nCount = RoleManager()->CountNpcByName(pUser->GetMapID(), szData);
			}
			else if(stricmp(szField, "type") == 0)
			{
				int nType = atol(szData);
				nCount = RoleManager()->CountNpcByType(pUser->GetMapID(), nType);
			}
			else
				return false;

			if(stricmp(szOpt, "<") == 0 && nCount < nNum)
				return true;
			else if(stricmp(szOpt, "==") == 0 && nCount == nNum)
				return true;
			else
				return false;

			return false;
		}
		break;
	case	ACTION_ITEM_LAYNPC:
		{
			CHECKF(pszAccept && strlen(pszAccept) > 0);
			if(!pItem)
			{
				LogSave("ERROR: CGameAction::ProcessActionItemOnly() ActionID=%u at %s, %d", pAction->GetInt(ACTIONDATA_ID_), __FILE__, __LINE__);
				return false;
			}

			NPCINFO_ST	info;
			memset(&info, 0, sizeof(NPCINFO_ST));

			int nPose=0, nFrame=0, nDir = 0;

			int ret = sscanf(pszAccept, "%d %d %d %d %d", &info.nPosX, &info.nPosY, &nDir, &nFrame, &nPose);
			IF_NOT_(ret >= 3)
				return false;

			void* pTemp;
			if(pUser->GetMap()->QueryObjInPos(info.nPosX, info.nPosY, OBJ_NPC, IPP_OF(pTemp)))
				return false;

			NAMESTR	szName;
			const char* pName = szName;
			int		nRegionType = 0;
			ASSERT(_MAX_NPCTASK == 8);
			ASSERT(MAX_NPCDATA == 4);
			ret = sscanf(szParam, "%s %d %d %d %d ""%d %d %d %d ""%d %d %d %d %d %d %d %d ""%d %d %d %d %s",
							szName, &info.nType, &info.nSort, &info.nLookFace, &info.nOwnerType,
							&info.nLife, &nRegionType, &info.idBase, &info.idLink,
							&info.setTask[0], &info.setTask[1], &info.setTask[2], &info.setTask[3],
							&info.setTask[4], &info.setTask[5], &info.setTask[6], &info.setTask[7],
							&info.setData[0], &info.setData[1], &info.setData[2], &info.setData[3], info.szData);
			IF_NOT_(ret >= 5)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}
			if(info.nType == _ROLE_FURNITURE_NPC || info.nType == _ROLE_3DFURNITURE_NPC || info.nType == _TRUNCK_NPC)
			{
				CGameMap* pGameMap = pUser->GetMap();

				MYASSERT(pGameMap);
				if(!pGameMap->IsFamilyMap())
				{
					LOGERROR("[%s] tried to place furniture [%s] outside a home map.", pUser->GetName(), szName);
					return false;
				}
				if(pGameMap->GetOwnerID() != pUser->GetID())
				{
					LOGERROR("[%s] tried to place furniture [%s] in another player's home.", pUser->GetName(), szName);
					return false;
				}
			}

			if(info.nType == _STATUARY_NPC)
			{
				pName				= pUser->GetName();
				info.nLookFace		= ::MaskDir(nDir);
				info.setTask[0]		= pUser->GetMountTypeID();
				info.setTask[1]		= pUser->GetArmorTypeID();
				info.setTask[2]		= pUser->GetWeaponRTypeID();
				info.setTask[3]		= pUser->GetWeaponLTypeID();
				info.setTask[4]		= nFrame;
				info.setTask[5]		= nPose;
				info.setTask[6]		= pUser->GetLookFace();
				info.setTask[7]		= (pUser->GetSynRankShow()<<16) + pUser->GetHair();
			}
			else
			{
				info.nLookFace	= ::MakeLookFace(::MaskLook(info.nLookFace), ::MaskDir(nDir));
			}

			if(pUser->GetMap()->IsTeleportDisable() && info.nType == _SYNTRANS_NPC)
			{
				pUser->SendSysMsg(STR_DISABLE_LAY_SYNTRANS);
				return false;
			}

			if(nRegionType && !pUser->GetMap()->IsInRegionType(nRegionType, info.nPosX, info.nPosY))
				return false;

			info.idMap	= pUser->GetMapID();
			switch(info.nOwnerType)
			{
			case	OWNER_NONE:
				{
					info.idOwner	= ID_NONE;
				}
				break;
			case	OWNER_USER:
				{
					info.idOwner	= pUser->GetID();
				}
				break;
			case	OWNER_SYN:
				{
					if(pUser->GetSynID() == ID_NONE)
						return false;
					CSyndicate* pSyn = SynManager()->QuerySyndicate(pUser->GetSynID());
					CHECKF(pSyn);
					info.idOwner	= pSyn->GetMasterSynID();
				}
				break;
			case	OWNER_NPC:
				{
					MYASSERT(!"OWNER_NPC");
				}
				break;
			}

			CNpc* pNpc = CNpc::CreateNew();
			IF_OK(pNpc)
			{
				IF_OK(pNpc->Create(m_idProcess, &info, pItem->GetInt(ITEMDATA_TYPE), pName))
				{
					RoleManager()->QuerySet()->AddObj(pNpc->QueryRole());

					pUser->SetTaskNpcID(pNpc->GetID());
					m_pRole		= pNpc->QueryRole();
					return true;
				}
				else
					pNpc->ReleaseByOwner();
			}

			return false;
		}
		break;
	case	ACTION_ITEM_DELTHIS:
		{
			if(!pItem)
			{
				LogSave("ERROR: CGameAction::ProcessActionItemOnly() ActionID=%u at %s, %d", pAction->GetInt(ACTIONDATA_ID_), __FILE__, __LINE__);
				return false;
			}
			if(!pUser->EraseItem(pItem->GetID(), SYNCHRO_TRUE))
				return false;
			pUser->SetTaskItemID(ID_NONE);
			pUser->SendSysMsg(STR_USE_ITEM);

			m_pItem	= NULL;

			return true;
		}
		break;
	default:
		LOGERROR("Unsupported ACTION[%u] type [%u]", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE));
	}

	return false;
}

bool CGameAction::ProcessActionItem(CActionData* pAction, LPCTSTR szParam, CUser* pUser, IRole* pRole, CItem* pItem, LPCTSTR pszAccept)
{
	CHECKF(pAction);
	if(!pUser)
	{
		LOGERROR("CGameAction::ProcessActionItem() ActionID=%u, ActionType=%u at %s, %d", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE),
				__FILE__, __LINE__);
		return false;
	}

	switch(pAction->GetInt(ACTIONDATA_TYPE))
	{
	case	ACTION_ITEM_EQUIPFIND:
		{
			int		nPos = 0, nData = 0, nVarIdx = 0;
			OBJID	idxField = 0;
			char	szOpt[256] = "";
			if(5 != sscanf(szParam, "%d %u %s %d to %d", &nPos, &idxField, szOpt, &nData, &nVarIdx))
			{
				LOGERROR("ACTION %u: bad parameter", pAction->GetID());
				break;
			}

			int nOpt = 0;
			if(strcmp(">", szOpt) == 0)
				nOpt = 1;
			else if(strcmp(">=", szOpt) == 0)
				nOpt = 2;
			else if(strcmp("<", szOpt) == 0)
				nOpt = 3;
			else if(strcmp("<=", szOpt) == 0)
				nOpt = 4;
			nVarIdx = ::CutOverflow(::CutTrail(nVarIdx, 0), 8);

			int nEquipPos = pUser->FindEquipPos(nPos, idxField, nOpt, nData);
			if(nEquipPos == -1)
				return false;
			pUser->SetVarData(nVarIdx, nEquipPos);
			return true;
		}
		break;
	case	ACTION_ITEM_EQUIPATTR:
		{
			OBJID	idPos = 0, idxField = 0;
			int		nData = 0, nSynchro = 0;
			char	szOpt[256] = "";
			if(sscanf(szParam, "%u %u %s %d %d", &idPos, &idxField, szOpt, &nData, &nSynchro) < 4)
			{
				LOGERROR("ACTION %u: bad parameter", pAction->GetID());
				break;
			}
			CItem* pEquip = pUser->GetEquipItemByPos(idPos);
			if(!pEquip)
			{
				LOGERROR("ACTION %u: bad parameter", pAction->GetID());
				break;
			}

			pUser->GetEquipBattlePower(pEquip);
			if(stricmp("set", szOpt) == 0)
			{
				pEquip->SetInt((ITEMDATA)idxField, nData, true);
				pUser->UpdateBattlePower();
				if(nSynchro)
				{
					CMsgItemInfo	msg;
					if(msg.Create(pEquip, ITEMINFO_UPDATE))
						pUser->SendMsg(&msg);
				}
				return true;
			}
			else if(stricmp("+=", szOpt) == 0)
			{
				pEquip->SetInt((ITEMDATA)idxField, pEquip->GetInt((ITEMDATA)idxField) + nData, true);
				if(nSynchro)
				{
					CMsgItemInfo	msg;
					if(msg.Create(pEquip, ITEMINFO_UPDATE))
						pUser->SendMsg(&msg);
				}
				return true;
			}
			else if(stricmp("==", szOpt) == 0)
			{
				return pEquip->GetInt((ITEMDATA)idxField) == nData;
			}
			else if(stricmp(">=", szOpt) == 0)
			{
				return pEquip->GetInt((ITEMDATA)idxField) >= nData;
			}

			LOGERROR("ACTION %u: bad parameter", pAction->GetID());
		}
		break;

	case	ACTION_ITEM_EQUIPCOLOR:
		{
			int nData = 0, nEquipType = 0;

			int ret = sscanf(szParam, "%d %d", &nEquipType, &nData);
			IF_OK_ (2 == ret)
			{
				CItem* pItem = pUser->GetEquipItemByPos(nEquipType);
				if (!pItem)
					return false;
				if (pItem->ChangeColor(nData))
				{
					CMsgItemInfoEx msg;
					IF_OK(msg.Create(pItem, pUser->GetID(), 0, ITEMINFOEX_EQUIPMENT))
						pUser->BroadcastRoomMsg(&msg, true);
				}
			}
		}
		break;

	case	ACTION_ITEM_EQUIPEXIST:
		{
			CItem* pEquip = pUser->GetEquipItemByPos(pAction->GetInt(ACTIONDATA_DATA));
			if (pEquip)
				return true;
		}
		break;

	case	ACTION_ITEM_EQUIPTEST:
		{
			char szCmd[256] = "";
			char szOpt[256] = "";
			int nData = 0, nEquipType = 0;

			int ret = sscanf(szParam, "%d %s %s %d", &nEquipType, szCmd, szOpt, &nData);
			IF_OK_ (4 == ret)
			{
				CItem* pItem = pUser->GetEquipItemByPos(nEquipType);
				if (pItem)
				{
					int nTestData = 0;
					if (0 == stricmp("level", szCmd))
					{
						nTestData = pItem->GetLevel();
					}
					else if (0 == stricmp("quality", szCmd))
					{
						nTestData = pItem->GetQuality();
					}
					else if (0 == stricmp("durability", szCmd))
					{
						if (-1 == nData)
							nData = pItem->GetInt(ITEMDATA_AMOUNTLIMIT)/100;

						nTestData = pItem->GetInt(ITEMDATA_AMOUNT)/100;
					}
					else if (0 == stricmp("max_dur", szCmd))
					{
						if (-1 == nData)
							nData = pItem->GetInt(ITEMDATA_AMOUNTLIMIT_ORIGINAL)/100;

						nTestData = (pItem->GetInt(ITEMDATA_AMOUNTLIMIT)*100/(100+pItem->GetGemDurEffect(0)))/100;
					}
					else
					{
						LOGERROR("ACTION %u: bad parameter", pAction->GetID());
						return false;
					}

					if (0 == stricmp(szOpt, "=="))
					{
						return (nTestData == nData);
					}
					else if (0 == stricmp(szOpt, "<="))
					{
						return (nTestData <= nData);
					}
					else if (0 == stricmp(szOpt, ">="))
					{
						return (nTestData >= nData);
					}
					else
					{
						LOGERROR("ACTION %u: bad parameter", pAction->GetID());
					}
				}
			}
			else
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
		}
		break;

	case	ACTION_ITEM_UPEQUIPMENT:
		{
			char szCmd[256] = "";
			int nPos = 0;
			int nFlag = 0;

			if (sscanf(szParam, "%s %d %d", szCmd, &nPos, &nFlag) >= 2)
			{
				if (0 == stricmp("up_lev", szCmd))
				{
					if (pUser->UpEquipmentLevel(nPos, nFlag != 0))
						return true;
				}
				else if (0 == stricmp("down_lev", szCmd))
				{
					if (pUser->GetEquipItemByPos(nPos) && pUser->DownEquipmentLevel(nPos, true))
						return true;
				}
				else if (0 == stricmp("up_quality", szCmd))
				{
					if (pUser->UpEquipmentQuality(nPos, nFlag != 0))
						return true;
				}
				else if (0 == stricmp("recover_dur", szCmd))
				{
					if (pUser->RecoverEquipmentDur(nPos))
						return true;
				}
				else if (0 == stricmp("up_levultra", szCmd))
				{
					if (pUser->UpEquipmentLevelUltra(nPos))
						return true;
				}
				else if (0 == stricmp("up_levultra2", szCmd))
				{
					if (pUser->UpEquipmentLevelUltra2(nPos))
						return true;
				}
				else
					LOGERROR("ACTION %u: bad parameter", pAction->GetID());
			}
			else
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
		}
		break;

	case	ACTION_ITEM_MULTIDEL:
		{
			OBJID idType0 = ID_NONE, idType1 = ID_NONE;
			int nNum = 0;
			int ret = sscanf(szParam, "%u %u %d", &idType0, &idType1, &nNum);
			IF_OK_ (3 == ret)
			{
				return pUser->MultiDelItem(idType0, idType1, nNum);
			}
		}
		break;

	case ACTION_ITEM_MULTICHK:
		{
			OBJID idType0 = ID_NONE, idType1 = ID_NONE;
			int nNum = 0;
			int ret = sscanf(szParam, "%u %u %d", &idType0, &idType1, &nNum);
			IF_OK_ (3 == ret)
			{
				return pUser->MultiCheckItem(idType0, idType1, nNum);
			}
		}
		break;

	case	ACTION_ITEM_HOLE:
		{
			char szCmd[256] = "";
			int nData = 0;
			if (2 == sscanf(szParam, "%s %u", szCmd, &nData))
			{
				if (0 == stricmp(szCmd, "ChkHole"))
				{
					CItem* pItem = pUser->GetWeaponR();
					if (pItem)
					{
						if (1 == nData && GEM_NONE != pItem->GetInt(ITEMDATA_GEM1))
							return true;
						else if (2 == nData && GEM_NONE != pItem->GetInt(ITEMDATA_GEM2))
							return true;
					}
				}
				else if (0 == stricmp(szCmd, "MakeHole"))
				{
					CItem* pItem = pUser->GetWeaponR();
					if (pItem)
					{
						pUser->GetEquipBattlePower(pItem);
						if (1 == nData && GEM_NONE == pItem->GetInt(ITEMDATA_GEM1))
						{
							pItem->SetInt(ITEMDATA_GEM1, GEM_HOLE, true);
							pUser->UpdateBattlePower();

							CMsgItemInfo msg;
							IF_OK (msg.Create(pItem, ITEMINFO_UPDATE))
								pUser->SendMsg(&msg);

							return true;
						}

						if (2 == nData
							&& GEM_NONE != pItem->GetInt(ITEMDATA_GEM1)
							&& GEM_NONE == pItem->GetInt(ITEMDATA_GEM2))
						{
							pItem->SetInt(ITEMDATA_GEM2, GEM_HOLE, true);
							pUser->UpdateBattlePower();

							CMsgItemInfo msg;
							IF_OK (msg.Create(pItem, ITEMINFO_UPDATE))
								pUser->SendMsg(&msg);

							return true;
						}
					}
				}
			}
		}
		break;

	case	ACTION_ITEM_ADD:
		{
			CItem* pItem = pUser->AwardItem(pAction->GetInt(ACTIONDATA_DATA), false);
			if(pItem)
			{
				int	nAmount, nAmountLimit, nIdent, nGem1, nGem2, nMagic1, nMagic2, nMagic3, nReduceDmg, nAddLife, nAntiMonster, nColor;
				nAmount = nAmountLimit = nIdent = nGem1 = nGem2 = nMagic1 = nMagic2 = nMagic3 = nReduceDmg = nAddLife = nAntiMonster = 0;
				nColor = 3;
				if(sscanf(szParam, "%u %u %u %u %u %u %u %u %u %u %u %u",
					&nAmount, &nAmountLimit, &nIdent, &nGem1, &nGem2, &nMagic1, &nMagic2, &nMagic3, &nReduceDmg, &nAddLife, &nAntiMonster, &nColor) > 0)
				{
					if(nAmount)
						pItem->SetInt(ITEMDATA_AMOUNT, nAmount);
					if(nAmountLimit)
						pItem->SetInt(ITEMDATA_AMOUNTLIMIT, nAmountLimit);
					if(nGem1)
						pItem->SetInt(ITEMDATA_GEM1, nGem1);
					if(nGem2)
						pItem->SetInt(ITEMDATA_GEM2, nGem2);
					if(nMagic1)
						pItem->SetInt(ITEMDATA_MAGIC1, nMagic1);
					if(nMagic2)
						pItem->SetInt(ITEMDATA_MAGIC2, nMagic2);
					if(nMagic3)
						pItem->SetInt(ITEMDATA_MAGIC3, nMagic3);
					if(nReduceDmg)
						pItem->SetInt(ITEMDATA_REDUCE_DMG, nReduceDmg);
					if(nAddLife)
						pItem->SetInt(ITEMDATA_ADD_LIFE, nAddLife);
					if(nAntiMonster)
						pItem->SetInt(ITEMDATA_ANTI_MONSTER, nAntiMonster);
					pItem->SetInt(ITEMDATA_COLOR, nColor);

					pItem->SetIdent(nIdent != 0, UPDATE_TRUE);
				}

				CMsgItemInfo msg;
				if(msg.Create(pItem, ITEMINFO_ADDITEM))
					pUser->SendMsg(&msg);
				return true;
			}
		}
		break;
	case	ACTION_ITEM_DEL:
		{
			CItemPtr pItem = NULL;
			if(pAction->GetInt(ACTIONDATA_DATA))
				pItem = pUser->GetTaskItemByType(pAction->GetInt(ACTIONDATA_DATA));
			else if(strlen(szParam) > 0)
				pItem = pUser->GetTaskItem(szParam);
			if(pItem && !pItem->IsPlunder() && !pItem->TestSpecialFlag(2) && !pItem->TestSpecialFlag(1)
					&& !pUser->IsItemProtected(pItem) && pUser->EraseItem(pItem->GetID(), SYNCHRO_TRUE))
				return true;
		}
		break;
	case	ACTION_ITEM_CHECK:
		{
			CItemPtr pItem = NULL;
			if(pAction->GetInt(ACTIONDATA_DATA))
				pItem = pUser->GetTaskItemByType(pAction->GetInt(ACTIONDATA_DATA));
			else if(strlen(szParam) > 0)
				pItem = pUser->GetTaskItem(szParam);
			if(pItem && !pItem->IsPlunder() && !pItem->TestSpecialFlag(2) && !pItem->TestSpecialFlag(1)
					&& !pUser->IsItemProtected(pItem))
				return true;
		}
		break;
	case	ACTION_ITEM_LEAVESPACE:
		{
			int		nSpace = pAction->GetInt(ACTIONDATA_DATA);
			if(nSpace <= 0 || pUser->IsBackPackSpare(nSpace))
				return true;
		}
		break;
	case	ACTION_ITEM_TRANSFORM:
		{
			int		nMagicType, nMagicLevel, nMonsterType, nKeepSecs;
			int ret = sscanf(szParam, "%d %d %d %d", &nMagicType, &nMagicLevel, &nMonsterType, &nKeepSecs);
			IF_NOT_(4 == ret)
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());

			CMsgMagicEffect	msg;
			IF_OK(msg.Create(pUser->GetID(), nMagicType, nMagicLevel, pUser->GetID(), 0))
				pUser->BroadcastRoomMsg(&msg, true);
			return pUser->TransformLook(nMonsterType, nKeepSecs, true);
		}
		break;
	default:
		LOGERROR("Unsupported ACTION[%u] type [%u]", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE));
	}

	return false;
}

bool CGameAction::ProcessActionNpcOnly(CActionData* pAction, LPCTSTR szParam, CUser* pUser_NoUse, IRole* pRole, CItem* pItem_NoUse, LPCTSTR pszAccept)
{
	CHECKF(pAction);

	CNpc* pNpc;
	IF_NOT(pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
		return false;

	switch(pAction->GetInt(ACTIONDATA_TYPE))
	{
	default:
		LOGERROR("Unsupported ACTION[%u] type [%u]", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE));
	}

	return false;
}

bool CGameAction::ProcessActionSyn(CActionData* pAction, LPCTSTR szParam, CUser* pUser, IRole* pRole, CItem* pItem, LPCTSTR pszAccept)
{
	CHECKF(pAction);
	if(!pUser)
	{
		LOGERROR("CGameAction::ProcessActionSyn() ActionID=%u, ActionType=%u at %s, %d", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE),
				__FILE__, __LINE__);
		return false;
	}

	int nActionType = pAction->GetInt(ACTIONDATA_TYPE);
	switch(nActionType)
	{
	case	ACTION_SYN_CREATE:
		{
			int		nLevel, nMoney, nMoneyLeave;
			if(sscanf(szParam, "%u %u %u", &nLevel, &nMoney, &nMoneyLeave) < 3)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			if(pUser->GetSynID() != ID_NONE)
				return false;

			if(!pszAccept)
				return false;

			if (strlen(pszAccept) < 2 ||
					strlen(pszAccept) >= _MAX_NAMESIZE ||
						!::NameStrCheck((char*)pszAccept))
			{
				pUser->SendSysMsg(STR_INVALID_GUILD_NAME);
				return false;
			}

			OBJID idSyn = pUser->QuerySynAttr()->CreateSyndicate(pszAccept, nLevel, nMoney, nMoneyLeave);
			if(idSyn == ID_NONE)
				return false;
			{
				CMsgSyndicate	msg;
				OBJID idFealty = ID_NONE;
				IF_OK(msg.Create(NPCMSG_CREATE_SYN, idFealty))
					MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(idSyn, &msg);
			}
			return true;
		}
		break;
	case	ACTION_SYN_CREATE_SUB:
		{
			int		nLevel, nMoney, nMoneyLeave;
			if(sscanf(szParam, "%u %u %u", &nLevel, &nMoney, &nMoneyLeave) < 3)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			if(pUser->GetSynID() == ID_NONE || pUser->GetSynRankShow() != RANK_LEADER)
				return false;
			CSyndicate* pSyn = SynManager()->QuerySyndicate(pUser->GetSynID());
			IF_NOT(pSyn)
				return false;

			if(!pszAccept)
				return false;
			if (strlen(pszAccept) < 2 || strlen(pszAccept) > 6 || !::NameStrCheck((char*)pszAccept))
			{
				pUser->SendSysMsg(STR_INVALID_GUILD_NAME);
				return false;
			}

			if(pSyn->GetInt(SYNDATA_MONEY) < nMoney)
			{
				pUser->SendSysMsg(STR_GUILD_FUND_NOT_ENOUGH, nMoney);
				return false;
			}

			CTeam* pTeam = pUser->GetTeam();
			if(!pTeam)
				return false;
			CUser* pTarget = NULL;
			for(int i = 0; i < pTeam->GetMemberAmount(); i++)
			{
				OBJID idUser = pTeam->GetMemberByIndex(i);
				if(idUser != ID_NONE && idUser != pUser->GetID())
				{
					pTarget = UserManager()->GetUser(idUser);
					break;
				}
			}
			if(!pTarget || pTarget->GetSynID() != pSyn->GetID())
				return false;

			OBJID idSubSyn = pTarget->QuerySynAttr()->CreateSubSyn(pszAccept, nLevel, nMoney, nMoneyLeave);
			if(idSubSyn == ID_NONE)
				return false;
			{
				CMsgSyndicate	msg;
				OBJID idFealty = pSyn->GetID();
				IF_OK(msg.Create(NPCMSG_CREATE_SYN, idFealty))
					MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(idSubSyn, &msg);
			}
			return true;
		}
		break;
	case	ACTION_SYN_CHANGE_LEADER:
		{
			int		nNeedLevel;
			if(sscanf(szParam, "%u", &nNeedLevel) != 1)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			if(pUser->GetSynID() == ID_NONE || pUser->GetSynRankShow() != RANK_LEADER)
				return false;
			CSyndicate* pSyn = SynManager()->QuerySyndicate(pUser->GetSynID());
			IF_NOT(pSyn)
				return false;

			if(!pszAccept)
				return false;
			CSyndicate* pSubSyn = SynManager()->QuerySynByName(pszAccept);
			if(!pSubSyn || pSubSyn->GetInt(SYNDATA_FEALTY) != pSyn->GetID())
				return false;

			CTeam* pTeam = pUser->GetTeam();
			if(!pTeam)
				return false;
			CUser* pTarget = NULL;
			for(int i = 0; i < pTeam->GetMemberAmount(); i++)
			{
				OBJID idUser = pTeam->GetMemberByIndex(i);
				if(idUser != ID_NONE && idUser != pUser->GetID())
				{
					pTarget = UserManager()->GetUser(idUser);
					break;
				}
			}
			if(!pTarget || pTarget->GetSynID() != pSyn->GetID())
				return false;

			return pTarget->QuerySynAttr()->ChangeLeader(pszAccept, atol(szParam));
		}
		break;
	case	ACTION_SYN_DESTROY:
		{
			OBJID idSyn = pUser->QuerySynAttr()->GetSynID();
			if(idSyn == ID_NONE)
				return false;

			if(pUser->GetSynRank() != RANK_LEADER)
			{
				pUser->SendSysMsg(STR_NOT_AUTHORIZED);
				return false;
			}

			CSyndicate* pSyn = SynManager()->QuerySyndicate(idSyn);
			if(pSyn && pSyn->GetInt(SYNDATA_AMOUNT) > 15)
			{
				pUser->SendSysMsg(STR_NO_DISBAND);
				return false;
			}
			CSynSet::Iterator pSub = SynManager()->QuerySynSet()->NewEnum();
			while(pSub.Next())
			{
				if(pSub && pSub->GetInt(SYNDATA_FEALTY) == idSyn)
				{
					pUser->SendSysMsg(STR_NO_DISBAND);
					return false;
				}
			}

			if(!SynManager()->QueryModify()->DestroySyndicate(idSyn))
				return false;
			{
				CMsgSyndicate	msg;
				IF_OK(msg.Create(NPCMSG_DESTROY_SYN, ID_NONE))
					MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(idSyn, &msg);
			}
			return true;
		}
		break;
	case	ACTION_SYN_ALLY:
		{
			OBJID idSyn = pUser->GetSynID();
			if(idSyn == ID_NONE)
				return false;
			CSynPtr pSyn = SynManager()->QuerySyndicate(idSyn);
			if(!pSyn || !pSyn->ReloadData())
				return false;
			if(pUser->GetSynRankShow() != RANK_LEADER)
			{
				return false;
			}

			CTeam* pTeam = pUser->GetTeam();
			if(!pTeam)
				return false;
			CUser* pTarget = NULL;
			for(int i = 0; i < pTeam->GetMemberAmount(); i++)
			{
				OBJID idUser = pTeam->GetMemberByIndex(i);
				if(idUser != ID_NONE && idUser != pUser->GetID())
				{
					pTarget = UserManager()->GetUser(idUser);
					break;
				}
			}
			if(!pTarget)
				return false;
			OBJID	idTargetSyn = pTarget->GetSynID();
			if(idTargetSyn == ID_NONE || idTargetSyn == pUser->GetSynID())
				return false;
			if(pTarget->GetSynRankShow() != RANK_LEADER)
				return false;
			CSyndicate* pTargetSyn = SynManager()->QuerySyndicate(idTargetSyn);
			if(!pTargetSyn || !pTargetSyn->ReloadData())
				return false;
			pTargetSyn = pTargetSyn->GetMasterSyn();

			int nIdx = -1;
			for(int i = 0; i < MAX_SYNALLYSIZE; i++)
			{
				OBJID idTarget = pSyn->GetInt((SYNDATA)(SYNDATA_ALLY0+i));
				if(idTarget == ID_NONE || SynManager()->QuerySyndicate(idTarget) == NULL || idTarget == pTargetSyn->GetID())
					nIdx = i;
			}
			if(nIdx == -1)
			{
				pUser->SendSysMsg(STR_ALLY_FULL);
				return false;
			}
			int nIdx2 = -1;
			for(int i = 0; i < MAX_SYNALLYSIZE; i++)
			{
				OBJID idTarget = pTargetSyn->GetInt((SYNDATA)(SYNDATA_ALLY0+i));
				if(idTarget == ID_NONE || SynManager()->QuerySyndicate(idTarget) == NULL || idTarget == pSyn->GetID())
					nIdx2 = i;
			}
			if(nIdx2 == -1)
			{
				pUser->SendSysMsg(STR_HIS_ALLY_FULL);
				return false;
			}
			pSyn->QueryModify()->SetData((SYNDATA)(SYNDATA_ALLY0+nIdx), pTargetSyn->GetID(), true);
			pTargetSyn->QueryModify()->SetData((SYNDATA)(SYNDATA_ALLY0+nIdx2), pSyn->GetID(), true);
			{
				CMsgSyndicate	msg;
				IF_OK(msg.Create(SET_ALLY, pTargetSyn->GetID()))
					MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(pSyn->GetID(), &msg);
			}

			return true;
		}
		break;
	case	ACTION_SYN_CLEAR_ALLY:
		{
			if(!pszAccept)
				return false;

			OBJID idSyn = pUser->QuerySynAttr()->GetSynID();
			if(idSyn == ID_NONE)
				return false;
			CSynPtr pSyn = SynManager()->QuerySyndicate(idSyn);
			if(!pSyn)
				return false;
			if(pUser->GetSynRankShow() != RANK_LEADER)
			{
				return false;
			}

			CSyndicate* pTargetSyn = SynManager()->QuerySynByName(pszAccept);
			if(!pTargetSyn)
				return false;
			pTargetSyn = pTargetSyn->GetMasterSyn();

			for(int i = 0; i < MAX_SYNALLYSIZE; i++)
			{
				OBJID idTarget = pSyn->GetInt((SYNDATA)(SYNDATA_ALLY0+i));
				if(idTarget == pTargetSyn->GetID())
					pSyn->QueryModify()->SetData((SYNDATA)(SYNDATA_ALLY0+i), ID_NONE, true);
			}
			for(int i = 0; i < MAX_SYNALLYSIZE; i++)
			{
				OBJID idTarget = pTargetSyn->GetInt((SYNDATA)(SYNDATA_ALLY0+i));
				if(idTarget == pSyn->GetID())
					pTargetSyn->QueryModify()->SetData((SYNDATA)(SYNDATA_ALLY0+i), ID_NONE, true);
			}
			{
				CMsgSyndicate	msg;
				IF_OK(msg.Create(CLEAR_ALLY, pTargetSyn->GetID()))
					MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(pSyn->GetID(), &msg);
			}
			return true;
		}
		break;
	case	ACTION_SYN_ANTAGONIZE:
		{
			if(!pszAccept)
				return false;

			OBJID idSyn = pUser->QuerySynAttr()->GetSynID();
			if(idSyn == ID_NONE)
				return false;
			CSynPtr pSyn = SynManager()->QuerySyndicate(idSyn);
			if(!pSyn)
				return false;
			if(pUser->GetSynRankShow() != RANK_LEADER)
			{
				return false;
			}

			CSyndicate* pTargetSyn = SynManager()->QuerySynByName(pszAccept);
			if(!pTargetSyn || pTargetSyn->GetID() == idSyn)
				return false;
			pTargetSyn = pTargetSyn->GetMasterSyn();

			int nCount = 0;
			for(int i = 0; i < MAX_SYNENEMYSIZE; i++)
			{
				OBJID idTarget = pSyn->GetInt((SYNDATA)(SYNDATA_ENEMY0+i));
				if(idTarget != ID_NONE)
				{
					nCount++;
					if(idTarget == pTargetSyn->GetID())
						return false;
				}
			}
			if(nCount == MAX_SYNENEMYSIZE)
				return false;
			for(int i = 0; i < MAX_SYNENEMYSIZE; i++)
			{
				OBJID idTarget = pSyn->GetInt((SYNDATA)(SYNDATA_ENEMY0+i));
				if(idTarget == ID_NONE || SynManager()->QuerySyndicate(idTarget) == NULL)
				{
					pSyn->QueryModify()->SetData((SYNDATA)(SYNDATA_ENEMY0+i), pTargetSyn->GetID(), true);
					break;
				}
			}
			{
				CMsgSyndicate	msg;
				IF_OK(msg.Create(SET_ANTAGONIZE, pTargetSyn->GetID()))
					MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(pSyn->GetID(), &msg);
			}
			return true;
		}
		break;
	case	ACTION_SYN_CLEAR_ANTAGONIZE:
		{
			if(!pszAccept)
				return false;

			OBJID idSyn = pUser->QuerySynAttr()->GetSynID();
			if(idSyn == ID_NONE)
				return false;
			CSynPtr pSyn = SynManager()->QuerySyndicate(idSyn);
			if(!pSyn)
				return false;
			if(pUser->GetSynRankShow() != RANK_LEADER)
			{
				return false;
			}

			OBJID idTargetSyn = ID_NONE;
			CSyndicate* pTargetSyn = SynManager()->QuerySynByName(pszAccept);
			if(pTargetSyn)
				pTargetSyn = pTargetSyn->GetMasterSyn();
			if(pTargetSyn)
				idTargetSyn = pTargetSyn->GetID();

			for(int i = 0; i < MAX_SYNENEMYSIZE; i++)
			{
				OBJID idTarget = pSyn->GetInt((SYNDATA)(SYNDATA_ENEMY0+i));
				if(idTarget != ID_NONE && (idTarget == idTargetSyn || SynManager()->QuerySyndicate(idTarget) == NULL))
				{
					pSyn->QueryModify()->SetData((SYNDATA)(SYNDATA_ENEMY0+i), ID_NONE, true);
					CMsgSyndicate	msg;
					IF_OK(msg.Create(CLEAR_ANTAGONIZE, idTarget))
						MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(pSyn->GetID(), &msg);
				}
			}
			return true;
		}
		break;
	case	ACTION_SYN_DONATE:
		{
			if(!pszAccept)
				return false;

			char* pTemp;
			int nMoney = strtol(pszAccept, &pTemp, 10);
			if(nMoney <= 0 || pUser->GetMoney() < nMoney)
				return false;

			pUser->QuerySynAttr()->DonateMoney(nMoney); return true;
		}
		break;
	case	ACTION_SYN_DEMISE:
		{
			if(!pszAccept)
				return false;

			OBJID idSyn = pUser->GetSynID();
			if(idSyn == ID_NONE)
				return false;
			CSynPtr pSyn = SynManager()->QuerySyndicate(idSyn);
			if(!pSyn)
				return false;

			if(pUser->GetSynRank() != RANK_LEADER)
				return false;

			CUser* pTarget = UserManager()->GetUser(pszAccept);
			if(!pTarget)
			{
				pUser->SendSysMsg(STR_NOT_HERE);
				return false;
			}
			if(pTarget->GetID() == pUser->GetID())
				return false;

			int nNeedLevel = atol(szParam);
			if(nNeedLevel && pTarget->GetLev() < nNeedLevel)
			{
				pUser->SendSysMsg(STR_LEVEL_NOT_ENOUGH);
				return false;
			}
			OBJID	idTargetSyn = pTarget->GetSynID();
			if(idTargetSyn == ID_NONE || idTargetSyn != idSyn)
				return false;

			pSyn->QueryModify()->Demise(pUser->GetID(), pTarget->GetID(), pTarget->GetName(), pSyn->GetID(), true); return true;
		}
		break;
	case	ACTION_SYN_SET_ASSISTANT:
		{
			if(!pszAccept)
				return false;

			OBJID idSyn = pUser->GetSynID();
			if(idSyn == ID_NONE)
				return false;
			CSynPtr pSyn = SynManager()->QuerySyndicate(idSyn);
			if(!pSyn)
				return false;
			int nRank = pUser->GetSynRank();
			if(nRank != RANK_LEADER)
				return false;

			if(pUser->QuerySynAttr()->GetAssistantCount() >= 5)
			{
				pUser->SendSysMsg(STR_SYN_PLACE_FULL);
				return false;
			}

			CUser* pTarget = UserManager()->GetUser(pszAccept);
			if(!pTarget)
			{
				pUser->SendSysMsg(STR_NOT_HERE);
				return false;
			}
			if(pUser->GetID() == pTarget->GetID())
				return false;
			OBJID	idTargetSyn = pTarget->GetSynID();
			if(idTargetSyn == ID_NONE || idTargetSyn != idSyn)
				return false;

			pTarget->QuerySynAttr()->SetRank(90, false); return true;
		}
		break;
	case	ACTION_SYN_CLEAR_RANK:
		{
			if(!pszAccept)
				return false;

			OBJID idSyn = pUser->GetSynID();
			if(idSyn == ID_NONE)
				return false;
			CSynPtr pSyn = SynManager()->QuerySyndicate(idSyn);
			if(!pSyn)
				return false;
			int nRank = pUser->GetSynRank();
			if(nRank != RANK_LEADER)
				return false;

			CUser* pTarget = UserManager()->GetUser(pszAccept);
			if(!pTarget)
			{
				pUser->SendSysMsg(STR_NOT_HERE);
				return false;
			}
			if(pUser->GetID() == pTarget->GetID())
				return false;
			OBJID	idTargetSyn = pTarget->GetSynID();
			if(idTargetSyn == ID_NONE || idTargetSyn != idSyn)
				return false;

			pTarget->QuerySynAttr()->SetRank(50, false); return true;
		}
		break;
	case	ACTION_SYN_PRESENT_MONEY:
		{
			if(!pszAccept)
				return false;

			OBJID idSyn = pUser->GetSynID();
			if(idSyn == ID_NONE)
				return false;
			CSynPtr pSyn = SynManager()->QuerySyndicate(idSyn);
			if(!pSyn)
				return false;
			int nRank = pUser->GetSynRank();
			if(nRank != RANK_LEADER)
			{
				return false;
			}

			int nMoney = atol(pszAccept);
			if(nMoney <= 0 || pSyn->GetInt(SYNDATA_MONEY) < nMoney)
				return false;
			if(nMoney < 10000)
			{
				return false;
			}

			OBJID	idTargetSyn = pUser->TaskIterator();
			if(idTargetSyn == ID_NONE || idTargetSyn == pUser->GetSynID())
				return false;

			pSyn->QueryModify()->PresentMoney(idTargetSyn, nMoney, true); return true;
		}
		break;
	case	ACTION_SYN_FIND_NEXT_SYN:
		{
			return SynManager()->FindNextSyn(pUser->TaskIterator());
		}
		break;
	case	ACTION_SYN_FIND_BY_NAME:
		{
			if(!pszAccept || strlen(pszAccept) >= _MAX_NAMESIZE)
				return false;

			CSynPtr pSyn = SynManager()->QuerySynByName(pszAccept);
			if(pSyn)
				pUser->TaskIterator()	= pSyn->GetID();
			else
				pUser->TaskIterator()	= ID_NONE;
			return pUser->TaskIterator() != ID_NONE;
		}
		break;
	case	ACTION_SYN_FIND_NEXT_SYNMEMBER:
		{
			OBJID idSyn = pUser->GetSynID();
			if(idSyn != ID_NONE)
				return UserManager()->FindNextSynMember(idSyn, pUser->TaskIterator());
		}
		break;
	case	ACTION_SYN_KICKOUT_MEMBER:
		{
			if(!pszAccept || strlen(pszAccept) >= _MAX_NAMESIZE)
				return false;

			OBJID idSyn = pUser->GetSynID();
			int	  nRank = pUser->GetSynRank();
			if(idSyn == ID_NONE || nRank != RANK_LEADER)
				return false;
			if(strcmp(pszAccept, pUser->GetName()) == 0)
				return false;

			CSyndicate* pSyn = SynManager()->QuerySyndicate(idSyn);
			if(pSyn && pSyn->GetInt(SYNDATA_MONEY) < SYN_MONEY_BASELINE)
			{
				pUser->SendSysMsg(STR_TOO_LOWER_SYN_MONEY);
				return false;
			}

			MSGBUF	szMsg;
			sprintf(szMsg, STR_KICKOUT_MEMBER_s, pszAccept);
			IF_OK(pSyn)
				pSyn->BroadcastSubSynMsg(pUser->GetSocketID(), pUser->GetID(), szMsg);

			CUser* pTarget = UserManager()->GetUser(pszAccept);
			if(pTarget)
			{
				pTarget->SendSysMsg(STR_KICKOUT_ME_s, pUser->GetName());
				if(pTarget->GetSynID() == idSyn)
					return pTarget->QuerySynAttr()->LeaveSyn(true, true);
			}
			else
			{
				return pUser->QuerySynAttr()->KickoutMember(pszAccept);
			}
		}
		break;
	case	ACTION_SYN_ATTR:
		{
			PARAMBUF	szField, szOpt;
			int			nData;
			OBJID		idSyn = ID_NONE;
			if(sscanf(szParam, "%s %s %d %u", szField, szOpt, &nData, &idSyn) < 3)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}
			if(idSyn == ID_NONE)
				idSyn = pUser->GetSynID();
			CHECKF(idSyn);

			CSyndicate* pSyn = SynManager()->QuerySyndicate(idSyn);
			CHECKF(pSyn);

			if(stricmp(szField, "money") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
					{ pSyn->QueryModify()->AddData(SYNDATA_MONEY, nData, true); return true; }
				else if(strcmp(szOpt, "<") == 0)
				{
					if(pSyn->GetInt(SYNDATA_MONEY) < nData)
						return true;
				}
			}
			else if(stricmp(szField, "fealty") == 0)
			{
				if(strcmp(szOpt, "==") == 0)
				{
					if(pSyn->GetInt(SYNDATA_FEALTY) == nData)
						return true;
				}
			}
		}
		break;
	default:
		LOGERROR("Unsupported ACTION[%u] type [%u]", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE));
	}

	return false;
}

bool CGameAction::ProcessActionUser(CActionData* pAction, LPCTSTR szParam, CUser* pUser, IRole* pRole, CItem* pItem, LPCTSTR pszAccept)
{
	CHECKF(pAction);
	if(!pUser)
	{
		LOGERROR("CGameAction::ProcessActionUser() ActionID=%u, ActionType=%u at %s, %d", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE),
				__FILE__, __LINE__);
		return false;
	}

	switch(pAction->GetInt(ACTIONDATA_TYPE))
	{
	case	ACTION_USER_EXP_BY_TIME:
		{
			UINT	nTime = 0, nVarIdx = 0;
			if(2 != sscanf(szParam, "%u %u", &nTime, &nVarIdx))
				return false;

			pUser->SetVarData(nVarIdx, pUser->CalcExpByTime(nTime));
			return true;
		}
		break;

	case	ACTION_USER_LEV_BY_EXP:
		{
			__int64	i64Exp = 0;
			UINT	nLevIdx = 0, nPercentIdx = 0;
			if(3 != sscanf(szParam, "%I64d %u %u", &i64Exp, &nLevIdx, &nPercentIdx))
				return false;

			int		nLev = 0, nPercent = 0;
			pUser->CalcLevByExp(i64Exp, &nLev, &nPercent);
			pUser->SetVarData(nLevIdx, (DWORD)nLev);
			pUser->SetVarData(nPercentIdx, (DWORD)nPercent);
			return true;
		}
		break;

	case	ACTION_USER_TIMECARD:
		{
			char	szOpt[256]	= "";
			int		nData		= 0;
			if(2 != sscanf(szParam, "%s %d", szOpt, &nData))
			{
				LOGERROR("ACTION %u: bad parameter", pAction->GetID());
				return false;
			}

			if(strcmp(szOpt, "<") == 0)
				return pUser->GetTimeCardLeft(2, false) < nData;
			else if(strcmp(szOpt, "==") == 0)
				return pUser->GetTimeCardLeft(2, false) == nData;
			else if(strcmp(szOpt, "+=") == 0)
			{
				pUser->AddTimeCardHours(nData);
				return true;
			}
			else if(strcmp(szOpt, "chk") == 0)
			{
				if(nData == 0)
					return pUser->IsTimeCardOn();
				else
					return pUser->IsTimeCardStatus();
			}

			LOGERROR("ACTION %u: bad parameter", pAction->GetID());
			return false;
		}
		break;

	case	ACTION_USER_SQL_VARSTR:
		{
			char	szBuf[256]	= "";
			if(!Database()->GetFieldData(szParam, szBuf, 256))
				return false;

			pUser->SetVarStr(pAction->GetInt(ACTIONDATA_DATA), szBuf);
			return true;
		}
		break;

	case	ACTION_USER_EQUIP_SUBTYPE:
		{
			int		nPos = 0, nSubType = 0;
			if(2 != sscanf(szParam, "%d %d", &nPos, &nSubType))
			{
				LOGERROR("ACTION %u: bad parameter", pAction->GetID());
				return false;
			}

			CItem* pEquip = pUser->GetEquipItemByPos(nPos);
			if(!pEquip)
				return false;

			return pEquip->GetSubType() == nSubType;
		}
		break;

	case	ACTION_USER_ACTION_MSG:
		{
			char	szOpt[256]	= "";
			int		nAction = 0, nData = 0;
			if(3 != sscanf(szParam, "%s %d %d", szOpt, &nAction, &nData))
			{
				LOGERROR("ACTION %u: bad parameter", pAction->GetID());
				return false;
			}

			bool	bBroadcast = (stricmp("send", szOpt) != 0);
			CMsgAction	msg;
			if(msg.Create(pUser->GetID(), pUser->GetPosX(), pUser->GetPosY(), pUser->GetDir(), nAction, nData))
			{
				if(bBroadcast)
					pUser->BroadcastRoomMsg(&msg, true);
				else
					pUser->SendMsg(&msg);
			}
			return true;
		}
		break;

	case	ACTION_USER_STC_COMPARE:
		{
			OBJID	idEvent = 0, idType = 0;
			int		nData = 0;
			char	szOpt[256]	= "";
			if(4 != sscanf(szParam, "stc(%u, %u) %s %d", &idEvent, &idType, szOpt, &nData))
			{
				LOGERROR("ACTION %u: bad parameter", pAction->GetID());
				return false;
			}

			if(strcmp(szOpt, ">") == 0)
				return pUser->GetStatistic(idEvent, idType) > nData;
			else if(strcmp(szOpt, ">=") == 0)
				return pUser->GetStatistic(idEvent, idType) >= nData;
			else
				return pUser->GetStatistic(idEvent, idType) == nData;
		}
		break;

	case	ACTION_USER_STC_OPE:
		{
			char	szOpt[256]	= "";
			OBJID	idEvent = 0, idType = 0;
			int		nData = 0, nSynchro = 0;
			if(5 != sscanf(szParam, "stc(%u, %u) %s %d %d", &idEvent, &idType, szOpt, &nData, &nSynchro))
			{
				LOGERROR("ACTION %u: bad parameter", pAction->GetID());
				return false;
			}

			if(strcmp(szOpt, "+=") == 0)
			{
				pUser->SetStatistic(idEvent, idType, pUser->GetStatistic(idEvent, idType) + nData, false, nSynchro > 0, true);
				return true;
			}
			else if(strcmp(szOpt, "=") == 0)
			{
				pUser->SetStatistic(idEvent, idType, nData, false, nSynchro > 0, true);
				return true;
			}

			LOGERROR("ACTION %u: bad parameter", pAction->GetID());
			return false;
		}
		break;

	case	ACTION_USER_STC_TIMESTAMP:
		{
			char	szOpt[256]	= "";
			OBJID	idEvent = 0, idType = 0;
			int		nData = 0;
			if(4 != sscanf(szParam, "timestamp(%u, %u) %s %d", &idEvent, &idType, szOpt, &nData))
			{
				LOGERROR("ACTION %u: bad parameter", pAction->GetID());
				return false;
			}

			if(stricmp(szOpt, "+=") == 0)
			{
				pUser->SetStatisticTimestamp(idEvent, idType, pUser->GetStatisticTimestamp(idEvent, idType) + nData);
				return true;
			}
			else if(stricmp(szOpt, "set") == 0)
			{
				pUser->SetStatisticTimestamp(idEvent, idType, nData ? nData : time(NULL));
				return true;
			}
			else if(stricmp(szOpt, ">=") == 0)
			{
				DWORD	dwTime = nData ? nData : time(NULL);
				return pUser->GetStatisticTimestamp(idEvent, idType) >= dwTime;
			}
			else if(stricmp(szOpt, ">") == 0)
			{
				DWORD	dwTime = nData ? nData : time(NULL);
				return pUser->GetStatisticTimestamp(idEvent, idType) > dwTime;
			}
			else if(stricmp(szOpt, "==") == 0)
			{
				DWORD	dwTime = nData ? nData : time(NULL);
				return pUser->GetStatisticTimestamp(idEvent, idType) == dwTime;
			}

			LOGERROR("ACTION %u: bad parameter", pAction->GetID());
			return false;
		}
		break;

	case	ACTION_USER_STC_INTERVAL:
		{
			char	szOpt[256]	= "";
			OBJID	idEvent = 0, idType = 0;
			DWORD	dwDays = 0;
			int		nData = 0;
			if(5 != sscanf(szParam, "interval(%u, %u, %u) %s %d", &idEvent, &idType, &dwDays, szOpt, &nData))
			{
				LOGERROR("ACTION %u: bad parameter", pAction->GetID());
				return false;
			}

			DWORD	dwTimestamp = pUser->GetStatisticTimestamp(idEvent, idType);
			if(!dwTimestamp)
				return false;

			TIME_ZONE_INFORMATION	tzi;
			::GetTimeZoneInformation(&tzi);
			DWORD	dwNow		= time(NULL);
			int		nDayStamp	= (dwTimestamp - tzi.Bias*60) / 86400;
			int		nDayNow		= (dwNow - tzi.Bias*60) / 86400;
			if(stricmp(szOpt, ">=") == 0)
			{
				if(dwDays == 0)
					return dwNow >= dwTimestamp + nData;
				else
					return nDayNow >= nDayStamp + nData;
			}
			else if(stricmp(szOpt, ">") == 0)
			{
				if(dwDays == 0)
					return dwNow > dwTimestamp + nData;
				else
					return nDayNow > nDayStamp + nData;
			}
			else if(stricmp(szOpt, "==") == 0)
			{
				if(dwDays == 0)
					return dwNow == dwTimestamp + nData;
				else
					return nDayNow == nDayStamp + nData;
			}

			LOGERROR("ACTION %u: bad parameter", pAction->GetID());
			return false;
		}
		break;

	case	ACTION_USER_DELAY_ACTION:
		{
			UINT	nSecs = 0;
			OBJID	idAction = ID_NONE;
			if(2 != sscanf(szParam, "%u %u", &nSecs, &idAction))
				return false;

			pUser->AddDelayAction(nSecs, idAction);
			return true;
		}
		break;

	case	ACTION_USER_MAPITEM_MSG:
		{
			CMsgMapItem	msg;
			if(msg.Create(pAction->GetInt(ACTIONDATA_DATA), pUser->GetPosX(), pUser->GetPosY(), 13))
				pUser->SendMsg(&msg);
			return true;
		}
		break;

	case	ACTION_USER_TARGET_ROLE:
		return pUser->ProcessTargetRole(pRole->GetID());

	case	ACTION_USER_ACCEPT_SET:
		{
			if(!pszAccept)
				return false;
			return pUser->SetAcceptData(atoi(pszAccept));
		}
		break;

	case	ACTION_USER_ACCEPT_CHK:
		{
			if(pUser->GetAcceptKey() == 0)
				return true;
			if(!pszAccept)
				return false;
			int	nKey = atoi(pszAccept);
			return pUser->GetAcceptKey() == nKey;
		}
		break;

	case	ACTION_USER_VAR_COMPARE:
		{
			char	szOpt[256]	= "";
			int		nVarIdx = 0;
			__int64	i64Data = 0;
			if(3 != sscanf(szParam, "var(%d) %s %I64d", &nVarIdx, szOpt, &i64Data))
				return false;

			if(strcmp(szOpt, ">") == 0)
				return pUser->GetVarData(nVarIdx) > i64Data;
			else if(strcmp(szOpt, ">=") == 0)
				return pUser->GetVarData(nVarIdx) >= i64Data;
			else
				return pUser->GetVarData(nVarIdx) == i64Data;
		}
		break;

	case	ACTION_USER_VAR_OPE:
		{
			int		nVarIdx = 0;
			__int64	i64Data = 0;
			char	szOpt[256]	= "";
			if(3 != sscanf(szParam, "var(%d) %s %I64d", &nVarIdx, szOpt, &i64Data))
				return false;

			if(strcmp(szOpt, "+=") == 0)
				pUser->SetVarData(nVarIdx, pUser->GetVarData(nVarIdx) + i64Data);
			else if(strcmp(szOpt, "-=") == 0)
				pUser->SetVarData(nVarIdx, pUser->GetVarData(nVarIdx) - i64Data);
			else if(strcmp(szOpt, "*=") == 0)
				pUser->SetVarData(nVarIdx, pUser->GetVarData(nVarIdx) * i64Data);
			else if(strcmp(szOpt, "/=") == 0)
			{
				if(i64Data == 0)
					return false;
				pUser->SetVarData(nVarIdx, pUser->GetVarData(nVarIdx) / i64Data);
			}
			else if(strcmp(szOpt, "mod=") == 0)
			{
				if(i64Data == 0)
					return false;
				pUser->SetVarData(nVarIdx, pUser->GetVarData(nVarIdx) % i64Data);
			}
			else if(strcmp(szOpt, "&=") == 0)
				pUser->SetVarData(nVarIdx, pUser->GetVarData(nVarIdx) & i64Data);
			else if(strcmp(szOpt, "|=") == 0)
				pUser->SetVarData(nVarIdx, pUser->GetVarData(nVarIdx) | i64Data);
			else if(strcmp(szOpt, "^=") == 0)
				pUser->SetVarData(nVarIdx, pUser->GetVarData(nVarIdx) ^ i64Data);
			else
				return false;
			return true;
		}
		break;

	case	ACTION_USER_VAR_SET:
		{
			int		nVarIdx = 0;
			__int64	i64Data = 0;
			char	szOpt[256]	= "";
			if(3 != sscanf(szParam, "var(%d) %s %I64d", &nVarIdx, szOpt, &i64Data))
				return false;

			pUser->SetVarData(nVarIdx, i64Data);
			return true;
		}
		break;

	case	ACTION_USER_DROP_ITEM3:
		{
			OBJID	idData = 0;
			int		nData1 = 0, nData2 = 0;
			if(3 != sscanf(szParam, "%u %d %d", &idData, &nData1, &nData2))
				return false;

			return pUser->DropItem3(idData, nData1, nData2);
		}
		break;

	case	ACTION_USER_DBL_EXP:
		{
			int		nPercent = 100;
			DWORD	dwSecs = 0;
			if(2 != sscanf(szParam, "%d %u", &nPercent, &dwSecs))
				return false;

			pUser->SetDoubleExp(nPercent, dwSecs);
			return false;
		}
		break;

	case	ACTION_USER_CHKIN_CARD2:
		{
			if (pUser->CheckInCard2())
				return true;
		}
		break;

	case ACTION_USER_CHKOUT_CARD2:
		{
			if (pUser->CheckOutCard2())
				return true;
		}
		break;

	case	ACTION_USER_CHKIN_CARD:
		{
			if (pUser->CheckInCard())
				return true;
		}
		break;

	case ACTION_USER_CHKOUT_CARD:
		{
			if (pUser->CheckOutCard())
				return true;
		}
		break;

	case	ACTION_USER_MEDIAPLAY:
		{
			struct
			{
				char	szOpt[256];
				char	szMedia[256];
				union
				{
					DWORD	dwData;
					struct	{ USHORT usPosX, usPosY; };
				};
				DWORD	dwTimes;
			} info;
			memset(&info, 0, sizeof(info));
			info.dwTimes	= 1;
			if (sscanf(szParam, "%s %s %u %u %u", info.szOpt, info.szMedia, &info.usPosX, &info.usPosY, &info.dwTimes) < 2)
				return false;

			if (0 == stricmp(info.szOpt, "play"))
			{
				CMsgName msg;
				IF_OK(msg.Create(NAMEACT_PLAYER_WAVE, info.szMedia, info.dwData))
				{
					char	szTimes[256]	= "";
					sprintf(szTimes, "%u", info.dwTimes);
					msg.AppendStr(szTimes);
					pUser->SendMsg(&msg);
				}

			}

			else if (0 == stricmp(info.szOpt, "broacast"))
			{
				CMsgName msg;
				IF_OK(msg.Create(NAMEACT_PLAYER_WAVE, info.szMedia, info.dwData))
				{
					char	szTimes[256]	= "";
					sprintf(szTimes, "%u", info.dwTimes);
					msg.AppendStr(szTimes);
					pUser->BroadcastRoomMsg(&msg, true);
				}
			}
			return true;
		}
		break;

	case	ACTION_USER_TASKMASK:
		{
			char szOpt[256] = "";
			int idx = 0;

			if (2 == sscanf(szParam, "%s %d", szOpt, &idx))
			{
				if (0 == stricmp(szOpt, "chk"))
				{
					if (pUser->ChkTaskMask(idx))
						return true;
				}
				else if (0 == stricmp(szOpt, "add"))
				{
					pUser->AddTaskMask(idx);
					return true;
				}
				else if (0 == stricmp(szOpt, "clr"))
				{
					pUser->ClrTaskMask(idx);
					return true;
				}
				else
					LOGERROR("ACTION %u: bad parameter", pAction->GetID());
			}
		}
		break;

	case	ACTION_USER_EFFECT:
		{
			char szOpt[256] = "";
			char szEffect[256] = "";
			char szAct[256] = "";

			if (sscanf(szParam, "%s %s %s", szOpt, szEffect, szAct) < 2)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				return false;
			}

			int	nActType = NAMEACT_ROLEEFFECT;
			if (0 == stricmp(szAct, "del"))
				nActType = 24;
				if (0 == stricmp(szOpt, "self"))
				{
					CMsgName msg;
					IF_OK (msg.Create(nActType, szEffect, pUser->GetID()))
						pUser->BroadcastRoomMsg(&msg, true);

					return true;
				}
				else if (0 == stricmp(szOpt, "couple"))
				{
					CMsgName msg;
					IF_OK (msg.Create(nActType, szEffect, pUser->GetID()))
						pUser->BroadcastRoomMsg(&msg, true);

					if (pUser->IsMarried())
					{
						CUser* pMate = UserManager()->GetUser(pUser->GetMate());
						if (pMate)
						{
							IF_OK (msg.Create(nActType, szEffect, pMate->GetID()))
								pMate->BroadcastRoomMsg(&msg, true);
						}
					}

					return true;
				}
				else if (0 == stricmp(szOpt, "team"))
				{
					CMsgName msg;
					IF_OK (msg.Create(nActType, szEffect, pUser->GetID()))
						pUser->BroadcastRoomMsg(&msg, true);

					CTeam* pTeam = pUser->GetTeam();
					if (pTeam)
					{
						int nTeamMembers = pTeam->GetMemberAmount();
						for (int i=0; i<nTeamMembers; i++)
						{
							OBJID idMember = pTeam->GetMemberByIndex(i);
							CUser* pMember = UserManager()->GetUser(idMember);
							if (pMember && pMember != pUser)
							{
								CMsgName msg;
								IF_OK (msg.Create(nActType, szEffect, pMember->GetID()))
									pMember->BroadcastRoomMsg(&msg, true);
							}
						}
					}

					return true;
				}
				else
					LOGERROR("ACTION %u: bad parameter", pAction->GetID());
		}
		break;

	case	ACTION_USER_SEX:
		{
			return (pUser->GetSex() == 1);
		}
		break;

	case	ACTION_USER_MARRIAGE:
		{
			return pUser->IsMarried();
		}
		break;

	case	ACTION_USER_DIVORCE:
		{
			return pUser->Divorce();
		}
		break;

	case	ACTION_USER_BONUS:
		{
			return pUser->DoBonus();
		}
		break;

	case	ACTION_USER_LOG:
		{
			::MsgLogSave("gmlog/action", szParam);
			return true;
		}
		break;

	case	ACTION_USER_LOG_FILE:
		{
			char	szFile[256]		= "";
			char	szText[1024]	= "";
			if (0 == sscanf(szParam, "%s %s", szFile, szText))
				return false;

			strcpy(szText, szParam + strlen(szFile) + 1);
			::MyLogSave(szFile, szText);
			return true;
		}
		break;

	case	ACTION_USER_ATTR:
		{
			PARAMBUF	szAttr;
			PARAMBUF	szOpt;
			PARAMBUF	szData;
			PARAMBUF	szExt	= "";
			if(sscanf(szParam, "%s %s %s %s", szAttr, szOpt, szData, szExt) < 3)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}
			int		nData = atoi(szData);

			if(stricmp(szAttr, "life") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					return pUser->AddAttrib(_USERATTRIB_LIFE, nData, SYNCHRO_TRUE, 0);
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetLife() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetLife() < nData);
				}
			}
			else if(stricmp(szAttr, "mana") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					return pUser->AddAttrib(_USERATTRIB_MANA, nData, SYNCHRO_TRUE, 0);
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetMana() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetMana() < nData);
				}
			}
			else if(stricmp(szAttr, "lookface") == 0)
			{
				if(strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetLookFace() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetLookFace() < nData);
				}
				else if(stricmp(szOpt, "set") == 0)
				{
					pUser->SetAttrib(_USERATTRIB_LOOKFACE, nData, SYNCHRO_BROADCAST);
					return true;
				}
			}
			else if(stricmp(szAttr, "look") == 0)
			{
				DWORD	dwLookFace = pUser->GetLookFace();
				if(strcmp(szOpt, "==") == 0)
				{
					return (dwLookFace%1000 == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)(dwLookFace%1000) < nData);
				}
				else if(stricmp(szOpt, "set") == 0)
				{
					pUser->SetAttrib(_USERATTRIB_LOOKFACE, (dwLookFace/1000)*1000 + nData%1000, SYNCHRO_BROADCAST);
					return true;
				}
			}
			else if(stricmp(szAttr, "face") == 0)
			{
				DWORD	dwLookFace = pUser->GetLookFace();
				if(strcmp(szOpt, "==") == 0)
				{
					return ((dwLookFace/10000)%1000 == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)((dwLookFace/10000)%1000) < nData);
				}
				else if(stricmp(szOpt, "set") == 0)
				{
					pUser->SetAttrib(_USERATTRIB_LOOKFACE, dwLookFace - ((dwLookFace/10000)%1000)*10000 + (nData%1000)*10000, SYNCHRO_BROADCAST);
					return true;
				}
			}
			else if(stricmp(szAttr, "sex") == 0)
			{
				DWORD	dwLookFace = pUser->GetLookFace();
				if(strcmp(szOpt, "==") == 0)
				{
					return ((dwLookFace/1000)%10 == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)((dwLookFace/1000)%10) < nData);
				}
				else if(stricmp(szOpt, "set") == 0)
				{
					pUser->SetAttrib(_USERATTRIB_LOOKFACE, dwLookFace - ((dwLookFace/1000)%10)*1000 + (nData%10)*1000, SYNCHRO_BROADCAST);
					return true;
				}
			}
			else if(stricmp(szAttr, "transform") == 0)
			{
				DWORD	dwLookFace = pUser->GetLookFace();
				if(strcmp(szOpt, "==") == 0)
				{
					return (dwLookFace/10000000 == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)(dwLookFace/10000000) < nData);
				}
			}
			else if(stricmp(szAttr, "money") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					if(pAction->GetInt(ACTIONDATA_DATA) && pUser->IsIncomeLimited())
						nData = nData * pUser->GetIncomePercent() / 100;
					return pUser->AddAttrib(_USERATTRIB_MONEY, nData, SYNCHRO_TRUE, 0);
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetMoney() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetMoney() < nData);
				}
			}
			else if(stricmp(szAttr, "e_money") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					if(pAction->GetInt(ACTIONDATA_DATA) && pUser->IsIncomeLimited())
						nData = nData * pUser->GetIncomePercent() / 100;
					if(!pUser->AddAttrib(_USERATTRIB_EMONEY, nData, SYNCHRO_TRUE, 0))
						return false;

					OBJID	idSource = ID_NONE;
					if(nData <= 0)
						idSource = pUser->GetID();
					OBJID	idTarget = ID_NONE;
					if(nData > 0)
						idTarget = pUser->GetID();
					pUser->SaveEmoneyLog(1, idSource, idTarget, abs(nData));
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetEmoney() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetEmoney() < nData);
				}
			}
			else if(stricmp(szAttr, "exp") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					pUser->AwardExp(nData, stricmp(szExt, "nocontribute") != 0);
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetExp() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return (pUser->GetExp() < nData);
				}
			}
			else if(stricmp(szAttr, "pk") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					pUser->AddPk(nData);
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					if(pUser->GetPk() == nData)
						return true;
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					if(pUser->GetPk() < nData)
						return true;
				}
			}
			else if(stricmp(szAttr, "profession") == 0)
			{
				if(strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetProfession() == nData);
				}
				else if(strcmp(szOpt, ">=") == 0)
				{
					return (pUser->GetProfession() >= nData);
				}
				else if(strcmp(szOpt, "<=") == 0)
				{
					return (pUser->GetProfession() <= nData);
				}
				else if(strcmp(szOpt, "set") == 0)
				{
					pUser->QueryDataForAction()->SetProfession(nData, true);
					pUser->SetAttrib(_USERATTRIB_PORFESSION, nData, SYNCHRO_BROADCAST);
					return true;
				}
			}
			else if(stricmp(szAttr, "level") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					pUser->SetAttrib(_USERATTRIB_EXP, 0, SYNCHRO_TRUE);
					pUser->IncLev(1, 0);
					if(pUser->IsAutoAllot())
						pUser->AllotPoint();
					else
						pUser->AddAttrib(_USERATTRIB_ADDPOINT, 3, SYNCHRO_TRUE, 0);
					pUser->SetAttrib(_USERATTRIB_LIFE, pUser->GetMaxLife(), SYNCHRO_TRUE);
					if(pUser->GetTeam())
						pUser->BroadcastTeamLife(true);
					pUser->SaveInfo();
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					if(pUser->GetLev() == nData)
						return true;
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					if((int)pUser->GetLev() < nData)
						return true;
				}
			}
			else if(stricmp(szAttr, "force") == 0 || stricmp(szAttr, "strength") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					pUser->AddAttrib(_USERATTRIB_FORCE, nData, SYNCHRO_TRUE, 0);
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					return ((int)pUser->GetForce() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetForce() < nData);
				}
			}
			else if(stricmp(szAttr, "speed") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					pUser->AddAttrib(_USERATTRIB_SPEED, nData, SYNCHRO_TRUE, 0);
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					return ((int)pUser->GetSpeed() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetSpeed() < nData);
				}
			}
			else if(stricmp(szAttr, "health") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					pUser->AddAttrib(_USERATTRIB_HEALTH, nData, SYNCHRO_TRUE, 0);
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					return ((int)pUser->GetHealth() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetHealth() < nData);
				}
			}
			else if(stricmp(szAttr, "soul") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					pUser->AddAttrib(_USERATTRIB_SOUL, nData, SYNCHRO_TRUE, 0);
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					return ((int)pUser->GetSoul() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetSoul() < nData);
				}
			}
			else if(stricmp(szAttr, "rank") == 0)
			{
				if(pUser->GetSynID() == ID_NONE)
					return false;

				if(strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetSynRank() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetSynRank() < nData);
				}
			}
			else if(stricmp(szAttr, "rankshow") == 0)
			{
				if(pUser->GetSynID() == ID_NONE)
					return false;

				if(strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetSynRankShow() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetSynRankShow() < nData);
				}
			}
			else if(stricmp(szAttr, "iterator") == 0)
			{
				if(strcmp(szOpt, "=") == 0)
				{
					pUser->TaskIterator() = nData;
					return true;
				}
				else if(strcmp(szOpt, "<=") == 0)
				{
					if (pUser->TaskIterator() <= nData)
						return true;
				}
				else if(strcmp(szOpt, "+=") == 0)
				{
					pUser->TaskIterator() += nData;
					return true;
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					if (pUser->TaskIterator() == nData)
						return true;
				}
			}
			else if(stricmp(szAttr, "crime") == 0)
			{
				if (strcmp(szOpt, "==") == 0)
				{
					if (nData == 0)
						return (pUser->QueryStatus(STATUS_CRIME) == NULL);
					else
						return (pUser->QueryStatus(STATUS_CRIME) != NULL);
				}
				else if (strcmp(szOpt, "set") == 0)
				{
					if (nData == 0)
					{
						CRole::DetachStatus(pUser->QueryRole(), STATUS_CRIME);
						return true;
					}
					else
					{
						pUser->SetCrimeStatus(STATUS_CRIME, 15);
						return true;
					}
				}
			}
			else if(stricmp(szAttr, "virtue") == 0)
			{
				if(strcmp(szOpt, "+=") == 0)
				{
					if(pUser->IsIncomeLimited())
						nData = nData * pUser->GetIncomePercent() / 100;
					return pUser->AddAttrib(_USERATTRIB_VIRTUE, nData, SYNCHRO_TRUE, 0);
				}
				else if(strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetVirtue() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetVirtue() < nData);
				}
			}
			else if(stricmp(szAttr, "gamecard") == 0)
			{
				if (strcmp(szOpt, "==") == 0)
				{
					return (pUser->CountCard() == nData);
				}
				else if(strcmp(szOpt, ">=") == 0)
				{
					return (pUser->CountCard() >= nData);
				}
				else if(strcmp(szOpt, "<=") == 0)
				{
					return (pUser->CountCard() <= nData);
				}
			}
			else if(stricmp(szAttr, "gamecard2") == 0)
			{
				if (strcmp(szOpt, "==") == 0)
				{
					return (pUser->CountCard2() == nData);
				}
				else if(strcmp(szOpt, ">=") == 0)
				{
					return (pUser->CountCard2() >= nData);
				}
				else if(strcmp(szOpt, "<=") == 0)
				{
					return (pUser->CountCard2() <= nData);
				}
			}
			else if(stricmp(szAttr, "xp") == 0)
			{
				if (strcmp(szOpt, "+=") == 0)
				{
					pUser->AddXp(nData);
					return true;
				}
				else if (strcmp(szOpt, "set") == 0)
				{
					pUser->SetXp(nData);
					return true;
				}
				else if (strcmp(szOpt, "<") == 0)
				{
					return (pUser->GetXp() < nData);
				}
				else if (strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetXp() == nData);
				}
			}
			else if(stricmp(szAttr, "ep") == 0)
			{
				if (strcmp(szOpt, "+=") == 0)
				{
					return pUser->AddAttrib(_USERATTRIB_ENERGY, nData, SYNCHRO_TRUE, 0);
				}
				else if (strcmp(szOpt, "set") == 0)
				{
					pUser->SetAttrib(_USERATTRIB_ENERGY, nData, SYNCHRO_TRUE);
					return true;
				}
				else if (strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetEnergy() < nData);
				}
				else if (strcmp(szOpt, "==") == 0)
				{
					return ((int)pUser->GetEnergy() == nData);
				}
			}
			else if(stricmp(szAttr, "addpoint") == 0)
			{
				if (strcmp(szOpt, "+=") == 0)
				{
					return pUser->AddAttrib(_USERATTRIB_ADDPOINT, nData, SYNCHRO_TRUE, 0);
				}
				else if (strcmp(szOpt, "set") == 0)
				{
					pUser->SetAttrib(_USERATTRIB_ADDPOINT, nData, SYNCHRO_TRUE);
					return true;
				}
				else if (strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetAddPoint() < nData);
				}
				else if (strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetAddPoint() == nData);
				}
			}
			else if(stricmp(szAttr, "metempsychosis") == 0)
			{
				if (strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetMetempsychosis() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetMetempsychosis() < nData);
				}
			}
			else if(stricmp(szAttr, "client_ver") == 0)
			{
				if (strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetClientVersion() == nData);
				}
				else if(strcmp(szOpt, "<") == 0)
				{
					return ((DWORD)pUser->GetClientVersion() < (DWORD)nData);
				}
			}
			else if(stricmp(szAttr, "peerage") == 0)
			{
				if (stricmp(szOpt, "==") == 0)
				{
					LOGERROR("ACTION %u %u: Root is NULL", pAction->GetID(), pAction->GetInt(ACTIONDATA_TYPE));
					return false;
				}
			}
			else if(stricmp(szAttr, "business") == 0)
			{
				if (stricmp(szOpt, "==") == 0)
				{
					return (nData == pUser->GetBusinessman());
				}
				else if(stricmp(szOpt, "<") == 0)
				{
					return (::TimeGet(TIME_DAY) + 19000000 < (DWORD)pUser->GetBusinessman());
				}
				else if(stricmp(szOpt, "set") == 0)
				{
					if (nData == 0)
					{
						pUser->SetBusinessman(0);
						pUser->SynchroBusinessman(0);
						::MyLogSave("gmlog/businessman", "UserID:%u\tName:%s\tBusinessState:%u\t", pUser->GetID(), pUser->GetName(), 0);
					}
					else
					{
						pUser->BecomeBusinessman();
					}
					return true;
				}
			}
			else if(stricmp(szAttr, "storage_money") == 0)
			{
				if (strcmp(szOpt, "+=") == 0)
				{
					return pUser->AddMoneySaved(nData, true);
				}
				else if (strcmp(szOpt, "==") == 0)
				{
					return (pUser->GetMoneySaved() == nData);
				}
				else if (strcmp(szOpt, "<") == 0)
				{
					return ((int)pUser->GetMoneySaved() < nData);
				}
			}
			else
				LOGERROR("ACTION_USER_ATTR ActionID:%u, %s %s %s %s at %s, %d", pAction->GetKey(), szAttr, szOpt, szData, szExt, __FILE__, __LINE__);
		}
		break;

	case	ACTION_USER_FULL:
		{
			if (0 == stricmp(szParam, "life"))
			{
				return pUser->AddAttrib(_USERATTRIB_LIFE, pUser->GetMaxLife(), SYNCHRO_TRUE, 0);
			}
			else if (0 == stricmp(szParam, "mana"))
			{
				return pUser->AddAttrib(_USERATTRIB_MANA, pUser->GetMaxMana(), SYNCHRO_TRUE, 0);
			}
		}
		break;
	case	ACTION_USER_CHGMAP:
		{
			OBJID idMap = ID_NONE;
			int nMapX=0, nMapY=0;
			BOOL bImmediacy = false;

			if (sscanf(szParam, "%u %d %d %d", &idMap, &nMapX, &nMapY, &bImmediacy) < 3)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			if (!pUser->GetMap()->IsTeleportDisable() || bImmediacy)
			{
				int ret = pUser->FlyMap(idMap, nMapX, nMapY);
				if(ret == FLYMAP_NORMAL)
					return true;
				else if(ret == FLYMAP_MAPGROUP)
				{
					m_pUser	= NULL;
					return true;
				}
				else if(ret == FLYMAP_ERROR)
				{
					pUser->FlyMap(1002, 440, 400);
					UserManager()->KickOutSocket(pUser->GetSocketID(), "Bad task map-change position!");
				}
			}
		}
		break;
	case	ACTION_USER_CHGMAPRECORD:
		{
			int ret = pUser->FlyMap(pUser->QueryDataForAction()->GetRecordMap(),
									pUser->QueryDataForAction()->GetRecordX(),
									pUser->QueryDataForAction()->GetRecordY());
			if(ret == FLYMAP_NORMAL)
				return true;
			else if(ret == FLYMAP_MAPGROUP)
			{
				m_pUser	= NULL;
				return true;
			}
		}
		break;
	case	ACTION_USER_RECORDPOINT:
		{
			OBJID idMap = ID_NONE;
			int nMapX=0, nMapY=0;
			if (3 == sscanf(szParam, "%u %d %d", &idMap, &nMapX, &nMapY))
			{
				pUser->SetRecordPos(idMap, nMapX, nMapY, true);
				return true;
			}
		}
		break;
	case	ACTION_USER_HAIR:
		{
			PARAMBUF szCmd = "";
			DWORD dwData = 0;
			if (2 == sscanf(szParam, "%s %u", szCmd, &dwData))
			{
				int nHair = pUser->GetHair();

				if (0 == stricmp("color", szCmd))
				{
					dwData %= 10;
					nHair = nHair-((nHair%1000)/100)*100+dwData*100;
					pUser->SetAttrib(_USERATTRIB_HAIR, nHair, SYNCHRO_TRUE);

					return true;
				}
				else if (0 == stricmp("style", szCmd))
				{
					dwData %= 100;

					nHair = nHair-(nHair%100)+dwData;
					if (nHair < 100)
					{
						const int DEFAULT_COLOR = 3;
						nHair += DEFAULT_COLOR*100;
					}

					pUser->SetAttrib(_USERATTRIB_HAIR, nHair, SYNCHRO_TRUE);

					return true;
				}
			}
		}
		break;
	case	ACTION_USER_TALK:
		{
			int nChannel = pAction->GetInt(ACTIONDATA_DATA);
			if (nChannel == 0)
				nChannel = _TXTATR_NORMAL;
			pUser->SendSysMsg(nChannel, "%s", szParam);
			return true;
		}
		break;
	case	ACTION_USER_MAGIC:
		{
			PARAMBUF	szCmd;
			int			nType;
			int			nData=0;
			int	nNum = 0;
			if((nNum=sscanf(szParam, "%s %u %u", szCmd, &nType, &nData)) < 2)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			if(stricmp(szCmd, "check") == 0)
			{
				if(nNum == 3)
					return pUser->QueryMagic()->CheckLevel(nType, nData);
				else
					return pUser->QueryMagic()->CheckType(nType);
			}
			else if(stricmp(szCmd, "learn") == 0)
			{
				return pUser->QueryMagic()->LearnMagic(nType, 0) != ID_NONE;
			}
			else if(stricmp(szCmd, "uplevel") == 0)
			{
				return pUser->QueryMagic()->UpLevelByTask(nType, true);
			}
			else if(stricmp(szCmd, "addexp") == 0)
			{
				return pUser->QueryMagic()->AddMagicExp(nType, nData);
			}
			else
			{
				MYASSERT(!"ACTION_USER_MAGIC 错误的参数类型!");
			}
			return true;
		}
		break;

	case ACTION_USER_WEAPONSKILL:
		{
			PARAMBUF szCmd = "";
			int nSkillType = 0, nLev = 0;
			if (3 == sscanf(szParam, "%s %d %d", szCmd, &nSkillType, &nLev))
			{
				if (0 == stricmp("check", szCmd))
				{
					CUserWeaponSkill* pSkill = pUser->GetWeaponSkill(nSkillType);
					if (pSkill)
					{
						if (pSkill->GetLevel() >= nLev)
							return true;
					}

				}
				else if (0 == stricmp("learn", szCmd))
				{
					if (pUser->AwardWeaponSkill(nSkillType, nLev))
						return true;
				}
				else if (0 == stricmp("addexp", szCmd))
				{
					pUser->AddWeaponSkillExp(nSkillType, nLev);
					return true;
				}
			}
		}
		break;

	case ACTION_USER_CREATEMAP:
		{
			PARAMBUF	szName;
			OBJID		idOwner, idRebornMap;
			int			nOwnerType, nMapDoc, nType, nPortalX, nPortalY, nRebornPortal, nResLev;
			if(sscanf(szParam, "%s %d %u %d %d %d %d %u %d %d",
					szName, &nOwnerType, &idOwner, &nMapDoc, &nType, &nPortalX, &nPortalY, &idRebornMap, &nRebornPortal, &nResLev) < 10)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			NewMapInfo	info;
			memset(&info, 0, sizeof(NewMapInfo));
			SafeCopy(info.szName, szName, _MAX_NAMESIZE);
			info.nMapDoc		= nMapDoc;
			info.nType			= nType;
			info.nOwnerType		= nOwnerType;
			info.idOwner		= idOwner;
			info.nMapGroup		= MapGroup(PID)->GetMapGroup();
			info.idLikeMap		= pUser->GetMapID();
			info.nLinkX			= pUser->GetPosX();
			info.nLinkY			= pUser->GetPosY();
			info.nPortalX		= nPortalX;
			info.nPortalY		= nPortalY;
			info.idRebornMap	= idRebornMap;
			info.nRebornPortal	= nRebornPortal;
			info.nResLev		= nResLev;

			OBJID idMap = MapManager()->CreateDynaMap(&info);
			if(idMap == ID_NONE)
				return false;

			pUser->QueryDataForAction()->SetHomeID(idMap);

			return true;
		}
		break;
	case ACTION_USER_ENTER_HOME:
		{
			OBJID idHome = pUser->GetHomeID();
			if(idHome == ID_NONE)
				return false;
			CGameMap* pMap = MapManager()->QueryMap(idHome);
			CHECKF(pMap);
			POINT	pos = pMap->GetPortal();
			OBJID idOldMap = pUser->GetMapID();
			int	nOldX = pUser->GetPosX();
			int	nOldY = pUser->GetPosY();
			if (pUser->FlyMap(pMap->GetID(), pos.x, pos.y) == FLYMAP_NORMAL)
			{
				CTeam* pTeam = pUser->GetTeam();
				if(pTeam && pTeam->GetLeader() == pUser->GetID())
				{
					for(int i = pTeam->GetMemberAmount()-1; i >= 0; i--)
					{
						OBJID idTarget = pTeam->GetMemberByIndex(i);
						CUser* pTarget = UserManager()->GetUser(idTarget);
						if(pTarget && pTarget->GetMapID() == idOldMap && pTarget->GetDistance(nOldX, nOldY) <= ENTER_HOME_TEAM_DISTANCE)
						{
							pTarget->FlyMap(pMap->GetID(), pos.x, pos.y);
							pTarget->Synchro();
						}
					}
				}
				return true;
			}
		}
		break;
	case ACTION_USER_ENTER_MATE_HOME:
		{
			OBJID idHome = pUser->GetMateHomeID();
			if(idHome == ID_NONE)
				return false;
			CGameMap* pMap = MapManager()->QueryMap(idHome);
			CHECKF(pMap);
			POINT	pos = pMap->GetPortal();
			return (pUser->FlyMap(pMap->GetID(), pos.x, pos.y) != FLYMAP_ERROR);
		}
		break;
	case	ACTION_USER_UNLEARN_MAGIC:
		{
			int		nData[MAX_UNLEARN_NUMBER+1] = { 0 };
			ASSERT(MAX_UNLEARN_NUMBER == 20);
			int nNum = sscanf(szParam, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
						&nData[0], &nData[1], &nData[2], &nData[3], &nData[4],
						&nData[5], &nData[6], &nData[7], &nData[8], &nData[9],
						&nData[10], &nData[11], &nData[12], &nData[13], &nData[14],
						&nData[15], &nData[16], &nData[17], &nData[18], &nData[19],
						&nData[20] );
			if(nNum < 1 || nNum > MAX_UNLEARN_NUMBER)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				return false;
			}

			bool bDrop = false;
			for(int i = 0; i < nNum; i++)
			{
				int nType = nData[i];
				pUser->QueryMagic()->UnlearnMagic(nType, bDrop);
			}

			return true;
		}
		break;

	case	ACTION_USER_RESET_MAGIC:
		{
			int		nData[MAX_UNLEARN_NUMBER+1] = { 0 };
			ASSERT(MAX_UNLEARN_NUMBER == 20);
			int nNum = sscanf(szParam, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
						&nData[0], &nData[1], &nData[2], &nData[3], &nData[4],
						&nData[5], &nData[6], &nData[7], &nData[8], &nData[9],
						&nData[10], &nData[11], &nData[12], &nData[13], &nData[14],
						&nData[15], &nData[16], &nData[17], &nData[18], &nData[19],
						&nData[20] );
			if(nNum < 1 || nNum > MAX_UNLEARN_NUMBER)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				return false;
			}

			for(int i = 0; i < nNum; i++)
			{
				int nType = nData[i];
				pUser->QueryMagic()->UnlearnMagic(nType, UNLEARN_RESET);
			}

			return true;
		}
		break;
	case	ACTION_USER_DROP_MAGIC:
		{
			int		nData[MAX_UNLEARN_NUMBER+1] = { 0 };
			ASSERT(MAX_UNLEARN_NUMBER == 20);
			int nNum = sscanf(szParam, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
						&nData[0], &nData[1], &nData[2], &nData[3], &nData[4],
						&nData[5], &nData[6], &nData[7], &nData[8], &nData[9],
						&nData[10], &nData[11], &nData[12], &nData[13], &nData[14],
						&nData[15], &nData[16], &nData[17], &nData[18], &nData[19],
						&nData[20] );
			if(nNum < 1 || nNum > MAX_UNLEARN_NUMBER)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				return false;
			}

			int nMode = UNLEARN_DROP;
			for(int i = 0; i < nNum; i++)
			{
				int nType = nData[i];
				pUser->QueryMagic()->UnlearnMagic(nType, nMode);
			}

			return true;
		}
		break;
	case	ACTION_USER_UNLEARN_SKILL:
		{
			return pUser->UnlearnAllSkill();
		}
		break;
	case	ACTION_USER_REBIRTH:
		{
			int		nProf = 0, nLook = 0, nFlag = 1;
			if (sscanf(szParam, "%d %d %d", &nProf, &nLook, &nFlag) < 2)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				return false;
			}

			if(pUser->Rebirth(nProf, nLook, nFlag))
			{
				::MyLogSave("gmlog/rebirth", "User[%s][%d], prof[%d], level[%d] rebirth to prof[%d], look[%d]",
					pUser->GetName(), pUser->GetID(), pUser->GetProfession(), pUser->GetLev(), nProf, nLook);
				return true;
			}
		}
		break;
	case ACTION_USER_WEBPAGE:
		{
			pUser->SendSysMsg(_TXTATR_WEBPAGE, szParam);
			return true;
		}
		break;
	case ACTION_USER_OPEN_DIALOG:
		{
			CMsgAction	msg;
			IF_OK(msg.Create(pUser->GetID(), pUser->GetPosX(), pUser->GetPosY(), pUser->GetDir(), actionOpenDialog, pAction->GetInt(ACTIONDATA_DATA)))
				pUser->SendMsg(&msg);

			return true;
		}
		break;
	case ACTION_USER_REALLOT_POINT:
		{
			pUser->ResetUserPoint();
			return true;
		}
		break;

	case	ACTION_TEAM_ATTR:
		{
			CTeam* pTeam = pUser->GetTeam();
			if(!pTeam || pUser->GetID() != pTeam->GetLeader() || pTeam->GetMemberAmount() <= 1)
				return false;

			PARAMBUF	szField, szOpt="(none)";
			int		nData=0;
			if (sscanf(szParam, "%s %s %d", szField, szOpt, &nData) < 1)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				return false;
			}

			if(strcmp(szField, "count")==0)
			{
				if(strcmp(szOpt, "<")==0)
				{
					if(!( pTeam->GetMemberAmount() < nData ))
						return false;
				}
				else if(strcmp(szOpt, "==")==0)
				{
					if(!( pTeam->GetMemberAmount() == nData ))
						return false;
				}
				else
				{
					return false;
				}
				return true;
			}

			for(int i = 0; i < pTeam->GetMemberAmount(); i++)
			{
				CUser* pMember = pUser->FindAroundUser(pTeam->GetMemberByIndex(i));
				if(!pMember)
					return false;
				if(pMember && pMember->GetID() != pUser->GetID())
				{
					if(strcmp(szField, "money")==0)
					{
						if(strcmp(szOpt, "+=")==0)
						{
							pMember->GainMoney(nData, SYNCHRO_TRUE);
						}
						else if(strcmp(szOpt, "<")==0)
						{
							if(!( (int)pMember->GetMoney() < nData ))
								return false;
						}
						else if(strcmp(szOpt, ">")==0)
						{
							if(!( (int)pMember->GetMoney() > nData ))
								return false;
						}
						else if(strcmp(szOpt, "==")==0)
						{
							if(!( pMember->GetMoney() == nData ))
								return false;
						}
						else
							return false;
					}
					else if(strcmp(szField, "level")==0)
					{
						if(strcmp(szOpt, "<")==0)
						{
							if(!( (int)pMember->GetLev() < nData ))
								return false;
						}
						else if(strcmp(szOpt, ">")==0)
						{
							if(!( (int)pMember->GetLev() > nData ))
								return false;
						}
						else if(strcmp(szOpt, "==")==0)
						{
							if(!( pMember->GetLev() == nData ))
								return false;
						}
						else
							return false;
					}
					else if(strcmp(szField, "mate")==0)
					{
						if(!( pMember->IsMate(pUser) && pMember->IsAlive() ))
							return false;
					}
					else if(strcmp(szField, "friend")==0)
					{
						if(!( pMember->GetFriend(pUser->GetID()) && pMember->IsAlive() ))
							return false;
					}
					else if(strcmp(szField, "count_near")==0)
					{
						if(!( pMember->GetMapID() == pUser->GetMapID() && pMember->IsAlive() ))
							return false;
					}
					else
						return false;
				}
			}
			return true;
		}
		break;
	case ACTION_TEAM_CHGMAP:
		{
			CTeam* pTeam = pUser->GetTeam();
			if(!pTeam || pUser->GetID() != pTeam->GetLeader() || pTeam->GetMemberAmount() <= 1)
				return false;

			OBJID	idMap;
			int		nPosX, nPosY;
			if (sscanf(szParam, "%u %d %d", &idMap, &nPosX, &nPosY) < 3)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				return false;
			}

			for(int i = pTeam->GetMemberAmount()-1; i >= 0; i--)
			{
				CUser* pMember = pUser->FindAroundUser(pTeam->GetMemberByIndex(i));
				if(pMember && pMember->GetID() != pUser->GetID() && pMember->IsAlive())
				{
					pMember->FlyMap(idMap, nPosX, nPosY);
					pMember->Synchro();
				}
			}

			MYASSERT(pUser->FlyMap(idMap, nPosX, nPosY) == FLYMAP_NORMAL);
			return true;
		}
		break;
	case ACTION_USER_LOTTERY:
		{
			int				nLotteryType	= atoi(szParam);
			int				nColor			= pAction->GetInt(ACTIONDATA_DATA);
			ILotteryMgr*	pLotteryMgr		= (ILotteryMgr*)MapGroup(PID)->GetAdapter()->Find(ADAPTER_MODULE_LOTTERY, 0);
			IF_NOT(pLotteryMgr)
				return false;
			ILotteryActor*	pActor			= (ILotteryActor*)MapGroup(PID)->GetAdapter()->GetActor(ADAPTER_MODULE_LOTTERY, pUser);
			IF_NOT(pActor)
				return false;
			bool bRet = pLotteryMgr->Lottery(nLotteryType, nColor, pActor, false);	pActor->Release();	return bRet;	}

	default:
		LOGERROR("Unsupported ACTION[%u] type [%u]", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE));
	}

	return false;
}

bool CGameAction::ProcessActionEvent(CActionData* pAction, LPCTSTR szParam, CUser* pUser_NoUse, IRole* pRole_NoUse, CItem* pItem_NoUse, LPCTSTR pszAccept)
{
	CHECKF(pAction);

	switch(pAction->GetInt(ACTIONDATA_TYPE))
	{
	case ACTION_EVENT_MAPUSER_ACTION:
		{
			OBJID	idMap = ID_NONE, idAction = ID_NONE;
			int		nCount = 0;
			if (3 != sscanf(szParam, "%u %u %d", &idMap, &idAction, &nCount))
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				return false;
			}
			if (nCount == -1)
				nCount = 0xFFFF;

			CUserManager::Iterator pUser = UserManager()->NewEnum();
			while(pUser.Next())
			{
				if (pUser && pUser->GetMapID() == idMap)
				{
					GameAction()->ProcessAction(idAction, pUser);
					if (--nCount <= 0)
						break;
				}
			}
			return true;
		}
		break;
	case ACTION_EVENT_DELNPC_GENID:
		{
			OBJID	idGen = pAction->GetInt(ACTIONDATA_DATA);
			for(IRoleSet::Iter i = RoleManager()->QuerySet()->Begin(); i != RoleManager()->QuerySet()->End(); )
			{
				IRole* pRole = RoleManager()->QuerySet()->GetObjByIter(i);
				i++;
				CMonster* pMonster;
				if(pRole->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)) && pMonster->GetGenID() == idGen)
				{
					if(!pMonster->IsDeleted())
						pMonster->DelMonster(DEL_NOW);
				}
			}

			return true;
		}
		break;

	case ACTION_EVENT_COMPARE:
		{
			int			data1, data2;
			PARAMBUF	opt;
			if (3 != sscanf(szParam, "%d %s %d", &data1, opt, &data2))
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				return false;
			}

			if (strcmp(opt, "==") == 0)
			{
				if(data1 == data2)
					return true;
			}
			else if (strcmp(opt, "<") == 0)
			{
				if(data1 < data2)
					return true;
			}

			if (strcmp(opt, "<=") == 0)
			{
				if(data1 <= data2)
					return true;
			}
		}
		break;
	case	ACTION_EVENT_CREATEPET:
		{
			OBJID	idOwner, idMap, idGen, idType;
			int		nOwnerType, nPosX, nPosY, nData=0;
			PARAMBUF	szName = "";
			if(sscanf(szParam, "%d %u %u %d %d %u %u %d %s", &nOwnerType, &idOwner, &idMap, &nPosX, &nPosY, &idGen, &idType, &nData, szName) < 7)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			const char* pszName = szName;
			if(pszAccept && strlen(pszAccept) > 0)
			{
				if(strlen(pszAccept) >= _MAX_NAMESIZE || !::NameStrCheck((char*)pszAccept))
				{
					return false;
				}
				pszName = pszAccept;
			}

			ST_CREATENEWNPC	info;
			memset(&info, 0, sizeof(ST_CREATENEWNPC));
			info.idMap			= idMap;
			info.idData			= idGen;
			info.idOwner		= idOwner;
			info.usPosX			= nPosX;
			info.usPosY			= nPosY;
			info.usType			= idType;
			info.ucOwnerType	= nOwnerType;
			info.nData			= nData;

			if(strlen(pszName) > 0 && strcmp(pszName, ACTIONSTR_NONE) != 0)
				return NpcManager()->CreateSynPet(&info, pszName);
			else
				return NpcManager()->CreateSynPet(&info);
		}
		break;

	case	ACTION_EVENT_CREATENEW_NPC:
		{
			NPCINFO_ST	info;
			memset(&info, 0, sizeof(NPCINFO_ST));

			NAMESTR	szName;
			ASSERT(_MAX_NPCTASK == 8);
			ASSERT(MAX_NPCDATA == 4);
			int ret = sscanf(szParam, "%s %d %d %d %d %d ""%d %d %d ""%d %d %d ""%d %d %d %d %d %d %d %d ""%d %d %d %d %s",
							szName, &info.nType, &info.nSort, &info.nLookFace, &info.nOwnerType, &info.idOwner,
							&info.idMap, &info.nPosX, &info.nPosY,
							&info.nLife, &info.idBase, &info.idLink,
							&info.setTask[0], &info.setTask[1], &info.setTask[2], &info.setTask[3],
							&info.setTask[4], &info.setTask[5], &info.setTask[6], &info.setTask[7],
							&info.setData[0], &info.setData[1], &info.setData[2], &info.setData[3], info.szData);
			IF_NOT_(ret >= 9)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			CNpc* pNpc = CNpc::CreateNew();
			IF_OK(pNpc)
			{
				IF_OK(pNpc->Create(m_idProcess, &info, ID_NONE, szName))
				{
					RoleManager()->QuerySet()->AddObj(pNpc->QueryRole());
					return true;
				}
				else
					pNpc->ReleaseByOwner();
			}

			return false;
		}
		break;
	case	ACTION_EVENT_COUNTMONSTER:
		{
			OBJID		idMap;
			PARAMBUF	szField;
			PARAMBUF	szData;
			PARAMBUF	szOpt;
			int			nNum;
			int ret = sscanf(szParam, "%u %s %s %s %d", &idMap, szField, szData, szOpt, &nNum);
			IF_NOT_(ret == 5)
				return false;

			int nCount = 0;
			if(stricmp(szField, "name") == 0)
			{
				nCount = RoleManager()->CountMonsterByName(idMap, szData);
			}
			else if(stricmp(szField, "gen_id") == 0)
			{
				OBJID idGen = atol(szData);
				nCount = RoleManager()->CountMonsterByGenID(idMap, idGen);
			}
			else
				return false;

			if(stricmp(szOpt, "<") == 0 && nCount < nNum)
				return true;
			else if(stricmp(szOpt, "==") == 0 && nCount == nNum)
				return true;
			else
				return false;

			return false;
		}
		break;
	case	ACTION_EVENT_DELETEMONSTER:
		{
			OBJID	idMap;
			OBJID	idType;
			int		nData=0;
			PARAMBUF	szName = "";
			if(sscanf(szParam, "%u %u %d %s", &idMap, &idType, &nData, szName) < 2)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			bool	ret = false;
			for(IRoleSet::Iter i = RoleManager()->QuerySet()->Begin(); i != RoleManager()->QuerySet()->End(); )
			{
				IRole* pRole = RoleManager()->QuerySet()->GetObjByIter(i);
				i++;
				CMonster* pPet;
				if(pRole && pRole->QueryObj(OBJ_MONSTER, IPP_OF(pPet))
							&& pPet->GetMap()->GetID() == idMap && pPet->GetType() == idType)
				{
					if(!( nData && pPet->GetData() != nData || strlen(szName) && strcmp(szName, pPet->GetName()) != 0 ))
					{
						if(!pPet->IsDeleted())
						{
							pPet->DelMonster(DEL_NOW);
							ret = true;
						}
					}
				}
			}
			return ret;
		}
		break;
	case	ACTION_EVENT_ERASE:
		{
			OBJID	idMap;
			int		nType;
			PARAMBUF	szName = "";
			if(sscanf(szParam, "%u %d", &idMap, &nType) < 2)
			{
				LOGERROR("ACTION %u: wrong parameter count", pAction->GetID());
				break;
			}

			CGameMap* pMap = MapManager()->QueryMap(idMap);
			if(pMap)
			{
				pMap->DelNpcByType(nType);
				return true;
			}
			else
			{
				PROCESS_ID idProcess = MapManager()->GetProcessID(idMap);
				if(idProcess != PROCESS_NONE)
				{
					CEventPack	evt(OBJ_MAP, idMap, RPC_MAP_DELNPC);
					evt << nType;
					MapGroup(PID)->QueryIntraMsg()->SendRpc(evt);
					return true;
				}
			}
		}
		break;
	case	ACTION_TRAP_CREATE:
		{
			TRAP_INFO	info;
			memset(&info, 0, sizeof(TRAP_INFO));
			int			nData;
			int			nType = 0, nLook = 0, nPosX = 0, nPosY = 0;
			int ret = sscanf(szParam, "%d %d %u %u %d %d %d", &nType, &nLook, &info.idOwner, &info.idMap, &nPosX, &nPosY, &nData);
			info.usType = nType;	info.usLook = nLook;	info.usPosX = nPosX;	info.usPosY = nPosY;
			IF_NOT_(ret == 7)
				return false;

			CMapTrap*	pTarp = CMapTrap::CreateNew(m_idProcess, &info, 0);
			if(pTarp)
			{
				RoleManager()->QuerySet()->AddObj(pTarp->QueryRole());
				return true;
			}
		}
		break;
	case	ACTION_TRAP_ERASE:
		{
			IRole* pRole = pRole_NoUse;
			CHECKF(pRole);
			CMapTrap* pTrap;
			if(pRole->QueryObj(OBJ_TRAP, IPP_OF(pTrap)))
			{
				pTrap->DelTrap();
				return true;
			}
		}
		break;
	default:
		LOGERROR("Unsupported ACTION[%u] type [%u]", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE));
	}

	return false;
}

bool CGameAction::ProcessActionWanted(CActionData* pAction, LPCTSTR szParam, CUser* pUser, IRole* pRole, CItem* pItem, LPCTSTR pszAccept)
{
	CHECKF(pAction);
	if(!pUser)
	{
		LOGERROR("CGameAction::ProcessActionWanted() ActionID=%u, ActionType=%u at %s, %d", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE),
				__FILE__, __LINE__);
		return false;
	}

	switch(pAction->GetInt(ACTIONDATA_TYPE))
	{
	case	ACTION_POLICEWANTED_CHECK:
		{
			if (PoliceWanted().GetWanted(pUser->GetID()) != NULL)
				return true;
		}
		break;

	case	ACTION_POLICEWANTED_NEXT:
		{
			++pUser->TaskIterator();

			if (PoliceWanted().GetWantedByIndex(pUser->TaskIterator()) != NULL)
				return true;
		}
		break;

	case	ACTION_POLICEWANTED_ORDER:
		{
			if (pszAccept)
			{
				int idx = atoi(pszAccept);
				PoliceWantedStruct* pInfo = PoliceWanted().GetWantedByIndex(idx);
				if (pInfo)
				{
					{
						CMsgName msg;
						IF_OK (msg.Create(NAMEACT_POLICEWANTED, pInfo->strName.c_str()))
							pUser->SendMsg(&msg);
					}

					CUser* pTarget = UserManager()->GetUser(pInfo->idUser);
					if (pTarget)
					{
						pUser->PoliceWantedID() = pInfo->idUser;
						pUser->SendSysMsg(_TXTATR_NORMAL, STR_POLICE_WANTED_ORDER, pInfo->strName.c_str(), pUser->GetMap()->GetName());
						return true;
					}
				}
				else
					pUser->SendSysMsg(STR_INVALID_WANTED);
			}
		}
		break;

	case	ACTION_WANTED_MODIFYID:
		{
			if (pszAccept && strlen(pszAccept) > 0 && pUser)
			{
				OBJID id = atoi(pszAccept);
				if (id != ID_NONE)
				{
					if (CWantedList::s_WantedList.GetWanted(id))
					{
						pUser->WantedInfo().idModifyWanted = id;
						return true;
					}
					else
						pUser->SendSysMsg(STR_INVALID_WANTED);
				}
			}
		}
		break;

	case	ACTION_WANTED_SUPERADD:
		{
			if (pszAccept && strlen(pszAccept) > 0 && pUser)
			{
				CWantedData* pData = CWantedList::s_WantedList.GetWanted(pUser->WantedInfo().idModifyWanted);
				if (pData)
				{
					int nAddBonuty = atoi(pszAccept);
					if (nAddBonuty >= _MIN_ADDBONUTY)
					{
						if (pData->GetInt(DATA_BOUNTY)+nAddBonuty > _MAX_BONUTY)
						{
							pUser->SendSysMsg(STR_MAX_BONUTY, _MAX_BONUTY);
							return false;
						}
						else
						{
							if (pUser->SpendMoney(nAddBonuty, true))
							{
								int nNewBonuty = pData->GetInt(DATA_BOUNTY)+nAddBonuty;
								pData->SetInt(DATA_BOUNTY, nNewBonuty);
								pData->Update();

								pUser->SendSysMsg(STR_SUPERADD_BONUTY, pUser->WantedInfo().idModifyWanted, nNewBonuty);
								return true;
							}
							else
								pUser->SendSysMsg(STR_NOT_ENOUGH_MONEY, nAddBonuty);
						}
					}
					else
						pUser->SendSysMsg(STR_LOWEST_SUPERADD_BONUTY, _MIN_ADDBONUTY);
				}
				else
					pUser->SendSysMsg(STR_INVALID_WANTED);
			}
		}
		break;

	case	ACTION_WANTED_CANCEL:
		{
			if (pszAccept && strlen(pszAccept) > 0 && pUser)
			{
				OBJID id = atoi(pszAccept);
				if (id != ID_NONE)
				{
					CWantedData* pData = CWantedList::s_WantedList.GetWanted(id);
					if (pData)
					{
						if (pUser->SpendMoney(pData->GetInt(DATA_BOUNTY)*2, true))
						{
							CWantedList::s_WantedList.DelWanted(id);

							pUser->SendSysMsg(STR_CANCEL_WANTED, id);
							return true;
						}
						else
							pUser->SendSysMsg(STR_NOT_ENOUGH_MONEY, pData->GetInt(DATA_BOUNTY)*2);
					}
					else
						pUser->SendSysMsg(STR_INVALID_WANTED);
				}
			}
		}
		break;

	case	ACTION_WANTED_ORDER:
		{
			if (pszAccept && strlen(pszAccept) > 0 && pUser)
			{
				OBJID id = atoi(pszAccept);
				if (id != ID_NONE)
				{
					CWantedData* pData = CWantedList::s_WantedList.GetWanted(id);
					if (pData)
					{
						if (pUser->SpendMoney(_WANTED_ORDERCOST, true))
						{
							pUser->WantedInfo().idWanted = id;

							{
								CMsgName msg;
								IF_OK (msg.Create(NAMEACT_WANTED, pData->GetStr(DATA_TARGET_NAME)))
									pUser->SendMsg(&msg);
							}

							{
								char szMsg[1024] = "";
								sprintf(szMsg, STR_WANTED_ORDER, _WANTED_ORDERCOST, id, pData->GetStr(DATA_TARGET_NAME), pData->GetInt(DATA_BOUNTY));

								CMsgTalk msg;
								if (msg.Create(SYSTEM_NAME, pUser->GetName(), szMsg, NULL, 0x00ff0000, _TXTATR_NORMAL))
									pUser->SendMsg(&msg);
							}

							CUser* pTarget = UserManager()->GetUser(pData->GetStr(DATA_TARGET_NAME));
							if (pTarget)
							{
								CGameMap* pMap = pTarget->GetMap();
								if (pMap)
								{
									char szMsg[1024] = "";
									sprintf(szMsg, STR_WANTED_INFO, pMap->GetName());

									CMsgTalk msg;
									if (msg.Create(SYSTEM_NAME, pUser->GetName(), szMsg, NULL, 0x00ff0000, _TXTATR_NORMAL))
										pUser->SendMsg(&msg);
								}
							}

							return true;
						}
						else
							pUser->SendSysMsg(STR_NOT_ENOUGH_MONEY, _WANTED_ORDERCOST);
					}
					else
						pUser->SendSysMsg(STR_INVALID_WANTED);
				}
			}
		}
		break;

	case	ACTION_WANTED_NEXT:
		{
			++pUser->TaskIterator();

			if (CWantedList::s_WantedList.GetWantedByIndex(pUser->TaskIterator()) != NULL)
				return true;
		}
		break;

	case	ACTION_WANTED_NAME:
		{
			if (pszAccept && strlen(pszAccept) > 0 && pUser)
			{
				::SafeCopy(pUser->WantedInfo().szName, pszAccept, _MAX_NAMESIZE);
				return true;
			}
		}
		break;

	case	ACTION_WANTED_BONUTY:
		{
			if (pszAccept && strlen(pszAccept) > 0 && pUser)
			{
				int nBonuty = atoi(pszAccept);
				if (nBonuty > _MAX_BONUTY)
				{
					pUser->SendSysMsg(STR_MAX_BONUTY, _MAX_BONUTY);
					return false;
				}
				else
				{
					pUser->WantedInfo().nBonuty = nBonuty;
					return true;
				}
			}
		}
		break;

	case	ACTION_WANTED_NEW:
		{
			if (pUser->WantedInfo().nBonuty >= _MIN_BONUTY)
			{
				if (pUser->SpendMoney(pUser->WantedInfo().nBonuty, SYNCHRO_TRUE))
				{
					if (CWantedList::s_WantedList.AddWanted(pUser->WantedInfo().szName, pUser->GetName(), pUser->WantedInfo().nBonuty))
						return true;
				}
				else
					pUser->SendSysMsg(STR_NOT_ENOUGH_MONEY, pUser->WantedInfo().nBonuty);
			}
			else
				pUser->SendSysMsg(STR_MIN_BONUTY, _MIN_BONUTY);
		}
		break;

	default:
		break;
	}

	return false;
}

bool CGameAction::ProcessActionPkBonus(CActionData* pAction, LPCTSTR szParam, CUser* pUser, IRole* pRole, CItem* pItem, LPCTSTR pszAccept)
{
	if(!pAction)
	{
		LOGERROR("CGameAction::ProcessActionPkBonus() at %s, %d", __FILE__, __LINE__);
		return false;
	}
	if(!pUser)
	{
		LOGERROR("CGameAction::ProcessActionPkBonus() ActionID=%u ActionType=%u at %s, %d", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE),
				__FILE__, __LINE__);
		return false;
	}

	switch(pAction->GetInt(ACTIONDATA_TYPE))
	{
	default:
		LOGERROR("Unsupported ACTION[%u] type [%u]", pAction->GetKey(), pAction->GetInt(ACTIONDATA_TYPE));
	}

	return false;
}
