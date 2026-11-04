#include "AllMsg.h"
#include "GameMap.h"
#include "AINpc.h"
#include "MapGroup.h"
#include "MapItem.h"
#include "basefunc.h"
#include "ActionDefine.h"
#include "MapTrap.h"
#include "UserTable.h"
#include "Item.h"
#include "DeadLoop.h"

const int	RECRUITMAGIC_SECS					= 10;
const int	DEFAULT_MONSTER_MAGIC_LEVEL			= 0;
const int	UNIDENT_PERCENT						= 20;

#define EXPLODEMONEY_RATE	4
#define EXPLODEMONEY_DOUBLE	15
MYHEAP_IMPLEMENTATION(CMonster,s_heap)
char	szPetTable[] = _TBL_PET;

CMonster::CMonster()
{
	SetObjType(OBJ_MONSTER);
	m_pMap			= NULL;		m_setStatus		= NULL;
	m_pType			= NULL;
	m_idNpc			= ID_NONE;
	m_pBattleSystem	= NULL;
	m_pData			= NULL;

	m_tFootPrint	= 0;
	m_posFootPrint.x	= 0;
	m_posFootPrint.y	= 0;
	m_link.Init(this);
	m_pMagicType	= NULL;
}
CMonster::~CMonster()
{
	if(m_pData)
	{
		if(IsSynPet())
			MapManager()->RecyclePetID(GetID());
		else if(IsCallPet())
			MapManager()->RecycleCallPetID(GetID());
		if(m_pData->GetInt(PETDATA_LIFE) > 0)
			SaveInfo();
		else
			m_pData->DeleteRecord();
		m_pData->Release();
	}
	if(m_idNpc != ID_NONE)
	{
		LeaveMapGroup();
		m_idNpc = ID_NONE;
	}
	if (m_pBattleSystem)
	{
		delete m_pBattleSystem;
		m_pBattleSystem	= NULL;
	}
	if(m_setStatus)	m_setStatus->Release();
}
bool CMonster::Create(PROCESS_ID idProcess, IRecordset* pRes)
{
	m_idProcess		= idProcess;

	MYASSERT(!m_pData);

	m_pData = CPetData::CreateNew();
	CHECKF(m_pData);
	IF_NOT(m_pData->Create(pRes))
		return false;

	OBJID idType = m_pData->GetInt(PETDATA_TYPE);
	m_pType			= MonsterType()->GetObj(idType);
	if(!m_pType)
	{
		LOGERROR("Bad monster type [%d]", idType);
		return false;
	}

	m_nDir			= ::RandGet(8);
	m_pMap			= MapManager()->GetGameMap(m_pData->GetInt(PETDATA_MAPID), false);
	if(!m_pMap)
	{
		m_pData->Release();
		m_pData = NULL;
		return false;
	}

	m_idNpc			= MapManager()->SpawnNewPetID();
	m_nPosX			= m_pData->GetInt(PETDATA_RECORD_X);
	m_nPosY			= m_pData->GetInt(PETDATA_RECORD_Y);
	m_dwStatus		= STATUS_NORMAL;
	m_dwEffect		= KEEPEFFECT_NORMAL;
	if((m_pType->GetInt(NPCTYPEDATA_ATKUSER) & ATKUSER_WING))
		m_dwEffect		= KEEPEFFECT_WING;
	m_nPose			= POSE_STAND;
	m_idGen			= m_pData->GetInt(PETDATA_GENID);

	m_nCurrLife		= m_pData->GetInt(PETDATA_LIFE);
	m_nCurrMana		= m_pData->GetInt(PETDATA_MANA);
	m_tFight.SetInterval(m_pType->GetInt(NPCTYPEDATA_ATKSPEED));
	m_tFight.Update();

	m_pBattleSystem	= new CBattleSystem(this->m_idProcess, this);
	CHECKF(m_pBattleSystem);
	m_setStatus = CStatusSet::CreateNew(true);
	CHECKF(m_setStatus);

	ST_CREATENEWNPC	info;
	memset(&info, 0, sizeof(ST_CREATENEWNPC));
	info.id				= GetID();
	info.usAction		= MSGAINPCINFO_CREATENEW;
	info.usType			= GetType();
	info.ucOwnerType	= m_pData->GetInt(PETDATA_OWNERTYPE);
	info.idOwner		= m_pData->GetInt(PETDATA_OWNERID);
	info.idMap			= GetMap()->GetID();
	info.usPosX			= GetPosX();
	info.usPosY			= GetPosY();
	info.idData			= m_pData->GetInt(PETDATA_GENID);
	info.nData			= GetData();
	CMsgMonsterInfo	msg;
	IF_OK(msg.Create(&info))
		this->SendMsg(&msg);

	return true;
}

bool CMonster::Create(PROCESS_ID idProcess, CNpcType* pType, const ST_CREATENEWNPC* pInfo, CUser* pUser)
{
	CHECKF(pType && pInfo && pUser);

	m_idProcess		= idProcess;
	CGameMap* pMap = MapManager()->GetGameMap(pInfo->idMap);
	if(pMap && !pMap->IsStandEnable(pInfo->usPosX, pInfo->usPosY))
	{
		pUser->SendSysMsg(STR_INVALID_COORDINATE);
		return false;
	}

	IF_OK(Create(idProcess, pType, pInfo, pUser->GetName()))
	{
		CHECKF(IsCallPet());

		if(IsMagicAtk())
		{
			m_pMagicType	= CMagic::FindMagicType(GetMagicType(), DEFAULT_MONSTER_MAGIC_LEVEL);
			IF_NOT(m_pMagicType)
			{
				LOGERROR("Can't find magic type [%d] in monster type [%d]", GetMagicType(), pType->GetID());
				return false;
			}
		}

		CMsgCallPetInfo	msg;
		IF_OK(msg.Create(GetID(), GetName(), GetLookFace(), pType->GetInt(NPCTYPEDATA_AITYPE), GetPosX(), GetPosY()))
			pUser->SendMsg(&msg);

		m_pOwner		= pUser->QueryLink();
		return true;
	}
	return false;
}

