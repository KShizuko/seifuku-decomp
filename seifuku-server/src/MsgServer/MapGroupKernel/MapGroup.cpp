#include "MessagePort.h"
#pragma warning(disable:4786)
#include "AllMsg.h"
#include "MapGroup.h"
#pragma warning(disable:4786)
#include "protocol.h"
#include "itemtype.h"
#include "MapManager.h"
#include "ActionDefine.h"
#include "WantedList.h"
#include "DeadLoop.h"
#include "BattleEffect.h"
#include "GMToolCfg.h"
#include "ItemAddition.h"
#include "Booth.h"
#include "FuseItem.h"
#include "Item.h"
DEADLOOP_SET g_setDeadLoop;	CMyCriticalSection CDeadLoop::s_xDump;	CTimeOut CDeadLoop::s_tDump;	int CDeadLoop::s_nDumpCount = 0;	int CDeadLoop::s_nDumpLeft = 0;	CMyCriticalSection CDeadLoop::s_xCtrl;

CMapGroup::MAPGROUP_SET		CMapGroup::m_setMapGroup;
struct DETECT_DATA { DWORD dwData; float fAnswer[3]; };
std::vector<DETECT_DATA>	g_setDetectData;
CItemType*		g_pItemType		= NULL;
IStaticMapSet*	g_setStaticMap	= NULL;
ITaskSet*		g_setTask		= NULL;
IActionSet*		g_setAction		= NULL;
ILevupexpSet*	g_setLevupexp	= NULL;
IMagicTypeSet*	g_setMagicType	= NULL;
IMagicTypeSet*	g_setAutoMagicType	= NULL;
IAddPointSet*	g_setAddPoint	= NULL;
ITrapTypeSet*	g_setTrapType	= NULL;
IMonsterTypeSet*	g_setMonsterType	= NULL;
IRebirthSet*	g_setRebirth	= NULL;
CItemAddition*	g_pItemAddition	= NULL;
int				g_nMaxLevel		= 130;

void LoadDetectData()
{
	char	szFile[]	= "verify.dat";
	FILE*	fp = fopen(szFile, "rb");
	if(!fp)
	{
		::LogSave("Cannot open verify data file [%s]", szFile);
		return;
	}

	int nRead = 0;
	do{
		DETECT_DATA	info;
		nRead = fread(&info, sizeof(info), 1, fp);
		if(nRead == 1)
			g_setDetectData.push_back(info);
	}while(nRead >= 1);
	fclose(fp);
}

void CreateDetectData(DWORD* pData, DWORD* pKey, DWORD* pAnswer)
{
	*pData = 0;		*pKey = 0;		*pAnswer = 0;
	int nAmount = g_setDetectData.size();
	if(nAmount > 0)
	{
		int idx = ::RandGet(nAmount);
		*pData		= g_setDetectData[idx].dwData;
		*pKey		= ::RandGet(3);
		*pAnswer	= (int)g_setDetectData[idx].fAnswer[*pKey];
	}
}
CMapGroup::CMapGroup()
{
	m_idProcess		= NULL;
	m_pSocket		= NULL;
	m_pDb			= NULL;
	m_pMsgPort		= NULL;

	m_pRoleManager	= NULL;
	m_pUserManager	= NULL;
	m_pMapManager	= NULL;
	m_pNpcManager	= NULL;
	m_pSynManager	= NULL;
	m_pLeaveWord	= NULL;
	m_pGameAction	= NULL;
	m_pAdapter		= NULL;
	m_dwReserved7C	= 1;
}

