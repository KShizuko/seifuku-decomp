#include "AllMsg.h"
#include "Syndicate.h"
#include "MapGroup.h"

char	szSynTable[] = "cq_syndicate";

MYHEAP_IMPLEMENTATION(CSyndicate,s_heap)
CSyndicate::CSyndicate()
{
	SetObjType(OBJ_SYN);
	m_pData	= NULL;
	m_obj.m_pOwner	= this;
	m_link.Init(this);
}

CSyndicate::~CSyndicate()
{
	if(m_pData)
	{
		m_pData->Update();
		m_pData->Release();
	}
}

bool CSyndicate::Create(IRecordset* pRes)
{
	MYASSERT(!m_pData);

	m_pData = CSynBase::CreateNew();
	CHECKF(m_pData);
	IF_NOT(m_pData->Create(pRes))
		return false;

	if(m_pData->GetInt(SYNDATA_DEL_FLAG) != 0)
		return false;

	return true;
}

bool CSyndicate::ReloadData()
{
	MYASSERT(m_pData);

	return true;
}

bool CSyndicate::Create(const CreateSyndicateInfo* pInfo, bool bSave)
{
	MYASSERT(!m_pData);

	OBJID idSyn = bSave ? ID_NONE : pInfo->idSyn;
	m_pData = CSynBase::CreateNew();
	CHECKF(m_pData);
	IF_NOT(m_pData->Create(GameDataDefault()->GetSynData(), idSyn))
		return false;

	m_pData->SetInt(SYNDATA_ID, idSyn);
	m_pData->SetStr(SYNDATA_NAME, pInfo->szName, _MAX_NAMESIZE);
	m_pData->SetStr(SYNDATA_TENET, pInfo->szTenet, MAX_TENETSIZE);
	m_pData->SetStr(SYNDATA_ANNOUNCE, pInfo->szLeaderTitle, _MAX_NAMESIZE);
	m_pData->SetStr(SYNDATA_MEMBERTITLE, pInfo->szMemberTitle, _MAX_NAMESIZE);
	m_pData->SetInt(SYNDATA_LEADERID, pInfo->idLeader);
	m_pData->SetStr(SYNDATA_LEADERNAME, pInfo->szLeader, _MAX_NAMESIZE);
	m_pData->SetInt(SYNDATA_MONEY, pInfo->nMoney);
	m_pData->SetInt(SYNDATA_FEALTY, pInfo->idFealty);
	m_pData->SetInt(SYNDATA_AMOUNT, 1);

	if(bSave)
	{
		return m_pData->InsertRecord() != ID_NONE;
	}
	else
	{
		m_pData->ClearUpdateFlags();
		return true;
	}
}

bool CSyndicate::Demise(OBJID idOldLeader, OBJID idNewLeader, LPCTSTR szNewLeader, OBJID idBackSyn, bool bSave)
{
	if(!( idOldLeader==(OBJID)m_pData->GetInt(SYNDATA_LEADERID) || (OBJID)m_pData->GetInt(SYNDATA_LEADERID)==ID_NONE ))
		return false;

	m_pData->SetInt(SYNDATA_LEADERID, idNewLeader);
	m_pData->SetStr(SYNDATA_LEADERNAME, szNewLeader, _MAX_NAMESIZE);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();

	CUser* pOldUser = UserManager()->GetUser(idOldLeader);
	if(pOldUser)
	{
		pOldUser->QuerySynAttr()->SetIdRank(idBackSyn, RANK_NEWBIE, true);
	}

	CUser* pNewUser = UserManager()->GetUser(idNewLeader);
	if(pNewUser)
	{
		pNewUser->QuerySynAttr()->SetIdRank(this->GetID(), RANK_LEADER, true);
	}

	return true;
}

bool CSyndicate::SetTenet(LPCTSTR szTenet, bool bSave)
{
	CHECKF(strlen(szTenet)<MAX_TENETSIZE);

	m_pData->SetStr(SYNDATA_TENET, szTenet, MAX_TENETSIZE);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();
	return true;
}

bool CSyndicate::SetLeaderTitle(LPCTSTR szLeaderTitle, bool bSave)
{
	m_pData->SetStr(SYNDATA_ANNOUNCE, szLeaderTitle, _MAX_NAMESIZE);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();
	return true;
}

