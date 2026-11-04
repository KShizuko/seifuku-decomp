#include "stdafx.h"
#include <afxinet.h>
#include "ServerStatus.h"
#include "BaseFunc.h"

CServerStatus::CServerStatus()
{

}

CServerStatus::~CServerStatus()
{
	this->Destroy();
}

void CServerStatus::Destroy()
{
	int nAmount = m_setServerInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		ServerInfo* pInfo = m_setServerInfo[i];
		SAFE_DELETE(pInfo);
	}
	m_setServerInfo.clear();
}

int CServerStatus::GetServerStatus(char* pszServerName)
{
	if(!pszServerName)
		return _SERVERSATUS_NONE;
	int nAmount = m_setServerInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		ServerInfo* pInfo = m_setServerInfo[i];
		if(pInfo && strcmp(pszServerName, pInfo->szServerName) == 0)
		{
			return pInfo->nServerStatus;
		}
	}
	return _SERVERSATUS_NONE;
}

void CServerStatus::FlashStatus()
{
	this->Destroy();

	char szUrl[1024] = "";
	if(!::IniStrGet("ini/common.ini", "ServerStatus", "URL", szUrl))
		return;
	try
	{
		CInternetSession	session;
		CInternetFile* file = (CInternetFile*)session.OpenURL(szUrl);
		if(file)
		{
			while(true)
			{
				CString string = "";
				if(!file->ReadString(string))
					break;

				char	szServerName[256] = "";
				int		nServerStatus;
				if (2 == sscanf(string, "%s %d", szServerName, &nServerStatus))
				{
					ServerInfo* pInfo = new ServerInfo;
					if(pInfo)
					{
						pInfo->nServerStatus = nServerStatus;
						strcpy(pInfo->szServerName, szServerName);
						m_setServerInfo.push_back(pInfo);
					}
				}
			}
			file->Close();
			delete	file;
		}
	}
	catch(CInternetException* pEx)
	{
		pEx->Delete();
	}
}
