#include "Enemy.h"
#include "Hero.h"
#include "GamePlayerSet.h"
#include "GameDataSet.h"

CEnemy::CEnemy()
{
	m_idEnemy	=	ID_NONE;
	strcpy(m_szName, "");
	m_bFlagOnline	=	false;

}

CEnemy::~CEnemy()
{

}

CEnemy* CEnemy::CreateNew(OBJID id, char* pszName, BOOL m_bFlagOnline)
{
	CEnemy* pEnemy = new(CEnemy);
	assert(pEnemy);
	
	pEnemy->SetID(id);
	if(pszName)
		pEnemy->SetName(pszName);
	if(m_bFlagOnline)
		pEnemy->Online();
	else
		pEnemy->Offline();
	
	return pEnemy;
}
char* CEnemy::GetProfessionalName()
{
	return g_objHero.GetProfessionalName(this->GetProfessional());
}
char* CEnemy::GetSyndicateName()
{
	return g_objPlayerSet.GetSyndicateName(m_IDSyndicate);
}
char* CEnemy::GetRankName()
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
