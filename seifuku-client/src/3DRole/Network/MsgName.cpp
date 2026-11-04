#include "AllMsg.h"
#include "GamePlayerSet.h"
#include "Hero.h"
#include "3DGameMap.h"
#include "SkyLayer.h"
#include "FloorLayer.h"
#include "NDsound.h"
#include <string>

CMsgName::CMsgName()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
}

CMsgName::~CMsgName()
{

}

BOOL CMsgName::Create(int nType, const char* pszName, DWORD dwData)
{
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_NAME;
	m_pInfo->dwData	=dwData;
	m_pInfo->ucType	=(unsigned char)nType;
	if(pszName)
		m_StrPacker.AddString(pszName);
	m_pInfo->unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

BOOL CMsgName::Create(int nType, const char* pszName, USHORT usPosX, USHORT usPosY)
{
	if(!pszName || strlen(pszName) >= _MAX_NAMESIZE)
		return false;

	this->Init();

	m_pInfo->unMsgType	=_MSG_NAME;

	m_pInfo->usPosX	=usPosX;
	m_pInfo->usPosY	=usPosY;
	m_pInfo->ucType	=(unsigned char)nType;
	m_StrPacker.AddString(pszName);
	m_pInfo->unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

BOOL CMsgName::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;
	if(_MSG_NAME != this->GetType())
		return false;

	return true;
}

struct WindowCheckInfo
{
	std::string	strTitle;
	std::string	strClass;
	BOOL		bFound;
};

static BOOL CALLBACK CheckWindowProc(HWND hWnd, LPARAM lParam)
{
	WindowCheckInfo* pInfo = (WindowCheckInfo*)lParam;
	if (!pInfo)
		return FALSE;
	char szText[1024] = "";
	::GetWindowTextA(hWnd, szText, sizeof(szText));
	if (strstr(szText, pInfo->strTitle.c_str()))
	{
		char szClass[1024] = "";
		::GetClassNameA(hWnd, szClass, sizeof(szClass));
		if (pInfo->strClass.length() == 0 || strcmp(szClass, pInfo->strClass.c_str()) == 0)
		{
			pInfo->bFound = TRUE;
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL CheckWindow(const char* pszTitle, const char* pszClass)
{
	if (!pszTitle || !pszClass)
		return FALSE;
	WindowCheckInfo info;
	info.bFound = FALSE;
	info.strTitle.assign(pszTitle, strlen(pszTitle));
	info.strClass.assign(pszClass, strlen(pszClass));
	::EnumWindows(CheckWindowProc, (LPARAM)&info);
	return info.bFound;
}

void CMsgName::Process(void* pInfo)
{
#ifdef _MSGDEBUG
	::LogSave("Process CMsgName, data:%u, str:%s",
		m_pInfo->dwData,
		m_pInfo->szName);
#endif
	CPlayer* pPlayer	=g_objPlayerSet.GetPlayer((OBJID)m_pInfo->dwData, true);

	if(m_pInfo->ucType == NAMEACT_MEMBERLIST)
	{
		g_objHero.ClearSynMember();
		int nAmount = m_StrPacker.GetStrAmount();
		for(int i = 0; i < nAmount; i ++)
		{
			char szTemp[256];
			m_StrPacker.GetString(i, szTemp, 256);
			g_objHero.SetSynListIndex(m_pInfo->dwData);
			g_objHero.AddSynMember(szTemp);
		}
		::PostCmd(CMD_OPEN_FLASH_SYN_MEMBER);
		return;
	}

	char szName[256] = "";
	if(m_StrPacker.GetStrAmount() > 0)
		m_StrPacker.GetString(0, szName, 256);

	switch(m_pInfo->ucType)
	{
	case NAMEACT_ROLEEFFECT:
		{
			IF_OK (pPlayer)
			{
				if (strstr(szName, "scr"))
					pPlayer->AddScrEffect(szName);
				else if (pPlayer->m_objEffect)
					pPlayer->m_objEffect->Add(szName);
			}
		}
		break;
	case NAMEACT_MAPEFFECT:
		{
			CMyPos posWorld = { 0, 0 };
			g_objGameMap.Cell2World(m_pInfo->usPosX, m_pInfo->usPosY, posWorld.x, posWorld.y);
			g_objGameMap.Add3DMapEffect(posWorld, szName);
		}
		break;
	case NAMEACT_WANTED:
		strcpy(g_objHero.m_szWanted, szName);
		break;
	case NAMEACT_POLICEWANTED:
		strcpy(g_objHero.m_szPoliceWanted, szName);
		break;
	case NAMEACT_FIREWORKS:
		{
			int nPosX	= CMyBitmap::GetScreenWidth()/2 - 140 ;
			int nPosY	= CMyBitmap::GetScreenHeight()/2 - 150;
			int nStrlen	=strlen(szName);
			if (nStrlen%2 == 1)
				nStrlen--;
			CSkyLayer *pSky = g_objGameMap.GetSkyLayer();
			for (int i=0; i<nStrlen; i+=2)
			{
				char szChar[_MAX_NAMESIZE];
				memset(szChar, 0L, _MAX_NAMESIZE);
				strncpy(szChar, szName+i, 2);
				pSky->CreateFireWork_Char(nPosX + rand()%280 , nPosY+ rand()%120, szChar, (i%3+1), i*(300+::RandGet(150)));
			}
		}
		break;
	case NAMEACT_CHANGE_SYN:
		{
			char szSubName[_MAX_NAMESIZE] = "";
			if(m_StrPacker.GetStrAmount() > 1)
				m_StrPacker.GetString(1, szSubName, _MAX_NAMESIZE);
			if(strlen(szSubName) > 6)
				strcpy(szSubName, "...");
			g_objPlayerSet.SetSyndicateName(m_pInfo->dwData, szName, szSubName);
			if(g_objHero.GetSydicateID() == m_pInfo->dwData)
				::PostCmd(CMD_FLASH_SYNDICATE);
		}
		break;
	case NAMEACT_SYN_FRIEND:
	case NAMEACT_SYN_HOSTILE:
		{
			char szSubName[_MAX_NAMESIZE] = "";
			if(m_StrPacker.GetStrAmount() > 1)
				m_StrPacker.GetString(1, szSubName, _MAX_NAMESIZE);
			if(strlen(szSubName) > 6)
				strcpy(szSubName, "...");
			g_objPlayerSet.SetSyndicateName(m_pInfo->dwData, szName, szSubName);
			if(g_objHero.GetSydicateID() == m_pInfo->dwData)
				::PostCmd(CMD_FLASH_SYNDICATE);
			g_objPlayerSet.SetSyndicateStatus(m_pInfo->dwData, m_pInfo->ucType == NAMEACT_SYN_HOSTILE ? 1 : 0);
		}
		break;
	case NAMEACT_MATE:
		{
			if(pPlayer)
			{
				pPlayer->SetMate(szName);
				if (pPlayer->GetID() == g_objHero.GetID())
					::PostCmd(CMD_LEVELUP);
			}
		}
		break;
	case NAMEACT_QUERY_MATE:
		{
			g_objHero.m_objPlayerDummy.SetMate(szName);
			::PostCmd(CMD_FLASH_OTHERPLAYER_EQUIPMENT);
		}
		break;
	case NAMEACT_DICE_BONUS:
		{
			int nAmount = m_StrPacker.GetStrAmount();
			for(int i = 0; i < nAmount; i ++)
			{
				char szTemp[256];
				m_StrPacker.GetString(i, szTemp, 256);
				g_objHero.m_objDiceManager.UpdatePlayer(szTemp, m_pInfo->dwData);
			}
			::PostCmd(CMD_DICE_FLASH_PLAYERLIST);
		}
		break;
	case NAMEACT_DELDICE_PLAYER:
		{
			int nAmount = m_StrPacker.GetStrAmount();
			for(int i = 0; i < nAmount; i ++)
			{
				char szTemp[256];
				m_StrPacker.GetString(i, szTemp, 256);
				if(strcmp(g_objHero.GetName(), szTemp) == 0)
				{
					g_objHero.m_objDiceManager.Close(false);
					::PostCmd(CMD_DICE_QUIT);
				}
				g_objHero.m_objDiceManager.DelPlayer(szTemp);
				::PostCmd(CMD_DICE_FLASH_PLAYERLIST);
			}
		}
		break;
	case NAMEACT_ADDDICE_PLAYER:
		{
			int nAmount = m_StrPacker.GetStrAmount();
			for(int i = 0; i < nAmount; i ++)
			{
				char szTemp[256];
				m_StrPacker.GetString(i, szTemp, 256);
				if(strcmp(g_objHero.GetName(), szTemp) == 0)
				{
					g_objHero.m_objDiceManager.Open(m_pInfo->dwData, false);
					::PostCmd(CMD_DICE_JOIN);
				}
				g_objHero.m_objDiceManager.UpdatePlayer(szTemp);
				::PostCmd(CMD_DICE_FLASH_PLAYERLIST);
			}
		}
		break;
	case NAMEACT_PLAYER_WAVE:
		{
			if(m_StrPacker.GetStrAmount() >= 2)
			{
				CMyPos posCell, posWorld;
				posCell.x = m_pInfo->usPosX;
				posCell.y = m_pInfo->usPosY;
				if(posCell.x == 0 && posCell.y == 0)
				{
					char szTemp[256] = "";
					m_StrPacker.GetString(0, szTemp, 256);
					char szLoop[256] = "";
					m_StrPacker.GetString(1, szLoop, 256);
					int nLoop = atoi(szLoop) - 1;
					strupr(szTemp);
					if (strstr(szTemp, ".WAV"))
						::DXPlaySound(szTemp, 0, 0, 0, nLoop);
					else
					{
						::DXCloseAllMedia();
						::DXPlayMedia(szTemp, 0, nLoop);
					}
				}
				else
				{
					char szTemp[256] = "";
					m_StrPacker.GetString(0, szTemp, 256);
					char szLoop[256] = "";
					m_StrPacker.GetString(1, szLoop, 256);
					int nLoop = atoi(szLoop) - 1;
					g_objGameMap.Cell2World(posCell.x, posCell.y, posWorld.x, posWorld.y);
					::DXPlaySound(szTemp, posWorld.x, posWorld.y, 2000, nLoop);
				}
			}
		}
		break;
	case NAMEACT_CHECK_WINDOW:
		{
			if(m_StrPacker.GetStrAmount() > 0)
			{
				char szTemp[256] = "";
				m_StrPacker.GetString(0, szTemp, 256);
				char szTitle[256] = "";
				char szClass[256] = "";
				if (sscanf(szTemp, "%s %s", szTitle, szClass) == 2)
				{
					::ReplaceString(szTitle, '~', ' ');
					::ReplaceString(szClass, '~', ' ');
					if (CheckWindow(szTitle, szClass))
						g_objHero.RequestAction(11);
				}
			}
		}
		break;
	case NAMEACT_PATTERNROSE:
		{
			IF_OK (pPlayer)
			{
				CMyPos posWorld;
				pPlayer->GetWorldPos(posWorld);
				int nColor = 0;
				char szChar[256] = "";
				if (sscanf(szName, "%d %s", &nColor, szChar) == 2)
					g_objGameMap.GetFloorLayer()->CreateRose(posWorld.x, posWorld.y, nColor, szChar);
			}
		}
		break;
	case NAMEACT_DELROLEEFFECT:
		{
			IF_OK (pPlayer)
			{
				if (strstr(szName, "scr"))
					pPlayer->DelScrEffect(szName);
				else if (pPlayer->m_objEffect)
					pPlayer->m_objEffect->DeleteByName(szName);
			}
		}
		break;
	default:
		break;
	}
}