bool CMonster::Create(PROCESS_ID idProcess, CNpcType* pType, const ST_CREATENEWNPC* pInfo, LPCTSTR pszName )
{
	m_idProcess		= idProcess;

	CHECKF(pType);

	m_idNpc			= pInfo->id;
	m_nDir			= ::RandGet(8);
	m_pMap			= MapManager()->GetGameMap(pInfo->idMap);
	m_nPosX			= pInfo->usPosX;
	m_nPosY			= pInfo->usPosY;
	m_dwStatus		= STATUS_NORMAL;
	m_dwEffect		= KEEPEFFECT_NORMAL;
	m_nPose			= POSE_STAND;
	m_idGen			= pInfo->idData;

	m_pType			= pType;

	m_nCurrLife		= m_pType->GetInt(NPCTYPEDATA_LIFE);
	m_nCurrMana		= m_pType->GetInt(NPCTYPEDATA_MANA);
	m_tFight.SetInterval(m_pType->GetInt(NPCTYPEDATA_ATKSPEED));
	m_tFight.Update();

	if(!m_pMap)
		return false;

	m_pBattleSystem	= new CBattleSystem(this->m_idProcess, this);
	CHECKF(m_pBattleSystem);
	m_setStatus = CStatusSet::CreateNew(true);
	CHECKF(m_setStatus);

	if(!IsCallPet())
	{
		CMsgMonsterInfo	msg;
		IF_OK(msg.Create(pInfo))
			this->SendMsg(&msg);
	}

	if(!IsSynPet()
		|| pInfo->idOwner == ID_NONE
		|| pInfo->ucOwnerType == 0)
		return true;

	m_pData = CPetData::CreateNew();
	IF_OK(m_pData)
	{
		IF_OK(m_pData->Create(GameDataDefault()->GetPetData(), ID_NONE))
		{
			m_pData->SetInt(PETDATA_OWNERID, pInfo->idOwner);
			m_pData->SetInt(PETDATA_OWNERTYPE, pInfo->ucOwnerType);
			m_pData->SetInt(PETDATA_GENID, pInfo->idData);
			m_pData->SetInt(PETDATA_TYPE, pInfo->usType);
			if(pszName)
				m_pData->SetStr(PETDATA_NAME, pszName, _MAX_NAMESIZE);
			else
				m_pData->SetStr(PETDATA_NAME, pType->GetStr(NPCTYPEDATA_NAME), _MAX_NAMESIZE);
			m_pData->SetInt(PETDATA_LIFE, m_nCurrLife);
			m_pData->SetInt(PETDATA_MANA, m_nCurrMana);
			m_pData->SetInt(PETDATA_MAPID, pInfo->idMap);
			m_pData->SetInt(PETDATA_RECORD_X, pInfo->usPosX);
			m_pData->SetInt(PETDATA_RECORD_Y, pInfo->usPosY);
			m_pData->SetInt(PETDATA_DATA, pInfo->nData);
			OBJID idPet = m_pData->InsertRecord();

			return idPet != ID_NONE;
		}
		else
		{
			SAFE_RELEASE(m_pData);
		}
	}
	return false;
}
void CMonster::OnTimer(time_t tCurr)
{
	if(!this->IsAlive() && !m_bLeaveMap)
	{
		if ((m_tDie.GetRemain() < NPCDIEDELAY_SECS/2)
			&& !(m_dwStatus & STATUS_DISAPPEARING))
		{
			this->SetStatus(STATUS_DISAPPEARING);
		}
	}
	if(IsDeleted())
		return ;

	DEBUG_TRY
	if (IsAlive() && m_pType->GetInt(NPCTYPEDATA_MAGIC_TYPE) == SIMPLEMAGIC_RECRUIT && m_tAddLife.IsActive() && m_tAddLife.ToNextTime(RECRUITMAGIC_SECS))
	{
		if(GetLife() < GetMaxLife())
		{
			int nAddLife = GetMaxLife() * GetRecruitMagicPercent() / 100;
			int nLoseLife = ::CutRange((int)GetMaxLife() - (int)GetLife(), (int)0, (int)GetMaxLife());
			nAddLife = ::CutOverflow(nAddLife, nLoseLife);
			AddAttrib(_USERATTRIB_LIFE, nAddLife, SYNCHRO_TRUE, NULL);
			CMsgMagicEffect	msg;
			IF_OK(msg.Create(GetID(), SIMPLEMAGIC_RECRUIT, 0, GetID(), nAddLife))
				BroadcastRoomMsg(&msg, EXCLUDE_SELF);

			m_nFightPause	+= 1000;
		}
		else
		{
			m_tAddLife.Clear();
		}
	}
	DEBUG_CATCH("CMonster add life");

	DEBUG_TRY
	if (m_pBattleSystem && m_pBattleSystem->IsActived()
		&& m_tFight.ToNextTick(GetInterAtkRate() + m_nFightPause))
	{
		m_pBattleSystem->ProcAttack_Hand2Hand();
		m_nFightPause	= 0;
	}
	DEBUG_CATCH("CMonster ProcAttack_Hand2Hand");
	DEBUG_TRY
	for(int i = QueryStatusSet()->GetAmount()-1; i >= 0; i--)
	{
		IStatus* pStatus = QueryStatusSet()->GetObjByIndex(i);
		if(pStatus)
		{
			pStatus->OnTimer(tCurr);
			if(!pStatus->IsValid())
				QueryStatusSet()->DelObj(pStatus->GetID());
		}
	}
	DEBUG_CATCH("@monster magic poison@")

	DEBUG_TRY
	if(IsCallPet() && m_pOwner == NULL)
	{
		if(!IsDeleted())
		{
			LOGERROR("Call pet don't find owner! del self.");
			DelMonster(DEL_NOW);
		}
	}
	DEBUG_CATCH("@RoleManager()->QuerySet()->DelObj@")
}

void CMonster::ClearAllStatus()
{
	for(int i = QueryStatusSet()->GetAmount() - 1; i >= 0; i--)
	{
		IStatus* pStatus = QueryStatusSet()->GetObjByIndex(i);
		if(pStatus)
			QueryStatusSet()->DelObj(pStatus->GetID());
	}
}

int	 CMonster::AdjustData(int nData, int nAdjust, int nMaxData )
{
	if(nAdjust>=ADJUST_PERCENT)
		return MulDiv(nData, nAdjust-ADJUST_PERCENT, 100);

	if(nAdjust<=ADJUST_SET)
		return -1*nAdjust + ADJUST_SET;

	if(nAdjust==ADJUST_FULL)
	{
		MYASSERT(nMaxData != 0);
		return nMaxData;
	}

	return nData + nAdjust;
}

bool CMonster::SendMsg(CNetMsg* pMsg)
{
	if (this->IsCallPet())
		return true;
	else
		return MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(m_idNpc, pMsg);
}

void CMonster::BroadcastRoomMsg(CNetMsg* pMsg, bool bSendSelf )
{
	CGameMap* pMap = GetMap();
	if(pMap)
		pMap->BroadcastBlockMsg(QueryMapThing(), pMsg, bSendSelf);
}

void CMonster::BroadcastRoomMsg(LPCTSTR szMsg, bool bSendSelf )
{
	CMsgTalk	msg;
	if(msg.Create(GetName(), ALLUSERS_NAME, szMsg, NULL, 0xff0000, _TXTATR_SYSTEM))
		this->BroadcastRoomMsg(&msg, bSendSelf);
}

void CMonster::BroadcastMapMsg(CNetMsg* pMsg, bool bSendSelf )
{
	CMapPtr pMap = this->GetMap();
	if(pMap)
		UserManager()->BroadcastMapMsg(pMap->GetID(), pMsg);
	if(bSendSelf)
		MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(ID_NONE, pMsg);
}

void CMonster::BroadcastMapMsg(LPCTSTR szMsg, bool bSendSelf )
{
	CMsgTalk	msg;
	if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szMsg, NULL, 0xff0000, _TXTATR_SYSTEM))
		this->BroadcastMapMsg(&msg, bSendSelf);
}

bool CMonster::MoveToward(int nDir)
{
	m_nDir	= nDir%8;
	int	nDirX	= _DELTA_X[m_nDir];
	int	nDirY	= _DELTA_Y[m_nDir];

	CMapPtr pMap = this->GetMap();
	CHECKF(pMap);

	int nNewPosX = GetPosX() + nDirX;
	int nNewPosY = GetPosY() + nDirY;
	IF_NOT(GetMap()->IsValidPoint(nNewPosX, nNewPosY))
		return false;

	pMap->MoveTo(this, m_nPosX + nDirX, m_nPosY + nDirY);
	m_nPosX	+= nDirX;
	m_nPosY	+= nDirY;

	int x,y,z;
	CUser* pUser = NULL;
	FOR_9_BLOCKTHINGS(pMap, GetPosX(), GetPosY())
	{
		IMapThing* pTarget = pMap->QueryThingByIndex(x,y,z);
		if(pTarget && pTarget->QueryObj(OBJ_USER, IPP_OF(pUser)))
		{
			int	nDistX	= (pTarget->GetPosX() - this->GetPosX()) * nDirX;
			int	nDistY	= (pTarget->GetPosY() - this->GetPosY()) * nDirY;
			if(nDirX && nDistX == CELLS_PER_VIEW || nDirY && nDistY == CELLS_PER_VIEW)
			{
				CMsgPlayer	msg;
				if(msg.Create(pUser->QueryRole()))
					this->SendMsg(&msg);
				if(msg.Create((IRole*)this))
					pUser->SendMsg(&msg);
			}
		}
	}

	ProcessAfterMove();
	return true;
}

