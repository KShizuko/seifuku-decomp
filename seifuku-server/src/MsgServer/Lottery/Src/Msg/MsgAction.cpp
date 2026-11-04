#include "MsgAction.h"
#include "common.h"
#include <mmsystem.h>

CMsgAction::CMsgAction()
{
	Init();
	m_pInfo	= (MSG_Info*)m_bufMsg;
}

bool CMsgAction::Create(OBJID idUser, int nPosX, int nPosY, USHORT usDir, USHORT usAction, DWORD dwData)
{
	IF_NOT(usAction != ID_NONE)
		return false;
	if (idUser == ID_NONE)
		return false;

	this->Init();
	m_pInfo->unMsgSize		= sizeof(MSG_Info);
	m_pInfo->unMsgType		= _MSG_ACTION;

	m_pInfo->dwTimeStamp	= ::timeGetTime();
	m_pInfo->idUser			= idUser;
	m_pInfo->usPosX			= nPosX;
	m_pInfo->usPosY			= nPosY;
	m_pInfo->usDir			= usDir;
	m_pInfo->dwData			= dwData;
	m_pInfo->usAction		= usAction;
	return true;
}