bool CMapGroup::Create(PROCESS_ID idProcess, ISocket* pSocket, IDatabase* pDb, IMessagePort* pMsgPort)
{
	m_idProcess	= idProcess;
	m_pSocket	= pSocket;
	m_pDb		= pDb;
	m_pMsgPort	= pMsgPort;

	if(!(
			(m_pGameItemDataDefault			= Database()->CreateNewRecordset("SELECT * FROM " _TBL_ITEM " WHERE id=0 LIMIT 1", false))
			&& (m_pUserDataDefault			= Database()->CreateNewRecordset("SELECT * FROM " _TBL_USER " WHERE id=0 LIMIT 1", false))
			&& (m_pGameMapDataDefault		= Database()->CreateNewRecordset("SELECT * FROM " _TBL_MAP " WHERE id=0 LIMIT 1", false))
			&& (m_pUserWeaponSkillDefault	= Database()->CreateNewRecordset("SELECT * FROM " _TBL_WEAPONSKILL " WHERE id=0 LIMIT 1", false))
			&& (m_pMagicDataDefault			= Database()->CreateNewRecordset("SELECT * FROM " _TBL_MAGIC " WHERE id=0 LIMIT 1", false))
			&& (m_pSynDataDefault			= Database()->CreateNewRecordset("SELECT * FROM " _TBL_SYNDICATE " WHERE id=0 LIMIT 1", false))
			&& (m_pSynAttrDataDefault		= Database()->CreateNewRecordset("SELECT * FROM " _TBL_SYNATTR " WHERE id=0 LIMIT 1", false))
			&& (m_pPetDataDefault			= Database()->CreateNewRecordset("SELECT * FROM " _TBL_PET " WHERE id=0 LIMIT 1", false))
			&& (m_pDynaNpcDataDefault		= Database()->CreateNewRecordset("SELECT * FROM " _TBL_DYNANPC " WHERE id=0 LIMIT 1", false))
			&& (m_pDynaMapDataDefault		= Database()->CreateNewRecordset("SELECT * FROM " _TBL_DYNAMAP " WHERE id=0 LIMIT 1", false))
			&& (m_pUserStatisticDataDefault	= Database()->CreateNewRecordset("SELECT * FROM " _TBL_PREHEAD "statistic WHERE id=0 LIMIT 1", false))
		))
		return false;

	if(m_idProcess == MSGPORT_MAPGROUP_FIRST)
	{
		g_pItemType = CItemType::CreateNew();
		IF_NOT(g_pItemType && g_pItemType->Create(pDb))
			return false;

		g_setStaticMap = CStaticMapSet::CreateNew(true);
		bool ret = g_setStaticMap && g_setStaticMap->Create("SELECT * FROM " _TBL_MAP, pDb);
		IF_NOT_(ret)
			return false;

		g_setTask	= CTaskSet::CreateNew(true);
		ret = g_setTask && g_setTask->Create("SELECT * FROM " _TBL_TASK, Database());
		IF_NOT_(ret)
			return false;

		g_setAction	= CActionSet::CreateNew(true);
		ret = g_setAction && g_setAction->Create("SELECT * FROM " _TBL_ACTION, Database());
		IF_NOT_(ret)
			return false;

		g_setLevupexp	= CLevupexpSet::CreateNew(true);
		ret = g_setLevupexp && g_setLevupexp->Create("SELECT * FROM " _TBL_LEVEXP, Database());
		IF_NOT_(ret)
			return false;

		g_nMaxLevel = g_setLevupexp->GetAmount() + 1;

		g_setAddPoint	= CAddPointSet::CreateNew(true);
		ret = g_setAddPoint && g_setAddPoint->Create("SELECT * FROM " _TBL_POINTALLOT, Database());
		IF_NOT_(ret)
			return false;

		g_setMagicType	= CMagicTypeSet::CreateNew(true);
		ret = g_setMagicType && g_setMagicType->Create("SELECT * FROM " _TBL_MAGICTYPE, Database());
		IF_NOT_(ret)
			return false;

		g_setAutoMagicType	= CMagicTypeSet::CreateNew(true);
		ret = g_setAutoMagicType && g_setAutoMagicType->Create("SELECT * FROM " _TBL_MAGICTYPE " WHERE auto_learn!=0", Database());
		IF_NOT_(ret)
			return false;

		IF_NOT(CWantedList::s_WantedList.Create(Database()))
			return false;

		g_setTrapType	= CTrapTypeSet::CreateNew(true);
		ret = g_setTrapType && g_setTrapType->Create("SELECT * FROM " _TBL_TRAPTYPE, Database());
		IF_NOT_(ret)
			return false;

		g_setMonsterType	= CMonsterTypeSet::CreateNew(true);
		ret = g_setMonsterType && g_setMonsterType->Create("SELECT * FROM " _TBL_MONSTERTYPE, Database());
		IF_NOT_(ret)
			return false;

		g_setRebirth	= CRebirthSet::CreateNew(true);
		ret = g_setRebirth && g_setRebirth->Create("SELECT * FROM " _TBL_REBIRTH, Database());
		IF_NOT_(ret)
			return false;

		IF_NOT_(CBattleEffect::Init(Database()))
			return false;

		g_pGMToolCfg = CGMToolCfg::CreateNew(Database());
		IF_NOT(g_pGMToolCfg)
			return false;

		this->LoadIncomeLimit();
		this->LoadConfig();
		this->LoadItemScore();

		g_pItemAddition	= CItemAddition::CreateNew();
		IF_NOT(g_pItemAddition && g_pItemAddition->Create(pDb))
			return false;

		::LoadDetectData();

		IF_NOT(CBooth::InitSysBuyInfo(pDb))
			return false;

		g_pFuseItem = new CFuseItem;
		IF_NOT(g_pFuseItem && g_pFuseItem->Create(pDb))
			return false;
	}

	CMapManager* pMapManager = new CMapManager(m_idProcess);
	if (!pMapManager || !pMapManager->Create())
	{
		ASSERT(!"pMapManager->Create");
		return false;
	}
	m_pMapManager = pMapManager->GetInterface();

	CRoleManager* pRoleManager = new CRoleManager(m_idProcess);
	if (!pRoleManager || !pRoleManager->Create())
	{
		ASSERT(!"pRoleManager->Create");
		return false;
	}
	m_pRoleManager = pRoleManager->GetInterface();

	CUserManager* pUserManager = new CUserManager(m_idProcess);
	if (!pUserManager || !pUserManager->Create())
	{
		ASSERT(!"pUserManager->Create");
		return false;
	}
	m_pUserManager = pUserManager->GetInterface();

	m_pSynManager = new CSynManager(m_idProcess);
	if (!m_pSynManager || !m_pSynManager->Create())
	{
		ASSERT(!"m_pSynManager->Create");
		return false;
	}

	CNpcManager* pNpcManager = new CNpcManager(m_idProcess);
	if (!pNpcManager || !pNpcManager->Create())
	{
		ASSERT(!"pNpcManager->Create");
		return false;
	}
	m_pNpcManager = pNpcManager->GetInterface();

	m_pLeaveWord = new CLeaveWord();
	if (!m_pLeaveWord || !m_pLeaveWord->Create(m_idProcess))
	{
		ASSERT(!"m_pLeaveWord->Create");
		return false;
	}

	m_pGameAction = CGameAction::CreateNew(m_idProcess);
	if (!m_pGameAction)
	{
		ASSERT(!"m_pGameAction->Create");
		return false;
	}

	if(m_idProcess == MSGPORT_MAPGROUP_FIRST)
	{
		m_pAdapter = ::AdapterCreate(m_idProcess);
		if(!m_pAdapter)
		{
			ASSERT(!"AdapterCreate");
			return false;
		}
	}

	const int INTERVAL_EVENT = 60;
	m_tmEvent.Startup(INTERVAL_EVENT);
	m_tmHour.Startup(3600);
	return true;
}

