#include "allmsg.h"
#include "common.h"
#include "SynAttr.h"
#include "MapGroup.h"
#include "AutoPtr.h"

const int	MINI_SUB_SYN			= 20;

MYHEAP_IMPLEMENTATION(CSynAttr,s_heap)
char	szSynAttrTable[]	= _TBL_SYNATTR;
CSynAttr::CSynAttr()
{
	m_pData	= NULL;
}

CSynAttr::~CSynAttr()
{
	SaveInfo();
	if(m_pData)
		m_pData->Release();
}

CSynAttr* CSynAttr::CreateNew(PROCESS_ID idProcess, CUser* pUser)
{
	CSynAttr* pSynAttr = new CSynAttr;
	CHECKF(pSynAttr);

	pSynAttr->m_idProcess	= idProcess;
	pSynAttr->m_pUser		= pUser;

	return pSynAttr;
}

bool CSynAttr::Create()
{
	CHECKF(CMapGroup::IsValidMapGroupProcessID(m_idProcess));
	CHECKF(!m_pData);
	CHECKF(m_pUser);

	m_pData	= CSynAttrData::CreateNew();
	CHECKF(m_pData);

	if(!m_pData->Create(m_pUser->GetID(), Database()))
	{
		SAFE_RELEASE (m_pData);
		return true;
	}

	if(SynManager()->QuerySyndicate(GetSynID()) == NULL)
	{
		SAFE_RELEASE (m_pData);
		return true;
	}

	return true;
}

bool CSynAttr::SendInfoToClient()
{
	SynAttrInfoStruct	info;
	GetInfo(&info);

	CSyndicate* pSyn = SynManager()->QuerySyndicate(GetSynID());

	CMsgSynAttrInfo msg;
	IF_OK(msg.Create(&info, pSyn))
		m_pUser->SendMsg(&msg);

	return true;
}

bool CSynAttr::SaveInfo()
{
	if(m_pData)
		return m_pData->Update();
	return true;
}

bool CSynAttr::GetInfo(SynAttrInfoStruct* pInfo)
{
	CHECKF(pInfo);

	memset(pInfo, 0, sizeof(SynAttrInfoStruct));
	pInfo->id			= m_pUser->GetID();
	if(m_pData)
	{
		pInfo->idSyn		= m_pData->GetInt(SYNATTRDATA_SYNID);
		pInfo->ucRank		= m_pData->GetInt(SYNATTRDATA_RANK);
		pInfo->ucRankShow	= GetSynRankShow();
		pInfo->nProffer		= m_pData->GetInt(SYNATTRDATA_PROFFER);
	}

	return true;
}

bool CSynAttr::AppendSynAttr(SynAttrInfoStruct* pInfo)
{
	CHECKF(!m_pData);
	if(pInfo->idSyn != ID_NONE)
	{
		m_pData = CSynAttrData::CreateNew();
		CHECKF(m_pData);
		IF_NOT(m_pData->Create(GameDataDefault()->GetSynAttrData(), pInfo->id))
			return false;

		m_pData->SetInt(SYNATTRDATA_SYNID, pInfo->idSyn);
		m_pData->SetInt(SYNATTRDATA_RANK, pInfo->ucRank);
		m_pData->SetInt(SYNATTRDATA_PROFFER, pInfo->nProffer);
		m_pData->ClearUpdateFlags();
	}

	return true;
}

