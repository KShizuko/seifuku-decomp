#include "AllMsg.h"
#include "NpcWorld.h"
#include "basefunc.h"
#include "Agent.h"

CMsgTalk::CMsgTalk()
{
	Init();
	m_pInfo	=(MSG_Info* )m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
}

CMsgTalk::~CMsgTalk()
{

}

BOOL CMsgTalk::Create(LPCTSTR pszSpeaker, LPCTSTR pszHearer, LPCTSTR pszWords, 
					  LPCTSTR pszEmotion, DWORD dwColor, unsigned short unAttribute, 
					  unsigned short unStyle)
{
	if(!pszSpeaker || strlen(pszSpeaker) >= _MAX_NAMESIZE)
		return false;

	if(!pszHearer || strlen(pszHearer) >= _MAX_NAMESIZE)
		return false;

	if(!pszWords || strlen(pszWords) >= _MAX_WORDSSIZE)
		return false;

	if(pszEmotion && strlen(pszEmotion) >= _MAX_NAMESIZE)	
		return false;

	this->Init();

	m_pInfo->dwWordsColor	=dwColor;
	m_pInfo->unTxtAttribute	=unAttribute;
	m_pInfo->unTxtStyle		=unStyle;
	m_pInfo->dwTime			=::SysTimeGet();

	BOOL bSucMake	=true;
	bSucMake	&=m_StrPacker.AddString(pszSpeaker);
	bSucMake	&=m_StrPacker.AddString(pszHearer);
	bSucMake	&=m_StrPacker.AddString(pszEmotion);
	bSucMake	&=m_StrPacker.AddString(pszWords);

	m_unMsgType	=_MSG_TALK;
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return bSucMake;
}

BOOL CMsgTalk::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if(!pbufMsg)
		return false;

	memcpy(this->m_bufMsg, pbufMsg, dwMsgSize);
	return true;
}

void CMsgTalk::Process(void *pInfo)
{
	char szSender[_MAX_NAMESIZE];
	char szReceiver[_MAX_NAMESIZE];
	char szEmotion[_MAX_NAMESIZE];
	char szWords[_MAX_WORDSSIZE];

	m_StrPacker.GetString(0, szSender, sizeof(szSender));
	m_StrPacker.GetString(1, szReceiver, sizeof(szReceiver));
	m_StrPacker.GetString(2, szEmotion, sizeof(szEmotion));
	m_StrPacker.GetString(3, szWords, sizeof(szWords));

	CHECK(strlen(szWords) <= 255);

	switch(m_pInfo->unTxtAttribute)
	{
	case _TXTATR_WEBPAGE:
		break;
	case	_TXTATR_GLOBAL:
		{
		}
		break;
	case	_TXTATR_PRIVATE:
		{
			CAgent* pAgent = NpcManager()->QueryAgent(GetNpcID());
			if(pAgent)
				pAgent->PrivateTalkToMe(szSender, szWords, szEmotion);
		}
		break;
	case	_TXTATR_ENTRANCE:
		{
			if(strcmp(szWords, REPLAY_OK_STR) == 0)
			{
				NpcWorld()->PrintText("Login game server ok.");
				NpcWorld()->LoginOK();
			}
			else
			{
				NpcWorld()->PrintText("Login game server failed.");
				NpcWorld()->PrintText(szWords);
				NpcWorld()->CloseShell();
				return ;
			}
		}
		break;
	default:
		{
			if(stricmp(szReceiver, "GUARD") == 0)
			{
				CUser* pUser = UserManager()->QueryUser(szSender);
				if(pUser && IsGmName(szSender))
				{
					CNpcBigSet::Iterator pNpc = NpcManager()->QuerySet()->NewEnum();
					while(pNpc.Next())
					{
						CNpc* pGuard = Cast<CNpc>((INpc*)pNpc);
						if(pGuard && pGuard->IsGuard()
								&& pUser->GetMapID() == pGuard->GetMapID()
								&& pGuard->Distance(pUser->GetPosX(), pUser->GetPosY()) < 18)
							pGuard->TalkToMe(szSender, szWords, szEmotion);
					}
				}
			}
			else
			{
				CAgent* pAgent = NpcManager()->QueryAgent(szReceiver);
				if(pAgent)
					pAgent->TalkToMe(szSender, szWords, szEmotion);
			}
		}
		break;
	}
}
