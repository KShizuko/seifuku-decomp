#include "AllMsg.h"
#include "MapTrap.h"
#include "MapGroup.h"

MYHEAP_IMPLEMENTATION(CMapTrap,s_heap)
CMapTrap::CMapTrap()
{
	SetObjType(OBJ_TRAP);

	memset(&m_info, 0, sizeof(m_info));
	m_bDie			= false;
	m_nTimes		= 0;

	m_pType			= NULL;
	m_pBattleSystem	= NULL;
	m_setStatus		= NULL;
	m_pMap			= NULL;
}

CMapTrap::~CMapTrap()
{
	if (m_pBattleSystem)
		delete m_pBattleSystem;

	SAFE_RELEASE (m_setStatus);

	if(m_pMap)
		m_pMap->LeaveRoom(QueryMapThing());
	if(IsDynaTrapID(GetID()))
		MapManager()->RecycleTrapID(GetID());
}

bool CMapTrap::Init()
{
	m_nTimes	= m_pType->GetInt(TRAPTYPEDATA_TIMES);

	m_setStatus	= CStatusSet::CreateNew(true);
	CHECKF(m_setStatus);

	m_pBattleSystem	= new CBattleSystem(this->m_idProcess, QueryRole());
	CHECKF(m_pBattleSystem);

	m_pMap	= MapManager()->QueryMap(m_info.idMap);
	CHECKF(m_pMap);
	m_pMap->EnterRoom(QueryMapThing());

	return true;
}

CMapTrap* CMapTrap::CreateNew(PROCESS_ID idProcess, TRAP_INFO* pInfo, CUser* pUser )
{
	CMapTrap*	pTrap	= new CMapTrap;
	CHECKF(pTrap);
	pTrap->m_idProcess	= idProcess;

	pTrap->m_pType	= TrapTypeSet()->GetObj(pInfo->usType);
	CHECKF(pTrap->m_pType);

	memcpy(&pTrap->m_info, pInfo, sizeof(pTrap->m_info));
	ASSERT(pTrap->m_info.id == ID_NONE);
	pTrap->m_info.id	= MapGroup(idProcess)->GetMapManager()->SpawnNewTrapID();
	pTrap->m_info.usLook	= ::MakeLookFace(::MaskLook(pTrap->m_pType->GetInt(TRAPTYPEDATA_LOOK)), ::MaskDir(pTrap->m_info.usLook));

	CHECKF(pTrap->Init());
	if(pUser)
		pTrap->m_pUser	= pUser->QueryLink();

	CMsgMapItem	msg;
	IF_OK(msg.Create(MSGMAPITEM_CAST_TRAP, pTrap))
		pTrap->BroadcastRoomMsg(&msg);

	return pTrap;
}

CMapTrap* CMapTrap::CreateNew(PROCESS_ID idProcess, IRecord* pRes)
{
	CMapTrap*	pTrap	= new CMapTrap;
	CHECKF(pTrap);
	pTrap->m_idProcess	= idProcess;

	if(!MapGroup(idProcess)->GetMapManager()->GetGameMap(pRes->GetInt(TRAPDATA_MAPID)))
	{
		pTrap->ReleaseByOwner();
		return NULL;
	}

	pTrap->m_pType	= TrapTypeSet()->GetObj(pRes->GetInt(TRAPDATA_TYPE));
	if(!pTrap->m_pType)
	{
		LOGERROR("invalid trap type %d in %d", pRes->GetInt(TRAPDATA_TYPE), pRes->GetInt(TRAPDATA_ID_));
		return NULL;
	}

	pTrap->m_info.id			= pRes->GetInt(TRAPDATA_ID_);
	pTrap->m_info.usType		= pRes->GetInt(TRAPDATA_TYPE);
	pTrap->m_info.usLook		= pRes->GetInt(TRAPDATA_LOOK);
	pTrap->m_info.idOwner		= pRes->GetInt(TRAPDATA_OWNERID);
	pTrap->m_info.idMap			= pRes->GetInt(TRAPDATA_MAPID);
	pTrap->m_info.usPosX		= pRes->GetInt(TRAPDATA_POS_X);
	pTrap->m_info.usPosY		= pRes->GetInt(TRAPDATA_POS_Y);

	CHECKF(pTrap->Init());
	return pTrap;
}