void CMonster::TransPos(int nPosX, int nPosY)
{
	CMapPtr pMap = this->GetMap();
	CHECK(pMap);
	if(!pMap->IsValidPoint(nPosX, nPosY))
		return;

	pMap->MoveTo(this, nPosX, nPosY);
	m_nPosX = nPosX;
	m_nPosY = nPosY;

	if(!IsCallPet())
		pMap->SendBlockInfo(this);
	SendSelfToBlock();

	ProcessAfterMove();
}

void CMonster::JumpPos(int nPosX, int nPosY)
{
	int	nDirX	= (nPosX > m_nPosX) ? 1 : ((nPosX < m_nPosX) ? -1 : 0);
	int	nDirY	= (nPosY > m_nPosY) ? 1 : ((nPosY < m_nPosY) ? -1 : 0);
	int	nDeltaX	= abs(nPosX - m_nPosX);
	int	nDeltaY	= abs(nPosY - m_nPosY);

	CMapPtr pMap = this->GetMap();
	CHECK(pMap);

	IF_NOT(pMap->IsValidPoint(nPosX, nPosY))
		return;

	pMap->MoveTo(this, nPosX, nPosY);
	m_nPosX = nPosX;
	m_nPosY = nPosY;

	int x,y,z;
	CUser* pUser = NULL;
	FOR_9_BLOCKTHINGS(pMap, GetPosX(), GetPosY())
	{
		IMapThing* pTarget = pMap->QueryThingByIndex(x,y,z);
		if(pTarget && pTarget->QueryObj(OBJ_USER, IPP_OF(pUser)))
		{
			int	nDistX	= (pTarget->GetPosX() - this->GetPosX()) * nDirX;
			int	nDistY	= (pTarget->GetPosY() - this->GetPosY()) * nDirY;
			if(nDirX && nDistX > CELLS_PER_VIEW - nDeltaX && nDistX <= CELLS_PER_VIEW
				|| nDirY && nDistY > CELLS_PER_VIEW - nDeltaY && nDeltaY && nDistY <= CELLS_PER_VIEW)
			{
				CMsgPlayer	msg;
				if(msg.Create(pUser->QueryRole()))
					this->SendMsg(&msg);
				if(msg.Create((IRole*)this))
					pUser->SendMsg(&msg);
			}
		}
	}

	ProcessAfterMove();
}

bool CMonster::SendSelfToBlock()
{
	CMsgPlayer msg;
	if (msg.Create((IRole*)this))
		this->BroadcastRoomMsg(&msg);

	return true;
}

bool CMonster::SendLeaveFromBlock()
{
	if(!m_bLeaveMap)
	{
		m_bLeaveMap = true;

		CMsgAction	msg;
		if(msg.Create(GetID(), 0, 0, 0, actionLeaveMap))
		{	this->BroadcastRoomMsg(&msg, INCLUDE_SELF);
			if (this->IsCallPet() && QueryOwnerUser())
				QueryOwnerUser()->SendMsg(&msg);	}
	}

	return true;
}

void CMonster::SendShow(IRole* pRole)
{
	CHECK(pRole);

	if(!IsDeleted())
	{
		CMsgPlayer	msg;
		if(msg.Create((IRole*)this))
			pRole->SendMsg(&msg);
	}
}

bool CMonster::SendSysMsg(const char* fmt, ...)
{
	MSGBUF	szMsg;
    vsprintf( szMsg, fmt, (char*) ((&fmt)+1) );

	CMsgTalk msg;
	if (msg.Create(SYSTEM_NAME, GetName(), szMsg, NULL, 0xff0000, _TXTATR_SYSTEM))
		return this->SendMsg(&msg);

	return false;
}

bool CMonster::AddAttrib(int idxAttr, __int64 i64Data, int nSynchro, DWORD* pMask)
{
	CHECKF(idxAttr < 32);

	DWORD dwMask = pMask ? *pMask : (1 << idxAttr);
	if(i64Data)
	{
		int nDataTemp = (int)i64Data;
		switch(idxAttr)
		{
		case	_USERATTRIB_LIFE:
			{
				if ((int)this->GetLife() + nDataTemp <= 0)
				{
					m_nCurrLife	= 0;
				}
				else
					m_nCurrLife	= this->GetLife() + nDataTemp;
			}
			break;
		case	_USERATTRIB_MANA:
		case	_USERATTRIB_EXP:
		case	_USERATTRIB_MONEY:
			break;
		default:
			return false;
		}
		dwMask |= (1 << idxAttr);
	}

	if(nSynchro != SYNCHRO_FALSE && dwMask)
	{
		CMsgUserAttrib	msg;
		if(!msg.Create(GetID(), _USERATTRIB_NONE, _USERATTRIB_NONE))
			return false;

		bool bAppend = false;
		if(dwMask & (1 << _USERATTRIB_LIFE))
			bAppend = msg.Append(_USERATTRIB_LIFE, GetLife());
		else if(dwMask & (1 << _USERATTRIB_MANA))
			;
		else if(dwMask & (1 << _USERATTRIB_EXP))
			;
		else if(dwMask & (1 << _USERATTRIB_MONEY))
			;
		if(!bAppend)
			return false;

		dwMask = 0;
		switch(nSynchro)
		{
		case SYNCHRO_TRUE:
			this->SendMsg(&msg);
			break;

		case SYNCHRO_BROADCAST:
			this->BroadcastRoomMsg(&msg, INCLUDE_SELF);
			break;

		default:
			break;
		}
	}

	if(pMask)
		*pMask = dwMask;
	return true;
}

void CMonster::ProcessAfterMove()
{
	IThingSet* pSet = GetMap()->QueryBlock(GetPosX(), GetPosY()).QuerySet();
	for(int i = 0; i < pSet->GetAmount(); i++)
	{
		IMapThing* pTarget = pSet->GetObjByIndex(i);
		CMapTrap* pTrap;
		if(pTarget && pTarget->QueryObj(OBJ_TRAP, IPP_OF(pTrap))
				&& pTrap->IsTrapSort() && pTarget->GetPosX() == GetPosX() && pTarget->GetPosY() == GetPosY())
		{
			pTrap->TrapAttack(QueryRole());
		}
	}
}

void CMonster::ProcessOnMove(int nMoveMode)
{
	SetPose(_ACTION_STANDBY);
	if(nMoveMode != MOVEMODE_JUMPMAGICATTCK && nMoveMode != MOVEMODE_COLLIDE && nMoveMode != MOVEMODE_SYNCHRO)
		ClrAttackTarget();

	m_tFootPrint = ::TimeGet();
	m_posFootPrint.x = this->GetPosX();
	m_posFootPrint.y = this->GetPosY();
}

void CMonster::GetFootPrint	(int& nPosX, int& nPosY)
{
	if (::TimeGet()-m_tFootPrint >= TIME_FOOTPRINT)
	{
		nPosX = this->GetPosX();
		nPosY = this->GetPosY();
	}
	else
	{
		nPosX = m_posFootPrint.x;
		nPosY = m_posFootPrint.y;
	}
}

