#include "TradeBuddy.h"
#include <math.h>
#include "AllMsg.h"
#include "MsgDataArray.h"
#include "Package.h"
#include "User.h"
#include "MapGroup.h"
#include "netmsg.h"
#include "ItemType.h"
#include "team.h"
#include "StatusOnce.h"
#include "Transformation.h"
#include "WantedList.h"
#include "PoliceWanted.h"
#include "MapTrap.h"
#include "I_MessagePort.h"
#include "DeadLoop.h"
#include "AutoPtr.h"
#include "I_Shell.h"
#include "math.h"
#include "BattleEffect.h"
#include "ClientDetector.h"
long	g_sKickoutCheat = 0;
const int	ADD_ENERGY_STAND_SECS			= 5;
const int	ADD_ENERGY_STAND				= 50;
const int	DEL_ENERGY_PELT_SECS			= 3;
const int	DEL_ENERGY_PELT					= 2;
const int	KEEP_STAND_MS					= 1500;
const int	CRUSH_ENERGY_NEED				= 100;
const int	CRUSH_ENERGY_EXPEND				= 100;
const int	SYNWAR_PROFFER_PERCENT			= 1;
const int	SYNWAR_MONEY_PERCENT			= 2;
const int	MIN_SUPERMAP_KILLS				= 25;
const int	VETERAN_DIFF_LEVEL				= 20;
const int	HIGHEST_WATER_WIZARD_PROF		= 135;
const int	MOUNT_ADD_INTIMACY_SECS			= 3600;
const int	INTIMACY_DEAD_DEC				= 20;
const int	SLOWHEALLIFE_MS					= 1000;
const int	AUTOHEALLIFE_TIME				= 10;
const int	AUTOHEALLIFE_EACHPERIOD			= 6;
const int	AUTOHEAL_MAXLIFE_TIME			= 60;
const int	AUTOHEAL_MAXLIFE_FLESH_WOUND	= 16;
const int	AUTOHEAL_MAXLIFE_GBH			= 4;
const int	TICK_SECS						= 10;
const int	MAX_PKLIMIT						= 30000;
const int	PILEMONEY_CHANGE				= 5000;
const int	_ADDITIONALPOINT_NUM			= 3;
const int	PK_DEC_TIME						= 60;
const int   PKVALUE_DEC_ONCE                = 1;
const int	PKVALUE_DEC_ONCE_IN_PRISON		= 3;
const int	PKVALUE_DEC_PERDEATH			= 10;
const int	TIME_TEAMPRC					= 5;
const int	DURABILITY_DEC_TIME				= 12;
const int	USER_ATTACK_SPEED				= 800;
const int	POISONDAMAGE_INTERVAL			= 2;
const int	WARGHOST_CHECK_INTERVAL			= 10;
const int	AUTO_REBORN_SECS				= 30;
const int	INC_POTENTIAL_SECS				= 6*60;
const int	INC_POTENTIAL_NUM				= 1;
const int	ADD_POTENTIAL_RELATIONSHIP		= 6;

const int	SPRITE_ADD_EXP_SECS				= 60;
const int	SPRITE_ADD_EXP					= 1;

const int	MAX_RETURN_BORN_POS_LEV			= 4;

struct ST_LOGOUT_RECORD
{
	DWORD	dwData;
	DWORD	dwDoubleExp;
	DWORD	dwDoubleExpSecs;
	DWORD	dwDoubleExpRemain;
};
CMyCriticalSection						s_xLogoutRecordLock;
std::map<OBJID, ST_LOGOUT_RECORD>		s_setLogoutRecord;

MYHEAP_IMPLEMENTATION(CUser,s_heap)
char	szActionTable[]			= _TBL_ACTION;
char	szTaskTable[]			= _TBL_TASK;
char	szMagicTypeTable[]		= _TBL_MAGICTYPE;
char	szBonusTable[]			= _TBL_BONUS;
char	szAddPointTable[]		= _TBL_POINTALLOT;
char	szTrapTypeTable[]		= _TBL_TRAPTYPE;
char	szMonsterTypeTable[]	= _TBL_MONSTERTYPE;
char	szRebirthTable[]		= _TBL_REBIRTH;

bool CUser::IsCheater(OBJID idKind)
{
	CHECKF(idKind < 32);

	return (m_maskCheat & (1<<idKind)) != 0;
}

bool CUser::IsInCheatJail()
{
	return GetMapID() == 6002;
}

bool	g_bCheckVerifyData		= false;

char* g_setName[] = { "_TYPE_MAGIC", "_TYPE_WS", "_TYPE_FY", "_TYPE_USE_LIFE", "_TYPE_USE_MANA",
		"_TYPE_USE_WS2_32", "_TYPE_TICK_ERROR", "_TYPE_INVALID_CLIENT", "_TYPE_ACTION_MODE", "_TYPE_VERIFY_DATA", "_TYPE_AUTO_ROBOT", "_TYPE_ZFJL", "null", "null", "null",
		"null", "null", "null", "null", "null", "null", "null", "null", "null", "null",
		"null", "null", "null", "null", "null", "null", "null", "null", "null", "null", "null",  };
void CUser::SetCheatMark(OBJID idCheatKind, DWORD dwCheatMark)
{
	CHECK(idCheatKind < 32);

	if(m_maskCheat & (1<<idCheatKind))
		return;

	::MyLogSave("gmlog/cheater", "Cheater found. Account[%u], User:[%s][%u], cheat type:[%s], subtype:[%u]", GetAccountID(), GetName(), GetID(), g_setName[idCheatKind], dwCheatMark);

	m_maskCheat = m_maskCheat | (1<<idCheatKind);
}

void CUser::KickoutCheat(OBJID idKind)
{
	CHECK(idKind < 32);
	CHECK(IsCheater(idKind));

#if	!defined(_DEBUG)
#endif

	if((int)GetLev() >= LOGIN_FREE_LEVEL && (g_sKickoutCheat&(1<<idKind)) != 0 )
	{
#if	defined(_DEBUG)
		SendSysMsg("★CHEAT★");
#endif
		UserManager()->KickOutSocket(m_idSocket, "cheat");
	}
	else
	{
		LOGCHEAT("need kick: user[%s], account[%u], type[%s]", GetName(), GetAccountID(), g_setName[idKind]);
	}
}

bool CUser::CheckJumpTime(DWORD dwTimeStamp)
{
	m_dwLastJump = dwTimeStamp;
	if(m_setJumpTime.size() < 4)
	{
		m_setJumpTime.push_back(dwTimeStamp);
		return true;
	}

	if(QueryStatus(0x2000) || QueryStatus(0x40000))
		return true;
	if(m_pTransformation && m_pTransformation->GetMonsterType() >= 2020 && m_pTransformation->GetMonsterType() <= 2024)
		return true;

	m_setJumpTime.pop_front();
	m_setJumpTime.push_back(dwTimeStamp);

	int nTotal = 0;
	for(int i = 3; i >= 1; i--)
		nTotal += m_setJumpTime[i] - m_setJumpTime[i-1];

	if(IsWing())
		return true;
	return (int)((DWORD)nTotal/3) >= 500;
}

bool CUser::IsMagicAtkCheat (OBJID idTarget, int nTargetX, int nTargetY, DWORD dwTimeStamp)
{
	IRole* pRole = RoleManager()->QueryRole(idTarget);
	if (!pRole)
		return false;

	CONST int TEST_SIZE = 8;
	m_setActionTime.push_back(dwTimeStamp);

	if (m_setActionTime.size() > TEST_SIZE)
		m_setActionTime.pop_front();

	if (m_setActionTime.size() != TEST_SIZE)
		return false;

	{
		deque<int> setDeltaTime;
		for (int i=1; i<TEST_SIZE; i++)
			setDeltaTime.push_back(m_setActionTime[i]-m_setActionTime[i-1]);

		CONST int SUSPECT_TIME = 20;

		int nSuspectCount = 0;
		for (int i=1; i<TEST_SIZE-1; i++)
		{
			if (abs(setDeltaTime[0] - setDeltaTime[i]) <= SUSPECT_TIME)
				nSuspectCount++;
		}

		if (nSuspectCount >= TEST_SIZE-2)
		{
			return true;
		}
	}

	if (nTargetX == 0 && nTargetY == 0)
		return true;

	return false;
}

void CUser::SetCheatKickout(int nSecs)
{
	if(!m_tCheatKickout.IsActive())
		m_tCheatKickout.Startup(nSecs);
}

void CUser::DoCheaterPunish	(int nData )
{
	CONST OBJID idMapCheaterPrison = 6010;
	CONST int nPosX = 34, nPosY = 74;

	if (this->GetMapID() == idMapCheaterPrison)
		return;

	this->SetRecordPos(idMapCheaterPrison, nPosX, nPosY, true);
	this->FlyMap(idMapCheaterPrison, nPosX, nPosY);
}

void CUser::SendVerifyCode(int nSecs)
{
	if(IsInCheatJail())
		return;

	m_tVerify.Startup(nSecs);
	DWORD dwTime = ::timeGetTime();
	m_nVerifyIndex = dwTime & 0xFF;

	CMsgAction	msg;
	if(msg.Create(GetID(), GetPosX(), GetPosY(), GetDir(), 100, dwTime))
		SendMsg(&msg);
}

void CUser::CheckVerifyCode(DWORD dwAnswer)
{
	int nSecs = 0;
	switch(GetID() % 10)
	{
	case 2:		nSecs = 2;		break;
	case 5:		nSecs = 5;		break;
	case 8:		nSecs = 8;		break;
	}

	if((DWORD)(m_tVerify.GetInterval() - m_tVerify.GetRemain()) < (DWORD)nSecs)
		SetCheatMark(_TYPE_INVALID_CLIENT, 0);
	else if(m_setVerifyAnswer[m_nVerifyIndex] != dwAnswer)
		SetCheatMark(_TYPE_INVALID_CLIENT, 1);
	else
		m_tVerify.Clear();
}

bool CUser::IsVerifyEnable(int nData)
{
	if(IsGM())
		return false;
	if(IsInCheatJail())
		return false;
	return GetMap()->GetID() != 1039;
}

void CUser::SendDetectMsg(int nType)
{
	if(nType != 1)
		return;

	DWORD dwData = 0, dwKey = 0;
	::CreateDetectData(&dwData, &dwKey, &m_dwDetectData);
	if(!dwData)
		return;

	CMsgAction	msg;
	if(msg.Create(GetID(), GetPosX(), GetPosY(), dwKey, 128, dwData))
		SendMsg(&msg);
}

void CUser::VerifyDataFailed(int nData)
{
	if(IsInCheatJail())
		return;
	if(!g_bCheckVerifyData)
		return;
	if(::RandGet(3) != 1)
		return;

	::MyLogSave("gmlog/cheater", "cheater[%s] found: verify data failed.  account id=[%u]", GetName(), GetAccountID());
	GotoJail(6002, STR_GOTO_CHEATJAIL);
}

void CUser::OnDetectMsg(int nType, DWORD dwData)
{
	if(nType != 1 || !m_pVerifyDataDetector)
		return;

	if(dwData == m_dwDetectData)
	{
		::DebugSave("detect msg receive and matched at %u", ::timeGetTime());
		m_pVerifyDataDetector->OnMatched();
	}
	else
		::MyLogSave("gmlog/cheater", "!!!![%s] - account id [%u] may be a cheat maker; ban the account!", GetName(), GetAccountID());
}

CUser::CUser()
{
	SetObjType(OBJ_USER);

	m_link.Init(this);
	m_nStep			= STEP_LOGIN;
	m_dwUnk6AC		= 0;
	m_dwTimerCount		= 0;

	m_pWeaponR		= NULL;
	m_pWeaponL		= NULL;
	m_pHelmet		= NULL;
	m_pNecklace		= NULL;
	m_pArmor		= NULL;
	m_pRingR		= NULL;
	m_pRingL		= NULL;
	m_pShoes		= NULL;
	m_pOvercoat		= NULL;
	m_pMap			= NULL;

	m_bUpdateWeight	= true;

	m_nPosX			= 0;
	m_nPosY			= 0;
	m_nDir			= 0;
	m_nPose			= 0;
	m_nEmotin		= 0;

	m_dwXP			= 0;
	m_pTeam			= NULL;

	m_dwFirstServerTick	= 0;
	m_dwLastServerTick	= 0;

	m_dwLastClientTick	= 0;
	m_dwFirstClientTick	= 0;
	m_dwLastCoolShow	= 0;

	m_dwLastRcvClientTick = 0;
	m_nUnk858		= 0;

	m_ifaceSquareDeal.m_pParent	= this;

	m_pBattleSystem	= NULL;
	m_pStorage		= NULL;
	m_idTaskNpc		= ID_NONE;
	m_idTaskItem	= ID_NONE;
	m_idTaskItemAction	= ID_NONE;
	m_bDebugAction	= false;

	m_pUserWeaponSkillSet	= CUserWeaponSkillSet::CreateNew(true);
	m_pMagic		= NULL;
	m_pSyn			= NULL;
	m_setStatus		= NULL;
	m_bGhost		= false;

	m_dwUnk1C		= 0;
	m_dwUnk20		= 0;
	m_nUnk24		= 100;
	m_bUnk28		= true;
	m_dwUnk2C		= 0;

	m_bHaveLeaveWord	= true;	m_dwUnkF4	= 0;
	m_pTransformation	= NULL;
	m_dwUnk610		= 0;

	m_pBooth		= NULL;
	m_nDoubleExp	= 100;
	m_idxLastArrow	= -1;

	m_tFootPrint	= 0;
	m_posFootPrint.x	= 0;
	m_posFootPrint.y	= 0;

	m_nCurrSupermanKills	= 0;
	m_nTopSupermanKills		= 0;
	m_nSupermanOrder		= 0;

	m_dwLastJump = 0;
	m_dwMsgCount = 0;

	m_nVerifyIndex		= 0;
	m_dwDetectData		= 0;
	m_pStatisticSet		= NULL;
	m_dwUnk638			= 0;
	m_dwUnk63C			= 0;
	m_dwUnk5B0			= 0;
	m_pAnalyser			= CActionAnalyser::CreateNew(20, 500, 100, 1500);
	m_nUnk8D4			= -1;
	m_i64LastSaveExp	= 0;
	m_i64ContributeExp	= 0;
	m_pVerifyDataDetector	= NULL;
	m_dwLastSaveExpTime	= 0;
	m_dwUnk914			= 0;
	m_dwUnk8CC			= 0;
	m_nUnk8D0			= 2;
	m_dwUnk8D8			= 0;
	m_dwUnk8E8			= 0;
	m_dwUnk8EC			= 0;
	m_dwUnk34			= 0;
}

CUser::~CUser()
{
	{
		CMySingleLock xLock(&s_xLogoutRecordLock, true);
		ST_LOGOUT_RECORD rec;
		rec.dwData				= m_dwUnk638;
		rec.dwDoubleExp			= GetTimeCardStatusRemain();
		rec.dwDoubleExpSecs		= 0;
		rec.dwDoubleExpRemain	= 0;
		if(!m_tDoubleExp.IsTimeOut())
		{
			rec.dwDoubleExpSecs		= m_nDoubleExp;
			rec.dwDoubleExpRemain	= m_tDoubleExp.GetRemain();
		}
		s_setLogoutRecord[GetID()]	= rec;
	}

	if (m_pBattleSystem)
	{
		delete m_pBattleSystem;
		m_pBattleSystem	= NULL;
	}
	if(m_pStorage)
		m_pStorage->Release();

	if (m_pUserWeaponSkillSet)
		m_pUserWeaponSkillSet->Release();

	if (m_pMagic)
		m_pMagic->ReleaseByOwner();
	if (m_pSyn)
		m_pSyn->ReleaseByOwner();

	if (m_setStatus)
		m_setStatus->Release();

	if (m_pTeam)
		m_pTeam->ReleaseByOwner();
	if (m_pTransformation)
		m_pTransformation->Release();

	SAFE_RELEASE(m_pStatisticSet);
	SAFE_DELETE(m_pAnalyser);
	SAFE_DELETE(m_pVerifyDataDetector);
}

void CUser::OnLogout()
{
	DEBUG_TRY
	if(m_pMap)
	{
		DetachLuckyStatus();
		LeaveMapGroup();
	}
	DestroyBooth();
	DEBUG_CATCH("CUser::OnLogout")
}

CUser* CUser::CreateNewUser(PROCESS_ID idProcess, SOCKET_ID idSocket, UserInfoStruct* pInfo)
{
	CHECKF(CMapGroup::IsValidMapGroupProcessID(idProcess));
	CHECKF(pInfo);

	CUserPtr pUser = CUser::CreateNew();
	CHECKF(pUser);
	if(pUser)
	{
		pUser->m_idProcess	= idProcess;
		pUser->m_idSocket	= idSocket;
		memcpy(&pUser->m_data.m_Info, pInfo, sizeof(UserInfoStruct));
		pUser->m_data.m_pRes = MapGroup(idProcess)->QueryDataDefault()->GetUserData()->CreateNewRecord(pInfo->id);
		if(!pUser->m_data.m_pRes)
		{
			pUser->ReleaseByOwner();
			return NULL;
		}
	}

	pUser->Init();
	pUser->QueryEnemy()->Create(idProcess, pUser);
	pUser->m_pMap	= NULL;

	if(pUser->m_data.m_Info.dwMonsterType && pUser->m_data.m_Info.dwMonsterSecs)
	{
		MYASSERT(pUser->Transform(pUser->m_data.m_Info.dwMonsterType, pUser->m_data.m_Info.dwMonsterSecs));
	}

	return pUser;
}

bool CUser::CreateBooth(int nPosX, int nPosY, int nDir)
{
	if(m_pBooth)
		return false;

	if(nPosX != m_nPosX+1 && nPosY != m_nPosY)
		return false;

	CGameMap* pMap = m_pMap;
	if(!pMap || !pMap->IsBoothEnable())
		return false;

	CNpc* pFlag = NULL;
	if(!pMap->QueryObjInPos(m_nPosX-2, m_nPosY, OBJ_NPC, IPP_OF(pFlag)) || !pFlag)
		return false;
	if(pFlag->GetType() != _ROLE_BOOTH_FLAG_NPC)
		return false;

	CNpc* pNpc = NULL;
	if(pMap->QueryObjInPos(nPosX, nPosY, OBJ_NPC, IPP_OF(pNpc)))
		return false;

	return (m_pBooth = CBooth::CreateNew(m_idProcess, this, nPosX, nPosY, nDir)) != NULL;
}

bool CUser::EnterMap()
{
	if(m_nStep != STEP_LOGIN)
		return false;

	SendLight();

	CMapPtr pMap = m_pMap;
	if(!pMap)
		return false;

	pMap->EnterRoom(this->QueryMapThing(), false);
	pMap->SendBlockInfo((IRole*)this);
	pMap->SendRegionInfo(this);
	pMap->SendMapInfo(this);

	SendSelfToBlock();

	if(!pMap->IsWingEnable())
		QueryStatusSet()->DelObj(STATUS_WING);

	m_tLock.Startup(CHGMAP_LOCK_SECS);
	m_nStep = STEP_ENTERMAP;
	return true;
}

bool CUser::SendSelfToBlock()
{
	if(!m_pMap)
		return false;
	if(m_pMap->IsNewbieMap())
		return true;

	CMsgPlayer msg;
	if (msg.Create((IRole*)this, 0))
		this->BroadcastRoomMsg(&msg, EXCLUDE_SELF);

	return true;
}

bool CUser::AddAttrib(int idxAttr, __int64 i64Data, int nSynchro, DWORD* pMask)
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
					SetLife(0, false);
					m_nUnk858	= 0;
				}
				else
				{
					if(nDataTemp < 0 && m_nUnk858 > 0)
					{
						m_nUnk858 += nDataTemp;
						if(m_nUnk858 < 0)
							m_nUnk858 = 0;
					}
					SetLife(__min(this->GetMaxLife(), this->GetLife() + nDataTemp), false);
				}
			}
			break;
		case	_USERATTRIB_MANA:
			{
				if ((int)this->GetMana() + nDataTemp <= 0)
					m_data.SetMana(0, false);
				else
					m_data.SetMana(__min(this->GetMaxMana(), this->GetMana() + nDataTemp), false);
			}
			break;
		case	_USERATTRIB_LEV:
			{
				IF_NOT (nDataTemp >= 0)
					return false;
				m_data.SetLev(__min(g_nMaxLevel, this->GetLev() + nDataTemp), true);
			}
			break;
		case	_USERATTRIB_MONEY:
			{
				if (nDataTemp < 0 && (int)this->GetMoney() + nDataTemp < 0)
					return false;
				m_data.SetMoney(GetMoney() + nDataTemp, false);
			}
			break;
		case	_USERATTRIB_EXP:
			{
				__int64 i64Exp = ::CutRange(this->GetExp() + i64Data, (__int64)0x8000000000000008, (__int64)0x7FFFFFFFFFFFFFF8);
				if(i64Exp < 0)
					i64Exp = 0;
				m_data.SetExp(i64Exp, false);
			}
			break;
		case	_USERATTRIB_PK:
			m_data.SetPk(this->GetPk() + nDataTemp, true);
			break;
		case _USERATTRIB_ENERGY:
			AddEnergy(nDataTemp);
			break;
		case _USERATTRIB_ADDPOINT:
			m_data.SetAddPoint(m_data.GetAddPoint() + nDataTemp, true);
			break;
		case _USERATTRIB_SOUL:
			m_data.SetSoul(m_data.GetSoul() + nDataTemp, true);
			break;
		case _USERATTRIB_HEALTH:
			m_data.SetHealth(m_data.GetHealth() + nDataTemp, true);
			break;
		case _USERATTRIB_FORCE:
			m_data.SetForce(m_data.GetForce() + nDataTemp, true);
			break;
		case _USERATTRIB_SPEED:
			m_data.SetSpeed(m_data.GetSpeed() + nDataTemp, true);
			break;
		case _USERATTRIB_VIRTUE:
			m_data.SetVirtue(m_data.GetVirtue() + nDataTemp, true);
			break;
		case _USERATTRIB_XP:
			m_dwXP = ::CutRange(m_dwXP + nDataTemp, 0, MAX_USER_XP);
			break;
		case _USERATTRIB_UNK29:
			{
				if(nDataTemp < 0 && (int)m_dwUnk638 < -nDataTemp)
					m_dwUnk638	= 0;
				else
				{
					m_dwUnk638 += nDataTemp;
					if(m_dwUnk638 > 7200000)
						m_dwUnk638	= 7200000;
				}
			}
			break;
		case _USERATTRIB_EMONEY:
			{
				int nEmoney = m_data.m_Info.dwEmoney + nDataTemp;
				if(nEmoney < 0)
					return false;
				m_data.SetEmoney(nEmoney);
			}
			break;
		default:
			return false;
		}
		dwMask |= (1 << idxAttr);
	}

	if(nSynchro != SYNCHRO_FALSE && dwMask)
	{
		CMsgUserAttrib	msg;
		if (!msg.Create(GetID(), _USERATTRIB_NONE, 0))
			return false;

		bool bAppend = false;
		if(dwMask & (1 << _USERATTRIB_LIFE))
			bAppend = msg.Append(_USERATTRIB_LIFE, this->GetLife());
		else if(dwMask & (1 << _USERATTRIB_MANA))
			bAppend = msg.Append(_USERATTRIB_MANA, this->GetMana());
		else if(dwMask & (1 << _USERATTRIB_EXP))
			bAppend = msg.Append(_USERATTRIB_EXP, this->GetExp());
		else if(dwMask & (1 << _USERATTRIB_PK))
			bAppend = msg.Append(_USERATTRIB_PK, this->GetPk());
		else if(dwMask & (1 << _USERATTRIB_MONEY))
			bAppend = msg.Append(_USERATTRIB_MONEY, this->GetMoney());
		else if(dwMask & (1 << _USERATTRIB_EMONEY))
			bAppend = msg.Append(_USERATTRIB_EMONEY, m_data.m_Info.dwEmoney);
		else if(dwMask & (1 << _USERATTRIB_ADDPOINT))
			bAppend = msg.Append(_USERATTRIB_ADDPOINT, this->GetAddPoint());
		else if(dwMask & (1 << _USERATTRIB_LEV))
			bAppend = msg.Append(_USERATTRIB_LEV, this->GetLev());
		else if(dwMask & (1 << _USERATTRIB_SOUL))
			bAppend = msg.Append(_USERATTRIB_SOUL, m_data.GetSoul());
		else if(dwMask & (1 << _USERATTRIB_HEALTH))
			bAppend = msg.Append(_USERATTRIB_HEALTH, m_data.GetHealth());
		else if(dwMask & (1 << _USERATTRIB_FORCE))
			bAppend = msg.Append(_USERATTRIB_FORCE, m_data.GetForce());
		else if(dwMask & (1 << _USERATTRIB_SPEED))
			bAppend = msg.Append(_USERATTRIB_SPEED, m_data.GetSpeed());
		else if(dwMask & (1 << _USERATTRIB_XP))
			bAppend = msg.Append(_USERATTRIB_XP, m_dwXP);
		else if(dwMask & (1 << _USERATTRIB_ENERGY))
			bAppend = msg.Append(_USERATTRIB_ENERGY, this->GetEnergy());
		else if(dwMask & (1 << _USERATTRIB_VIRTUE))
			bAppend = msg.Append(_USERATTRIB_VIRTUE, m_data.GetVirtue());
		else if(dwMask & (1 << _USERATTRIB_UNK29))
			bAppend = msg.Append(_USERATTRIB_UNK29, m_dwUnk638);
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

void CUser::SetAttrib(int idxAttr, __int64 i64Data, int nSynchro)
{
	CMsgUserAttrib	msg;
	if(!msg.Create(GetID(), _USERATTRIB_NONE, 0))
		return;

	switch(idxAttr)
	{
	case	_USERATTRIB_ENERGY:
		{
			this->SetEnergy(i64Data);
			if(!msg.Append(_USERATTRIB_ENERGY, this->GetEnergy()))
				return;
		}
		break;

	case	_USERATTRIB_PK:
		{
			m_data.SetPk(i64Data, true);
			if(!msg.Append(_USERATTRIB_PK, this->GetPk()))
				return;
		}
		break;
	case	_USERATTRIB_LIFE:
		{
			i64Data = __max(0, __min(this->GetMaxLife(), i64Data));

			SetLife(i64Data);

			if(!msg.Append(_USERATTRIB_LIFE, GetLife()))
				return;
		}
		break;
	case _USERATTRIB_LEV:
		{
			int nNewLev = __max(0, __min(g_nMaxLevel, i64Data));

			m_data.SetLev(nNewLev);
			UpdateBattlePower();
			if(!msg.Append(_USERATTRIB_LEV, GetLev()))
				return;

		}
		break;
	case _USERATTRIB_ADDPOINT:
		{
			i64Data = __max(0, i64Data);

			m_data.SetAddPoint(i64Data);
			if(!msg.Append(_USERATTRIB_ADDPOINT, GetAddPoint()))
				return;
		}
		break;
	case	_USERATTRIB_MANA:
		{
			i64Data = __max(0, __min(this->GetMaxMana(), i64Data));

			m_data.SetMana(i64Data);
			if(!msg.Append(_USERATTRIB_MANA, GetMana()))
				return;
		}
		break;
	case	_USERATTRIB_EMONEY:
		{
			MYASSERT(!"not support chg emoney here!");
		}
		break;
	case _USERATTRIB_FORCE:
		{
			m_data.SetForce(__max(0, (int)i64Data));
			if(!msg.Append(_USERATTRIB_FORCE, GetForce()))
				return;
		}
		break;
	case _USERATTRIB_SOUL:
		{
			m_data.SetSoul(i64Data);
			if(!msg.Append(_USERATTRIB_SOUL, GetSoul()))
				return;
		}
		break;
	case _USERATTRIB_HEALTH:
		{
			m_data.SetHealth(i64Data);
			if(!msg.Append(_USERATTRIB_HEALTH, GetHealth()))
				return;
		}
		break;
	case _USERATTRIB_SPEED:
		{
			m_data.SetSpeed(i64Data);
			if(!msg.Append(_USERATTRIB_SPEED, GetSpeed()))
				return;
		}
		break;
	case _USERATTRIB_XP:
		{
			m_dwXP = ::CutRange((int)i64Data, 0, MAX_USER_XP);
			if(!msg.Append(_USERATTRIB_XP, m_dwXP))
				return;
		}
		break;
	case	_USERATTRIB_EXP:
		{
			m_data.SetExp(i64Data);
			if(!msg.Append(_USERATTRIB_EXP, GetExp()))
				return;
		}
		break;
	case	_USERATTRIB_MONEY:
		{
			m_data.SetMoney(__max(0, (int)i64Data));
			if(!msg.Append(_USERATTRIB_MONEY, GetMoney()))
				return;
		}
		break;
	case	_USERATTRIB_KEEPEFFECT:
		{
			ResetEffect(0xFFFFFFFF, 0);
			this->SetEffect((DWORD)i64Data, 0);
			if(!msg.Append(idxAttr, this->GetEffect(0)))
				return;
		}
		break;
	case	_USERATTRIB_HAIR:
		{
			m_data.SetHair(i64Data);

			if(!msg.Append(idxAttr, m_data.GetHair()))
				return;

			if (nSynchro != SYNCHRO_FALSE)
				nSynchro = SYNCHRO_BROADCAST;
		}
		break;
	case	_USERATTRIB_PORFESSION:
		{
			m_data.SetProfession(i64Data, true);
			if(!msg.Append(idxAttr, m_data.GetProfession()))
				return;
		}
		break;
	case	_USERATTRIB_LOOKFACE:
		{
			DWORD dwOldBody = this->GetLookFace() % 1000;
			DWORD dwNewBody = (DWORD)i64Data % 1000;
			if(dwOldBody <= 4 && dwNewBody <= 4 && dwOldBody != dwNewBody)
				m_data.SetLookFace(i64Data, true);
			if(!msg.Append(idxAttr, i64Data))
				return;
		}
		break;
	case	_USERATTRIB_SUPERMAP_ORDER:
		{
			if(!msg.Append(idxAttr, i64Data))
				return;
		}
		break;
	case	_USERATTRIB_METEMPSYCHOSIS:
		{
			i64Data = __max(0, i64Data);

			m_data.SetMetempsychosis(i64Data, true);
			if(!msg.Append(idxAttr, i64Data))
				return;
		}
		break;
	case	_USERATTRIB_UNK29:
		{
			m_dwUnk638	= __max(0, i64Data);
			if(!msg.Append(idxAttr, m_dwUnk638))
				return;
		}
		break;

	default:
		return;
	}

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

bool CUser::SynPosition(int nPosX, int nPosY, int nMaxDislocation )
{
	if (nMaxDislocation <= 0 || (nPosX == 0 && nPosY == 0))
		return true;

	int nDislocation = this->GetDistance(nPosX, nPosY);
	if (nDislocation >= nMaxDislocation)
		return false;

	if (nDislocation <= 0)
		return true;

	if (this->IsGM())
		this->SendSysMsg("syn move: (%u, %u)->(%u, %u)",
									this->GetPosX(), this->GetPosY(),
									nPosX, nPosY);

	CMapPtr pMap = this->GetMap();
	IF_NOT(pMap && pMap->IsValidPoint(nPosX, nPosY))
		return false;

	CMsgAction msg;
	if (msg.Create(this->GetID(), nPosX, nPosY, this->GetDir(), actionSynchro, nPosX, nPosY))
		this->BroadcastRoomMsg(&msg, EXCLUDE_SELF);

	this->ProcessOnMove(MOVEMODE_SYNCHRO);
	this->JumpPos(nPosX, nPosY);
	return true;
}

void CUser::ProcessAfterMove()
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

void CUser::ProcessOnMove(int nMoveMode)
{
	if(QueryBooth())
		QueryBooth()->LeaveMap();

	SetPose(_ACTION_STANDBY);

	if(nMoveMode != MOVEMODE_JUMPMAGICATTCK
		&& nMoveMode != MOVEMODE_COLLIDE && nMoveMode != MOVEMODE_SYNCHRO)
	{
		ClrAttackTarget();
	}

	if(nMoveMode == MOVEMODE_RUN || nMoveMode == MOVEMODE_JUMP
		|| (nMoveMode == MOVEMODE_WALK && QueryMagic()->IsKeepBow()))
	{
		QueryMagic()->AbortMagic(false);

		ResetEnergyInc();
	}

	if(nMoveMode != MOVEMODE_SHIFT && nMoveMode != MOVEMODE_SYNCHRO)
		QueryStatusSet()->DelObj(STATUS_WEAPONDAMAGE);

	if(nMoveMode != MOVEMODE_SHIFT && nMoveMode != MOVEMODE_SYNCHRO)
		QueryStatusSet()->DelObj(STATUS_KEEPBOW);

	this->StopMine();

	m_tFootPrint = ::TimeGet();
	m_posFootPrint.x = this->GetPosX();
	m_posFootPrint.y = this->GetPosY();

	m_tLock.Clear();
	DetachLuckyStatus();
	m_dwUnk63C	= ::TimeGet();
}

void CUser::ProcessOnAttack()
{
	m_tLock.Clear();
	DetachLuckyStatus();
	m_dwUnk63C	= ::TimeGet();
}
void CUser::GetFootPrint	(int& nPosX, int& nPosY)
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

void CUser::DetachLuckyStatus()
{
	if(!QueryStatusSet())
		return;

	if(QueryStatus(STATUS_LUCKY))
	{
		QueryStatusSet()->DelObj(STATUS_LUCKY);
		AddAttrib(_USERATTRIB_UNK29, 0, SYNCHRO_TRUE);
	}

	if(QueryStatus(STATUS_LUCKY_PRAY))
	{
		QueryStatusSet()->DelObj(STATUS_LUCKY_PRAY);
		AddAttrib(_USERATTRIB_UNK29, 0, SYNCHRO_TRUE);

		MAPTHING_SET	setThing;		POINT pos;	pos.x = GetPosX();	pos.y = GetPosY();
		GetMap()->CollectMapThing(setThing, pos, 3, OBJ_USER);
		for(MAPTHING_SET::iterator iter = setThing.begin(); iter != setThing.end(); iter++)
		{
			IMapThing* pThing = *iter;
			CUser* pUser = NULL;
			if(pThing && pThing->QueryObj(OBJ_USER, IPP_OF(pUser)) && pUser->GetID() != GetID())
			{
				pUser->QueryStatusSet()->DelObj(STATUS_LUCKY);
				pUser->AddAttrib(_USERATTRIB_UNK29, 0, SYNCHRO_TRUE);
			}
		}
	}
}

void CUser::SetLife(int nLife, BOOL bUpdate)
{
	if(QueryTransformation())
	{
		if(nLife > 0)
		{
			QueryTransformation()->SetLife(nLife);

			nLife = ::CutTrail(1, MulDiv(nLife, GetMaxLife(), QueryTransformation()->GetMaxLife()));
		}
		else
		{
			SAFE_RELEASE(m_pTransformation);
			SynchroTransform();
		}
	}

	m_data.SetLife(nLife, bUpdate);
}

DWORD CUser::GetLife()
{
	if(QueryTransformation())
		return QueryTransformation()->GetLife();

	int nMaxLife = this->GetMaxLife();
	if (m_data.GetLife() > nMaxLife)
		m_data.SetLife(nMaxLife);

	return m_data.GetLife();
}

DWORD CUser::GetMaxLife()
{
	if(QueryTransformation())
		return QueryTransformation()->GetMaxLife();

	int nMaxLife = (m_data.GetHealth()*8 + m_data.GetSoul() + m_data.GetSpeed() + m_data.GetForce()) * 3;
	if(GetProfessionSort() == 1)
	{
		int nPercent = 0;
		switch(m_data.GetProfession() % 10)
		{
		case 1:	nPercent = 5;	break;
		case 2:	nPercent = 8;	break;
		case 3:	nPercent = 10;	break;
		case 4:	nPercent = 12;	break;
		case 5:	nPercent = 15;	break;
		}
		nMaxLife += nMaxLife * nPercent / 100;
	}

	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if(pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
			nMaxLife += pEquip->GetInt(ITEMDATA_LIFE) + pEquip->GetAddition(3) + pEquip->GetInt(ITEMDATA_ADD_LIFE);
	}

	return __max(0, nMaxLife);
}

DWORD CUser::GetMaxMana()
{
	int nMaxMana = m_data.GetSoul()*5;
	if(GetProfessionSort() > 10)
	{
		int nLevel = m_data.GetProfession() % 10;
		if(nLevel >= 5)
			nMaxMana = m_data.GetSoul()*30;
		else if(nLevel >= 4)
			nMaxMana = m_data.GetSoul()*25;
		else if(nLevel >= 3)
			nMaxMana = m_data.GetSoul()*20;
		else if(nLevel >= 2)
			nMaxMana = m_data.GetSoul()*15;
		else if(nLevel >= 1)
			nMaxMana = m_data.GetSoul()*10;
	}

	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if(pEquip)
			nMaxMana += pEquip->GetInt(ITEMDATA_MANA);
	}

	return __max(0, nMaxMana);
}

DWORD CUser::GetAttack()
{
	if(QueryTransformation())
		return QueryTransformation()->GetAttack();

	return m_data.GetForce();
}

DWORD CUser::GetMinAtk()
{
	if(QueryTransformation())
		return QueryTransformation()->GetMinAtk();

	int nMinAtk	= this->GetAttack();

	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
		{
			int nEquipAtk = pEquip->GetMinAtk() + pEquip->GetAddition(5);

			if (ITEMPOSITION_WEAPONL == i && !pEquip->IsShield())
				nEquipAtk = nEquipAtk/2;

			nMinAtk += nEquipAtk;
		}
	}

	return __max(0, nMinAtk);
}

