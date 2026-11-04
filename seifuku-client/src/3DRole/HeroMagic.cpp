#include "Hero.h"
#include "MsgInteract.h"
#include "GamePlayerSet.h"
#include "GameMsg.h"
#include "GameDataSet.h"
#include "3DGameMap.h"
#include "AllMsg.h"
#include "3DRoleData.h"
#include "NDsound.h"
void CHero::DestroyMagicSet()
{
	int nAmount = m_setMagic.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = m_setMagic[i];
		SAFE_DELETE(pMagic);
	}

	m_setMagic.clear();
}
void CHero::AddMagic(OBJID idMagic, DWORD dwLevel, DWORD dwExp)
{
	int nAmount = m_setMagic.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = m_setMagic[i];
		if(pMagic && (pMagic->m_infoMagicType.idMagicType == idMagic))
		{
			MagicTypeInfo infoMagicType;
			if(CMagic::s_objMagicData.GetMagicTypeInfo(idMagic, dwLevel, infoMagicType))
			{
				pMagic->m_infoMagicType = infoMagicType;
				pMagic->m_dwExp = dwExp;
				const CMediaEffectInfo* pMedia = RoleDataQuery()->GetMediaEffectInfo(idMagic * 100 + dwLevel);
				if (pMedia)
				{
					if (pMedia->strEffect.size() > 0)
					{
						if (m_objEffect)
							m_objEffect->Add((char*)pMedia->strEffect.c_str());
					}
					if (pMedia->strSound.size() > 0)
						::DXPlaySound((char*)pMedia->strSound.c_str(), 0, 0, 0, 0, 100);
				}
			}
			return;
		}
	}
	CMagic* pMagic = CMagic::CreateNew(idMagic, dwLevel, dwExp);
	if (pMagic)
		m_setMagic.push_back(pMagic);
}
void CHero::DelMagic(OBJID idMagicType)
{
	int nAmount = m_setMagic.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = m_setMagic[i];
		if(pMagic && pMagic->m_infoMagicType.idMagicType == idMagicType)
		{
			m_setMagic.erase(m_setMagic.begin() + i);
			SAFE_DELETE(pMagic);
			::PostCmd(CMD_FLASH_MAGIC);
			return;
		}
	}
}
int CHero::GetMagicAmount()
{
	int nMagicAmount = 0;
	int nAmount = m_setMagic.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = m_setMagic[i];
		if(pMagic && pMagic->m_infoMagicType.dwXp == TYPE_MAGIC)
		{
			nMagicAmount ++;
		}
	}
	return nMagicAmount;
}
CMagic*	CHero::GetMagicByIndex(int nIndex)
{
	if(nIndex < 0)
		return NULL;

	int nMagicAmount = 0;
	int nAmount = m_setMagic.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = m_setMagic[i];
		if(pMagic && pMagic->m_infoMagicType.dwXp == TYPE_MAGIC)
		{
			if(nMagicAmount == nIndex)
				return pMagic;

			nMagicAmount ++;
		}
	}

	return NULL;
}

