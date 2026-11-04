#include "Friend.h"
#include "Hero.h"
#include "GamePlayerSet.h"
#include "GameDataSet.h"

CFriend::CFriend()
{
	m_idFriend	=	ID_NONE;
	strcpy(m_szName, "");
	m_bFlagOnline	=	false;
	m_bFlagNewMsg = false;
}

CFriend::~CFriend()
{
}
CFriend* CFriend::CreateNew(OBJID id, char* pszName, BOOL m_bFlagOnline)
{
	CFriend* pFriend = new(CFriend);
	assert(pFriend);

	pFriend->SetID(id);
	if(pszName)
		pFriend->SetName(pszName);
	if(m_bFlagOnline)
		pFriend->Online();
	else
		pFriend->Offline();
	return pFriend;
}
char* CFriend::GetProfessionalName()
{
	return g_objHero.GetProfessionalName(this->GetProfessional());
}
char* CFriend::GetSyndicateName()
{
	return g_objPlayerSet.GetSyndicateName(m_IDSyndicate);
}
char* CFriend::GetRankName()
{
	for(int i = 0; i < RANKAMOUNT; i++)
	{
		InfoSyndicateRank infoRank = SYNDICATERANKINFOSET[i];
		if(infoRank.dwRank == m_dwRank)
		{
			return (char*)GameDataSetQuery()->GetStr(infoRank.idTitle);
		}
	}
	return (char*)GameDataSetQuery()->GetStr(SYNDICATERANKINFOSET[RANKAMOUNT-1].idTitle);
}