DWORD CUser::GetMaxAtk()
{
	if(QueryTransformation())
		return QueryTransformation()->GetMaxAtk();

	int nMaxAtk	= this->GetAttack();

	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
		{
			int nEquipAtk = pEquip->GetMaxAtk() + pEquip->GetAddition(4);

			if (ITEMPOSITION_WEAPONL == i && !pEquip->IsShield())
				nEquipAtk = nEquipAtk/2;

			nMaxAtk += nEquipAtk;
		}
	}

	return __max(0, nMaxAtk);
}

DWORD CUser::GetDefence()
{
	if(QueryTransformation())
		return QueryTransformation()->GetDefence();

	return 0;
}

DWORD CUser::GetMetempsychosisRate(IRole* pTarget)
{
	int nRate;
	if(QueryTransformation())
		nRate = QueryTransformation()->GetDefence2();
	else
	{
		nRate = 7000;
		if(!GetMetempsychosis() || m_data.GetProfession() % 10 < 3)
			nRate = 10000;
		if(GetMetempsychosis() >= 2)
		{
			if(pTarget->IsMonster())
				nRate = 7000;
			else
			{
				CUser* pUser = NULL;
				if(pTarget->QueryObj(OBJ_USER, IPP_OF(pUser)))
					nRate = (pUser->GetMetempsychosis() >= 2) ? 7000 : 5000;
			}
		}
	}
	return nRate;
}

DWORD CUser::GetDef()
{
	if(QueryTransformation())
		return QueryTransformation()->GetDef();

	int nDef = this->GetDefence();

	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if(pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
			nDef += pEquip->GetDef() + pEquip->GetAddition(6);
	}

	return __max(0, nDef);
}

DWORD CUser::GetDexterity()
{
	if(QueryTransformation())
		return QueryTransformation()->GetDexterity();

	return m_data.GetSpeed();
}

DWORD CUser::GetDex()
{
	if(QueryTransformation())
		return (DWORD)((__int64)QueryTransformation()->GetDex() * 100.0f);

	int nDex = 0;
	int nUnused = 0;
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if(pEquip && i != ITEMPOSITION_WEAPONL && pEquip->GetInt(ITEMDATA_AMOUNT))
		{
			nDex += pEquip->GetDexteriy() + pEquip->GetGemHitRateEffect();
			nDex += pEquip->GetAddition(9);
			nUnused += pEquip->GetGemHitRateEffect();
		}
	}

	return (DWORD)((__int64)this->GetDexterity() * (nDex + 100.0f) * 0.01f * 100.0f);
}

DWORD CUser::GetDodge()
{
	if(QueryTransformation())
		return QueryTransformation()->GetDodge();

	return 50;
}

DWORD CUser::GetDdg()
{
	if(QueryTransformation())
		return QueryTransformation()->GetDdg();

	int nDge = this->GetDodge();

	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if(pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
			nDge +=	pEquip->GetDodge() + pEquip->GetAddition(0x0A);
	}

	IStatus * pStatus = QueryStatus(STATUS_ADJUST_DODGE);
	if (pStatus)
	{
		nDge = AdjustData(nDge, pStatus->GetPower());
	}

	return __max(0, nDge);
}

DWORD CUser::GetMagicAttack()
{
	if(QueryTransformation())
		return QueryTransformation()->GetAttack();

	int nAtk = 0;
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if(pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
			nAtk += pEquip->GetInt(ITEMDATA_MAGICATK);
	}

	return __max(0, nAtk);
}

DWORD CUser::GetMagicDef()
{
	if(QueryTransformation())
		return QueryTransformation()->GetMagicDef();

	int nDef = 0;
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if(pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
			nDef += pEquip->GetMgcDef();
	}

	IStatus * pStatus = QueryStatus(STATUS_ADJUST_MAGICDEF);
	if (pStatus)
		nDef = AdjustData(nDef, pStatus->GetPower());
	if (nDef > 90)
		nDef = 90;

	return __max(0, nDef);
}

int CUser::GetAddPoint()
{
	return m_data.GetAddPoint();
}

DWORD CUser::GetLev()
{
	return m_data.GetLev();
}

DWORD CUser::GetMeteLev()
{
	return m_data.m_Info.dwMeteLev;
}

void CUser::IncLev(int nLev, bool* pbSatori)
{
	if(pbSatori)
		*pbSatori	= false;

	int nOldLev = this->GetLev();
	if(nOldLev >= g_nMaxLevel)
		return;

	int nNewLev	= nOldLev + nLev;
	this->SetAttrib(_USERATTRIB_LEV, nNewLev, SYNCHRO_TRUE);

	if(nOldLev < LOGIN_FREE_LEVEL && nNewLev >= LOGIN_FREE_LEVEL)
		MapGroup(PID)->QueryIntraMsg()->LevelUp(GetID(), nNewLev);

	QueryMagic()->OwnerUpLevel(nNewLev);
	if(GetSynID() != ID_NONE)
		QuerySynAttr()->UpLevel(nNewLev);

	CLevupexpData* pLevupexp	= LevupexpSet()->GetObj(nOldLev);
	CHECK(pLevupexp);
	__int64	i64LevupExp	= pLevupexp->GetInt64(LEVUPEXPDATA_EXP);
	if(nOldLev < 40)
	{
		CTeam* pTeam = this->GetTeam();
		if(pTeam && pTeam->IsAwardVirtueEnable(this))
		{
			CUser* pLeader = UserManager()->GetUser(pTeam->GetLeader());
			if(pLeader)
			{
				int nAwardVirtue	= i64LevupExp / (nOldLev*30);
				if(pLeader->IsIncomeLimited())
					nAwardVirtue	= nAwardVirtue * pLeader->GetIncomePercent() / 100;
				pLeader->AddAttrib(_USERATTRIB_VIRTUE, nAwardVirtue, SYNCHRO_TRUE);

				{
					CMsgTeamAward	msg;
					IF_OK(msg.Create(0, 0, nAwardVirtue))
						pLeader->SendMsg(&msg);
				}

				char	szMsg[1024];
				sprintf(szMsg, STR_AWARD_VIRTUE_su, pLeader->GetName(), nAwardVirtue);
				CMsgTalk	msg;
				IF_OK(msg.Create(szMsg))
					pTeam->BroadcastTeamMsg(&msg);
			}
		}
	}
	else if(nOldLev < 70)
	{
		CTeam* pTeam = this->GetTeam();
		if(pTeam && pTeam->IsAwardVirtueEnable(this))
		{
			CUser* pLeader = UserManager()->GetUser(pTeam->GetLeader());
			if(pLeader)
			{
				int nFactor	= 0;
				if(nOldLev < 50)
					nFactor	= 3;
				else if(nOldLev < 60)
					nFactor	= 4;
				else
					nFactor	= (nOldLev < 65) ? 6 : 10;
				int nAwardVirtue	= i64LevupExp*0.9/(nFactor*nOldLev*30) + i64LevupExp*0.1/(nOldLev*30);
				if(pLeader->IsIncomeLimited())
					nAwardVirtue	= nAwardVirtue * pLeader->GetIncomePercent() / 100;
				pLeader->AddAttrib(_USERATTRIB_VIRTUE, nAwardVirtue, SYNCHRO_TRUE);

				{
					CMsgTeamAward	msg;
					IF_OK(msg.Create(0, 0, nAwardVirtue))
						pLeader->SendMsg(&msg);
				}

				char	szMsg[1024];
				sprintf(szMsg, STR_AWARD_VIRTUE_su, pLeader->GetName(), nAwardVirtue);
				CMsgTalk	msg;
				IF_OK(msg.Create(szMsg))
					pTeam->BroadcastTeamMsg(&msg);
			}
		}
	}

	GameAction()->ProcessAction(80000004, this);

	CMsgAction	msg;
	if(msg.Create(GetID(), 0, 0, 0, 92, 0, 0))
		BroadcastRoomMsg(&msg, INCLUDE_SELF);

	if(GetLev() >= 130)
	{
		int nLev		= GetLev();
		int nMeteLev	= m_data.m_Info.dwMeteLev;
		int nSatoriLev	= nMeteLev / 100000;
		if(nSatoriLev >= 130 && nLev*100000 < nMeteLev)
		{
			int nLevBefore = GetLev();
			this->SetAttrib(_USERATTRIB_LEV, nSatoriLev, SYNCHRO_TRUE);
			CLevupexpData* pSatoriExp	= LevupexpSet()->GetObj(nSatoriLev);
			if(pSatoriExp)
			{
				__int64	i64SatoriExp	= pSatoriExp->GetInt64(LEVUPEXPDATA_EXP);
				this->SetAttrib(_USERATTRIB_EXP, (nMeteLev % 100000) * (double)i64SatoriExp * 0.00001, SYNCHRO_TRUE);
			}
			if((int)GetLev() > nLevBefore)
				AddAttrib(_USERATTRIB_ADDPOINT, GetLev()*3 - nLevBefore*3, SYNCHRO_TRUE);
			SendSysMsg(STR_SATORI);
			if(pbSatori)
				*pbSatori	= true;
		}
	}
}

void CUser::AllotPoint()
{
	int nProf = this->GetProfessionSort();
	if (nProf > 10)
		nProf = (nProf/10)*10;

	int nAmount = AddPointSet()->GetAmount();
	for(int i = 0; i < nAmount; i++)
	{
		CAddPointData* pData = AddPointSet()->GetObjByIndex(i);
		if(pData
				&& pData->GetInt(ADDPOINTDATA_PROF) == nProf
				&& pData->GetInt(ADDPOINTDATA_LEVEL) == this->GetLev())
		{
			m_data.SetForce(pData->GetInt(ADDPOINTDATA_FORCE));
			m_data.SetHealth(pData->GetInt(ADDPOINTDATA_HEALTH));
			m_data.SetSoul(pData->GetInt(ADDPOINTDATA_SOUL));
			m_data.SetSpeed(pData->GetInt(ADDPOINTDATA_DEX), UPDATE_TRUE);

			{
				CMsgUserAttrib	msg;
				if(msg.Create(GetID(), _USERATTRIB_FORCE, GetForce())
						&& msg.Append(_USERATTRIB_SPEED, GetSpeed())
						&& msg.Append(_USERATTRIB_HEALTH, GetHealth())
						&& msg.Append(_USERATTRIB_SOUL, GetSoul())	)
					SendMsg(&msg);
			}
			return ;
		}
	}

	this->AddAttrib(_USERATTRIB_ADDPOINT, _ADDITIONALPOINT_NUM, SYNCHRO_TRUE);
	return;
}

int CUser::GetPose()
{
	return m_nPose;
}

bool CUser::IsAllNonsuchEquip (void)
{
	CItemPtr pWeaponR = NULL;
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		if (i == ITEMPOSITION_RINGL || i == ITEMPOSITION_MOUNT)
			continue;

		CItem* pEquip = GetEquipItemByPos(i);

		if (i == ITEMPOSITION_WEAPONR)
			pWeaponR = pEquip;

		if (!pEquip)
		{
			if (i == ITEMPOSITION_WEAPONL)
			{
				switch(this->GetProfessionSort())
				{
				case PROFSORT_MAGICIAN:
				case PROFSORT_SOLDIER:
					{
						if (this->GetLev() >= 40)
						{
							if (pWeaponR && !pWeaponR->IsWeapon2())
								return false;
						}
					}
					break;

				default:
					break;
				}

				continue;
			}
			else
				return false;
		}

		if (!pEquip->IsNonsuch())
			return false;
	}

	return true;
}

DWORD CUser::GetEffect(int nIndex)
{
	return m_data.m_Info.dwEffect[nIndex % 2];
}

int CUser::GetDir()
{
	return m_nDir;
}

DWORD CUser::GetLookFace()
{
	enum {LOOK_MALEGHOST=98, LOOK_FEMALEGHOST};

	if (IsGhost())
	{
		if (SEX_MAN == this->GetSex())
			return m_data.GetLookFace() + MASK_CHANGELOOK*LOOK_MALEGHOST;
		else
			return m_data.GetLookFace() + MASK_CHANGELOOK*LOOK_FEMALEGHOST;
	}
	else if(QueryTransformation())
	{
		return m_data.GetLookFace()
				+ MASK_CHANGELOOK * QueryTransformation()->GetLook();
	}
	else if(m_dwUnk610 != -1)
		return m_dwUnk610 * MASK_CHANGELOOK + m_data.GetLookFace();
	else
		return m_data.GetLookFace();
}

DWORD CUser::GetWeightLimit()
{
	return 1000000;
}

bool CUser::LeaveMap()
{
	if(m_nStep != STEP_ENTERMAP)
		return false;

	DestroyBooth();
	KillCallPet(true);

	CMapPtr pMap = GetMap();
	if(pMap)
	{
		pMap->LeaveRoom(QueryMapThing());
		pMap->ClearRegionInfo(this);

		CMsgAction msg;
		if (msg.Create(GetID(), 0,0,0, actionLeaveMap))
			this->BroadcastRoomMsg(&msg, EXCLUDE_SELF);
	}

	m_nStep = STEP_LOGIN;
	return true;
}

bool CUser::SendItemSet()
{
	SendAllItemInfo();
	UpdateBattlePower();
	ClearUnkItemSet();
	return true;
}

bool CUser::SendGoodFriend()
{
	SendFriendInfo();
	return true;
}

bool CUser::SetAttackTarget(IRole* pTarget )
{
	if (!m_pBattleSystem)
		return false;

	if(pTarget == NULL)
	{
		m_pBattleSystem->ResetBattle();
		return true;
	}

	if (!this->IsAtkable(pTarget, true))
		return false;

	if (GetMap()->IsDeadIsland())
	{
		CUser* pTargetUser = NULL;
		pTarget->QueryObj(OBJ_USER, IPP_OF(pTargetUser));
		if (!pTargetUser)
			pTargetUser = pTarget->QueryOwnerUser();
		if (pTargetUser)
		{
			bool bNewbie = (this->GetLev() <= 25);
			if (bNewbie)
			{
				this->SendSysMsg(STR_LEV_NOT_ENOUGH_TO_PK);
				return false;
			}

			bNewbie = (pTargetUser->GetLev() <= 25);
			if (bNewbie)
			{
				if (this->GetPk() >= 100)
				{
					char szMsg[256] = "";
					sprintf(szMsg, STR_GOTO_JAIL_PK_NEWBIE, this->GetName());
					GotoJail(6000, szMsg);
				}
				else
				{
					this->SendSysMsg(STR_LEV_MAP_NEWBIE_PROTECT);
				}
				return false;
			}
		}
	}

	if (this->GetPk() >= 1000)
	{
		if (!GetMap()->IsPkField() && !GetMap()->IsSynMap())
		{
			CUser* pOwner = pTarget->QueryOwnerUser();
			if (pOwner && pOwner->GetPk() < 30 && !pOwner->IsEvil())
			{
				this->SendSysMsg(STR_PK);
				return false;
			}
		}
	}

	if (IsImmunity(pTarget))
		return false;

	if(pTarget->IsWing() && !this->IsWing() && !(this->IsBowman() || this->IsSimpleMagicAtk()))
		return false;

	CNpc* pNpc;
	pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc));
	if(pNpc && pNpc->IsGoal() && (GetLev() < pNpc->GetLev() || pNpc->GetType()!=_WEAPONGOAL_NPC))
		return false;

	if(this->GetDistance(pTarget->QueryMapThing()) > GetAttackRange(pTarget->GetSizeAdd()))
	{
		m_pBattleSystem->ResetBattle();
		return false;
	}

	CheckCrime(pTarget);

	if(IsBowman() && !IsArrowPass(pTarget->GetPosX(), pTarget->GetPosY()))
		return false;

	m_pBattleSystem->CreateBattle(pTarget->GetID());
	CalcFightRate();

	QueryMagic()->AbortMagic(false);

	CItem* pEquipL = GetEquipItemByPos(ITEMPOSITION_WEAPONL);
	if(!pEquipL || pEquipL->GetInt(ITEMDATA_MAGIC1) == MAGIC_NONE)
		UnequipMagicItem(ITEMPOSITION_WEAPONL);

	return true;
}

int CUser::GetGemMgcExpEffect()
{
	int nAddPercent = 0;
	for (int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (pEquip)
			nAddPercent += pEquip->GetGemMgcExpEffect();
	}

	return nAddPercent;
}

int CUser::GetGemWpnExpEffect()
{
	int nAddPercent = 0;
	for (int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (pEquip)
			nAddPercent += pEquip->GetGemWpnExpEffect();
	}

	return nAddPercent;
}

int CUser::GetGemExpEffect()
{
	int nAddPercent = 0;
	for (int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (pEquip)
			nAddPercent += pEquip->GetGemExpEffect();
	}

	return nAddPercent;
}
int	CUser::GetGemAtkEffect		(void)
{
	int nAddEffect = 0;
	for (int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (pEquip)
			nAddEffect += pEquip->GetGemDmgEffect();
	}

	return nAddEffect;
}

int CUser::GetGemMgcAtkEffect	(void)
{
	int nAddEffect = 0;
	for (int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (pEquip)
			nAddEffect += pEquip->GetGemEffect700071();
	}

	return nAddEffect;
}

int CUser::GetGemHitRateEffect	(void)
{
	int nAddEffect = 0;
	for (int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (pEquip)
			nAddEffect += pEquip->GetGemMgcAtkEffect();
	}

	return nAddEffect;
}

void CUser::SendGemEffect(bool bAttack)
{
	vector<int> setGem;

	int nGem = 0;
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
		{
			nGem = pEquip->GetInt(ITEMDATA_GEM1);
			if (nGem != GEM_NONE && nGem != GEM_HOLE)
				setGem.push_back(nGem);

			nGem = pEquip->GetInt(ITEMDATA_GEM2);
			if (nGem != GEM_NONE && nGem != GEM_HOLE)
				setGem.push_back(nGem);
		}
	}

	string strEffect;
	int nGems = setGem.size();
	if (nGems > 0)
	{
		if (bAttack)
		{
			switch(setGem[::RandGet(nGems)])
			{
			case 3:
				strEffect = "phoenix";
				break;
			case 13:
				strEffect = "goldendragon";
				break;
			case 23:
				strEffect = "fastflash";
				break;
			case 33:
				strEffect = "rainbow";
				break;
			case 43:
				strEffect = "goldenkylin";
				break;
			case 53:
				strEffect = "purpleray";
				break;
			case 63:
				strEffect = "moon";
				break;
			default:
				break;
			}
		}
		else
		{
			if (setGem[::RandGet(nGems)] == 73)
				strEffect = "recovery";
		}
	}

	if (!bAttack)
	{
		int nReduceDmg = GetEquipReduceDmg();
		if (nReduceDmg > 41)
			strEffect = "Aegis4";
		else if (nReduceDmg > 29)
			strEffect = "Aegis3";
		else if (nReduceDmg > 17 && nReduceDmg <= 29)
			strEffect = "Aegis2";
		else if (nReduceDmg > 0 && nReduceDmg <= 17)
			strEffect = "Aegis1";
	}
	if (!strEffect.empty())
	{
		CMsgName msg;
		IF_OK (msg.Create(NAMEACT_ROLEEFFECT, strEffect.c_str(), this->GetID()))
			this->BroadcastRoomMsg(&msg, true);
	}
}

int CUser::GetEquipReduceDmg()
{
	int nReduceDmg = 0;
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
			nReduceDmg += pEquip->GetInt(ITEMDATA_REDUCE_DMG);
	}

	return nReduceDmg;
}

bool CUser::IsAntiMonster(IRole* pTarget)
{
	CMonster* pMonster = NULL;
	if (pTarget && pTarget->IsMonster() && pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
	{
		for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
		{
			CItem* pEquip = GetEquipItemByPos(i);
			if (pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
			{
				int nAntiMonster = pEquip->GetInt(ITEMDATA_ANTI_MONSTER);
				if (nAntiMonster && nAntiMonster == pMonster->QueryType()->GetInt(NPCTYPEDATA_ANTI_MONSTER))
					return true;
			}
		}
	}
	return false;
}

int CUser::Attack(IRole* pTarget)
{
 	CHECKF(pTarget);

	int nPower;
	if (IsSimpleMagicAtk())
		nPower = CMagic::CalcMagicPower(QueryRole(), pTarget, GetAttack());
	else
		nPower = CBattleSystem::CalcAttackPower(QueryRole(), pTarget, false, 0, 0, true);

	int nDamage = pTarget->BeAttack(QueryRole(), nPower, true);
	int nLoseLife = ::CutOverflow(nDamage, (int)pTarget->GetLife());
	if (nLoseLife > 0)
		pTarget->AddAttrib(_USERATTRIB_LIFE, -1*nLoseLife, SYNCHRO_TRUE, NULL);

	CNpc* pNpc = NULL;
	pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc));

	if (!pNpc || !pNpc->IsGoal())
		this->DecEquipmentDurability(ATTACK_TIME, BY_WEAPON);

	if (QueryStatus(STATUS_CRIME) && !pTarget->IsEvil())
		this->SetCrimeStatus(STATUS_CRIME, 15);

	if (nLoseLife > 0 && (pTarget->IsMonster() || pNpc && pNpc->IsGoal()) && !QueryTransformation())
	{
		int nWpnExpEffect = GetGemWpnExpEffect();
		if (m_pWeaponR)
		{
			int nSkillType = m_pWeaponR->GetSubType();
			int nExp = MulDiv(nLoseLife, 100+nWpnExpEffect, 100);
			if (pNpc && pNpc->IsGoal())
				nExp = MulDiv(nExp, 20, 100);
			this->AddWeaponSkillExp(nSkillType, nExp);

			if (m_pWeaponL)
				this->AddWeaponSkillExp(m_pWeaponL->GetSubType(), MulDiv(nLoseLife, 100+nWpnExpEffect, 100)/2);
		}
		else
		{
			int nExp = MulDiv(nLoseLife, 100+nWpnExpEffect, 100);
			if (pNpc && pNpc->IsGoal())
				nExp = MulDiv(nExp, 20, 100);
			this->AddWeaponSkillExp(0, nExp);
		}
	}

	if (nDamage > 0)
	{
		if (::RandGet(10) == 7)
			this->SendGemEffect(true);
	}

	return nDamage;
}

bool CUser::WeaponMagicAttack(OBJID idTarget, int x, int y)
{
	if (m_pWeaponL && m_pWeaponL->GetInt(ITEMDATA_MAGIC1) && m_pWeaponL->GetInt(ITEMDATA_AMOUNT))
		return false;

	int idxMagic;
	if (m_pWeaponR && (idxMagic = m_pWeaponR->GetInt(ITEMDATA_MAGIC1)) != 0 && m_pWeaponR->GetInt(ITEMDATA_AMOUNT))
		QueryMagic()->WeaponMagicAttack(MagicType(idxMagic), idTarget, x, y);

	return true;
}

int CUser::BeAttack(IRole* pTarget, int nPower, bool bReflectEnable)
{
	CHECKF(pTarget && nPower >= 0);

	QueryStatusSet()->DelObj(STATUS_LURKER);

	if(this->CheckReflect() && bReflectEnable)
	{
		int nPower2 = ::CutOverflow((int)pTarget->GetLife(), nPower);
		nPower2 = ::CutOverflow(nPower2, this->GetReflectLimit());
		pTarget->AddAttrib(_USERATTRIB_LIFE, -1*nPower2, SYNCHRO_TRUE, NULL);
		pTarget->BeAttack(QueryRole(), nPower, false);

		int nDeltaBattleEffect = 0;
		CUser* pTargetUser = pTarget->QueryOwnerUser();
		if (pTargetUser)
			nDeltaBattleEffect = this->GetBattlePower() - pTargetUser->GetBattlePower();

		CMsgInteract msg;
		if (msg.Create(INTERACT_REFLECTWEAPON, GetID(), pTarget->GetID(), GetPosX(), GetPosY(), nPower2, nDeltaBattleEffect))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);

		if (!pTarget->IsAlive())
			pTarget->BeKill(QueryRole(), 0);

		nPower = 0;
	}

	if(nPower > 0)
	{
		BroadcastTeamLife();
	}
	this->DecEquipmentDurability(BEATTACK_TIME, BY_WEAPON);

	if (_ACTION_SITDOWN ==  this->GetPose())
	{
		this->SetPose(_ACTION_STANDBY);
		this->SetAttrib(_USERATTRIB_ENERGY, this->GetEnergy()/2, SYNCHRO_TRUE);
	}

	if (nPower > 0)
	{
		if (::RandGet(10) == 7)
			this->SendGemEffect(false);
	}

	return nPower;
}

