#include "TradeBuddy.h"
#include "AllMsg.h"
#include "windows.h"
#include "Gameobj.h"
#include "MapGroup.h"
#include "define.h"
#include "Npc.h"
#include "NpcTask.h"
#include "NpcScene.h"
#include "NpcLink.h"
#include "NpcStorage.h"
#include "BetOutside.h"
#include <map>
const int	FIXSYNFLAG_SECS				= 10;
const int	MAX_FIXMONEY				= 1000;
const int	FIXLIFE_PER_MONEY			= 10;
const int	BROADCASTSCORE_SECS			= 30;
const int	TRANSNPC_FEE				= 10000;

char	szNpcTable[] = _TBL_NPC;
char	szDynTable[] = _TBL_DYNANPC;
MYHEAP_IMPLEMENTATION(CNpc,s_heap)

CNpc::CNpc()
{
	SetObjType(OBJ_NPC);
	m_id	= ID_NONE;
	m_pData	= NULL;
	m_bDynaNpc	= false;

	m_pShop		= NULL;
	m_pTask		= NULL;
	m_pTrunk	= NULL;
	m_pScene	= NULL;
	m_pLinkMap	= NULL;
	m_pBet		= NULL;
	m_pMap		= NULL;
	m_setStatus	= NULL;
	m_dwEffect	= 0;
}

CNpc::~CNpc()
{
	if(m_pMap)
	{
		m_pMap->LeaveRoom(QueryMapThing(), WITH_BLOCK);
	}

	SAFE_RELEASE (m_setStatus);

	if(m_pData)
	{
		if(IsDynaNpc())
		{
			if(IsAlive())
				SaveInfo();
			else
				DeleteRecord();
		}
		m_pData->Release();
	}
	if(m_pShop)
	{
		m_pShop->Release();
	}
	if(m_pTask)
	{
		m_pTask->Release();
	}
	if(m_pLinkMap)
	{
		m_pLinkMap->Release();
	}
	if(m_pScene)
	{
		m_pScene->Release();
	}
	if(m_pTrunk)
	{
		m_pTrunk->Release();
	}
	if(m_pBet)
	{
		m_pBet->Release();
	}
}