OBJID CSynAttr::CreateSyndicate(LPCTSTR szName, int nLevel, int nMoney, int nMoneyLeave)
{
	CHECKF(!m_pData);

	if((int)m_pUser->GetLev() < nLevel)
	{
		m_pUser->SendSysMsg(STR_NOT_ENOUGH_LEV, nLevel);
		return false;
	}

	if((int)m_pUser->GetMoney() < nMoney)
	{
		m_pUser->SendSysMsg(STR_NOT_ENOUGH_MONEY, nMoney);
		return false;
	}
	CreateSyndicateInfo info;
	info.idSyn = ID_NONE;
	SafeCopy(info.szName, szName, _MAX_NAMESIZE);
	SafeCopy(info.szTenet, DEFAULT_TENET, MAX_TENETSIZE);
	SafeCopy(info.szLeaderTitle, DEFAULT_LEADERTITLE, _MAX_NAMESIZE);
	SafeCopy(info.szMemberTitle, DEFAULT_MEMBERTITLE, _MAX_NAMESIZE);
	info.idLeader = m_pUser->GetID();
	SafeCopy(info.szLeader, m_pUser->GetName(), _MAX_NAMESIZE);
	info.nMoney = nMoneyLeave;
	info.idFealty = ID_NONE;
	OBJID idSyn = SynManager()->QueryModify()->CreateSyndicate(&info);
	if(idSyn == ID_NONE)
	{
		m_pUser->SendSysMsg(STR_FORBIDDEN_GUILD_NAME);
		return false;
	}

	if(!JoinSyn(idSyn, RANK_LEADER, nMoneyLeave))
	{
		::GmLogSave("ERROR: player [%s] created a syndicate, but the leader record could not be created!", m_pUser->GetName());
		LOGERROR("ERROR: player [%s] created a syndicate, but the leader record could not be created!", m_pUser->GetName());
	}
	CHECKF(m_pData);
	m_pUser->SpendMoney(nMoney, SYNCHRO_TRUE);

	return idSyn;
}

OBJID CSynAttr::CreateSubSyn(LPCTSTR szName, int nLevel, int nMoney, int nMoneyLeave)
{
	CHECKF(m_pData);
	if((int)m_pUser->GetLev() < nLevel)
	{
		m_pUser->SendSysMsg(STR_NOT_ENOUGH_LEV, nLevel);
		return false;
	}

	CSyndicate* pFealty = SynManager()->QuerySyndicate(GetSynID());
	CHECKF(pFealty);
	if(pFealty->GetInt(SYNDATA_MONEY) < nMoney)
	{
		m_pUser->SendSysMsg(STR_GUILD_FUND_NOT_ENOUGH, nMoney);
		return false;
	}

	CreateSyndicateInfo info;
	info.idSyn = ID_NONE;
	SafeCopy(info.szName, szName, _MAX_NAMESIZE);
	SafeCopy(info.szTenet, DEFAULT_TENET, MAX_TENETSIZE);
	SafeCopy(info.szLeaderTitle, DEFAULT_LEADERTITLE, _MAX_NAMESIZE);
	SafeCopy(info.szMemberTitle, DEFAULT_MEMBERTITLE, _MAX_NAMESIZE);
	info.idLeader = m_pUser->GetID();
	SafeCopy(info.szLeader, m_pUser->GetName(), _MAX_NAMESIZE);
	info.nMoney = nMoneyLeave;
	info.idFealty = GetSynID();

	OBJID idNewSyn = SynManager()->QueryModify()->CreateSyndicate(&info);
	if(idNewSyn == ID_NONE)
	{
		m_pUser->SendSysMsg(STR_FORBIDDEN_GUILD_NAME);
		return false;
	}

	DelMemberList(GetSynID(), m_pUser->GetName());
	AddMemberList(idNewSyn, m_pUser->GetName(), m_pUser->GetLev());

	pFealty->QueryModify()->AddData(SYNDATA_MONEY, -1*nMoney);
	pFealty->QueryModify()->AddData(SYNDATA_AMOUNT, -1);

	CMsgSyndicate msg;
	IF_OK(msg.Create(SET_SUBSYN, idNewSyn))
		pFealty->BroadcastSynMsg(&msg);

	return idNewSyn;
}

bool CSynAttr::ChangeLeader(LPCTSTR szSyn, int nLevel)
{
	CHECKF(m_pData);
	if((int)m_pUser->GetLev() < nLevel)
	{
		m_pUser->SendSysMsg(STR_NOT_ENOUGH_LEV, nLevel);
		return false;
	}

	CSyndicate* pSyn = SynManager()->QuerySyndicate(GetSynID());
	CHECKF(pSyn);

	CSyndicate* pTargetSyn = SynManager()->QuerySynByName(szSyn);
	if(!pTargetSyn)
	{
		m_pUser->SendSysMsg(STR_GUILD_INEXIST);
		return false;
	}

	OBJID idOldLeader = pTargetSyn->GetInt(SYNDATA_LEADERID);

	pTargetSyn->QueryModify()->Demise(idOldLeader, m_pUser->GetID(), m_pUser->GetName(), pSyn->GetID(), true);

	return true;
}

