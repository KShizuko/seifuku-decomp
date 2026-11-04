#include "MsgSynMemberInfo.h"
#include "Hero.h"
#include "GameDataSet.h"

CMsgSynMemberInfo::CMsgSynMemberInfo()
{
	Init();
	m_pInfo	=(MSG_Info* )m_bufMsg;
}

CMsgSynMemberInfo::~CMsgSynMemberInfo()
{

}

BOOL CMsgSynMemberInfo::Create(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;

	if(_MSG_SYNMEMBERINFO != this->GetType())
		return false;
	return true;
}
BOOL CMsgSynMemberInfo::Create(char* pszName)
{
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_SYNMEMBERINFO;
	if(!pszName && strlen(pszName) >= _MAX_NAMESIZE)
		return false;
	strcpy(m_pInfo->szName, pszName);
	return true;
}
void CMsgSynMemberInfo::Process(void* pInfo)
{
	SynMemberInfo* pMyInfo = g_objHero.GetReturnSynMemberInfo();

	sprintf(pMyInfo->szProffer, "%d", m_pInfo->nProffer);
	
	int j;
	BOOL bFound = false;
	for(j = 0; j < RANKAMOUNT; j++)
	{
		if(SYNDICATERANKINFOSET[j].dwRank == m_pInfo->ucRank)
		{
			strcpy(pMyInfo->szRank, (char*)GameDataSetQuery()->GetStr(SYNDICATERANKINFOSET[j].idTitle));
			bFound = true;
		}
	}

	if(!bFound)
	{
		for(j = 0; j < RANKAMOUNT; j++)
		{
			if(SYNDICATERANKINFOSET[j].dwRank == RANK_NONE)
			{
				strcpy(pMyInfo->szRank, (char*)GameDataSetQuery()->GetStr(SYNDICATERANKINFOSET[j].idTitle));
			}
		}
	}

	::PostCmd(CMD_OPEN_FLASH_SYN_MEMBER_INFO);
}