void CMapGroup::Destroy()
{
	if(m_pGameAction)	m_pGameAction->Release();
	if(m_pLeaveWord)	m_pLeaveWord->Release();
	if(m_pNpcManager)	m_pNpcManager->Release();
	if(m_pSynManager)	m_pSynManager->Release();
	if(m_pUserManager)	m_pUserManager->Release();
	if(m_pRoleManager)	m_pRoleManager->Release();
	if(m_pMapManager)	m_pMapManager->Release();
	if(m_pAdapter)		{ ::operator delete(m_pAdapter); m_pAdapter = NULL; }

	m_pGameAction	= NULL;
	m_pLeaveWord	= NULL;
	m_pSynManager	= NULL;
	m_pRoleManager	= NULL;
	m_pUserManager	= NULL;
	m_pMapManager	= NULL;
	m_pNpcManager	= NULL;

	if(m_idProcess == MSGPORT_MAPGROUP_FIRST)
	{
		CWantedList::s_WantedList.Release();
		CBattleEffect::Destroy();

		if(g_setRebirth)		g_setRebirth->Release();
		if(g_setMonsterType)	g_setMonsterType->Release();
		if(g_setTrapType)		g_setTrapType->Release();
		if(g_setAutoMagicType)	g_setAutoMagicType->Release();
		if(g_setMagicType)		g_setMagicType->Release();
		if(g_setAddPoint)		g_setAddPoint->Release();
		if(g_setLevupexp)		g_setLevupexp->Release();
		if(g_setAction)			g_setAction->Release();
		if(g_setTask)			g_setTask->Release();
		if(g_setStaticMap)		g_setStaticMap->Release();
		if(g_pItemType)			g_pItemType->Release();
		if(g_pItemAddition)		g_pItemAddition->Release();
		if(g_pGMToolCfg)		g_pGMToolCfg->Release();
		SAFE_DELETE(g_pFuseItem);
	}
	if(m_pUserStatisticDataDefault)	m_pUserStatisticDataDefault->Release();
	if(m_pDynaMapDataDefault)		m_pDynaMapDataDefault->Release();
	if(m_pDynaNpcDataDefault)		m_pDynaNpcDataDefault->Release();
	if(m_pPetDataDefault)			m_pPetDataDefault->Release();
	if(m_pSynAttrDataDefault)		m_pSynAttrDataDefault->Release();
	if(m_pSynDataDefault)			m_pSynDataDefault->Release();
	if(m_pMagicDataDefault)			m_pMagicDataDefault->Release();
	if(m_pUserWeaponSkillDefault)	m_pUserWeaponSkillDefault->Release();
	if(m_pGameItemDataDefault)		m_pGameItemDataDefault->Release();
	if(m_pUserDataDefault)			m_pUserDataDefault->Release();
	if(m_pGameMapDataDefault)		m_pGameMapDataDefault->Release();

	m_setConfig.clear();
}

void CMapGroup::OnTimer(time_t tCurr)
{
	DEBUG_TRY
	DEADLOOP_CHECK(PID, "RoleManager()->OnTimer")
	RoleManager()->OnTimer(tCurr);
	DEBUG_CATCH("RoleManager()->OnTimer(tCurr);")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "MapManager()->OnTimer")
	MapManager()->OnTimer(tCurr);
	DEBUG_CATCH("MapManager()->OnTimer(tCurr);")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "UserManager()->OnTimer")
	UserManager()->OnTimer(tCurr);
	DEBUG_CATCH("UserManager()->OnTimer(tCurr);")

	DEBUG_TRY
	DEADLOOP_CHECK(PID, "LeaveWord()->OnTimer")
	LeaveWord()->OnTimer(tCurr);
	DEBUG_CATCH("LeaveWord()->OnTimer(tCurr);")
	DEBUG_TRY
	DEADLOOP_CHECK(PID, "ProcessEvent")
	clock_t	tStart = timeGetTime();

	if (m_tmEvent.ToNextTime())
		this->ProcessEvent();

	EVENT_SET::iterator it = m_setEvent.begin();
	while(it != m_setEvent.end())
	{
		EVENT_SET::iterator iter = it++;
		if(iter->tmEvent.TimeOver())
		{
			GetGameAction()->ProcessAction(iter->idAction);
			m_setEvent.erase(iter);
		}
	}
	clock_t tUsed = timeGetTime()-tStart;
	if(tUsed > 1000)
		ASSERT(!"EVENT定时任务超时!");
	DEBUG_CATCH("ProcessEvent();")
}

