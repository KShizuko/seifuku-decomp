#include "AllMsg.h"
#include "mapgroup.h"
#include "WantedList.h"
#include "PoliceWanted.h"
#include "BetOutside.h"

CMsgName::CMsgName()
{
	Init();
	m_pInfo	=(MSG_Info* )m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
}

CMsgName::~CMsgName()
{

}

BOOL CMsgName::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_NAME != this->GetType())
		return false;

	return true;
}

BOOL CMsgName::Create(int nType, const char* pszName, DWORD dwData )
{
	if (nType == NAMEACT_NONE || !pszName)
		return false;

	this->Init();

	m_pInfo->ucType	= nType;
	m_pInfo->dwData	= dwData;

	m_StrPacker.AddString(pszName);

	m_unMsgType	=_MSG_NAME;
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

BOOL CMsgName::Create(int nType, const char* pszName, USHORT usPosX, USHORT usPosY)
{
	if (nType == NAMEACT_NONE || !pszName)
		return false;

	this->Init();

	m_pInfo->ucType	= nType;
	m_pInfo->usPosX	= usPosX;
	m_pInfo->usPosY	= usPosY;

	m_StrPacker.AddString(pszName);

	m_unMsgType	=_MSG_NAME;
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

BOOL CMsgName::	AppendStr(const char* pszName)
{
	CHECKF(pszName);

	m_StrPacker.AddString(pszName);
	m_unMsgSize	=sizeof(MSG_Info)-1+m_StrPacker.GetSize();

	return true;
}

void CMsgName::Process(void *pInfo)
{
	CUserPtr pUser = UserManager()->GetUser(this);
	if(!pUser)
		return;

	char szName[_MAX_WORDSSIZE];
	m_StrPacker.GetString(0, szName, sizeof(szName));

	switch(m_pInfo->ucType)
	{
	case	NAMEACT_QUERY_WANTED:
		{
			CWantedData* pWanted = CWantedList::s_WantedList.GetWanted(pUser->WantedInfo().idWanted);
			if (pWanted)
			{
				CUser* pTarget = UserManager()->GetUser(pWanted->GetStr(DATA_TARGET_NAME));
				if (pTarget && pUser->FindAroundUser(pTarget->GetID()))
					pUser->SendSysMsg(_TXTATR_NORMAL, STR_FOND_WANTED, pTarget->GetName());
			}
		}
		break;

	case	NAMEACT_QUERY_POLICEWANTED:
		{
			OBJID idWanted = pUser->PoliceWantedID();
			PoliceWantedStruct* pInfo = PoliceWanted().GetWanted(idWanted);

			if (pUser->FindAroundUser(idWanted) && pInfo)
				pUser->SendSysMsg(_TXTATR_NORMAL, STR_FOND_POLICEWANTED, pInfo->strName.c_str());
		}
		break;

	case	NAMEACT_FIREWORKS:
		{
			pUser->BroadcastRoomMsg(this, EXCLUDE_SELF);
		}
		break;
	case	NAMEACT_CREATE_SYN:
		{
			ASSERT(!"NAMEACT_CREATE_SYN");
		}
		break;
	case	NAMEACT_SYNDICATE:
		{
			ASSERT(!"NAMEACT_SYNDICATE");
		}
		break;
	case	NAMEACT_CHANGE_TITLE: 
		{
			ASSERT(!"NAMEACT_CHANGE_TITLE");
		}
		break;
	case	NAMEACT_DELROLE:
		{
			ASSERT(!"NAMEACT_DELROLE");
		}
		break;
	case	NAMEACT_MATE:
		{
			ASSERT(!"NAMEACT_MATE");
		}
		break;
	case	NAMEACT_QUERY_NPC:
		{
			OBJID idNpc = m_pInfo->idTarget;
			if(idNpc == ID_NONE)
				return ;
			CNpc* pNpc;
			IRole* pRole = RoleManager()->QuerySet()->GetObj(idNpc);
			if (pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
			{
				Create(NAMEACT_QUERY_NPC, pNpc->GetName(), idNpc);
				pUser->SendMsg(this);
			}
		}
		break;
	case	NAMEACT_MEMBERLIST:
		{
			OBJID idSyn = pUser->GetSynID();
			CHECK(idSyn != ID_NONE);
			SetTransData(idSyn);
			MapGroup(PID)->QueryIntraMsg()->TransmitWorldMsg(this);
		}
		break;
	case	NAMEACT_QUERY_MATE:
		{
			CUser* pTarget = UserManager()->GetUser(m_pInfo->idTarget);
			if(pTarget)
			{
				IF_OK(Create(NAMEACT_QUERY_MATE, pTarget->GetMate(), m_pInfo->idTarget))
					SendMsg(this);
			}
		}
		break;
	case	NAMEACT_ADDDICE_PLAYER:
		{
			IRole* pRole = pUser->FindAroundRole(m_pInfo->idTarget);
			CNpc* pNpc;
			if(pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsBetNpc())
			{
				pNpc->QueryBet()->JoinBet(pUser);
			}
		}
		break;
	case	NAMEACT_DELDICE_PLAYER:
		{
			CMsgName	msg;
			IF_OK(msg.Create(NAMEACT_DELDICE_PLAYER, pUser->GetName(), m_pInfo->idTarget))
				pUser->SendMsg(&msg);

			IRole* pRole = RoleManager()->QueryRole(m_pInfo->idTarget);
			CNpc* pNpc;
			if(pRole && pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)) && pNpc->IsBetNpc())
			{
				pNpc->QueryBet()->LeaveBet(pUser);
			}
		}
		break;
	case	NAMEACT_QUERY_PLAYER:
		{
			if (szName[0] == 0)
				break;
			CUser* pTarget = UserManager()->GetUser(szName);
			if (!pTarget)
				break;

			char szSyn[_MAX_NAMESIZE] = "";
			CSyndicate* pSyn = pTarget->GetSyndicate();
			if (pSyn)
				SafeCopy(szSyn, pSyn->GetStr(SYNDATA_NAME), _MAX_NAMESIZE);
			char szUnknown[_MAX_NAMESIZE] = "";
			char szBuf[256] = "";
			_snprintf(szBuf, 256, "%u %d %d #%s #%s %s %d %d", pTarget->GetID(), pTarget->GetLev(), pTarget->GetBattlePower(),
						szSyn, szUnknown, pTarget->GetMate(), pTarget->GetNobilityRank(), pTarget->GetSex());
			this->AppendStr(szBuf);
			pUser->SendMsg(this);
		}
		break;
	case	NAMEACT_SET_LAYOUT:
		{
			if (!pUser->IsGM() || szName[0] == 0)
				break;
			CUser* pTarget = UserManager()->GetUser(szName);
			if (pTarget)
				pTarget->QueryDataForAction()->SetCurrentLayoutType(true);
			else
				pUser->SendSysMsg("用户不存在!");
		}
		break;
	case	MANEACT_KICKOUT_SYNMEM:
		{
			if (szName[0] == 0 || strlen(szName) >= _MAX_NAMESIZE || !::NameStrCheck(szName))
				break;

			OBJID idSyn = pUser->GetSynID();
			int	  nRank = pUser->GetSynRank();
			if(idSyn == ID_NONE || nRank != RANK_LEADER)
				break;
			if(strcmp(szName, pUser->GetName()) == 0)
				break;

			CSyndicate* pSyn = SynManager()->QuerySyndicate(idSyn);
			if(pSyn && pSyn->GetInt(SYNDATA_MONEY) < SYN_MONEY_BASELINE)
			{
				pUser->SendSysMsg(STR_TOO_LOWER_SYN_MONEY);
				break;
			}

			MSGBUF	szMsg;
			sprintf(szMsg, STR_KICKOUT_MEMBER_s, szName);
			if(pSyn)
				pSyn->BroadcastSubSynMsg(pUser->GetSocketID(), pUser->GetID(), szMsg);

			CUser* pTarget = UserManager()->GetUser(szName);
			if(pTarget)
			{
				pTarget->SendSysMsg(STR_KICKOUT_ME_s, pUser->GetName());
				if(pTarget->GetSynID() == idSyn)
					pTarget->QuerySynAttr()->LeaveSyn(true, true);
			}
			else
			{
				pUser->QuerySynAttr()->KickoutMember(szName);
			}
		}
		break;
	}
}