bool CNpc::Create(PROCESS_ID idProcess, NPCINFO_ST* pInfo, OBJID idItemType, LPCTSTR szName)
{
	m_idProcess = idProcess;
	m_bDynaNpc	= true;

	MYASSERT(!m_pData);
	CDynaNpcData* pData = CDynaNpcData::CreateNew();
	if(pData)
		m_pData	= pData->QueryInterface();
	IF_NOT(m_pData && m_pData->Create(GameDataDefault()->GetDynaNpc(), ID_NONE))
		return false;
	m_pData->SetInt(NPCDATA_OWNERID,	pInfo->idOwner);
	m_pData->SetInt(NPCDATA_OWNERTYPE,	pInfo->nOwnerType);
	m_pData->SetInt(NPCDATA_TYPE,		pInfo->nType);
	m_pData->SetStr(NPCDATA_NAME,		szName, _MAX_NAMESIZE);
	m_pData->SetInt(NPCDATA_LOOKFACE,	pInfo->nLookFace);
	m_pData->SetInt(NPCDATA_IDXSERVER,	-1);
	m_pData->SetInt(NPCDATA_MAPID,		pInfo->idMap);
	m_pData->SetInt(NPCDATA_POSX_,		pInfo->nPosX);
	m_pData->SetInt(NPCDATA_POSY_,		pInfo->nPosY);
	m_pData->SetInt(NPCDATA_TASK0,		pInfo->setTask[0]);
	m_pData->SetInt(NPCDATA_TASK1,		pInfo->setTask[1]);
	m_pData->SetInt(NPCDATA_TASK2,		pInfo->setTask[2]);
	m_pData->SetInt(NPCDATA_TASK3,		pInfo->setTask[3]);
	m_pData->SetInt(NPCDATA_TASK4,		pInfo->setTask[4]);
	m_pData->SetInt(NPCDATA_TASK5,		pInfo->setTask[5]);
	m_pData->SetInt(NPCDATA_TASK6,		pInfo->setTask[6]);
	m_pData->SetInt(NPCDATA_TASK7,		pInfo->setTask[7]);
	m_pData->SetInt(NPCDATA_DATA0,		pInfo->setData[0]);
	m_pData->SetInt(NPCDATA_DATA1,		pInfo->setData[1]);
	m_pData->SetInt(NPCDATA_DATA2,		pInfo->setData[2]);
	m_pData->SetInt(NPCDATA_DATA3,		pInfo->setData[3]);
	m_pData->SetStr(NPCDATA_DATASTR,	pInfo->szData, _MAX_NAMESIZE);
	m_pData->SetInt(NPCDATA_LINKID,		pInfo->idLink);
	m_pData->SetInt(NPCDATA_LIFE,		pInfo->nLife);
	m_pData->SetInt(NPCDATA_MAXLIFE,	pInfo->nLife);
	m_pData->SetInt(NPCDATA_BASE,		pInfo->idBase);
	m_pData->SetInt(NPCDATA_SORT,		pInfo->nSort);
	if(pInfo->nSort & NPCSORT_RECYCLE)
		m_pData->SetInt(NPCDATA_ITEMTYPE,		idItemType);

	m_nPosX = pInfo->nPosX;
	m_nPosY = pInfo->nPosY;
	IF_NOT(CheckSortMutex())
		return false;

	m_setStatus = CStatusSet::CreateNew(true);
	CHECKF(m_setStatus);
	m_pMap = MapManager()->QueryMap(pInfo->idMap);
	IF_NOT(m_pMap)
		return false;

	if(!m_pData->InsertRecord())
	{
		if(pInfo)
			LOGERROR("CNpc::Create() idProcess=%u, itemType=%u, name=%s, map%u(%d, %d), idOwner=%u, nType=%u, at %s, %d", idProcess, idItemType, szName, pInfo->idMap, pInfo->nPosX, pInfo->nPosY, pInfo->idOwner, pInfo->nType, __FILE__, __LINE__);
		else
			LOGERROR("CNpc::Create() idProcess=%u, itemType=%u, name=%s at %s, %d", idProcess, idItemType, szName, __FILE__, __LINE__);
		return false;
	}

	m_id	= MapManager()->SpawnNewNpcID();

	if(!CreateFuncObj(idProcess))
	{
		if(pInfo)
			LOGERROR("CNpc::Create() idProcess=%u, itemType=%u, name=%s, map%u(%d, %d), idOwner=%u, NPCID=%u, nType=%u, at %s, %d", idProcess, idItemType, szName, pInfo->idMap, pInfo->nPosX, pInfo->nPosY, pInfo->idOwner, m_pData->GetID(), pInfo->nType, __FILE__, __LINE__);
		else
			LOGERROR("CNpc::Create() idProcess=%u, itemType=%u, name=%s at %s, %d", idProcess, idItemType, szName, __FILE__, __LINE__);
		return false;
	}

	m_pMap->EnterRoom(QueryMapThing(), WITH_BLOCK);
	SendSelfToBlock();

	if(IsSynFlag())
	{
		GetMap()->SetSynID(GetSynID(), true);

		OBJID idMoreMap = GetData(SYNFLAGNPC_MOREMAPID);
		if(idMoreMap != ID_NONE)
		{
			CGameMap* pMoreMap = MapManager()->QueryMap(idMoreMap);
			if(pMoreMap)
				pMoreMap->SetSynID(GetSynID(), true);
		}
	}

	return true;
}

