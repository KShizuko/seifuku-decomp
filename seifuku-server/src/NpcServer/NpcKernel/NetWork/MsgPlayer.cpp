#include "AllMsg.h"
#include "NpcWorld.h"

CMsgPlayer::CMsgPlayer()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;

	m_StrPacker.SetBuf(m_pInfo->szBuf, _MAX_MSGSIZE-sizeof(MSG_Info)+1);
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

void CMsgPlayer::Process(void *pInfo)
{
#ifdef _MSGDEBUG
	::LogMsg("Process CMsgPlayer, id:%u", m_pInfo->id);
#endif

	if(UserManager()->QuerySet()->DelObj(m_pInfo->id))
	{
	}

	char szName[_MAX_NAMESIZE]		="";

	m_StrPacker.GetString(0, szName, _MAX_NAMESIZE);

	ST_USERINFO	info;
	memset(&info, 0, sizeof(ST_USERINFO));
	info.id				= m_pInfo->id;
	SafeCopy(info.szName, szName, _MAX_NAMESIZE);
	info.dwLookFace		= m_pInfo->dwLookFace;
	info.usHair			= m_pInfo->usHair;

	info.i64Effect		= m_pInfo->dwEffect[0];
	info.idSyn			= m_pInfo->dwSynID_Rank & MASK_SYNID;
	info.nRank			= (UCHAR)m_pInfo->dwSynID_Rank >> MASK_RANK_SHIFT;
	info.dwHelmetType	= m_pInfo->dwHelmetType;
	info.dwArmorType	= m_pInfo->dwArmorType;
	info.dwWeaponRType	= m_pInfo->dwWeaponRType;
	info.dwWeaponLType	= m_pInfo->dwWeaponLType;
	info.dwMountType	= m_pInfo->dwMountType;
	info.dwMedalSelect	= m_pInfo->dwMedalSelect;

	info.usPosX			= m_pInfo->usPosX;
	info.usPosY			= m_pInfo->usPosY;
	info.ucDir			= m_pInfo->ucDir;
	info.ucPose			= m_pInfo->ucPose;

	info.ucSizeAdd		= 0;
	int nTransLook	= info.dwLookFace / MASK_CHANGELOOK;
	if(nTransLook > 0)
		info.ucSizeAdd	= CUserManager::GetSizeAdd(nTransLook);

	CNpcPtr pNpc = NpcManager()->QueryNpc(this->GetNpcID());
	IF_NOT(pNpc)
	{
		LOGWARNING("CMsgPlayer: no NPC to receive the message; the game server may have restarted.");
		return;
	}

	if(NpcManager()->QueryNpc(info.id))
		return ;

	OBJID idMap = pNpc->GetMapID();
	MYASSERT(idMap);
	if(idMap == ID_NONE)
		return;

	CUserPtr pUser = CUser::CreateNew();
	MYASSERT(pUser);
	if(pUser->Create(&info, idMap))
	{
		UserManager()->QuerySet()->AddObj(pUser);
		return ;
	}
	else
	{
		pUser->ReleaseByOwner();
		MYASSERT(!"pUser->Create()");
	}
}
