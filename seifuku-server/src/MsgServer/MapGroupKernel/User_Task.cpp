#include "AllMsg.h"
#include "User.h"
#include "MapGroup.h"
#include "netmsg.h"
#include "ItemType.h"
#include "MapItem.h"
#include "Package.h"
#include "ActionDefine.h"
#include "Syndicate.h"
#include "npc.h"
#include "WantedList.h"

const int VAL_SKIPTEST	=999;
const int	PRESENTMONEY_LIMIT		= 10000;

bool CUser::TestTask(CTaskData* pTask)
{
	CHECKF(pTask);

	try
	{
		if(!CheckItem(pTask))
			return false;

		if ((int)this->GetMoney() < pTask->GetInt(TASKDATA_MONEY))
			return false;

		if (pTask->GetInt(TASKDATA_PROFESSION) && this->GetProfession() != pTask->GetInt(TASKDATA_PROFESSION))
			return false;

		if (this->GetPk() < pTask->GetInt(TASKDATA_MINPK) || this->GetPk() > pTask->GetInt(TASKDATA_MAXPK))
			return false;

		return true;
	}
	catch(...)
	{
		ASSERT(!"catch");
		::LogSave("exception catched In CGameTask::TestUser.");
		return false;
	}
}

bool CUser::CheckItem(CTaskData* pTask)
{
	if(strlen(pTask->GetStr(TASKDATA_ITEMNAME1)) > 0)
	{
		if(!this->GetTaskItem(pTask->GetStr(TASKDATA_ITEMNAME1)))
			return false;

		if(strlen(pTask->GetStr(TASKDATA_ITEMNAME2)) > 0)
		{
			if(!this->GetTaskItem(pTask->GetStr(TASKDATA_ITEMNAME2)))
				return false;
		}
	}

	return true;
}

bool CUser::ProcessClientTask(OBJID idTask, LPCTSTR pszAccept)
{
	CTaskData* pTask = TaskSet()->GetObj(idTask);
	CHECKF(pTask && pTask->GetInt(TASKDATA_CLIENTACT));

	if (!this->TestTask(pTask))
	{
		OBJID idNext	= pTask->GetInt(TASKDATA_IDNEXTFAIL);
		if (idNext != ID_NONE)
		{
			GameAction()->ProcessAction(idNext, this, NULL, NULL, pszAccept);
			return true;
		}

		this->SendSysMsg(STR_UNQUALIFIED);
		return false;
	}

	OBJID idNext	= pTask->GetInt(TASKDATA_IDNEXT);
	if (idNext != ID_NONE)
		GameAction()->ProcessAction(idNext, this, NULL, NULL, pszAccept);

	return true;
}

int CUser::PushTaskID(OBJID idTask, OBJID idIter)
{
	if(idTask != ID_NONE && m_setTaskID.size() < MAX_MENUTASKSIZE)
	{
		m_setTaskID.push_back(idTask);
		m_setIterID.push_back(idIter);
		return m_setTaskID.size()-1;
	}
	return INDEX_NONE;
}

bool CUser::ProcessTask(int idx, LPCTSTR pszAccept)
{
	OBJID	idTask = GetTaskID(idx);
	if (idTask == ID_NONE)
		return false;

	CTaskData* pTask = TaskSet()->GetObj(idTask);
	if (!pTask)
	{
		LOGERROR("Don't find ACTION ID in menu: %d", idTask);
		return false;
	}

	CNpc* pNpc = NULL;
	if (!pTask->GetInt(TASKDATA_CLIENTACT))
	{
		if(m_idTaskNpc == ID_NONE)
			return false;
		if(!GetMap()->QueryObj(GetPosX(), GetPosY(), OBJ_NPC, m_idTaskNpc, IPP_OF(pNpc)))
			return false;
		IF_NOT(pNpc)
			return false;
	}

	SetIterID(idx);
	ClearTaskID();

	if (!this->TestTask(pTask))
	{
		OBJID idNext	= pTask->GetInt(TASKDATA_IDNEXTFAIL);
		if (idNext != ID_NONE)
		{
			GameAction()->ProcessAction(idNext, this, pNpc ? pNpc->QueryRole() : NULL, NULL, pszAccept);
			return true;
		}

		this->SendSysMsg(STR_UNQUALIFIED);
		return false;
	}

	OBJID idNext	= pTask->GetInt(TASKDATA_IDNEXT);
	if (idNext != ID_NONE)
		GameAction()->ProcessAction(idNext, this, pNpc ? pNpc->QueryRole() : NULL, NULL, pszAccept);

	return true;
}

void CUser::AddTaskMask		(int idx)
{
	IF_NOT (idx >= 0 && idx < 32)
		return;

	DWORD dwMask = m_data.GetTaskMask()|(1<<idx);
	m_data.SetTaskMask(dwMask);
}

void CUser::ClrTaskMask		(int idx)
{
	IF_NOT (idx >= 0 && idx < 32)
		return;

	DWORD dwMask = m_data.GetTaskMask();
	dwMask	&= ~(1<<idx);
	m_data.SetTaskMask(dwMask);
}

bool CUser::ChkTaskMask		(int idx)
{
	IF_NOT (idx >= 0 && idx < 32)
		return false;

	return (m_data.GetTaskMask() & (1<<idx)) != 0;
}
