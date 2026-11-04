#include "AllMsg.h"
#include "basefunc.h"
#include "NpcWorld.h"
#include "SynManager.h"

CMsgSyndicate::CMsgSyndicate()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgSyndicate::~CMsgSyndicate()
{
}

BOOL	CMsgSyndicate::Create		(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;

	if(_MSG_SYNDICATE != this->GetType())
		return false;

	return true;
}

void	CMsgSyndicate::Process		(void* pSocket)
{
	OBJID idSyn = GetNpcID();
	CHECK(idSyn != ID_NONE);
	CSyndicate* pSyn = SynManager()->QuerySyndicate(idSyn);
	if(m_pInfo->usAction == NPCMSG_CREATE_SYN)
	{
		if(pSyn)
		{
			LOGERROR("NPCMSG_CREATE_SYN, idSyn=%u is NOT NULL at %s, %d", idSyn, __FILE__, __LINE__);
			return;
		}
	}
	else if(!( m_pInfo->usAction == SET_WHITE_SYN || m_pInfo->usAction == SET_BLACK_SYN ))
	{
		if(!pSyn)
		{
			LOGERROR("action=%u, idSyn=%u is NULL at %s, %d", m_pInfo->usAction, idSyn, __FILE__, __LINE__);
			return;
		}
	}

	switch(m_pInfo->usAction)
	{
	case	SET_ALLY:
			pSyn->QueryAllySet()->push_back(m_pInfo->idTarget);
		break;
	case	CLEAR_ALLY:
		{
			for(int i = 0; i < (int)pSyn->QueryAllySet()->size(); i++)
			{
				if(pSyn->QueryAllySet()->at(i) == m_pInfo->idTarget)
				{
					pSyn->QueryAllySet()->erase(pSyn->QueryAllySet()->begin() + i);
					break;
				}
			}
		}
		break;
	case	SET_ANTAGONIZE:
		{
			pSyn->QueryEnemySet()->push_back(m_pInfo->idTarget);
		}
		break;
	case	CLEAR_ANTAGONIZE:
		{
			for(int i = 0; i < (int)pSyn->QueryEnemySet()->size(); i++)
			{
				if(pSyn->QueryEnemySet()->at(i) == m_pInfo->idTarget)
				{
					pSyn->QueryEnemySet()->erase(pSyn->QueryEnemySet()->begin() + i);
					break;
				}
			}
		}
		break;
	case	NPCMSG_CREATE_SYN:
		{
			CSyndicate* pSyn = CSyndicate::CreateNew();
			IF_OK(pSyn)
			{
				IF_OK(pSyn->Create(idSyn, m_pInfo->idTarget))
					SynManager()->QuerySynSet()->AddObj(pSyn);
				else
					pSyn->ReleaseByOwner();
			}
		}
		break;
	case	NPCMSG_DESTROY_SYN:
		{
			SynManager()->QuerySynSet()->DelObj(idSyn);
		}
		break;
	default:
		return;
	}
}