bool CMonster::SetAttackTarget(IRole* pTarget )
{
	if(pTarget == NULL)
	{
		m_pBattleSystem->ResetBattle();
		return true;
	}

	if(this->GetDistance(pTarget->QueryMapThing()) > GetAttackRange(pTarget->GetSizeAdd()))
		return false;

	CUser* pOwner = NULL;
	if(IsCallPet() && (pOwner = QueryOwnerUser()) != NULL)
	{
		CUser* pTargetUser = pTarget->QueryOwnerUser();
		if(pTargetUser)
		{
			if(GetMap()->IsPkDisable() || GetMap()->IsFamilyMap())
				return false;
			if(GetMap()->IsDeadIsland())
			{
				bool bNewbie = (pTargetUser->GetLev() <= 25);
				if(bNewbie)
					return false;
			}
		}

		if(pOwner->GetPk() >= 1000 && !GetMap()->IsPkField() && !GetMap()->IsSynMap()
			&& pTargetUser && pTargetUser->GetPk() < 30 && !pTargetUser->IsEvil())
		{
			pOwner->SendSysMsg(STR_PK);
			return false;
		}

		pOwner->CheckCrime(pTarget);
	}

	m_pBattleSystem->CreateBattle(pTarget->GetID());
	return true;
}

bool CMonster::AutoSkillAttack(IRole* pTarget)
{
	CHECKF(pTarget);

	if(!IsMagicAtk())
		return false;

	if(!m_pMagicType)
	{
		if(m_pType)
			::LogSave("CMonster::AutoSkillAttack() MonsterType id=%u, name=%s, magic_type=%u at %s, %d", m_pType->GetID(), m_pType->GetStr(NPCTYPEDATA_NAME), GetMagicType(), __FILE__, __LINE__);
		return false;
	}
	switch(m_pMagicType->GetInt(MAGICTYPEDATA_SORT))
	{
	case	MAGICSORT_RECRUIT:
		{
			if(m_pOwner && m_tMagic.ToNextTime(m_pMagicType->GetInt(MAGICTYPEDATA_STEP)))
			{
				if(m_pOwner->GetLife() < m_pOwner->GetMaxLife())
				{
					int nAdd = m_pMagicType->GetInt(MAGICTYPEDATA_POWER);
					m_pOwner->AddAttrib(_USERATTRIB_LIFE, nAdd, SYNCHRO_TRUE, NULL);

					CMsgMagicEffect	msg;
					IF_OK(msg.Create(GetID(), GetMagicType(), DEFAULT_MONSTER_MAGIC_LEVEL, m_pOwner->GetID(), nAdd))
						BroadcastRoomMsg(&msg, INCLUDE_SELF);
				}
			}
		}
		break;
	case	MAGICSORT_ADDMANA:
		{
			if(m_pOwner && m_tMagic.ToNextTime(m_pMagicType->GetInt(MAGICTYPEDATA_STEP)))
			{
				if(m_pOwner->GetMana() < m_pOwner->GetMaxMana())
				{
					int nAdd = m_pMagicType->GetInt(MAGICTYPEDATA_POWER);
					m_pOwner->AddAttrib(_USERATTRIB_MANA, nAdd, SYNCHRO_TRUE, NULL);

					CMsgMagicEffect	msg;
					IF_OK(msg.Create(GetID(), GetMagicType(), DEFAULT_MONSTER_MAGIC_LEVEL, m_pOwner->GetID(), nAdd))
						BroadcastRoomMsg(&msg, INCLUDE_SELF);
				}
			}
		}
		break;
	case	MAGICSORT_BOMB:
		{
			DEBUG_TRY
			{
				if(!IsCallPet())
					ProcessBomb();
				else
					return false;
			}
			DEBUG_CATCH("CMonster::ProcessBomb()")
		}
		break;
	case	MAGICSORT_VAMPIRE:
	case	MAGICSORT_INSTEAD:
		{
			return false;
		}
		break;
	default:
		MYASSERT(!"Nonsupport monster magic sort!!!");
		return false;
	}

	return true;
}

bool CMonster::AdditionMagic(int nLifeLost, int nDamage)
{
	if(!IsMagicAtk())
		return false;

	if(!m_pMagicType)
	{
		if(m_pType)
			::LogSave("CMonster::AutoSkillAttack() MonsterType id=%u, name=%s, magic_type=%u at %s, %d", m_pType->GetID(), m_pType->GetStr(NPCTYPEDATA_NAME), GetMagicType(), __FILE__, __LINE__);
		return false;
	}
	switch(m_pMagicType->GetInt(MAGICTYPEDATA_SORT))
	{
	case	MAGICSORT_RECRUIT:
	case	MAGICSORT_ADDMANA:
	case	MAGICSORT_BOMB:
	case	MAGICSORT_INSTEAD:
		{
			return false;
		}
		break;
	case	MAGICSORT_VAMPIRE:
		{
			if(m_pOwner && nLifeLost)
			{
				if(m_pOwner->GetLife() < m_pOwner->GetMaxLife())
				{
					int nAdd = ::CutTrail(0, CUser::AdjustData(nLifeLost, m_pMagicType->GetInt(MAGICTYPEDATA_POWER)));
					if(nAdd)
						m_pOwner->AddAttrib(_USERATTRIB_LIFE, nAdd, SYNCHRO_TRUE, NULL);
				}
			}
		}
		break;
	default:
		MYASSERT(!"Nonsupport monster magic sort !!!");
		return false;
	}

	return true;
}

int CMonster::GetDieMode()
{
	CHECKF(m_pMagicType);

	if (m_pMagicType->GetInt(MAGICTYPEDATA_WEAPON_HIT) == CMagic::HIT_BY_MAGIC)
		return DIE_MAGIC;
	else if (IsBowman())
		return DIE_ARROWSHOOT;
	else
		return DIE_NORMAL;
}

