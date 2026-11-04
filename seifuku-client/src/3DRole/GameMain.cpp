#include "BaseFunc.h"
#include "3DGameMap.h"
#include "Role.h"
#include "GamePlayerSet.h"
#include "Hero.h"
#include "AllMsg.h"
#include "GameMsg.h"
#include "GameDataSet.h"
#include "RC5_321216.h"
#include "Entrance.h"

#include "MD5.h"
#include "Register.h"

#define	REPLAY_OK_STR		"ANSWER_OK"

HWND	g_hMyWnd	=NULL;
UINT	g_uStatus	=_STATUS_NONE;
CHAR	g_szAccount[256]	= "";

const unsigned char RC5PASSWORD_KEY[] = {0x3C, 0xDC, 0xFE, 0xE8, 0xC4, 0x54, 0xD6, 0x7E,0x16, 0xA6, 0xF8, 0x1A, 0xE8, 0xD0, 0x38, 0xBE};

int GameLoginServer(const char* pszAccount, const char* pszPsw, const char* pszServer, int nPort, const char* pszServerName)
{
	g_objHero.Reset();
	strcpy(g_szAccount, pszAccount);

	if (0 == stricmp(pszServer, "127.0.0.1"))
		return -1;

#ifdef _ACCOUNT_DISABLE
	if (!g_objNetwork.Init(pszServer, _PORT_GAMESERVER))
	{
		g_uStatus	=_STATUS_EXIT;

		char szMsg[256];
		sprintf(szMsg, "服务器连接错误, 可能是由于服务器维护或网络堵塞引起，请稍候再登录。");
		g_objEntrance.AddMsg(szMsg);

		return -1;
	}

	g_uStatus	=_STATUS_LOGIN;

	CMsgLogin msg;
	if (msg.Create(_VERSION, pszAccount, pszPsw))
		msg.Send();

	return 0;

#else
	if (!g_objNetwork.Init(pszServer, nPort))
	{
		g_uStatus	=_STATUS_EXIT;

		g_objEntrance.AddMsg(GameDataSetQuery()->GetStr(10397));

		::Sleep(5000);
		return -1;
	}

	int nType = 0;
	::IniDataGet("ini/oem.ini", "AccountSetup", "Type", nType, false);
	if (nType == 1)
	{
		if (strlen(pszAccount) > 64 || strlen(pszPsw) > 64)
		{
			ErrorMsg("invalid Account or Psw, size > 64!");
			return -1;
		}
		CMsgLoginAccountEx msg;
		msg.Create(pszAccount, pszPsw, pszServerName);
		msg.Send();
	}
	else if (nType == 2)
	{
		if (strlen(pszAccount) > 64 || strlen(pszPsw) > 64)
		{
			ErrorMsg("invalid Account or Psw, size > 64!");
			return -1;
		}
		char szPsw[256] = "";
		MD5Password(szPsw, pszPsw);
		CMsgLoginAccountEx msg;
		msg.Create(pszAccount, szPsw, pszServerName);
		msg.Send();
	}
	else
	{
		if (strlen(pszAccount) >= 16 || strlen(pszPsw) >= 16)
		{
			ErrorMsg("invalid Account or Psw, size >= 16!");
			return -1;
		}
		char szPwdCopy[16] = "";
		strcpy(szPwdCopy, pszPsw);
		CRc5_321216* pRc5 = new(CRc5_321216);
		if(!pRc5)
			return -1;
		pRc5->Rc5InitKey(RC5PASSWORD_KEY);
		pRc5->Rc5Encrypt(szPwdCopy, 16);
		delete(pRc5);
		pRc5 = NULL;

		CMsgAccount msg;
		if (msg.Create(pszAccount, szPwdCopy, pszServerName))
			msg.Send();
	}

	g_uStatus	=_STATUS_ACCOUNT;
	return 0;
#endif
}

void GameScreenMsg(const char* pszMsg)
{
	if (!pszMsg)
		return;

	int nScrWidth=0, nScrHeight=0;
	CMyBitmap::GetScreenSize(nScrWidth, nScrHeight);

	int nFontWidth=0, nFontHeight=0;
	nFontHeight	=CMyBitmap::GetFontSize();
	nFontWidth	=nFontHeight/2;

	CMyBitmap::Begin3D ();
	CMyBitmap::ClearBuffer ( true, true, D3DCOLOR_XRGB ( 0, 0, 0 ) );

	int nShowX	=(nScrWidth-strlen(pszMsg)*nFontWidth)/2;
	int nShowY	=(nScrHeight-nFontHeight)/2;

	CMyBitmap::ShowString(nShowX, nShowY, 0x00ffffff, pszMsg);

	CMyBitmap::End3D ();
	CMyBitmap::Flip ( NULL, NULL, NULL );
}

BOOL GameInit(HWND hWnd)
{
	g_hMyWnd	=hWnd;

	if (!CMyBitmap::Init3DEx (	hWnd,
								_SCR_WIDTH,
								_SCR_HEIGHT,
								1,
								1))
		return false;

	MYASSERT (CMyBitmap::GameFontCreate());

	g_objHero.Create(2);

#ifdef _NETWORK_DISABLE
	MYASSERT (g_objGameMap.LoadDataMap ("map/map/desert.DMap"));
	g_objGameMap.m_idDoc = 1000;

	g_objHero.SetPos(501, 645);
	g_objGameMap.AddInteractiveObj(&g_objHero);
#endif

	g_objGameMsg.SetShowLines(6);
	return true;
}

