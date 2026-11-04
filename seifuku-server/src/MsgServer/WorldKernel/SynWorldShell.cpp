#define	_WINSOCKAPI_
#include "AllMsg.h"
#include "I_MessagePort.h"
#include "define.h"
#include "MessageBoard.h"
#include "UserList.h"
#include "SynWorldShell.h"
#include "WorldKernel.h"

const int	SYNMONEY_LIMIT		= 2000000000;
const int	MEMBERLIST_SIZE		= 10;
const int	MEMBERLIST_LIMIT	= 1234567890;

MYHEAP_IMPLEMENTATION(CSyndicateWorld,s_heap)
CSyndicateWorld::CSyndicateWorld()
{
	m_pData	= NULL;
	m_ptrMsgBd	= CMessageBoard::CreateNew(_TXTATR_MSG_SYN);
}

CSyndicateWorld::~CSyndicateWorld()
{
	if(m_pData)
	{
		m_pData->Update();
		m_pData->Release();
	}
}

bool CSyndicateWorld::Create(IRecordset* pRes)
{
	MYASSERT(!m_pData);

	m_pData = CSynWorldBase::CreateNew();
	CHECKF(m_pData);
	IF_NOT(m_pData->Create(pRes))
		return false;

	if(m_pData->GetInt(SYNDATA_DEL_FLAG) != 0)
		return false;

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT COUNT(*) FROM cq_synattr as s LEFT JOIN cq_user AS u ON u.id=s.id WHERE s.syn_id=%u && u.id IS NOT NULL", m_pData->GetID());
	IRecordset* pStatRes = GameWorld()->GetDatabase()->CreateNewRecordset(szSQL);
	if(pStatRes)
	{
		m_pData->SetInt(SYNDATA_AMOUNT, pStatRes->GetInt(0));
		m_pData->Update();
		pStatRes->Release();
	}

	sprintf(szSQL, "SELECT u.name,u.level FROM cq_synattr as s LEFT JOIN cq_user AS u ON u.id=s.id WHERE s.syn_id=%u && u.id IS NOT NULL ORDER BY s.rank DESC, u.level DESC LIMIT %d", m_pData->GetID(), MEMBERLIST_LIMIT);
	 pStatRes = GameWorld()->GetDatabase()->CreateNewRecordset(szSQL);
	if(pStatRes)
	{
		for(int i = 0; i < (int)pStatRes->RecordCount(); i++,pStatRes->MoveNext())
		{
			m_setMember.push_back(MemberInfo(pStatRes->GetStr(0), pStatRes->GetInt(1)));
		}
		pStatRes->Release();
	}

	return true;
}