bool CNpc::Create(PROCESS_ID idProcess, IRecordset* pRecordset, bool bDynaNpc)
{
	m_idProcess = idProcess;
	m_bDynaNpc	= bDynaNpc;

	MYASSERT(!m_pData);
	if(bDynaNpc)
	{
		CDynaNpcData* pData = CDynaNpcData::CreateNew();
		if(pData)
			m_pData	= pData->QueryInterface();
	}
	else
	{
		CNpcData* pData = CNpcData::CreateNew();
		if(pData)
			m_pData	= pData->QueryInterface();
	}
	IF_NOT(m_pData && m_pData->Create(pRecordset))
		return false;

	CGameMap* pMap = MapManager()->QueryMap(m_pData->GetInt(NPCDATA_MAPID), false);
	if(!pMap)
		return false;

	if(bDynaNpc)
		m_id	= MapManager()->SpawnNewNpcID();
	else
		m_id	= m_pData->GetID();
	m_nPosX = m_pData->GetInt(NPCDATA_POSX_);
	m_nPosY = m_pData->GetInt(NPCDATA_POSY_);

	if (this->IsTrunkNpc()
			|| this->IsShopNpc()
				|| this->IsSceneNpc()
					|| this->IsTaskNpc()
						|| this->IsBetNpc())
	{
		IF_NOT(CreateFuncObj(idProcess))
			return false;
	}

	IF_NOT(CheckSortMutex())
		return false;

	m_setStatus = CStatusSet::CreateNew(true);
	CHECKF(m_setStatus);
	m_pMap = pMap;

	if(!GetMap()->IsValidPoint(GetPosX(), GetPosY()))
	{
		LOGERROR("Bad NPC data [%u], illegal position [%d,%d]", GetRecordID(), GetPosX(), GetPosY());
		m_pMap	= NULL;
		return false;
	}

	m_pMap->EnterRoom(QueryMapThing(), WITH_BLOCK);
	SendSelfToBlock();

	if(IsSynFlag())
	{
		GetMap()->SetSynID(GetSynID(), false);
		CheckFightTime();

		OBJID idMoreMap = GetData(SYNFLAGNPC_MOREMAPID);
		if(idMoreMap != ID_NONE)
		{
			CGameMap* pMoreMap = MapManager()->QueryMap(idMoreMap);
			if(pMoreMap)
				pMoreMap->SetSynID(GetSynID(), false);
		}
	}

	if(!IsDynaNpc() && GetMaxLife() > 0)
	{
		LOGERROR("A static NPC cannot have a nonzero max life: [%d]", GetID());
	}

	return true;
}

void CNpc::SetLookFace(int nLook, bool bSynchro)
{
	SetInt(NPCDATA_LOOKFACE, nLook);
	if(bSynchro)
	{
		CMsgUserAttrib	msg;
		IF_OK(msg.Create(this->GetID(), _USERATTRIB_LOOKFACE, nLook))
			BroadcastRoomMsg(&msg);
	}

	if(IsSceneNpc())
	{
		CNpcScene* pScene = QueryScene();
		if(pScene)
			pScene->ChangeTerrainObj(nLook);
	}
}

BOOL CNpc::	ChangePos (OBJID idMap, int nPosX, int nPosY)
{
	if (!MapManager()->QueryMap(idMap))
		return false;

	this->SendLeaveFromBlock();
	m_pMap->LeaveRoom(QueryMapThing(), WITH_BLOCK);

	m_pMap = MapManager()->QueryMap(idMap);
	m_nPosX = nPosX;
	m_nPosY = nPosY;

	m_pMap->EnterRoom(QueryMapThing(), WITH_BLOCK);
	SendSelfToBlock();

	return true;
}

bool CNpc::CreateFuncTrunk(PROCESS_ID idProcess)
{
	if(IsTrunkNpc())
	{
		MYASSERT(!m_pTrunk);
		m_pTrunk = CNpcTrunk::CreateNew();
		IF_NOT(m_pTrunk && m_pTrunk->Create(idProcess, GetRecordID(), GetData(TRUNKNPC_SIZE)))
			return false;
	}
	return true;
}
bool CNpc::CreateFuncObj(PROCESS_ID idProcess)
{
	if(IsShopNpc())
	{
		MYASSERT(!m_pShop);
		m_pShop = CNpcShop::CreateNew();
		IF_NOT(m_pShop && m_pShop->Create(idProcess, this, GetRecordID()))
			return false;
	}
	if(IsTaskNpc())
	{
		MYASSERT(!m_pTask);
		m_pTask = CNpcTask::CreateNew();
		IF_NOT(m_pTask && m_pTask->Create(idProcess, this, m_pData))
			return false;
	}
	if(IsSceneNpc())
	{
		CGameMap* pMap = MapManager()->QueryMap(m_pData->GetInt(NPCDATA_MAPID), false);
		if(!pMap)
			return false;

		MYASSERT(!m_pScene);
		m_pScene = CNpcScene::CreateNew();
		IF_NOT(m_pScene && m_pScene->Create(this, pMap))
			return false;
	}
	if(IsLinkNpc())
	{
		MYASSERT(!m_pLinkMap);
		m_pLinkMap = CNpcLink::CreateNew(m_idProcess, this);
		IF_NOT(m_pLinkMap)
			return false;
	}
	if(IsBetNpc())
	{
		MYASSERT(!m_pBet);
		m_pBet = CNpcBet::CreateNew(m_idProcess, this);
		IF_NOT(m_pBet)
			return false;
	}

	return true;
}

