#include "Hero.h"
#include "AllMsg.h"
#include "GameMsg.h"
#include "GamePlayerSet.h"
#include "GameDataSet.h"

void CHero::SetLastSyndicateApplyID(OBJID idApply)
{
	m_idLastSyndicateApply = idApply;
}
void CHero::SyndicateApplyJoin(OBJID idTarget)
{
	if(this->GetSydicateID() != ID_NONE)
	{
		OBJID idLeaveSyndicate = 100065;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idLeaveSyndicate));
		return;
	}
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(idTarget);
	{
		if(!pPlayer)
		{
			OBJID idErrorTarget = 100066;
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idErrorTarget));
			return;
		}
	}
	if(pPlayer->GetSydicateID() == ID_NONE)
	{
		char szMsg[128];
		const OBJID idCanNotJion = 100067;
		sprintf(szMsg, GameDataSetQuery()->GetStr(idCanNotJion), pPlayer->GetName());
		g_objGameMsg.AddMsg(szMsg);
		return;
	}

	CMsgSyndicate msg;
	if(msg.Create(APPLY_JOIN, idTarget))
	{
		msg.Send();
		OBJID idApplicationSend = 100069;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idApplicationSend));
	}
}
void CHero::SynDicateAcceptJoin()
{
	CMsgSyndicate msg;
	if(msg.Create(INVITE_JOIN, m_idLastSyndicateApply))
		msg.Send();
}
void CHero::SyndicateApplyInvite(OBJID idTarget)
{
	if(this->GetSydicateID() == ID_NONE)
	{
		OBJID idPleaseJoinSyndicateFirst = 100070;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idPleaseJoinSyndicateFirst));
		return;
	}
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(idTarget);
	{
		if(!pPlayer)
		{
			OBJID idErrorTarget = 100072;
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idErrorTarget));
			return;
		}
	}
	if(pPlayer->GetSydicateID() != ID_NONE)
	{
		OBJID idTargetHaveSyndicate = 100073;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idTargetHaveSyndicate));
		return;
	}
	CMsgSyndicate msg;
	if(msg.Create(INVITE_JOIN, idTarget))
		msg.Send();
}
void CHero::SyndicateAcceptInvite()
{
	CMsgSyndicate msg;
	if(msg.Create(APPLY_JOIN, m_idLastSyndicateApply))
		msg.Send();
}
void CHero::SetLastSyndicateID(OBJID idSynDicate)
{
	m_idLastSyndicate = idSynDicate;
}
OBJID CHero::GetLastSyndicateID()
{
	return m_idLastSyndicate;
}
void CHero::LeaveSyndicate()
{
	if(this->GetSydicateID() == ID_NONE)
		return;

	CONST int MINPROFFER_QUITSYN = 20000;
	if(this->GetSyndicateProffer() < MINPROFFER_QUITSYN)
		return;

	CMsgSyndicate msg;
	if(msg.Create(LEAVE_SYN, m_idLastSyndicateApply))
		msg.Send();
}
void CHero::SynContribute(DWORD dwMoney)
{
	if(this->GetSydicateID() == ID_NONE)
	{
		OBJID idHaveNoSynDicate = 100074;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idHaveNoSynDicate));
		return;
	}
	if(this->GetMoney() < dwMoney)
	{
		OBJID idMoneyNotEnough = 100075;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idMoneyNotEnough));
		return;
	}
	CMsgSyndicate msg;
	if(msg.Create(DONATE_MONEY, dwMoney))
		msg.Send();
}
void CHero::SetSyndicateAnnounce(char* pszWord)
{
	if(pszWord)
	{
		strcpy(m_szSyndicateAnnounce, pszWord);
		::PostCmd(CMD_FLASH_SYNDICATE_WORD);
	}
}
char* CHero::GetSyndicateAnnounce()
{
	return m_szSyndicateAnnounce;
}
void CHero::QuerySynicateInfo()
{
	CMsgSyndicate msg;
	if(msg.Create(QUERY_SYNATTR, this->GetID()))
		msg.Send();
}
int		CHero::GetSynWarReportAmount ()
{
	return m_dequeSynWarReport.size();
}
char*	CHero::GetSynWarReportLine(int nIndex)
{
	int nAmount = m_dequeSynWarReport.size();
	if(nIndex >= nAmount && nIndex < 0)
		return NULL;
	return m_dequeSynWarReport[nIndex];
}
void	CHero::ClearSynWarReport()
{
	int nAmount = m_dequeSynWarReport.size();
	for(int i = 0; i < nAmount; i ++)
	{
		char* pszChar = m_dequeSynWarReport[i];
		delete[] pszChar;
	}
	m_dequeSynWarReport.clear();
}
void	CHero::AddSynWarReport(char* pszLine)
{
	if(!pszLine)
		return;
	if(strlen(pszLine) >= 512)
		return;
	char* pszTemp = new char[512];
	strcpy(pszTemp, pszLine);
	m_dequeSynWarReport.push_back(pszTemp);
	m_dwTimeSynWarReportLastFlash = ::TimeGet();
}
void	CHero::ShowSynWarReport()
{
	const DWORD dwDelay = 60 * 1000;
	if(::TimeGet() - m_dwTimeSynWarReportLastFlash > dwDelay)
		return;
	CMyPos posShow = {CMyBitmap::GetScreenWidth()-300, 5};
	CMySize sizeFont;
	CMyBitmap::GetFontSize(sizeFont);
	int nAmount = this->GetSynWarReportAmount();
	for(int i = 0; i < nAmount; i ++)
	{
		char* pszLine = this->GetSynWarReportLine(i);
		if(pszLine)
		{
			posShow.y += sizeFont.iHeight +5;
			CMyBitmap::ShowString(posShow.x, posShow.y, _COLOR_YELLOW, pszLine);
		}
	}
}
void	CHero::SendSyndicateAnnounce(char* pszStr)
{
	if(!pszStr)
		return;
	CMsgTalk msg;
	if(msg.Create((char*)g_objHero.GetName(), "大家", pszStr, NULL, 0xffffff, _TXTATR_SYNANNOUNCEG, _TXT_NORMAL))
		msg.Send();
}
void	CHero::QuerySynMemberList(int nIndex)
{
	CMsgName msg;
	if(msg.Create(NAMEACT_MEMBERLIST, NULL, nIndex == 0 ? -1 : m_nSynListIndex))
		msg.Send();
}
int CHero::GetSynMemberAmount()
{
	return m_setSyndicateMember.size();
}
SynMemberInfo* CHero::GetSynMemberByIndex(int nIndex)
{
	int nAmount = m_setSyndicateMember.size();
	if(nIndex < 0 || nIndex >= nAmount)
		return NULL;
	 return m_setSyndicateMember[nIndex];
}
void CHero::ClearSynMember()
{
	int nAmount = m_setSyndicateMember.size();
	for(int i = 0; i < nAmount; i ++)
	{
		SynMemberInfo* pInfo = m_setSyndicateMember[i];
		SAFE_DELETE(pInfo);
	}
	m_setSyndicateMember.clear();
}
void CHero::AddSynMember(char* pszMember)
{
	if(!pszMember)
		return;
	char szTemp[256];
	SynMemberInfo* pInfo = new SynMemberInfo;
	MYASSERT(pInfo);
	sscanf(pszMember, "%s %d %d", szTemp, &pInfo->ucRank , &pInfo->bOnLine);
	if(strlen(szTemp) < _MAX_NAMESIZE)
		strcpy(pInfo->szName, szTemp);
	else
		strcpy(pInfo->szName, "overflow");
	strcpy(pInfo->szRank, "...");
	strcpy(pInfo->szProffer, "...");

	m_setSyndicateMember.push_back(pInfo);
}
void CHero::KickoutSynMember(char* pszTargetName)
{
	if(!pszTargetName)
		return;
	CMsgName msg;
	if(msg.Create(MANEACT_KICKOUT_SYNMEM, pszTargetName))
		msg.Send();
}
void CHero::UniteSubSyn(OBJID idSource, OBJID idTarget)
{
	if(g_objHero.GetSydicateID() == idSource)
	{
		g_objHero.SetSyndicateID(idTarget);
		g_objHero.SetSyndicateRank(RANK_SAN);
	}
	int nAmount = g_objPlayerSet.GetPlayerAmount();
	for(int i = 0; i < nAmount; i ++)
	{
		CPlayer* pPlayer = g_objPlayerSet.GetPlayerByIndex(i);
		if(pPlayer && pPlayer->GetSydicateID() == idSource)
		{
			pPlayer->SetSyndicateID(idTarget);
		}
	}
	g_objPlayerSet.UpdateSyndicateName(idTarget);
}
void CHero::UniteSyn(OBJID idSource, OBJID idTarget)
{
}
void CHero::QuerySynMember(char* pszName)
{
	if(!pszName)
		return;
	strcpy(m_objSynMemberInfo.szName, pszName);
	strcpy(m_objSynMemberInfo.szRank, "...");
	strcpy(m_objSynMemberInfo.szProffer, "...");

	CMsgSynMemberInfo msg;
	if(msg.Create(pszName))
		msg.Send();
}
