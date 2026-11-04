#include "windows.h"
#include "define.h"
#include "UserManager.h"
#include "NpcManager.h"
#include "MapManager.h"
#include "NpcWorld.h"
#include "AllMsg.h"
#include "Protocol.h"
#include "I_Shell.h"
#include "inifile.h"
#include "I_mydb.h"

IKernel*	CreateNewKernel()
{
	return new CNpcWorld;
}

CNpcWorld*	CNpcWorld::m_pNpcWorld;

CNpcWorld::CNpcWorld()
{
	m_pNpcManager		= NULL;
	m_pUserManager		= NULL;
	m_pMapManager		= NULL;
	m_pSynManager		= NULL;

	m_pDatabase			= NULL;

	SetLoginState(STATE_NONE);
}

CNpcWorld::~CNpcWorld()
{
	if(m_pSynManager)
		SAFE_DELETE(m_pSynManager);
	if(m_pNpcManager)
		SAFE_DELETE(m_pNpcManager);
	if(m_pUserManager)
		SAFE_DELETE(m_pUserManager);
	if(m_pMapManager)
		SAFE_DELETE(m_pMapManager);

	if(m_pDatabase)
		m_pDatabase->Release();
}

bool CNpcWorld::Create(IShell* pShell)
{
	CHECKF(pShell);
#ifdef	DUMP_DEBUG_MSG
	char	buf[256] = "syslog\\dump ";
	::DateTime(buf + strlen(buf));
	strcpy(buf + 22, ".log");
	::DeleteFile(buf);
#endif

	m_pShell	= pShell;
	m_pNpcWorld	= this;
	m_timeoutLogin.Startup(RESEND_MSGLOGIN_SECS);
	m_tUpdateStateWin.Startup(UPDATESTATEWIN_SECS);

	PrintText("Open the database...");
	CIniFile	ini(CONFIG_INI_FILENAME, "Database");
	IPSTR	szIP, szLogin, szPassword, szLibName;
	ini.GetString(szIP, "DBServer", IPSTR_SIZE);
	ini.GetString(szLogin, "LoginName", IPSTR_SIZE);
	ini.GetString(szPassword, "Password", IPSTR_SIZE);
	ini.GetString(szLibName, "DBName", IPSTR_SIZE);
	int nPort = ini.GetInt("DBPort");
	if(nPort < 0) nPort = 0;
	m_pDatabase		= ::CreateDatabase(szIP, szLogin, szPassword, szLibName, nPort);
	if(!m_pDatabase)
	{
		PrintText("ERROR: Can't open database!");
		return false;
	}

	PrintText("Create map set...");
	m_pMapManager	= new CMapManager;
	if(!m_pMapManager || !m_pMapManager->Create())
		return false;
	PrintText("Create player set...");
	m_pUserManager	= new CUserManager;
	if(!m_pUserManager || !m_pUserManager->Create())
		return false;
	PrintText("Create npc set...");
	m_pNpcManager	= new CNpcManager;
	if(!m_pNpcManager || !m_pNpcManager->Create())
		return false;
	PrintText("Create syn set...");
	m_pSynManager	= new CSynManager;
	if(!m_pSynManager || !m_pSynManager->Create())
		return false;

	if(!m_pItemType->Create(m_pDatabase))
		return false;

	PrintText("======== Create cq server kernel ok! ========");
	return true;
}

void CNpcWorld::ProcessMsg(OBJID idPacket, const char* pbufMsg, int nSize, OBJID idNpc)
{
	if(!pbufMsg || nSize <= 0 || nSize >_MAX_MSGSIZE)
        return ;

	CNetMsg* pMsg	=CNetMsg::CreateMsg(idNpc, idPacket, pbufMsg, nSize);
	if(pMsg)
	{
		try {
			pMsg->Process(this);
		}
		catch(...)
		{
			LOGCATCH("CGameSocket::ProcessMsg()! MsgType:%d, NpcID:%u", idPacket, idNpc);
		}
		SAFE_DELETE(pMsg);
	}

	return;
}