void CHero::MagicAttack(OBJID idMagicType, CMyPos posTarget)
{
	const int MAGIC_CIRCLEATK = 1115;
	if (MAGIC_CIRCLEATK == idMagicType)
	{
		CItem* pWeaponR = this->GetEquipment(ITEMPOSITION_WEAPONR);
		CItem* pWeaponL = this->GetEquipment(ITEMPOSITION_WEAPONL);
		if (!(pWeaponR && pWeaponL
				&& (pWeaponR->GetWeaponSkillType() == pWeaponL->GetWeaponSkillType())))
		{
			const OBJID idStr = 100117;
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idStr));
			return;
		}
	}
	if(m_bMagicAttackBusy)
	{
		OBJID idMagicAttackToFast = 100055;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idMagicAttackToFast));
		return;
	}
	CMagic* pMagic = this->GetMagic(idMagicType);
	if (!pMagic)
	{
		const OBJID idMagicTypeError = 100056;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idMagicTypeError));
		return;
	}
	this->BeginChangeUp(pMagic->GetIDType(), pMagic->GetLevel());
	if (g_objGameMap.GetType() & MAPTYPE_BOOTH_ENABLE)
	{
		if(pMagic->m_infoMagicType.dwCrime)
			return;
	}
	int nManaCost	= pMagic->m_infoMagicType.dwMpCost;
	if (this->GetMana() < (DWORD)nManaCost)
	{
		OBJID idLowPower = 100057;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idLowPower));
		return;
	}
	if(pMagic->m_infoMagicType.dwActionSort == MAGICSORT_TRANSFORM)
	{
		m_dwBeginUseTransformMagicTime = ::TimeGet();
	}
	int nPhysicalForceCost = pMagic->m_infoMagicType.dwUsePP;
	if (this->GetPhysicalForce() < nPhysicalForceCost)
	{
		OBJID idLowPP = 100118;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idLowPP));
		return;
	}
	int	nRange	= pMagic->m_infoMagicType.dwDistance;
	CMyPos posHero = this->GetAlignPos();
	if (abs(posTarget.x - posHero.x) > nRange || abs(posTarget.y - posHero.y) > nRange)
	{
		OBJID idNoTarget = 100058;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idNoTarget));
		return;
	}
	int nLevel	= pMagic->m_infoMagicType.dwLevel;
	CMsgInteract msg;
	OBJID idUser = this->GetID();
	unsigned short usMagicType = (unsigned short)idMagicType;
	OBJID idTarget = ID_NONE;
	int nPosX = posTarget.x;
	int nPosY = posTarget.y;
	ENCODE_MAGICATTACK(idUser, usMagicType, idTarget, nPosX, nPosY)
	if(msg.Create(INTERACT_MAGICATTACK, idUser, idTarget, nPosX, nPosY, usMagicType, (unsigned short)nLevel, ID_NONE))
		msg.Send();
	if(pMagic->m_infoMagicType.dwActionSort == MAGICSORT_TRANSFORM)
		this->MagicLock();
	if (m_objEffect)
	m_objEffect->Add(pMagic->m_infoMagicType.szIntoneEffect);
	this->StartMagicBusy(pMagic->m_infoMagicType.dwIntoneDuration+pMagic->m_infoMagicType.dwMagicBreak);
	this->SetDir(this->GetDir(posTarget));
	this->Stop();
	this->OnAttackSent(idTarget, 3, ((DWORD)usMagicType << 16) + nLevel);
}

void CHero::StartMagicBusy(DWORD dwDuration)
{
	if (m_bMagicAttackBusy)
		return;

	m_dwMagicAttackBegin	= ::TimeGet();
	m_dwMagicAttackBusy		= dwDuration;
	m_bMagicAttackBusy		= true;
	::PostCmd(CMD_START_MAGIC_BUSY);

	::DebugMsg("magic begin at:%06u --- end at:%06u\n", m_dwMagicAttackBegin, m_dwMagicAttackBegin+m_dwMagicAttackBusy);
}
void CHero::EndMagicBusy()
{
	m_bMagicAttackBusy		= false;
	::PostCmd(CMD_END_MAGIC_BUSY);
}