int CUser::BeAttackEx(IRole* pTarget, int nPower, bool bReflectEnable)
{
	CHECKF(pTarget && nPower >= 0);

	if (!IsAlive())
		return 0;

	QueryStatusSet()->DelObj(STATUS_LURKER);

	if(this->CheckReflect() && bReflectEnable)
	{
		int nPower2 = ::CutOverflow((int)pTarget->GetLife(), nPower);
		nPower2 = ::CutOverflow(nPower2, this->GetReflectLimit());
		pTarget->AddAttrib(_USERATTRIB_LIFE, -1*nPower2, SYNCHRO_TRUE, NULL);
		pTarget->BeAttackEx(QueryRole(), nPower, false);

		int nDeltaBattleEffect = 0;
		CUser* pTargetUser = pTarget->QueryOwnerUser();
		if (pTargetUser)
			nDeltaBattleEffect = this->GetBattlePower() - pTargetUser->GetBattlePower();

		CMsgInteract msg;
		if (msg.Create(INTERACT_REFLECTMAGIC, GetID(), pTarget->GetID(), GetPosX(), GetPosY(), nPower, nDeltaBattleEffect))
			BroadcastRoomMsg(&msg, INCLUDE_SELF);

		if (!pTarget->IsAlive())
			pTarget->BeKill(NULL, 1);

		nPower = 0;
	}

	if(nPower > 0)
		BroadcastTeamLife();

	if (_ACTION_SITDOWN ==  this->GetPose())
	{
		this->SetPose(_ACTION_STANDBY);
		this->SetAttrib(_USERATTRIB_ENERGY, this->GetEnergy()/2, SYNCHRO_TRUE);
	}

	if (nPower > 0)
	{
		if (::RandGet(10) == 7)
			this->SendGemEffect(false);
	}

	return nPower;
}

void CUser::SendDamageMsg(OBJID idTarget, int nDamage)
{
	OBJID idMagicType = ID_NONE;
	if(QueryTransformation())
		idMagicType = QueryTransformation()->GetMagicType();

	if (!FindAroundRole(idTarget))
		return;

	int nDeltaBattleEffect = 0;
	IRole* pTarget = RoleManager()->QueryRole(idTarget);
	if (pTarget)
	{
		CUser* pTargetUser = pTarget->QueryOwnerUser();
		if (pTargetUser)
			nDeltaBattleEffect = this->GetBattlePower() - pTargetUser->GetBattlePower();
	}

	if(idMagicType != ID_NONE)
	{
		CMsgMagicEffect	msg;
		IF_OK(msg.Create(GetID(), idMagicType, 0, idTarget, nDamage, nDeltaBattleEffect))
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

bool CUser::BroadcastTeamLife(bool bMaxLife)
{
	CTeam* pTeam	= GetTeam();
	if (pTeam && pTeam->GetMemberAmount() > 1)
	{
		pTeam->BroadcastMemberLife(this, bMaxLife);
		return true;
	}
	return false;
}

bool CUser::DecEquipmentDurability(bool bBeAttack, bool bMagic)
{
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if (!pEquip)
			continue;
		if (CItem::GetItemSort(pEquip->GetInt(ITEMDATA_TYPE)) == 10)
			continue;
		if (pEquip->QueryType()->GetInt(ITEMTYPEDATA_FRAY_MODE))
			continue;

		if (!bMagic)
		{
			if (i == ITEMPOSITION_RINGR ||
					i == ITEMPOSITION_WEAPONR ||
						i == ITEMPOSITION_WEAPONL)
			{
				if(bBeAttack)
					continue;
			}
			else
			{
				if(!bBeAttack)
					continue;
			}
			if (::RandGet(3) == 1)
				AddEquipmentDurability(i, -1);
		}
		else
		{
			if (bBeAttack)
			{
				if (i >= ITEMPOSITION_WEAPONR && i <= ITEMPOSITION_RINGR)
					continue;
				if (::RandGet(3) == 1)
					AddEquipmentDurability(i, -1);
			}
			else
			{
				if (i == ITEMPOSITION_WEAPONL || i == ITEMPOSITION_SHOES)
					continue;
				if (::RandGet(10) == 1)
					AddEquipmentDurability(i, -1);
			}
		}
	}
	return true;
}

bool CUser::MagicAttack(int nType, OBJID idTarget, int x, int y)
{
	CGameMap* pMap = GetMap();
	if (!pMap)
		return false;
	if (pMap->GetID() == 1039 && nType == 1090)
		return false;
	if (pMap->IsPkDisable() || pMap->IsFamilyMap())
	{
		if (QueryMagic()->IsCrimeMagic())
			return false;
	}
	if (nType == 1115 && m_pWeaponL && m_pWeaponR && m_pWeaponR->GetSubType() != m_pWeaponL->GetSubType())
		return false;

	DetachLuckyStatus();
	m_dwUnk63C = ::TimeGet();

	return QueryMagic()->MagicAttack(nType, idTarget, x, y);
}

__int64	CUser::CalcExpLostOfDeath (IRole* pKiller)
{
	CUser* pUserKiller = NULL;
	if (!pKiller->QueryObj(OBJ_USER, IPP_OF(pUserKiller)))
		return 0;

	int nParam = 50;
	if (this->GetEffect(0)&KEEPEFFECT_RED)
		nParam = 10;
	else if (this->GetEffect(0)&KEEPEFFECT_DEEPRED)
		nParam = 5;
	else
		nParam = 50;

	__int64 nExpLost = __max(0, this->GetExp()/nParam);

	if (this->GetSynID() != ID_NONE && !pUserKiller->IsMyWantedTarget(this))
	{
		int nDecPercent = 0;
		CSyndicate* pSyn = this->GetSyndicate();
		IF_OK (pSyn)
		{
			int nExpPayBySyn = 0;
			int nProffer = this->QuerySynProffer();
			if (nProffer > 0)
			{
				int nRank = this->QuerySynRank();
				switch(nRank)
				{
				case RANK_LEADER:
					nDecPercent = 80;
					break;

				case 90:
					nDecPercent = 60;
					break;
				case RANK_SUBLEADER:
					nDecPercent = 40;
					break;
				default:
					nDecPercent = 25;
					break;
				}

				CONST int MONEYCOST_PEREXP = 100;
				int nFundSyn  = pSyn->GetInt(SYNDATA_MONEY);
				int nFundLost = nExpLost*nDecPercent/(100*MONEYCOST_PEREXP);
				if (nFundLost > nFundSyn)
					nFundLost = nFundSyn;

				pSyn->QueryModify()->AddData(SYNDATA_MONEY, -1*nFundLost, true);

				if (nRank != RANK_LEADER)
					this->SetSynProffer(nProffer-nFundLost);

				nExpPayBySyn = nFundLost*MONEYCOST_PEREXP;
				nExpLost -= nExpPayBySyn;
			}
			else
			{
				if (pSyn->IsWhiteSynMapOwner() || pSyn->IsBlackSynMapOwner())
				{
					nDecPercent = 20;
				}
				else
				{
					nDecPercent = 10;
				}

				CONST int MONEYCOST_PEREXP = 100;
				int nFundSyn  = pSyn->GetInt(SYNDATA_MONEY);
				int nFundLost = nExpLost*nDecPercent/(100*MONEYCOST_PEREXP);
				if (nFundLost > nFundSyn)
					nFundLost = nFundSyn;

				pSyn->QueryModify()->AddData(SYNDATA_MONEY, -1*nFundLost, true);

				if (this->QuerySynRank() != RANK_LEADER)
					this->SetSynProffer(nProffer-nFundLost);

				nExpPayBySyn = nFundLost*MONEYCOST_PEREXP;
				nExpLost -= nExpPayBySyn;
			}

			this->SendSysMsg(_TXTATR_NORMAL, EXPLOST_PAYBY_SYNFUND, nExpPayBySyn);
		}
	}

	return __max(0, nExpLost);
}

int CUser::GetTeamKillCount(OBJID idUser)
{
	map<OBJID,int>::iterator iter = m_mapTeamKill.find(idUser);
	if (iter != m_mapTeamKill.end())
		return iter->second;
	return 0;
}

void CUser::SetTeamKillCount(OBJID idUser, int nCount)
{
	m_mapTeamKill[idUser] = nCount;
}

int CUser::GetKillStatistic(int nType)
{
	map<int,int>::iterator iter = m_mapKillStatistic.find(nType);
	if (iter != m_mapKillStatistic.end())
		return iter->second;
	return 0;
}

void CUser::SetKillStatistic(int nType, int nCount)
{
	m_mapKillStatistic[nType] = nCount;
}

bool CUser::IsNewInteractData(int nData)
{
	return find(m_setInteractData.begin(), m_setInteractData.end(), nData) == m_setInteractData.end();
}

void CUser::AddInteractData(int nData)
{
	m_setInteractData.push_back(nData);
}

int CUser::SumKillStatistic()
{
	int nSum = 0;
	for(map<int,int>::iterator iter = m_mapKillStatistic.begin(); iter != m_mapKillStatistic.end(); iter++)
		nSum += iter->second;
	return nSum;
}

void CUser::SendKillStatistic(bool bTeam)
{
	for(map<int,int>::iterator iter = m_mapKillStatistic.begin(); iter != m_mapKillStatistic.end(); iter++)
	{
		if (iter->second > 0)
		{
			CMsgInteract msg;
			msg.Create(28, GetID(), 0, 0, 0, 0, (USHORT)iter->first, iter->second);
			SendMsg(&msg);
		}
		iter->second = 0;
	}

	int nMembers = 0;
	for(map<OBJID,int>::iterator it = m_mapTeamKill.begin(); it != m_mapTeamKill.end(); it++)
	{
		CUser* pUser = UserManager()->GetUser(it->first);
		if (pUser && it->second > 0)
		{
			if (bTeam)
			{
				CMsgInteract msg;
				if (msg.Create(29, pUser->GetID(), 0, 0, 0, 0, 0, it->second))
					SendMsg(&msg);
			}
			this->SendSysMsg(STR_TEAM_STATISTIC, pUser->GetName(), it->second);
			nMembers++;
			it->second = 0;
		}
	}

	if (bTeam && nMembers > 0)
	{
		CMsgInteract msg;
		if (msg.Create(29, GetID(), 0, 0, 0, 0, 0, -1))
			SendMsg(&msg);
	}
}

void CUser::Kill(IRole* pTarget, DWORD dwDieWay, int nReserved)
{
	CHECK(pTarget);

	if (m_dwXP < 99)
		m_dwXP++;

	bool bXpSkill = false;
	IStatus* pStatus = QueryStatus(STATUS_SUPERMAN);
	if (pStatus)
	{
		pStatus->IncTime(1, 120);
		bXpSkill = true;
	}
	pStatus = QueryStatus(STATUS_TORNADO);
	if (pStatus)
	{
		pStatus->IncTime(1, 120);
		bXpSkill = true;
	}

	if (pTarget->IsMonster() || pTarget->GetID() >= 500001 && pTarget->GetID() <= 599999)
	{
		if (bXpSkill)
		{
			if (nReserved == 0)
				m_nCurrSupermanKills++;

			int nKills = GetStatistic(2, 0) + 1;
			SetStatistic(2, 0, nKills, true, nKills % 200 == 0, false);
		}

		CMonster* pMonster = NULL;
		if (pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
		{
			int nStcType = pMonster->QueryType()->GetInt(NPCTYPEDATA_STC_TYPE);
			if (nStcType)
			{
				int nKills = GetStatistic(1, nStcType) + 1;
				SetStatistic(1, nStcType, nKills, true, nKills % 200 == 0, false);

				CTeam* pTeam = GetTeam();
				if (pTeam)
					pTeam->AddKillStatistic(GetID(), pTarget, 1, nStcType);
			}
		}

		if (m_dwUnk638 > 0
				&& !(GetEffect(0) & KEEPEFFECT_XPFULL)
				&& GetLev() < pTarget->GetLev() + 5
				&& m_dwXP >= 50
				&& ::RandGet(1000) < 5)
		{
			AddXp(100);
			SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_FULL_XP);

			CMsgName msg;
			IF_OK(msg.Create(NAMEACT_ROLEEFFECT, STR_LUCKY_EFFECT, this->GetID()))
				this->BroadcastRoomMsg(&msg, true);
		}
	}

	int nDeltaBattleEffect = 0;
	CUser* pTargetOwner = pTarget->QueryOwnerUser();
	if (pTargetOwner)
		nDeltaBattleEffect = this->GetBattlePower() - pTargetOwner->GetBattlePower();

	CMsgInteract msg;
	if(msg.Create(INTERACT_KILL, this->GetID(), pTarget->GetID(), pTarget->GetPosX(), pTarget->GetPosY(), dwDieWay, (USHORT)m_nCurrSupermanKills, nDeltaBattleEffect))
		pTarget->BroadcastRoomMsg(&msg, true);

	QueryMagic()->UserKillTarget(pTarget);

	CUser* pTargetUser	= NULL;
	CMonster* pMonster	= NULL;
	if(pTarget->QueryObj(OBJ_USER, IPP_OF(pTargetUser)))
	{
		ProcessPk(pTargetUser);
	}
	else if (pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pMonster))
				&& pMonster->GetID() >= 700001 && pMonster->GetID() <= 799999
				&& pMonster->IsInsteadMagic()
				&& pMonster->QueryOwnerUser())
	{
		ProcessPk(pMonster->QueryOwnerUser());
	}

	pTarget->BeKill(QueryRole(), 1);

	if (pTargetUser)
		GameAction()->ProcessAction(80000001, this, pTarget);
}

void CUser::ProcessPk (CUser* pTargetUser)
{
	MYASSERT(pTargetUser);

	CGameMap* pMap = MapManager()->QueryMap(this->GetMapID());
	IF_OK (pMap)
	{
		if (pMap->IsPkField())
		{
		}
		else if (pMap->IsSynMap())
		{
		}
		else if (pMap->IsPrisonMap())
		{
		}
		else
		{
			if (!(pTargetUser->GetEffect(0)&KEEPEFFECT_RED)
					&& !(pTargetUser->GetEffect(0)&KEEPEFFECT_DEEPRED))
			{
				if (this->GetEffect(0)&KEEPEFFECT_DEEPRED)
					PoliceWanted().AddWanted(this);
			}

			__int64	i64ExpLost		= 0;
			__int64	i64TargetExp	= pTargetUser->GetExp();
			if (this->IsTimeCardOn())
			{
				i64ExpLost	= pTargetUser->CalcExpLostOfDeath(this->QueryRole());
				if (i64TargetExp < 0)
					i64ExpLost	= 0;
				else if (i64TargetExp < i64ExpLost)
					i64ExpLost	= i64TargetExp;
				pTargetUser->AddAttrib(_USERATTRIB_EXP, -1*i64ExpLost, SYNCHRO_TRUE, NULL);
				if (i64ExpLost != 0)
					pTargetUser->SendSysMsg(STR_LOST_EXPERIENCE, i64ExpLost);
			}
			else if (pTargetUser->IsTimeCardOn())
			{
				pTargetUser->SendSysMsg(STR_GODBLESS_BEKILL_HINT);
				CRole::AttachStatus(this->QueryRole(), 0x08000000, -1, 300);
			}
			else
			{
				i64ExpLost	= pTargetUser->CalcExpLostOfDeath(this->QueryRole());
				if (i64TargetExp < 0)
					i64ExpLost	= 0;
				else if (i64TargetExp < i64ExpLost)
					i64ExpLost	= i64TargetExp;
				pTargetUser->AddAttrib(_USERATTRIB_EXP, -1*i64ExpLost, SYNCHRO_TRUE, NULL);
				if (i64ExpLost != 0)
					pTargetUser->SendSysMsg(STR_LOST_EXPERIENCE, i64ExpLost);
			}

			int nManaLost = pTargetUser->GetMana()/2;
			pTargetUser->AddAttrib(_USERATTRIB_MANA, -1*nManaLost, SYNCHRO_TRUE, NULL);

			int nLevDiff = this->GetLev() - pTargetUser->GetLev();
			if (nLevDiff <= 0 && i64ExpLost > 0)
			{
				CLevupexpData* pLevupexp	= LevupexpSet()->GetObj(this->GetLev());
				if (pLevupexp)
				{
					int nPercent = __max(10, nLevDiff*2 + 50);
					__int64 i64ExpGet = i64ExpLost * nPercent / 100;
					__int64 i64ExpMax = pLevupexp->GetInt64(LEVUPEXPDATA_EXP) * 300 / pLevupexp->GetInt(LEVUPEXPDATA_UP_LEV_TIME);
					if (i64ExpGet > i64ExpMax)
						i64ExpGet = i64ExpMax;
					this->AwardBattleExp(i64ExpGet, false, 1);
				}
			}

			if (!pTargetUser->IsEvil()
				&& pTargetUser->GetPk() < 30
				&& pTargetUser->GetExp() >= 0)
			{
				int nPkValue = 10;
				if (this->IsHostileSynMember(pTargetUser))
					nPkValue = 3;
				else if (this->QueryEnemy()->GetName(pTargetUser->GetID()) != NULL)
					nPkValue = 5;

				this->AddPk(nPkValue);

				if (QueryStatus(STATUS_CRIME))
					this->SetCrimeStatus(STATUS_CRIME, 120);

				this->SendSysMsg(STR_KILLING_TO_MUCH);
			}

			if (this->IsMyWantedTarget(pTargetUser))
			{
				OBJID idWanted = this->WantedInfo().idWanted;
				CWantedData* pData = CWantedList::s_WantedList.GetWanted(idWanted);

				int nBonuty = 0;
				if (pData)
					nBonuty = pData->GetInt(DATA_BOUNTY);

				this->AddAttrib(_USERATTRIB_MONEY, nBonuty*(100-_BONUTY_TAX)/100, SYNCHRO_TRUE, NULL);

				CWantedList::s_WantedList.DelWanted(idWanted);

				this->ResetWantedInfo();

				CMsgName msg;
				IF_OK_ (msg.Create(NAMEACT_WANTED, ""))
					this->SendMsg(&msg);

				__int64 i64LevExp	= LevupexpSet()->GetObj(pTargetUser->GetLev())->GetInt64(LEVUPEXPDATA_EXP);
				__int64 i64CurExp	= pTargetUser->GetExp();
				__int64 i64WantedLost = __min((__int64)nBonuty, i64LevExp);
				if (i64CurExp < 0)
					i64WantedLost	= 0;
				else if (i64CurExp < i64WantedLost)
					i64WantedLost	= i64CurExp;

				pTargetUser->AddAttrib(_USERATTRIB_EXP, -1*i64WantedLost, SYNCHRO_TRUE, NULL);
			}

			if (pTargetUser->GetID() == this->PoliceWantedID())
			{
				this->PoliceWantedID() = ID_NONE;

				int nKillAward = __min(-1, -1*(int(pTargetUser->GetLev())-40)/10);
				this->AddAttrib(_USERATTRIB_PK, nKillAward, SYNCHRO_TRUE, NULL);
			}
			else
			{
				CTeam* pTeam = this->GetTeam();
				if (pTeam)
				{
					CUser* pLeader = UserManager()->GetUser(pTeam->GetLeader());
					if (pLeader)
					{
						if (pLeader->PoliceWantedID() == pTargetUser->GetID())
							pLeader->TeamHuntKill(pTargetUser->GetID(), pTargetUser->GetLev(), pTargetUser->GetName());
					}
					else
					{
						char buf[1024] = {0};
						REMOTE_CALL0* pInfo = (REMOTE_CALL0*)&buf;
						pInfo->ucFuncType	= REMOTE_CALL_TEAM_HUNTKILL;
						pInfo->idUser		= pTeam->GetLeader();
						pInfo->IntParam[0]	= pTargetUser->GetID();
						pInfo->IntParam[1]	= pTargetUser->GetLev();
						strcpy((char*)(pInfo->IntParam[2]), pTargetUser->GetName());
						pInfo->nSize		= sizeof(REMOTE_CALL0) + 2*sizeof(int)+strlen(pTargetUser->GetName())+1;
						MapGroup(PID)->QueryIntraMsg()->RemoteCall(pInfo);
					}
				}
			}
		}
	}
}

void CUser::TeamHuntKill (OBJID idTarget, int nTargetLev, const char* pszTargetName)
{
	if (idTarget != this->PoliceWantedID())
		return;

	CTeam* pTeam = this->GetTeam();
	IF_NOT (pTeam)
		return;

	IF_NOT (pTeam->GetLeader() == this->GetID())
		return;

	this->PoliceWantedID() = ID_NONE;

	int nKillAward = __min(-1, -1*(nTargetLev-40)/10);
	this->AddAttrib(_USERATTRIB_PK, nKillAward, SYNCHRO_TRUE, NULL);

	MSGBUF szMsg = "";
	sprintf(szMsg, STR_TEAM_HUNT, pszTargetName, this->GetName());
	this->BroadcastMapMsg(szMsg, true);
}

bool CUser::IsMyWantedTarget (CUser* pTarget)
{
	IF_NOT (pTarget)
		return false;

	OBJID idWanted = this->WantedInfo().idWanted;
	if (ID_NONE == idWanted)
		return false;

	CWantedData* pData = CWantedList::s_WantedList.GetWanted(idWanted);
	return (pData && 0 == strcmp(pData->GetStr(DATA_TARGET_NAME), pTarget->GetName()));
}

void CUser::BeKill(IRole* pRole, int nReserved)
{
	DetachLuckyStatus();
	m_dwUnk63C	= ::TimeGet();

	ClrAttackTarget();
	this->SetStatus(STATUS_DIE, true);
	m_tAutoReborn.Startup(20);

	m_bGhost	= false;
	OBJID	idRebornMap = ID_NONE;
	POINT	pos;
	IF_OK(GetMap()->GetRebornMap(&idRebornMap, &pos))
		this->SetRecordPos(idRebornMap, pos.x, pos.y);

	GameAction()->ProcessAction(80000000, this);
	KillCallPet();

	for (int i = m_setItem.size()-1; i >= 0; i--)
	{
		CItem* pItem = m_setItem[i];
		if (pItem && pItem->IsAlwaysDropWhenDead() && !pItem->IsHighValue())
		{
			POINT	posDrop;
			posDrop.x	= GetPosX();
			posDrop.y	= GetPosY();
			if (GetMap()->FindDropItemCell(USERDROPITEM_RANGE, &posDrop))
			{
				::MyLogSave("gmlog/drop_item3", "%s drop item(%u).", GetName(), pItem->GetID());
				this->DropItem(pItem->GetID(), posDrop.x, posDrop.y);
			}
		}
	}

	CGameMap* pMap = MapManager()->QueryMap(this->GetMapID());
	IF_NOT (pMap)
		return;

	if (pMap->IsPkField() || pMap->IsSynMap())
	{
		for (int i = m_setItem.size()-1; i >= 0; i--)
		{
			CItem* pItem = m_setItem[i];
			if (pItem && ::RandGet(100) < 30
					&& (pItem->IsEatEnable() || !pItem->IsNeverDropWhenDead() && CItem::GetItemSort(pItem->GetInt(ITEMDATA_TYPE)) == 3)
					&& !pItem->IsHighValue())
			{
				POINT	posDrop;
				posDrop.x	= GetPosX();
				posDrop.y	= GetPosY();
				if (GetMap()->FindDropItemCell(USERDROPITEM_RANGE, &posDrop))
				{
					if (pItem->IsNonsuchItem())
						::MyLogSave("gmlog/drop_item2", "%s drop item(%u) when be killed in pk field or syndicate map.", GetName(), pItem->GetID());
					this->DropItem(pItem->GetID(), posDrop.x, posDrop.y);
				}
			}
		}

		if (pMap->IsSynMap() && !pMap->IsWarTime())
		{
			CONST OBJID ID_DDCITY	= 1002;
			CONST POINT POS_REBORN	= { 438, 398 };
			this->SetRecordPos(ID_DDCITY, POS_REBORN.x, POS_REBORN.y);
		}

		return;
	}
	else if (pMap->IsPrisonMap())
	{
		int nChance = __min(90, 20+this->GetPk()/2);
		for (int i = m_setItem.size()-1; i >= 0; i--)
		{
			CItem* pItem = m_setItem[i];
			if (pItem && !pItem->IsNeverDropWhenDead() && !pItem->IsHighValue() && ::RandGet(100) < nChance)
			{
				POINT	posDrop;
				posDrop.x	= GetPosX();
				posDrop.y	= GetPosY();
				if (GetMap()->FindDropItemCell(USERDROPITEM_RANGE, &posDrop))
				{
					if (pItem->IsNonsuchItem())
						::MyLogSave("gmlog/drop_item2", "%s drop item(%u) when be killed in prison map.", GetName(), pItem->GetID());
					this->DropItem(pItem->GetID(), posDrop.x, posDrop.y);
				}
			}
		}

		return;
	}

	if(!pRole)
		return ;

	MSGBUF	szMsg;
	sprintf(szMsg, STR_BEKILL_ss, this->GetName(), pRole->GetName());
	this->BroadcastRoomMsg(szMsg, true);

	CUser* pUserKiller = pRole->QueryOwnerUser();
	if (pUserKiller)
	{
		if (nReserved)
			QueryEnemy()->Add(pUserKiller->GetID(), pUserKiller->GetName(), SYNCHRO_TRUE, UPDATE_TRUE);

		int nMoneyDrop = this->GetMoney() * __min(95, this->GetPk()+10) / 100;
		if (nMoneyDrop > 0)
			this->DropMoney(nMoneyDrop, this->GetPosX(), this->GetPosY());

		int nChance = __min(90, 20+this->GetPk()/2);
		for (int i = m_setItem.size()-1; i >= 0; i--)
		{
			CItem* pItem = m_setItem[i];
			if (pItem && !pItem->IsNeverDropWhenDead() && !pItem->IsHighValue() && ::RandGet(100) < nChance)
			{
				POINT	posDrop;
				posDrop.x	= GetPosX();
				posDrop.y	= GetPosY();
				if (GetMap()->FindDropItemCell(USERDROPITEM_RANGE, &posDrop))
				{
					if (pItem->IsNonsuchItem())
						::MyLogSave("gmlog/drop_item2", "%s drop item(%u) when be pk with pk value(%d).", GetName(), pItem->GetID(), GetPk());
					this->DropItem(pItem->GetID(), posDrop.x, posDrop.y);
				}
			}
		}

		if (PoliceWanted().GetWanted(this->GetID()))
		{
			this->GotoJail(6000, STR_GOTO_JAIL);
			PoliceWanted().DelWanted(this->GetID());
		}

		if (this->GetPk() >= 30 && pMap->GetID() != 6002)
		{
			char szText[1024] = "";
			sprintf(szText, STR_GOTO_BLKNAME_JAIL, pUserKiller->GetName(), "%s");
			this->GotoJail(6000, szText);

			char szSQL[1024] = "";
			_snprintf(szSQL, 1024, "INSERT INTO cq_pk_bonus SET target=%d, target_name='%s', hunter=%d, hunter_name='%s', b_type=0, bonus=0",
						this->GetID(), this->GetName(), pUserKiller->GetID(), pUserKiller->GetName());
			szSQL[1023] = 0;
			Database()->ExecuteAsyncSQL(szSQL);
		}

		return;
	}

	CMonster* pMonster = NULL;
	pRole->QueryObj(OBJ_MONSTER, IPP_OF(pMonster));
	bool bNewbie = false;
	if (pMonster && pMonster->IsMonster())
		bNewbie = (this->GetLev() <= 15);
	if (!bNewbie)
	{
		int nMoneyDrop = ::RandGet(this->GetMoney()/3);
		if (nMoneyDrop > 0)
			this->DropMoney(nMoneyDrop, this->GetPosX(), this->GetPosY());

		int nChance = 33;
		if (this->GetLev() < 10)
			nChance = 5;
		for (int i = m_setItem.size()-1; i >= 0; i--)
		{
			CItem* pItem = m_setItem[i];
			if (pItem && !pItem->IsNeverDropWhenDead() && !pItem->IsHighValue() && ::RandGet(100) < nChance)
			{
				POINT	posDrop;
				posDrop.x	= GetPosX();
				posDrop.y	= GetPosY();
				if (GetMap()->FindDropItemCell(USERDROPITEM_RANGE, &posDrop))
				{
					if (pItem->IsNonsuchItem())
						::MyLogSave("gmlog/drop_item2", "%s drop item(%u) when be killed by monster with pk value(%d).", GetName(), pItem->GetID(), GetPk());
					this->DropItem(pItem->GetID(), posDrop.x, posDrop.y);
				}
			}
		}
	}

	if (pMonster && (pMonster->IsGuard() || pMonster->IsPkKiller()) && this->GetPk() >= 30)
		this->GotoJail(6000, STR_GOTO_JAIL);
}

void CUser::ContributeExp(bool bLev)
{
	if (this->GetLev() >= g_nMaxLevel)
		return;

	DEBUG_TRY
	DEBUG_CATCH3("CUser::ContributeExp() [UserID: %d, m_i64ContributeExp: %I64d]", GetID(), m_i64ContributeExp)
}

__int64 CUser::AwardExp(__int64 i64Exp, bool bContribute)
{
	if (this->IsIncomeLimited())
		i64Exp = i64Exp * this->GetIncomePercent() / 100;

	int		nLev		= this->GetLev();
	__int64	i64NewExp	= this->GetExp() + i64Exp;
	DWORD	nLevUp		= 0;
	while (true)
	{
		CLevupexpData* pLevupexp	= LevupexpSet()->GetObj(nLev);
		if (!pLevupexp)
			break;
		__int64	i64LevupExp	= pLevupexp->GetInt64(LEVUPEXPDATA_EXP);
		if (i64NewExp < i64LevupExp)
			break;
		i64NewExp	-= i64LevupExp;
		nLevUp++;
		nLev++;
	}

	bool bSatori = false;
	if (nLevUp > 0)
	{
		this->IncLev(nLevUp, &bSatori);
		if(IsAutoAllot())
			this->AllotPoint();
		else
			this->AddAttrib(_USERATTRIB_ADDPOINT, nLevUp*_ADDITIONALPOINT_NUM, SYNCHRO_TRUE, NULL);

		this->SetAttrib(_USERATTRIB_LIFE, this->GetMaxLife(), SYNCHRO_TRUE);

		if (this->GetTeam())
			this->BroadcastTeamLife(true);
	}

	if (!bSatori || this->GetExp() < i64NewExp)
		this->SetAttrib(_USERATTRIB_EXP, i64NewExp, SYNCHRO_TRUE);

	m_data.SaveInfo();

	if (bContribute)
	{
		this->AddContributeExp(i64Exp);
		this->ContributeExp(false);
	}
	return i64NewExp;
}

