#include "AllMsg.h"
#include "Hero.h"
#include "GameDataSet.h"
#include "Entrance.h"

extern UINT g_uStatus;

CMsgLoginReply::CMsgLoginReply()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgLoginReply::~CMsgLoginReply()
{

}

BOOL CMsgLoginReply::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_LOGINREPLY != this->GetType())
		return false;

	return true;
}

void CMsgLoginReply::Process(void* pInfo)
{
	if (m_pInfo->idAccount == ID_NONE)
	{
		g_uStatus	=_STATUS_EXIT;
		char szInfo[256] = "";
		if ( strstr( m_pInfo->szInfo, "服务器未启动" ) != NULL )
			strcpy( szInfo, GameDataSetQuery()->GetStr( 10417 ) );
		else if ( strstr( m_pInfo->szInfo, "帐号名或口令错" ) != NULL )
			strcpy( szInfo, GameDataSetQuery()->GetStr( 10418 ) );
		else if ( strstr( m_pInfo->szInfo, "请稍后重新登录" ) != NULL )
			strcpy( szInfo, GameDataSetQuery()->GetStr( 10419 ) );
		else if ( strstr( m_pInfo->szInfo, "该帐号被封号" ) != NULL )
			strcpy( szInfo, GameDataSetQuery()->GetStr( 10438 ) );
		else if ( strstr( m_pInfo->szInfo, "该帐号锁定中" ) != NULL )
			strcpy( szInfo, GameDataSetQuery()->GetStr( 10440 ) );
		else if ( strstr( m_pInfo->szInfo, "服务器人数已满" ) != NULL )
			strcpy( szInfo, GameDataSetQuery()->GetStr( 10480 ) );
		else if ( strstr( m_pInfo->szInfo, "服务器忙请稍候" ) != NULL )
			strcpy( szInfo, GameDataSetQuery()->GetStr( 10480 ) );
		else
			strcpy( szInfo, GameDataSetQuery()->GetStr( 10439 ) );

		char szMsg[256] = "";
		const OBJID idLoginInError = 100099;
		sprintf(szMsg, GameDataSetQuery()->GetStr(idLoginInError), szInfo);
		g_objEntrance.AddMsg(szMsg);

		return;
	}
	else
		g_objHero.m_idAccount	=m_pInfo->idAccount;

	DWORD dwEncyptCode = (m_pInfo->idAccount+m_pInfo->dwData)^0x4321;

	CMsgConnect msg;

	const char szVersionFile[]="res.dat";
	FILE* fp = fopen(szVersionFile, "r");
	DWORD dwRes = 0;
	if(fp)
	{
		fscanf(fp, "%d\n", &dwRes);
		fclose(fp);
	}
	if (msg.Create(m_pInfo->idAccount, dwRes, "res.dat"))
		msg.Send();

	{
		g_objNetwork.Destroy();

		if (!g_objNetwork.Init(m_pInfo->szInfo, m_pInfo->nServePort))
		{
			g_uStatus	=_STATUS_EXIT;
			const OBJID idConnectError = 100100;
			g_objEntrance.AddMsg(GameDataSetQuery()->GetStr(idConnectError));

			return;
		}
	}

	char szLanguage[_MAX_LANGUAGESIZE] = "";
	strncpy(szLanguage, GameDataSetQuery()->GetVersionInfo(), _MAX_LANGUAGESIZE);
	std::vector<MAC_ADDRESS> setMac;
	GetMacAddress(setMac);

	if (msg.Create(m_pInfo->idAccount, m_pInfo->dwData, _VERSION, szLanguage, &setMac[0], dwRes))
		msg.Send();

	g_objNetwork.ChangeCode(m_pInfo->dwData^dwEncyptCode);

	g_uStatus = _STATUS_LOGIN;
}