BOOL CMapGroup::CheckTime(const DWORD dwType, const char* pszParam)
{
	if (!pszParam)
		return false;

	time_t ltime;
	time( &ltime );

	struct tm *pTime;
	pTime = localtime( &ltime );

	int nCurYear, nCurMonth, nCurDay, nCurWeekDay, nCurYearDay, nCurHour, nCurMinute;
	nCurYear	=pTime->tm_year+1900;
	nCurMonth	=pTime->tm_mon+1;
	nCurDay		=pTime->tm_mday;
	nCurWeekDay	=pTime->tm_wday;
	nCurYearDay	=pTime->tm_yday;
	nCurHour	=pTime->tm_hour;
	nCurMinute	=pTime->tm_min;

	int nYear0=0, nMonth0=0, nDay0=0, nHour0=0, nMinute0=0;
	int nYear1=0, nMonth1=0, nDay1=0, nHour1=0, nMinute1=0;

	if (dwType == 0)
	{
		if (10 == sscanf(pszParam, "%d-%d-%d %d:%d %d-%d-%d %d:%d",
			&nYear0, &nMonth0, &nDay0, &nHour0, &nMinute0,
			&nYear1, &nMonth1, &nDay1, &nHour1, &nMinute1))
		{
			struct tm time0, time1;
			memset(&time0, 0L, sizeof(tm));
			memset(&time1, 0L, sizeof(tm));

			time0.tm_year	=nYear0-1900;
			time0.tm_mon	=nMonth0-1;
			time0.tm_mday	=nDay0;
			time0.tm_hour	=nHour0;
			time0.tm_min	=nMinute0;
			time0.tm_isdst	=pTime->tm_isdst;

			time1.tm_year	=nYear1-1900;
			time1.tm_mon	=nMonth1-1;
			time1.tm_mday	=nDay1;
			time1.tm_hour	=nHour1;
			time1.tm_min	=nMinute1;
			time1.tm_sec	=59;
			time1.tm_isdst	=pTime->tm_isdst;

			time_t t0	=mktime(&time0);
			time_t t1	=mktime(&time1);

			if (ltime >= t0 && ltime<= t1)
				return true;
		}
	}
	else if (dwType == 1)
	{
		if (8 == sscanf(pszParam, "%d-%d %d:%d %d-%d %d:%d",
			&nMonth0, &nDay0, &nHour0, &nMinute0,
			&nMonth1, &nDay1, &nHour1, &nMinute1))
		{
			struct tm time0, time1;
			memset(&time0, 0L, sizeof(tm));
			memset(&time1, 0L, sizeof(tm));

			time0.tm_year	=pTime->tm_year;
			time0.tm_mon	=nMonth0-1;
			time0.tm_mday	=nDay0;
			time0.tm_hour	=nHour0;
			time0.tm_min	=nMinute0;
			time0.tm_isdst	=pTime->tm_isdst;

			time1.tm_year	=pTime->tm_year;
			time1.tm_mon	=nMonth1-1;
			time1.tm_mday	=nDay1;
			time1.tm_hour	=nHour1;
			time1.tm_min	=nMinute1;
			time1.tm_sec	=59;
			time1.tm_isdst	=pTime->tm_isdst;

			time_t t0	=mktime(&time0);
			time_t t1	=mktime(&time1);

			if (ltime >= t0 && ltime<= t1)
				return true;
		}
	}
	else if (dwType == 2)
	{
		if (6 == sscanf(pszParam, "%d %d:%d %d %d:%d",
			&nDay0, &nHour0, &nMinute0,
			&nDay1, &nHour1, &nMinute1))
		{
			struct tm time0, time1;
			memset(&time0, 0L, sizeof(tm));
			memset(&time1, 0L, sizeof(tm));

			time0.tm_year	=pTime->tm_year;
			time0.tm_mon	=pTime->tm_mon;
			time0.tm_mday	=nDay0;
			time0.tm_hour	=nHour0;
			time0.tm_min	=nMinute0;
			time0.tm_isdst	=pTime->tm_isdst;

			time1.tm_year	=pTime->tm_year;
			time1.tm_mon	=pTime->tm_mon;
			time1.tm_mday	=nDay1;
			time1.tm_hour	=nHour1;
			time1.tm_min	=nMinute1;
			time1.tm_sec	=59;
			time1.tm_isdst	=pTime->tm_isdst;

			time_t t0	=mktime(&time0);
			time_t t1	=mktime(&time1);

			if (ltime >= t0 && ltime<= t1)
				return true;
		}
	}
	else if (dwType == 3)
	{
		if (6 == sscanf(pszParam, "%d %d:%d %d %d:%d",
			&nDay0, &nHour0, &nMinute0,
			&nDay1, &nHour1, &nMinute1))
		{
			int timeNow	=nCurWeekDay*24*60+nCurHour*60+nCurMinute;
			if (timeNow >= nDay0*24*60+nHour0*60+nMinute0 &&
					timeNow <= nDay1*24*60+nHour1*60+nMinute1)
			{
				return true;
			}
		}
	}
	else if (dwType == 4)
	{
		if (4 == sscanf(pszParam, "%d:%d %d:%d",
			&nHour0, &nMinute0,
			&nHour1, &nMinute1))
		{
			int timeNow	=nCurHour*60+nCurMinute;
			if (timeNow >= nHour0*60+nMinute0 &&
					timeNow <= nHour1*60+nMinute1)
			{
				return true;
			}
		}
	}
	else if (dwType == 5)
	{
		if (2 == sscanf(pszParam, "%d %d",
			&nMinute0,
			&nMinute1))
		{
			if (nCurMinute >= nMinute0 && nCurMinute <= nMinute1)
			{
				return true;
			}
		}
	}

	return false;
}

