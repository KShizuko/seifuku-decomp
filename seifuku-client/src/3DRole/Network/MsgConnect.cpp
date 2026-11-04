#include "AllMsg.h"
#include "Hero.h"
#include "GameDataSet.h"
#include "Register.h"
#include "Entrance.h"
#include "Shop.h"
#include <nb30.h>

extern UINT g_uStatus;

typedef struct tagASTAT
{
	ADAPTER_STATUS	adapt;
	NAME_BUFFER		NameBuff[30];
} ASTAT, *LPASTAT;
static ASTAT s_Adapter;
void GetMacAddress(std::vector<MAC_ADDRESS>& setMac)
{
	setMac.clear();
	if (TRUE)
	{
		MAC_ADDRESS mac;
		memset(&mac, 0, sizeof(mac));
		setMac.push_back(mac);
		return;
	}
	try
	{
		NCB ncb;
		UCHAR uRetCode;
		LANA_ENUM lana_enum;
		memset(&ncb, 0, sizeof(ncb));
		ncb.ncb_command = NCBENUM;
		ncb.ncb_buffer = (unsigned char*)&lana_enum;
		ncb.ncb_length = sizeof(lana_enum);
		uRetCode = Netbios(&ncb);
		if (uRetCode != 0)
			return;
		for (int i = 0; i < lana_enum.length; i++)
		{
			NCB ncb;
			UCHAR uRetCode;
			memset(&ncb, 0, sizeof(ncb));
			ncb.ncb_command = NCBRESET;
			ncb.ncb_lana_num = lana_enum.lana[i];
			uRetCode = Netbios(&ncb);
			memset(&ncb, 0, sizeof(ncb));
			ncb.ncb_command = NCBASTAT;
			ncb.ncb_lana_num = lana_enum.lana[i];
			strcpy((char*)ncb.ncb_callname, "*               ");
			ncb.ncb_buffer = (unsigned char*)&s_Adapter;
			ncb.ncb_length = sizeof(s_Adapter);
			uRetCode = Netbios(&ncb);
			if (uRetCode == 0)
			{
				MAC_ADDRESS mac;
				memcpy(&mac, s_Adapter.adapt.adapter_address, MAX_MACADDRSIZE);
				setMac.push_back(mac);
			}
		}
	}
	catch (...)
	{
		MAC_ADDRESS mac;
		memset(&mac, 0, sizeof(mac));
		setMac.push_back(mac);
	}
}

CMsgConnect::CMsgConnect()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;

}

CMsgConnect::~CMsgConnect()
{

}

BOOL CMsgConnect::Create(OBJID idAccount, DWORD dwData, const char* pszInfo)
{
	if (!pszInfo || strlen(pszInfo) >= _MAX_NAMESIZE)
		return false;

	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_CONNECT;

	m_pInfo->idAccount		=idAccount;
	m_pInfo->dwData			=dwData;
	strcpy(m_pInfo->szInfo, pszInfo);

	return true;
}

BOOL CMsgConnect::Create(OBJID idAccount, DWORD dwData, int nVersion, const char* pszLanguage, const MAC_ADDRESS* pMac, DWORD dwRes)
{
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_CONNECT;

	m_pInfo->idAccount		=idAccount;
	m_pInfo->dwData			=dwData;
	m_pInfo->game.usVersion	=(USHORT)nVersion;
	m_pInfo->game.dwRes		=dwRes;
	memcpy(m_pInfo->game.szLanguage, pszLanguage, _MAX_LANGUAGESIZE);
	memcpy(m_pInfo->game.abyMac, pMac, MAX_MACADDRSIZE);

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

void CMsgConnect::Process(void* pInfo)
{
	if (m_pInfo->idAccount == ID_NONE)
	{
		char szInfo[256] = "";
		g_uStatus	=_STATUS_EXIT;

		if (strstr(m_pInfo->szInfo, "服务器未启动") != NULL)
			strcpy(szInfo, GameDataSetQuery()->GetStr(10417));
		else if (strstr(m_pInfo->szInfo, "帐号名或口令错") != NULL)
			strcpy(szInfo, GameDataSetQuery()->GetStr(10418));
		else if (strstr(m_pInfo->szInfo, "请稍后重新登录") != NULL)
			strcpy(szInfo, GameDataSetQuery()->GetStr(10419));
		else if (strstr(m_pInfo->szInfo, "该帐号被封号") != NULL)
			strcpy(szInfo, GameDataSetQuery()->GetStr(10438));
		else if (strstr(m_pInfo->szInfo, "该帐号锁定中") != NULL)
			strcpy(szInfo, GameDataSetQuery()->GetStr(10440));
		else if (strstr(m_pInfo->szInfo, "服务器人数已满") != NULL)
			strcpy(szInfo, GameDataSetQuery()->GetStr(10480));
		else if (strstr(m_pInfo->szInfo, "服务器忙请稍候") != NULL)
			strcpy(szInfo, GameDataSetQuery()->GetStr(10480));
		else
		{
			char szTemp[256] = "";
			strcat(szTemp, GameDataSetQuery()->GetStr(10439));
			char szMsg[256] = "";
			const OBJID idLoginInError = 100099;
			sprintf(szMsg, GameDataSetQuery()->GetStr(idLoginInError), szInfo);
			strcat(szTemp, szMsg);
			g_objEntrance.AddMsg(szTemp);
			return;
		}

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

		if (!g_objNetwork.Init(m_pInfo->szInfo, _PORT_GAMESERVER))
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
