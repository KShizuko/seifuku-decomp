#include "Hero.h"
#include "AllMsg.h"
#include "3DGameMap.h"
#include "GamePlayerSet.h"

void CHero::DestroyTaskDialog()
{
	m_objTaskDialog.Destroy();	
}
int CHero::GetTaskDialogLineAmount()
{
	return m_objTaskDialog.GetLineAmount();
}
TaskDialogLine*	CHero::GetTaskDialogLineByIndex(int nIndex)
{
	return m_objTaskDialog.GetLineByIndex(nIndex);
}
void CHero::AddTaskDialogLine(int nType, DWORD dwData, int nIndex, char* szStr, CMyPos posLine)
{
	m_objTaskDialog.AddLine(nType, dwData, nIndex, szStr, posLine);
}
void CHero::AnswerTaskDialog(int nIndex, char* pszEdit)
{
	CMsgTaskDialog msg;
	if(msg.Create(nIndex, pszEdit))
		msg.Send();

}
void CHero::CreateTaskDialog()
{
	m_objTaskDialog.Create();
}
void CHero::ActiveNpc(OBJID idNpc)
{
	if (idNpc == ID_NONE)
		return;
	CMsgNpc msg;
	if (msg.Create(EVENT_BEACTIVED, idNpc))
		msg.Send();
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(idNpc);
	if(pPlayer)
	{
		int nType = pPlayer->GetRoleType();
		switch(nType)
		{
		case _ROLE_STATUARY_NPC:
		case _ROLE_SYNFLAG_NPC:
		case _ROLE_BOOTH_NPC:
		case _ROLE_WEAPONGOAL_NPC:
		case _ROLE_MAGICGOAL_NPC:
		case _ROLE_BOWGOAL_NPC:
			break;
		default:
			{
				int nDir = pPlayer->GetDir(this);
				pPlayer->m_pIRoleView->SetVariable(_VAR_DIRECT_DIR, nDir);
			}
			break;
		}
	}
}
void CHero::AddTaskDialogLine(TaskDialogLine* pLine)
{
	m_objTaskDialog.AddLine(pLine);
}
void CHero::ActiveAction(OBJID idAction, char* pszStr)
{
	CMsgTaskDialog msg;
	if(msg.Create(0, pszStr, idAction, MSGDIALOG_TASKID))
		msg.Send();
}
void CHero::BeginTaskKillCount()
{
	m_bTaskKillCount = true;
	m_nKillCountPending = 0;
	m_nTaskKillCount = 0;
	m_pKillCounter->SetEffectPrefix("CountT");
	m_pKillCounter->Add(m_nTaskKillCount);
}
void CHero::EndTaskKillCount()
{
	m_bTaskKillCount = false;
	m_nTaskKillCount = 0;
	m_pKillCounter->SetEffectPrefix("CountB");
	if (m_nKillCountPending != 0)
	{
		for (DWORD i = 0; i < (DWORD)m_nKillCountPending; i++)
		{
			m_nKillCount++;
			m_pKillCounter->Add(m_nKillCount);
		}
	}
	else
		m_pKillCounter->Add(m_nKillCount);
	m_nKillCountPending = 0;
}
void CHero::SetTaskKillCount(int nCount)
{
	m_nTaskKillCount = nCount;
	m_pKillCounter->Add(m_nTaskKillCount);
}
void CHero::SetKillCountPending(int nCount)
{
	m_nKillCountPending = nCount;
}