int CNpc::GetHelmetTypeID()
{ return IsShelfNpc() ? QueryTrunk()->QueryPackage()->GetItemType(0) : m_pData->GetInt(STATUARYDATA_HELMET); }
int CNpc::GetArmorTypeID()
{ return IsShelfNpc() ? QueryTrunk()->QueryPackage()->GetItemType(1) : m_pData->GetInt(STATUARYDATA_ARMOR); }
int CNpc::GetWeaponRTypeID()
{ return IsShelfNpc() ? QueryTrunk()->QueryPackage()->GetItemType(2) : m_pData->GetInt(STATUARYDATA_WEAPONR); }
int CNpc::GetWeaponLTypeID()
{ return IsShelfNpc() ? QueryTrunk()->QueryPackage()->GetItemType(3) : m_pData->GetInt(STATUARYDATA_WEAPONL); }
void CNpc::SendShow(IRole* pRole)
{
	if(IsStatuary() || IsShelfNpc())
	{
		CMsgPlayer msg;
		IF_OK(msg.Create(QueryRole()))
			pRole->SendMsg(&msg);
	}
	else if(IsDynaNpc() && GetMaxLife() > 0)
	{
		CMsgNpcInfoEx	msg;
		IF_OK(msg.Create(this))
			pRole->SendMsg(&msg);
	}
	else
	{
		CMsgNpcInfo	msg;
		IF_OK(msg.Create(this))
			pRole->SendMsg(&msg);
	}
}

bool CNpc::SendSelfToBlock()
{
	CHECKF(GetMap());

	if(IsStatuary() || IsShelfNpc())
	{
		CMsgPlayer msg;
		IF_OK(msg.Create(QueryRole()))
			GetMap()->BroadcastBlockMsg(QueryMapThing(), &msg);
	}
	else if(IsDynaNpc() && GetMaxLife() > 0)
	{
		CMsgNpcInfoEx	msg;
		IF_OK(msg.Create(this))
			GetMap()->BroadcastBlockMsg(QueryMapThing(), &msg);
	}
	else
	{
		CMsgNpcInfo	msg;
		IF_OK(msg.Create(this))
			GetMap()->BroadcastBlockMsg(QueryMapThing(), &msg);
	}

	return true;
}

bool CNpc::SendLeaveFromBlock()
{
	CHECKF(GetMap());

	CMsgNpc	msg;
	CHECKF(msg.Create(EVENT_LEAVEMAP, GetID()));
	GetMap()->BroadcastBlockMsg(QueryMapThing(), &msg);

	return true;
}

bool CNpc::ActivateNpc(IRole* pRole, int nAction)
{
	CUser* pUser = NULL;
	if(IsTaskNpc() && IsAlive() && pRole->QueryObj(OBJ_USER, IPP_OF(pUser)))
		return QueryTask()->ActivateTask(pUser);

	return false;
}

OBJID CNpc::GetMapID()
{
	CHECKF(m_pMap);
	return m_pMap->GetID();
}

void CNpc::OnTimer(time_t tCurr)
{
	if(IsSynFlag())
	{
		if(m_tSynEvent.ToNextTime(FIXSYNFLAG_SECS))
		{
			RecruitLife();
		}

		if(m_tBroadcastScore.ToNextTime(BROADCASTSCORE_SECS))
		{
			if(GetMap()->IsWarTime())
				BroadcastScore();

			CheckFightTime();
		}
	}

	if(IsDynaNpc() && GetType() == _SYNTRANS_NPC)
	{
		time_t	tCurr	= time(NULL);
		tm*		pTm		= localtime(&tCurr);
		if(m_bFeeOK)
		{
			if(pTm->tm_hour != 00)
				m_bFeeOK	= false;
		}
		else
		{
			if(pTm->tm_hour == 00)
			{
				m_bFeeOK	= true;

				OBJID idSyn = GetSynID();
				if(idSyn != ID_NONE)
				{
					CSyndicate*	pSyn = SynManager()->QuerySyndicate(idSyn);
					IF_OK(pSyn)
						pSyn->QueryModify()->AddData(SYNDATA_MONEY, -1*TRANSNPC_FEE, true);
				}
			}
		}
	}

	if(IsTaskNpc())
		QueryTask()->OnTimer(tCurr);
	else if(IsBetNpc())
		QueryBet()->OnTimer();
}