void CMapGroup::ProcessEvent(void)
{
	CONST OBJID ID_EVENT_BEGIN	= 2000000;
	CONST DWORD MAX_EVENT		= 100;

	OBJID idBegin = ID_EVENT_BEGIN+m_idProcess*10000;
	for (OBJID id=idBegin; id<idBegin+MAX_EVENT; id++)
	{
		if (ActionSet()->GetObj(id))
			GameAction()->ProcessAction(id);
	}
}

bool CMapGroup::SendClientMsg(SOCKET_ID idSocket, CNetMsg* pMsg)
{
	pMsg->AppendInfo(m_idProcess, idSocket, ID_NONE);
	return m_pSocket->SendMsg(pMsg);
}

bool CMapGroup::SendNpcMsg(OBJID idNpc, CNetMsg* pMsg)
{
	pMsg->AppendInfo(m_idProcess, SOCKET_NONE, idNpc);
	return m_pSocket->SendMsg(pMsg);
}

bool CMapGroup::SendBroadcastMsg(CNetMsg* pMsg)
{
	for(int i = MSGPORT_MAPGROUP_FIRST; i < m_pMsgPort->GetSize(); i++)
	{
		if(i != m_idProcess)
		{
			MESSAGESTR	buf;
			CLIENTMSG_PACKET0* pPacket = (CLIENTMSG_PACKET0*)buf;
			pPacket->idSocket	= pMsg->GetSocketID();
			pPacket->idPacket	= pMsg->GetType();
			memcpy(pPacket->buf, pMsg->GetBuf(), pMsg->GetSize());
			pPacket->nSize		= pMsg->GetSize();
			m_pMsgPort->Send(i, MAPGROUP_BROADCASTMSG, STRUCT_TYPE(buf), pPacket);
		}
	}
	return true;
}

bool CMapGroup::TransmitMsg(CNetMsg* pMsg, SOCKET_ID idSocket, OBJID idNpc)
{
	MYASSERT(idSocket != SOCKET_NONE || idNpc != ID_NONE);

	MESSAGESTR	buf;
	TRANSMSG_PACKET0* pPacket = (TRANSMSG_PACKET0*)buf;
	pPacket->idSocket	= idSocket;
	pPacket->idNpc		= idNpc;
	pPacket->idPacket	= pMsg->GetType();
	memcpy(pPacket->buf, pMsg->GetBuf(), pMsg->GetSize());
	pPacket->nSize		= pMsg->GetSize();
	pPacket->nTrans		= pMsg->GetTransData();
	for(int i = MSGPORT_MAPGROUP_FIRST; i < m_pMsgPort->GetSize(); i++)
	{
		if(i != m_idProcess)
			m_pMsgPort->Send(i, MAPGROUP_TRANSMITMSG, STRUCT_TYPE(buf), pPacket);
	}
	return true;
}

void CMapGroup::RemoteCall(REMOTE_CALL0* pInfo)
{
	for(int i = MSGPORT_MAPGROUP_FIRST; i < m_pMsgPort->GetSize(); i++)
	{
		if (i != m_idProcess)
			m_pMsgPort->Send(i, MAPGROUP_REMOTECALL, BUFFER_TYPE(pInfo->nSize), pInfo);
	}
}

bool CMapGroup::TransmitWorldTalk(LPCTSTR szText)
{
	if(!szText)
		return false;

	if(m_pMsgPort)
		m_pMsgPort->Send(MSGPORT_WORLD, WORLD_SHELLTALK, STRING_TYPE(szText), szText);
	return true;
}

