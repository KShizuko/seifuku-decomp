#include "AllMsg.h"
#include "SynAttr.h"
#include "MapGroup.h"
#pragma	warning(disable:4786)

CMsgSynAttrInfo::CMsgSynAttrInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgSynAttrInfo::~CMsgSynAttrInfo()
{

}

BOOL CMsgSynAttrInfo::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_SYNATTRINFO != this->GetType())
		return false;

	return true;
}

BOOL CMsgSynAttrInfo::Create(SynAttrInfoStruct* pInfo, CSyndicate* pSyn)
{
	this->Init();

	m_pInfo->idSyn			= pInfo->idSyn;
	m_pInfo->ucRankShow		= pInfo->ucRankShow;
	m_pInfo->nProffer		= pInfo->nProffer;

	if(pSyn)
	{
		m_pInfo->dwSyndicateFund	= pSyn->GetInt(SYNDATA_MONEY);
		m_pInfo->dwSyndicatePopulation	= pSyn->GetMemberAmount();
		SafeCopy(m_pInfo->szLeader, pSyn->GetStr(SYNDATA_LEADERNAME), _MAX_NAMESIZE);
	}
	else
	{
		m_pInfo->dwSyndicateFund		= 0;
		m_pInfo->dwSyndicatePopulation	= 0;
		m_pInfo->szLeader[0]			= 0;
	}

	m_unMsgType	=_MSG_SYNATTRINFO;
	m_unMsgSize	=sizeof(MSG_Info);

	return true;
}

void CMsgSynAttrInfo::Process(void *pInfo)
{
	ASSERT(!"CMsgSynAttrInfo::Process()");
	return;

#ifdef _MSGDEBUG
	::LogMsg("Process CMsgSynAttrInfo, id:%u", m_info.id);
#endif
}