__int64 CUser::AwardBattleExp(__int64 i64Exp, bool bGemEffect, int nReserved)
{
	if (i64Exp == 0)
		return 0;

	if (i64Exp < 0)
	{
		this->AddAttrib(_USERATTRIB_EXP, i64Exp, SYNCHRO_TRUE, NULL);
		return i64Exp;
	}

	if (this->IsTimeCardStatus())
		return 0;

	if (this->IsTimeCardOn())
		i64Exp += i64Exp * 20 / 100;

	if ((int)this->GetLev() >= g_nMaxLevel)
		return 0;

	if ((int)this->GetLev() >= MASTER_USERLEV)
		i64Exp /= 2;

	if (this->GetMetempsychosis() == 2)
	{
		i64Exp /= 3;
		if (i64Exp < 1)
			i64Exp = 1;
	}

	if (GetMap() && GetMap()->GetID() != 1039 && !m_tDoubleExp.IsTimeOut())
		i64Exp = i64Exp * m_nDoubleExp / 100;

	DEBUG_TRY
	if (this->IsIncomeLimited())
		i64Exp = i64Exp * this->GetIncomePercent() / 100;

	CLevupexpData* pLevupexp	= LevupexpSet()->GetObj(this->GetLev());
	if (!pLevupexp)
		return 0;

	__int64	i64LevupExp	= pLevupexp->GetInt64(LEVUPEXPDATA_EXP);
	__int64	i64OldExp	= this->GetExp();
	__int64	i64NewExp	= ::CutRange(i64OldExp + i64Exp, (__int64)0x8000000000000008, (__int64)0x7FFFFFFFFFFFFFF8);
	if (i64NewExp >= i64LevupExp)
	{
		int nLevUp = 0;
		for (int nLev = this->GetLev(); nLev < g_nMaxLevel; nLev++)
		{
			CLevupexpData* pData	= LevupexpSet()->GetObj(nLev);
			if (!pData)
				return 0;
			if (i64NewExp < pData->GetInt64(LEVUPEXPDATA_EXP))
				break;
			i64NewExp	-= pData->GetInt64(LEVUPEXPDATA_EXP);
			nLevUp++;
		}

		bool bSatori = false;
		this->IncLev(nLevUp, &bSatori);
		if (!bSatori || this->GetExp() < i64NewExp)
			this->SetAttrib(_USERATTRIB_EXP, i64NewExp, SYNCHRO_TRUE);
		if(IsAutoAllot())
			this->AllotPoint();
		else
			this->AddAttrib(_USERATTRIB_ADDPOINT, nLevUp*_ADDITIONALPOINT_NUM, SYNCHRO_TRUE, NULL);

		this->SetAttrib(_USERATTRIB_LIFE, this->GetMaxLife(), SYNCHRO_TRUE);

		if (this->GetTeam())
			this->BroadcastTeamLife(true);

		m_data.SaveInfo();
	}
	else
	{
		this->SetAttrib(_USERATTRIB_EXP, i64NewExp, SYNCHRO_TRUE);

		__int64	i64CurExp	= this->GetExp();
		int		nLev		= this->GetLev();
		if (nLev >= 130)
		{
			DWORD dwNow = ::TimeGet();
			if (i64NewExp - m_i64LastSaveExp >= 2000000 && dwNow - m_dwLastSaveExpTime >= 300000)
			{
				m_i64LastSaveExp		= i64NewExp;
				m_dwLastSaveExpTime		= dwNow;
				m_data.SaveInfo();
			}
		}
		else
		{
			int nOldProgress	= i64OldExp * 1000 / i64LevupExp;
			int nNewProgress	= i64CurExp * 1000 / i64LevupExp;
			int nStep = 200;
			if (nLev >= 110)
				nStep = 10;
			else if (nLev >= 100)
				nStep = 20;
			else if (nLev >= 80)
				nStep = 50;
			else if (nLev >= 60)
				nStep = 100;
			if (nOldProgress/nStep != nNewProgress/nStep)
				m_data.SaveInfo();
		}

		if (this->IsVirtueAwardLev())
		{
			CTeam* pTeam = this->GetTeam();
			if(pTeam && pTeam->IsAwardVirtueEnable(this))
			{
				CUser* pLeader = UserManager()->GetUser(pTeam->GetLeader());
				if(pLeader)
				{
					int nLev	= this->GetLev();
					int nFactor	= 1;
					if (nLev >= 40 && nLev < 50)
						nFactor	= 3;
					else if (nLev >= 50 && nLev < 60)
						nFactor	= 4;
					else if (nLev >= 60 && nLev < 65)
						nFactor	= 6;
					else if (nLev >= 65 && nLev < 70)
						nFactor	= 10;
					int nStep = 1000 / nFactor;
					if (1000 % nStep >= nFactor / 2)
						nStep++;
					int nOldProgress	= i64OldExp * 1000 / i64LevupExp;
					int nNewProgress	= i64CurExp * 1000 / i64LevupExp;
					if (nOldProgress/nStep != nNewProgress/nStep)
					{
						int nAwardVirtue	= i64LevupExp*0.9/(nFactor*nLev*30);
						if (nAwardVirtue > 0)
						{
							if(pLeader->IsIncomeLimited())
								nAwardVirtue	= pLeader->GetIncomePercent() * nAwardVirtue / 100;
							pLeader->AddAttrib(_USERATTRIB_VIRTUE, nAwardVirtue, SYNCHRO_TRUE, NULL);

							{
								CMsgTeamAward	msg;
								IF_OK(msg.Create(0, 0, nAwardVirtue))
									pLeader->SendMsg(&msg);
							}

							char	szMsg[1024];
							sprintf(szMsg, STR_AWARD_VIRTUE_su, pLeader->GetName(), nAwardVirtue);
							CMsgTalk	msg;
							IF_OK(msg.Create(szMsg))
								pTeam->BroadcastTeamMsg(&msg);
						}
					}
				}
			}
		}
	}

	if (nReserved)
		this->AddContributeExp(i64Exp);
	return i64Exp;
	DEBUG_CATCH("[early debug] AwardBattleExp error.")
	return i64Exp;
}

bool CUser::IsAtkable(IRole* pTarget, bool bSendHint)
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
		return false;

	if (!this->IsPkEnable(pTarget))
	{
		if (bSendHint)
			this->SendSysMsg(STR_NOT_PK_MODE);
		return false;
	}

	if(GetID() == pTarget->GetID())
		return false;

	if(!pTarget->IsBeAtkable())
		return false;

	CNpc* pNpc;
	if(pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
	{
		if(pNpc->IsSynFlag() && GetSynID() != ID_NONE && pNpc->GetSynID() != ID_NONE)
		{
			CSyndicate* pSyn = SynManager()->QuerySyndicate(GetSynID());
			if(pSyn && pSyn->IsNearSyn(pNpc->GetSynID()))
				return false;
		}
	}

	CMonster* pMonster;
	if(pTarget->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
	{
		CSyndicate* pSyn = GetSynID() ? SynManager()->QuerySyndicate(GetSynID()) : NULL;
		if(pSyn && pSyn->IsNearSyn(pMonster->GetSynID()))
			return false;
	}
	CUser* pUserTarget = NULL;
	pTarget->QueryObj(OBJ_USER, IPP_OF(pUserTarget));

	if (pUserTarget && this->GetExp() < 0)
	{
		if (bSendHint)
			this->SendSysMsg(STR_NOT_PK_EXP);
		return false;
	}

	if (!(this->GetMap() && pTarget->GetMap() && this->GetMap() == pTarget->GetMap()))
		return false;

	if (GetMap()->IsNewbieMap() || GetMap()->IsBoothEnable() || (GetMap()->IsPkDisable() && pUserTarget))
	{
		if (bSendHint)
			this->SendSysMsg(STR_PK_FORBIDDEN);
		return false;
	}

	if (GetMap()->IsPkDisable() && pMonster && pMonster->GetID() >= CALLPETID_FIRST && pMonster->GetID() <= CALLPETID_LAST)
	{
		if (bSendHint)
			this->SendSysMsg(STR_PK_FORBIDDEN);
		return false;
	}

	return true;
}

bool CUser::Create(PROCESS_ID idProcess, SOCKET_ID idSocket, OBJID idUser)
{
	CHECKF(CMapGroup::IsValidMapGroupProcessID(idProcess));
	m_idProcess	= idProcess;
	m_idSocket	= idSocket;

	m_nDir		= 0;
	m_nPose		= 0;
	m_nEmotin	= 0;

	if(!m_data.Create(idUser, Database()))
		return false;

	m_data.m_Info.usMaxEnergy	= 100;
	m_data.m_Info.usEnergy		= 0;
	m_data.m_Info.ucPkMode		= PKMODE_SAFE;

	OBJID idMap	= ID_NONE;
	if(m_data.m_Info.dwAutoExercise == 1)
	{
		CGameMap* pMap = MapManager()->GetGameMap(601);
		CHECKF(pMap);
		POINT	pos;
		pMap->GetRebornMap(&idMap, &pos);	idMap = 601;
		m_nPosX		= pos.x;
		m_nPosY		= pos.y;
	}
	else
	{
		m_nPosX		= this->m_data.m_Info.usRecordX;
		m_nPosY		= this->m_data.m_Info.usRecordY;
		idMap		= this->m_data.m_Info.idRecordmap;
	}
	m_pMap		= MapManager()->GetGameMap(idMap);
	if(!m_pMap)
	{
		m_nPosX		= 440;
		m_nPosY		= 400;
		idMap		= 1002;
		m_pMap		= MapManager()->GetGameMap(idMap);
		CHECKF(m_pMap);
	}
	else if(m_data.m_Info.dwAutoExercise != 1 && m_pMap->IsRecordDisable())
	{
		m_nPosX		= 430;
		m_nPosY		= 380;
		idMap		= 1002;
		m_pMap		= MapManager()->GetGameMap(idMap);
		CHECKF(m_pMap);
	}

	Init();

	IDatabase* pDb = Database();
	if(!CreateAllFriend(pDb))
		return false;
	if(!CreateAllBusiness(pDb))
		return false;
	if(!QueryEnemy()->Create(idProcess, this, pDb))
		return false;
	if(!CreateAllItem(pDb))
		return false;
	if (!CreateWeaponSkill(pDb))
		return false;
	if(!m_pBattleSystem)
		return false;
	if (!m_pMagic || !m_pMagic->CreateAll())
		return false;
	if (!m_pSyn || !m_pSyn->Create())
		return false;
	if (!m_setStatus)
		return false;

	if (this->GetLife() <= 0)
		this->AddAttrib(_USERATTRIB_LIFE, 1, SYNCHRO_TRUE, NULL);
	m_data.SetLastLogin();

	InitVerifyAnswer(GetID());
	m_dwUnk6AC	= 1;
	return true;
}

void CUser::OnLoginComplete()
{
	this->SetStatus(STATUS_NORMAL, SYNCHRO_TRUE);
	m_dwUnk8CC	= 0;
	m_nUnk8D0	= 2;
	if (this->IsTimeCardOn())
	{
		this->OnTimeCardMapOut();
		if (this->GetMapID() == 601 || this->GetMapID() == 1039)
			this->OnTimeCardMapIn();
	}

	CMySingleLock xLock(&s_xLogoutRecordLock, true);
	std::map<OBJID, ST_LOGOUT_RECORD>::iterator it = s_setLogoutRecord.find(GetID());
	if (it != s_setLogoutRecord.end())
	{
		m_dwUnk638	= it->second.dwData;
		if (it->second.dwDoubleExp > 0)
			CRole::AttachStatus(this->QueryRole(), 0x08000000, 1, it->second.dwDoubleExp, 0);
		if (it->second.dwDoubleExpRemain > 0)
			this->SetDoubleExp(it->second.dwDoubleExpSecs, it->second.dwDoubleExpRemain);
		s_setLogoutRecord.erase(it);
	}

	DWORD dwSecs = this->GetTimeCardLeft(0, false);
	if (dwSecs > 0)
	{
		char szTime[256] = "";
		this->GetBlessTimeStr(szTime);
		this->SendSysMsg(_TXTATR_SYSTEM, STR_GODBLESS_HINT, szTime);
		if (dwSecs < 86400)
			this->SendSysMsg(STR_GODBLESS_RECHARGE_HINT);
		CRole::AttachStatus(this->QueryRole(), 0x10000000, 1, dwSecs, 0);
	}

	CMsgUserAttrib	msg;
	if (msg.Create(GetID(), 128, m_data.m_Info.dwExpBallUsage))
		this->SendMsg(&msg);

	char szSQL[1024] = "";
	_snprintf(szSQL, 1024, "SELECT * FROM cq_pk_bonus WHERE hunter=%d AND bonus>0", GetID());
	szSQL[1023] = 0;
	if (Database()->CountRecord(szSQL) > 0)
		this->SendSysMsg(STR_PK_HUNTER_BONUS);

	int nCard1 = this->GetEmoneyCard1();
	int nCard2 = this->GetEmoneyCard2();
	if (nCard1 > 0 || nCard2 > 0)
	{
		CMsgInteract	msgCard;
		if (msgCard.Create(31, GetID(), 0, 0, 0, nCard1, nCard2))
			this->SendMsg(&msgCard);
	}

	int nBusiness = this->GetBusinessman();
	if (nBusiness == 0)
	{
		if (GetMap()->IsNewbieMap())
		{
			CMsgInteract	msgBusiness;
			if (msgBusiness.Create(33, GetID(), 0, 0, 0, 0, 0))
				this->SendMsg(&msgBusiness);
		}
	}
	else if (nBusiness == 255)
	{
		this->SynchroBusinessman(255);
	}
	else
	{
		int nToday = ::TimeGet(TIME_DAY) + 19000000;
		if (nToday >= nBusiness)
		{
			this->SetBusinessman(255);
			this->SynchroBusinessman(255);
			::MyLogSave("gmlog/businessman", "UserID:%u\tName:%s\tBusinessState:%u\t", GetID(), GetName(), 255);
		}
		else
		{
			for (int i = 1; i <= 5; i++)
			{
				if (::DateStampPass(nToday, i) == nBusiness)
				{
					CMsgInteract	msgBusiness;
					if (msgBusiness.Create(35, GetID(), 0, 0, 0, 0, i))
						this->SendMsg(&msgBusiness);
					break;
				}
			}
		}
	}
}

void CUser::InitVerifyAnswer(DWORD dwSeed)
{
	srand(dwSeed);
	for (int i = 0; i < 256; i++)
		m_setVerifyAnswer[i] = rand();
}

bool CUser::Init			()
{
	m_tFight.SetInterval(this->GetIntervalAtkRate());
	m_tFight.Update();

	m_tXp.SetInterval(ADD_XP_SECS);
	m_tXp.Update();

	m_tAutoHealLife.SetInterval(10);
	m_tAutoHealLife.Update();

	m_tEquipmentDurability.SetInterval(5);
	m_tEquipmentDurability.Update();

	m_tPK.SetInterval(360);
	m_tPK.Update();

	m_tOnlineTraining.SetInterval(60);
	m_tOnlineTraining.Update();

	m_tBattleEffect.SetInterval(5);
	m_tBattleEffect.Update();

	m_tContributeExp.SetInterval(300);
	m_tContributeExp.Update();

	m_tTeam.SetInterval(TIME_TEAMPRC);
	m_tTeam.Update();

	m_pVerifyDataDetector = CClientDetector::CreateNew(this, 1, 600, 120);
	m_pBattleSystem	= new CBattleSystem(this->m_idProcess, this);
	MYASSERT(m_pBattleSystem);
	m_pMagic = CMagic::CreateNew(m_idProcess, this);
	MYASSERT(m_pMagic);
	m_pSyn	= CSynAttr::CreateNew(m_idProcess, this);
	MYASSERT(m_pSyn);
	m_setStatus	= CStatusSet::CreateNew(true);
	MYASSERT(m_setStatus);

	return true;
}

void CUser::SaveInfo			()
{
	this->ContributeExp(true);
	this->ContributeExp(false);
	m_data.SaveInfo();
	SaveItemInfo();
	SaveAllWeaponSkillInfo();
	m_pMagic->SaveInfo();
	m_pSyn->SaveInfo();
	this->SaveStatistic();

}

bool CUser::SaveAllWeaponSkillInfo()
{
	for(int i = 0; i < m_pUserWeaponSkillSet->GetAmount(); i++)
	{
		CUserWeaponSkill* pData = m_pUserWeaponSkillSet->GetObjByIndex(i);
		if(pData)
			pData->SaveInfo();
	}
	return true;
}

bool CUser::AddLeaveWord(LPCTSTR szRecv, LPCTSTR szWords)
{
	if(!IsLeaveWordEnable())
	{
		SendSysMsg(STR_MSG_FAILED);
		return false;
	}

	for(int i = 0; i < m_setFriend.size(); i++)
	{
		CFriend* pFriend = m_setFriend[i];
		if(pFriend && strcmp(pFriend->GetFriendName(), szRecv) == 0 && UserManager()->GetUser(pFriend->GetFriendID()) == NULL)
		{
			LeaveWord()->InsertWords(GetName(), szRecv, szWords);
			SendSysMsg(STR_MSG_SENT);
			return true;
		}
	}

	for(vector<CTradeBuddy*>::iterator it = m_vecTradeBuddy.begin(); it != m_vecTradeBuddy.end(); it++)
	{
		CTradeBuddy* pBuddy = *it;
		if(pBuddy && strcmp(pBuddy->GetName(), szRecv) == 0 && UserManager()->GetUser(pBuddy->GetBuddyID()) == NULL)
		{
			LeaveWord()->InsertWords(GetName(), szRecv, szWords);
			SendSysMsg(STR_MSG_SENT);
			return true;
		}
	}
	return false;
}

int	 CUser::AdjustData(int nData, int nAdjust, int nMaxData )
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

bool CUser::SendMsg(CNetMsg* pMsg)
{
	DEADLOOP_CHECK(PID, "msg port busy")
	if(false
		||pMsg->GetType() == _MSG_INTERACT
		)
	{
		DEBUG_SENDMSG(pMsg->GetType(), m_idSocket)
	}

	return MapGroup(PID)->QueryIntraMsg()->SendClientMsg(m_idSocket, pMsg);
}

void CUser::BroadcastRoomMsg(CNetMsg* pMsg, bool bSendSelf )
{
	CGameMap* pMap = this->GetMap();
	CHECK(pMap);
	pMap->BroadcastBlockMsg(QueryMapThing(), pMsg, bSendSelf);
}

void CUser::BroadcastRoomMsg(LPCTSTR szMsg, bool bSendSelf )
{
	CMsgTalk	msg;
	if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szMsg, NULL, 0xff0000, _TXTATR_SYSTEM))
		this->BroadcastRoomMsg(&msg, bSendSelf);
}

void CUser::BroadcastMapMsg(CNetMsg* pMsg, bool bSendSelf )
{
	CMapPtr pMap = this->GetMap();
	if(pMap)
		UserManager()->BroadcastMapMsg(pMap->GetID(), pMsg, bSendSelf?NULL:this);
}

void CUser::BroadcastMapMsg(LPCTSTR szMsg, bool bSendSelf )
{
	CMsgTalk	msg;
	if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szMsg, NULL, 0xff0000, _TXTATR_SYSTEM))
		this->BroadcastMapMsg(&msg, bSendSelf);
}

bool CUser::SendSysMsg(const char* fmt, ...)
{
	MSGBUF	szMsg;
    vsprintf( szMsg, fmt, (char*) ((&fmt)+1) );

	CMsgTalk msg;
	if (msg.Create(SYSTEM_NAME, GetName(), szMsg, NULL, 0xff0000, _TXTATR_SYSTEM))
		return this->SendMsg(&msg);

	return false;
}

bool CUser::SendSysMsg(int nChannel, const char* fmt, ...)
{
	MSGBUF	szMsg;
    vsprintf( szMsg, fmt, (char*) ((&fmt)+1) );

	CMsgTalk msg;
	if (msg.Create(SYSTEM_NAME, GetName(), szMsg, NULL, 0x00ffff00, nChannel))
		return this->SendMsg(&msg);

	return false;
}
bool CUser::MoveToward(int nDir)
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

	if(GetMap() && !GetMap()->IsStandEnable(nNewPosX, nNewPosY))
	{
		SendSysMsg(STR_INVALID_COORDINATE);
		KickBack();
		return false;
	}

	pMap->MoveTo(this, nNewPosX, nNewPosY);
	pMap->ChangeRegion(this, nNewPosX, nNewPosY);
	m_nPosX = nNewPosX;
	m_nPosY = nNewPosY;

	{
		int x,y,z;
		IRole*	pRole = NULL;
		FOR_9_BLOCKTHINGS(pMap, GetPosX(), GetPosY())
		{
			IMapThing* pTarget = pMap->QueryThingByIndex(x,y,z);
			if(pTarget)
			{
				int	nDistX	= (pTarget->GetPosX() - this->GetPosX()) * nDirX;
				int	nDistY	= (pTarget->GetPosY() - this->GetPosY()) * nDirY;
				if(nDirX && nDistX == CELLS_PER_VIEW || nDirY && nDistY == CELLS_PER_VIEW)
				{
					pTarget->SendShow(this);

					if(pTarget->QueryRole(IPP_OF(pRole)))
						this->SendShow(pRole);
				}
			}
		}
	}

	ProcessAfterMove();
	return (++m_nUnk8F0 % 3) == 0;
}

void CUser::TransPos(int nPosX, int nPosY)
{
	if(nPosX == GetPosX() && nPosY == GetPosY())
		return;

	CMapPtr pMap = this->GetMap();
	if(!pMap)
		return;
	if(!pMap->IsValidPoint(nPosX, nPosY))
		{ ::LogSave("CUser::TransPos() from map%u(%d, %d) to (%d, %d) at %s, %d", GetMapID(), GetPosX(), GetPosY(), nPosX, nPosY, __FILE__, __LINE__); return; }

	if(GetMap() && !GetMap()->IsStandEnable(nPosX, nPosY))
	{
		SendSysMsg(STR_INVALID_COORDINATE);
		POINT pos = GetMap()->GetPortal();
		this->SetRecordPos(GetMapID(), pos.x, pos.y);
		UserManager()->KickOutSocket(m_idSocket, STR_INVALID_COORDINATE);
		return ;
	}

	pMap->MoveTo(this, nPosX, nPosY);
	pMap->ChangeRegion(this, nPosX, nPosY);
	m_nPosX = nPosX;
	m_nPosY = nPosY;

	pMap->SendBlockInfo(this);
	SendSelfToBlock();

	ProcessAfterMove();
}

void CUser::JumpPos(int nPosX, int nPosY)
{
	if(nPosX == GetPosX() && nPosY == GetPosY())
		return;

	CMapPtr pMap = this->GetMap();
	IF_NOT(pMap && pMap->IsValidPoint(nPosX, nPosY))
		return;

	int	nDeltaX	= abs(nPosX - m_nPosX);
	int	nDeltaY	= abs(nPosY - m_nPosY);

	if(nDeltaX > CELLS_PER_BLOCK || nDeltaY > CELLS_PER_BLOCK)
		return ;

	if(GetMap() && !GetMap()->IsStandEnable(nPosX, nPosY))
	{
		SendSysMsg(STR_INVALID_COORDINATE);
		POINT pos = GetMap()->GetPortal();
		this->SetRecordPos(GetMapID(), pos.x, pos.y);
		UserManager()->KickOutSocket(m_idSocket, STR_INVALID_COORDINATE);
		return ;
	}

	pMap->MoveTo(this, nPosX, nPosY);
	pMap->ChangeRegion(this, nPosX, nPosY);
	int	nOldX	= m_nPosX;
	int	nOldY	= m_nPosY;
	m_nPosX = nPosX;
	m_nPosY = nPosY;

	int x,y,z;
	IRole*	pRole = NULL;
	FOR_9_BLOCKTHINGS(pMap, GetPosX(), GetPosY())
	{
		IMapThing* pTarget = pMap->QueryThingByIndex(x,y,z);
		if(pTarget)
		{
			if(abs(pTarget->GetPosX() - nOldX) > CELLS_PER_VIEW
				&& abs(pTarget->GetPosX() - this->GetPosX()) <= CELLS_PER_VIEW
				|| abs(pTarget->GetPosY() - nOldY) > CELLS_PER_VIEW
				&& abs(pTarget->GetPosY() - this->GetPosY()) <= CELLS_PER_VIEW )
			{
				pTarget->SendShow(this);

				if(pTarget->QueryRole(IPP_OF(pRole)))
					this->SendShow(pRole);
			}
		}
	}

	ProcessAfterMove();
}

bool CUser::ChangeMap()
{
	DetachLuckyStatus();
	m_dwUnk63C	= ::TimeGet();

	POINT	pos;
	pos.x	= GetPosX();
	pos.y	= GetPosY();

	int nX[9]={0,1,-1,1,0,1,-1,-1,0};
	int nY[9]={0,1,-1,0,1,-1,1,0,-1};
	for (int i=0; i<9; i++)
	{
		POINT posTest = pos;
		posTest.x += nX[i];
		posTest.y += nY[i];

		OBJID	idMap = ID_NONE;
		POINT	posTarget = {0, 0};
		if (GetMap()->GetPassageMap(&idMap, &posTarget, posTest))
		{
			if(FlyMap(idMap, posTarget.x, posTarget.y) != FLYMAP_ERROR)
				return true;
			else
				break;
		}
	}

	UserManager()->KickOutSocket(m_idSocket, "no map passway!");
	return false;
}

OBJID CUser::GetMapID()
{
	if(m_pMap)
		return m_pMap->GetID();
	return ID_NONE;
}

OBJID CUser::GetRecordMapID()
{
	return m_data.GetRecordMap();
}

void CUser::GotoJail(OBJID idMap, LPCTSTR pszText)
{
	CGameMap* pMap = MapManager()->QueryMap(idMap);
	IF_OK (pMap)
	{
		if(strcmp(STR_GOTO_CHEATJAIL, pszText) != 0)
		{
			char szMsg[1024] = "";
			if(pszText)
				sprintf(szMsg, pszText, this->GetName());
			UserManager()->BroadcastMsg(szMsg);
		}

		this->QuitTeam();

		POINT pos = pMap->GetPortal();
		this->FlyMap(idMap, pos.x, pos.y);

		{
			OBJID	idRebornMap = ID_NONE;
			POINT	pos;
			IF_OK(GetMap()->GetRebornMap(&idRebornMap, &pos))
				this->SetRecordPos(idRebornMap, pos.x, pos.y);
		}
	}
}

int CUser::FlyMap(OBJID idMap, int nPosX, int nPosY)
{
	if(m_pMap == NULL)
	{
		LOGWARNING("Player changed maps repeatedly.");
		return FLYMAP_ERROR;
	}

	if (idMap == ID_NONE)
		idMap = this->GetMapID();

	DetachLuckyStatus();
	m_dwUnk63C	= ::TimeGet();

	PROCESS_ID idProcess = MapManager()->GetProcessID(idMap);
	if(!CMapGroup::IsValidMapGroupProcessID(idProcess) || idProcess == m_idProcess)
	{
		CGameMap* pNewMap = MapManager()->GetGameMap(idMap);
		if(!pNewMap || !pNewMap->IsValidPoint(nPosX, nPosY))
		{
			LOGMSG("FLY: map [%u] position [%u][%u] illegal!", idMap, nPosX, nPosY);
			return FLYMAP_ERROR;
		}

		DEBUG_TRY
		LeaveMap();
		m_pMap		= pNewMap;
		m_nPosX		= nPosX;
		m_nPosY		= nPosY;

		CMsgAction	msg;
		if(msg.Create(GetID(), nPosX, nPosY, GetDir(), 86, pNewMap->GetDocID()))
			SendMsg(&msg);

		EnterMap();
		DEBUG_CATCH("FlyMap")

		switch(m_nUnk8D0)
		{
		case 0:
			if(idMap == 601 || idMap == 1039)
				OnTimeCardMapIn();
			break;
		case 1:
			if(idMap != 601 && idMap != 1039)
				OnTimeCardMapLeave();
			break;
		}
		return FLYMAP_NORMAL;
	}
	else
	{
		ChangeMapGroup(idProcess, idMap, nPosX, nPosY);
		return FLYMAP_MAPGROUP;
	}
}

void CUser::AddDelayAction(int nSecs, OBJID idAction)
{
	DELAY_ACTION	info;
	info.tDelay.Startup(nSecs);
	info.idAction	= idAction;
	m_setDelayAction.push_back(info);
}

void CUser::ProcessLuckyTime()
{
	DWORD	dwNow	= ::TimeGet();
	if(QueryStatus(STATUS_LUCKY_PRAY))
	{
		AddAttrib(_USERATTRIB_UNK29, 1500, SYNCHRO_FALSE);
		if(!(m_dwTimerCount & 1))
		{
			MAPTHING_SET	setThing;
			POINT	pos;
			pos.x	= GetPosX();
			pos.y	= GetPosY();
			GetMap()->CollectMapThing(setThing, pos, 3, OBJ_USER);
			for(MAPTHING_SET::iterator iter = setThing.begin(); iter != setThing.end(); iter++)
			{
				IMapThing* pThing = *iter;
				CUser* pUser = NULL;
				if(pThing && pThing->QueryObj(OBJ_USER, IPP_OF(pUser)) && pUser->IsAlive()
						&& dwNow >= pUser->m_dwUnk63C + 3000
						&& pUser->GetID() != GetID()
						&& pUser->GetMetempsychosis() < 2
						&& !pUser->QueryStatus(STATUS_LUCKY))
				{
					CRole::AttachStatus(pUser->QueryRole(), STATUS_LUCKY, 100, 1000000, 0);
					pUser->AddAttrib(_USERATTRIB_UNK29, 0, SYNCHRO_TRUE);
				}
			}
		}
	}
	else if(QueryStatus(STATUS_LUCKY))
		AddAttrib(_USERATTRIB_UNK29, 500, SYNCHRO_FALSE);
	else if(m_dwUnk638 > 0)
		AddAttrib(_USERATTRIB_UNK29, -500, !(m_dwTimerCount & 1));
}