bool CSyndicate::SetMemberTitle(LPCTSTR szMemberTitle, bool bSave)
{
	m_pData->SetStr(SYNDATA_MEMBERTITLE, szMemberTitle, _MAX_NAMESIZE);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();
	return true;
}

bool CSyndicate::AddData(SYNDATA idx, int nData, bool bSave)
{
	int nOldMoney	= m_pData->GetInt(SYNDATA_MONEY);

	int nAllData = ::AddToTop(m_pData->GetInt(idx), nData, SYNMONEY_LIMIT);
	m_pData->SetInt(idx, nAllData);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();

	if(nOldMoney > 0 && m_pData->GetInt(SYNDATA_MONEY) <= 0)
	{
		CMsgTalk	msg;
		IF_OK(msg.Create(_TXTATR_GM, STR_DESTROY_SYNDICATE_SOON))
			BroadcastSynMsg(&msg);
	}

	if(nData > 0)
	{
		this->SynchroInfo();
	}

	return true;
}

bool CSyndicate::SetData(SYNDATA idx, int nData, bool bSave)
{
	int	nOldData = m_pData->GetInt(idx);
	m_pData->SetInt(idx, nData);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();

	if(idx >= SYNDATA_ENEMY0 && idx < SYNDATA_ENEMY0+MAX_SYNENEMYSIZE)
	{
		CMsgSyndicate msg;
		if(nData)
		{
			IF_OK(msg.Create(SET_ANTAGONIZE, nData))
				BroadcastSynMsg(&msg);
		}
		else
		{
			IF_OK(msg.Create(CLEAR_ANTAGONIZE, nOldData))
				BroadcastSynMsg(&msg);
		}
	}
	else if(idx >= SYNDATA_ALLY0 && idx < SYNDATA_ALLY0+MAX_SYNALLYSIZE)
	{
		CMsgSyndicate msg;
		if(nData)
		{
			IF_OK(msg.Create(SET_ALLY, nData))
				BroadcastSynMsg(&msg);
		}
		else
		{
			IF_OK(msg.Create(CLEAR_ALLY, nOldData))
				BroadcastSynMsg(&msg);
		}
	}

	return true;
}

bool CSyndicate::PresentMoney(OBJID idSyn, int nMoney, bool bSave)
{
	if(nMoney <= 0 || m_pData->GetInt(SYNDATA_MONEY) < nMoney)
		return false;
	CSyndicate* pTarget = SynManager()->QuerySyndicate(idSyn);
	if(!pTarget)
		return false;

	m_pData->SetInt(SYNDATA_MONEY, m_pData->GetInt(SYNDATA_MONEY)-nMoney);
	pTarget->m_pData->SetInt(SYNDATA_MONEY, pTarget->m_pData->GetInt(SYNDATA_MONEY)+nMoney);
	if(bSave)
	{
		m_pData->Update();
		pTarget->m_pData->Update();
	}
	else
	{
		m_pData->ClearUpdateFlags();
		pTarget->m_pData->ClearUpdateFlags();
	}
	return true;
}