bool CMapTrap::ChangeType(int nNewType)
{
	m_pType	= TrapTypeSet()->GetObj(nNewType);
	CHECKF(m_pType);

	m_info.usType	= nNewType;

	CMsgMapItem	msg;
	IF_OK(msg.Create(MSGMAPITEM_CAST_TRAP, this))
		BroadcastRoomMsg(&msg);

	return true;
}

bool CMapTrap::ChangeLook(int nNewLook)
{
	m_info.usLook	= nNewLook;

	CMsgMapItem	msg;
	IF_OK(msg.Create(MSGMAPITEM_CAST_TRAP, this))
		BroadcastRoomMsg(&msg);

	return true;
}

void CMapTrap::SendShow(IRole* pRole)
{
	CMsgMapItem	msg;
	IF_OK(msg.Create(MSGMAPITEM_SYNCHRO_TRAP, this))
		pRole->SendMsg(&msg);
}

void CMapTrap::BroadcastRoomMsg(CNetMsg* pMsg, bool bSendSelf )
{
	CHECK(GetMap());
	GetMap()->BroadcastBlockMsg(QueryMapThing(), pMsg, bSendSelf);
}

void CMapTrap::BroadcastRoomMsg(LPCTSTR szMsg, bool bSendSelf )
{
	CMsgTalk	msg;
	if(msg.Create(GetName(), ALLUSERS_NAME, szMsg, NULL, 0xff0000, _TXTATR_SYSTEM))
		this->BroadcastRoomMsg(&msg, bSendSelf);
}

void CMapTrap::OnTimer(time_t tCurr)
{
	if(!IsAlive())
		return ;

	if(IsAutoSort())
	{
		DEBUG_TRY
		if ( m_tFight.ToNextTick(GetInterAtkRate()))
		{
			MagicAttack();

			if(m_pType->GetInt(TRAPTYPEDATA_TIMES))
			{
				if(m_nTimes <= 0)
					DelTrap();
				else
					--m_nTimes;
			}
		}
		DEBUG_CATCH("CMapTrap ProcAttack_Hand2Hand");
	}
}

bool CMapTrap::DelTrap()
{
	if(IsDeleted())
		return false;

	CMsgMapItem	msg;
	IF_OK(msg.Create(MSGMAPITEM_DROP_TRAP, this))
		BroadcastRoomMsg(&msg);

	m_bDie	= true;
	return true;
}

void CMapTrap::TrapAttack(IRole* pTarget)
{
	CHECK(IsTrapSort());

	if(!m_tFight.ToNextTick(GetInterAtkRate()))
		return ;

	if(!pTarget->IsBeAtkable())
		return ;
	if(m_pUser && m_pUser->IsImmunity(pTarget))
		return ;

	int nAtkMode = m_pType->GetInt(TRAPTYPEDATA_ATKMODE);
	void* pTemp;
	if(!( (nAtkMode & ATKMODE_USER) && pTarget->QueryObj(OBJ_USER, &pTemp)
			|| (nAtkMode & ATKMODE_MONSTER) && pTarget->QueryObj(OBJ_MONSTER, &pTemp) ))
		return ;

	if(m_pUser && !pTarget->IsEvil())
		m_pUser->SetCrimeStatus(STATUS_CRIME, 15);

	if((nAtkMode & ATKMODE_NOATTACK) == 0)
	{
		m_pBattleSystem->CreateBattle(pTarget->GetID());
		m_pBattleSystem->ProcAttack_Hand2Hand();
	}

	OBJID idAction = m_pType->GetInt(TRAPTYPEDATA_ACTION);
	if(idAction != ID_NONE)
	{
		CUser* pUser=NULL;
		pTarget->QueryObj(OBJ_USER, IPP_OF(pUser));
		GameAction()->ProcessAction(idAction, pUser, QueryRole());
	}
	if(m_pType->GetInt(TRAPTYPEDATA_TIMES))
	{
		if(m_nTimes <= 0)
			DelTrap();
		else
			--m_nTimes;
	}
}

void CMapTrap::MagicAttack	()
{
	CHECK(IsAutoSort());

	switch(m_pType->GetInt(TRAPTYPEDATA_SORT))
	{
	case	TRAPSORT_FIREWALL:
		{
			ProcessFireWall();
		}
		break;
	}
}

