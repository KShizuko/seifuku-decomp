#include "stdafx.h"
#include "afxinet.h"
#include "BaseFunc.h"
#include "GameDataSet.h"
#include "ServerAdv.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static void ReplaceChar(char* pszStr, char cOld, char cNew)
{
	if(!pszStr)
		return;

	int nLen = strlen(pszStr);
	for(int i = 0; i < nLen; i ++)
	{
		if(pszStr[i] == cOld)
			pszStr[i] = cNew;
	}
}

CServerAdv::CServerAdv()
{

}

CServerAdv::~CServerAdv()
{

}
void CServerAdv::Init(BOOL b1024)
{
	char szSubtitle[256] = "";
	if(!b1024)
		strcpy(szSubtitle, "800x600");
	else
		strcpy(szSubtitle, "1024x768");

	char szUrl[1024] = "";
	if(!::IniStrGet("ini/common.ini", "ServerAdv", szSubtitle, szUrl))
	{
		if(stricmp(GameDataSetQuery()->GetVersionInfo(), "English") == 0)
			exit(-1);
		return;
	}

	char szFile[256] = "";
	sprintf(szFile, "log/%s.adv", szSubtitle);
	const int _ADV_EXPIRE = 12 * 60 * 60;
	time_t tNow;
	time(&tNow);
	BOOL bDownload = true;
	FILE* fp = fopen(szFile, "r");
	if(fp)
	{
		DWORD dwTime = 0;
		fscanf(fp, "%u\n", &dwTime);
		if((int)((DWORD)tNow - dwTime) < _ADV_EXPIRE)
		{
			while(true)
			{
				char szLine[1024] = "";
				if(!fgets(szLine, 0x400, fp))
					break;
				szLine[strlen(szLine) - 1] = 0;
				ServerAdvInfo info;
				memset(&info, 0, sizeof(info));
				if (sscanf(szLine, "%s %d %d %s %d %x %s %s", info.szServerName, &info.nPosX, &info.nPosY, info.szFont,
							&info.nFontSize, &info.dwColor, info.szText, info.szUrl) >= 7)
				{
					ReplaceChar(info.szText, '~', ' ');
					ReplaceChar(info.szFont, '~', ' ');
					m_setAdv.push_back(info);
				}
			}
			bDownload = false;
		}
		fclose(fp);
	}
	if(!bDownload)
		return;

	CInternetSession	session;
	try
	{
		CInternetFile* file = (CInternetFile*)session.OpenURL(szUrl);
		if(file)
		{
			vector<ServerAdvInfo> setAdv;
			while(true)
			{
				CString string = "";
				if(!file->ReadString(string))
					break;

				ServerAdvInfo info;
				memset(&info, 0, sizeof(info));
				if (sscanf(string, "%s %d %d %s %d %x %s %s", info.szServerName, &info.nPosX, &info.nPosY, info.szFont,
							&info.nFontSize, &info.dwColor, info.szText, info.szUrl) >= 7)
				{
					setAdv.push_back(info);
					ReplaceChar(info.szText, '~', ' ');
					ReplaceChar(info.szFont, '~', ' ');
					m_setAdv.push_back(info);
				}
			}
			file->Close();
			delete	file;

			FILE* fpOut = fopen(szFile, "w");
			if(fpOut)
			{
				fprintf(fpOut, "%u\n", (DWORD)tNow);
				for(DWORD i = 0; i < m_setAdv.size(); i ++)
				{
					ServerAdvInfo infoLine = setAdv[i];
					fprintf(fpOut, "%s %d %d %s %d %x %s %s\n", infoLine.szServerName, infoLine.nPosX, infoLine.nPosY, infoLine.szFont,
							infoLine.nFontSize, infoLine.dwColor, infoLine.szText, infoLine.szUrl);
				}
				fclose(fpOut);
			}
		}
	}
	catch(CInternetException* pEx)
	{
		pEx->Delete();
	}
}
bool CServerAdv::GetAdv(const char* pszServerName, vector<ServerAdvInfo>& setAdv)
{
	if(!pszServerName)
		return false;

	setAdv.clear();
	for(vector<ServerAdvInfo>::iterator it = m_setAdv.begin(); it != m_setAdv.end(); it ++)
	{
		ServerAdvInfo* pInfo = &(*it);
		if(strcmp(pszServerName, pInfo->szServerName) == 0)
			setAdv.push_back(*pInfo);
	}
	return setAdv.size() > 0;
}