void CHero::MagicAttack(OBJID idMagicType, OBJID idPlayer, BOOL bForce)
{
	CMagic* pMagic = this->GetMagic(idMagicType);
	if (!pMagic)
	{
		const OBJID idMagicTypeError = 100056;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idMagicTypeError));
		return;
	}
	if ((g_objGameMap.GetType() & MAPTYPE_BOOTH_ENABLE) || (g_objGameMap.GetType() & MAPTYPE_FAMILY))
	{
		if (pMagic->m_infoMagicType.dwCanBeusedInMarket == 0)
		{
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(10382));
			return;
		}
	}
	if (this->IsJumping() && pMagic->m_infoMagicType.dwActionSort == MAGICSORT_CALLPET)
	{
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(10382));
		return;
	}
	CPlayer* pPet = g_objPlayerSet.GetPlayer(idPlayer);
	if (pPet && idPlayer >= PETID_FIRST && idPlayer <= PETID_LAST
		&& pPet->GetSydicateID() != 0 && pPet->GetSydicateID() == this->GetSydicateID())
		return;
	const int MAGIC_BUMP = 1051;
	if (MAGIC_BUMP == idMagicType)
	{
		if (_COMMAND_BUMP == this->GetCommandType())
			return;
		DWORD dwTimeNow = ::TimeGet();
		if (dwTimeNow-m_dwLastBump < 1000)
			return;
		else
			m_dwLastBump = dwTimeNow;
		CPlayer* pTarget = g_objPlayerSet.GetPlayer(idPlayer);
		if (!pTarget)
			return;
		if (!this->CanPk(pTarget))
			return;
		const OBJID idGuard = 100028;
		if(pTarget->GetRoleType() == _ROLE_TASK_NPC)
			return;
		if (pTarget->IsNpc() && strstr(pTarget->GetName(), GameDataSetQuery()->GetStr(idGuard)))
		{
			if (!(bForce && PKMODE_FREE == this->GetPkMode()))
			{
				const OBJID idCanNotPkGuard = 100029;
				g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idCanNotPkGuard));
				return;
			}
		}
		if (pMagic->GetWeaponRequired() != 0)
		{
			CItem* pWeaponR = this->GetEquipment(ITEMPOSITION_WEAPONR);
			CItem* pWeaponL = this->GetEquipment(ITEMPOSITION_WEAPONL);
			BOOL bCanUse = FALSE;
			if ((pWeaponR && pWeaponR->GetWeaponSkillType() == pMagic->GetWeaponRequired())
					|| (pWeaponL && pWeaponL->GetWeaponSkillType() == pMagic->GetWeaponRequired()))
			{
				bCanUse = TRUE;
			}
			if (bCanUse == FALSE)
				return;
		}
		CCommand cmd;
		cmd.iType		=_COMMAND_BUMP;
		cmd.iStatus		=_CMDSTATUS_BEGIN;
		cmd.idTarget	=pTarget->GetID();
		this->SetCommand(&cmd);
		return;
	}
	const int MAGIC_CIRCLEATK = 1115;
	if (MAGIC_CIRCLEATK == idMagicType)
	{
		CItem* pWeaponR = this->GetEquipment(ITEMPOSITION_WEAPONR);
		CItem* pWeaponL = this->GetEquipment(ITEMPOSITION_WEAPONL);
		if (!(pWeaponR && pWeaponL
				&& pWeaponR->GetWeaponSkillType() == pWeaponL->GetWeaponSkillType()))
		{
			const OBJID idStr = 100117;
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idStr));
			return;
		}
	}
	if (m_bMagicAttackBusy)
	{
		OBJID idMagicAttackToFast = 100055;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idMagicAttackToFast));
		return;
	}
	this->BeginChangeUp(pMagic->GetIDType(), pMagic->GetLevel());
	if(pMagic->m_infoMagicType.dwActionSort == MAGICSORT_DANCE)
	{
		const OBJID idDance = 100030;
		if(this->GetLWeapon() == ID_NONE && this->GetRWeapon() == ID_NONE)
			this->Emotion(pMagic->m_infoMagicType.dwSenderAction);
		else
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(100030));
		return;
	}
	if (pMagic->GetWeaponRequired() != 0)
	{
		CItem* pWeaponR = this->GetEquipment(ITEMPOSITION_WEAPONR);
		CItem* pWeaponL = this->GetEquipment(ITEMPOSITION_WEAPONL);
		BOOL bCanUse = FALSE;
		if ((pWeaponR && pWeaponR->GetWeaponSkillType() == pMagic->GetWeaponRequired())
			|| (pWeaponL && pWeaponL->GetWeaponSkillType() == pMagic->GetWeaponRequired()))
		{
			bCanUse = TRUE;
		}
		if (bCanUse == FALSE)
			return;
	}
	if(pMagic->m_infoMagicType.dwActionSort == MAGICSORT_TRANSFORM)
	{
		m_dwBeginUseTransformMagicTime = ::TimeGet();
	}
	int nManaCost	= pMagic->m_infoMagicType.dwMpCost;
	if (this->GetMana() < (DWORD)nManaCost)
	{
		OBJID idLowPower = 100057;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idLowPower));
		return;
	}
	int nPhysicalForceCost = pMagic->m_infoMagicType.dwUsePP;
	if (this->GetPhysicalForce() < nPhysicalForceCost)
	{
		OBJID idLowPP = 100118;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idLowPP));
		return;
	}
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(idPlayer);
	if (!pPlayer)
	{
		if (pMagic->TestTarget(MAGIC_TARGET_SELF)
				|| pMagic->TestTarget(MAGIC_TARGET_NONE))
		{
			if (this->GetID() == idPlayer)
				pPlayer = this;
		}
		if (!pPlayer)
		{
			OBJID idErrorTarget = 100059;
			g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idErrorTarget));
			return;
		}
	}
	const OBJID idGuard = 100028;
	if (pPlayer->IsNpc() && strstr(pPlayer->GetName(), GameDataSetQuery()->GetStr(idGuard)))
	{
		if (!(bForce && PKMODE_FREE == this->GetPkMode()))
		{
			const OBJID idCanNotPkGuard = 100029;
			g_objGameMsg.AddMsg(_TXTATR_SYSTEM, GameDataSetQuery()->GetStr(idCanNotPkGuard));
			return;
		}
	}
	const OBJID MAGIC_REBORN	= 1100;
	const OBJID XP_GETSOUL  = 1050;
	if (pPlayer->IsDead() && XP_GETSOUL != idMagicType && MAGIC_REBORN != idMagicType)
	{
		OBJID idErrorTarget = 100060;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idErrorTarget));
		return;
	}
	if (pPlayer->IsNpc() && (MAGIC_REBORN == idMagicType || XP_GETSOUL == idMagicType))
	{
		OBJID idErrorTarget = 100060;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idErrorTarget));
		return;
	}
	if (pMagic->m_infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_SINGLE_HP
			&& !this->CanPk(pPlayer)
				&& (!bForce || this->GetPkMode() != PKMODE_FREE))
	{
		OBJID idCanNotPk = 100061;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idCanNotPk));
		return;
	}
	CMyPos posHero = this->GetAlignPos();
	CMyPos posTarget;
	pPlayer->GetPos(posTarget);
	int	nRange	= pMagic->m_infoMagicType.dwDistance;
	if (abs(posTarget.x - posHero.x) > nRange || abs(posTarget.y - posHero.y) > nRange)
	{
		OBJID idNoTarget = 100058;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idNoTarget));
		return;
	}
	switch(idMagicType)
	{
	case 1015:
		{
		}
		break;
	case 1020:
		{
		}
		break;
	default:
		this->Stop();
		break;
	}
	int nPosX, nPosY;
	this->GetPos(nPosX, nPosY);
	if(idMagicType == MAGIC_CIRCLEATK)
	{
		nPosX = posHero.x;
		nPosY = posHero.y;
	}
	else if(pMagic->m_infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_SINGLE_HP ||
			pMagic->m_infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_SINGLE_STATUS)
	{
		nPosX = posTarget.x;
		nPosY = posTarget.y;
	}
	int nLevel	= pMagic->m_infoMagicType.dwLevel;
	CMsgInteract msg;
	OBJID idUser = this->GetID();
	unsigned short usMagicType = (unsigned short)idMagicType;
	OBJID idTarget = idPlayer;
	int nMyPosX = nPosX;
	int nMyPosY = nPosY;
	ENCODE_MAGICATTACK(idUser, usMagicType, idTarget, nMyPosX, nMyPosY)
	if (msg.Create( INTERACT_MAGICATTACK,
					idUser, idTarget,
					nMyPosX, nMyPosY,
					usMagicType, (unsigned short)nLevel, idPlayer))
		msg.Send();
	if(pMagic->m_infoMagicType.dwActionSort == MAGICSORT_TRANSFORM)
		this->MagicLock();
	if (m_objEffect)
	m_objEffect->Add(pMagic->m_infoMagicType.szIntoneEffect);
	this->StartMagicBusy(pMagic->m_infoMagicType.dwIntoneDuration+pMagic->m_infoMagicType.dwMagicBreak);
	this->OnAttackSent(idTarget, 3, ((DWORD)usMagicType << 16) + nLevel);
}

