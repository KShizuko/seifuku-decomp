#include "AllMsg.h"
#include "mapgroup.h"
#include "User.h"
#include "AiNpc.h"

const int	STATUARY_LIFE_SCALE					= 100;
CMsgPlayer::CMsgPlayer()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-offsetof(MSG_Info, szBuf));
}

CMsgPlayer::~CMsgPlayer()
{

}

BOOL CMsgPlayer::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_PLAYER != this->GetType())
		return false;

	return true;
}

BOOL CMsgPlayer::Create(IRole* pRole, bool bQuery)
{
	if (!pRole)
		return false;

	this->Init();

	m_pInfo->id			= pRole->GetID();
	m_pInfo->dwStatus[0]	= pRole->GetEffect(0);
	m_pInfo->dwStatus[1]	= pRole->GetEffect(1);
	m_pInfo->dwLookFace	= pRole->GetLookFace();
	m_pInfo->usHair		= (USHORT)pRole->GetHair();
	m_pInfo->dwSynID_Rank	= (pRole->GetSynRankShow()<<MASK_RANK_SHIFT) | (pRole->GetSynID()&MASK_SYNID);
	m_pInfo->dwMountType	= pRole->GetMountTypeID();
	m_pInfo->dwHelmetType	= pRole->GetHelmetTypeID();
	m_pInfo->dwArmorType	= pRole->GetArmorTypeID();
	m_pInfo->dwWeaponRType	= pRole->GetWeaponRTypeID();
	m_pInfo->dwWeaponLType	= pRole->GetWeaponLTypeID();

	CUser* pUser = NULL;
	if (pRole->QueryObj(OBJ_USER, IPP_OF(pUser)))
	{
		CItem* pItem = pUser->GetEquipItemByPos(ITEMPOSITION_WEAPONR);
		m_pInfo->usWeaponRAntiMonster	= pItem ? (USHORT)pItem->GetInt(ITEMDATA_ANTI_MONSTER) : 0;
		pItem = pUser->GetEquipItemByPos(ITEMPOSITION_WEAPONL);
		m_pInfo->usWeaponLAntiMonster	= pItem ? (USHORT)pItem->GetInt(ITEMDATA_ANTI_MONSTER) : 0;
		m_pInfo->ucMetempsychosis		= pUser->GetMetempsychosis();

	}
	else
	{
		m_pInfo->dwSpriteType	= pRole->GetSpriteTypeID();
	}

	if (IsSceneID(pRole->GetID()))
	{
		CNpc* pNpc;
		IF_OK(pRole->QueryObj(OBJ_NPC, IPP_OF(pNpc)))
		{
			if(pNpc->GetType()==_STATUARY_NPC)
			{
				m_pInfo->usStatuaryLife	 = pRole->GetLife() / STATUARY_LIFE_SCALE;
				m_pInfo->usStatuaryFrame = pNpc->GetInt(STATUARYDATA_FRAME);
			}
			else
			{
				m_pInfo->usLife	 = pRole->GetLife();
				m_pInfo->usLevel = pNpc->GetInt(NPCDATA_MAXLIFE);
			}
		}
	}
	else if (IsNpcID(pRole->GetID()) || pRole->GetID() >= CALLPETID_FIRST && pRole->GetID() <= CALLPETID_LAST)
	{
		m_pInfo->usLife	 = pRole->GetLife();
		m_pInfo->usLevel = pRole->GetLev();
	}
	else
	{
		m_pInfo->dwMantleType	= pRole->GetMantleTypeID();
	}

	(void)bQuery;

	m_pInfo->usPosX		= pRole->GetPosX();
	m_pInfo->usPosY		= pRole->GetPosY();
	m_pInfo->ucDir		= pRole->GetDir();
	m_pInfo->ucPose		= pRole->GetPose();

	BOOL bSucMake	=true;
	bSucMake	&=m_StrPacker.AddString((char*)pRole->GetName());

	m_unMsgType	=_MSG_PLAYER;
	m_unMsgSize	=offsetof(MSG_Info, szBuf)+m_StrPacker.GetSize();

	return true;
}

void CMsgPlayer::Process(void *pInfo)
{
	return;
	CUser* pUser = g_UserManager.GetUser(this);
	pUser->BroadcastRoomMsg(this, EXCLUDE_SELF);

#ifdef _MSGDEBUG
	::LogMsg("Process CMsgPlayer, id:%u", m_pInfo->id);
#endif

	char szName[_MAX_NAMESIZE]		="";
	char szMate[_MAX_NAMESIZE]		="";
	char szSyndicate[_MAX_NAMESIZE]	="";
	char szTitle[_MAX_NAMESIZE]		="";

	m_StrPacker.GetString(0, szName, _MAX_NAMESIZE);
	m_StrPacker.GetString(1, szMate, _MAX_NAMESIZE);
	m_StrPacker.GetString(2, szSyndicate, _MAX_NAMESIZE);
	m_StrPacker.GetString(3, szTitle, _MAX_NAMESIZE);
}
