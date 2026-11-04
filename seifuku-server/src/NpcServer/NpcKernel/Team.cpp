#include "allmsg.h"
#include "Team.h"
#include "npcworld.h"
#include "Agent.h"
#include "Sense.h"

MYHEAP_IMPLEMENTATION(CTeam,s_heap)

CTeam::CTeam()
{
}

CTeam::~CTeam()
{

}

CTeam* CTeam::CreateNew()
{
	CTeam* pTeam	= new CTeam();
	return pTeam;
}

BOOL CTeam::Create(CAgent* pAgent)
{
	this->Init();
	m_pOwner	= pAgent;
	return true;
}

BOOL CTeam::Apply(OBJID idUser)
{
	CSense* pSense = Cast<CSense>(m_pOwner);
	if(pSense)
		pSense->TeamApply(idUser);

	return true;
}

BOOL CTeam::DelMember(OBJID idMember)
{
	for(int i = 0; i < (int)m_setMember.size(); i++)
	{
		if(m_setMember[i].id == idMember)
		{
			m_setMember.erase(m_setMember.begin() + i);
			return true;
		}
	}

	return false;
}

void CTeam::Init()
{
	m_setMember.clear();
}

OBJID CTeam::GetLeaderID()
{
	CHECKF(m_setMember.size());
	return m_setMember[0].id;
}

int CTeam::GetMemberAmount()
{
	return __min(_MAX_TEAMAMOUNT, m_setMember.size());
}

bool CTeam::AddMember(int nAmount, const TeamMemberInfo *setInfo)
{
	CHECKF(nAmount <= _MAX_TEAMAMOUNT);

	for(int i = 0; i < nAmount; i++)
	{
		m_setMember.push_back(setInfo[i]);
	}

	return true;
}
