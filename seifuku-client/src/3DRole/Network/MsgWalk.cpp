#include "AllMsg.h"
#include "3DGameMap.h"
#include "GamePlayerSet.h"
#include "Player.h"
#include "Hero.h"
#include "GameMsg.h"
#include "Pet.h"

CMsgWalk::CMsgWalk()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgWalk::~CMsgWalk()
{

}

BOOL CMsgWalk::Create(OBJID idPlayer, int nDir, int nScondDir )
{
	if (idPlayer == ID_NONE)
		return false;
	this->Init();

	m_unMsgSize	=sizeof(MSG_Info);
	m_unMsgType	=_MSG_WALK;

	m_pInfo->idUser		=idPlayer;
	m_pInfo->ucDir		=nDir;
	m_pInfo->ucRun		=nScondDir;

	return true;
}

BOOL CMsgWalk::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;
	if(_MSG_WALK != this->GetType())
		return false;

	return true;
}

void CMsgWalk::Send(void)
{
	if (m_pInfo->idUser == g_objHero.GetID())
	{
		int nDir = m_pInfo->ucDir % 8;
		g_objHero.m_objCheckKeys.SetKey(m_pInfo->ucRun ? nDir + 8 : nDir, nDir);
	}
	CNetMsg::Send();
}

void CMsgWalk::Process(void *pInfo)
{
#ifdef _MSGDEBUG
	::LogMsg("Process CMsgWalk, idUser:%u, dir:%u",
		m_pInfo->idUser,
		m_pInfo->ucDir);
#endif
	m_pInfo->ucDir %= 8;
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_pInfo->idUser);
	if (!pPlayer)
	{
		if (m_pInfo->idUser == g_objHero.GetID())
		{
			CMyPos posHero	=g_objHero.GetAlignPos();
			g_objHero.SetAlignPos(posHero.x+_DELTA_X[m_pInfo->ucDir], posHero.y+_DELTA_Y[m_pInfo->ucDir]);
		}
		else
		{
			CMsgAction msg;
			if (msg.Create(g_objHero.GetID(), 0, 0, g_objHero.GetDir(), actionQueryPlayer, m_pInfo->idUser))
				msg.Send();
			if (strstr(g_objHero.GetName(), "PM"))
				g_objGameMsg.AddMsg("QueryPlayer: %u", m_pInfo->idUser);
		}
		return;
	}
	if(pPlayer->IsMyPet())
	{
		CPet* pPet = (CPet*)pPlayer;
		CMyPos posAlign;
		pPet->GetAlignPos(posAlign);
		pPet->SetAlignPos(posAlign.x+_DELTA_X[m_pInfo->ucDir], posAlign.y+_DELTA_Y[m_pInfo->ucDir]);
		return;
	}
	CCommand cmd;
	cmd.iType		=m_pInfo->ucRun ? _COMMAND_RUNFORWARD : _COMMAND_WALKFORWARD;
	cmd.iStatus		=_CMDSTATUS_BEGIN;
	cmd.nDir		=m_pInfo->ucDir;
	pPlayer->SetCommand(&cmd);
}