void CHero::ProcessMagicBusy()
{
	if(!m_bMagicAttackBusy)
		return;	

	if(::TimeGet() >= m_dwMagicAttackBegin + m_dwMagicAttackBusy)
		this->EndMagicBusy();
}
CMagic* CHero::GetMagic(OBJID idType)
{
	int nAmount = m_setMagic.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = m_setMagic[i];
		if(pMagic && (pMagic->m_infoMagicType.idMagicType == idType))
		{
			return pMagic;
		}
	}
	return NULL;
}
int CHero::GetXpSkillAmount()
{
	int nMagicAmount = 0;
	int nAmount = m_setMagic.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = m_setMagic[i];
		if(pMagic && pMagic->m_infoMagicType.dwXp == TYPE_XPSKILL)
		{
			nMagicAmount ++;
		}
	}
	return nMagicAmount;
}
CMagic* CHero::GetXpSkillByIndex(int nIndex)
{
	if(nIndex < 0)
		return NULL;

	int nXpAmount = 0;
	int nAmount = m_setMagic.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = m_setMagic[i];
		if(pMagic && pMagic->m_infoMagicType.dwXp == TYPE_XPSKILL)
		{
			if(nXpAmount == nIndex)
				return pMagic;

			nXpAmount ++;
		}
	}
	return NULL;
}
int CHero::GetKongfuAmount()
{
	int nMagicAmount = 0;
	int nAmount = m_setMagic.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = m_setMagic[i];
		if(pMagic && pMagic->m_infoMagicType.dwXp == TYPE_KONGFU)
		{
			nMagicAmount ++;
		}
	}
	return nMagicAmount;
}
CMagic* CHero::GetKongfuByIndex(int nIndex)
{
	if(nIndex < 0)
		return NULL;

	int nKongfuAmount = 0;
	int nAmount = m_setMagic.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = m_setMagic[i];
		if(pMagic && pMagic->m_infoMagicType.dwXp == TYPE_KONGFU)
		{
			if (nKongfuAmount == nIndex)
				return pMagic;

			nKongfuAmount++;
		}
	}
	return NULL;
}