void CNpcWorld::OnTimer(DWORD nCurr)
{
	if(!m_pShell->IsConnectOK())
		return ;

	if(m_nLoginState == STATE_NORMAL)
	{
		if(m_timeoutLogin.IsActive())
		{
			if(m_timeoutLogin.IsTimeOut(LOGIN_DELAY_SECS))
			{
				m_timeoutLogin.Clear();
				PrintText("Generator begin.");
			}
			return ;
		}

		m_pMapManager->OnTimer(nCurr);
		m_pUserManager->OnTimer(nCurr);
		m_pNpcManager->OnTimer(nCurr);

		if(m_tUpdateStateWin.ToNextTime())
		{
			int nAmount		= 0;
			int nGenAmount	= 0;
			int nMaxNpc		= 0;
			int	nGenActive	= 0;
			int	nGenFull	= 0;
			int	nGenOver	= 0;
			for(int i = 0; i < NpcManager()->QueryGenSet()->GetAmount(); i++)
			{
				CGenPtr pGen = NpcManager()->QueryGenSet()->GetObjByIndex(i);
				if(pGen)
				{
					if(pGen->GetMaxNpc() > pGen->GetGenAmount())
						nGenActive++;
					else if(pGen->GetMaxNpc() == pGen->GetGenAmount())
						nGenFull++;
					else
						nGenOver++;
					nAmount		+= pGen->GetAmount();
					nGenAmount	+= pGen->GetGenAmount();
					nMaxNpc		+= pGen->GetMaxNpc();
				}
			}
			int	nUserAmount = 0, nPetAmount = 0, nOtherAmount = 0;
			UserManager()->GetUserAmount(&nUserAmount, &nPetAmount, &nOtherAmount);

			char	buf[STATESTR_SIZE];
			_snprintf(buf, sizeof(buf), "Player + Pet + Other: %u + %u + %u\n"
							"NPC active / total: %u / %u\n"
							"NPC: in game / generated / limit\n\t%d / %d / %d\n"
							"Generator: active+full+over / total\n\t%u + %u + %u / %u",
				nUserAmount, nPetAmount, nOtherAmount,
				NpcManager()->CountActiveNpc(), NpcManager()->QuerySet()->GetAmount(),
				nAmount, nGenAmount, nMaxNpc,
				nGenActive, nGenFull, nGenOver, NpcManager()->QueryGenSet()->GetAmount());
			m_pShell->SetState(buf);
		}
	}
	else if(m_nLoginState == STATE_LOGINING)
	{
		if(m_timeoutLogin.ToNextTime(RESEND_MSGLOGIN_SECS))
		{
			PrintText("Login game server . . .");
		}
	}
	else if(m_nLoginState == STATE_NONE)
	{
		m_nLoginState = STATE_LOGINING;

		CMsgLogin	msg;
		if(msg.Create(0, NPCSERVER_ACCOUNT, NPCSERVER_PASSWORD))
			SendMsg(NPCSERVER_VERSION, &msg);

		LOGDUMP("CMsgLogin STATE_NONE");
	}
	else if(m_nLoginState == STATE_BREAK)
	{
		m_nLoginState = STATE_LOGINING;

		CMsgLogin	msg;
		if(msg.Create(0, NPCSERVER_ACCOUNT, NPCSERVER_PASSWORD))
			SendMsg(NPCSERVER_VERSION_RELOGIN, &msg);

		LOGDUMP("CMsgLogin STATE_BREAK");
	}

	else
		MYASSERT(!"CNpcWorld::OnTimer");
}

void CNpcWorld::ShellCommond(LPCTSTR)
{
}

bool CNpcWorld::SendMsg(OBJID idNpc, CNetMsg* pMsg)
{
	NETMSGBUF	buf;
	int			nNetPacketSize = UniteNpcPacket(buf, sizeof(buf), pMsg->GetType(), pMsg->GetBuf(), pMsg->GetSize(), idNpc);
	MYASSERT(nNetPacketSize);
	if(nNetPacketSize)
	{

		DEBUG_TRY
		if(!m_pShell->SendPacket(buf, nNetPacketSize))
			SetLoginState(STATE_BREAK);
		DEBUG_CATCH("m_pShell->SendPacket()")

		return true;
	}

	return false;
}

void CNpcWorld::LoginOK()
{
	m_nLoginState = STATE_NORMAL;
	m_timeoutLogin.Update();

	char	szMsg[1024];
	sprintf(szMsg, "Login OK. generator monster after %d secs", LOGIN_DELAY_SECS);
	PrintText(szMsg);
	LOGDUMP(szMsg);
}
