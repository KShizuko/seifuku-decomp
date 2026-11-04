#include "AllMsg.h"
#include "SynAttr.h"
#include "MapGroup.h"

CMsgSynMemberInfo::CMsgSynMemberInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgSynMemberInfo::~CMsgSynMemberInfo()
{

}

BOOL CMsgSynMemberInfo::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_SYNMEMBERINFO != this->GetType())
		return false;

	return true;
}

BOOL CMsgSynMemberInfo::Create(LPCTSTR szName, int nRankShow, int nProffer)
{
	CHECKF(szName);

	this->Init();

	SafeCopy(m_pInfo->szName, szName, _MAX_NAMESIZE);
	m_pInfo->ucRank			= nRankShow;
	m_pInfo->nProffer		= nProffer;

	m_unMsgType	=_MSG_SYNMEMBERINFO;
	m_unMsgSize	=sizeof(MSG_Info);
	
	return true;
}

void CMsgSynMemberInfo::Process(void *pInfo)
{
	CUser* pUser = UserManager()->GetUser(this);
	if(!pUser)
	{
		CUser* pTargetUser = UserManager()->GetUser(m_pInfo->szName);
		if(pTargetUser)
		{
			CHECK(pTargetUser->GetSynID() != ID_NONE);

			IF_OK(Create(pTargetUser->GetName(), pTargetUser->QuerySynAttr()->GetSynRankShow(), pTargetUser->QuerySynAttr()->GetInt(SYNATTRDATA_PROFFER)))
				SendMsg(this);
		}
		return ;
	}

	if(pUser->GetSynID() == ID_NONE)
		return ;

	CUser* pTargetUser = UserManager()->GetUser(m_pInfo->szName);
	if(pTargetUser)
	{
		CHECK(pTargetUser->GetSynID() != ID_NONE);

		IF_OK(Create(pTargetUser->GetName(), pTargetUser->QuerySynAttr()->GetSynRankShow(), pTargetUser->QuerySynAttr()->GetInt(SYNATTRDATA_PROFFER)))
			SendMsg(this);
	}
	else
	{
		MapGroup(PID)->QueryIntraMsg()->TransmitMsg(this, GetSocketID(), GetNpcID());
	}
}