bool CSynAttr::JoinSyn(OBJID idSyn, int nRank, int nProffer)
{
	CHECKF(!m_pData);

	m_pData	= CSynAttrData::CreateNew();
	CHECKF(m_pData);
	IF_NOT(m_pData->Create(GameDataDefault()->GetSynAttrData(), m_pUser->GetID()))
	{
		m_pData->Release();
		m_pData = NULL;
		return false;
	}
	m_pData->SetInt(SYNATTRDATA_ID, m_pUser->GetID());
	m_pData->SetInt(SYNATTRDATA_SYNID, idSyn);
	m_pData->SetInt(SYNATTRDATA_RANK, nRank);
	m_pData->SetInt(SYNATTRDATA_PROFFER, nProffer);

	if(m_pData->InsertRecord() != m_pUser->GetID())
	{
		m_pData->SetInt(SYNATTRDATA_SYNID, idSyn);
		m_pData->SetInt(SYNATTRDATA_RANK, nRank);
		m_pData->SetInt(SYNATTRDATA_PROFFER, nProffer);
		m_pData->Update();
	}

	CSyndicate* pSyn = SynManager()->QuerySyndicate(GetSynID());
	if(pSyn)
		pSyn->QueryModify()->AddData(SYNDATA_AMOUNT, 1);

	AddMemberList(idSyn, m_pUser->GetName(), m_pUser->GetLev());

	SynchroInfo(true);
	if(pSyn)
		pSyn->SendInfoToClient(m_pUser);
	return true;
}

bool CSynAttr::AddMemberList(OBJID idSyn, LPCTSTR szName, int nLevel)
{
	MESSAGESTR	buf;
	SynFuncInfo0*	pInfo = (SynFuncInfo0*)&buf;
	pInfo->idSyn		= idSyn;
	pInfo->ucFuncType	= SYNFUNC_ADD_MEMBER;
	pInfo->IntParam[0]	= nLevel;
	SafeCopy((char*)&pInfo->IntParam[1], szName, _MAX_NAMESIZE);
	pInfo->ucSaveFlag		= true;
	pInfo->nSize		= sizeof(SynFuncInfo0) + sizeof(int) + _MAX_NAMESIZE;
	MapGroup(PID)->QuerySynMessage()->ChangeSyndicate(pInfo);

	return true;
}

void CSynAttr::UpLevel(int nLevel)
{
	CHECK(GetSynID());

	MESSAGESTR	buf;
	SynFuncInfo0*	pInfo = (SynFuncInfo0*)&buf;
	pInfo->idSyn		= GetSynID();
	pInfo->ucFuncType	= SYNFUNC_UP_LEVEL;
	pInfo->IntParam[0]	= m_pUser->GetLev();
	SafeCopy((char*)&pInfo->IntParam[1], m_pUser->GetName(), _MAX_NAMESIZE);
	pInfo->ucSaveFlag		= true;
	pInfo->nSize		= sizeof(SynFuncInfo0) + sizeof(int) + _MAX_NAMESIZE;
	MapGroup(PID)->QuerySynMessage()->ChangeSyndicate(pInfo);
}

void CSynAttr::SynchroInfo(BOOL bAnnounce)
{
	SynAttrInfoStruct	info;
	GetInfo(&info);
	CSyndicate* pSyn = SynManager()->QuerySyndicate(GetSynID());
	CMsgSynAttrInfo msg;
	IF_OK(msg.Create(&info, pSyn))
		m_pUser->SendMsg(&msg);
	{
		CMsgPlayer msg;
		IF_OK(msg.Create(m_pUser->QueryRole()))
			m_pUser->BroadcastRoomMsg(&msg, INCLUDE_SELF);
	}

	if(bAnnounce)
	{
		LPCTSTR pAnnounce = "";
		if(pSyn)
		{
			CSyndicate* pMaster = pSyn->GetMasterSyn();
			MYASSERT(pMaster);
			pAnnounce = pMaster->GetStr(SYNDATA_TENET);
		}
		m_pUser->SendSysMsg(_TXTATR_SYNANNOUNCE, pAnnounce);
	}
}