void CMonster::ProcessBomb()
{
	CHECK(IsMagicAtk());
	CHECK(m_pMagicType);

	int	nPower = m_pMagicType->GetInt(MAGICTYPEDATA_POWER);
	int	nRange = m_pMagicType->GetInt(MAGICTYPEDATA_RANGE);

	POINT	pos;
	pos.x			= GetPosX();
	pos.y			= GetPosY();
	int	nSize		= nRange*2 + 1;
	int	nBufSize	= nSize * nSize;
	MAPTHING_SET	setTarget;
	GetMap()->CollectMapThing(setTarget, pos, nRange, OBJ_USER|OBJ_MONSTER|OBJ_NPC);
	vector<IRole*>	setRole;
	vector<int>		setPower;

	for(int i = ::CutTrail((int)(pos.x-nRange), 0); i <= pos.x+nRange && i < GetMap()->GetWidth(); i++)
	{
		for(int j = ::CutTrail((int)(pos.y-nRange), 0); j <= pos.y+nRange && j < GetMap()->GetHeight(); j++)
		{
			POINT posThis;
			posThis.x	= i;
			posThis.y	= j;
			if(!CUser::IsInCircle(posThis, pos, nRange))
				continue;

			int idx = POS2INDEX(posThis.x - pos.x + nRange, posThis.y - pos.y + nRange, nSize, nSize);
			MYASSERT(0 <= idx && idx < (int)setTarget.size());
			if(setTarget[idx])
			{
				IRole* pRole = NULL;
				if(setTarget[idx]->QueryRole(IPP_OF(pRole)) && pRole->IsAlive() && !IsImmunity(pRole))
				{
					int	nPower	= 0;
					if(m_pMagicType->GetInt(MAGICTYPEDATA_WEAPON_HIT) == CMagic::HIT_BY_MAGIC)
						nPower += CMagic::CalcMagicPower(QueryRole(), pRole, m_pMagicType->GetInt(MAGICTYPEDATA_POWER));
					if(m_pMagicType->GetInt(MAGICTYPEDATA_WEAPON_HIT) == CMagic::HIT_BY_WEAPON)
						nPower += CBattleSystem::CalcAttackPower(QueryRole(), pRole, true, m_pMagicType->GetInt(MAGICTYPEDATA_POWER), 0, false);

					{
						setRole.push_back(pRole);
						setPower.push_back(nPower);
					}
				}
			}
		}
	}

	CMsgMagicEffect	msg;
	IF_OK(msg.CreateByPos(GetID(), GetMagicType(), DEFAULT_MONSTER_MAGIC_LEVEL, pos.x, pos.y))
	{
		for(int i = 0; i < (int)setRole.size(); )
		{
			int nDeltaBattleEffect = 0;
			CUser* pTargetUser = NULL;
			if(m_pOwner && (pTargetUser = setRole[i]->QueryOwnerUser()) != NULL)
				nDeltaBattleEffect = m_pOwner->GetBattlePower() - pTargetUser->GetBattlePower();
			if(msg.AppendRole(setRole[i]->GetID(), setPower[i], nDeltaBattleEffect))
				i++;
			else
			{
				BroadcastRoomMsg(&msg, INCLUDE_SELF);
				msg.CreateByPos(GetID(), GetMagicType(), DEFAULT_MONSTER_MAGIC_LEVEL, pos.x, pos.y);
			}
		}
		BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}

	if(m_pOwner)
	{
		for(int i = 0; i < (int)setRole.size(); i++)
		{
			if(m_pOwner->CheckCrime(setRole.at(i)))
				break;
		}

		int	nExp = 0;
		map<OBJID, int>	setExpRate;
		for (int i = 0; i < (int)setRole.size(); i++)
		{
			setPower[i] = setRole[i]->BeAttackEx(QueryRole(), setPower[i], true);
			int nLifeLost = __min(setRole[i]->GetLife(), setPower[i]);
			if(nLifeLost <= 0)
				continue;

			setRole[i]->AddAttrib(_USERATTRIB_LIFE, -1*nLifeLost, SYNCHRO_TRUE, NULL);
			CNpc* pNpc = NULL;
			if(setRole[i]->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsAwardScore())
				m_pOwner->AwardSynWarScore(pNpc, nLifeLost);

			if (setRole[i]->IsMonster())
			{
				int nAdjustExp = 0;
				CMonster* pMonster = NULL;
				if(setRole[i]->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
				{
					if(setExpRate.find(pMonster->GetType()) == setExpRate.end())
					{
						nAdjustExp = m_pOwner->AdjustExp(setRole[i], nLifeLost);
						setExpRate[pMonster->GetType()] = nAdjustExp*10000 / nLifeLost;
					}
					else
						nAdjustExp = setExpRate[pMonster->GetType()] * nLifeLost / 10000;
				}
				else
					nAdjustExp = m_pOwner->AdjustExp(setRole[i], nLifeLost);
				nExp += nAdjustExp;
			}

			if(!setRole[i]->IsAlive())
				Kill(setRole[i], GetDieMode(), 0);
		}

		AwardBattleExp(nExp, true, 0);
	}
}

int CMonster::Attack(IRole* pTarget)
{
	CHECKF(pTarget);

	int nDamage = 0;
	if(IsSimpleMagicAtk())
		nDamage = CMagic::CalcMagicPower(QueryRole(), pTarget, GetAttack());
	else
		nDamage = CBattleSystem::CalcAttackPower(QueryRole(), pTarget, false, 0, 0, false);

	nDamage = pTarget->BeAttack(QueryRole(), nDamage, true);
	int nLoseLife = ::CutOverflow(nDamage, (int)pTarget->GetLife());
	if (nLoseLife > 0)
		pTarget->AddAttrib(_USERATTRIB_LIFE, -1*nLoseLife, SYNCHRO_TRUE, NULL);

	return nDamage;
}

int CMonster::BeAttack(IRole* pTarget, int nPower, bool bReflectEnable)
{
	if (m_pType->GetInt(NPCTYPEDATA_MAGIC_TYPE) == SIMPLEMAGIC_RECRUIT && !m_tAddLife.IsActive())
		m_tAddLife.Startup(RECRUITMAGIC_SECS);

	return nPower;
}

void CMonster::Kill(IRole* pTarget, DWORD dwDieWay, int nReserved)
{
	CHECK(pTarget);

	int nDeltaBattleEffect = 0;
	CUser* pOwnerUser = QueryOwnerUser();
	CUser* pTargetOwner = pTarget->QueryOwnerUser();
	if(pOwnerUser && pTargetOwner)
		nDeltaBattleEffect = pOwnerUser->GetBattlePower() - pTargetOwner->GetBattlePower();

	CMsgInteract msg;
	if(msg.Create(INTERACT_KILL, this->GetID(), pTarget->GetID(), pTarget->GetPosX(), pTarget->GetPosY(), dwDieWay, nDeltaBattleEffect))
		pTarget->BroadcastRoomMsg(&msg, true);

	if(m_pOwner)
	{
		CUser*	pTargetUser=NULL;
		CMonster* pMonster = NULL;
		if(pTarget->QueryObj(OBJ_USER, IPP_OF(pTargetUser)))
			m_pOwner->ProcessPk(pTargetUser);
		else if(pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)) && pMonster->IsCallPet() && pMonster->IsInsteadMagic() && pMonster->QueryOwnerUser())
			m_pOwner->ProcessPk(pMonster->QueryOwnerUser());
	}

	pTarget->BeKill((IRole*)this, true);
}

