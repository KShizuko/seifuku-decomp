#include "windows.h"
#include "define.h"
#include "common.h"
#include "Agent.h"
#include "Mapgroup.h"

const int	_MAX_USERITEMSIZE	= 40;

MYHEAP_IMPLEMENTATION(CAgent,s_heap)
CAgent::CAgent()
{

}

CAgent::~CAgent()
{

}

bool CAgent::Create(PROCESS_ID idProcess, OBJID idAgent)
{
	return CUser::Create(idProcess, SOCKET_NONE, idAgent);
}

bool CAgent::SendMsg(CNetMsg* pMsg)
{
	return MapGroup(PID)->QueryIntraMsg()->SendNpcMsg(GetID(), pMsg);
}

bool CAgent::FullItem(int nItemType)
{

	for(int i = 0; i < _MAX_USERITEMSIZE; i++)
	{
		if(this->IsBackPackFull())
			return true;
		else
			this->AwardItem(nItemType, SYNCHRO_TRUE);
	}
	return true;
}