bool CSyndicateWorld::Create(IRecordset* pDefault, const CreateSyndicateInfo* pInfo, bool bSave)
{
	MYASSERT(!m_pData);

	OBJID idSyn = bSave ? ID_NONE : pInfo->idSyn;
	m_pData = CSynWorldBase::CreateNew();
	CHECKF(m_pData);
	IF_NOT(m_pData->Create(pDefault, idSyn))
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

bool CSyndicateWorld::Demise(OBJID idOldLeader, OBJID idNewLeader, LPCTSTR szNewLeader, OBJID idBackSyn, bool bSave)
{
	if(!( idOldLeader==(OBJID)m_pData->GetInt(SYNDATA_LEADERID) || (OBJID)m_pData->GetInt(SYNDATA_LEADERID)==ID_NONE ))
		return false;
	NAMESTR	szOldLeader;
	SafeCopy(szOldLeader, m_pData->GetStr(SYNDATA_LEADERNAME), _MAX_NAMESIZE);
	m_pData->SetInt(SYNDATA_LEADERID, idNewLeader);
	m_pData->SetStr(SYNDATA_LEADERNAME, szNewLeader, _MAX_NAMESIZE);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();

	int	RANK_NEWBIE		= 50;
	int RANK_LEADER		=100;
	SQLBUF	szSQL;
	sprintf(szSQL, "UPDATE cq_synattr SET syn_id=%u,rank=%d WHERE id=%u LIMIT 1", idBackSyn, RANK_NEWBIE, idOldLeader);
	GameWorld()->GetDatabase()->ExecuteSQL(szSQL);

	if(GetID() != idBackSyn)
	{
		CSyndicateWorld * pBackSyn = SynWorldManager()->QuerySyndicate(idBackSyn);
		IF_OK(pBackSyn)
		{
			int	nLevel = 0;
			DelMember(szOldLeader, &nLevel);
			pBackSyn->AddMember(szOldLeader, nLevel);
			pBackSyn->DelMember(szNewLeader, &nLevel);
			AddMember(szNewLeader, nLevel);
		}
	}

	return true;
}

bool CSyndicateWorld::SetTenet(LPCTSTR szTenet, bool bSave)
{
	CHECKF(strlen(szTenet)<MAX_TENETSIZE);

	m_pData->SetStr(SYNDATA_TENET, szTenet, MAX_TENETSIZE);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();
	return true;
}

bool CSyndicateWorld::SetLeaderTitle(LPCTSTR szLeaderTitle, bool bSave)
{
	m_pData->SetStr(SYNDATA_ANNOUNCE, szLeaderTitle, _MAX_NAMESIZE);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();
	return true;
}

bool CSyndicateWorld::SetMemberTitle(LPCTSTR szMemberTitle, bool bSave)
{
	m_pData->SetStr(SYNDATA_MEMBERTITLE, szMemberTitle, _MAX_NAMESIZE);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();
	return true;
}

bool CSyndicateWorld::AddData(SYNDATA idx, int nData, bool bSave)
{
	int nAllData = ::AddToTop(m_pData->GetInt(idx), nData, SYNMONEY_LIMIT);
	m_pData->SetInt(idx, nAllData);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();
	return true;
}

bool CSyndicateWorld::SetData(SYNDATA idx, int nData, bool bSave)
{
	m_pData->SetInt(idx, nData);
	if(bSave)
		m_pData->Update();
	else
		m_pData->ClearUpdateFlags();
	return true;
}

bool CSyndicateWorld::PresentMoney(OBJID idSyn, int nMoney, bool bSave)
{
	if(nMoney <= 0 || m_pData->GetInt(SYNDATA_MONEY) < nMoney)
		return false;
	CSyndicateWorld* pTarget = GameWorld()->QuerySynManager()->QuerySyndicate(idSyn);
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
	::GmLogSave("EVENT: syndicate [%s] transferred funds to syndicate [%s]: [%d] silver.", GetStr(SYNDATA_NAME), pTarget->GetStr(SYNDATA_NAME), nMoney);
	return true;
}

bool CSyndicateWorld::ChangeSyndicate(const SynFuncInfo0* pFuncInfo)
{
	bool ret = false;
	switch(pFuncInfo->ucFuncType)
	{
	case	SYNFUNC_DEMISE:
		ret = Demise(pFuncInfo->IntParam[0], pFuncInfo->IntParam[1], (LPCTSTR)&pFuncInfo->IntParam[3], pFuncInfo->IntParam[2], pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_SETTENET:
		ret = SetTenet(pFuncInfo->StrParam, pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_SETLEADERTITLE:
		ret = SetLeaderTitle(pFuncInfo->StrParam, pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_SETMEMBERTITLE:
		ret = SetMemberTitle(pFuncInfo->StrParam, pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_ADDDATA:
		ret = AddData((SYNDATA)pFuncInfo->IntParam[0], pFuncInfo->IntParam[1], pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_SETDATA:
		ret = SetData((SYNDATA)pFuncInfo->IntParam[0], pFuncInfo->IntParam[1], pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_PRESENT:
		ret = PresentMoney(pFuncInfo->IntParam[0], pFuncInfo->IntParam[1], pFuncInfo->ucSaveFlag!=0);
		break;
	case	SYNFUNC_ADD_MEMBER:
		ret = AddMember((LPCTSTR)&pFuncInfo->IntParam[1], pFuncInfo->IntParam[0]);
		return ret;
		break;
	case	SYNFUNC_DEL_MEMBER:
		ret = DelMember(pFuncInfo->StrParam);
		return ret;
		break;
	case	SYNFUNC_UP_LEVEL:
		ret = SetMemberLevel((LPCTSTR)&pFuncInfo->IntParam[1], pFuncInfo->IntParam[0]);
		return ret;
		break;
	default:
		MYASSERT(!"switch CSynShell::ChangeSyndicate()");
		return false;
		break;
	}

	if(ret)
	{
		MESSAGESTR	buf;
		SynFuncInfo0*	pInfo = (SynFuncInfo0*)buf;
		memcpy(buf, pFuncInfo, MAX_MESSAGESIZE);
		pInfo->ucSaveFlag	= (unsigned char)false;
		GameWorld()->QuerySynMessage()->ChangeSyndicate(pInfo);
	}

	return ret;
}

bool CSyndicateWorld::AddMember(LPCTSTR szName, int nUserRank)
{
	CHECKF(szName);
	MYASSERT(!DelMember(szName));

	m_setMember.push_back(MemberInfo(szName, nUserRank));
	return true;
}

bool CSyndicateWorld::DelMember(LPCTSTR szName, int* pLevel)
{
	CHECKF(szName);

	for(int i = m_setMember.size()-1; i >= 0; i--)
	{
		if(strcmp(m_setMember[i].szName.c_str(), szName) == 0)
		{
			if(pLevel)
				*pLevel = m_setMember[i].nRank;
			m_setMember.erase(m_setMember.begin() + i);
			return true;
		}
	}

	return false;
}

bool CSyndicateWorld::SetMemberLevel(LPCTSTR szName, int nLevel)
{
	for(int i = m_setMember.size()-1; i >= 0; i--)
	{
		if(strcmp(m_setMember[i].szName.c_str(), szName) == 0)
		{
			m_setMember[i].nRank = nLevel;
			break;
		}
	}

	return true;
}

int CSyndicateWorld::AppendOnlineMember(class CMsgName& msg, int idx)
{
	int	nAmount = m_setMember.size();
	int nCount	= 0;
	for(int i = idx; i < nAmount && nCount < MEMBERLIST_SIZE; i++)
	{
		bool	bOnline = (UserList()->GetPlayer(m_setMember[i].szName.c_str()) != NULL);
		if(bOnline)
		{
			char	szMsg[256];
			sprintf(szMsg, "%s %d %d", m_setMember[i].szName.c_str(), m_setMember[i].nRank, bOnline);

			CHECKF(msg.AppendString(szMsg, i));
			nCount++;
		}
	}
	if (nCount < MEMBERLIST_SIZE)
	{
	}
	return nCount;
}
void CSyndicateWorld::AppendOfflineMember(class CMsgName& msg, int idx, int nCount)
{
	int	nAmount = m_setMember.size();
	for(int i = idx; i < nAmount && nCount < MEMBERLIST_SIZE; i++)
	{
		bool	bOnline = (UserList()->GetPlayer(m_setMember[i].szName.c_str()) != NULL);
		if(!bOnline)
		{
			char	szMsg[256];
			sprintf(szMsg, "%s %d %d", m_setMember[i].szName.c_str(), m_setMember[i].nRank, bOnline);
			CHECK(msg.AppendString(szMsg, i));
			nCount++;
		}
	}
}

void CSyndicateWorld::SendMemberList(CPlayer *pUser, int idxLast)
{
	if(idxLast < -1 || idxLast >= static_cast<int>(m_setMember.size()))
		return ;

	CMsgName	msg;
	CHECK(msg.CreateHead(NAMEACT_MEMBERLIST, idxLast));

	bool	bOnline = true;
	int nCount = 0;

	if(idxLast != -1)
		bOnline = (UserList()->GetPlayer(m_setMember[idxLast].szName.c_str()) != NULL);

	int idx	= idxLast + 1;
	if(bOnline)
	{
		nCount	= AppendOnlineMember(msg, idx);
		if(nCount < MEMBERLIST_SIZE)
			AppendOfflineMember(msg, 0, nCount);
	}
	else
	{
		AppendOfflineMember(msg, idx);
	}

	pUser->SendMsg(&msg);
}

void CSyndicateWorld::CombineMemberList(const MEMBER_SET& setMember)
{
	if(setMember.size())
		m_setMember.insert(m_setMember.end(), setMember.begin(), setMember.end());
}
