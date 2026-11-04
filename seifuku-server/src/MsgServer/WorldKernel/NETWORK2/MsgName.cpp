#include "AllMsg.h"
#include "WorldKernel.h"
using namespace world_kernel;
#include "userlist.h"

CMsgName::CMsgName()
{
	Init();
	m_pInfo	=(MSG_Info* )m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
}

CMsgName::~CMsgName()
{

}

BOOL CMsgName::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_NAME != this->GetType())
		return false;

	return true;
}

BOOL CMsgName::Create(int nType, const char* pszName, DWORD dwData )
{
	if (nType == NAMEACT_NONE || !pszName)
		return false;

	this->Init();

	m_pInfo->ucType	= nType;
	m_pInfo->dwData	= dwData;

	m_StrPacker.AddString(pszName);

	m_unMsgType	=_MSG_NAME;
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

BOOL CMsgName::CreateHead(int nType, DWORD dwData)
{
	if (nType == NAMEACT_NONE)
		return false;

	this->Init();

	m_pInfo->ucType	= nType;
	m_pInfo->dwData	= dwData;

	m_unMsgType	=_MSG_NAME;
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

BOOL CMsgName::AppendString(const char* pString, DWORD dwData)
{
	if (!pString)
		return false;

	m_pInfo->dwData	= dwData;
	m_StrPacker.AddString(pString);
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

void CMsgName::Process(void *pInfo)
{
	CPlayer* pUser = UserList()->GetPlayerBySocket(GetSocketID());
	if(!pUser) return;
	char szName[_MAX_WORDSSIZE];
	m_StrPacker.GetString(0, szName, sizeof(szName));
	switch(m_pInfo->ucType)
	{
	case	NAMEACT_MEMBERLIST:
		{
			CSyndicateWorld* pSyn = GameWorld()->QuerySynManager()->QuerySyndicate(this->GetTransData());
			CHECK(pSyn);
			pSyn->SendMemberList(pUser, m_pInfo->dwData);
		}
		break;
	}
}