bool CSynAttr::LeaveSyn(bool bDeleteRecord , bool bSynchro )
{
	CHECKF(m_pData);

	OBJID idOldSyn = GetSynID();

	if(bDeleteRecord)
		m_pData->DeleteRecord();
	m_pData->Release();
	m_pData	= NULL;

	if(bSynchro)
	{
		CSyndicate* pSyn = SynManager()->QuerySyndicate(idOldSyn);
		IF_OK(pSyn && pSyn->GetInt(SYNDATA_AMOUNT) > 0)
		{
			pSyn->QueryModify()->AddData(SYNDATA_AMOUNT, -1);
		}

		DelMemberList(idOldSyn, m_pUser->GetName());
		SynchroInfo();
	}

	return true;
}

bool CSynAttr::DelMemberList(OBJID idOldSyn, LPCTSTR szName)
{
	MESSAGESTR	buf;
	SynFuncInfo0*	pInfo = (SynFuncInfo0*)&buf;
	pInfo->idSyn		= idOldSyn;
	pInfo->ucFuncType	= SYNFUNC_DEL_MEMBER;
	SafeCopy(pInfo->StrParam, szName, _MAX_NAMESIZE);
	pInfo->ucSaveFlag		= true;
	pInfo->nSize		= sizeof(SynFuncInfo0) + _MAX_NAMESIZE;
	MapGroup(PID)->QuerySynMessage()->ChangeSyndicate(pInfo);

	return true;
}

bool CSynAttr::DonateMoney(int nMoney)
{
	if(!m_pData)
		return false;

	if(nMoney <= 0)
		return false;

	CSyndicate* pSyn = SynManager()->QuerySyndicate(GetSynID());
	if(!pSyn)
		return false;

	if((int)m_pUser->GetMoney() < nMoney)
	{
		m_pUser->SendSysMsg(STR_NOT_SO_MUCH_MONEY);
		return false;
	}

	if(pSyn->QueryModify()->AddData(SYNDATA_MONEY, nMoney))
	{
		m_pData->SetInt(SYNATTRDATA_PROFFER, ::AddToTop(m_pData->GetInt(SYNATTRDATA_PROFFER), nMoney, SYNPROFFER_LIMIT));
		m_pUser->SpendMoney(nMoney, SYNCHRO_TRUE);

		MSGBUF szMsg;
		sprintf(szMsg, STR_DONATE_ssd, m_pUser->GetName(), pSyn->GetStr(SYNDATA_NAME), nMoney);
		m_pUser->BroadcastRoomMsg(szMsg, INCLUDE_SELF);

		if(nMoney >= CHANGELEADER_MONEY)
		{
			ChangeLeader();
		}
		SynchroInfo();

		if(nMoney >= 10000)
			::MsgLogSave("gmlog/syndenote", "User:%s[%u] denote syn money %u", m_pUser->GetName(), m_pUser->GetID(), nMoney);
		return true;
	}

	return false;
}

bool CSynAttr::AddProffer(int nData)
{
	CHECKF(m_pData && nData > 0);

	int	nProffer = ::AddToTop(m_pData->GetInt(SYNATTRDATA_PROFFER), nData, SYNPROFFER_LIMIT);
	m_pData->SetInt(SYNATTRDATA_PROFFER, nProffer);

	CMsgUserAttrib	msg;
	IF_OK(msg.Create(m_pUser->GetID(), _USERATTRIB_PROFFER, nProffer))
		m_pUser->SendMsg(&msg);
	return true;
}

