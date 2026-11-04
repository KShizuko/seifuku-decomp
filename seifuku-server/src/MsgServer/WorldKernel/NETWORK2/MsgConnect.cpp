#include "AllMsg.h"
#include "UserList.h"
#include "logfile.h"
using namespace world_kernel;
#include "WorldKernel.h"

CMsgConnect::CMsgConnect()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;

}

CMsgConnect::~CMsgConnect()
{

}

BOOL CMsgConnect::Create(OBJID idAccount, DWORD dwData, char* pszInfo)
{
	if (!pszInfo || strlen(pszInfo) >= _MAX_NAMESIZE)
		return false;
	
	this->Init();

	m_unMsgSize	=sizeof(MSG_Info);
	m_unMsgType	=_MSG_CONNECT;

	m_pInfo->idAccount		=idAccount;
	m_pInfo->dwData			=dwData;
	strcpy(m_pInfo->szInfo, pszInfo);

	return true;
}

BOOL CMsgConnect::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_CONNECT != this->GetType())
		return false;

	return true;
}

void CMsgConnect::Process(void* pSocket)
{
#ifdef _MYDEBUG_X
	::LogSave("Process CMsgConnect, idAccount:%u, Data:%d, Info:%s",
				m_pInfo->idAccount,
				m_pInfo->dwData,
				m_pInfo->szInfo);
#endif
	GameWorld()->QueryUserList()->SetClientVersion(m_pInfo->idAccount, m_pInfo->usVersion);

	if (!pSocket || m_pInfo->idAccount == ID_NONE)
		return;

	bool bWrongLanguage = strcmp(CStringManager::Instance()->GetStr("Language"), "中文") != 0
							&& strncmp(m_pInfo->szLanguage, "中", 2) == 0;
	if (bWrongLanguage)
	{
		CMsgTalk msg;
		if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, STR_ERROR_VERSION, NULL, _COLOR_WHITE, _TXTATR_ENTRANCE))
			SendMsg(&msg);
		return;
	}
#ifdef	PALED_DEBUG
#else
	if (m_pInfo->usVersion < GAME_VERSION)
	{
		CMsgTalk msg;
		if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, STR_ERROR_LOWER_VERSION, NULL, _COLOR_WHITE, _TXTATR_ENTRANCE))
			SendMsg(&msg);
		return;
	}
#endif
	extern DWORD g_dwMaxPlayer;
	if(GameWorld()->QueryUserList()->GetUserAmount() > (int)g_dwMaxPlayer && (int)m_pInfo->idAccount > MAX_GM_ID)
	{
		CMsgTalk msg;
		if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, STR_ERROR_SERVER_FULL, NULL, _COLOR_WHITE, _TXTATR_ENTRANCE))
			SendMsg(&msg);
		return;
	}

	if(!GameWorld()->QueryUserList()->CheckAccountLock(m_pInfo->idAccount, m_pInfo->bufClient,
								GameWorld()->GetSocketIP(GetSocketID()), m_pInfo->dwResData))
	{
		CMsgTalk msg;
		if(msg.Create(SYSTEM_NAME, ALLUSERS_NAME, "account locked.", NULL, _COLOR_WHITE, _TXTATR_ENTRANCE))
			SendMsg(&msg);
		return;
	}

	DWORD dwEncyptCode = (m_pInfo->idAccount+m_pInfo->dwData)^0x4321;
	GameWorld()->ChangeCode(GetSocketID(), m_pInfo->dwData^dwEncyptCode);

	if(GameWorld()->ClientConnect(m_pInfo->idAccount, m_pInfo->dwData, m_pInfo->szInfo, this->GetSocketID()))
	{
		CPlayer* pPlayer	= g_UserList.GetPlayerByAccountID(m_pInfo->idAccount);
		MYASSERT(pPlayer);
		if(pPlayer)
			UserList()->LoginToMapGroup(pPlayer);
	}
}