bool CSyndicate::ChangeSyndicate(const SynFuncInfo0* pFuncInfo)
{
	CHECKF(pFuncInfo->idSyn == GetID());

	switch(pFuncInfo->ucFuncType)
	{
	case	SYNFUNC_DEMISE:
		return Demise(pFuncInfo->IntParam[0], pFuncInfo->IntParam[1], (LPCTSTR)&pFuncInfo->IntParam[3], pFuncInfo->IntParam[2], pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_SETTENET:
		return SetTenet(pFuncInfo->StrParam, pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_SETLEADERTITLE:
		return SetLeaderTitle(pFuncInfo->StrParam, pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_SETMEMBERTITLE:
		return SetMemberTitle(pFuncInfo->StrParam, pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_ADDDATA:
		return AddData((SYNDATA)pFuncInfo->IntParam[0], pFuncInfo->IntParam[1], pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_SETDATA:
		return SetData((SYNDATA)pFuncInfo->IntParam[0], pFuncInfo->IntParam[1], pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_PRESENT:
		return PresentMoney(pFuncInfo->IntParam[0], pFuncInfo->IntParam[1], pFuncInfo->ucSaveFlag!=0);
		break;
	default:
		MYASSERT(!"switch CSyndicate::ChangeSyndicate()");
		break;
	}
	return false;
}

bool CSyndicate::XSynModify::Demise(OBJID idOldLeader, OBJID idNewLeader, LPCTSTR szNewLeader, OBJID idBackSyn, bool bSave)
{
	MESSAGESTR	buf;
	SynFuncInfo0*	pInfo = (SynFuncInfo0*)&buf;
	pInfo->idSyn		= This()->GetID();
	pInfo->ucFuncType	= SYNFUNC_DEMISE;
	pInfo->IntParam[0]	= idOldLeader;
	pInfo->IntParam[1]	= idNewLeader;
	pInfo->IntParam[2]	= idBackSyn;
	SafeCopy((char*)(&(pInfo->IntParam[3])), szNewLeader, _MAX_NAMESIZE);
	pInfo->ucSaveFlag		= (unsigned char)bSave;
	pInfo->nSize		= sizeof(SynFuncInfo0) + sizeof(int)*3 + _MAX_NAMESIZE;

	MapGroup(This()->m_idProcess)->QuerySynMessage()->ChangeSyndicate(pInfo);
	return true;
}

bool CSyndicate::XSynModify::SetTenet(LPCTSTR szTenet, bool bSave)
{
	MESSAGESTR	buf;
	SynFuncInfo0*	pInfo = (SynFuncInfo0*)&buf;
	pInfo->idSyn		= This()->GetID();
	pInfo->ucFuncType	= SYNFUNC_SETTENET;
	SafeCopy(pInfo->StrParam, szTenet, MAX_TENETSIZE);
	pInfo->ucSaveFlag		= (unsigned char)bSave;
	pInfo->nSize		= sizeof(SynFuncInfo0) + MAX_TENETSIZE;

	MapGroup(This()->m_idProcess)->QuerySynMessage()->ChangeSyndicate(pInfo);
	return true;
}

bool CSyndicate::XSynModify::SetLeaderTitle(LPCTSTR szLeaderTitle, bool bSave)
{
	MESSAGESTR	buf;
	SynFuncInfo0*	pInfo = (SynFuncInfo0*)&buf;
	pInfo->idSyn		= This()->GetID();
	pInfo->ucFuncType	= SYNFUNC_SETLEADERTITLE;
	SafeCopy(pInfo->StrParam, szLeaderTitle, _MAX_NAMESIZE);
	pInfo->ucSaveFlag		= (unsigned char)bSave;
	pInfo->nSize		= sizeof(SynFuncInfo0) + _MAX_NAMESIZE;

	MapGroup(This()->m_idProcess)->QuerySynMessage()->ChangeSyndicate(pInfo);
	return true;
}

bool CSyndicate::XSynModify::SetMemberTitle(LPCTSTR szMemberTitle, bool bSave)
{
	MESSAGESTR	buf;
	SynFuncInfo0*	pInfo = (SynFuncInfo0*)&buf;
	pInfo->idSyn		= This()->GetID();
	pInfo->ucFuncType	= SYNFUNC_SETMEMBERTITLE;
	SafeCopy(pInfo->StrParam, szMemberTitle, _MAX_NAMESIZE);
	pInfo->ucSaveFlag		= (unsigned char)bSave;
	pInfo->nSize		= sizeof(SynFuncInfo0) + _MAX_NAMESIZE;

	MapGroup(This()->m_idProcess)->QuerySynMessage()->ChangeSyndicate(pInfo);
	return true;
}

bool CSyndicate::XSynModify::AddData(SYNDATA idx, int nData, bool bSave)
{
	MESSAGESTR	buf;
	SynFuncInfo0*	pInfo = (SynFuncInfo0*)&buf;
	pInfo->idSyn		= This()->GetID();
	pInfo->ucFuncType	= SYNFUNC_ADDDATA;
	pInfo->IntParam[0]	= idx;
	pInfo->IntParam[1]	= nData;
	pInfo->ucSaveFlag	= (unsigned char)bSave;
	pInfo->nSize		= sizeof(SynFuncInfo0) + sizeof(int)*2;

	MapGroup(This()->m_idProcess)->QuerySynMessage()->ChangeSyndicate(pInfo);
	return true;
}

bool CSyndicate::XSynModify::SetData(SYNDATA idx, int nData, bool bSave)
{
	MESSAGESTR	buf;
	SynFuncInfo0*	pInfo = (SynFuncInfo0*)&buf;
	pInfo->idSyn		= This()->GetID();
	pInfo->ucFuncType	= SYNFUNC_SETDATA;
	pInfo->IntParam[0]	= idx;
	pInfo->IntParam[1]	= nData;
	pInfo->ucSaveFlag	= (unsigned char)bSave;
	pInfo->nSize		= sizeof(SynFuncInfo0) + sizeof(int)*2;

	MapGroup(This()->m_idProcess)->QuerySynMessage()->ChangeSyndicate(pInfo);
	return true;
}

bool CSyndicate::XSynModify::PresentMoney(OBJID idSyn, int nMoney, bool bSave)
{
	MESSAGESTR	buf;
	SynFuncInfo0*	pInfo = (SynFuncInfo0*)&buf;
	pInfo->idSyn		= This()->GetID();
	pInfo->ucFuncType	= SYNFUNC_PRESENT;
	pInfo->IntParam[0]	= idSyn;
	pInfo->IntParam[1]	= nMoney;
	pInfo->ucSaveFlag	= (unsigned char)bSave;
	pInfo->nSize		= sizeof(SynFuncInfo0) + sizeof(int)*2;

	MapGroup(This()->m_idProcess)->QuerySynMessage()->ChangeSyndicate(pInfo);
	return true;
}

int	CSyndicate::GetInt(SYNDATA idx)
{
	OBJID idFealty = m_pData->GetInt(SYNDATA_FEALTY);
	if(idFealty != ID_NONE)
	{
		if(idx >= SYNDATA_ENEMY0 && idx < SYNDATA_ENEMY0 + MAX_SYNENEMYSIZE
				|| idx >= SYNDATA_ALLY0 && idx < SYNDATA_ALLY0 + MAX_SYNALLYSIZE)
		{
			CSyndicate* pSyn = SynManager()->QuerySyndicate(idFealty);
			IF_OK (pSyn)
				return pSyn->m_pData->GetInt(idx);
		}
	}

	return m_pData->GetInt(idx);
}

void CSyndicate::BroadcastLocalSynMsg(CNetMsg* pMsg, CUser* pExcludeSender)
{
	CHECK(pMsg);
	CUserManager::Iterator pUser = UserManager()->NewEnum();
	while(pUser.Next())
	{
		if(pUser && pUser->GetSynID() == GetID()
				&& !(pExcludeSender && pExcludeSender->GetID() == pUser->GetID()) )
		{
			pUser->SendMsg(pMsg);
		}
	}
}

void CSyndicate::SynchroInfo(CUser* pExcludeSender)
{
	CUserManager::Iterator pUser = UserManager()->NewEnum();
	while(pUser.Next())
	{
		if(pUser && pUser->GetSynID() == GetID()
				&& !(pExcludeSender && pExcludeSender->GetID() == pUser->GetID()) )
		{
			pUser->QuerySynAttr()->SendInfoToClient();
		}
	}
}

void CSyndicate::BroadcastSynMsg(CNetMsg* pMsg, CUser* pExcludeSender)
{
	CSyndicate* pMaster = this->GetMasterSyn();
	CHECK(pMaster);
	pMaster->BroadcastLocalSynMsg(pMsg, pExcludeSender);

	{
		OBJID idMaster = pMaster->GetID();
		CHECK(idMaster != ID_NONE);
		for(int i = 0; i < SynManager()->QuerySynSet()->GetAmount(); i++)
		{
			CSynPtr pSyn = SynManager()->QuerySynSet()->GetObjByIndex(i);
			if(pSyn && pSyn->GetInt(SYNDATA_FEALTY) == idMaster)
				pSyn->BroadcastLocalSynMsg(pMsg, pExcludeSender);
		}
	}
}

void CSyndicate::BroadcastSubSynMsg(SOCKET_ID idSocket, OBJID idNpc, LPCTSTR szMsg, CUser* pExcludeSender, LPCTSTR pszEmotion, DWORD dwColor, int nAttribute)
{
	CHECK(szMsg);
	CMsgTalk	msg;
	IF_OK(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, szMsg, pszEmotion, dwColor, nAttribute))
		BroadcastSynMsg(&msg, pExcludeSender);

	MapGroup(PID)->QueryIntraMsg()->TransmitMsg(&msg, idSocket, idNpc);
}

LPCTSTR CSyndicate::GetFealtyName()
{
	OBJID idFealty = GetInt(SYNDATA_FEALTY);
	if(idFealty)
	{
		CSyndicate* pSyn = SynManager()->QuerySyndicate(idFealty);
		if(pSyn)
			return pSyn->GetStr(SYNDATA_NAME);
	}

	return SYNNAME_NONE;
}

bool CSyndicate::IsNearSyn(OBJID idTarget)
{
	if(idTarget == ID_NONE)
		return false;

	CSyndicate* pTarget = SynManager()->QuerySyndicate(idTarget);
	if(!pTarget)
		return false;

	return pTarget->GetMasterSynID() == GetMasterSynID();
}

CSyndicate* CSyndicate::GetMasterSyn()
{
	if(GetInt(SYNDATA_FEALTY) != ID_NONE)
	{
		CSyndicate* pFealtySyn= SynManager()->QuerySyndicate(GetInt(SYNDATA_FEALTY));

		IF_OK(pFealtySyn)
			return pFealtySyn;
	}

	return this;
}

void CSyndicate::SendInfoToClient(CUser* pUser)
{
	CHECK(pUser);

	for(int i = 0; i < MAX_SYNENEMYSIZE; i++)
	{
		if(this->GetInt((SYNDATA)(SYNDATA_ENEMY0+i)) != ID_NONE)
		{
			CMsgSyndicate	msg;
			IF_OK(msg.Create(SET_ANTAGONIZE, this->GetInt((SYNDATA)(SYNDATA_ENEMY0+i))))
				pUser->SendMsg(&msg);
		}
	}

	for(int i = 0; i < MAX_SYNALLYSIZE; i++)
	{
		if(this->GetInt((SYNDATA)(SYNDATA_ALLY0+i)) != ID_NONE)
		{
			CMsgSyndicate	msg;
			IF_OK(msg.Create(SET_ALLY, this->GetInt((SYNDATA)(SYNDATA_ALLY0+i))))
				pUser->SendMsg(&msg);
		}
	}

	OBJID idMaster = GetID();
	if(GetInt(SYNDATA_FEALTY) != 0)
		idMaster = GetInt(SYNDATA_FEALTY);
	CHECK(idMaster != ID_NONE);
	{
		CMsgSyndicate	msg;
		IF_OK(msg.Create(SET_MAINSYN, idMaster))
			pUser->SendMsg(&msg);
	}
	for(int i = 0;  i < SynManager()->QuerySynSet()->GetAmount(); i++)
	{
		CSynPtr pSyn = SynManager()->QuerySynSet()->GetObjByIndex(i);
		if(pSyn && pSyn->GetInt(SYNDATA_FEALTY) == idMaster)
		{	CMsgSyndicate	msg;
			IF_OK(msg.Create(SET_SUBSYN, pSyn->GetID()))
				pUser->SendMsg(&msg);
		}
	}

	CSyndicate* pMaster = GetMasterSyn();
	MYASSERT(pMaster);
	LPCTSTR pTenet = pMaster->GetStr(SYNDATA_TENET);
	if(strlen(pTenet) > 0)
		pUser->SendSysMsg(_TXTATR_SYNANNOUNCE, pTenet);
}

void CSyndicate::AddSynWarScore(OBJID idNpc, int nScore)
{
	for(int i = 0; i < (int)m_setSynWarScore.size(); i++)
	{
		if(m_setSynWarScore[i].idNpc == idNpc)
		{
			m_setSynWarScore[i].nScore	+= nScore;
			return;
		}
	}

	SynWarInfo	info;
	info.idNpc		= idNpc;
	info.nScore		= nScore;
	m_setSynWarScore.push_back(info);
}

int CSyndicate::GetSynWarScore(OBJID idNpc)
{
	for(int i = 0; i < (int)m_setSynWarScore.size(); i++)
	{
		if(m_setSynWarScore[i].idNpc == idNpc)
			return m_setSynWarScore[i].nScore;
	}
	return 0;
}

void CSyndicate::ClearSynWarScore(OBJID idNpc)
{
	for(int i = 0; i < (int)m_setSynWarScore.size(); i++)
	{
		if(m_setSynWarScore[i].idNpc == idNpc)
		{
			m_setSynWarScore.erase(m_setSynWarScore.begin() + i);
			return ;
		}
	}
}

void CSyndicate::ClearAllRelation()
{
	for(int i = SYNDATA_ENEMY0; i < SYNDATA_ENEMY0 + MAX_SYNENEMYSIZE; i++)
	{
		CSyndicate* pSyn = SynManager()->QuerySyndicate(GetInt((SYNDATA)i));
		if(pSyn)
			pSyn->ClearRelation(GetID());
	}
	for(int i = SYNDATA_ALLY0; i < SYNDATA_ALLY0 + MAX_SYNALLYSIZE; i++)
	{
		CSyndicate* pSyn = SynManager()->QuerySyndicate(GetInt((SYNDATA)i));
		if(pSyn)
			pSyn->ClearRelation(GetID());
	}
}

void CSyndicate::ClearRelation(OBJID idSyn)
{
	for(int i = SYNDATA_ENEMY0; i < SYNDATA_ENEMY0 + MAX_SYNENEMYSIZE; i++)
	{
		if(GetInt((SYNDATA)i) == idSyn)
			SetData((SYNDATA)i, ID_NONE, false);
	}
	for(int i = SYNDATA_ALLY0; i < SYNDATA_ALLY0 + MAX_SYNALLYSIZE; i++)
	{
		if(GetInt((SYNDATA)i) == idSyn)
			SetData((SYNDATA)i, ID_NONE, false);
	}
	if(m_pData)
		m_pData->Update();
}

bool CSyndicate::IsFriendly(OBJID idTarget, BOOL bNest)
{
	if(idTarget == ID_NONE)
		return false;
	CSyndicate* pTarget = SynManager()->QuerySyndicate(idTarget);
	CHECKF(pTarget);
	idTarget = pTarget->GetMasterSynID();

	if(bNest && GetInt(SYNDATA_FEALTY) != ID_NONE)
	{
		CSyndicate* pSyn = SynManager()->QuerySyndicate(GetInt(SYNDATA_FEALTY));
		IF_OK(pSyn)
			return pSyn->IsFriendly(idTarget);
	}

	for(int i = SYNDATA_ALLY0; i < SYNDATA_ALLY0 + MAX_SYNALLYSIZE; i++)
	{
		if(GetInt((SYNDATA)i) == idTarget)
			return true;
	}

	return false;
}

bool CSyndicate::IsHostile(OBJID idTarget, BOOL bNest)
{
	CSyndicate* pTarget = SynManager()->QuerySyndicate(idTarget);
	idTarget = pTarget->GetMasterSynID();

	if(bNest && GetInt(SYNDATA_FEALTY) != ID_NONE)
	{
		CSyndicate* pSyn = SynManager()->QuerySyndicate(GetInt(SYNDATA_FEALTY));
		IF_OK(pSyn)
			return pSyn->IsHostile(idTarget);
	}

	for(int i = SYNDATA_ENEMY0; i < SYNDATA_ENEMY0 + MAX_SYNENEMYSIZE; i++)
	{
		if(GetInt((SYNDATA)i) == idTarget)
			return true;
	}

	return false;
}

int CSyndicate::GetMemberAmount()
{
	int nMemberSum	= GetInt(SYNDATA_AMOUNT);

	if (GetInt(SYNDATA_FEALTY) == ID_NONE)
	{
		int nAmount = SynManager()->QuerySynSet()->GetAmount();
		for(int i = 0; i < nAmount; i++)
		{
			CSyndicate* pSyn = SynManager()->QuerySynSet()->GetObjByIndex(i);
			if(pSyn && pSyn->GetInt(SYNDATA_FEALTY) == this->GetID())
			{
				nMemberSum += pSyn->GetInt(SYNDATA_AMOUNT);
			}
		}
	}

	return nMemberSum;
}

void CSyndicate::SetNewSubName(LPCTSTR szName)
{
	m_pData->SetStr(SYNDATA_NAME, szName, _MAX_NAMESIZE);
	m_pData->Update();
}

bool CSyndicate::IsWhiteSynMapOwner()
{
	return GetID() == SynManager()->GetMapSynID(SET_WHITE_SYN);
}

bool CSyndicate::IsBlackSynMapOwner()
{
	return GetID() == SynManager()->GetMapSynID(SET_BLACK_SYN);
}
