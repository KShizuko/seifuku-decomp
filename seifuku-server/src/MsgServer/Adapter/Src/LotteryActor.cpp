#include "LotteryActor.h"
#include "LotteryPrize.h"
#include "MapGroup.h"
#include "NetMsg.h"

ILotteryActor* LotteryActorCreate(CUser* pUser, PROCESS_ID idProcess)
{
	if (!idProcess)
		return NULL;

	CLotteryActor* pActor = new CLotteryActor(pUser, idProcess);
	IF_NOT(pActor)
		return NULL;
	return pActor;
}

CLotteryActor::CLotteryActor(CUser* pUser, PROCESS_ID idProcess)
{
	m_pUser		= pUser;
	m_idProcess	= idProcess;
}

void CLotteryActor::SetBigPrizeFails(int nData, bool bUpdate)
{
	m_pUser->SetBigPrizeFails(nData, bUpdate);
}

void CLotteryActor::SetSmallPrizeFails(int nData, bool bUpdate)
{
	m_pUser->SetSmallPrizeFails(nData, bUpdate);
}

ILotteryPrize* CLotteryActor::AwardItem(int nItemType)
{
	CItem* pItem = m_pUser->AwardItem(nItemType, true);
	IF_NOT(pItem)
		return NULL;

	ILotteryPrize* pPrize = LotteryPrizeCreate(pItem, m_idProcess);
	IF_NOT(pPrize)
		return NULL;
	return pPrize;
}

bool CLotteryActor::SendMsg(PLUGIN_MSG* pMsg)
{
	CNetMsg* pNetMsg = CNetMsg::CreateClientMsg(m_idProcess, m_pUser->GetSocketID(), pMsg->unMsgType, pMsg->bufBody, pMsg->unMsgSize, 0);
	IF_NOT(pNetMsg)
		return false;

	bool bRet = MapGroup(m_idProcess)->QueryIntraMsg()->SendClientMsg(m_pUser->GetSocketID(), pNetMsg);
	SAFE_DELETE(pNetMsg);
	return bRet;
}

bool CLotteryActor::SendSysMsg(const char* fmt, ...)
{
	char szMsg[1024];
	vsprintf(szMsg, fmt, (char*) ((&fmt)+1));

	return m_pUser->SendSysMsg(szMsg);
}

bool CLotteryActor::BroadcastRoomMsg(PLUGIN_MSG* pMsg)
{
	CNetMsg* pNetMsg = CNetMsg::CreateClientMsg(m_idProcess, m_pUser->GetSocketID(), pMsg->unMsgType, pMsg->bufBody, pMsg->unMsgSize, 0);
	IF_NOT(pNetMsg)
		return false;

	m_pUser->BroadcastRoomMsg(pNetMsg, true);
	SAFE_DELETE(pNetMsg);
	return true;
}

bool CLotteryActor::BroadcastMsg(const char* fmt, ...)
{
	char szMsg[1024];
	vsprintf(szMsg, fmt, (char*) ((&fmt)+1));

	_RoleManager(m_idProcess)->BroadcastMsg(szMsg, NULL);
	return true;
}