bool CMapTrap::IsAtkable(IRole* pTarget, bool bSendHint)
{
	if (!pTarget)
		return false;

	if (!this->IsAlive())
	{
		if (bSendHint)
			this->SendSysMsg(STR_DIE_FORGIVE);
		return false;
	}

	if (!pTarget->IsAlive())
	{
		return false;
	}

	if(GetID() == pTarget->GetID())
		return false;

	if(!pTarget->IsBeAtkable())
		return false;

	if (!(this->GetMap() && pTarget->GetMap() && this->GetMap() == pTarget->GetMap()))
		return false;

	return true;
}

int	 CMapTrap::Attack(IRole* pTarget)
{
	CHECKF(pTarget);

	int nPower;
	if (IsSimpleMagicAtk())
		nPower = CMagic::CalcMagicPower(QueryRole(), pTarget, GetAttack());
	else
		nPower = CBattleSystem::CalcAttackPower(QueryRole(), pTarget, false, 0, 0, false);

	int nDamage = pTarget->BeAttack(QueryRole(), nPower, true);
	int nLoseLife = ::CutOverflow(nDamage, (int)pTarget->GetLife());
	if (nLoseLife > 0)
		pTarget->AddAttrib(_USERATTRIB_LIFE, -1*nLoseLife, SYNCHRO_TRUE, NULL);

	return nDamage;
}

void CMapTrap::SendDamageMsg(OBJID idTarget, int nDamage)
{
	if(IsSimpleMagicAtk())
	{
		CMsgMagicEffect	msg;
		IF_OK(msg.Create(GetID(), m_pType->GetInt(TRAPTYPEDATA_MAGICTYPE), 0, idTarget, nDamage))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}
	else if(IsBowman())
	{
		CMsgInteract msg;
		IF_OK(msg.Create(INTERACT_SHOOT, GetID(), idTarget, GetPosX(), GetPosY(), nDamage))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}
	else
	{
		CMsgInteract msg;
		IF_OK(msg.Create(INTERACT_ATTACK, GetID(), idTarget, GetPosX(), GetPosY(), nDamage))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}
}

void CMapTrap::Kill(IRole* pTarget, DWORD dwDieWay, int nReserved)
{
	CHECK(pTarget);

	if(m_pUser)
	{
		m_pUser->Kill(pTarget, dwDieWay, 0);
		return ;
	}

	CMsgInteract msg;
	IF_OK(msg.Create(INTERACT_KILL, this->GetID(), pTarget->GetID(), pTarget->GetPosX(), pTarget->GetPosY(), dwDieWay))
		pTarget->BroadcastRoomMsg(&msg, INCLUDE_SELF);

	pTarget->BeKill(this->QueryRole(), 1);
}

void CMapTrap::ProcessFireWall()
{
	int	 nRange		= m_pType->GetInt(TRAPTYPEDATA_SIZE) / 2;
	bool bVertical	= ((m_info.usLook%4) / 2) == 0;

	int x,y,z;
	CGameMap* pMap = GetMap();
	FOR_9_BLOCKTHINGS(pMap, GetPosX(), GetPosY())
	{
		IMapThing* pTarget = pMap->QueryThingByIndex(x,y,z);
		void* pTemp;
		IRole* pRole = NULL;
		if(pTarget && GetDistance(pTarget) < nRange && pTarget->QueryRole(IPP_OF(pRole))
				&& !pTarget->QueryObj(OBJ_TRAP, &pTemp) && !pTarget->QueryObj(OBJ_NPC, &pTemp))
		{
			if(!pRole->IsBeAtkable())
				continue;
			if(m_pUser && m_pUser->IsImmunity(pRole))
				continue;

			if(!bVertical && pTarget->GetPosX() == GetPosX()
					|| bVertical && pTarget->GetPosY() == GetPosY() )
			{
				if(m_pUser && !pRole->IsEvil())
					m_pUser->SetCrimeStatus(STATUS_CRIME, 15);

				m_pBattleSystem->CreateBattle(pTarget->GetID());
				m_pBattleSystem->ProcAttack_Hand2Hand();
			}
		}
	}
}