void CUser::OnTimer(time_t tCurr)
{
	if(m_pMap == NULL)
		return;

	m_dwTimerCount++;
	ProcessLuckyTime();

#ifndef _DEBUG
	DEBUG_TRY
	DEADLOOP_CHECK(PID, "m_dequeServerTick")
	if(!IsAgent() && !IsGM())
	{
		DWORD dwTimeNow = ::TimeGet();

		if(m_dequeServerTick.size() >= 10)
		{
			UserManager()->KickOutSocket(m_idSocket);
			return;
		}

		if (m_dwFirstServerTick == 0)
		{
			m_dwFirstServerTick = dwTimeNow;
			m_dwLastServerTick	= dwTimeNow;

			m_dwLastRcvClientTick = dwTimeNow;

			CMsgTick msg;
			if(msg.Create(GetID()))
				this->SendMsg(&msg);

			m_dequeServerTick.push_back(dwTimeNow);
		}
		else
		{
			if (dwTimeNow >= m_dwLastServerTick+TICK_SECS*1000)
			{
				m_dwLastServerTick	= dwTimeNow;
				m_dequeServerTick.push_back(dwTimeNow);

				CMsgTick msg;
				if(msg.Create(GetID()))
					this->SendMsg(&msg);
			}
		}

		if (m_dwLastRcvClientTick != 0)
		{
			if (dwTimeNow-m_dwLastRcvClientTick >= TICK_SECS*1000*2.5)
			{
				UserManager()->KickOutSocket(m_idSocket);
				return;
			}
		}
	}
	DEBUG_CATCH("@m_dwServerTickCount@")
#endif

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "ProcessAutoAttack")
	ProcessAutoAttack();
	DEBUG_CATCH("CUser ProcAttack_Hand2Hand carsh")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "ProcXpVal")
	if (m_tXp.ToNextTime())
		this->ProcXpVal();
	DEBUG_CATCH("CUser ProcXpVal crash")

	for(DELAY_ACTION_SET::iterator it = m_setDelayAction.begin(); it != m_setDelayAction.end(); )
	{
		DELAY_ACTION_SET::iterator iter = it++;
		if(iter->tDelay.TimeOver())
		{
			GameAction()->ProcessAction(iter->idAction, this);
			m_setDelayAction.erase(iter);
		}
	}

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "ProcAutoHealLife")
	if (m_tAutoHealLife.ToNextTime())
		this->ProcAutoHealLife();
	DEBUG_CATCH("CUser autoheallife timer crash.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "autoheallife timer")
	if(IsAlive() && GetEnergy() < GetMaxEnergy() && !m_pTransformation && !IsWing())
	{
		if (_ACTION_SITDOWN == this->GetPose())
		{
			if (m_tEnergy.ToNextTime(1))
				AddAttrib(_USERATTRIB_ENERGY, 10, SYNCHRO_TRUE);
		}
		else
		{
			if (m_tEnergy.ToNextTime(2))
				AddAttrib(_USERATTRIB_ENERGY, 3, SYNCHRO_TRUE);
		}
	}
	DEBUG_CATCH("CUser autoheallife timer crash.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "QueryMagic()->OnTimer")
	QueryMagic()->OnTimer(tCurr);
	DEBUG_CATCH("CUser magic timer crash.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "superman list")
	for(int i = QueryStatusSet()->GetAmount()-1; i >= 0; i--)
	{
		IStatus* pStatus = QueryStatusSet()->GetObjByIndex(i);
		if(pStatus)
		{
			pStatus->OnTimer(tCurr);
			if (!pStatus->IsValid())
			{
				OBJID idStatus = pStatus->GetID();
				QueryStatusSet()->DelObj(idStatus);

				if (idStatus == STATUS_SUPERMAN || idStatus == STATUS_TORNADO)
				{
					CalcSupermanKills();
					CalcFightRate();
				}
				if (idStatus == 0x10000000)
					OnTimeCardEnd();
			}
		}
	}
	DEBUG_CATCH("CUser status timer crash.")

	DEBUG_TRY
	if (m_tEquipmentDurability.ToNextTime())
		this->UpdateEquipmentDurability();
	DEBUG_CATCH("UpdateEquipmentDurability crashed.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "Booth")
	if(m_pBooth)
		m_pBooth->OnTimer();
	DEBUG_CATCH("Booth crashed.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "ProcessPkTimer")
	if (m_tPK.ToNextTime())
		this->ProcessPkTimer();
	DEBUG_CATCH("CUser pk timer crash.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "ProcessOnlineTraining")
	if (m_tOnlineTraining.ToNextTime())
		this->ProcessOnlineTraining();
	DEBUG_CATCH("CUser Online training crash.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "ProcessCheckBattleEffect")
	if (m_tBattleEffect.ToNextTime())
		this->ProcessCheckBattleEffect();
	DEBUG_CATCH("CUser Check BattleEffect crash.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "ProcessContributeExp")
	if (m_tContributeExp.ToNextTime())
		this->ContributeExp(false);
	DEBUG_CATCH("CUser Check ProcessContributeExp crash.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "ProcessMineTimer")
	if (m_tMine.IsActive() && m_tMine.ToNextTime())
	{
		this->ProcessMineTimer();
		ResetEnergyInc();
	}
	DEBUG_CATCH("CUser mine timer crash.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "ProcessTeamTimer")
	if (this->GetTeam() && m_tTeam.ToNextTime())
		this->ProcessTeamTimer();
	DEBUG_CATCH("CUser team timer crash.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "AbortTransform")
	if (m_pTransformation && m_tTransformation.TimeOver())
	{
		AbortTransform();
	}
	if (m_dwUnk610 != -1 && m_tUnk614.TimeOver())
	{
		AbortLook();
	}
	DEBUG_CATCH("CUser Transformation crash.")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "KillCallPet")
	if(m_tCallPet.IsActive() && m_tCallPet.TimeOver())
	{
		KillCallPet();
	}
	DEBUG_CATCH("CUser CallPet crash.")

	if(m_pVerifyDataDetector)
		m_pVerifyDataDetector->OnTimer();
	if(!m_tVerify.IsActive() && !m_tCheatKickout.IsActive() && g_bCheckVerifyData && !IsInCheatJail())
	{
		if(::RandGet(360) == 7)
			SendVerifyCode(180);
	}
	if(!IsGM() && m_tVerify.TimeOver())
	{
		if(IsCheater(8) || IsCheater(7))
		{
			::MyLogSave("gmlog/cheater", "Invalid Client Detected. User:%s[%u].", GetName(), GetID());
			m_tCheatKickout.Startup(::RandGet(60) + 300);
		}
	}
	if(m_tCheatKickout.TimeOver() && g_bCheckVerifyData)
		GotoJail(6002, STR_GOTO_CHEATJAIL);
}

void CUser::ProcessAutoAttack		()
{
	int nFightPause = m_nFightRate + m_nFightPause;

	if (m_pBattleSystem && m_pBattleSystem->IsActived()
		&& m_tFight.ToNextTick(nFightPause))
	{
		m_pBattleSystem->ProcAttack_Hand2Hand();
		m_nFightPause	= 0;
		if(m_pBattleSystem->GetTargetID() != ID_NONE)
			ResetEnergyInc();

		DetachLuckyStatus();
		m_dwUnk63C	= ::TimeGet();
	}
}

void CUser::ClearAllStatus		(DWORD dwKeepStatus)
{
	ClsXpVal(SYNCHRO_FALSE);
	for(int i = QueryStatusSet()->GetAmount() - 1; i >= 0; i--)
	{
		IStatus* pStatus = QueryStatusSet()->GetObjByIndex(i);
		if(pStatus)
		{
			StatusInfoStruct	info;
			pStatus->GetInfo(&info);
			if(!(info.nStatus & dwKeepStatus))
				QueryStatusSet()->DelObj(pStatus->GetID());
		}
	}
}

void CUser::ProcessPkTimer()
{
	if (this->GetPk() > 0)
		this->AddPk(-1*PKVALUE_DEC_ONCE);
}

bool CUser::IsPkEnable(IRole* pRole)
{
	if (!pRole || this->GetID() == pRole->GetID())
		return false;

	CMonster* pMonster = NULL;
	if (pRole->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
	{
		if (pMonster->IsMonster())
			return true;

		if (pMonster->GetID() >= PETID_FIRST && pMonster->GetID() <= PETID_LAST && pMonster->GetOwnerLink() == NULL)
			return true;
	}

	CNpc* pNpc = NULL;
	pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc));

	if (pNpc)
		return true;

	bool bCanPk = false;
	switch(this->GetPkMode())
	{
	case	PKMODE_FREE:
		bCanPk = true;
		break;

	case	PKMODE_TEAM:
		{
			bCanPk = true;

			CTeam* pTeam = this->GetTeam();
			if(pTeam && pTeam->IsTeamMember(pRole->GetID()))
				bCanPk = false;

			if (this->GetFriend(pRole->GetID()))
				bCanPk = false;

			CUser* pUser = NULL;
			if (pRole->QueryObj(OBJ_USER, IPP_OF(pUser)) && this->GetSynID() != ID_NONE)
			{
				if (this->GetSynID() == pUser->GetSynID() || this->IsMate(pUser))
					bCanPk = false;
				else
				{
					CSyndicate* pSyn = SynManager()->QuerySyndicate(this->GetSynID());
					if (pSyn)
					{
						if (pSyn->IsNearSyn(pUser->GetSynID()) || pSyn->IsFriendly(pUser->GetSynID(), true))
							bCanPk = false;
					}
				}
			}
			return bCanPk;
		}
		break;

	case	PKMODE_ARRESTMENT:
		{
			if(pRole->IsEvil())
				bCanPk = true;
		}
		break;

	case	PKMODE_SAFE:
		bCanPk = false;
		break;

	default:
		MYASSERT(!"switch pk mode");
		bCanPk = false;
		break;
	}

	return bCanPk;
}

void CUser::Reborn(bool bChgPos, bool bAction)
{
	if (this->IsAlive())
		return;

	if (bChgPos && !m_tAutoReborn.TimeOver())
	{
		::LogSave("Cheat: %s reborn in %d seconds!", this->GetName(), m_tAutoReborn.GetInterval() - m_tAutoReborn.GetRemain());
		m_tAutoReborn.Startup(60);
		return;
	}

	SetStatus(STATUS_NORMAL);
	m_bGhost	= false;
	ClsXpVal(SYNCHRO_FALSE);

	SetAttrib(_USERATTRIB_LIFE, GetMaxLife(), SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_KEEPEFFECT, GetEffect(0), SYNCHRO_BROADCAST);
	SetAttrib(_USERATTRIB_LOOKFACE, GetLookFace(), SYNCHRO_BROADCAST);
	AddAttrib(_USERATTRIB_ENERGY, 100, SYNCHRO_TRUE);

	const bool bMaxLife = false;
	this->BroadcastTeamLife(bMaxLife);

	if (bChgPos)
	{
		if (ID_NONE != m_data.GetRecordMap())
			this->FlyMap(m_data.GetRecordMap(), m_data.GetRecordX(), m_data.GetRecordY());
	}

	int nTimeCard = this->GetTimeCardLeft(0, false);
	if (nTimeCard > 0)
		CRole::AttachStatus(this->QueryRole(), 0x10000000, 1, nTimeCard, 0);
	m_tLock.Startup(CHGMAP_LOCK_SECS);

	if (bAction)
		GameAction()->ProcessAction(80000003, this);
}

void CUser::TransformGhost()
{
	m_bGhost = true;

	SetAttrib(_USERATTRIB_KEEPEFFECT, SetEffect(KEEPEFFECT_GHOST, 0), SYNCHRO_BROADCAST);
	SetAttrib(_USERATTRIB_LOOKFACE, GetLookFace(), SYNCHRO_BROADCAST);
}

int CUser::AddPk(int nAdd)
{
	MYASSERT(nAdd != 0);

	if(abs(m_data.GetPk() + nAdd) < MAX_PKLIMIT)
		m_data.SetPk(m_data.GetPk() + nAdd);
	else
		m_data.SetPk(MAX_PKLIMIT);

	if(nAdd > 0)
		SetStatus(STATUS_PKVALUE, SYNCHRO_TRUE);
	else
		ClsStatus(STATUS_PKVALUE, SYNCHRO_TRUE);

	this->SetAttrib(_USERATTRIB_PK, this->GetPk(), SYNCHRO_TRUE);
	return m_data.GetPk();
}

void CUser::ProcXpVal()
{

	if (this->GetLev() < 3)
		return;

	if(!IsAlive())
	{
		ClsXpVal(SYNCHRO_FALSE);
		return;
	}

	if (m_dwXP >= MAX_USER_XP)
	{
		ClsXpVal(SYNCHRO_TRUE);
	}
	else
	{
		m_dwXP	= __min(m_dwXP + ADD_XP_VALUE, MAX_USER_XP);
		if (m_dwXP == MAX_USER_XP)
		{
			SetEffect(KEEPEFFECT_XPFULL, 0);
			SetAttrib(_USERATTRIB_KEEPEFFECT, GetEffect(0), SYNCHRO_BROADCAST);
		}
	}
	SetAttrib(_USERATTRIB_XP, m_dwXP, SYNCHRO_TRUE);

#ifdef _DEBUG
	this->SendSysMsg("[Debug]玩家积累了%u的Xp。Xp=%d则满。", m_dwXP, MAX_USER_XP);
#endif
}

void CUser::ClsXpVal(bool bSynchro)
{
	m_dwXP	= 0;
	m_tXp.Update();

	ResetEffect(KEEPEFFECT_XPFULL, 0);
	if (bSynchro)
		SetAttrib(_USERATTRIB_KEEPEFFECT, GetEffect(0), SYNCHRO_BROADCAST);
}

bool CUser::BurstXp()
{
	if(m_dwXP < MAX_USER_XP)
		return false;

	ClsXpVal(SYNCHRO_TRUE);
	return true;
}

void CUser::SetXp(int nXp)
{
	if (nXp < 0 || GetXp() == nXp)
		return;

	this->SetAttrib(_USERATTRIB_XP, nXp, SYNCHRO_TRUE);
	if (m_dwXP >= MAX_USER_XP)
		this->SetAttrib(_USERATTRIB_KEEPEFFECT, GetEffect(0) | KEEPEFFECT_XPFULL, SYNCHRO_BROADCAST);
}

void CUser::AddXp(int nXp)
{
	if (nXp < 0)
		return;

	this->AddAttrib(_USERATTRIB_XP, nXp, SYNCHRO_TRUE);
	if (m_dwXP >= MAX_USER_XP)
		this->SetAttrib(_USERATTRIB_KEEPEFFECT, GetEffect(0) | KEEPEFFECT_XPFULL, SYNCHRO_BROADCAST);
}

void CUser::ProcAutoHealLife()
{
	if (this->GetPose() != _ACTION_SITDOWN ||
			this->GetLife() >= this->GetMaxLife() ||
				!this->IsAlive())
		return;

	this->AddAttrib(_USERATTRIB_LIFE, AUTOHEALLIFE_EACHPERIOD, SYNCHRO_TRUE);
}

void CUser::ProcessTick(DWORD dwClientTime)
{
#ifdef _DEBUG
	return;
#endif

	if (this->IsGM())
		return;

	if (m_dwFirstClientTick == 0)
		m_dwFirstClientTick = dwClientTime;

	if (dwClientTime < m_dwLastClientTick)
	{
		this->SendSysMsg(STR_CONNECTION_OFF);
		UserManager()->KickOutSocket(m_idSocket, "ProcessTick");
		return;
	}

	const int CRITICAL_TICK_PERCENT = 5;

	int nServerTicks = m_dequeServerTick.size();
	if (m_dwLastClientTick != 0 &&
			nServerTicks >= 2 &&
				dwClientTime > m_dwLastClientTick+TICK_SECS*(100+CRITICAL_TICK_PERCENT)*10)
	{
		if (dwClientTime - m_dwLastClientTick > ::TimeGet() - m_dequeServerTick[0])
		{
			::GmLogSave("Player (%s) uses a speed hack.", this->GetName());

			this->SendSysMsg(STR_CONNECTION_OFF);
			UserManager()->KickOutSocket(m_idSocket, "Speed hack");
		}
	}

	m_dwLastClientTick = dwClientTime;

	if (m_dequeServerTick.size() >= 2)
		m_dequeServerTick.pop_front();

	m_dwLastRcvClientTick = ::TimeGet();
}

CFriend* CUser::GetFriend(OBJID idFriend)
{
	for(int i = 0; i < (int)m_setFriend.size(); i++)
	{
		CFriend* pFriend = m_setFriend[i];
		if(pFriend && pFriend->GetFriendID() == idFriend)
			return m_setFriend[i];
	}

	return NULL;
}

bool CUser::AddFriend(OBJID idFriend, LPCTSTR szFriendName)
{
	CFriend* pFriend = CFriend::CreateNew();
	if(pFriend && pFriend->CreateRecord(GetID(), idFriend, szFriendName, Database()))
	{
		m_setFriend.push_back(pFriend);
		return true;
	}

	return false;
}

bool CUser::DelFriend(OBJID idFriend)
{
	for(int i = 0; i < (int)m_setFriend.size(); i++)
	{
		CFriend* pFriend = m_setFriend[i];
		if(pFriend && pFriend->GetFriendID() == idFriend)
		{
			pFriend->DeleteRecord(Database());
			delete pFriend;
			m_setFriend.erase(m_setFriend.begin() + i);
			return true;
		}
	}

	return false;
}

BOOL CUser::CreateWeaponSkill(IDatabase* pDb)
{
	CHECKF(pDb);

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT * FROM cq_weapon_skill WHERE owner_id=%u && unlearn=0", this->GetID());
	IRecordset* pRes = pDb->CreateNewRecordset(szSQL);
	if(pRes)
	{
		for (int i=0; i<(int)pRes->RecordCount(); i++)
		{
			CUserWeaponSkill*	pSkill	= CUserWeaponSkill::CreateNew(pRes);
			if (pSkill)
				m_pUserWeaponSkillSet->AddObj(pSkill);

			pRes->MoveNext();
		}
		pRes->Release();
	}
	return true;
}
bool CUser::CheckWeaponSubType(int nSubType, int nNum)
{
	CHECKF(nSubType);

	if(nSubType == 900)
	{
		if(m_pWeaponL && m_pWeaponL->IsShield() && m_pWeaponL->GetInt(ITEMDATA_AMOUNT) >= nNum)
			return true;

		return false;
	}

	if (m_pWeaponR && m_pWeaponR->GetItemSubType() == nSubType && m_pWeaponR->GetInt(ITEMDATA_AMOUNT) >= nNum)
		return true;
	if (m_pWeaponL && m_pWeaponL->GetItemSubType() == nSubType && m_pWeaponL->GetInt(ITEMDATA_AMOUNT) >= nNum)
		return true;

	if(nSubType == 420)
	{
		nSubType	= 421;
		if (m_pWeaponR && m_pWeaponR->GetItemSubType() == nSubType && m_pWeaponR->GetInt(ITEMDATA_AMOUNT) >= nNum)
			return true;
		if (m_pWeaponL && m_pWeaponL->GetItemSubType() == nSubType && m_pWeaponL->GetInt(ITEMDATA_AMOUNT) >= nNum)
			return true;
	}

	return false;
}

void CUser::AddWeaponSkillExp(int nSkillType, int nExp)
{
	if(nSkillType == ARROW_SUBTYPE)
		return ;

	if (this->IsIncomeLimited())
		nExp = ::MulDiv(nExp, this->GetIncomePercent(), 100);

	for (int i=0; i<m_pUserWeaponSkillSet->GetAmount(); i++)
	{
		CUserWeaponSkill* pSkill	= m_pUserWeaponSkillSet->GetObjByIndex(i);
		if (pSkill && pSkill->GetType() == nSkillType)
		{
			int nIncreaseLev	= 0;

			if (pSkill->GetLevel() > MASTER_WEAPONSKILLLEVEL)
			{
				int nRatio = 100-(pSkill->GetLevel()-MASTER_WEAPONSKILLLEVEL)*20;
				if (nRatio < 10)
					nRatio = 10;

				nExp = ::MulDiv(nExp, nRatio, 100)/2;
			}

			int nNewExp	= (int)__min((__int64)nExp+pSkill->GetExp(), (__int64)0x7FFFFFFF);

#ifdef _DEBUG
			this->SendSysMsg(_TXTATR_NORMAL, "Add Weapon Skill exp: %u, CurExp: %u", nExp, nNewExp);
#endif

			int nLevel = pSkill->GetLevel();
			if (nLevel >= 1 && nLevel < MAX_WEAPONSKILLLEVEL)
			{
				if (nNewExp > nWeaponSkillLeveUpNeedExp[nLevel])
				{
					nNewExp			= 0;
					if (nLevel+1 >= pSkill->GetOldLev()/2 && nLevel+1 < pSkill->GetOldLev())
					{
						nIncreaseLev	= pSkill->GetOldLev() - pSkill->GetLevel();
						pSkill->SetOldLev(0, true);
					}
					else
						nIncreaseLev	= 1;
				}
			}
			else
				return;

			bool bSave = false;
			if (nIncreaseLev > 0)
				bSave = true;
			else if (pSkill->GetLevel() < MASTER_WEAPONSKILLLEVEL)
			{
				CHECK(nWeaponSkillLeveUpNeedExp[nLevel]);
				int nOldExpPercent = ::MulDiv(pSkill->GetExp(), 100, nWeaponSkillLeveUpNeedExp[nLevel]);
				int nNewExpPercent = ::MulDiv(nNewExp, 100, nWeaponSkillLeveUpNeedExp[nLevel]);

				if ((nOldExpPercent/10) != (nNewExpPercent/10))
					bSave = true;
			}

			pSkill->SetExp(nNewExp, bSave);

			if (nIncreaseLev > 0)
			{
				pSkill->SetLevel(pSkill->GetLevel() + nIncreaseLev, true);

				CMsgWeaponSkill msg;
				if (msg.Create(pSkill->GetType(), pSkill->GetLevel(), pSkill->GetExp()))
					this->SendMsg(&msg);

				this->SendSysMsg(STR_WEAPON_SKILL_UP);

				QueryMagic()->UserWeaponSkillUpLevel(nSkillType, pSkill->GetLevel());
			}
			else
			{
				CMsgFlushExp msg;
				if (msg.Create(MSGFLUSHEXP_WEAPONSKILL, pSkill->GetType(), nNewExp))
					this->SendMsg(&msg);

				if(nExp)
					QueryMagic()->AutoLearnWeaponSkill(nSkillType, pSkill->GetLevel(), nExp);
			}

			return;
		}
	}

	UserWeaponSkillInfoStruct info;
	info.id			= ID_NONE;
	info.idowner	= this->GetID();
	info.nExp		= __max(0, nExp);
	info.nType		= nSkillType;
	info.ucLevel	= 1;
	info.ucOldLev	= 1;
	info.ucUnlearn	= 0;

	OBJID idNew = ID_NONE;
	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id FROM cq_weapon_skill WHERE owner_id=%u && type=%d", GetID(), nSkillType);
	CAutoPtr<IRecordset> pRes = _Database(PID)->CreateNewRecordset(szSQL);
	if(pRes)
		idNew = pRes->GetInt(0);

	CUserWeaponSkill* pSkill	= CUserWeaponSkill::InsertNewRecord(&info, GameDataDefault()->GetUserWeaponSkill(), idNew);
	IF_OK(pSkill)
	{
		m_pUserWeaponSkillSet->AddObj(pSkill);
		CMsgWeaponSkill msg;
		if (msg.Create(pSkill->GetType(), pSkill->GetLevel(), pSkill->GetExp()))
			this->SendMsg(&msg);
	}
}

DWORD CUser::GetWeaponSkillLev (OBJID idWeaponType)
{
	int nSkillType = (idWeaponType%1000000)/1000;
	int nWeaponSkills = m_pUserWeaponSkillSet->GetAmount();
	for (int i=0; i<nWeaponSkills; i++)
	{
		CUserWeaponSkill* pSkill	= m_pUserWeaponSkillSet->GetObjByIndex(i);
		if (pSkill && pSkill->GetType() == nSkillType)
			return pSkill->GetLevel();
	}

	return 1;
}

CUserWeaponSkill* CUser::GetWeaponSkill (int nSkillType)
{
	IF_NOT (m_pUserWeaponSkillSet)
		return NULL;

	int nWeaponSkills = m_pUserWeaponSkillSet->GetAmount();
	for (int i=0; i<nWeaponSkills; i++)
	{
		CUserWeaponSkill* pSkill	= m_pUserWeaponSkillSet->GetObjByIndex(i);
		if (pSkill && pSkill->GetType() == nSkillType)
			return pSkill;
	}

	return NULL;
}

BOOL CUser::AwardWeaponSkill (int nSkillType, int nLev)
{
	CUserWeaponSkill* pSkill = this->GetWeaponSkill(nSkillType);
	if (pSkill)
	{
		pSkill->SetLevel(nLev, true);

		CMsgWeaponSkill msg;
		IF_OK (msg.Create(pSkill->GetType(), pSkill->GetLevel(), pSkill->GetExp()))
			this->SendMsg(&msg);

		SendSysMsg(STR_WEAPON_SKILL_UPLEV);
		return true;
	}
	else
	{
		UserWeaponSkillInfoStruct info;
		info.id			= ID_NONE;
		info.idowner	= this->GetID();
		info.nExp		= 0;
		info.nType		= nSkillType;
		info.ucLevel	= nLev;
		info.ucOldLev	= 1;
		info.ucUnlearn	= 0;

		OBJID idNew = ID_NONE;
		SQLBUF	szSQL;
		sprintf(szSQL, "SELECT id FROM cq_weapon_skill WHERE owner_id=%u && type=%d", GetID(), nSkillType);
		CAutoPtr<IRecordset> pRes = _Database(PID)->CreateNewRecordset(szSQL);
		if(pRes)
			idNew = pRes->GetInt(0);

		CUserWeaponSkill* pSkill = CUserWeaponSkill::InsertNewRecord(&info, GameDataDefault()->GetUserWeaponSkill(), idNew);
		IF_OK (pSkill)
		{
			m_pUserWeaponSkillSet->AddObj(pSkill);

			CMsgWeaponSkill msg;
			IF_OK (msg.Create(pSkill->GetType(), pSkill->GetLevel(), pSkill->GetExp()))
				this->SendMsg(&msg);
		}

		return true;
	}
}

void CUser::SendAllWeaponSkillInfo()
{
	for (int i=0; i<m_pUserWeaponSkillSet->GetAmount(); i++)
	{
		CUserWeaponSkill* pSkill	= m_pUserWeaponSkillSet->GetObjByIndex(i);
		if (pSkill)
		{
			CMsgWeaponSkill msg;
			if (msg.Create(pSkill->GetType(), pSkill->GetLevel(), pSkill->GetExp()))
				this->SendMsg(&msg);
		}
	}
}

bool CUser::UnlearnAllSkill()
{
	CHECKF(m_pUserWeaponSkillSet);

	int nWeaponSkills = m_pUserWeaponSkillSet->GetAmount();
	for (int i = nWeaponSkills-1; i >= 0; i--)
	{
		CUserWeaponSkill* pSkill	= m_pUserWeaponSkillSet->GetObjByIndex(i);
		if(pSkill)
		{
			pSkill->SetOldLev(pSkill->GetLevel(), UPDATE_FALSE);
			pSkill->SetUnlearn(true, UPDATE_TRUE);

			int nType = pSkill->GetType();
			m_pUserWeaponSkillSet->DelObj(pSkill->GetID());

			CMsgAction	msg;
			IF_OK(msg.Create(this->GetID(), this->GetPosX(), this->GetPosY(), this->GetDir(), actionDropSkill, nType))
				this->SendMsg(&msg);
		}
	}

	return true;
}
bool CUser::CreateAllFriend(IDatabase* pDb)
{
	SQLBUF		szSQL;
	sprintf(szSQL, "SELECT * FROM cq_friend WHERE userid = %u LIMIT %d", GetID(), _MAX_USERFRIENDSIZE);
	IRecordset*	pRes = pDb->CreateNewRecordset(szSQL);
	if(pRes)
	{
		for(int i = 0; i < (int)pRes->RecordCount(); i++)
		{
			CFriendPtr	pFriend = CFriend::CreateNew();
			if(pFriend)
			{
				if(pFriend->Create(pRes))
					m_setFriend.push_back(pFriend);
				else
					delete pFriend;
			}
			pRes->MoveNext();
		}
		pRes->Release();
	}
	return true;
}
void CUser::DeleteAllFriend()
{
	for(int i = 0; i < (int)m_setFriend.size(); i++)
		SAFE_DELETE(m_setFriend[i]);
	m_setFriend.clear();
}

void CUser::SendFriendInfo()
{
	int	nAmount = m_setFriend.size();
	CHECK(nAmount <= _MAX_USERFRIENDSIZE);

	for(int i = 0; i < nAmount; i++)
	{
		CFriend* pFriend = m_setFriend[i];
		if(pFriend)
		{
			OBJID	idFriend	= pFriend->GetFriendID();
			CUser* pFriendUser	= UserManager()->GetUser(idFriend);
			bool	bOnline		= pFriendUser!=0;
			int		nPeerage	= pFriendUser ? pFriendUser->GetPeerageRank() : 0;
			int		nSex		= pFriendUser ? pFriendUser->GetSex() : 0;

			CMsgFriend	msg;
			if(msg.Create(_FRIEND_GETINFO, idFriend, nPeerage, nSex, pFriend->GetFriendName(), bOnline))
				SendMsg(&msg);

			if(!bOnline)
				MapGroup(PID)->QueryIntraMsg()->TransmitMsg(&msg, GetSocketID(), GetID());
		}
	}

	for(int i = 0; i < nAmount; i++)
	{
		CFriend* pFriend = m_setFriend[i];
		if(pFriend)
		{
			OBJID	idFriend	= pFriend->GetFriendID();
			CMsgFriend	msg;
			IF_OK(msg.Create(_FRIEND_ONLINE, GetID(), GetPeerageRank(), GetSex(), GetName()))
				MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(&msg, idFriend);
		}
	}

	return;
}

void CUser::BroadcastFriendsMsg	(CNetMsg* pMsg)
{
	if (!pMsg)
		return;

	int	nAmount = m_setFriend.size();
	CHECK(nAmount <= _MAX_USERFRIENDSIZE);

	for(int i = 0; i < nAmount; i++)
	{
		CFriend* pFriend = m_setFriend[i];
		if(pFriend)
		{
			OBJID	idFriend	= pFriend->GetFriendID();
			CUser* pUserFriend = UserManager()->GetUser(idFriend);
			if (pUserFriend)
				pUserFriend->SendMsg(pMsg);
			else
			{
				MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(pMsg, pFriend->GetFriendID());
			}
		}
	}

	return;
}

const int	_MAX_TRADE_BUDDY_SIZE	= 20;

bool CUser::DelTradeBuddy(OBJID idBuddy)
{
	for(vector<CTradeBuddy*>::iterator iter = m_vecTradeBuddy.begin(); iter != m_vecTradeBuddy.end(); iter++)
	{
		CTradeBuddy* pTradeBuddy = *iter;
		if(pTradeBuddy && pTradeBuddy->GetBuddyID() == idBuddy)
		{
			pTradeBuddy->DeleteRecord(Database());
			SAFE_DELETE(pTradeBuddy);
			m_vecTradeBuddy.erase(iter);
			return true;
		}
	}

	return false;
}

CTradeBuddy* CUser::GetTradeBuddy(OBJID idBuddy)
{
	for(vector<CTradeBuddy*>::iterator iter = m_vecTradeBuddy.begin(); iter != m_vecTradeBuddy.end(); iter++)
	{
		CTradeBuddy* pTradeBuddy = *iter;
		if(pTradeBuddy && pTradeBuddy->GetBuddyID() == idBuddy)
			return pTradeBuddy;
	}

	return NULL;
}

int CUser::GetTradeBuddyAmount()
{
	return m_vecTradeBuddy.size();
}
CTradeBuddy* CUser::GetTradeBuddyByIndex(int idx)
{
	CHECKF(0 <= idx && idx < (int)m_vecTradeBuddy.size());
	return m_vecTradeBuddy[idx];
}
bool CUser::CreateAllBusiness(IDatabase* pDb)
{
	SQLBUF		szSQL;
	_snprintf(szSQL, 1024, "SELECT * FROM cq_business WHERE userid = %u LIMIT %d", GetID(), _MAX_TRADE_BUDDY_SIZE);
	IRecordset*	pRes = pDb->CreateNewRecordset(szSQL);
	if(pRes)
	{
		for(int i = 0; i < (int)pRes->RecordCount(); i++)
		{
			CTradeBuddy*	pTradeBuddy = CTradeBuddy::CreateNew();
			if(pTradeBuddy)
			{
				if(pTradeBuddy->Create(pRes))
					m_vecTradeBuddy.push_back(pTradeBuddy);
				else
					SAFE_DELETE(pTradeBuddy);
			}
			pRes->MoveNext();
		}
		pRes->Release();
	}
	return true;
}
void CUser::DeleteAllTradeBuddy()
{
	for(vector<CTradeBuddy*>::iterator iter = m_vecTradeBuddy.begin(); iter != m_vecTradeBuddy.end(); iter++)
		SAFE_DELETE(*iter);
	m_vecTradeBuddy.clear();
}
void CUser::SendTradeBuddyInfo()
{
	CHECK(m_vecTradeBuddy.size() <= _MAX_TRADE_BUDDY_SIZE);

	DWORD	dwNow	= time(NULL);
	for(vector<CTradeBuddy*>::iterator iter = m_vecTradeBuddy.begin(); iter != m_vecTradeBuddy.end(); iter++)
	{
		CTradeBuddy* pTradeBuddy = *iter;
		if(!pTradeBuddy)
			continue;

		int		nMinute	= 0;
		DWORD	dwDate	= pTradeBuddy->GetDate();
		if(dwDate > 0)
		{
			nMinute	= (int)(dwDate - dwNow + 259200) / 60;
			if(nMinute <= 0)
			{
				nMinute	= 0;
				pTradeBuddy->ClearDate();
				SendSysMsg(STR_FORM_TRADE_BUDDY, pTradeBuddy->GetName());
			}
		}

	}

	SQLBUF	szSQL;
	_snprintf(szSQL, 1024, "update cq_business set date=0 where (259200 - (%u-date)) / 60 <= 0 and userid=%u", dwNow, GetID());
	Database()->ExecuteSQL(szSQL);

}

bool CUser::ChangeMapGroup(PROCESS_ID idProcess, OBJID idMap, int nPosX, int nPosY)
{
	DetachLuckyStatus();
	m_dwUnk63C	= ::TimeGet();
	SaveInfo();
#ifdef	PALED_DEBUG
	SendSysMsg("从地图[%s][%d][%d]切屏到[%d][%d]...", GetMap()->GetName(), GetMapID(), m_idProcess-3, idMap, idProcess-3);
#endif
	m_data.m_Info.dwMonsterType	= 0;
	m_data.m_Info.dwMonsterSecs = m_tTransformation.GetRemain();
	if(QueryTransformation() && m_data.m_Info.dwMonsterSecs > 0)
	{
		m_data.m_Info.dwMonsterType	= QueryTransformation()->GetMonsterType();
	}

	MapGroup(PID)->QueryIntraMsg()->SendSocketUserInfo(idProcess, m_idSocket, GetInfo());

	int i;
	for(i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if(pEquip)
		{
			ItemInfoStruct info;
			pEquip->GetInfo(&info);
			MapGroup(PID)->QueryIntraMsg()->SendObjInfo(idProcess, GetID(), INFO_ITEM, &info, sizeof(info));
		}
	}

	for( i = 0; i < (int)m_setItem.size(); i++)
	{
		CItemPtr pItem = m_setItem[i];
		if(pItem)
		{
			ItemInfoStruct info;
			pItem->GetInfo(&info);
			MapGroup(PID)->QueryIntraMsg()->SendObjInfo(idProcess, GetID(), INFO_ITEM, &info, sizeof(info));
		}
	}

	for( i = 0; i < (int)m_setFriend.size(); i++)
	{
		CFriendPtr pFriend = m_setFriend[i];
		if(pFriend)
			MapGroup(PID)->QueryIntraMsg()->SendObjInfo(idProcess, GetID(), INFO_FRIEND, pFriend->GetInfo(), sizeof(FriendInfoStruct));
	}

	{
		CEnemyInfoStruct	info;
		if(QueryEnemy()->GetInfo(&info))
			MapGroup(PID)->QueryIntraMsg()->SendObjInfo(idProcess, GetID(), INFO_ENEMY, &info, sizeof(CEnemyInfoStruct));
	}

	for( i = 0; i < QueryWeapenSkill()->GetAmount(); i++)
	{
		CUserWeaponSkill* pSkill = QueryWeapenSkill()->GetObjByIndex(i);
		if(pSkill)
			MapGroup(PID)->QueryIntraMsg()->SendObjInfo(idProcess, GetID(), INFO_WEAPONSKILL, pSkill->GetInfo(), sizeof(UserWeaponSkillInfoStruct));
	}

	for( i = 0; i < QueryMagic()->QuerySet()->GetAmount(); i++)
	{
		MagicInfoStruct	info;
		if(QueryMagic()->GetInfoByIndex(&info, i))
			MapGroup(PID)->QueryIntraMsg()->SendObjInfo(idProcess, GetID(), INFO_MAGIC, &info, sizeof(info));
	}
	{
		SynAttrInfoStruct info;
		QuerySynAttr()->GetInfo(&info);
		MapGroup(PID)->QueryIntraMsg()->SendObjInfo(idProcess, GetID(), INFO_SYNATTR, &info, sizeof(info));
	}

	for( i = 0; i < QueryStatusSet()->GetAmount(); i++)
	{
		IStatus* pStatus = QueryStatusSet()->GetObjByIndex(i);
		if(pStatus)
		{
			StatusInfoStruct	info;
			if(pStatus->GetInfo(&info))
				MapGroup(PID)->QueryIntraMsg()->SendObjInfo(idProcess, GetID(), INFO_STATUS, &info, sizeof(info));
		}
	}

	if(GetTeam())
	{
		TeamInfoStruct info;
		GetTeam()->GetInfo(&info);
		MapGroup(PID)->QueryIntraMsg()->SendObjInfo(idProcess, GetID(), INFO_TEAM, &info, sizeof(info));

		ClrTeam();
	}

	MapGroup(PID)->QueryIntraMsg()->ChangeMapGroup(idProcess, GetID(), idMap, nPosX, nPosY);

	UserManager()->LogoutUser(GetSocketID(), false);

	{
		extern struct STAT_STRUCT	g_stat;
		InterlockedIncrement(&g_stat.setDebug[3]);
	}

	return true;
}

bool CUser::LeaveMapGroup()
{

	CMsgAction msg;
	if (msg.Create(GetID(), 0,0,0, actionLeaveMap))
		this->BroadcastRoomMsg(&msg, EXCLUDE_SELF);

	DestroyBooth();

	if (m_pTeam)
		this->QuitTeam();
	SaveInfo();

	DeleteAllFriend();
	DeleteAllTradeBuddy();
	DeleteAllItem();

	LeaveMap();

	return true;
}

bool CUser::JoinMap(OBJID idMap, int nPosX, int nPosY)
{
	m_pMap		= MapManager()->GetGameMap(idMap);
	m_nPosX		= nPosX;
	m_nPosY		= nPosY;

	CMsgAction	msg;
	if(msg.Create(GetID(), nPosX, nPosY, GetDir(), 86, m_pMap->GetDocID()))
		SendMsg(&msg);

	EnterMap();

	return true;
}

bool CUser::AppendObjInfo(INFO_ID nType, void* pInfo)
{
	switch(nType)
	{
	case	INFO_ITEM:
		AppendItem((ItemInfoStruct*)pInfo);
		break;
	case	INFO_FRIEND:
		AppendFriend((FriendInfoStruct*)pInfo);
		break;
	case	INFO_ENEMY:
		QueryEnemy()->AppendInfo((CEnemyInfoStruct*)pInfo);
		break;
	case	INFO_WEAPONSKILL:
		AppendWeaponSkill((UserWeaponSkillInfoStruct*)pInfo);
		break;
	case	INFO_MAGIC:
			QueryMagic()->AppendMagic((MagicInfoStruct*)pInfo);
		break;
	case	INFO_SYNATTR:
		QuerySynAttr()->AppendSynAttr((SynAttrInfoStruct*)pInfo);
		break;
	case	INFO_STATUS:
		AppendStatus((StatusInfoStruct*)pInfo);
		break;
	case	INFO_TEAM:
		AppendTeam((TeamInfoStruct*)pInfo);
		break;
	default:
		MYASSERT(!"switch INFO_TYPE");
	}
	return true;
}

bool CUser::AppendItem(ItemInfoStruct* pInfo)
{
	CHECKF(pInfo->idOwner == GetID());

	CItemPtr pItem = CItem::CreateNew();
	IF_NOT(pItem)
		return false;

	IF_NOT(pItem->Create(GameDataDefault()->GetGameItemData(), pInfo, false))
	{
		pItem->ReleaseByOwner();
		return false;
	}

	if(pInfo->nPosition >= ITEMPOSITION_EQUIPBEGIN && pInfo->nPosition < ITEMPOSITION_EQUIPEND)
	{
		CItemPtr& refpEquip = this->GetEquipItemRef(pInfo->nPosition);
		IF_OK(!refpEquip)
		{
			refpEquip = pItem;
			pItem = NULL;
		}
	}

	if(pItem)
		m_setItem.push_back(pItem);

	return true;
}

bool CUser::AppendFriend(FriendInfoStruct* pInfo)
{
	CHECKF(pInfo->idUser == GetID());

	CFriendPtr pFriend;
	pFriend = CFriend::CreateNewFriend(pInfo);
	if(!pFriend)
		return false;

	m_setFriend.push_back(pFriend);

	return true;
}

bool CUser::AppendWeaponSkill(UserWeaponSkillInfoStruct* pInfo)
{
	CHECKF(pInfo->idowner == GetID());

	CUserWeaponSkill*	pSkill	= CUserWeaponSkill::CreateNew(pInfo, GameDataDefault()->GetUserWeaponSkill());
	if (!pSkill)
		return false;

	m_pUserWeaponSkillSet->AddObj(pSkill);

	return true;
}

bool CUser::AppendStatus(StatusInfoStruct* pInfo)
{
	if(pInfo->nTimes > 0)
	{
		CStatusMore* pStatus = CStatusMore::CreateNew();
		if(pStatus)
		{
			if(pStatus->Create(QueryRole(), pInfo->nStatus, pInfo->nPower, pInfo->nSecs, pInfo->nTimes))
				QueryStatusSet()->AddObj(pStatus->QueryInterface());
			else
				pStatus->ReleaseByOwner();
		}
	}
	else
	{
		CStatusOnce* pStatus = CStatusOnce::CreateNew();
		if(pStatus)
		{
			if(pStatus->Create(QueryRole(), pInfo->nStatus, pInfo->nPower, pInfo->nSecs))
				QueryStatusSet()->AddObj(pStatus->QueryInterface());
			else
				pStatus->ReleaseByOwner();
		}
	}

	return true;
}

bool CUser::SpendMoney(DWORD dwMoney, bool bSynchro)
{
	if(dwMoney == 0)
		return true;

	if (this->GetMoney() < dwMoney)
		return false;

	DWORD dwNewMoney = this->GetMoney() - dwMoney;
	m_data.SetMoney(dwNewMoney);

	if(bSynchro)
	{
		CMsgUserAttrib	msg;
		if(msg.Create(this->GetID(), _USERATTRIB_MONEY, dwNewMoney))
			SendMsg(&msg);
	}

	return true;
}

bool CUser::GainMoney(DWORD dwMoney, bool bSynchro )
{
	if(dwMoney == 0)
		return true;

	if(dwMoney < 0)
	{
		LOGWARNING("GainMoney: dwMoney < 0");
	}

	DWORD dwNewMoney = this->GetMoney() + dwMoney;
	if (dwNewMoney > _MAX_MONEYLIMIT)
		return false;

	m_data.SetMoney(dwNewMoney);

	if(bSynchro)
	{
		CMsgUserAttrib	msg;
		if(msg.Create(this->GetID(), _USERATTRIB_MONEY, dwNewMoney))
			SendMsg(&msg);
	}

	return true;
}

bool CUser::SaveMoney(DWORD dwMoney, bool bSynchro )
{
	if (this->GetMoney() < dwMoney)
	{
		return false;
	}

	DWORD dwNewMoney = this->GetMoney() - dwMoney;
	DWORD dwNewMoneySaved = this->GetMoneySaved() + dwMoney;
	if (dwNewMoneySaved > _MAX_MONEYLIMIT)
	{
		return false;
	}

	m_data.SetMoney(dwNewMoney);
	m_data.SetMoneySaved(dwNewMoneySaved);

	if(bSynchro)
	{
		CMsgUserAttrib	msg;
		if(msg.Create(this->GetID(), _USERATTRIB_MONEY, dwNewMoney))
		{
			msg.Append(_USERATTRIB_MONEYSAVED, this->GetMoneySaved());
			SendMsg(&msg);
		}
	}

	return true;
}

bool CUser::DrawMoney(DWORD dwMoney, bool bSynchro )
{
	if (this->GetMoneySaved() < dwMoney)
	{
		return false;
	}

	DWORD dwNewMoney = this->GetMoney() + dwMoney;
	DWORD dwNewMoneySaved = this->GetMoneySaved() - dwMoney;
	if (dwNewMoney > _MAX_MONEYLIMIT)
	{
		return false;
	}

	m_data.SetMoney(dwNewMoney);
	m_data.SetMoneySaved(dwNewMoneySaved);

	if(bSynchro)
	{
		CMsgUserAttrib	msg;
		if(msg.Create(this->GetID(), _USERATTRIB_MONEY, dwNewMoney))
		{
			msg.Append(_USERATTRIB_MONEYSAVED, this->GetMoneySaved());
			SendMsg(&msg);
		}
	}

	return true;
}

bool CUser::AddMoneySaved(int nAdd, bool bSynchro)
{
	int nNewMoneySaved = this->GetMoneySaved() + nAdd;
	if (nNewMoneySaved < 0 || nNewMoneySaved > _MAX_MONEYLIMIT)
		return false;

	m_data.SetMoneySaved(nNewMoneySaved);

	if(bSynchro)
	{
		CMsgUserAttrib	msg;
		if(msg.Create(this->GetID(), _USERATTRIB_MONEYSAVED, this->GetMoneySaved()))
			SendMsg(&msg);
	}

	return true;
}

void CUser::SendShow(IRole* pRole)
{
	CMsgPlayer	msg;
	if(msg.Create((IRole*)this))
		pRole->SendMsg(&msg);
}

void CUser::SetStatus(int nStatus, bool bSynchro)
{
	DWORD dwOldEffect0 = GetEffect(0);
	DWORD dwOldEffect1 = GetEffect(1);
	switch (nStatus)
	{
	case	STATUS_NORMAL:
		{
			m_data.m_Info.dwStatus	= nStatus;
			for(int i = 0; i < 2; i++)
				ResetEffect(0xFFFFFFFF, i);

			if (m_data.GetPk() >= PKVALUE_BLACKNAME)
			{
				m_data.m_Info.dwStatus	|= STATUS_PKVALUE;
				SetEffect(KEEPEFFECT_DEEPRED, 0);
			}
			else if (m_data.GetPk() >= PKVALUE_REDNAME)
			{
				m_data.m_Info.dwStatus	|= STATUS_PKVALUE;
				SetEffect(KEEPEFFECT_RED, 0);
			}

			CTeam* pTeam = GetTeam();
			if(pTeam && pTeam->GetLeader() == GetID())
			{
				m_data.m_Info.dwStatus	|= STATUS_TEAMLEADER;
				SetEffect(KEEPEFFECT_TEAMLEADER, 0);
			}

			if (IsTimeCardOn())
			{
				m_data.m_Info.dwStatus	|= 0x10000000;
				SetEffect(0x00000002, 1);
			}
			if (IsTimeCardStatus())
			{
				m_data.m_Info.dwStatus	|= 0x08000000;
				SetEffect(0x00000001, 1);
			}
		}
		break;
	case	STATUS_DIE:
		{
			m_data.m_Info.dwStatus	= nStatus;
			SetEffect((GetEffect(0) & KEEPEFFECT_NOT_CLEAR) | KEEPEFFECT_DIE, 0);

			if (IsTimeCardOn())
			{
				m_data.m_Info.dwStatus	|= 0x10000000;
				SetEffect(0x00000002, 1);
			}
			if (IsTimeCardStatus())
			{
				m_data.m_Info.dwStatus	|= STATUS_DIE;
				SetEffect(0x00000001, 1);
			}

			ClearAllStatus(0x18000000);
		}
		break;
	default:
		m_data.m_Info.dwStatus	|= nStatus;
		break;
	}

	if (nStatus & 0x08000000)
		SetEffect(0x00000001, 1);
	else if (nStatus & STATUS_PKVALUE)
	{
		if (m_data.GetPk() >= PKVALUE_BLACKNAME)
			SetEffect(KEEPEFFECT_DEEPRED, 0);
		else if (m_data.GetPk() >= PKVALUE_REDNAME)
			SetEffect(KEEPEFFECT_RED, 0);
	}
	else if (nStatus & 0x10000000)
		SetEffect(0x00000002, 1);
	else if (nStatus & STATUS_TEAMLEADER)
		SetEffect(KEEPEFFECT_TEAMLEADER, 0);
	else if (nStatus & STATUS_CRIME)
		SetEffect(KEEPEFFECT_CRIME, 0);
	else if (nStatus & STATUS_SYNCRIME)
		SetEffect(KEEPEFFECT_SYNCRIME, 0);
	else if (nStatus & 0x00000004)
		SetEffect(0x00000002, 0);
	else if (nStatus & STATUS_HITRATE)
		SetEffect(0x00000080, 0);
	else if (nStatus & STATUS_ADJUST_DODGE)
		SetEffect(0x04000000, 0);
	else if (nStatus & STATUS_DEFENCE)
		SetEffect(0x00000100, 0);
	else if (nStatus & STATUS_ATTACK)
		SetEffect(0x00000200, 0);
	else if (nStatus & STATUS_ADJUST_MAGICDEF)
		SetEffect(0x00001000, 0);
	else if (nStatus & STATUS_SUPER_MATK)
		SetEffect(0x00002000, 0);
	else if (nStatus & STATUS_ATKRANGE)
		SetEffect(0x00010000, 0);
	else if (nStatus & 0x00001000)
		SetEffect(0x00020000, 0);
	else if (nStatus & 0x00100000)
		SetEffect(0x02000000, 0);
	else if (nStatus & STATUS_SUPERMAN)
		SetEffect(0x00040000, 0);
	else if (nStatus & STATUS_WEAPONDAMAGE)
		SetEffect(0x00080000, 0);
	else if (nStatus & 0x00008000)
		SetEffect(0x00100000, 0);
	else if (nStatus & STATUS_ATKSPEED)
		SetEffect(0x00200000, 0);
	else if (nStatus & STATUS_LURKER)
		SetEffect(KEEPEFFECT_LURKER, 0);
	else if (nStatus & STATUS_TORNADO)
		SetEffect(0x00800000, 0);
	else if (nStatus & STATUS_WING)
		SetEffect(KEEPEFFECT_WING, 0);
	else if (nStatus & STATUS_KEEPBOW)
		SetEffect(0x10000000, 0);
	else if (nStatus & STATUS_LUCKY_PRAY)
		SetEffect(0x40000000, 0);
	else if (nStatus & STATUS_LUCKY)
		SetEffect(0x80000000, 0);

	if(bSynchro)
	{
		DWORD dwEffect0 = GetEffect(0);
		if(dwOldEffect0 != dwEffect0)
		{
			CMsgUserAttrib msg;
			IF_OK(msg.Create(GetID(), _USERATTRIB_KEEPEFFECT, dwEffect0))
			{
				if(GetMap())
					BroadcastRoomMsg(&msg, INCLUDE_SELF);
				else
					SendMsg(&msg);
			}
		}

		DWORD dwEffect1 = GetEffect(1);
		if(dwOldEffect1 != dwEffect1)
		{
			CMsgUserAttrib msg;
			IF_OK(msg.Create(GetID(), _USERATTRIB_KEEPEFFECT1, dwEffect1))
			{
				if(GetMap())
					BroadcastRoomMsg(&msg, INCLUDE_SELF);
				else
					SendMsg(&msg);
			}
		}
	}
}

void CUser::ClsStatus(int nStatus, bool bSynchro)
{
	DWORD dwOldEffect0 = GetEffect(0);
	DWORD dwOldEffect1 = GetEffect(1);
	if (STATUS_NORMAL == nStatus)
	{
		MYASSERT(!"STATUS_NORMAL");
		return;
	}

	m_data.m_Info.dwStatus	&= ~nStatus;

	if (nStatus & 0x08000000)
		ResetEffect(0x00000001, 1);
	else if (nStatus & STATUS_DIE)
		ResetEffect(KEEPEFFECT_DIE, 0);
	else if (nStatus & STATUS_TEAMLEADER)
		ResetEffect(KEEPEFFECT_TEAMLEADER, 0);
	else if (nStatus & STATUS_CRIME)
		ResetEffect(KEEPEFFECT_CRIME, 0);
	else if (nStatus & STATUS_SYNCRIME)
		ResetEffect(KEEPEFFECT_SYNCRIME, 0);
	else if (nStatus & 0x00000004)
		ResetEffect(0x00000002, 0);
	else if (nStatus & STATUS_PKVALUE)
	{
		if (m_data.GetPk() < PKVALUE_REDNAME)
			ResetEffect(KEEPEFFECT_RED | KEEPEFFECT_DEEPRED, 0);
		else if (m_data.GetPk() < PKVALUE_BLACKNAME)
		{
			ResetEffect(KEEPEFFECT_DEEPRED, 0);
			SetEffect(KEEPEFFECT_RED, 0);
		}
	}
	else if (nStatus & STATUS_HITRATE)
		ResetEffect(0x00000080, 0);
	else if (nStatus & STATUS_ADJUST_DODGE)
		ResetEffect(0x04000000, 0);
	else if (nStatus & STATUS_DEFENCE)
		ResetEffect(0x00000100, 0);
	else if (nStatus & STATUS_ATTACK)
		ResetEffect(0x00000200, 0);
	else if (nStatus & STATUS_ADJUST_MAGICDEF)
		ResetEffect(0x00001000, 0);
	else if (nStatus & STATUS_SUPER_MATK)
		ResetEffect(0x00002000, 0);
	else if (nStatus & STATUS_ATKRANGE)
		ResetEffect(0x00010000, 0);
	else if (nStatus & 0x00001000)
		ResetEffect(0x00020000, 0);
	else if (nStatus & 0x00100000)
		ResetEffect(0x02000000, 0);
	else if (nStatus & STATUS_SUPERMAN)
		ResetEffect(0x00040000, 0);
	else if (nStatus & STATUS_WEAPONDAMAGE)
		ResetEffect(0x00080000, 0);
	else if (nStatus & 0x00008000)
		ResetEffect(0x00100000, 0);
	else if (nStatus & STATUS_ATKSPEED)
		ResetEffect(0x00200000, 0);
	else if (nStatus & STATUS_LURKER)
		ResetEffect(KEEPEFFECT_LURKER, 0);
	else if (nStatus & STATUS_TORNADO)
		ResetEffect(0x00800000, 0);
	else if (nStatus & STATUS_WING)
		ResetEffect(KEEPEFFECT_WING, 0);
	else if (nStatus & STATUS_KEEPBOW)
		ResetEffect(0x10000000, 0);
	else if (nStatus & STATUS_LUCKY_PRAY)
		ResetEffect(0x40000000, 0);
	else if (nStatus & STATUS_LUCKY)
		ResetEffect(0x80000000, 0);

	if(bSynchro)
	{
		DWORD dwEffect0 = GetEffect(0);
		if(dwOldEffect0 != dwEffect0)
		{
			CMsgUserAttrib msg;
			IF_OK(msg.Create(GetID(), _USERATTRIB_KEEPEFFECT, dwEffect0))
				BroadcastRoomMsg(&msg, INCLUDE_SELF);
		}

		DWORD dwEffect1 = GetEffect(1);
		if(dwOldEffect1 != dwEffect1)
		{
			CMsgUserAttrib msg;
			IF_OK(msg.Create(GetID(), _USERATTRIB_KEEPEFFECT1, dwEffect1))
				BroadcastRoomMsg(&msg, INCLUDE_SELF);
		}
	}
}

void CUser::IncAddPoint(int nAddPoint)
{
	int nNewPoint	= nAddPoint + m_data.GetAddPoint();

	if (nNewPoint < 0)
		nNewPoint	= 0;

	m_data.SetAddPoint(nNewPoint);
}

bool CUser::IsAlive()
{
	if ((GetStatus() & STATUS_DIE) == STATUS_DIE || this->GetLife() <= 0)
		return false;

	return true;
}

void CUser::SetForce(DWORD dwForce)
{
	m_data.SetForce(dwForce);
}

void CUser::SetSpeed(DWORD dwSpeed)
{
	m_data.SetSpeed(dwSpeed);
}

void CUser::SetSoul(DWORD dwSoul)
{
	m_data.SetSoul(dwSoul);
}

void CUser::SetHealth(DWORD dwHealth)
{
	m_data.SetHealth(dwHealth);
}

void CUser::SetFace(DWORD dwFace)
{

	DWORD dwLookFace = m_data.GetLookFace();
	dwLookFace = (dwLookFace-GetFace(dwLookFace)*10000)+(dwFace%1000)*10000;

	m_data.SetLookFace(dwLookFace, true);
}

void CUser::SetLook(DWORD dwLook)
{
	DWORD dwLookFace = m_data.GetLookFace();
	dwLookFace = (dwLookFace/1000)*1000 + (dwLook%1000);

	m_data.SetLookFace(dwLookFace, true);
}

bool CUser::IsJumpPass(int x, int y, int nAlt)
{
	if(GetMapID() != 1038)
		return true;

	vector<POINT>	setLine;
	DDALineEx(GetPosX(), GetPosY(), x, y, setLine);

	int nSize = setLine.size();
	if(nSize <= 1)
		return true;
	MYASSERT(x == setLine[nSize-1].x);
	MYASSERT(y == setLine[nSize-1].y);
	float	fAlt	= (float)(GetMap()->GetFloorAlt(GetPosX(), GetPosY()) + nAlt) + 0.5;
	float	fDelta	= (float)(GetMap()->GetFloorAlt(x, y) - fAlt) / nSize;
	for(int i = 0; i < nSize; i++)
	{
		if(GetMap()->IsAltOver(setLine[i], (int)fAlt))
			return false;
	}

	return true;
}
bool CUser::IsArrowPass(int x, int y, int nAlt )
{
	return true;

	vector<POINT>	setLine;
	DDALineEx(GetPosX(), GetPosY(), x, y, setLine);

	int nSize = setLine.size();
	if(nSize <= 2)
		return true;
	ASSERT(x == setLine[nSize-1].x);
	ASSERT(y == setLine[nSize-1].y);
	float	fAlt	= (float)(GetMap()->GetFloorAlt(GetPosX(), GetPosY()) + nAlt) + 0.5;
	float	fDelta	= (float)(GetMap()->GetFloorAlt(x, y) - fAlt) / nSize;
	for(int i = 0; i < nSize; i++)
	{
		if(GetMap()->IsAltOver(setLine[i], (int)fAlt))
			return false;
		fAlt	+= fDelta;
	}

	return true;
}

bool CUser::IsInFan(POINT pos, int nRange, POINT posCenter)
{
	if(posCenter.x == GetPosX() && posCenter.y == GetPosY())
		return false;
	if(pos.x == GetPosX() && pos.y == GetPosY())
		return false;

	if(GetDistance(pos.x, pos.y) > nRange)
		return false;

	float PI = static_cast<float>(3.1415926535);
	float fRadianDelta	= (PI * 240 / 180) / 2;
	float fCenterLine	= GetRadian(GetPosX(), GetPosY(), posCenter.x, posCenter.y);
	float fTargetLine	= GetRadian(GetPosX(), GetPosY(), pos.x, pos.y);
	float fDelta	= fabs(fCenterLine - fTargetLine);
	if(fDelta <= fRadianDelta || fDelta >= 2*PI - fRadianDelta)
		return true;

	return false;
}

bool	CUser::EquipMagicItem(CItem* pItem, int nPosition)
{
	int idxMagic = pItem->GetInt(ITEMDATA_MAGIC1);
	if(idxMagic != MAGIC_NONE && pItem->GetInt(ITEMDATA_AMOUNT))
		MYASSERT(QueryMagic()->EquipWeaponMagic(POSITION2MAGIC_ID(nPosition), MagicType(idxMagic), WEAPONMAGIC_LEVEL));

	return true;
}

bool	CUser::UnequipMagicItem(int nPosition)
{
	OBJID idMagic = POSITION2MAGIC_ID(nPosition);

	QueryMagic()->UnequipWeaponMagic(idMagic);

	return true;
}

void	CUser::SynchroSupermanOrder(int nOrder)
{
	m_nSupermanOrder	= nOrder;

	SetAttrib(_USERATTRIB_SUPERMAP_ORDER, nOrder, SYNCHRO_TRUE);
}

void	CUser::CalcSupermanKills()
{
	if(IsGM() || m_nCurrSupermanKills < MIN_SUPERMAP_KILLS || m_nCurrSupermanKills <= m_nTopSupermanKills)
	{
		m_nCurrSupermanKills	= 0;
		return ;
	}

	m_nTopSupermanKills		= m_nCurrSupermanKills;
	m_nCurrSupermanKills	= 0;

	char	buf[sizeof(long)*2];
	VARIANT_SET0* pInfo = (VARIANT_SET0*)buf;
	pInfo->IntParam(0)	= GetID();
	pInfo->IntParam(1)	= m_nTopSupermanKills;
	MapGroup(PID)->QueryMsgPort()->Send(MSGPORT_WORLD, KERNEL_SUPERMANLIST, STRUCT_TYPE(buf), buf);
}

void	CUser::QuerySupermanSecs()
{
	char	buf[sizeof(long)*2];
	VARIANT_SET0* pInfo = (VARIANT_SET0*)buf;
	pInfo->IntParam(0)	= GetID();
	pInfo->IntParam(1)	= 0;
	MapGroup(PID)->QueryMsgPort()->Send(MSGPORT_WORLD, KERNEL_SUPERMANLIST, STRUCT_TYPE(buf), buf);
}

void	CUser::QuerySupermanList(int nIndex, OBJID idAction, int nNumber)
{
	char	buf[sizeof(long)*4];
	VARIANT_SET0* pInfo = (VARIANT_SET0*)buf;
	pInfo->IntParam(0)	= GetID();
	pInfo->IntParam(1)	= nIndex;
	pInfo->IntParam(2)	= idAction;
	pInfo->IntParam(3)	= nNumber;
	MapGroup(PID)->QueryMsgPort()->Send(MSGPORT_WORLD, KERNEL_QUERYSUPERMANLIST, STRUCT_TYPE(buf), buf);
}

CUser*	CUser::FindAroundUser(OBJID id)
{
	IRole* pRole = FindAroundRole(id);
	CUser* pUser = NULL;
	if(pRole)
		pRole->QueryObj(OBJ_USER, IPP_OF(pUser));

	return pUser;
}

DWORD	CUser::GetInterAtkRate()
{
	if(QueryTransformation())
		return QueryTransformation()->GetInterAtkRate();

	int nRate = GetIntervalAtkRate();
	int nRateR = 0, nRateL = 0;
	if (m_pWeaponR && m_pWeaponR->IsWeapon())
		nRateR = m_pWeaponR->GetInt(ITEMDATA_ATKSPEED);
	if (m_pWeaponL && m_pWeaponL->IsWeapon())
		nRateL = m_pWeaponL->GetInt(ITEMDATA_ATKSPEED);

	if(nRateR && nRateL)
		nRate = (nRateR + nRateL) / 2;
	else if(nRateR)
		nRate = nRateR;
	else if(nRateL)
		nRate = nRateL;

	IStatus* pStatus = QueryStatus(STATUS_ATKSPEED);
	if (pStatus)
	{
		nRate	= ::CutTrail(0, AdjustData(nRate, pStatus->GetPower()));
		if (this->IsPM())
			this->SendSysMsg(_TXTATR_NORMAL, "atack speed+: %d", pStatus->GetPower());
	}

	pStatus = QueryStatus(STATUS_SUPERMAN);
	if (pStatus)
	{
		nRate	= nRate/2;
		if (this->IsPM())
			this->SendSysMsg(_TXTATR_NORMAL, "superman +: %d", pStatus->GetPower());
	}

	pStatus = QueryStatus(STATUS_TORNADO);
	if (pStatus)
	{
		nRate	= ::CutTrail(0, AdjustData(nRate, pStatus->GetPower()));
		if (this->IsPM())
			this->SendSysMsg(_TXTATR_NORMAL, "tornado +: %d", pStatus->GetPower());
	}

	return nRate;
}

DWORD	CUser::GetIntervalAtkRate()
{
	if(QueryTransformation())
		return QueryTransformation()->GetIntervalAtkRate();

	return USER_ATTACK_SPEED;
}

int	CUser::GetAttackRange(int nTargetSizeAdd)
{
	if(QueryTransformation())
		return QueryTransformation()->GetAttackRange(nTargetSizeAdd);

	int nRange = 1;
	int nRangeR = 0, nRangeL = 0;
	if (m_pWeaponR && m_pWeaponR->IsWeapon())
		nRangeR = m_pWeaponR->GetInt(ITEMDATA_ATKRANGE);
	if (m_pWeaponL && m_pWeaponL->IsWeapon())
		nRangeL = m_pWeaponL->GetInt(ITEMDATA_ATKRANGE);

	if(nRangeR && nRangeL)
		nRange = (nRangeR + nRangeL) / 2;
	else if(nRangeR)
		nRange = nRangeR;
	else if(nRangeL)
		nRange = nRangeL;

	nRange += (GetSizeAdd()+nTargetSizeAdd+1)/2;

	IStatus* pStatus = QueryStatus(STATUS_ATKRANGE);
	if (pStatus)
		nRange	= ::CutTrail(1, AdjustData(nRange, pStatus->GetPower()));

	nRange += QueryMagic()->GetExtendAtkRange(m_pWeaponR, m_pWeaponL);
	return nRange;
}

int	CUser::GetSizeAdd()
{
	if(QueryTransformation())
		return QueryTransformation()->GetSizeAdd();

	return 0;
}

int CUser::AdjustHitRate(int nHitRate)
{
	IStatus* pStatus = QueryStatus(STATUS_HITRATE);
	if(pStatus)
	{
		nHitRate = ::CutTrail(0, AdjustData(nHitRate, pStatus->GetPower()));
#ifdef _DEBUG
		this->SendSysMsg(_TXTATR_NORMAL, "hit rate adjust+: %d", pStatus->GetPower());
#endif
	}

	return nHitRate;
}

int CUser::AdjustAttack(int nAtk)
{
	int nAddAtk = 0;

	IStatus* pStatus = QueryStatus(STATUS_ATTACK);
	if(pStatus)
	{
		nAddAtk += ::CutTrail(0, AdjustData(nAtk, pStatus->GetPower())) - nAtk;
#ifdef _DEBUG
		this->SendSysMsg(_TXTATR_NORMAL, "atack adjust STATUS_ATTACK+: %d", pStatus->GetPower());
#endif
	}

	pStatus = QueryStatus(STATUS_KEEPBOW);
	if(pStatus)
	{
		nAddAtk += ::CutTrail(0, AdjustData(nAtk, pStatus->GetPower())) - nAtk;
		QueryStatusSet()->DelObj(STATUS_KEEPBOW);
	}

	return nAtk + nAddAtk;
}

int CUser::AdjustDefence(int nDef)
{
	IStatus* pStatus = QueryStatus(STATUS_DEFENCE);
	if(pStatus)
		nDef = ::CutTrail(0, AdjustData(nDef, pStatus->GetPower()));

	return nDef;
}

int CUser::AdjustBowDefence(int nDef)
{
	IStatus* pStatus = QueryStatus(STATUS_SUPER_MATK);
	if(pStatus)
		return ::CutTrail(0, AdjustData(nDef, pStatus->GetPower()));

	return nDef;
}

int CUser::AdjustWeaponDamage(IRole* pAtker, int nDamage)
{
	IStatus* pStatus = QueryStatus(STATUS_WEAPONDAMAGE);
	if(pStatus)
	{
		nDamage = ::CutTrail(0, AdjustData(nDamage, pStatus->GetPower()));
#ifdef _DEBUG
		this->SendSysMsg(_TXTATR_NORMAL, "adjust STATUS_WEAPONDAMAGE+: %d", pStatus->GetPower());
#endif
	}

	nDamage=MulDiv(nDamage, GetMetempsychosisRate(pAtker), DEFAULT_DEFENCE2);

	if (m_dwUnk638 > 0 && ::RandGet(100) < 2)
	{
		nDamage = 0;
		SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_NO_DMG);

		CMsgName msg;
		IF_OK(msg.Create(NAMEACT_ROLEEFFECT, STR_LUCKY_EFFECT, this->GetID()))
			this->BroadcastRoomMsg(&msg, true);
	}
	return nDamage;
}

int CUser::AdjustMagicDamage(IRole* pAtker, int nDamage)
{
	IStatus* pStatus = QueryStatus(STATUS_MAGICDAMAGE);
	if(pStatus)
	{
		nDamage = ::CutTrail(0, AdjustData(nDamage, pStatus->GetPower()));
#ifdef _DEBUG
		this->SendSysMsg(_TXTATR_NORMAL, "adjust STATUS_MAGICDAMAGE+: %d", pStatus->GetPower());
#endif
	}

	nDamage=MulDiv(nDamage, GetMetempsychosisRate(pAtker), DEFAULT_DEFENCE2);

	if (m_dwUnk638 > 0 && ::RandGet(100) < 2)
	{
		nDamage = 0;
		SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_NO_DMG);

		CMsgName msg;
		IF_OK(msg.Create(NAMEACT_ROLEEFFECT, STR_LUCKY_EFFECT, this->GetID()))
			this->BroadcastRoomMsg(&msg, true);
	}
	return nDamage;
}

int CUser::LuckyCriticalHit(int& nDamage, IRole* pTarget)
{
	if (!pTarget)
		return nDamage;

	if (m_dwUnk638 > 0)
	{
		bool bLucky;
		if (pTarget->IsMonster())
			bLucky = ::RandGet(100) < 3;
		else
			bLucky = ::RandGet(100) < 1;

		if (bLucky)
		{
			nDamage *= 2;
			SendSysMsg(_TXTATR_SYSTEM, STR_LUCK_CRITICAL_HIT);

			CMsgName msg;
			IF_OK(msg.Create(NAMEACT_ROLEEFFECT, STR_LUCKY_EFFECT, this->GetID()))
				this->BroadcastRoomMsg(&msg, true);
		}
	}
	return nDamage;
}

void CUser::SendBonusInfo(void)
{
	char szSQL[256] = "";
	sprintf(szSQL, "SELECT * FROM cq_bonus WHERE id_account=%u", this->GetAccountID());

	IRecordset* pRes = Database()->CreateNewRecordset(szSQL);
	if (!pRes)
		return;

	int nBonus = pRes->RecordCount();
	if (nBonus > 0)
	{
		this->SendSysMsg(_TXTATR_GM, STR_BONUS, nBonus);
	}

	pRes->Release();
}

bool CUser::DoBonus(void)
{
	if (m_setItem.size() >= 40)
	{
		this->SendSysMsg(STR_BAG_FULL);
		return false;
	}

	enum{
		BONUSDATA_ACTION = 0,
		BONUSDATA_ID,
		BONUSDATA_ACCOUNT,
	};

	char szSQL[256] = "";
	sprintf(szSQL, "SELECT * FROM cq_bonus WHERE id_account=%u LIMIT 1", this->GetAccountID());

	CAutoPtr<IRecordset> pRes = Database()->CreateNewRecordset(szSQL);
	if (!pRes || pRes->RecordCount() <= 0)
		return false;

	OBJID idRecord	=pRes->GetInt(BONUSDATA_ID);
	OBJID idAction	=pRes->GetInt(BONUSDATA_ACTION);

	{
		GameAction()->ProcessAction(idAction, this);

		::MyLogSave("gmlog/bonus", "###Bonus: AccountID(%u), Player(%u) take bonus(%u).", this->GetAccountID(), this->GetID(), idAction);

		char szSQL[256] = "";
		sprintf(szSQL, "DELETE from cq_bonus WHERE id=%u LIMIT 1", idRecord);

		MYASSERT(g_db.ExecuteSQL(szSQL));

		return true;
	}
}

bool CRole::AttachStatus(IRole* pRole, int nStatus, int nPower, int nSecs, int nTimes)
{
	CGameMap* pMap = pRole->GetMap();
	IF_NOT (pMap)
		return false;

	if (STATUS_CRIME == nStatus
			&& (pMap->IsPkField() || pMap->IsSynMap()))
		return false;

	IStatus* pStatus = pRole->QueryStatus(nStatus);
	if(pStatus)
	{
		pStatus->ChangeData(nPower, nSecs, nTimes);
		if (nStatus == 0x10000000)
		{
			CMsgUserAttrib	msg;
			msg.Create(pRole->GetID(), 18, pStatus->GetRemain());
			pRole->SendMsg(&msg);
		}
		else if (nStatus == 0x08000000)
		{
			CMsgUserAttrib	msg;
			msg.Create(pRole->GetID(), 21, pStatus->GetRemain());
			pRole->SendMsg(&msg);
		}
		return true;
	}
	else
	{

		if(nTimes)
		{
			CStatusMore*	pNewStatus = CStatusMore::CreateNew();
			IF_OK(pNewStatus)
			{
				pNewStatus->Create(pRole, nStatus, nPower, nSecs, nTimes);
				pRole->QueryStatusSet()->AddObj(pNewStatus->QueryInterface());
				pRole->SetStatus(nStatus, SYNCHRO_TRUE);
				return true;
			}
		}
		else
		{
			CStatusOnce*	pNewStatus = CStatusOnce::CreateNew();
			IF_OK(pNewStatus)
			{
				pNewStatus->Create(pRole, nStatus, nPower, nSecs);
				pRole->QueryStatusSet()->AddObj(pNewStatus->QueryInterface());
				pRole->SetStatus(nStatus, SYNCHRO_TRUE);
				if (nStatus == 0x10000000)
				{
					CMsgUserAttrib	msg;
					msg.Create(pRole->GetID(), 18, nSecs);
					pRole->SendMsg(&msg);
				}
				else if (nStatus == 0x08000000)
				{
					CMsgUserAttrib	msg;
					msg.Create(pRole->GetID(), 21, nSecs);
					pRole->SendMsg(&msg);
				}
				return true;
			}
		}

	}

	return false;
}

void CRole::DetachWellStatus(IRole* pRole)
{
	CHECK(pRole);

	for(int i = pRole->QueryStatusSet()->GetAmount() - 1; i >= 0; i--)
	{
		IStatus* pStatus =  pRole->QueryStatusSet()->GetObjByIndex(i);
		if(pStatus && pStatus->GetID() != STATUS_CRIME)
		{
			if (CRole::IsWellStatus(pStatus->GetID()))
				pRole->QueryStatusSet()->DelObj(pStatus->GetID());
		}
	}
}

void CRole::DetachBadlyStatus(IRole* pRole)
{
	CHECK(pRole);

	for(int i =  pRole->QueryStatusSet()->GetAmount() - 1; i >= 0; i--)
	{
		IStatus* pStatus =  pRole->QueryStatusSet()->GetObjByIndex(i);
		if(pStatus && pStatus->GetID() != STATUS_CRIME)
		{
			if (CRole::IsBadlyStatus(pStatus->GetID()))
				pRole->QueryStatusSet()->DelObj(pStatus->GetID());
		}
	}
}

bool CRole::DetachStatus(IRole* pRole, int nStatus)
{
	CHECKF(pRole);

	for(int i =  pRole->QueryStatusSet()->GetAmount() - 1; i >= 0; i--)
	{
		IStatus* pStatus =  pRole->QueryStatusSet()->GetObjByIndex(i);
		if(pStatus && pStatus->GetID() == nStatus)
		{
			 pRole->QueryStatusSet()->DelObj(pStatus->GetID());
			 return true;
		}
	}

	return false;
}

void CRole::DetachAllStatus(IRole* pRole)
{
	CHECK(pRole);

	for(int i =  pRole->QueryStatusSet()->GetAmount() - 1; i >= 0; i--)
	{
		IStatus* pStatus =  pRole->QueryStatusSet()->GetObjByIndex(i);
		if(pStatus && pStatus->GetID() != STATUS_CRIME)
			 pRole->QueryStatusSet()->DelObj(pStatus->GetID());
	}
}

void CUser::RemoteCall(REMOTE_CALL0* pInfo)
{
	switch(pInfo->ucFuncType)
	{
	case	REMOTE_CALL_TEAM_HUNTKILL:
		{
			this->TeamHuntKill((OBJID)pInfo->IntParam[0], pInfo->IntParam[1], (const char*)pInfo->IntParam[2]);
		}
		break;
	case	REMOTE_CALL_DROP_LEADER:
		{
			CHECK(GetSynRank() == RANK_LEADER);

			QuerySynAttr()->SetIdRank(pInfo->IntParam[0], pInfo->IntParam[1], true);
		}
		break;
	case	REMOTE_CALL_LEAVE_SYN:
		{
			if(GetSynID() == pInfo->IntParam[0])
			{
				QuerySynAttr()->LeaveSyn(DELRECORD_FALSE, SYNCHRO_TRUE);
				SendSysMsg(STR_KICKOUT_ME_s, (char*)&pInfo->IntParam[1]);
			}
		}
		break;
	case	REMOTE_CALL_TEAM_CLOSE_MONEY:
		{
			CTeam* pTeam = GetTeam();
			if(pTeam)
			{
				if(pInfo->IntParam[0])
					pTeam->SetCloseMoney(true);
				else
					pTeam->SetCloseMoney(false);
			}
		}
		break;
	case	REMOTE_CALL_TEAM_CLOSE_ITEM:
		{
			CTeam* pTeam = GetTeam();
			if(pTeam)
			{
				if(pInfo->IntParam[0])
					pTeam->SetCloseItem(true);
				else
					pTeam->SetCloseItem(false);
			}
		}
		break;
	}
}

bool CUser::Transform(DWORD dwType, int nKeepSecs, bool bSynchro )
{
	bool bBack = false;

	if(m_pTransformation)
	{
		m_pTransformation->Release();
		m_pTransformation = NULL;
		bBack = true;
	}

	CNpcType* pType = MonsterType()->GetObj(dwType);
	CHECKF(pType);

	CTransformation* pTransform = CTransformation::CreateNew();
	IF_OK(pTransform)
	{
		IF_OK(pTransform->Create(this, pType))
		{
			m_pTransformation = pTransform;
			m_tTransformation.Startup(nKeepSecs);
			if(bSynchro)
				SynchroTransform();

			this->StopMine();
			return true;
		}
		else
			pTransform->Release();
	}

	if(bBack)
		SynchroTransform();

	return false;
}

bool CUser::TransformLook(DWORD dwType, int nKeepSecs, bool bSynchro)
{
	if(m_pTransformation)
		return false;

	CNpcType* pType = MonsterType()->GetObj(dwType);
	IF_NOT(pType)
		return false;

	m_dwUnk610 = (DWORD)pType->GetInt(NPCTYPEDATA_LOOKFACE) % 1000;
	m_tUnk614.Startup(nKeepSecs);
	if(bSynchro)
		SynchroTransform();

	this->StopMine();
	return true;
}

void CUser::AbortTransform()
{
	SAFE_RELEASE(m_pTransformation); SynchroTransform();
}

void CUser::AbortLook()		{ m_dwUnk610 = -1; SynchroTransform(); }

bool CUser::SynchroTransform()
{
	CMsgUserAttrib	msg;
	IF_OK( msg.Create(GetID(), _USERATTRIB_LOOKFACE, GetLookFace()) && msg.Append(_USERATTRIB_MAXLIFE, GetMaxLife()) && msg.Append(_USERATTRIB_LIFE, GetLife()) )
		BroadcastRoomMsg(&msg, INCLUDE_SELF);

	this->BroadcastTeamLife(true);

	return true;
}

bool CUser::CallPet(OBJID idMonsterType, int x, int y, int nKeepSecs )
{
	if (!m_pMap || !m_pMap->IsStandEnable(x, y))
	{
		SendSysMsg(STR_INVALID_COORDINATE);
		return false;
	}

	KillCallPet(true);

	CMonster* pMonster = NpcManager()->CreateCallPet(this, idMonsterType, x, y);
	if (!pMonster)
	{
		LOGERROR("CUser::CallPet() MonsterType[%u], Map%u(%d, %d) at %s, %d", idMonsterType, GetMapID(), x, y, __FILE__, __LINE__);
		return false;
	}
	m_pCallPet = pMonster->QueryLink();

	if(nKeepSecs)
		m_tCallPet.Startup(nKeepSecs);
	else
		m_tCallPet.Clear();

	if ((idMonsterType >= 9005 && idMonsterType <= 9008) || (idMonsterType >= 9010 && idMonsterType <= 9013))
	{
		if (!(QueryRole()->GetEffect(0) & KEEPEFFECT_INSTEAD))
			SetAttrib(_USERATTRIB_KEEPEFFECT, SetEffect(KEEPEFFECT_INSTEAD, 0), SYNCHRO_TRUE);
	}

	return true;
}

void CUser::KillCallPet(bool bNow )
{
	if(!m_pCallPet)
		return ;

	if(!m_pCallPet->IsDeleted())
		m_pCallPet->DelMonster(bNow);

	if (QueryRole()->GetEffect(0) & KEEPEFFECT_INSTEAD)
		SetAttrib(_USERATTRIB_KEEPEFFECT, (int)ResetEffect(KEEPEFFECT_INSTEAD, 0), SYNCHRO_TRUE);
}

OBJID CUser::GetCallPetID()			{ if(m_pCallPet) return QueryCallPet()->GetID(); return ID_NONE; }

bool CUser::IsSimpleMagicAtk	()
{ return QueryTransformation() && QueryTransformation()->GetMagicType() != ID_NONE; }

int CUser::GetSimpleMagicHitRate()
{ IF_OK(QueryTransformation()) return QueryTransformation()->GetMagicHitRate(); else return 100; }

bool CUser::SetWeather(int nType, int nIntensity, int nDir, DWORD nColor, DWORD nKeepSecs, DWORD nSpeedSecs)
{
	if(m_idWeather != ID_NONE)
	{
		CWeatherRegion* pRegion = GetMap()->QueryWeatherRegion(m_idWeather);
		IF_OK(pRegion)
			return pRegion->QueryWeather()->SetNewWeather(nType, nIntensity, nDir, nColor, nKeepSecs, nSpeedSecs);
	}

	return false;
}

void CUser::SendLight()
{
	CGameMap* pMap = GetMap();
	if (!pMap)
		return;

	CMsgAction	msg;
	IF_OK(msg.Create(GetID(), GetPosX(), GetPosY(), 0, actionMapARGB, pMap->GetLight()))
		SendMsg(&msg);
}

void CUser::AwardSynWarScore(CNpc* pNpc, int nScore)
{
	CHECK(pNpc);
	CHECK(nScore > 0);

	if(GetSynID() == ID_NONE || GetSynID() == pNpc->GetSynID())
		return ;

	int nAddProffer = MulDiv(nScore, SYNWAR_PROFFER_PERCENT, 100);
	if(nAddProffer)
		QuerySynAttr()->AddProffer(nAddProffer);

	int nAddMoney	= MulDiv(nScore, SYNWAR_MONEY_PERCENT, 100);
	if(nAddMoney > 0)
	{
		CSyndicate* pSyn = SynManager()->QuerySyndicate(pNpc->GetSynID());
		if(pSyn)
		{
			nAddMoney	= ::CutOverflow(nAddMoney, pSyn->GetInt(SYNDATA_MONEY));
			pSyn->QueryModify()->AddData(SYNDATA_MONEY, -1*nAddMoney, true);
			this->GainMoney(nAddMoney, SYNCHRO_TRUE);
		}
	}

	CSyndicate* pSyn = SynManager()->QuerySyndicate(GetSynID());
	if(pSyn)
	{
		pSyn = pSyn->GetMasterSyn();
		pSyn->AddSynWarScore(pNpc->GetID(), nScore);

	}
}
bool CUser::IsHostileSynMember	(CUser* pTarget)
{
	IF_NOT (pTarget)
		return false;

	OBJID idSynTarget	= pTarget->GetSynID();
	OBJID idSyn			= this->GetSynID();
	if (ID_NONE != idSynTarget && ID_NONE != idSyn)
	{
		CSyndicate* pSyn = SynManager()->QuerySyndicate(idSyn);
		if (pSyn && pSyn->IsHostile(idSynTarget))
			return true;
	}

	return false;
}

CSyndicate* CUser::GetSyndicate(void)
{
	if(this->GetSynID() != ID_NONE)
		return SynManager()->QuerySyndicate(this->GetSynID());

	return NULL;
}

OBJID CUser::GetMateHomeID()
{
	LPCTSTR szMateName = GetMate();
	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT home_id FROM cq_user WHERE name='%s' LIMIT 1", szMateName);
	CAutoPtr<IRecordset> pRes = _Database(PID)->CreateNewRecordset(szSQL);
	if(pRes)
		return pRes->GetInt(0);

	return ID_NONE;
}

void CUser::SetRecordPos(OBJID idMap, DWORD dwX, DWORD dwY, BOOL bUpdate )
{
	PROCESS_ID idProcess = MapManager()->GetProcessID(idMap);
	CHECK(idProcess != PROCESS_NONE);
	if(idProcess == m_idProcess)
	{
		CGameMap* pMap = MapManager()->GetGameMap(idMap);
		IF_NOT (pMap)
			return;

		if (!pMap->IsStandEnable(dwX, dwY))
		{
			LOGERROR("CUser::SetRecordPos() Map%u(%d, %d) at %s, %d", idMap, dwX, dwX, __FILE__, __LINE__);
			return;
		}
	}

	m_data.SetRecordPos(idMap, dwX, dwY, bUpdate);
}

bool CUser::KickBack()
{
	if(!this->IsSynchro())
		return false;

	CMsgAction	msg;
	IF_OK(msg.Create(GetID(), GetPosX(), GetPosY(), GetDir(), actionKickBack, GetPosX(), GetPosY()))
		SendMsg(&msg);

	this->SetSynchro(false);
	return true;
}

bool CUser::Synchro()
{
	CMsgAction	msg;
	IF_OK(msg.Create(GetID(), GetPosX(), GetPosY(), GetDir(), actionSynchro, GetPosX(), GetPosY()))
		SendMsg(&msg);

	this->SetSynchro(false);
	return true;
}

OBJID CUser::GetMateID()
{
	if(!IsMarried())
		return ID_NONE;

	CUser* pMate = UserManager()->GetUser(this->GetMate());
	if(pMate)
		return pMate->GetID();

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id FROM cq_user where name='%s' LIMIT 1", GetMate());
	CAutoPtr<IRecordset> pRes = _Database(PID)->CreateNewRecordset(szSQL);
	if(pRes)
		return pRes->GetInt(0);

	return ID_NONE;
}

int CUser::AdjustExp(IRole* pTarget, int nRawExp, bool bNewbieBonusMsg)
{
	CHECKF(pTarget);

	int nExp = nRawExp;

	if (this && !pTarget->IsAlive())
	{
		CNpc* pNpc = NULL;
		pTarget->QueryObj(OBJ_NPC, IPP_OF(pNpc));

		if (pTarget->IsMonster() || pNpc && pNpc->IsGoal())
		{
			CTeam* pTeam = this->GetTeam();
			if (pTeam && pTeam->IsTeamWithNewbie(this->GetID(), pTarget)
					&& GetMap()->GetID() != 1039)
			{
				nExp *= 2;

				int nShowExp = CBattleEffect::AdjustExp(this->QueryRole(), pTarget, nExp);
				if (bNewbieBonusMsg && GetMap()->GetID() != 1039)
					this->SendSysMsg(STR_KILLING_EXPEX, nShowExp);
			}
		}
	}

	nExp += nExp * (CBattleEffect::GetBattleExpRate(this->QueryRole(), pTarget) + this->GetGemExpEffect()) / 100;
	return CBattleEffect::AdjustExp(this->QueryRole(), pTarget, nExp);
}

bool CUser::CheckCrime(IRole* pRole)
{
	if (!pRole->IsAlive())
		return false;
	CHECKF(pRole);
	if(!pRole->IsEvil())
	{
		SetCrimeStatus(STATUS_CRIME, 15);
		return true;
	}

	OBJID idSyn = GetMap()->GetSynID();
	if(idSyn != ID_NONE && pRole->GetSynID() == idSyn)
	{
		CNpc* pNpc;
		if(pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsGoal())
			return false;

		SetCrimeStatus(STATUS_SYNCRIME, 15);
		return true;
	}

	return false;
}

bool CUser::IsImmunity(IRole* pRole)
{
	IF_NOT(pRole)
		return false;

	const bool	ATK_IT = false;
	const bool	IMMUNITY = true;

	if (GetMap()->IsDeadIsland())
	{
		CUser* pUser = NULL;
		pRole->QueryObj(OBJ_USER, IPP_OF(pUser));
		if (!pUser)
			pUser = pRole->QueryOwnerUser();
		if (pUser)
		{
			if (this->GetLev() <= 25 || pUser->GetLev() <= 25)
				return IMMUNITY;
		}
	}

	OBJID idRole = pRole->GetID();
	if(IsSceneID(idRole))
	{
		if (GetMap()->GetID() == 1039)
			return false;

		CNpc* pNpc = NULL;
		if(!pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) || !pNpc->IsBeAtkable())
			return IMMUNITY;

		bool bImmunity = IMMUNITY;
		switch(this->GetPkMode())
		{
		case PKMODE_FREE:
		case PKMODE_SAFE:
		case PKMODE_ARRESTMENT:
			bImmunity = ATK_IT;
			break;
		case PKMODE_TEAM:
			bImmunity = (this->GetSynID() != ID_NONE && pNpc->GetSynID() == this->GetSynID());
			break;
		}
		return bImmunity;
	}
	else if(IsNpcID(idRole) || (idRole >= CALLPETID_FIRST && idRole <= CALLPETID_LAST))
	{
		if (GetMap()->GetID() == 1039)
			return IMMUNITY;

		CMonster* pMonster = NULL;
		IF_NOT(pRole->QueryObj(OBJ_MONSTER, IPP_OF(pMonster)))
			return IMMUNITY;
		if(pMonster->GetMasterID() == GetID())
			return IMMUNITY;

		bool bImmunity = IMMUNITY;
		switch(this->GetPkMode())
		{
		case PKMODE_FREE:
			bImmunity = ATK_IT;
			break;
		case PKMODE_TEAM:
			bImmunity = ATK_IT;
			if (pMonster->IsCallPet())
			{
				CTeam* pTeam = GetTeam();
				if(pTeam && pTeam->IsTeamMember(pMonster->GetMasterID()))
					bImmunity = IMMUNITY;
			}
			break;
		case PKMODE_ARRESTMENT:
			if(pMonster->IsMonster() || pMonster->IsEvil())
				bImmunity = ATK_IT;
			break;
		case PKMODE_SAFE:
			if(pMonster->IsMonster() || (pMonster->IsSynPet() && pMonster->GetOwnerLink() == NULL))
				bImmunity = ATK_IT;
			break;
		}
		return bImmunity;
	}
	else
	{
		if (GetMap()->GetID() == 1039)
			return IMMUNITY;

		switch(this->GetPkMode())
		{
		case	PKMODE_FREE:
			return false;
			break;

		case	PKMODE_TEAM:
			{
				bool bImmunity = false;

				CTeam* pTeam = this->GetTeam();
				if(pTeam && pTeam->IsTeamMember(idRole))
					bImmunity = true;

				if (this->GetFriend(pRole->GetID()))
					bImmunity = true;

				CUser* pUser = NULL;
				if (pRole->QueryObj(OBJ_USER, IPP_OF(pUser)) && this->GetSynID() != ID_NONE)
				{
					if (this->GetSynID() == pUser->GetSynID())
						bImmunity = true;

					CSyndicate* pSyn = SynManager()->QuerySyndicate(this->GetSynID());
					if (pSyn)
					{
						if (pSyn->IsNearSyn(pUser->GetSynID())
							|| pSyn->IsFriendly(pUser->GetSynID(), true))

							bImmunity = true;
					}

					if (this->IsMate(pUser))
						bImmunity = true;
				}

				return bImmunity;
			}
			break;

		case	PKMODE_ARRESTMENT:
			{
				if(pRole->IsEvil())
					return false;
				return true;
			}
			break;

		case	PKMODE_SAFE:
			return true;
			break;

		default:
			MYASSERT(!"switch pk mode");
			break;
		}

		return false;
	}
}

void CUser::SetCrimeStatus(int nStatus , int nTime )
{
	MYASSERT(nStatus == STATUS_CRIME || nStatus == STATUS_SYNCRIME);

	if (!IsAlive())
		return;

	int nPower = 1;
	CRole::AttachStatus(QueryRole(), nStatus, nPower, nTime);
	if(QueryCallPet())
		CRole::AttachStatus(QueryCallPet()->QueryRole(), nStatus, nPower, nTime);
}
bool CUser::IsBeAtkable			()
{
	if (m_tLock.IsActive() && !m_tLock.IsTimeOut())
		return false;
	if (m_pCallPet && m_pCallPet->IsInsteadMagic()
			&& abs(GetPosX() - m_pCallPet->GetPosX()) < 12 && abs(GetPosY() - m_pCallPet->GetPosY()) < 12)
		return false;
	return true;
}

bool CUser::CheckRebirth(int nProf, CRebirthData* pData)
{
	if(this->GetMetempsychosis() >= pData->GetInt(REBIRTHDATA_METEMPSYCHOSIS))
		return false;
	if((int)GetLev() < pData->GetInt(REBIRTHDATA_NEED_LEVEL))
		return false;

	return true;
}

bool CUser::Rebirth(int nProf, int nLook, int nMetempsychosis)
{
	for(int i = 0; i < RebirthSet()->GetAmount(); i++)
	{
		CRebirthData* pData = RebirthSet()->GetObjByIndex(i);
		if(pData && GetProfession() == pData->GetInt(REBIRTHDATA_NEED_PROF) && nProf == pData->GetInt(REBIRTHDATA_NEW_PROF)
				&& nMetempsychosis == pData->GetInt(REBIRTHDATA_METEMPSYCHOSIS))
		{
			if(CheckRebirth(nProf, pData))
			{
				int nOldProf	= GetProfession();
				int nOldLevel	= GetLev();
				__int64	i64OldExp	= GetExp();
				ResetEquipLevel(pData->GetInt(REBIRTHDATA_NEW_LEVEL));
				ResetUserAttrib(nOldProf, nOldLevel, pData->GetInt(REBIRTHDATA_NEW_PROF), pData->GetInt(REBIRTHDATA_NEW_LEVEL), nLook, nMetempsychosis);

				this->UnEquipItem(ITEMPOSITION_WEAPONL, SYNCHRO_TRUE);
				int nMeteLev = nOldLevel * 100000;
				CLevupexpData* pLevupexp = LevupexpSet()->GetObj(nOldLevel);
				if (pLevupexp)
				{
					__int64	i64LevupExp	= pLevupexp->GetInt64(LEVUPEXPDATA_EXP);
					nMeteLev += (int)((double)i64OldExp * 100000 / i64LevupExp);
				}
				if (nMeteLev > (int)m_data.m_Info.dwMeteLev)
				{
					m_data.SetMeteLev(nMeteLev, true);
					CMsgUserAttrib	msg;
					if (msg.Create(GetID(), _USERATTRIB_METELEV, nMeteLev))
						this->SendMsg(&msg);
				}
				UpdateBattlePower();
				return true;
			}
		}
	}

	return false;
}

void CUser::ResetEquipLevel(int nNewLevel)
{
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = this->GetEquipItemByPos(i);
		if(pEquip)
		{
			DownEquipmentLevel(i, nNewLevel);
		}
	}
}

void CUser::ResetUserAttrib(int nOldProf, int nOldLevel, int nNewProf, int nNewLevel, int nNewLook, int nMetempsychosis)
{
	int nOldForce	= m_data.GetForce();
	int nOldSpeed	= m_data.GetSpeed();
	int nOldHealth	= m_data.GetHealth();
	int nOldSoul	= m_data.GetSoul();

	int nProfession = nNewProf;
	if (nProfession == 0)
		nProfession = 20;

	int nForce = 5, nHealth = 3, nDex = 2, nSoul = 0;
	if (nProfession >= 100)
	{
		nForce	= 0;
		nDex	= 2;
		nHealth = 3;
		nSoul	= 5;
	}
	else if (nProfession/10 == 4)
	{
		nForce	= 2;
		nDex	= 7;
		nHealth = 1;
		nSoul	= 0;
	}

	int nMaxLife = (nSoul + nHealth*8 + nDex + nForce) * 3;
	int nMaxMana = nSoul*5;
	int nAddPoint	= GetRebirthAddPoint(nOldProf, nOldLevel, nNewLevel, nMetempsychosis);

	DEBUG_TRY
	this->SetLook(nNewLook);
	this->SetAutoAllot(false);
	m_data.SetOldProf(nOldProf, true);
	SetAttrib(_USERATTRIB_LOOKFACE, GetLookFace(), SYNCHRO_BROADCAST);
	SetAttrib(_USERATTRIB_PORFESSION, nNewProf, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_LEV, nNewLevel, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_EXP, 0, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_FORCE, nForce, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_SPEED, nDex, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_HEALTH, nHealth, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_SOUL, nSoul, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_LIFE, nMaxLife, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_MANA, nMaxMana, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_ADDPOINT, nAddPoint, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_METEMPSYCHOSIS, nMetempsychosis, SYNCHRO_TRUE);
	DEBUG_CATCH("CUser::ResetUserAttrib")

	SaveInfo();
	::MyLogSave("gmlog/rebirth", "%s[%u] rebirth to %d metepsychosis from lev(%d), pro(%d), stength(%d), speed(%d), health(%d), soul(%d) to lev(%d), pro(%d), force(%d), speed(%d), health(%d), soul(%d)",
			GetName(), GetID(), nMetempsychosis, nOldLevel, nOldProf, nOldForce, nOldSpeed, nOldHealth, nOldSoul,
			nNewLevel, nNewProf, m_data.GetForce(), m_data.GetSpeed(), m_data.GetHealth(), m_data.GetSoul());
}

int CUser::GetRebirthAddPoint(int nOldProf, int nOldLevel, int nNewLevel, int nMetempsychosis)
{
	if (nMetempsychosis == 1)
	{
		int nLevel = nOldLevel;
		if (nLevel > 130)
			nLevel = 130;
		if(nOldProf == HIGHEST_WATER_WIZARD_PROF)
		{
			return 72 + (1 + (nLevel-110)/2 ) * ((nLevel-110)/2) / 2;
		}
		else
		{
			return 72 + (1 + (nLevel-120) ) * (nLevel-120) / 2;
		}
	}

	int nPoint = m_data.GetAddPoint() + m_data.GetSoul() + m_data.GetHealth() + m_data.GetSpeed() + m_data.GetForce()
					- nOldLevel*3 - 7;
	if (nPoint < 0)
		nPoint = 0;

	const int	setBonus[11] = { 0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55 };
	int nIndex = nOldLevel - 120;
	if (nIndex > 10)
		nIndex = 10;
	else if (nIndex < 0)
		nIndex = 0;
	return setBonus[nIndex] + nNewLevel*3 + nPoint - 3;
}

void CUser::ResetUserPoint()
{
	CHECK(this->GetMetempsychosis());

	int nForce = 5, nDex = 2, nHealth = 3, nSoul = 0;
	if (this->GetProfessionSort() >= 10)
	{
		nForce	= 0;
		nDex	= 2;
		nHealth = 3;
		nSoul	= 5;
	}
	else if (this->GetProfessionSort() == 4)
	{
		nForce	= 2;
		nDex	= 7;
		nHealth = 1;
		nSoul	= 0;
	}

	int nAddPoint = this->GetAddPoint() + m_data.GetForce() + m_data.GetSpeed() + m_data.GetHealth() + m_data.GetSoul() - 10;

	DEBUG_TRY
	SetAttrib(_USERATTRIB_FORCE, nForce, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_SPEED, nDex, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_HEALTH, nHealth, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_SOUL, nSoul, SYNCHRO_TRUE);
	SetAttrib(_USERATTRIB_ADDPOINT, nAddPoint, SYNCHRO_TRUE);
	DEBUG_CATCH("CUser::ResetUserPoint()")
	SaveInfo();
}
bool CUser::CheckReflect()
{
	if (GetMetempsychosis() && m_data.GetOldProf() == PROFSORT_HIGHEST_SOLDIER && (this->GetProfessionSort() == PROFSORT_SOLDIER || GetMetempsychosis() == 2))
	{
		if (GetLev() >= 15 && GetLev() < 40)	return ::RandGet(100) < 5;
		if (GetLev() >= 40 && GetLev() < 70)	return ::RandGet(100) < 10;
		if (GetLev() >= 70 && GetLev() < 100)	return ::RandGet(100) < 15;
		if (GetLev() >= 100)					return ::RandGet(100) < 20;
	}
	return false;
}
int CUser::GetReflectLimit()
{
	IF_OK(GetMetempsychosis() && m_data.GetOldProf() == PROFSORT_HIGHEST_SOLDIER && (this->GetProfessionSort() == PROFSORT_SOLDIER || GetMetempsychosis() == 2))
	{
		if (GetLev() >= 15 && GetLev() < 40)	return 300;
		if (GetLev() >= 40 && GetLev() < 70)	return 700;
		if (GetLev() >= 70 && GetLev() < 100)	return 1200;
		if (GetLev() >= 100)					return 1700;
	}
	return 0;
}

void CUser::SendSystemBBS(LPCTSTR szText)
{
	OBJID idUser = this->GetID();

	CMsgTalk	msg;
	IF_OK(msg.Create(this->GetName(), ALLUSERS_NAME, szText, NULL, 0xffffff, _TXTATR_MSG_SYSTEM))
		MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(&msg, idUser);
}

bool CUser::SetLock(DWORD dwKey)
{
	if(m_data.GetRealLockKey())
		return false;
	if(dwKey <= 99999)
		return false;
	if(dwKey == 123456 || dwKey == 654321)
		return false;
	if(dwKey % 111111 == 0)
		return false;

	m_data.SetLockKey(dwKey);
	return true;
}

bool CUser::OpenLock(DWORD dwKey)
{
	if(!m_data.GetRealLockKey() || m_data.GetRealLockKey() != dwKey)
		return false;

	m_data.SetCurrLockKey(0, UPDATE_FALSE);
	return true;
}

bool CUser::ClearLock(DWORD dwKey)
{
	if(m_data.GetRealLockKey() && m_data.GetRealLockKey() != dwKey)
		return false;

	m_data.SetLockKey(0, UPDATE_FALSE);
	return true;
}

bool CUser::CheckLock(bool bSynchro )
{
	if(m_data.GetCurrLockKey())
	{
		if(bSynchro)
			SendSysMsg(STR_LOCKKEY_DENY);
		return true;
	}

	return false;
}

bool CUser::SetAcceptData(DWORD dwKey)
{
	m_data.SetLockKey(dwKey);
	return true;
}

DWORD CUser::GetAcceptKey()
{
	return m_data.GetRealLockKey();
}

int CUser::GetEquipMagicAtkAdd()
{
	int nAdd = 0;
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if(pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
			nAdd += pEquip->GetAddition(7);
	}
	return nAdd;
}

int CUser::GetEquipMagicDefAdd()
{
	int nAdd = 0;
	for(int i = ITEMPOSITION_EQUIPBEGIN; i < ITEMPOSITION_EQUIPEND; i++)
	{
		CItem* pEquip = GetEquipItemByPos(i);
		if(pEquip && pEquip->GetInt(ITEMDATA_AMOUNT))
			nAdd += pEquip->GetAddition(8);
	}
	return nAdd;
}

void CUser::UpdateWeight()
{
	m_bUpdateWeight = true;
}

bool CUser::Court	(OBJID idTarget)
{
	if (this->IsMarried())
		return false;

	CUser* pTargetUser = this->FindAroundUser(idTarget);
	if (!pTargetUser)
		return false;

	if (this->GetSex() == pTargetUser->GetSex())
		return false;

	this->SetApply(CUser::APPLY_MARRY, idTarget);

	int nDeltaBattleEffect = 0;
	IRole* pRole = RoleManager()->QueryRole(idTarget);
	if (pRole)
	{
		CUser* pUser = pRole->QueryOwnerUser();
		if (pUser)
			nDeltaBattleEffect = this->GetBattlePower() - pUser->GetBattlePower();
	}

	CMsgInteract msg;
	IF_OK (msg.Create(INTERACT_COURT, this->GetID(), idTarget, 0, 0, 0, nDeltaBattleEffect))
		pTargetUser->SendMsg(&msg);

	return true;
}

bool CUser::Marry	(OBJID idTarget)
{
	if (this->IsMarried())
		return false;

	CUser* pTargetUser = this->FindAroundUser(idTarget);
	if (!pTargetUser)
		return false;

	if (pTargetUser->FetchApply(CUser::APPLY_MARRY) != this->GetID())
		return false;

	if (pTargetUser->IsMarried())
		return false;

	this->UpdateMate(pTargetUser->GetName(), true);
	pTargetUser->UpdateMate(this->GetName(), true);

	MSGBUF szMsg;
	sprintf(szMsg, STR_MARRY, this->GetName(), pTargetUser->GetName());

	CMsgTalk msg;
	IF_OK_ (msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szMsg, NULL, 0xff0000, _TXTATR_GM))
		UserManager()->BroadcastMsg(&msg);

	{
		CMsgItem msg;
		IF_OK (msg.Create(this->GetID(), ITEMACT_FIREWORKS))
			this->BroadcastRoomMsg(&msg, true);
	}

	::MyLogSave("gmlog/marriage", "%s married with %s", this->GetName(), pTargetUser->GetName());
	return true;
}

bool CUser::Divorce	(void)
{
	if (!this->IsMarried())
		return false;

	CUser* pMate = UserManager()->GetUser(this->GetMate());

	IDatabase* pDb = Database();
	IF_OK (pDb)
	{
		char szSQL[1024] = "";
		sprintf(szSQL, "SELECT id FROM cq_user WHERE name='%s'", this->GetMate());
		char szMateID[1024] = "";
		if (pDb->GetFieldData(szSQL, szMateID, 1024))
		{
			OBJID idMate = atol(szMateID);
			_snprintf(szSQL, 1024, "INSERT cq_item SET type=%u,owner_id=%u,player_id=%u,amount=1,amount_limit=1,position=%d",
							TYPE_DIVOICEITEM, idMate, idMate, ITEMPOSITION_BACKPACK);
			IF_OK (pDb->ExecuteSQL(szSQL))
			{
				OBJID idItem = pDb->GetInsertId();
				DWORD dwChkSum = CItemData::CalcChkSum(idItem, idMate, TYPE_DIVOICEITEM, 0, 0, 0, 0, 0, 0, 0, 0);
				sprintf(szSQL, "UPDATE cq_item SET chk_sum=%u WHERE id=%u LIMIT 1", dwChkSum, idItem);
				pDb->ExecuteAsyncSQL(szSQL);
			}
		}
	}

	MSGBUF szMsg;
	sprintf(szMsg, STR_DIVORCE, this->GetName(), this->GetMate());
	this->BroadcastRoomMsg(szMsg, true);

	if (pMate)
		pMate->UpdateMate(NOMATE_NAME, true);
	else
	{
		char szSQL[1024] = "";
		sprintf(szSQL, "UPDATE cq_user SET mate='%s' WHERE name='%s' LIMIT 1", NOMATE_NAME, this->GetMate());
		pDb->ExecuteSQL(szSQL);
	}

	this->UpdateMate(NOMATE_NAME, true);
	return true;
}

void CUser::UpdateMate	(const char* pszName, bool bUpdate )
{
	IF_NOT (pszName && strlen(pszName) < _MAX_NAMESIZE)
		return;

	m_data.SetMate(pszName, true);
	if (bUpdate)
	{
		CMsgName msg;
		IF_OK (msg.Create(NAMEACT_MATE, pszName, this->GetID()))
			this->SendMsg(&msg);
	}
}

bool CUser::IsMarried(void)
{
	const char* pszMate = this->GetMate();
	return (0 != strcmp(pszMate, NOMATE_NAME));
}

bool CUser::IsMate(CUser* pUser)
{
	IF_NOT (pUser)
		return false;

	return (0 == strcmp(this->GetMate(), pUser->GetName()));
}

bool CUser::DropItem3(OBJID idMap, int x, int y)
{
	this->ClrAttackTarget();
	this->SetLife(0, true);
	this->SetStatus(STATUS_DIE, true);
	m_tAutoReborn.Startup(20);
	m_bGhost = false;
	this->SetRecordPos(idMap, x, y);
	this->KillCallPet();

	for(int i = m_setItem.size()-1; i >= 0; i--)
	{
		CItem* pItem = m_setItem[i];
		if (pItem && pItem->IsAlwaysDropWhenDead())
		{
			POINT	pos;
			pos.x	= GetPosX();
			pos.y	= GetPosY();
			if (GetMap()->FindDropItemCell(USERDROPITEM_RANGE, &pos))
			{
				::MyLogSave("gmlog/drop_item3", "%s drop item(%u).", this->GetName(), pItem->GetID());
				this->DropItem(pItem->GetID(), pos.x, pos.y);
			}
		}
	}
	return true;
}

__int64 CUser::GetVarData(int idx)
{
	IF_NOT (idx >= 0 && idx <= MAX_ACTION_VAR)
		return 0;
	return _atoi64(m_szVarStr[idx].c_str());
}

void CUser::SetVarData(int idx, __int64 i64Data)
{
	IF_NOT (idx >= 0 && idx <= MAX_ACTION_VAR)
		return;

	char szBuf[1024] = "";
	sprintf(szBuf, "%I64d", i64Data);
	m_szVarStr[idx] = szBuf;
}

LPCTSTR CUser::GetVarStr(int idx)
{
	IF_NOT (idx >= 0 && idx <= MAX_ACTION_VAR)
		return NULL;
	return m_szVarStr[idx].c_str();
}

void CUser::SetVarStr(int idx, LPCTSTR pszStr)
{
	IF_NOT (idx >= 0 && idx <= MAX_ACTION_VAR)
		return;

	m_szVarStr[idx] = pszStr;
}

void CUser::SaveEmoneyLog(int nType, OBJID idSource, OBJID idTarget, int nNumber)
{
	DWORD	dwTime		= ::TimeGetMinute();
	DWORD	dwChkSum	= (nType*3 + idSource + idTarget + nNumber*2) ^ dwTime;
	CUser*	pSource		= UserManager()->GetUser(idSource);
	CUser*	pTarget		= UserManager()->GetUser(idTarget);
	int		nSourceBalance	= 0;
	int		nTargetBalance	= 0;
	if (pSource)
		nSourceBalance	= pSource->m_data.m_Info.dwEmoney;
	if (pTarget)
		nTargetBalance	= pTarget->m_data.m_Info.dwEmoney;

	char szSQL[1024];
	_snprintf(szSQL, 1024, "INSERT INTO %s SET type=%u, id_source=%u, id_target=%u, number=%d, time_stamp=%u, chk_sum=%u, source_balance=%d, target_balance=%d",
					"e_money", nType, idSource, idTarget, nNumber, dwTime, dwChkSum, nSourceBalance, nTargetBalance);
	Database()->ExecuteSQL(szSQL);
}

int CUser::GetEmoneyCard1()
{
	char szSQL[1024];
	sprintf(szSQL, "SELECT id FROM %s WHERE account_id=%u && used=0 && chk_sum=%u+(ref_id>>1)", "emoney_card", GetAccountID(), GetAccountID() ^ 0xBE6E0);
	return __max(Database()->CountRecord(szSQL), 0);
}

int CUser::GetEmoneyCard2()
{
	char szSQL[1024];
	sprintf(szSQL, "SELECT id FROM %s WHERE account_id=%u && used=0 && chk_sum=%u+(ref_id>>1)", "emoney_card2", GetAccountID(), GetAccountID() ^ 0xBE6E1);
	return __max(Database()->CountRecord(szSQL), 0);
}

int CUser::GetDragonType(int nType)
{
	LPCTSTR	pszTable	= "cq_db";
	if (nType == 2)
		pszTable	= "cq_db2";

	char szSQL[1024];
	_snprintf(szSQL, 1024, "SELECT id FROM %s WHERE account_id = %u AND used = %u AND chk_sum = %u + (ref_id >> 1)", pszTable, GetAccountID(), 0, GetAccountID() ^ 0x109A00);
	int nAmount = Database()->CountRecord(szSQL);
	if (nAmount < 0)
		return 0;
	return nAmount;
}

void CUser::UpdateBattlePower()
{
	bool bTwoHand = false;
	CItem* pWeaponR = GetEquipItemByPos(ITEMPOSITION_WEAPONR);
	if (pWeaponR && (CItem::GetItemSort(pWeaponR->GetInt(ITEMDATA_TYPE)) == ITEMSORT_WEAPON2 || CItem::IsMSword(pWeaponR->GetInt(ITEMDATA_TYPE))))
		bTwoHand = true;

	int nEquipPower = 0;
	for (int i = 1; i < 10; i++)
	{
		if (i == 7)
			continue;
		if (i == ITEMPOSITION_WEAPONL && bTwoHand)
			continue;

		CItem* pEquip = GetEquipItemByPos(i);
		if (!pEquip || !pEquip->GetInt(ITEMDATA_AMOUNT))
			continue;

		int nPower = __max(0, pEquip->GetInt(ITEMDATA_TYPE)%10 - 5);
		if (pEquip->GetInt(ITEMDATA_GEM1))
			nPower++;
		if (pEquip->GetInt(ITEMDATA_GEM2))
			nPower++;
		if (pEquip->GetInt(ITEMDATA_GEM1)%10 == 3)
			nPower++;
		if (pEquip->GetInt(ITEMDATA_GEM2)%10 == 3)
			nPower++;
		nPower += pEquip->GetInt(ITEMDATA_MAGIC3);
		if (CItem::GetItemSort(pEquip->GetInt(ITEMDATA_TYPE)) == ITEMSORT_WEAPON2 || CItem::IsMSword(pEquip->GetInt(ITEMDATA_TYPE)))
			nPower *= 2;
		if (pEquip->IsArrowSort())
			nPower = 0;
		nEquipPower += nPower;
	}

	m_dwUnk8D8	= GetLev() + nEquipPower + GetMetempsychosis()*5;
	m_dwUnk8D8	+= GetNobilityRank();
}

int CUser::GetEquipBattlePower(CItem* pItem)
{
	if (pItem && pItem->IsEquipment() && !pItem->IsArrowSort()
			&& pItem->GetInt(ITEMDATA_POSITION) != 7 && pItem->GetInt(ITEMDATA_POSITION) != 7)
	{
		int nPower = __max(0, pItem->GetInt(ITEMDATA_TYPE)%10 - 5);
		if (pItem->GetInt(ITEMDATA_GEM1))
			nPower++;
		if (pItem->GetInt(ITEMDATA_GEM2))
			nPower++;
		if (pItem->GetInt(ITEMDATA_GEM1)%10 == 3)
			nPower++;
		if (pItem->GetInt(ITEMDATA_GEM2)%10 == 3)
			nPower++;
		return nPower + pItem->GetInt(ITEMDATA_MAGIC3);
	}
	return 0;
}

void CUser::ProcessOnlineTraining()
{
	if (m_nUnk8D0 == 2 || m_nUnk8D0 == 1)
		return;

	if (this->GetLev() >= (DWORD)g_nMaxLevel)
		return;

	if (m_nUnk8D0 == 0)
	{
		m_dwUnk8CC	+= 10;
		CMsgUserAttrib	msg;
		msg.Create(this->GetID(), 32, 3);
		this->SendMsg(&msg);
	}

	if ((int)m_dwUnk8CC < 100)
		return;

	m_dwUnk8CC	= 0;
	CMsgUserAttrib	msg;
	msg.Create(this->GetID(), 32, 4);
	this->SendMsg(&msg);
	this->AwardOnlineTrainingExp();
}

void CUser::AwardOnlineTrainingExp()
{
	if (this->IsIncomeLimited() && !this->GetIncomePercent())
		return;

	int nLev = this->GetLev();
	if (nLev >= g_nMaxLevel)
		return;

	CLevupexpData* pLevupexp = LevupexpSet()->GetObj(nLev);
	MYASSERT(pLevupexp);
	__int64 i64LevExp = pLevupexp->GetInt64(LEVUPEXPDATA_EXP);
	if (i64LevExp <= 0)
		return;

	double dTrainTime	= (double)this->GetExp() * pLevupexp->GetInt(LEVUPEXPDATA_UP_LEV_TIME) / __max(i64LevExp, 1);
	double dAddTime		= 12.5;
	if (this->IsIncomeLimited())
		dAddTime	= this->GetIncomePercent() * 0.125;
	double dTotal		= dAddTime + dTrainTime;
	this->AddUnk8E8((int)dAddTime);

	int nAddLev = 0;
	for (int i = nLev; i < g_nMaxLevel; i++)
	{
		pLevupexp = LevupexpSet()->GetObj(i);
		MYASSERT(pLevupexp);
		int nUpLevTime = pLevupexp->GetInt(LEVUPEXPDATA_UP_LEV_TIME);
		if (nUpLevTime <= 0)
			break;
		if (dTotal < nUpLevTime)
			break;
		dTotal	-= nUpLevTime;
		nAddLev++;
	}

	__int64	i64NextExp	= 0;
	int		nUpLevTime	= 0;
	pLevupexp = LevupexpSet()->GetObj(nLev + nAddLev);
	if (pLevupexp)
	{
		i64NextExp	= pLevupexp->GetInt64(LEVUPEXPDATA_EXP);
		nUpLevTime	= pLevupexp->GetInt(LEVUPEXPDATA_UP_LEV_TIME);
	}
	__int64 i64Exp = (__int64)(i64NextExp * dTotal / __max(nUpLevTime, 1));

	bool bSatori = false;
	if (nAddLev > 0)
	{
		this->IncLev(nAddLev, &bSatori);
		if (this->IsAutoAllot())
			this->AllotPoint();
		else
			this->AddAttrib(_USERATTRIB_ADDPOINT, nAddLev*3, SYNCHRO_TRUE);
		this->SetAttrib(_USERATTRIB_LIFE, this->GetMaxLife(), SYNCHRO_TRUE);
		if (this->GetTeam())
			this->BroadcastTeamLife(true);
	}

	if (!bSatori || this->GetExp() < i64Exp)
		this->SetAttrib(_USERATTRIB_EXP, i64Exp, SYNCHRO_TRUE);
}

void CUser::ProcessCheckBattleEffect()
{
	int nPower = this->GetBattlePowerBase();
	if (m_nUnk8D4 < 0)
	{
		m_nUnk8D4	= nPower;
		return;
	}
	if (m_nUnk8D4 == nPower)
		return;

	m_nUnk8D4	= nPower;
}

bool CUser::IsBusinessman()
{
	return this->GetBusinessman() == 255;
}

bool CUser::IsIncomeLimited()
{
	switch(MapGroup(PID)->GetIncomeLimitMode())
	{
	case	1:
		return m_dwUnk1C != 0;
	case	2:
		return true;
	}
	return false;
}

void CUser::ProcessFeeInfo(int nType, DWORD dwData, int nTime)
{
	if (this->IsIncomeLimited() && nType != 107)
	{
		switch(nType)
		{
		case	106:
			{
				m_nUnk24	= 100;
				CMsgUserAttrib	msg;
				if (msg.Create(this->GetID(), 36, 0))
					this->SendMsg(&msg);
			}
			break;
		case	104:
			m_nUnk24	= 50;
			break;
		case	105:
			m_nUnk24	= 0;
			break;
		}

		CMsgUserAttrib	msg;
		if (msg.Create(this->GetID(), 33, m_nUnk24))
			this->SendMsg(&msg);
		this->SynchroFeeData(dwData);
		this->SynchroFeeTime(nTime);
	}

	if (m_bUnk28)
	{
		m_bUnk28	= false;
	}
}

void CUser::SynchroFeeTime(int nTime)
{
	CMsgUserAttrib	msg;
	if (msg.Create(this->GetID(), 35, nTime))
		this->SendMsg(&msg);
}

int CUser::GetHelmetColor()
{
	if (m_pHelmet)
		return m_pHelmet->GetInt(ITEMDATA_COLOR);
	return 0;
}

int CUser::GetArmorColor()
{
	if (m_pArmor)
		return m_pArmor->GetInt(ITEMDATA_COLOR);
	return 0;
}

int CUser::GetShieldColor()
{
	if (m_pWeaponL && CItem::GetItemSort(m_pWeaponL->GetInt(ITEMDATA_TYPE)) == ITEMSORT_SHIELD)
		return m_pWeaponL->GetInt(ITEMDATA_COLOR);
	return 0;
}

void CUser::SynchroBusinessman(int nDate)
{
	CMsgUserAttrib	msg;
	if (msg.Create(this->GetID(), 39, nDate))
		this->SendMsg(&msg);
}

void CUser::SynchroFeeData(DWORD dwData)
{
	CMsgUserAttrib	msg;
	if (msg.Create(this->GetID(), 34, dwData))
		this->SendMsg(&msg);
}

void CUser::OnTimeCardMapOut()
{
	m_nUnk8D0	= 0;
	CMsgUserAttrib	msg;
	msg.Create(this->GetID(), 32, 0);
	this->SendMsg(&msg);
}

void CUser::OnTimeCardMapIn()
{
	m_nUnk8D0	= 1;
	CMsgUserAttrib	msg;
	msg.Create(this->GetID(), 32, 1);
	this->SendMsg(&msg);
}

void CUser::OnTimeCardMapLeave()
{
	m_nUnk8D0	= 0;
	CMsgUserAttrib	msg;
	msg.Create(this->GetID(), 32, 2);
	this->SendMsg(&msg);
}

void CUser::OnTimeCardEnd()
{
	m_dwUnk8CC	= 0;	m_nUnk8D0	= 2;
	CMsgUserAttrib	msg;
	msg.Create(this->GetID(), 32, 5);
	this->SendMsg(&msg);
}

int CUser::GetBusinessmanDays()
{
	int nDate	= this->GetBusinessman();
	int nToday	= ::TimeGet(TIME_DAY) + 19000000;
	if (nDate <= 0 || nDate == 255)
		return 0;

	for (int i = 1; i <= 5; i++)
	{
		if (::DateStampPass(nToday, i) == nDate)
			return i;
	}
	return 0;
}

int CUser::GetBusinessman()
{
	return m_data.GetBusiness();
}

void CUser::SetBusinessman(int nDate)
{
	m_data.SetBusiness(nDate);
}

void CUser::BecomeBusinessman()
{
	if (GetMap()->IsNewbieMap())
	{
		CMsgInteract	msg;
		this->SetBusinessman(255);
		this->SynchroBusinessman(255);
		if (msg.Create(33, GetID(), 0, 0, 0, 0, 255))
		{
			this->SendMsg(&msg);
		}
		::MyLogSave("gmlog/businessman", "UserID:%u\tName:%s\tBusinessState:%u\t", GetID(), GetName(), 255);
		return;
	}

	int nDate = ::DateStampPass(::TimeGet(TIME_DAY) + 19000000, 5);
	this->SetBusinessman(nDate);
	this->SynchroBusinessman(nDate);
}

UCHAR CUser::GetNobilityRank()
{
	return 0;
}

bool CUser::CheckActionCount(DWORD dwCount)
{
	if (dwCount == m_dwUnk6AC)
		return true;

	this->SetCheatMark(7, 2);
	this->SetCheatKickout(::RandGet(60) + 30);
	return false;
}