bool CSynAttr::ChangeLeader()
{
	CHECKF(m_pData);

	CSyndicate* pSyn = SynManager()->QuerySyndicate(GetSynID());
	CHECKF(pSyn);

	OBJID idOldLeader = pSyn->GetInt(SYNDATA_LEADERID);
	if(UserManager()->GetUser(idOldLeader))
		return false;

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT * FROM cq_synattr WHERE rank=%d && syn_id=%u", RANK_LEADER, pSyn->GetID());
	IRecordset* pRes = Database()->CreateNewRecordset(szSQL);
	if(pRes)
	{
		bool ret = (pRes->RecordCount() <= 1 && pRes->LoadDWord("id") == idOldLeader);
		IF_NOT_(ret)
		{
			pRes->Release();
			return false;
		}
		sprintf(szSQL, "SELECT * FROM cq_user WHERE id=%u", idOldLeader);
		IRecordset* pUserRes = Database()->CreateNewRecordset(szSQL);
		if(pUserRes)
		{
			int nLastLogin = pUserRes->LoadDWord("last_login");
			pUserRes->Release();

			if(::DateStamp() <= ::DateStampPass(nLastLogin, CHANGELEADER_DAYS))
			{
				pRes->Release();
				return false;
			}

			pRes->SetInt(SYNATTRDATA_RANK, RANK_NEWBIE);
			pRes->UpdateRecord();
		}

		pRes->Release();
	}
	MSGBUF	szMsg;
	sprintf(szMsg, STR_SYN_LEADER_LOSE_uss, pSyn->GetID(), pSyn->GetStr(SYNDATA_NAME), m_pUser->GetName());
	::GmLogSave(szMsg);
	LOGMSG(szMsg);

	sprintf(szSQL, "SELECT * FROM cq_synattr WHERE rank=%d && syn_id=%u", RANK_LEADER, pSyn->GetID());
	IRecordset* pTempRes = Database()->CreateNewRecordset(szSQL);
	if(pTempRes)
	{
		pTempRes->Release();
		return false;
	}
	pSyn->QueryModify()->Demise(idOldLeader, m_pUser->GetID(), m_pUser->GetName(), GetSynID(), true);

	return true;
}

bool CSynAttr::SetRank(int nRank, bool bLeader )
{
	CHECKF(nRank != RANK_LEADER && m_pData->GetInt(SYNATTRDATA_RANK) != RANK_LEADER || bLeader);

	m_pData->SetInt(SYNATTRDATA_RANK, nRank);
	if(bLeader)
		m_pData->Update();

	SynchroInfo();
	return true;
}

bool CSynAttr::SetIdRank(OBJID idSyn, int nRank, bool bSynchro)
{
	m_pData->SetInt(SYNATTRDATA_SYNID, idSyn);
	m_pData->SetInt(SYNATTRDATA_RANK, nRank);
	m_pData->Update();

	if(bSynchro)
		SynchroInfo();
	return true;
}

int CSynAttr::GetAssistantCount()
{
	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT COUNT(*) FROM cq_synattr as s LEFT JOIN cq_user AS u ON u.id=s.id WHERE s.syn_id=%u && s.rank=%d && u.id IS NOT NULL", GetSynID(), 90);
	CAutoPtr<IRecordset> pStatRes = Database()->CreateNewRecordset(szSQL);
	IF_OK(pStatRes)
		return pStatRes->GetInt(0);

	MYASSERT(!"GetAssistantCount");
	return MAX_ASSISTANTSIZE;
}