void CHero::ProcessMagic()
{
	BOOL bFlash = false;
	const int _SHIELD_SUBTYPE = 900;
	int nAmount, i;
	nAmount = this->GetMagicAmount();
	for(i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = this->GetMagicByIndex(i);
		if(!pMagic)
			continue;
		if(pMagic->HeroMagicProcess())
		{
			bFlash = true;
		}
	}
	nAmount = this->GetXpSkillAmount();
	for(i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = this->GetXpSkillByIndex(i);
		if(!pMagic)
			continue;
		if(pMagic->HeroMagicProcess())
		{
			bFlash = true;
		}
	}
	nAmount = this->GetKongfuAmount();
	for(i = 0; i < nAmount; i ++)
	{
		CMagic* pMagic = this->GetKongfuByIndex(i);
		if(!pMagic)
			continue;
		if(pMagic->HeroMagicProcess())
		{
			bFlash = true;
		}
	}
	if(bFlash)
		::PostCmd(CMD_FLASH_MAGIC_STATUS);
}
void CHero::AbortTransform()
{
	CMsgAction msg;
	if(msg.Create(this->GetID(), 0, 0, 0, actionAbortTransform))
		msg.Send();
	this->UnMagicLock();
}
void CHero::BeginChangeUp(OBJID idMagicType, DWORD dwDelay)
{
	MagicTypeInfo info;
	if(!CMagic::s_objMagicData.GetMagicTypeInfo(idMagicType, dwDelay, info))
	{
		m_bChargeUp = false;
		return;
	}
	if(!(info.dwStatus & USERSTATUS_CHARGEUP))
	{
		m_bChargeUp = false;
		return;
	}
	m_bChargeUp = true;
	m_dwTimeChargeUp = ::TimeGet();
	const DWORD dwNatworkDelay = 500;
	m_dwChargeUpTotalTime = info.dwIntoneDuration+dwNatworkDelay;
}
void CHero::AbortChargeUp()
{
	CMsgAction  msg;
	if(msg.Create(g_objHero.GetID(), 0, 0, this->GetDir(), actionDischarge))
		msg.Send();
}
void CHero::ShowChargeUp()
{
	const int nBgSize		= 64;
	const int nBarLength	= 49;
	const int nBarHeight	= 9;
	const int nBarOffsetX	= 7;
	const int nBarOffsetY	= 27;
	const DWORD dwBarColor	= 0xff31A2FF;
	if(m_bChargeUp)
	{
		IAni* pAni = GameDataSetQuery()->GetDataAni("ani/common.ani", "ChargeUp");
		if(!pAni)
			return;
		if(m_dwChargeUpTotalTime == 0) 
			m_dwChargeUpTotalTime = 1;
		int nCurrentLength = nBarLength * (::TimeGet() - m_dwTimeChargeUp)/m_dwChargeUpTotalTime;
		if(nCurrentLength > nBarLength)
			nCurrentLength = nBarLength;
		CMyPos posShow;
		int nNameShowOffset = (OFFSET_NAMESHOW+80)*g_objGameMap.GetScale()/_NORMAL_SCALE;
		int nShowX	=m_Info.posScr.x-nBgSize/2;
		int nShowY	=m_Info.posScr.y-nNameShowOffset;
		posShow.x = nShowX;
		posShow.y = nShowY-this->GetOffsetY();
		int nFrame = 0;
		if(this->TestStatus(USERSTATUS_CHARGEUP))
		{
			nFrame = 1;
			nCurrentLength = nBarLength;
		}

		pAni->Show(nFrame, posShow.x, posShow.y);
		CMyBitmap::ShowBlock(posShow.x+nBarOffsetX, posShow.y+nBarOffsetY,
					posShow.x+nBarOffsetX+nCurrentLength, posShow.y+nBarOffsetY+nBarHeight, dwBarColor);
	}
}
void  CHero::StopChargeUp()
{
	m_bChargeUp = false;
}