bool CMapGroup::TransmitWorldMsg(CNetMsg* pMsg)
{
	MESSAGESTR	buf;
	TRANSMSG_PACKET0* pPacket = (TRANSMSG_PACKET0*)buf;
	pPacket->idSocket	= pMsg->GetSocketID();
	pPacket->idNpc		= pMsg->GetNpcID();
	pPacket->idPacket	= pMsg->GetType();
	memcpy(pPacket->buf, pMsg->GetBuf(), pMsg->GetSize());
	pPacket->nSize		= pMsg->GetSize();
	pPacket->nTrans		= pMsg->GetTransData();

	m_pMsgPort->Send(MSGPORT_WORLD, WORLD_TRANSMITMSG, STRUCT_TYPE(buf), pPacket);
	return true;
}

bool CMapGroup::TransmitWorldMsg(CNetMsg* pMsg, LPCTSTR szName)
{
	MESSAGESTR	buf;
	USERNAMEMSG_PACKET0* pPacket = (USERNAMEMSG_PACKET0*)buf;
	pPacket->idSocket	= pMsg->GetSocketID();
	pPacket->idPacket	= pMsg->GetType();
	memcpy(pPacket->buf, pMsg->GetBuf(), pMsg->GetSize());
	pPacket->nSize		= pMsg->GetSize();
	SafeCopy(pPacket->szName, szName, _MAX_NAMESIZE);
	m_pMsgPort->Send(MSGPORT_WORLD, WORLD_USERNAMEMSG, STRUCT_TYPE(buf), pPacket);

	return true;
}

bool CMapGroup::TransmitWorldMsg(CNetMsg* pMsg, OBJID idUser)
{
	MESSAGESTR	buf;
	USERIDMSG_PACKET0* pPacket = (USERIDMSG_PACKET0*)buf;
	pPacket->idSocket	= pMsg->GetSocketID();
	pPacket->idPacket	= pMsg->GetType();
	memcpy(pPacket->buf, pMsg->GetBuf(), pMsg->GetSize());
	pPacket->nSize		= pMsg->GetSize();
	pPacket->idUser		= idUser;
	m_pMsgPort->Send(MSGPORT_WORLD, WORLD_USERIDMSG, STRUCT_TYPE(buf), pPacket);

	return true;
}

bool CMapGroup::SendSocketUserInfo(PROCESS_ID idProcess, SOCKET_ID idSocket, UserInfoStruct* pInfo)
{
	MESSAGESTR	buf;
	ST_USERCHANGEMAPGORUP* pPacket = (ST_USERCHANGEMAPGORUP*)buf;
	pPacket->idSocket	= idSocket;
	memcpy(&pPacket->info, pInfo, sizeof(UserInfoStruct));
	return m_pMsgPort->Send(idProcess, MAPGROUP_SOCKETUSERINFO, STRUCT_TYPE(ST_USERCHANGEMAPGORUP), &buf);
}

bool CMapGroup::SendObjInfo(PROCESS_ID idProcess, OBJID idUser, INFO_ID nInfoID, void* pInfo, int nSize)
{
	CHECKF(nInfoID > INFO_NONE && nInfoID < 20);
	MESSAGESTR	buf;
	CHANGEMAPGORUP_INFO0* pPacket		= (CHANGEMAPGORUP_INFO0*)buf;
	pPacket->idUser		= idUser;
	pPacket->nInfoType	= nInfoID;
	memcpy(&pPacket->info, pInfo, nSize);
	return m_pMsgPort->Send(idProcess, MAPGROUP_SENDOBJINFO, STRUCT_TYPE(buf), pPacket);
}

bool CMapGroup::SetNpcProcessID(OBJID idNpc, bool bAddNew)
{
	CHANGE_NPCPROCESSID	buf;
	buf.idNpc		= idNpc;
	if(bAddNew)
		buf.idProcess	= m_idProcess;
	else
		buf.idProcess	= PROCESS_NONE;
	return m_pMsgPort->Send(MSGPORT_SOCKET, SOCKET_SETNPCPROCESSID, STRUCT_TYPE(CHANGE_NPCPROCESSID), &buf);
}

bool CMapGroup::ChangeMapGroup(PROCESS_ID idProcess, OBJID idUser, OBJID idMap, int nPosX, int nPosY)
{
	ST_CHANGEMAPGROUP	st;
	st.idUser	= idUser;
	st.idMap	= idMap;
	st.nPosX	= nPosX;
	st.nPosY	= nPosY;
	return m_pMsgPort->Send(idProcess, MAPGROUP_CHANGEMAPGROUP, STRUCT_TYPE(ST_CHANGEMAPGROUP), &st);
}

bool CMapGroup::PrintText			(LPCTSTR szText)
{
	return m_pMsgPort->Send(MSGPORT_SHELL, SHELL_PRINTTEXT, STRING_TYPE(szText), szText);
}

bool CMapGroup::CloseMapGroup(SOCKET_ID idGmSocket)
{
	return m_pMsgPort->Send(MSGPORT_SOCKET, SOCKET_BREAKALLCONNECT, VARTYPE_INT, &idGmSocket);

	RoleManager()->SaveAll();
}

bool CMapGroup::ChangeTeam(int nAction, OBJID idTeam, OBJID idUser, int nData)
{
	ST_CHANGETEAM	st;
	st.nAction	= nAction;
	st.idTeam	= idTeam;
	st.idUser	= idUser;
	st.nData	= nData;
	for(int i = MSGPORT_MAPGROUP_FIRST; i < m_pMsgPort->GetSize(); i++)
	{
		if(i != m_idProcess)
			m_pMsgPort->Send(i, MAPGROUP_CHANGETEAM, STRUCT_TYPE(st), &st);
	}
	return true;
}

