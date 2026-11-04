#include "Magic.h"
#include "Hero.h"
#include "3DGameMap.h"
CMagicData CMagic::s_objMagicData;

CMagic::CMagic()
{
	m_bEnable = true;
}

CMagic::~CMagic()
{

}

BOOL CMagic::Create(OBJID idType, DWORD dwLevel, DWORD dwExp)
{
	if(!s_objMagicData.GetMagicTypeInfo(idType, dwLevel, m_infoMagicType))
		return false;

	m_dwExp = dwExp;
	return true;
}
CMagic* CMagic::CreateNew(OBJID idType, DWORD dwLevel, DWORD dwExp)
{
	CMagic* pMagic = new CMagic;
	MYASSERT(pMagic);

	if(pMagic->Create(idType, dwLevel, dwExp))
	{
		return pMagic;
	}

	SAFE_DELETE(pMagic);
	return NULL;
}
char* CMagic::GetName()
{
	return m_infoMagicType.szName;
}
char* CMagic::GetDescribe()
{
	return m_infoMagicType.szDisc;
}
DWORD CMagic::GetExp()
{
	return m_dwExp;
}
DWORD CMagic::GetNextLevelExp()
{
	return m_infoMagicType.dwExpRequired;
}
DWORD CMagic::GetLevel()
{
	return m_infoMagicType.dwLevel;
}
OBJID CMagic::GetIDType()
{
	return m_infoMagicType.idMagicType;
}
BOOL CMagic::TestTarget(DWORD dwMask)
{
	return m_infoMagicType.dwTarget & dwMask;
}
DWORD CMagic::GetBusyDuration()
{
	DWORD dwProfession = g_objHero.GetProfession();
	DWORD dwRequireProfLevel = dwProfession%10;
	if(dwRequireProfLevel > 10)
		return m_infoMagicType.dwIntoneDuration;
	return (10 - dwRequireProfLevel) * 100 + m_infoMagicType.dwIntoneDuration;
}
BOOL CMagic::IsTransformMagic()
{
	return m_infoMagicType.dwActionSort == MAGICSORT_TRANSFORM;
}
BOOL CMagic::IsInMagicList()
{
	return m_infoMagicType.dwCanBeusedInMarket != 3;
}
BOOL CMagic::HeroMagicProcess()
{
	const int _SHIELD_SUBTYPE = 900;
	BOOL bEnable = true;
	if(g_objHero.IsTransform() && !this->IsTransformMagic())
		bEnable = false;

	CONST int MAGIC_CIRCLEATK = 1115;
	if (MAGIC_CIRCLEATK == this->GetIDType())
	{
		CItem* pWeaponR = g_objHero.GetEquipment(ITEMPOSITION_WEAPONR);
		CItem* pWeaponL = g_objHero.GetEquipment(ITEMPOSITION_WEAPONL);

		if (!(pWeaponR && pWeaponL
			&& pWeaponR->GetWeaponSkillType() == pWeaponL->GetWeaponSkillType()))
		{
			bEnable = true;
		}
	}
	else if(this->GetWeaponRequired() == _SHIELD_SUBTYPE)
	{
		CItem* pWeaponL = g_objHero.GetEquipment(ITEMPOSITION_WEAPONL);
		if(!pWeaponL)
			bEnable = false;
		else if(pWeaponL->GetWeaponSkillType() != _SHIELD_SUBTYPE)
			bEnable = false;
	}
	else if(this->GetWeaponRequired() != 0)
	{
		CItem* pWeaponR = g_objHero.GetEquipment(ITEMPOSITION_WEAPONR);
		if (!pWeaponR)
			bEnable = FALSE;
		else if (pWeaponR->GetWeaponSkillType() != this->GetWeaponRequired())
		{
			bEnable = FALSE;
			if (this->GetWeaponRequired() == 420 && pWeaponR->GetWeaponSkillType() == 421)
				bEnable = TRUE;
		}
		if (this->GetWeaponRequired() == 500)
		{
			CItem* pWeaponL = g_objHero.GetEquipment(ITEMPOSITION_WEAPONL);
			if (!pWeaponL || pWeaponL->GetAmount() == 0)
				bEnable = FALSE;
		}
		if (!bEnable)
		{
			CItem* pWeaponL = g_objHero.GetEquipment(ITEMPOSITION_WEAPONL);
			if (pWeaponL && (pWeaponL->GetWeaponSkillType() == this->GetWeaponRequired()
				|| (this->GetWeaponRequired() == 420 && pWeaponL->GetWeaponSkillType() == 421)))
			{
				bEnable = TRUE;
			}
		}
	}
	if(g_objHero.IsDead())
		bEnable = false;
	if(this->TestTarget(MAGIC_PASSIVE))
		bEnable = false;
	if(g_objGameMap.GetType() & MAPTYPE_BOOTH_ENABLE)
	{
		if(!this->m_infoMagicType.dwCanBeusedInMarket)
			bEnable = false;
	}
	if(m_infoMagicType.dwXp == TYPE_XPSKILL && !g_objHero.TestStatus(USERSTATUS_XPFULL))
	{
		bEnable = false;
	}
	if(m_infoMagicType.dwStatus == USERSTATUS_FLY && (g_objGameMap.GetType()&MAPTYPE_FLY_DISABLE))
	{
		bEnable = false;
	}
	if(m_infoMagicType.dwStatus == USERSTATUS_FLY && !g_objHero.HaveBow())
	{
		bEnable = false;
	}
	if(bEnable != this->IsEnable())
	{
		m_bEnable = bEnable;
		return true;
	}
	return false;
}
