#include "windows.h"
#include "define.h"
#include "Enemy.h"
#include "i_mydb.h"
#include "User.h"
#include "AllMsg.h"
#include "MapGroup.h"

MYHEAP_IMPLEMENTATION(CEnemy,s_heap)

CEnemy::CEnemy()
{
}

CEnemy::~CEnemy()
{
}
bool CEnemy::Create(PROCESS_ID idProcess, CUser *pOwner, IDatabase* pDb)
{
	m_idProcess	= idProcess;
	m_pOwner	= pOwner;

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT * FROM cq_enemy WHERE userid=%d ORDER BY time", m_pOwner->GetID());
	IRecordset*	pRes = pDb->CreateNewRecordset(szSQL, false);
	if(pRes)
	{
		for(int i = 0; i < pRes->RecordCount(); i++, pRes->MoveNext())
		{
			NAMESTR	szName;
			pRes->LoadString(szName, "enemyname", _MAX_NAMESIZE);
			Add(pRes->LoadDWord("enemy"), szName, SYNCHRO_FALSE, UPDATE_FALSE);
		}
		pRes->Release();
	}
	return true;
}
bool CEnemy::Create(PROCESS_ID idProcess, CUser *pOwner)
{
	m_idProcess	= idProcess;
	m_pOwner	= pOwner;
	return true;
}
bool CEnemy::GetInfo(CEnemyInfoStruct* pInfo)
{
	CHECKF(pInfo);
	memset(pInfo, 0, sizeof(CEnemyInfoStruct));
	for(int i = 0; i < MAX_ENEMYSIZE && i< m_set.Size(); i++)
	{
		pInfo->setEnemy[i] = m_set[i].first;
		SafeCopy(pInfo->setName[i], m_set[i].second, _MAX_NAMESIZE);
	}
	return true;
}
bool CEnemy::AppendInfo(const CEnemyInfoStruct* pInfo)
{
	CHECKF(pInfo);

	for(int i = 0; i < MAX_ENEMYSIZE; i++)
	{
		if(pInfo->setEnemy[i])
			m_set.Push(make_pair(pInfo->setEnemy[i], String(pInfo->setName[i])));
	}
	return true;
}

void CEnemy::Add(OBJID idEnemy, LPCTSTR szName, bool bSynchro, bool bUpdate)
{
	bool	bInsertNew = true;
	ENEMY_SET::Iterator pEnemy = m_set.NewEnum();
	while(pEnemy.Next())
	{
		if(pEnemy && pEnemy->first == idEnemy)
		{
			m_set.DelObj(pEnemy);
			bInsertNew	= false;
		}
	}
	m_set.Push(make_pair(idEnemy, String(szName)));

	if(bSynchro)
	{
		CUser* pEnemyUser = UserManager()->GetUser(idEnemy);
		if(pEnemyUser)
		{
			CMsgFriend	msg;
			if(msg.Create(_ENEMY_ADD, idEnemy, pEnemyUser->GetPeerageRank(), pEnemyUser->GetSex(), szName, true))
				m_pOwner->SendMsg(&msg);
		}
	}

	if(bUpdate)
	{
		if(bInsertNew)
		{
			SQLBUF	szSQL;
			sprintf(szSQL, "INSERT cq_enemy VALUES(0, %u, %u, '%s', %u)", m_pOwner->GetID(), idEnemy, szName, time(NULL));
			Database()->ExecuteSQL(szSQL);
		}
		else
		{
			SQLBUF	szSQL;
			sprintf(szSQL, "UPDATE cq_enemy SET time=%u WHERE userid=%u && enemy=%u LIMIT 1", time(NULL), m_pOwner->GetID(), idEnemy);
			Database()->ExecuteSQL(szSQL);
		}
	}

	if(m_set.Size() > MAX_ENEMYSIZE)
		Del(m_set[0].first, UPDATE_TRUE);
}

void CEnemy::Del(OBJID idEnemy, bool bUpdate)
{
	ENEMY_SET::Iterator pEnemy = m_set.NewEnum();
	while(pEnemy.Next())
	{
		if(pEnemy && pEnemy->first == idEnemy)
		{
			m_set.DelObj(pEnemy);

			{
				CMsgFriend	msg;
				BOOL	ret = msg.Create(_ENEMY_DEL, idEnemy, 0, 0, "");
				IF_OK_(ret)
					m_pOwner->SendMsg(&msg);
			}

			if(bUpdate)
			{
				SQLBUF	szSQL;
				sprintf(szSQL, "DELETE FROM cq_enemy WHERE userid=%u && enemy=%u LIMIT 1", m_pOwner->GetID(), idEnemy);
				Database()->ExecuteSQL(szSQL);
			}

			return ;
		}
	}
}

LPCTSTR CEnemy::GetName(OBJID idEnemy)
{
	ENEMY_SET::Iterator pEnemy = m_set.NewEnum();
	while(pEnemy.Next())
	{
		if(pEnemy && pEnemy->first == idEnemy)
			return pEnemy->second;
	}
	return NULL;
}
void CEnemy::SendToClient()
{
	ENEMY_SET::Iterator pEnemy = m_set.NewEnum();
	while(pEnemy.Next())
	{
		OBJID	idEnemy	= pEnemy->first;
		CUser*	pEnemyUser	= UserManager()->GetUser(idEnemy);
		bool	bOnline = (pEnemyUser != NULL);
		int		nPeerage	= 0;
		int		nSex		= 0;
		if(pEnemyUser)
		{
			nPeerage	= pEnemyUser->GetPeerageRank();
			nSex		= pEnemyUser->GetSex();
		}

		CMsgFriend	msg;
		if(msg.Create(_ENEMY_ADD, idEnemy, nPeerage, nSex, pEnemy->second, bOnline))
			m_pOwner->SendMsg(&msg);

		if(!bOnline)
			MapGroup(PID)->QueryIntraMsg()->TransmitMsg(&msg, m_pOwner->GetSocketID(), m_pOwner->GetID());
	}

	pEnemy.Reset();
	while(pEnemy.Next())
	{
		OBJID	idFriend	= pEnemy->first;
		CMsgFriend	msg;
		IF_OK(msg.Create(_ENEMY_ONLINE, m_pOwner->GetID(), m_pOwner->GetPeerageRank(), m_pOwner->GetSex(), m_pOwner->GetName()))
			MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(&msg, idFriend);
	}
}