void CMapGroup::ModifyNpc(OBJID idNpc, LPCTSTR szField, LPCTSTR szData)
{
	char	buf[1024];
	char*	ptr = buf;
	*(int*)ptr	= idNpc;
	ptr	+= sizeof(int);
	strcpy(ptr, szField);
	ptr	= ptr + strlen(szField) + 1;
	strcpy(ptr, szData);
	ptr = ptr + strlen(szData) + 1;

	for(int i = MSGPORT_MAPGROUP_FIRST; i < m_pMsgPort->GetSize(); i++)
	{
		if(i != m_idProcess)
			m_pMsgPort->Send(i, MAPGROUP_CHANGENPC, BUFFER_TYPE(ptr - buf), buf);
	}
}

void CMapGroup::SynchroNpc(OBJID idNpc, OBJID idOwner, OBJID idRecord, int nData0, int nData1, int nData2, bool bBroadcast)
{
	for(IRoleSet::Iter i = RoleManager()->QuerySet()->Begin(); i != RoleManager()->QuerySet()->End(); i++)
	{
		IRole* pRole = RoleManager()->QuerySet()->GetObjByIter(i);
		CNpc* pNpc;
		if(pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->GetRecordID() == idRecord)
		{
			CNpc* pTarget;
			IF_OK(pRole->QueryObj(OBJ_NPC, IPP_OF(pTarget)))
				pTarget->SynchroData(idNpc, idOwner, nData0, nData1, nData2);
			return;
		}
	}

	if(bBroadcast)
	{
		OBJID	setData[6];
		setData[0]	= idNpc;
		setData[1]	= idOwner;
		setData[2]	= idRecord;
		setData[3]	= nData0;
		setData[4]	= nData1;
		setData[5]	= nData2;
		for(int i = MSGPORT_MAPGROUP_FIRST; i < m_pMsgPort->GetSize(); i++)
		{
			if(i != m_idProcess)
				m_pMsgPort->Send(i, MAPGROUP_SETTRANSNPC, STRUCT_TYPE(setData), &setData);
		}
	}
}