void CMonster::BeKill(IRole* pRole , int nReserved)
{
	if(IsDeleted())
		return ;

	if(m_pData)
	{
		m_pData->SetInt(PETDATA_LIFE, 0);
	}

	CUser* pActionUser = NULL;
	if(pRole)
		pActionUser = pRole->QueryOwnerUser();
	OBJID idMapItemOwner = NULL;
	if(pActionUser)
		idMapItemOwner = pActionUser->GetID();

	if((IsCallPet() ) && IsMagicAtk() && m_pMagicType && m_pMagicType->GetInt(MAGICTYPEDATA_SORT) == MAGICSORT_BOMB)
	{
		DEBUG_TRY
		ProcessBomb();
		DEBUG_CATCH("CMonster::ProcessBomb()")
	}

	DWORD dwMoney = ::RandGet(m_pType->GetInt(NPCTYPEDATA_DROPMONEY)) * 2;
	int nHeapNum = 0;
	if(GetLev() <= 60)
	{
		int nRand = ::RandGet(100);
		if(nRand < 2)
		{
			dwMoney = dwMoney * 2000 / 100;
			switch(::RandGet(4))
			{
			case 0:		nHeapNum = 5;	break;
			case 1:		nHeapNum = 6;	break;
			case 2:		nHeapNum = 7;	break;
			default:	nHeapNum = 8;	break;
			}
		}
		else if(nRand < 27)
		{
			dwMoney = dwMoney * 240 / 100;
			int nRandHeap = ::RandGet(100);
			if(nRandHeap < 10)
				nHeapNum = 3;
			else if(nRandHeap < 40)
				nHeapNum = 2;
			else
				nHeapNum = 1;
		}
	}
	else
	{
		int nRand = ::RandGet(100);
		if(nRand < 3)
		{
			dwMoney = dwMoney * 1320 / 100;
			switch(::RandGet(4))
			{
			case 0:		nHeapNum = 5;	break;
			case 1:		nHeapNum = 6;	break;
			case 2:		nHeapNum = 7;	break;
			default:	nHeapNum = 8;	break;
			}
		}
		else if(nRand < 35)
		{
			dwMoney = dwMoney * 180 / 100;
			int nRandHeap = ::RandGet(100);
			if(nRandHeap < 10)
				nHeapNum = 3;
			else if(nRandHeap < 40)
				nHeapNum = 2;
			else
				nHeapNum = 1;
		}
	}
	if(nHeapNum > 0)
	{
		DWORD dwMoneyAve = dwMoney / nHeapNum;
		for(int i = 0; i < nHeapNum; i++)
		{
			DWORD dwMoneyTmp = (DWORD)(::RandomRateGet(0.8) * dwMoneyAve);
			if(::RandGet(100) < 40)
				this->DropEquipment(dwMoneyTmp, idMapItemOwner);
			else
				this->DropMoney(dwMoneyTmp, idMapItemOwner);
		}
	}

	int nRandQuality = ::RandGet(50000);
	if(nRandQuality < 1)
	{
		if(GetLev() > 80 ? ::RandGet(10) == 7 : ::RandGet(3) == 1)
			this->DropEquipment(GetLev()*100, idMapItemOwner, 9);
	}
	else if(nRandQuality < 6)
	{
		if(GetLev() <= 80 || ::RandGet(5) == 3)
			this->DropEquipment(GetLev()*100, idMapItemOwner, 8);
	}
	else if(nRandQuality < 31)
	{
		if(GetLev() <= 80 || ::RandGet(3) == 1)
			this->DropEquipment(GetLev()*50, idMapItemOwner, 7);
	}
	else if(nRandQuality < 131)
		this->DropEquipment(GetLev()*30, idMapItemOwner, 6);

	if(::RandGet(100) < 3)
		this->DropMedicine(idMapItemOwner);

	if(GetMap() && GetMap()->GetID() != 1002)
	{
		if(::RandGet(750) == 123)
			this->DropItem(1088001, idMapItemOwner);
		if(::RandGet(300000) == 123)
		{
			this->DropItem(1088000, idMapItemOwner);
			if(pActionUser)
			{
				char szMsg[1024] = "";
				sprintf(szMsg, DRAGON_BALL_DROP, pActionUser->GetName());
				pActionUser->BroadcastMapMsg(szMsg, true);
				::MyLogSave("gmlog/emoney_buy", "%u\t%s\t%u\t%u\t%u\t%d\t", idMapItemOwner, pActionUser->GetName(), pActionUser->GetAccountID(), 300, 308, 0);
			}
		}
	}

	int nGemChance = 0;
	if(GetLev() >= 117)
		nGemChance = 1400;
	else if(pRole->GetLev() > GetLev() + 5)
		nGemChance = 9000;
	else
		nGemChance = 900;
	if(::RandGet(nGemChance) == 13)
	{
		OBJID idGemType = ID_NONE;
		int nRandGem = ::RandGet(1000);
		if(nRandGem < 100)
			idGemType = 700011;
		else if(nRandGem < 200)
			idGemType = 700001;
		else if(nRandGem < 300)
			idGemType = 700031;
		else if(nRandGem < 475)
			idGemType = 700061;
		else if(nRandGem < 650)
			idGemType = 700051;
		else if(nRandGem < 825)
			idGemType = 700021;
		else
			idGemType = 700041;
		this->DropItem(idGemType, idMapItemOwner);
	}

	if(!IsCallPet())
		this->DropEventItem(idMapItemOwner);

	if (pActionUser)
	{
		GameAction()->ProcessAction(m_pType->GetInt(NPCTYPEDATA_ACTION), pActionUser, QueryRole());
	}

	this->SetStatus(STATUS_DIE, true);
	DelMonster();

	DEBUG_TRY
	if(IsInsteadMagic() && m_pOwner)
	{
		m_pOwner->SetLife(0, true);
		m_pOwner->BeKill(pRole, 0);
	}
	DEBUG_CATCH("instead kill!")
}

void CMonster::DelMonster(bool bNow )
{
	CHECK(!IsDeleted());

	if(m_pData)
	{
		m_pData->SetInt(PETDATA_LIFE, 0);
	}

	m_tDie.Startup(NPCDIEDELAY_SECS);
	if(bNow)
		SendLeaveFromBlock();
}

void CMonster::SendDamageMsg(OBJID idTarget, int nDamage)
{
	int nDeltaBattleEffect = 0;
	CUser* pOwnerUser = QueryOwnerUser();
	IRole* pTarget = RoleManager()->QueryRole(idTarget);
	if(pTarget)
	{
		CUser* pTargetUser = pTarget->QueryOwnerUser();
		if(pOwnerUser && pTargetUser)
			nDeltaBattleEffect = pOwnerUser->GetBattlePower() - pTargetUser->GetBattlePower();
	}
	if(IsSimpleMagicAtk())
	{
		CMsgMagicEffect	msg;
		IF_OK(msg.Create(GetID(), m_pType->GetInt(NPCTYPEDATA_MAGIC_TYPE), 0, idTarget, nDamage, nDeltaBattleEffect))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}
	else if(IsBowman())
	{
		CMsgInteract msg;
		IF_OK(msg.Create(INTERACT_SHOOT, GetID(), idTarget, GetPosX(), GetPosY(), nDamage, nDeltaBattleEffect))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}
	else
	{
		CMsgInteract msg;
		IF_OK(msg.Create(INTERACT_ATTACK, GetID(), idTarget, GetPosX(), GetPosY(), nDamage, nDeltaBattleEffect))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}
}

bool CMonster::DropMoney(DWORD dwMoney, OBJID idOwner)
{
	if (dwMoney <= 0)
		return true;

	if (this->GetLev() >= 80 && dwMoney <= 10)
		return true;

	CUser* pUser = UserManager()->GetUser(idOwner);
	if (pUser && pUser->IsIncomeLimited())
		dwMoney = pUser->GetIncomePercent() * dwMoney / 100;

	POINT	pos;
	pos.x	= GetPosX();
	pos.y	= GetPosY();
	if(GetMap()->FindDropItemCell(MONSTERDROPITEM_RANGE, &pos))
	{
		CMapItem* pMapItem = CMapItem::CreateNew();
		if(pMapItem)
		{
			if(pMapItem->CreateMoney(MapManager()->SpawnMapItemID(), GetMap(), pos, dwMoney, idOwner))
			{
				MapManager()->QueryMapItemSet()->AddObj(pMapItem);
				return true;
			}
			else
			{
				::LogSave("CMonster::DropMoney(): monster_type[%d], dwMoney[%u].", GetType(), dwMoney);
				pMapItem->ReleaseByOwner();
			}
		}
	}

	return false;
}

bool CMonster::DropItem(OBJID idItemType, OBJID idOwner)
{
	if (idItemType == ID_NONE)
		return true;

	POINT	pos;
	pos.x	= GetPosX();
	pos.y	= GetPosY();
	if(GetMap()->FindDropItemCell(MONSTERDROPITEM_RANGE, &pos))
	{
		CMapItem* pMapItem = CMapItem::CreateNew();
		if(pMapItem)
		{
			if(pMapItem->Create(MapManager()->SpawnMapItemID(), GetMap(), pos, idItemType, idOwner))
			{
				MapManager()->QueryMapItemSet()->AddObj(pMapItem);
				return true;
			}
			else
			{
				::LogSave("CMonster::DropItem(): monster_type[%d], item_type[%u].", GetType(), idItemType);
				pMapItem->ReleaseByOwner();
			}
		}
	}

	return false;
}