int nFrameRate	=0;
int GameMain()
{
	if (_STATUS_EXIT == g_uStatus)
	{
		const char* pszMsg	=g_objEntrance.GetLastMsg();
		if (pszMsg)
		{
			if (0 == stricmp(REPLAY_OK_STR, pszMsg))
			{
				g_uStatus	=_STATUS_NONE;
			}
			else
			{
				::MessageBox(NULL, pszMsg, "提示", MB_OK|MB_ICONERROR);
				g_uStatus	=_STATUS_DESTROY;
			}
		}

		return g_uStatus;
	}
	else if (_STATUS_REGISTER == g_uStatus)
	{
		GameScreenMsg("注册中。。。");

		g_objNetwork.ProcessNetMsg();
		const char* pszMsg	=g_objRegister.GetLastMsg();
		if (pszMsg)
		{
			g_uStatus	=_STATUS_EXIT;
			g_objEntrance.AddMsg(pszMsg);
		}

		return g_uStatus;
	}
	else if (_STATUS_ACCOUNT == g_uStatus)
	{
		GameScreenMsg("连接账号服务器中。。。");

		g_objNetwork.ProcessNetMsg();
		return g_uStatus;
	}
	else if (_STATUS_LOGIN == g_uStatus)
	{
		GameScreenMsg("登录服务器中。。。");

		g_objNetwork.ProcessNetMsg();

		const char* pszMsg	=g_objEntrance.GetLastMsg();
		if (pszMsg)
		{
			if (0 != stricmp(REPLAY_OK_STR, pszMsg))
			{
				g_objEntrance.AddMsg(pszMsg);
				g_uStatus	=_STATUS_EXIT;
			}
		}

		return g_uStatus;
	}

	static DWORD timeLoop	=0;
	DWORD timeNow	=::TimeGet();
	if (timeNow < timeLoop+30)
		::Sleep(timeLoop+30-timeNow);

	nFrameRate	=(nFrameRate+1000/__max(1, int(::TimeGet()-timeLoop)))/2;
	timeLoop	=::TimeGet();

	int nMouseX, nMouseY;
	int nEvent	=::MouseCheck(nMouseX, nMouseY);

	int nMapX, nMapY;
	g_objGameMap.Screen2Cell(nMouseX, nMouseY, nMapX, nMapY);

	if (nEvent)
	{
		CCommand cmd;
		cmd.iType		=_COMMAND_JUMP;
		cmd.iStatus		=_CMDSTATUS_BEGIN;
		cmd.posTarget.x	=nMapX;
		cmd.posTarget.y	=nMapY;

		g_objHero.SetCommand(&cmd);
	}

	CMyPos posHeroAlign	=g_objHero.GetAlignPos();
	g_objPlayerSet.Process(posHeroAlign.x, posHeroAlign.y);

	g_objGameMap.Process(NULL);

	CMyPos posWorld;
	g_objHero.GetWorldPos(posWorld);

	posWorld.x	-=CMyBitmap::GetScreenWidth()/2;
	posWorld.y	-=CMyBitmap::GetScreenHeight()/2;

	g_objGameMap.SetViewPos(posWorld);

	::MouseProcess();

	CMyBitmap::Begin3D ();
	CMyBitmap::ClearBuffer ( true, false, D3DCOLOR_XRGB ( 255, 0, 0 ) );

	g_objGameMap.Show();

	g_objPlayerSet.ShowX();

	g_objGameMsg.Show();

	CMyPos posView;
	g_objGameMap.GetBgViewport(posView.x, posView.y);

	CMyPos posHero;
	g_objHero.GetPos(posHero);

	CMyPos posAlign	=g_objHero.GetAlignPos();

	char szMsg[256] = "";
	sprintf(szMsg, "Hero(%03d,%03d [%03d, %03d]), Dir:%d, Viewport(%03d,%03d)",
		posHero.x, posHero.y, posAlign.x, posAlign.y,
		g_objHero.GetDir(),
		posView.x, posView.y);

	CMyBitmap::ShowString(10, 10, 0xffffff, szMsg);

	int nScrX, nScrY;
	g_objGameMap.Cell2Screen(nMapX, nMapY, nScrX, nScrY);

	CMyBitmap::ShowLine(nScrX, nScrY-16, nScrX-32, nScrY, 0xff000000);
	CMyBitmap::ShowLine(nScrX, nScrY-16, nScrX+32, nScrY, 0xff000000);
	CMyBitmap::ShowLine(nScrX, nScrY+16, nScrX-32, nScrY, 0xff000000);
	CMyBitmap::ShowLine(nScrX, nScrY+16, nScrX+32, nScrY, 0xff000000);

	CMyBitmap::End3D ();
	CMyBitmap::Flip ( NULL, NULL, NULL );

	GameDataSetQuery()->Process();

#ifndef _NETWORK_DISABLE
	g_objNetwork.ProcessNetMsg();
#endif

	return g_uStatus;
}