void CMapGroup::DelTransNpc(OBJID idNpc, OBJID idOwner, OBJID idMasterNpc, BOOL bSynchro, bool bBroadcast)
{
	for(IRoleSet::Iter i = RoleManager()->QuerySet()->Begin(); i != RoleManager()->QuerySet()->End(); i++)
	{
		IRole* pRole = RoleManager()->QuerySet()->GetObjByIter(i);
		if(pRole)
		{
			CNpc* pNpc;
			if(pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
			{
				if(pNpc->GetType() == _SYNTRANS_NPC && pNpc->GetInt(NPCDATA_LINKID) == idMasterNpc)
				{
					if(!bSynchro && pNpc->GetID() == idNpc || bSynchro && pNpc->GetID() != idNpc)
						MYASSERT(pNpc->DelNpc());
					else if(bSynchro && pNpc->GetID() == idNpc)
						pNpc->SetInt(NPCDATA_OWNERID, idOwner, false);
				}
			}
		}
	}

	if(bBroadcast)
	{
		OBJID	setData[4];
		setData[0]	= idNpc;
		setData[1]	= idOwner;
		setData[2]	= idMasterNpc;
		setData[3]	= bSynchro;
		for(int i = MSGPORT_MAPGROUP_FIRST; i < m_pMsgPort->GetSize(); i++)
		{
			if(i != m_idProcess)
				m_pMsgPort->Send(i, MAPGROUP_DELTRANSNPC, STRUCT_TYPE(setData), &setData);
		}
	}
}

void CMapGroup::ChangeCode (SOCKET_ID idSocket, DWORD dwData)
{
	CHANGE_USERDATA	st;
	st.idSocket		= idSocket;
	st.nData		= dwData;
	m_pMsgPort->Send(MSGPORT_SOCKET, SOCKET_CHANGECODE, STRUCT_TYPE(CHANGE_USERDATA), &st);
}

void CMapGroup::SendRpc(const CEventPack& pack)
{
	for(int i = MSGPORT_MAPGROUP_FIRST; i < m_pMsgPort->GetSize(); i++)
	{
		if(i != m_idProcess)
			m_pMsgPort->Send(i, MAPGROUP_RPC, BUFFER_TYPE(pack.GetSize()), pack.GetBuf());
	}
}

void CMapGroup::QuerySocketIP(PROCESS_ID idProcess, SOCKET_ID idSocket)
{
	ST_QUERYIP	st;
	memset(&st, 0, sizeof(st));
	st.idProcess	= idProcess;
	st.idSocket		= idSocket;
	m_pMsgPort->Send(MSGPORT_SOCKET, SOCKET_QUERYIP, STRUCT_TYPE(ST_QUERYIP), &st);
}

bool CMapGroup::CreateSyndicate		(const CreateSyndicateInfo* pInfo)
{
	return m_pMsgPort->Send(MSGPORT_WORLD, KERNEL_CREATESYNDICATE, STRUCT_TYPE(CreateSyndicateInfo), pInfo);
}

bool CMapGroup::DestroySyndicate(OBJID idSyn)
{
	return m_pMsgPort->Send(MSGPORT_WORLD, KERNEL_DESTROYSYNDICATE,	VARTYPE_ULONG, &idSyn);
}

bool CMapGroup::CombineSyndicate(OBJID idSyn, OBJID idTarget)
{
	OBJID	setID[2];
	setID[0]	= idSyn;
	setID[1]	= idTarget;
	return m_pMsgPort->Send(MSGPORT_WORLD, KERNEL_COMBINESYNDICATE, STRUCT_TYPE(setID), &setID);
}

bool CMapGroup::ChangeSyndicate		(const SynFuncInfo0* pFuncInfo)
{
	return m_pMsgPort->Send(MSGPORT_WORLD, KERNEL_CHANGESYNDICATE, BUFFER_TYPE(pFuncInfo->nSize), pFuncInfo);
}

void CMapGroup::SetMapSynID(OBJID idMap, OBJID idSyn)
{
	OBJID	setID[2];
	setID[0]	= idMap;
	setID[1]	= idSyn;
	for(int i = MSGPORT_MAPGROUP_FIRST; i < m_pMsgPort->GetSize(); i++)
	{
		if(i != m_idProcess)
			m_pMsgPort->Send(i, MAPGROUP_SETMAPSYNID, STRUCT_TYPE(setID), &setID);
	}
}

int CMapGroup::GetMapGroupAmount()
{
	CHECKF(m_pMsgPort);
	return m_pMsgPort->GetSize() - MSGPORT_MAPGROUP_FIRST + 1;
}

void CMapGroup::LevelUp		(OBJID idUser, int nLevel)
{
	m_pMsgPort->Send(MSGPORT_WORLD, WORLD_LEVELUP, VARTYPE_INT, &idUser);
}

void CMapGroup::QueryFee		(OBJID idAccount)
{
	m_pMsgPort->Send(MSGPORT_WORLD, WORLD_QUERYFEE, VARTYPE_INT, &idAccount);
}

void CMapGroup::LoadConfig()
{
	char szSQL[1024];
	_snprintf(szSQL, 1024, "SELECT * FROM " _TBL_PREHEAD "config where type between 7001 and 7003");
	IRecordset* pRes = Database()->CreateNewRecordset(szSQL, true);
	if(pRes)
	{
		CONFIG_INFO	info;
		memset(&info, 0, sizeof(info));
		for(int i = 0; i < pRes->RecordCount(); i++)
		{
			info.nData[0]	= pRes->GetInt(2);
			info.nData[1]	= pRes->GetInt(3);
			info.nData[2]	= pRes->GetInt(4);
			info.nData[3]	= pRes->GetInt(5);
			info.nData[4]	= pRes->GetInt(6);
			OBJID idKey		= pRes->GetInt(1);
			m_setConfig.rehash(m_setConfig.size() + 1);
			m_setConfig.insert(CONFIG_SET::value_type(idKey, info));
			pRes->MoveNext();
		}
		pRes->Release();
	}
}

CMapGroup::CONFIG_INFO* CMapGroup::QueryConfig(OBJID idKey)
{
	CONFIG_SET::iterator iter = m_setConfig.find(idKey);
	if(iter != m_setConfig.end())
		return &iter->second;
	return NULL;
}

void CMapGroup::LoadIncomeLimit()
{
	char szSQL[1024];
	_snprintf(szSQL, 1024, "SELECT * FROM " _TBL_PREHEAD "config where type = 6002");
	IRecordset* pRes = Database()->CreateNewRecordset(szSQL, true);
	if(pRes)
	{
		m_dwReserved7C = pRes->GetInt(2);
		pRes->Release();
	}
}

void CMapGroup::LoadItemScore()
{
	char szSQL[1024];
	_snprintf(szSQL, 1024, "select * from " _TBL_PREHEAD "item_value_type");	szSQL[sizeof(szSQL)-1] = 0;
	IRecordset* pRes = Database()->CreateNewRecordset(szSQL, true);
	if(pRes)
	{
		for(DWORD i = 0; i < pRes->RecordCount(); i++)
		{
			int nType	= pRes->GetInt(1);
			int nKey	= pRes->GetInt(2);
			int nValue	= pRes->GetInt(3);
			switch(nType)
			{
			case 1:	CItem::SetEmoneyRate(nValue);						break;
			case 2:	(*CItem::QueryGem1ScoreSet())[nKey]		= nValue;	break;
			case 3:	(*CItem::QueryGem2ScoreSet())[nKey]		= nValue;	break;
			case 4:	(*CItem::QueryAdditionScoreSet())[nKey]	= nValue;	break;
			case 5:	(*CItem::QueryTypeScoreSet())[nKey]		= nValue;	break;
			case 6:	(*CItem::QueryReduceDmgScoreSet())[nKey]	= nValue;	break;
			}
			pRes->MoveNext();
		}
		pRes->Release();
	}
}