void CMonster::DropEventItem(OBJID idOwner)
{
	CUser* pUser = UserManager()->GetUser(idOwner);
	if (!pUser)
		return;

	for (int idConfig = 7001; idConfig <= 7003; idConfig++)
	{
		CMapGroup::CONFIG_INFO* pInfo = MapGroup(PID)->QueryConfig(idConfig);
		if (!pInfo || pInfo->nData[0] != 1)
			continue;

		int nRand	= ::RandGet(10000);
		int nRate	= pInfo->nData[2];
		if (pUser->IsBowman())
			nRate	/= 3;
		if (pInfo->nData[3] == 1 && (int)(pUser->GetLev() - this->GetLev()) >= pInfo->nData[4])
			nRate	/= 3;

		if (nRand < nRate)
			this->DropItem(pInfo->nData[1], idOwner);
	}
}

bool CMonster::DropEquipment (DWORD dwValue, OBJID idOwner, DWORD dwQuality )
{
	CUser* pUser = UserManager()->GetUser(idOwner);
	if (pUser && pUser->IsIncomeLimited() && ::RandGet(100) >= pUser->GetIncomePercent())
		return false;

	if (dwValue < 100)
		return this->DropMoney(dwValue, idOwner);

	ItemInfoStruct info;
	if (CItem::CreateItemInfo(info, dwValue, m_pType, dwQuality))
	{
		POINT	pos;
		pos.x	= GetPosX();
		pos.y	= GetPosY();
		if(GetMap()->FindDropItemCell(MONSTERDROPITEM_RANGE, &pos))
		{
			CUser* pUserOwner = UserManager()->GetUser(idOwner);
			if(pUserOwner && pUserOwner->m_dwUnk638 > 0)
			{
				bool bLucky = false;
				if((info.idType == 1088000 || info.idType%10 == 9) && ::RandGet(100) < 75)
					bLucky = true;
				else if(info.idType == 1088001 && ::RandGet(100) < 10)
				{
					info.idType = 1088002;
					bLucky = true;
				}

				if(bLucky)
				{
					if(info.idType%10 == 9)
						pUserOwner->SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_ULTRA_EQP);
					else if(info.idType == 1088000)
						pUserOwner->SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_DRAGON_BALL);
					else if(info.idType == 1088002)
						pUserOwner->SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_SHT_STAR_TEAR);

					CMsgName msg;
					IF_OK(msg.Create(NAMEACT_ROLEEFFECT, STR_LUCKY_EFFECT, pUserOwner->GetID()))
						BroadcastRoomMsg(&msg, INCLUDE_SELF);
				}
			}

			CMapItem* pMapItem = CMapItem::CreateNew();
			if(pMapItem)
			{
				info.id = ID_NONE;
				if(pMapItem->Create(MapManager()->SpawnMapItemID(), GetMap(), pos, &info, idOwner))
				{
					MapManager()->QueryMapItemSet()->AddObj(pMapItem);
					if(info.nMagic3 > 0 && pUserOwner)
						pUserOwner->GuideNotify(info.nMagic3);
					return true;
				}
				else
				{
					::LogSave("CMonster::DropEquipment(): monster_type[%d], item_type[%u].", GetType(), info.idType);
					pMapItem->ReleaseByOwner();
					return false;
				}
			}
		}

		return false;
	}

	return this->DropMoney(dwValue, idOwner);
}

bool CMonster::DropMedicine(OBJID idOwner)
{
	int nAmount = 0;
	int nRate = ::RandGet(100);
	if (nRate < 2)
		nAmount = 8;
	else if (nRate < 3)
		nAmount = 6;
	else if (nRate < 5)
		nAmount = 5;
	else if (nRate < 8)
		nAmount = 4;
	else if (nRate < 10)
		nAmount = 3;
	else if (nRate < 25)
		nAmount = 2;
	else
		nAmount = 1;

	for (int i=0; i<nAmount; i++)
	{
		DWORD dwType = 0;
		if (1 == ::RandGet(2))
			dwType = m_pType->GetInt(NPCTYPEDATA_DROP_HP);
		else
			dwType = m_pType->GetInt(NPCTYPEDATA_DROP_MP);

		this->DropItem(dwType, idOwner);
	}

	return true;
}

bool CMonster::IsAtkable(IRole* pTarget, bool bSendHint)
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

bool CMonster::EnterMapGroup()
{
	CMapPtr pMap = this->GetMap();
	CHECKF(pMap);

	MapGroup(PID)->QueryIntraMsg()->SetNpcProcessID(GetID(), true);
	pMap->EnterRoom(this->QueryMapThing());
	if(!IsCallPet())
		pMap->SendBlockInfo((IRole*)this);
	SendSelfToBlock();
	{
		CMsgAction	msg;
		IF_OK(msg.Create(GetID(), GetPosX(), GetPosY(), GetDir(), actionMonsterBorn))
			this->BroadcastRoomMsg(&msg, EXCLUDE_SELF);
	}
	if(IsMapRoar())
	{
		CMsgAction	msg;
		IF_OK(msg.Create(GetID(), GetPosX(), GetPosY(), GetDir(), actionSoundEffect, GetType()))
			_UserManager(PID)->BroadcastMapMsg(pMap->GetID(), &msg);
	}

	return true;
}

bool CMonster::LeaveMapGroup()
{
	CMapPtr pMap = this->GetMap();
	CHECKF(pMap);

	MapGroup(PID)->QueryIntraMsg()->SetNpcProcessID(GetID(), false);

	SendLeaveFromBlock();

	pMap->LeaveRoom(this->QueryMapThing());
	return true;
}

DWORD CMonster::GetMinAtk()
{
	DWORD dwAtk	= m_pType->GetInt(NPCTYPEDATA_ATTACKMIN);

	return dwAtk;
}

DWORD CMonster::GetMaxAtk()
{
	DWORD dwAtk	= m_pType->GetInt(NPCTYPEDATA_ATTACKMAX);

	dwAtk	= dwAtk;

	return dwAtk;
}
DWORD CMonster::GetDdg()
{
	DWORD dwDodge	= this->GetDodge();

	IStatus* pStatus = QueryStatus(STATUS_ADJUST_DODGE);
	if(pStatus)
		dwDodge = AdjustData(dwDodge, pStatus->GetPower());

	return __max(0, dwDodge);
}

DWORD CMonster::GetDef()
{
	DWORD dwDef	= 0;

	dwDef	= dwDef + this->GetDefence();

	return dwDef;
}

DWORD CMonster::GetDex()
{
	float fDex = 0.0f;

	fDex = fDex + this->GetDexterity();

	return fDex*100.0f;
}

DWORD CMonster::GetMagicDef()
{
	int nDef	= m_pType->GetInt(NPCTYPEDATA_MAGIC_DEF);

	IStatus* pStatus = QueryStatus(STATUS_ADJUST_MAGICDEF);
	if(pStatus)
		nDef = AdjustData(nDef, pStatus->GetPower());

	nDef = __min(90, nDef);
	nDef = __max(0, nDef);

	return nDef;
}

DWORD CMonster::GetInterAtkRate()
{
	int nRate = GetIntervalAtkRate();

	IStatus* pStatus = QueryStatus(STATUS_ATKSPEED);
	if (pStatus)
		nRate	= ::CutTrail(0, AdjustData(nRate, pStatus->GetPower()));

	return nRate;
}