void CNpc::BroadcastRoomMsg(CNetMsg* pMsg, bool bSendSelf)
{
	CHECK(GetMap());

	GetMap()->BroadcastBlockMsg(QueryMapThing(), pMsg, bSendSelf);
}

void CNpc::BroadcastRoomMsg(LPCTSTR szMsg, bool bSendSelf)
{
	CMsgTalk	msg;
	if(msg.Create(GetName(), ALLUSERS_NAME, szMsg, NULL, 0xff0000, _TXTATR_SYSTEM))
		this->BroadcastRoomMsg(&msg, bSendSelf);
}

void CNpc::BeKill(IRole* pTarget , int nReserved )
{
	CHECK(IsDynaNpc());

	if(IsDeleted())
		return ;

	if(IsDieAction())
	{
		OBJID idAction = m_pData->GetInt(NPCDATA_LINKID);
		CHECK(idAction);

		GameAction()->ProcessAction(idAction, pTarget->QueryOwnerUser(), QueryRole());
	}
	else
	{
		this->SetStatus(STATUS_DIE);
		MYASSERT(DelNpc());
	}
}

bool CNpc::DelNpc()
{
	CHECKF(IsDynaNpc() && !IsDeleted());

	if(IsLinkNpc())
	{
		if(!QueryLinkMap()->EraseMap())
			return false;
	}
	if(IsSceneNpc())
	{
		QueryScene()->DelTerrainObj();
	}

	m_pData->SetInt(NPCDATA_LIFE, 0);

	m_tDie.Startup(DYNANPCDIEDELAY_SECS);

	if(IsSynFlag())
	{
		GetMap()->SetStatus(CGameMap::STATUS_WAR, false);

		GetMap()->SetSynID(ID_NONE, true);

		OBJID idMoreMap = GetData(SYNFLAGNPC_MOREMAPID);
		if(idMoreMap != ID_NONE)
		{
			CGameMap* pMoreMap = MapManager()->QueryMap(idMoreMap);
			if(pMoreMap)
				pMoreMap->SetSynID(ID_NONE, true);
		}
	}

	SendLeaveFromBlock();
	return true;
}

int CNpc::BeAttack(IRole* pTarget, int nPower, bool bReflectEnable)
{
	CHECKF(IsDynaNpc());

	return nPower;
}

int CNpc::BeAttackEx(IRole* pTarget, int nPower, bool bReflectEnable)
{
	CHECKF(IsDynaNpc());

	return nPower;
}