bool CSynAttr::KickoutMember(LPCTSTR szTarget)
{
	CHECKF(szTarget);

	OBJID idSyn = GetSynID();

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT id FROM cq_user WHERE name='%s' LIMIT 1", szTarget);
	IRecordset* pRes = Database()->CreateNewRecordset(szSQL);
	if(!pRes)
		return false;
	OBJID idUser = pRes->GetInt(0);
	pRes->Release();

	_snprintf(szSQL, 1024, "select * from cq_synattr where id=%u and syn_id=%u and rank<>%u limit 1", idUser, idSyn, RANK_LEADER);
	pRes = Database()->CreateNewRecordset(szSQL);
	if(!pRes)
		return false;
	pRes->Release();

	sprintf(szSQL, "DELETE FROM cq_synattr WHERE id=%u && syn_id=%u && rank!=%u LIMIT 1", idUser, idSyn, RANK_LEADER);
	Database()->ExecuteSQL(szSQL);

	char	buf[1024];
	REMOTE_CALL0* pInfo = (REMOTE_CALL0*)&buf;
	pInfo->idUser		= idUser;
	pInfo->ucFuncType	= REMOTE_CALL_LEAVE_SYN;
	pInfo->IntParam[0]	= GetSynID();
	SafeCopy((char*)&pInfo->IntParam[1], m_pUser->GetName(), _MAX_NAMESIZE);
	pInfo->nSize		= sizeof(REMOTE_CALL0) + sizeof(int) + _MAX_NAMESIZE;
	MapGroup(PID)->QueryIntraMsg()->RemoteCall(pInfo);

	OBJID	idOldSyn = GetSynID();
	{
		CSyndicate* pSyn = SynManager()->QuerySyndicate(idOldSyn);
		IF_OK(pSyn && pSyn->GetInt(SYNDATA_AMOUNT) > 0)
		{
			pSyn->QueryModify()->AddData(SYNDATA_AMOUNT, -1);
		}
	}
	DelMemberList(idOldSyn, szTarget);
	return true;
}

DWORD CSynAttr::GetSynRankShow		()
{
	if(GetSynID() != ID_NONE)
	{
		int nRankShow = m_pData->GetInt(SYNATTRDATA_RANK);
		if(nRankShow > RANK_NEWBIE)
		{
			CSyndicate* pSyn = SynManager()->QuerySyndicate(GetSynID());
			if(pSyn && pSyn->GetInt(SYNDATA_FEALTY) != ID_NONE)
			{
				nRankShow	-= RANK_SUB;
				if(pSyn->GetInt(SYNDATA_AMOUNT) < MINI_SUB_SYN)
					nRankShow		-= RANK_PRE;
			}
		}
		return nRankShow;
	}
	return RANK_NONE;
}

OBJID CSynAttr::GetMasterSynID()
{
	if(GetSynID() != ID_NONE)
	{
		CSyndicate* pSyn = SynManager()->QuerySyndicate(GetSynID());
		IF_OK(pSyn)
		{
			if(pSyn->GetInt(SYNDATA_FEALTY) != ID_NONE)
				return pSyn->GetInt(SYNDATA_FEALTY);
			else
				return pSyn->GetID();
		}
	}

	return ID_NONE;
}

int CSynAttr::SumSynMember()
{
	int nSum = 0;
	CSynSet::Iterator pSyn = SynManager()->QuerySynSet()->NewEnum();
	while(pSyn.Next())
	{
		if(pSyn && pSyn->GetMasterSynID() == GetMasterSynID())
		{
			nSum += pSyn->GetInt(SYNDATA_AMOUNT);
		}
	}
	return nSum;
}

LPCTSTR CSynAttr::GetRankTitle()
{
	switch(GetSynRankShow())
	{
	case	100:
		return STR_SYNDICATE_LEADER;
		break;
	case	90:
		return STR_SYNDICATE_LEADER2;
		break;
	case	80:
		return STR_SYNDICATE_LEADER3;
		break;
	case	70:
		return STR_SYNDICATE_LEADER4;
		break;
	case	60:
		return STR_SYNDICATE_LEADER5;
		break;
	case	50:
		return STR_SYNDICATE_MEMBER;
		break;
	}
	return STR_ERROR;
}

bool CSynAttr::ChangeSyn(CSyndicate *pSyn, CSyndicate *pTarget)
{
	CHECKF(pSyn && pTarget);
	CHECKF(m_pData->GetInt(SYNATTRDATA_RANK) != RANK_LEADER);

	if(pSyn->GetMasterSynID() != pTarget->GetMasterSynID())
		return false;

	pSyn->QueryModify()->AddData(SYNDATA_AMOUNT, -1);
	DelMemberList(pSyn->GetID(), m_pUser->GetName());

	pTarget->QueryModify()->AddData(SYNDATA_AMOUNT, 1);
	AddMemberList(pTarget->GetID(), m_pUser->GetName(), m_pUser->GetLev());

	this->SetIdRank(pTarget->GetID(), RANK_NEWBIE);

	return true;
}