void CMonster::SetStatus(int nStatus, bool bSynchro)
{
	DWORD	dwOldEffect = m_dwEffect;
	switch (nStatus)
	{
	case	STATUS_NORMAL:
		{
			m_dwStatus	= nStatus;
			m_dwEffect	= 0;
		}
		break;
	case	STATUS_DIE:
		{
			m_dwStatus	= nStatus;
			m_dwEffect	= KEEPEFFECT_DIE;
			ClearAllStatus();
		}
		break;
	case	STATUS_CRIME:
		{
			m_dwEffect	|= KEEPEFFECT_CRIME;
		}
		break;
	case	0x00000004:
		{
			m_dwEffect	|= 0x00000002;
		}
		break;
	case	STATUS_HITRATE:
		{
			m_dwEffect	|= 0x00000080;
		}
		break;
	case	STATUS_DEFENCE:
		{
			m_dwEffect	|= 0x00000100;
		}
		break;
	case	STATUS_ATTACK:
		{
			m_dwEffect	|= 0x00000200;
		}
		break;
	case	STATUS_DISAPPEARING:
		{
			m_dwEffect	|= 0x00000800;
		}
		break;
	case	STATUS_ADJUST_MAGICDEF:
		{
			m_dwEffect	|= 0x00001000;
		}
		break;
	case	STATUS_SUPER_MATK:
		{
			m_dwEffect	|= 0x00002000;
		}
		break;
	case	STATUS_WEAPONDAMAGE:
		{
			m_dwEffect	|= 0x00080000;
		}
		break;
	case	STATUS_MAGICDAMAGE:
		{
			m_dwEffect	|= 0x00100000;
		}
		break;
	case	STATUS_ATKSPEED:
		{
			m_dwEffect	|= 0x00200000;
		}
		break;
	case	STATUS_SYNCRIME:
		{
			m_dwEffect	|= KEEPEFFECT_SYNCRIME;
		}
		break;
	case	STATUS_ADJUST_DODGE:
		{
			m_dwEffect	|= 0x04000000;
		}
		break;
	case	STATUS_WING:
		{
			m_dwEffect	|= KEEPEFFECT_WING;
		}
		break;
	case	STATUS_KEEPBOW:
		{
			m_dwEffect	|= 0x10000000;
		}
		break;
	default:
		MYASSERT(!"switch status");
	}

	if(bSynchro && dwOldEffect != m_dwEffect)
	{
		CMsgUserAttrib msg;
		IF_OK(msg.Create(GetID(), _USERATTRIB_KEEPEFFECT, m_dwEffect))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}
}

void CMonster::ClsStatus(int nStatus, bool bSynchro)
{
	DWORD	dwOldEffect = m_dwEffect;
	switch (nStatus)
	{
	case	STATUS_NORMAL:
		{
			MYASSERT(!"STATUS_NORMAL");
		}
		break;
	case	STATUS_DIE:
		{
			m_dwStatus	= nStatus;
			m_dwEffect	&= ~KEEPEFFECT_DIE;
		}
		break;
	case	STATUS_CRIME:
		{
			m_dwEffect	&= ~KEEPEFFECT_CRIME;
		}
		break;
	case	0x00000004:
		{
			m_dwEffect	&= ~0x00000002;
		}
		break;
	case	STATUS_HITRATE:
		{
			m_dwEffect	&= ~0x00000080;
		}
		break;
	case	STATUS_DEFENCE:
		{
			m_dwEffect	&= ~0x00000100;
		}
		break;
	case	STATUS_ATTACK:
		{
			m_dwEffect	&= ~0x00000200;
		}
		break;
	case	STATUS_ADJUST_MAGICDEF:
		{
			m_dwEffect	&= ~0x00001000;
		}
		break;
	case	STATUS_SUPER_MATK:
		{
			m_dwEffect	&= ~0x00002000;
		}
		break;
	case	STATUS_WEAPONDAMAGE:
		{
			m_dwEffect	&= ~0x00080000;
		}
		break;
	case	STATUS_MAGICDAMAGE:
		{
			m_dwEffect	&= ~0x00100000;
		}
		break;
	case	STATUS_ATKSPEED:
		{
			m_dwEffect	&= ~0x00200000;
		}
		break;
	case	STATUS_SYNCRIME:
		{
			m_dwEffect	&= ~KEEPEFFECT_SYNCRIME;
		}
		break;
	case	STATUS_ADJUST_DODGE:
		{
			m_dwEffect	&= ~0x04000000;
		}
		break;
	case	STATUS_WING:
		{
			m_dwEffect	&= ~KEEPEFFECT_WING;
		}
		break;
	case	STATUS_KEEPBOW:
		{
			m_dwEffect	&= ~0x10000000;
		}
		break;
	default:
		MYASSERT(!"switch status");
	}

	if(bSynchro && dwOldEffect != m_dwEffect)
	{
		CMsgUserAttrib msg;
		IF_OK(msg.Create(GetID(), _USERATTRIB_KEEPEFFECT, m_dwEffect))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}
}

void CMonster::SaveInfo()
{
	if(m_pData)
	{
		m_pData->SetInt(PETDATA_LIFE, GetLife());
		m_pData->SetInt(PETDATA_RECORD_X, GetPosX());
		m_pData->SetInt(PETDATA_RECORD_Y, GetPosY());
		m_pData->Update();
	}
}

bool CMonster::IsAlive()
{
	if ((m_dwStatus & STATUS_DIE) == STATUS_DIE || this->GetLife() <= 0 || m_bLeaveMap)
		return false;

	return true;
}

bool CMonster::IsEvil()
{
	if(IsCallPet())
	{
		if(QueryOwnerUser() && QueryOwnerUser()->IsEvil())
			return true;
		return false;
	}
	return (m_pType->GetInt(NPCTYPEDATA_ATKUSER) & (ATKUSER_RIGHTEOUS|ATKUSER_NEUTRAL)) == 0;
}

__int64 CMonster::AwardBattleExp(__int64 i64Exp, bool bGemEffect, int nReserved)
{
	if(IsCallPet() && m_pOwner)
		return m_pOwner->AwardBattleExp(i64Exp, bGemEffect, 1);
	return 0;
}

bool CMonster::IsInsteadMagic()
{
	return m_pMagicType && m_pMagicType->GetInt(MAGICTYPEDATA_SORT) == MAGICSORT_INSTEAD;
}

OBJID CMonster::GetMasterID()
{
	if((IsCallPet() ) && m_pOwner)
		return m_pOwner->GetID();
	return ID_NONE;
}

bool CMonster::IsImmunity(IRole* pRole)
{
	CHECKF(pRole);

	if((IsCallPet() ) && m_pOwner)
		return m_pOwner->IsImmunity(pRole);

	const bool	ATK_IT = false;
	const bool	IMMUNITY = true;
	OBJID idRole = pRole->GetID();
	if(IsSceneID(idRole))
	{
		if(pRole->IsBeAtkable())
			return ATK_IT;
		else
			return IMMUNITY;
	}
	else if(IsNpcID(idRole) || idRole >= CALLPETID_FIRST && idRole <= CALLPETID_LAST)
	{
		if(IsEvil() && pRole->IsVirtuous() || IsVirtuous() && pRole->IsEvil())
			return ATK_IT;
		else
			return IMMUNITY;
	}
	else
	{
		CUser* pUser = NULL;
		pRole->QueryObj(OBJ_USER, IPP_OF(pUser));
		if(IsEvil())
			return ATK_IT;
		else
			return IMMUNITY;
	}
}
