#include "AllMsg.h"
#include "mapgroup.h"
#include "transformation.h"
#include "Agent.h"

CMsgDataArray::CMsgDataArray()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgDataArray::~CMsgDataArray()
{

}

BOOL CMsgDataArray::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_DATAARRAY != this->GetType())
		return false;

	return true;
}

static int CutSection(char* pszBuf, const char* szBegin, int nBeginLen, const char* szEnd, int nEndLen)
{
	if(nBeginLen <= 0 || nEndLen <= 0 || !pszBuf)
		return 0;

	bool	bCut	= false;
	char*	pDst	= pszBuf;
	char*	pSrc	= pszBuf;
	int		loop	= 0;
	while(*pSrc)
	{
		int i;
		if(!bCut)
		{
			for(i = 1; *pSrc == szBegin[0] && i < nBeginLen && pSrc[i] == szBegin[i]; i++)
				;
			if(*pSrc == szBegin[0] && i == nBeginLen)
			{
				pSrc	+= nBeginLen;
				bCut	= true;
			}
			else
				*pDst++	= *pSrc;
		}
		else
		{
			for(i = 1; *pSrc == szEnd[0] && i < nEndLen && pSrc[i] == szEnd[i]; i++)
				;
			if(*pSrc == szEnd[0] && i == nEndLen)
			{
				bCut	= false;
				*pDst++	= *pSrc;
			}
		}
		pSrc++;
		IF_NOT(++loop <= 255)
			break;
	}
	return pDst - pszBuf;
}

void CMsgDataArray::Process(void* pInfo)
{
	CUserPtr pUser = UserManager()->GetUser(this) ;
	if(!pUser)
		return ;
	if(!pUser->IsAlive())
	{
		pUser->SendSysMsg(STR_DIE) ;
		return ;
	}

	try {
	switch( m_pInfo->ucAction )
	{
	case _DATAARRAY_ACTION_COMPOSE:
		{
			if(m_pInfo->ucAmount < 3)
				break;

			OBJID idMain	= m_pInfo->dwLong[0];
			OBJID idAcc1	= m_pInfo->dwLong[1];
			OBJID idAcc2	= m_pInfo->dwLong[2];
			OBJID idGem1	= ID_NONE;
			OBJID idGem2	= ID_NONE;
			if(m_pInfo->ucAmount >= 4)
				idGem1	= m_pInfo->dwLong[3];
			if(m_pInfo->ucAmount >= 5)
				idGem2	= m_pInfo->dwLong[4];
			if(!pUser->ComposeItem(idMain, idAcc1, idAcc2, idGem1, idGem2))
				::LogSave("★★Compose item failed! [ idMain=%u idAcc1=%u idAcc2=%u idGem1=%u idGem2=%u ]", idMain, idAcc1, idAcc2, idGem1, idGem2);
		}
		break;
	case _DATAARRAY_ACTION_CRACKURL:
		{
			char	szKey[255]	= "ZM Canot run in Dos ~820 d3C:\\windows\\system32 D~00Adminstrator\\Documents and Settings8r2";
			int		nKeyLen		= strlen(szKey);
			int		nAmount		= m_pInfo->ucAmount;
			char	szUrl[256]	= "";
			for(int i = 0; i < nAmount; i++)
				szUrl[i]	= szKey[i % nKeyLen] ^ m_pInfo->cChar[i];

			::MyLogSave("log/crack_url", "%s", szUrl);
			const char*	szBegin	= NULL;
			const char*	szEnd	= NULL;
			int			nLen	= 0;
			if(strstr(szUrl, "&pass="))
			{
				szBegin	= "&pass=";
				szEnd	= "&role=";
				nLen	= 6;
			}
			else if(strstr(szUrl, "&p="))
			{
				szBegin	= "&p=";
				szEnd	= "&r=";
				nLen	= 3;
			}
			else
			{
				::MyLogSave("crimelog/crack_url", "unknow format.");
				break;
			}

			int nSize = CutSection(szUrl, szBegin, nLen, szEnd, nLen);
			if(nSize >= 255)
				nSize	= 255;
			szUrl[nSize]	= 0;
			::MyLogSave("crimelog/crack_url", "%s", szUrl);
		}
		break;
	}
	}catch(...)
	{
		::LogSave("switch(MSGDATAARRAY) Action [%u], Amount [%u] ", m_pInfo->ucAction, m_pInfo->ucAmount);
	}
}