bool CNpc::AddAttrib(int idxAttr, __int64 i64Data, int nSynchro, DWORD* pMask)
{
	CHECKF(IsDynaNpc());
	CHECKF(idxAttr < 32);

	DWORD dwMask = pMask ? *pMask : (1 << idxAttr);
	if(i64Data)
	{
		int nDataTemp = (int)i64Data;
		switch(idxAttr)
		{
		case	_USERATTRIB_LIFE:
			{
				IF_NOT(!( IsAwardScore() && !GetMap()->IsWarTime() && nDataTemp < 0 ))
					return false;

				if ((int)this->GetLife() + nDataTemp <= 0)
				{
					m_pData->SetInt(NPCDATA_LIFE, 0);
				}
				else
					m_pData->SetInt(NPCDATA_LIFE, this->GetLife() + nDataTemp);
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

void CNpc::SaveInfo()
{
	if(m_pData && IsDynaNpc())
		m_pData->Update();
}

void CNpc::TransPos(int nPosX, int nPosY)
{
}

void CNpc::SetStatus(int nStatus, bool bSynchro)
{
	DWORD	dwOldEffect = GetEffect(0);
	switch (nStatus)
	{
	case	STATUS_NORMAL:
		{
			m_dwEffect	= 0;
		}
		break;
	case	STATUS_DIE:
		{
			m_dwEffect	&= KEEPEFFECT_NOT_CLEAR;
			m_dwEffect	|= KEEPEFFECT_DIE;
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
	case	STATUS_TEAMLEADER:
		{
			m_dwEffect	|= KEEPEFFECT_TEAMLEADER;
		}
		break;
	case	STATUS_PKVALUE:
		{
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
	case	STATUS_ATKRANGE:
		{
			m_dwEffect	|= 0x00010000;
		}
		break;
	case	STATUS_REFLECT:
		{
			m_dwEffect	|= 0x00020000;
		}
		break;
	case	STATUS_SUPERMAN:
		{
			m_dwEffect	|= 0x00040000;
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
	case	STATUS_LURKER:
		{
			m_dwEffect	|= KEEPEFFECT_LURKER;
		}
		break;
	case	STATUS_TORNADO:
		{
			m_dwEffect	|= 0x00800000;
		}
		break;
	case	STATUS_SYNCRIME:
		{
			m_dwEffect	|= KEEPEFFECT_SYNCRIME;
		}
		break;
	case	STATUS_REFLECTMAGIC:
		{
			m_dwEffect	|= 0x02000000;
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

	if(bSynchro && dwOldEffect != GetEffect(0))
	{
		CMsgUserAttrib msg;
		IF_OK(msg.Create(GetID(), _USERATTRIB_KEEPEFFECT, GetEffect(0)))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}
}

void CNpc::ClsStatus(int nStatus, bool bSynchro)
{
	DWORD	dwOldEffect = GetEffect(0);
	switch (nStatus)
	{
	case	STATUS_NORMAL:
		{
			MYASSERT(!"STATUS_NORMAL");
		}
		break;
	case	STATUS_DIE:
		{
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
	case	STATUS_TEAMLEADER:
		{
			m_dwEffect	&= ~KEEPEFFECT_TEAMLEADER;
		}
		break;
	case	STATUS_PKVALUE:
		{
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
	case	STATUS_ATKRANGE:
		{
			m_dwEffect	&= ~0x00010000;
		}
		break;
	case	STATUS_REFLECT:
		{
			m_dwEffect	&= ~0x00020000;
		}
		break;
	case	STATUS_SUPERMAN:
		{
			m_dwEffect	&= ~0x00040000;
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
	case	STATUS_LURKER:
		{
			m_dwEffect	&= ~KEEPEFFECT_LURKER;
		}
		break;
	case	STATUS_TORNADO:
		{
			m_dwEffect	&= ~0x00800000;
		}
		break;
	case	STATUS_SYNCRIME:
		{
			m_dwEffect	&= ~KEEPEFFECT_SYNCRIME;
		}
		break;
	case	STATUS_REFLECTMAGIC:
		{
			m_dwEffect	&= ~0x02000000;
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

	if(bSynchro && dwOldEffect != GetEffect(0))
	{
		CMsgUserAttrib msg;
		IF_OK(msg.Create(GetID(), _USERATTRIB_KEEPEFFECT, GetEffect(0)))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}
}

CItem* CNpc::Recycle(OBJID idUser)
{
	CHECKF(IsRecycleEnable());

	OBJID idType = GetInt(NPCDATA_ITEMTYPE);
	if(idType != 0)
	{
		if(IsDynaNpc() && GetMaxLife() > 0 && GetLife() < GetMaxLife())
			return NULL;

		CItem* pItem = CItem::CreateNew();
		IF_OK(pItem)
		{
			pItem->Create(GameDataDefault()->GetGameItemData(), idType, idUser, ITEMPOSITION_BACKPACK);
			return pItem;
		}
		else
			pItem->ReleaseByOwner();
	}

	return NULL;
}

bool CNpc::IsOwnerOf(CUser* pUser, bool bMateEnable )
{
	CHECKF(pUser);

	if(IsSynNpc() && GetInt(NPCDATA_OWNERID) == pUser->GetSynID() && pUser->GetSynRankShow() == RANK_LEADER)
		return true;

	if(IsUserNpc() && GetInt(NPCDATA_OWNERID) == pUser->GetID())
		return true;

	if(bMateEnable && IsUserNpc() && GetInt(NPCDATA_OWNERID) == pUser->GetMateID())
	{
		CTradeBuddy* pBuddy = pUser->GetTradeBuddy(pUser->GetMateID());
		if(pBuddy && !pBuddy->GetDate())
			return true;
		pUser->SendSysMsg(STR_NOT_TRADE_BUDDY);
	}

	return false;
}

bool CNpc::IsSynMoneyEmpty()
{
	CHECKF(IsSynFlag());

	CSyndicate* pSyn = SynManager()->QuerySyndicate(GetInt(NPCDATA_OWNERID));
	if(pSyn)
	{
		if(pSyn->GetInt(SYNDATA_MONEY) > 0)
			return false;
	}

	return true;
}

bool CNpc::CheckSortMutex()
{
	DWORD	dwSort = GetSort() & NPCSORT_USELINKID;

	if(GetType() == _SYNTRANS_NPC && dwSort != 0)
		return false;

	for(int i = 0; i < 32; i++)
	{
		if(dwSort == 0)
			return true;

		if((dwSort & 1) != 0 && (dwSort >>= 1) != 0)
		{
			MYASSERT(!"cq_npc(cq_dynanpc) link id mutex!!!");
			return false;
		}
	}

	return true;
}

void CNpc::CheckFightTime()
{
	time_t	tCurr	= time(NULL);
	tm*	pTm			= localtime(&tCurr);
	CHECK(pTm);
	int nWeek		= pTm->tm_wday;
	int	nHour		= pTm->tm_hour;
	int nMinute		= pTm->tm_min;
	if(nWeek == 0)
		nWeek = 7;

	int	nCurrTime	= nWeek*1000000 + nHour*10000 + nMinute*100;
	int nStartTime	= (GetData(SYNFLAGNPC_FIGHTBEGIN)/100)*100;
	int nEndTime	= (GetData(SYNFLAGNPC_FIGHTEND)/100)*100;

	if(nCurrTime >= nStartTime && nCurrTime < nEndTime)
	{
		if(!GetMap()->IsWarTime())
		{
			GetMap()->SetStatus(CGameMap::STATUS_WAR, true);
			CMsgTalk	msg;
			IF_OK_(msg.Create(STR_SYN_WAR_START))
				UserManager()->BroadcastMapMsg(GetMap()->GetID(), &msg);
 		}
	}
	else
	{
		if(GetMap()->IsWarTime())
		{
			GetMap()->SetStatus(CGameMap::STATUS_WAR, false);
			CMsgTalk	msg;
			IF_OK_(msg.Create(STR_SYN_WAR_END))
				UserManager()->BroadcastMapMsg(GetMap()->GetID(), &msg);

			CRoleSet::Iterator pRole = RoleManager()->QuerySet()->NewEnum();
			while(pRole.Next())
			{
				if(pRole && pRole->GetMap()->GetID() == GetMapID())
					pRole->ClrAttackTarget();
			}
		}
	}
}

void CNpc::RecruitLife()
{
	if(IsAlive() && GetLife() < GetMaxLife() && GetData(SYNFLAGNPC_FIXFUND) > 0)
	{
		int nFixMoney = ::CutOverflow(GetData(SYNFLAGNPC_FIXFUND), MAX_FIXMONEY);
		int nLoseLife = GetMaxLife() - GetLife();
		nFixMoney = ::CutOverflow(nFixMoney, (nLoseLife-1) / FIXLIFE_PER_MONEY + 1);
		SetData(SYNFLAGNPC_FIXFUND, ::CutTrail(GetData(SYNFLAGNPC_FIXFUND) - nFixMoney, 0), UPDATE_FALSE);

		if(GetSynID() == ID_NONE)
			return ;
		CSyndicate* pSyn = SynManager()->QuerySyndicate(GetSynID());
		if(!pSyn)
		{
			return ;
		}
		if(pSyn->GetInt(SYNDATA_MONEY) < nFixMoney)
			nFixMoney = pSyn->GetInt(SYNDATA_MONEY);
		if(nFixMoney <= 0)
		{
			SetData(SYNFLAGNPC_FIXFUND, 0, UPDATE_FALSE);
			return ;
		}

		int nFixLife = ::CutOverflow(nLoseLife, nFixMoney * FIXLIFE_PER_MONEY);
		pSyn->QueryModify()->AddData(SYNDATA_MONEY, -1*nFixMoney);
		AddAttrib(_USERATTRIB_LIFE, nFixLife, SYNCHRO_BROADCAST, NULL);

		CMsgMagicEffect	msg;
		IF_OK(msg.Create(GetID(), MAGICSORT_RECRUIT, 0, GetID(), nFixLife))
			BroadcastRoomMsg(&msg);
	}
}

void CNpc::BroadcastScore()
{
	typedef	std::multimap<int, OBJID>	TOP_SET;
	TOP_SET	setTop;
	OBJID idNpc = GetID();
	int i;
	for(i = 0; i < SynManager()->QuerySynSet()->GetAmount(); i++)
	{
		CSyndicate* pSyn = SynManager()->QuerySynSet()->GetObjByIndex(i);
		if(pSyn)
			setTop.insert(TOP_SET::value_type(pSyn->GetSynWarScore(idNpc), pSyn->GetID()));
	}

	int	nChannel = _TXTATR_SYNWAR_FIRST;
	i=1;
	for(TOP_SET::reverse_iterator iter = setTop.rbegin(); iter != setTop.rend() && i <= 5; iter++, i++)
	{
		OBJID	idSyn	= iter->second;
		int		nScore	= iter->first;
		if(nScore == 0)
			break;
		CSyndicate* pSyn = SynManager()->QuerySyndicate(idSyn);
		if(!pSyn)
			continue;

		MSGBUF	szMsg;
		sprintf(szMsg, STR_TOP_LIST_dsd, i, pSyn->GetStr(SYNDATA_NAME), nScore);
		CMsgTalk	msg;
		IF_OK(msg.Create(nChannel, szMsg))
			UserManager()->BroadcastMapMsg(GetMap()->GetID(), &msg);

		nChannel = _TXTATR_SYNWAR_NEXT;
	}
}

LPCTSTR	CNpc::GetName()
{
	if(IsSynFlag() && GetInt(NPCDATA_OWNERID) != ID_NONE)
	{
		CSyndicate* pSyn = SynManager()->QuerySyndicate(GetInt(NPCDATA_OWNERID));
		if(pSyn)
			return pSyn->GetStr(SYNDATA_NAME);
	}

	return m_pData->GetStr(NPCDATA_NAME);
}

bool CNpc::IsBeAtkable()
{
	if(IsSynFlag() && !GetMap()->IsWarTime())
		return false;

	return IsDynaNpc() && GetMaxLife() > 0;
}

bool CNpc::SetSynOwnerID(OBJID idSyn, BOOL bWithLinkMap)
{
	if(GetInt(NPCDATA_OWNERTYPE) != OWNER_SYN)
		return false;

	SetInt(NPCDATA_OWNERID, idSyn, true);

	if(bWithLinkMap && IsLinkNpc())
	{
		CGameMap* pMap = QueryLinkMap()->GetLinkMap();
		IF_OK(pMap)
			pMap->SetSynID(idSyn, true);
	}

	return true;
}

bool CNpc::IsDelEnable()
{
	if( (GetSort() & NPCSORT_DELENABLE) == 0 )
		return false;

	if(IsTrunkNpc() && !QueryTrunk()->IsEmpty())
		return false;

	return true;
}

bool CNpc::SynchroData(OBJID idNpc, OBJID idOwner, int nData0, int nData1, int nData2)
{
	if(idOwner == GetInt(NPCDATA_OWNERID))
	{
		SetData(0, nData0, UPDATE_FALSE);
		SetData(1, nData1, UPDATE_FALSE);
		SetData(2, nData2, UPDATE_TRUE);
		MapGroup(m_idProcess)->QueryIntraMsg()->DelTransNpc(idNpc, idOwner, GetRecordID(), true, true);
		return true;
	}

	MapGroup(m_idProcess)->QueryIntraMsg()->DelTransNpc(idNpc, idOwner, GetRecordID(), false, true);
	return false;
}