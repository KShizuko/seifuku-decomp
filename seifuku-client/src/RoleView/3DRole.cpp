#include "3DRole.h"
#include "GameDataSet.h"
#include "3DRoleData.h"
#include "ndsound.h"
#include <mmsystem.h>

const OBJID ID_NONE = 0;
static const int _ACTION_STANDBY	= 100;
static const int _ACTION_ATTACK0	= 401;
static const int _ACTION_ATTACK1	= 402;
static const int _ACTION_ATTACK2	= 403;
static const int _ACTION_ATTACK3	= 404;
static const int _ACTION_FLY_ATTACK	= 520;
static const int _ACTION_FAST_SHOOT	= 620;

#ifdef _ANALYSIS_ON
DWORD g_dw3DRoleDrawTime = 0;
DWORD g_dw3DRoleDrawAmount = 0;
DWORD g_dw3DRoleDrawTimeMax = 0;
DWORD g_dw3DRoleDrawTimeFrame = 0;
DWORD g_dw3DRoleDrawAmountFrame = 0;
#endif

C3DRole::C3DRole ()
{
	m_fPosX = m_fPosY = 0;
	m_fPosH = 0;
	m_nRotateZ = 0;
	m_nRotateX = CMyBitmap::GetObliqueAngle();
	m_nRotateY = 0;
	m_fScale = 1.0f;
	m_LightOffset.x = 0;
	m_LightOffset.y = 0;
	m_LightOffset.z = -1000;
	m_nShadowType = 2;
	m_a = 1.0f;
	m_r = 0.3f;
	m_g = 0.3f;
	m_b = 0.3f;
	m_nAsb = -1;
	m_nAdb = -1;
	m_idArmor	= 0;
	m_idArmet	= 0;
	m_idLWeapon	= 0;
	m_idRWeapon	= 0;
	m_idMount	= 0;
	m_idMisc	= 0;
	m_idArmorType	= 0;
	m_idArmetType	= 0;
	m_nLook		= 0;
	m_nHair		= 0;
	m_idHead	= 0;
	m_dwTransStart	= 0;
	m_dwTransTime	= 0;
	m_dwFrame		= 0;
	m_dwBlendFrame	= 0;
	m_pMotionBlend	= NULL;
	m_pMotionArmor	= NULL;
	m_pMotionArmet	= NULL;
	m_pMotionWeaponR= NULL;
	m_pMotionWeaponL= NULL;
	m_pMotionMount	= NULL;
	m_pMotionMisc	= NULL;
	m_pRole = ::I3DRoleCreate();
}

C3DRole::~C3DRole ()
{
	if (m_pRole)
	{
		m_pRole->Release();
		m_pRole = NULL;
	}
}

void C3DRole::ResetMotion	(void)
{
	m_pMotionArmor	= NULL;
	m_pMotionArmet	= NULL;
	m_pMotionWeaponR= NULL;
	m_pMotionWeaponL= NULL;
	m_pMotionMount	= NULL;
	m_pMotionMisc	= NULL;
	m_pMotionBlend	= NULL;
}

DWORD C3DRole::GetWeaponActionData (void)
{
	OBJID idRWeapon = m_idRWeapon;
	OBJID idLWeapon = m_idLWeapon;
	if ((m_idLWeapon/1000)%1000 == 500)
		idRWeapon = m_idLWeapon;
	if (m_idRWeapon/10000 == 105)
		idLWeapon = m_idRWeapon;
	if (idRWeapon && !idLWeapon)
	{
		return (idRWeapon%1000000)/1000;
	}
	if (!idRWeapon && idLWeapon)
	{
		return 700;
	}
	if (idRWeapon && idLWeapon)
	{
		if (((idLWeapon%1000000)/100000) == 9)
		{
			return 7*100 + ((idRWeapon%1000000)/10000);
		}
		else if (((idLWeapon%1000000)/100000) == 4)
		{
			return 6*100 + ((idRWeapon%100000)/10000)*10 + ((idLWeapon%100000)/10000);
		}
		if(idRWeapon / 1000 == 500)
			return 500;
	}
	return 0;
}

DWORD C3DRole::GetMountActionData (void)
{
	if (!m_idMount)
		return 0;
	return (m_idMount/1000)%100;
}

DWORD C3DRole::GetMiscActionData (void)
{
	if (!m_idMisc)
		return 0;
	return (m_idMisc/10)%100;
}

I3DMotion* C3DRole::GetMountMotion (OBJID idMount, int nActionType)
{
	unsigned __int64 idMotion = (unsigned __int64)idMount*1000 + (__int64)nActionType;
	I3DMotion* pMotion = GameDataSetQuery()->Get3DMotion("mount", idMotion);
	if (pMotion)
		return pMotion;
	return GameDataSetQuery()->Get3DMotion("mount", (idMount/1000)*1000+999);
}

I3DMotion* C3DRole::GetWeaponMotion	(OBJID idWeapon, int nActionType)
{
	unsigned __int64 idKey = (unsigned __int64)this->GetMountActionData()*10000000 + (unsigned __int64)idWeapon%10000000;
	idKey = idKey*1000000 + (__int64)((m_nLook%1000)*1000);
	unsigned __int64 idMotion = idKey + (__int64)nActionType;
	I3DMotion* pMotion  = GameDataSetQuery()->Get3DMotion("r_weapon", idMotion);
	if (pMotion)
		return pMotion;
	idMotion = idKey + 999;
	pMotion  = GameDataSetQuery()->Get3DMotion("r_weapon", idMotion);
	if (pMotion)
		return pMotion;
	idMotion = idKey + (999 - (idKey/1000)%1000)*1000 + (__int64)nActionType;
	pMotion  = GameDataSetQuery()->Get3DMotion("r_weapon", idMotion);
	if (pMotion)
		return pMotion;
	idMotion = (idKey/1000000)*1000000 + 999999;
	pMotion  = GameDataSetQuery()->Get3DMotion("r_weapon", idMotion);
	return pMotion;
}

I3DMotion* C3DRole::GetArmetMotion (OBJID idArmet, DWORD dwWeaponData, int nActionType)
{
	unsigned __int64 idKey = (unsigned __int64)this->GetMountActionData()*100 + (idArmet%100);
	idKey = idKey*1000000000 + (__int64)((m_nLook%1000)*1000000) + ((dwWeaponData/1000)%1000)*1000;
	unsigned __int64 idMotion = idKey + (__int64)nActionType;
	I3DMotion* pMotion  = GameDataSetQuery()->Get3DMotion("armet", idMotion);
	if (pMotion)
		return pMotion;
	idMotion = idKey + 999;
	pMotion  = GameDataSetQuery()->Get3DMotion("armet", idMotion);
	if (pMotion)
		return pMotion;
	idMotion = idKey + (999 - (idKey/1000)%1000)*1000 + (__int64)nActionType;
	pMotion  = GameDataSetQuery()->Get3DMotion("armet", idMotion);
	if (pMotion)
		return pMotion;
	idMotion = (idKey/1000000)*1000000 + 999999;
	pMotion  = GameDataSetQuery()->Get3DMotion("armet", idMotion);
	return pMotion;
}

I3DMotion* C3DRole::GetMiscMotion (OBJID idMisc, DWORD dwWeaponData, int nActionType)
{
	unsigned __int64 idKey = (unsigned __int64)this->GetMountActionData()*100 + this->GetMiscActionData();
	idKey = idKey*1000000000 + (__int64)((m_nLook%1000)*1000000) + ((dwWeaponData/1000)%1000)*1000;
	unsigned __int64 idMotion = idKey + (__int64)nActionType;
	I3DMotion* pMotion  = GameDataSetQuery()->Get3DMotion("misc", idMotion);
	if (pMotion)
		return pMotion;
	idMotion = idKey + 999;
	pMotion  = GameDataSetQuery()->Get3DMotion("misc", idMotion);
	if (pMotion)
		return pMotion;
	idMotion = idKey + (999 - (idKey/1000)%1000)*1000 + (__int64)nActionType;
	pMotion  = GameDataSetQuery()->Get3DMotion("misc", idMotion);
	if (pMotion)
		return pMotion;
	idMotion = (idKey/1000000)*1000000 + 999999;
	pMotion  = GameDataSetQuery()->Get3DMotion("misc", idMotion);
	return pMotion;
}

BOOL C3DRole::SetLWeapon(OBJID idType)
{
	if (idType/10000 == 105)
	{
		if ((m_idLWeapon/1000)%1000 != 500)
		{
			this->ResetMotion();
			m_pRole->DelPart("l_weapon");
			m_idLWeapon = 0;
		}
		return this->SetRWeapon(idType);
	}
	if (idType == 0 && m_idRWeapon/10000 == 105)
	{
		this->ResetMotion();
		m_pRole->DelPart("r_weapon");
		m_idRWeapon = 0;
		return true;
	}
	idType %= 1000000;
	this->ResetMotion();
	m_pRole->DelPart("l_weapon");
	m_idLWeapon = 0;
	if(idType == ID_NONE)
		return false;
	if (!m_pRole->AddPart("l_weapon", "v_l_weapon", "body", idType, NULL))
		return false;
	m_idLWeapon = idType;
	return true;
}

BOOL C3DRole::SetRWeapon(OBJID idType)
{
	if ((idType/1000)%1000 == 500)
	{
		if (m_idRWeapon/10000 != 105)
		{
			this->ResetMotion();
			m_pRole->DelPart("r_weapon");
			m_idRWeapon = 0;
		}
		return this->SetLWeapon(idType);
	}
	if (idType/10000 != 105)
	{
		if ((m_idLWeapon/1000)%1000 == 500)
			this->SetLWeapon(0);
		idType %= 1000000;
	}
	this->ResetMotion();
	m_pRole->DelPart("r_weapon");
	m_idRWeapon = 0;
	if(idType == ID_NONE)
		return false;
	if (!m_pRole->AddPart("r_weapon", "v_r_weapon", "body", idType, NULL))
		return false;
	m_idRWeapon = idType;
	return true;
}

void C3DRole::SetHead(OBJID idHead)
{
	if (m_idHead == idHead)
		return;
	m_pRole->ReplacePartMesh("body", "head", m_idHead);
	m_idHead = 0;
	if (m_pRole->AddPartMesh("body", "head", idHead))
		m_idHead = idHead;
}

void C3DRole::SetArmor(OBJID idType)
{
	m_idArmorType = idType;
	this->SetBody(idType);
}

void C3DRole::SetBody(OBJID idType)
{
	this->ResetMotion();
	OBJID idOldArmor = m_idArmor;
	m_pRole->DelPart("body");
	DWORD dwClass = (idType/10000)%100;
	m_idArmor = 0;
	if (dwClass == 19)
	{
		if ((m_idArmet/100)%1000 != 119)
		{
			m_pRole->DelPart("armet");
			if (m_nHair != 0)
			{
				m_idArmet = m_nLook*1000000 + m_nHair + 119000;
				if (!m_pRole->AddPart("armet", "v_armet", "body", m_idArmet, NULL))
					m_idArmet = 0;
			}
		}
	}
	if ((idOldArmor/10000)%100 == 19 && dwClass != 19)
	{
		if (m_idArmetType != 0)
			this->SetArmetPart(m_idArmetType);
		if (m_nHair != 0)
			this->SetHair(m_nHair);
	}
	idType %= 1000000;
	m_idArmor = m_nLook*1000000+idType/10*10;
	if (!m_pRole->AddPart("body", "v_mount", "mount", m_idArmor, NULL))
	{
		m_idArmor = 0;
		return;
	}
	if (m_idHead != 0)
		m_pRole->AddPartMesh("body", "head", m_idHead);
}

void C3DRole::SetMount(OBJID idMountType)
{
	this->ResetMotion();
	m_pRole->DelPart("mount");
	idMountType %= 1000000;
	m_idMount = 0;
	if(idMountType == ID_NONE)
		return;
	if (m_pRole->AddPart("mount", "none", "none", idMountType, NULL))
		m_idMount = idMountType;
}

void C3DRole::SetMisc(OBJID idType)
{
	this->ResetMotion();
	m_pRole->DelPart("misc");
	m_idMisc = 0;
	idType %= 1000000;
	if(idType == ID_NONE)
		return;
	OBJID idMisc = m_nLook*1000000 + idType;
	if (m_pRole->AddPart("misc", "v_misc", "body", idMisc, NULL))
		m_idMisc = idMisc;
}

void C3DRole::SetLook(int nLook)
{
	MYASSERT (nLook>0 && nLook<1000);
	m_nLook = nLook;
	OBJID idArmor;
	if (!m_idArmor)
		idArmor = nLook*1000000;
	else
		idArmor = nLook*1000000 + m_idArmor%1000000;
	if (!m_pRole->AddPart("body", "v_mount", "mount", idArmor, NULL))
		return;
	m_idArmor = idArmor;
	if (m_idHead)
		this->SetHead(m_idHead);
	if (m_nHair)
		this->SetHair(m_nHair);
	if (m_idRWeapon)
		this->SetRWeapon(m_idRWeapon);
	if (m_idLWeapon)
		this->SetLWeapon(m_idLWeapon);
	if (m_idMount)
		this->SetMount(m_idMount);
	if (m_idMisc)
		this->SetMisc(m_idMisc);
}

void C3DRole::SetArmet(OBJID idArmetType)
{
	m_idArmetType = idArmetType;
	this->SetArmetPart(idArmetType);
}

void C3DRole::SetArmetPart(OBJID idArmetType)
{
	this->ResetMotion();
	if ((m_idArmorType/10000)%100 == 19)
	{
		if ((idArmetType%1000000)/1000 != 119)
			return;
		m_pRole->DelPart("armet");
		m_idArmet = m_nLook*1000000 + m_nHair + 119000;
		if (!m_pRole->AddPart("armet", "v_armet", "body", m_idArmet, NULL))
			m_idArmet = 0;
		return;
	}
	idArmetType = idArmetType%1000000;
	if (119 != (idArmetType%1000000)/1000)
		idArmetType = (idArmetType/10)*10;
	int nSubType = (idArmetType%1000000)/1000;
	if (nSubType == 117 || nSubType == 118 || nSubType == 177 || nSubType == 178 || (nSubType == 112 && (idArmetType/10)%10 == 8))
	{
		if (m_idArmet)
		{
			int nSubType = (m_idArmet%1000000)/1000;
			if (nSubType == 119)
				return;
			else
			{
				m_pRole->DelPart("armet");
				m_idArmet = 0;
				if (m_nHair != 0)
				{
					m_idArmet = m_nLook*1000000 + m_nHair + 119000;
					if (!m_pRole->AddPart("armet", "v_armet", "body", m_idArmet, NULL))
						m_idArmet = 0;
				}
			}
		}
		return;
	}
	else if (nSubType == 119)
	{
		if (m_idArmet)
		{
			int nSubType = (m_idArmet%1000000)/1000;
			if (nSubType != 119)
				return;
			m_pRole->DelPart("armet");
			m_idArmet = 0;
		}
		if ((idArmetType%1000) == 0)
			return;
		m_idArmet = m_nLook*1000000+idArmetType;
		if (!m_pRole->AddPart("armet", "v_armet", "body", m_idArmet, NULL))
			m_idArmet = 0;
		return;
	}
	else
	{
		if (idArmetType == ID_NONE)
		{
			if (!m_idArmet)
				return;
			int nSubType = (m_idArmet%1000000)/1000;
			if (nSubType == 119)
				return;
			m_pRole->DelPart("armet");
			m_idArmet = 0;
			if (m_nHair == 0)
				return;
			m_idArmet = m_nLook*1000000 + m_nHair + 119000;
		}
		else
		{
			m_pRole->DelPart("armet");
			m_idArmet = m_nLook*1000000+idArmetType;
		}
		if (!m_pRole->AddPart("armet", "v_armet", "body", m_idArmet, NULL))
			m_idArmet = 0;
	}
}

void C3DRole::SetHair(int nHair)
{
	m_nHair = nHair;
	this->SetArmetPart(nHair+119000);
}

void C3DRole::Transform(int nLook)
{
	this->ResetMotion();
	m_idLWeapon	= 0;
	m_idRWeapon	= 0;
	m_idMount	= 0;
	m_idMisc	= 0;
	m_idArmet	= 0;
	m_idArmor	= 0;
	m_pRole->DelPart(NULL);
	m_pRole->Clear3DEffect(NULL, NULL);
	this->SetLook(nLook);
}

void C3DRole::SetFrame (DWORD dwFrame)
{
	if(m_pRole)
	{
		m_pRole->SetFrame(dwFrame);
		m_dwFrame = dwFrame;
	}
}

BOOL	C3DRole::SetVariable(int nVariableType, DWORD dwData)
{
	switch(nVariableType)
	{
	case _VAR_LOOK:
		this->SetLook((int)dwData);
		return true;
	case _VAR_HAIR:
		this->SetHair((int)dwData);
		return true;
	case _VAR_HEAD:
		this->SetHead(dwData);
		return true;
	case _VAR_TRANSFORM:
		this->Transform((int)dwData);
		return true;
	case _VAR_ARMOR:
		this->SetArmor(dwData);
		return true;
	case _VAR_ARMET:
		this->SetArmet(dwData);
		return true;
	case _VAR_RWEAPON:
		return this->SetRWeapon(dwData);
	case _VAR_LWEAPON:
		return this->SetLWeapon(dwData);
	case _VAR_MOUNT:
		this->SetMount(dwData);
		return true;
	case _VAR_MISC:
		this->SetMisc(dwData);
		return true;
	case _VAR_FRAME:
		this->SetFrame(dwData);
		return true;
	case _VAR_ROTATEX:
		this->SetRotateX(dwData);
		return true;
	case _VAR_ROTATEY:
		m_nRotateY = dwData;
		return true;
	case _VAR_ROTATEZ:
		m_nRotateZ = dwData;
		return true;
	case _VAR_BLEND:
		m_nAdb = dwData >> 16;
		m_nAsb = dwData & 0xffff;
		return true;
	case _VAR_DRAWTYPE:
		if (m_pRole)
			m_pRole->SetDrawType(dwData, NULL);
		return true;
	}
	return false;
}

DWORD	C3DRole::GetVariable(int nVariableType)
{
	switch(nVariableType)
	{
		case _VAR_RWEAPON_ID:
			return m_idRWeapon;
		case _VAR_LWEAPON_ID:
			return m_idLWeapon;
		case _VAR_ARMET_ID:
			return m_idArmet;
		case _VAR_ARMOR_ID:
			return m_idArmor%1000000;
		case _VAR_MOUNT_ID:
			return m_idMount;
		case _VAR_MISC_ID:
			return m_idMisc%1000000;
		case _VAR_WEAPON_ACTION_DATA:
			return this->GetWeaponActionData();
		case _VAR_ACTION_DATA:
			return this->GetActionData();
	}
	return 0;
}

DWORD	C3DRole::GetActionData(int nActionType)
{
	return this->GetMountActionData() *1000000000 + m_nLook *1000000 + this->GetWeaponActionData()*1000 + nActionType;
}

DWORD	C3DRole::GetActionData(void)
{
	return this->GetActionData(m_nActionType);
}

void	C3DRole::SetPartEffect(const char* pszKind, OBJID idPart, int nActionType)
{
	if (!pszKind)
		return;
	for (int nTable = 0; nTable < 3; nTable++)
	{
		const char* pszEffect = RoleDataQuery()->GetRolePart3DEffect((idPart/1000000)%1000, nActionType, (idPart/1000)%1000, idPart%1000, nTable);
		if (!pszEffect)
			return;
		m_pRole->AddEffect(pszEffect, pszKind, FALSE);
	}
}

int		C3DRole::SetAction(int nActionType, BOOL bResetMotion, BOOL bPlaySound, int nWorldX, int nWorldY, BOOL bSetEffect)
{
	MYASSERT (m_idArmor);
	DWORD dwTime = ::timeGetTime();
	if (bResetMotion || !m_pMotionArmor)
	{
		I3DMotion* pOldMotion = m_pMotionArmor;
		OBJID idBodyMotion = this->GetActionData(nActionType);
		m_pMotionArmor = GameDataSetQuery()->Get3DMotion("body", idBodyMotion);
		m_dwTransTime = GameDataSetQuery()->GetActionTransTime(m_nActionType, nActionType);
		if (m_dwTransTime > 0)
		{
			m_pMotionBlend	= pOldMotion;
			m_dwTransStart	= dwTime;
			m_dwBlendFrame	= m_dwFrame;
		}
		else
			m_pMotionBlend = NULL;
		if (!m_pMotionArmor)
		{
			m_pMotionBlend = NULL;
			m_pMotionArmor	 = GameDataSetQuery()->Get3DMotion("body", idBodyMotion);
			if (!m_pMotionArmor)
				return 0;
		}
	}
	if (m_idArmor)
	{
		if (bSetEffect)
			this->SetPartEffect("body", m_idArmor, nActionType);
		m_pRole->SetMotion("body", m_pMotionArmor);
		if (m_pMotionBlend)
		{
			float fWeight = 1.0f - (float)(__int64)(dwTime - m_dwTransStart) / (float)(int)m_dwTransTime;
			if (fWeight < 0.0f)
			{
				fWeight = 0.0f;
				m_pMotionBlend = NULL;
				m_dwTransTime = 0;
			}
			m_pRole->SetBlendMotion("body", m_pMotionBlend, fWeight);
			m_pRole->SetBlendFrame(m_dwBlendFrame);
			m_dwBlendFrame++;
		}
		else
			m_pRole->SetBlendMotion("body", NULL, 0.0f);
	}
	m_nActionType = nActionType;
	if (m_idMisc)
	{
		if (bSetEffect)
			this->SetPartEffect("misc", m_idMisc, nActionType);
		if (bResetMotion || !m_pMotionMisc)
			m_pMotionMisc = this->GetMiscMotion(m_idMisc, this->GetWeaponActionData(), nActionType);
		m_pRole->SetMotion("misc", m_pMotionMisc);
	}
	if (m_idMount)
	{
		if (bSetEffect)
			this->SetPartEffect("mount", m_idMount, nActionType);
		if (bResetMotion || !m_pMotionMount)
			m_pMotionMount = this->GetMountMotion(m_idMount, nActionType);
		m_pRole->SetMotion("mount", m_pMotionMount);
	}
	if (m_idArmet)
	{
		if (bSetEffect)
			this->SetPartEffect("armet", m_idArmet, nActionType);
		if (bResetMotion || !m_pMotionArmet)
			m_pMotionArmet = this->GetArmetMotion(m_idArmet, this->GetWeaponActionData(), nActionType);
		m_pRole->SetMotion("armet", m_pMotionArmet);
	}
	if (m_idRWeapon)
	{
		if (bSetEffect)
			this->SetPartEffect("r_weapon", m_idRWeapon, nActionType);
		if (bResetMotion || !m_pMotionWeaponR)
			m_pMotionWeaponR = this->GetWeaponMotion(m_idRWeapon, nActionType);
		m_pRole->SetMotion("r_weapon", m_pMotionWeaponR);
	}
	if (m_idLWeapon)
	{
		if (bSetEffect)
			this->SetPartEffect("l_weapon", m_idLWeapon, nActionType);
		if (bResetMotion || !m_pMotionWeaponL)
			m_pMotionWeaponL = this->GetWeaponMotion(m_idLWeapon, nActionType);
		m_pRole->SetMotion("l_weapon", m_pMotionWeaponL);
	}
	if (bPlaySound && nActionType != _ACTION_STANDBY)
	{
		const char* pszSound = RoleDataQuery()->GetActionSound(m_nLook, this->GetWeaponActionData(), nActionType);
		if (pszSound)
		{
			const int nRange = 3000;
			::DXPlaySound ((char*)pszSound, nWorldX, nWorldY, nRange);
		}
	}
	return m_pMotionArmor->GetFrameAmount();
}

void	C3DRole::SetPos ( float fX, float fY, float fHeight, int nRotate, float fScale )
{
	m_fPosX = fX;
	m_fPosY = fY;
	if (!CMyBitmap::GameCameraIsProject())
		m_fPosH = fHeight*2;
	else
		m_fPosH = fHeight;
	m_nRotateZ = nRotate;
	m_fScale = fScale;
}

void	C3DRole::SetLightOffset( int type, float x, float y, float z, float a, float r, float g, float b )
{
	m_nShadowType = type < 1 ? 1 : type;
	m_LightOffset.x = x;
	m_LightOffset.y = -y;
	m_LightOffset.z = -z;
	m_a = a;
	m_r = r;
	m_g = g;
	m_b = b;
	if (m_pRole)
		m_pRole->SetLightOffset(type, x, -y, -z, a, r, g, b);
}

void	C3DRole::SetRGBA ( float alpha, float red, float green, float blue )
{
	if(m_pRole)
		m_pRole->SetARGB(alpha, red, green, blue);
}

void	C3DRole::AddPartEffect ( int nPart, const char* pszEffect )
{
	if (!pszEffect)
		return;
	if (!m_pRole)
		return;
	switch (nPart)
	{
	case _ROLE_PART_ARMET:
		if (!m_pRole->HasPart("armet"))
			m_pRole->AddPart("armet", "v_armet", "body", 0, NULL);
		m_pRole->AddEffect(pszEffect, "armet", FALSE);
		return;
	case _ROLE_PART_BODY:
		if (!m_pRole->HasPart("body"))
			m_pRole->AddPart("body", "v_mount", "mount", 0, NULL);
		m_pRole->AddEffect(pszEffect, "body", FALSE);
		return;
	case _ROLE_PART_RWEAPON:
		if (!m_pRole->HasPart("r_weapon"))
			m_pRole->AddPart("r_weapon", "v_r_weapon", "body", 0, NULL);
		m_pRole->AddEffect(pszEffect, "r_weapon", FALSE);
		return;
	case _ROLE_PART_LWEAPON:
		if (!m_pRole->HasPart("l_weapon"))
			m_pRole->AddPart("l_weapon", "v_l_weapon", "body", 0, NULL);
		m_pRole->AddEffect(pszEffect, "l_weapon", FALSE);
		return;
	case _ROLE_PART_MOUNT:
		if (!m_pRole->HasPart("mount"))
			m_pRole->AddPart("mount", "none", "none", 0, NULL);
		m_pRole->AddEffect(pszEffect, "mount", FALSE);
		return;
	case _ROLE_PART_MISC:
		if (!m_pRole->HasPart("misc"))
			m_pRole->AddPart("misc", "v_misc", "body", 0, NULL);
		m_pRole->AddEffect(pszEffect, "misc", FALSE);
		return;
	}
}

void	C3DRole::ClearPartEffect ( int nPart, const char* pszEffect )
{
	if (!pszEffect)
		return;
	if (!m_pRole)
		return;
	const char* pszKind = NULL;
	switch (nPart)
	{
	case _ROLE_PART_ARMET:		pszKind = "armet";		break;
	case _ROLE_PART_BODY:		pszKind = "body";		break;
	case _ROLE_PART_RWEAPON:	pszKind = "r_weapon";	break;
	case _ROLE_PART_LWEAPON:	pszKind = "l_weapon";	break;
	case _ROLE_PART_MOUNT:		pszKind = "mount";		break;
	case _ROLE_PART_MISC:		pszKind = "misc";		break;
	default:
		return;
	}
	if (m_pRole->HasPart(pszKind))
		m_pRole->Clear3DEffect(pszEffect, pszKind);
}

BOOL	C3DRole::Reload ( void )
{
	this->ResetMotion();
	if (!m_pRole)
		return true;
	return m_pRole->Reload();
}

BOOL	C3DRole::HitTest ( int nHitScreenX, int nHitScreenY, int nMapX, int nMapY )
{
	if (!m_pRole)
		return false;
	D3DXVECTOR3 vFrom, vDir;
	CMyBitmap::ScrRayBuild ( nHitScreenX, nHitScreenY, vFrom, vDir );
	D3DXMATRIXA16 matrix;
	D3DXMatrixInverse ( &matrix, NULL, &m_matrix );
	D3DXVECTOR3 from, dir;
	D3DXVec3TransformCoord ( &from, &vFrom, &matrix );
	D3DXVec3TransformNormal ( &dir, &vDir, &matrix );
	D3DXVECTOR3 min, max;
	min.x = -40.0f;		min.y = -40.0f;		min.z = -180.0f;
	max.x = 40.0f;		max.y = 40.0f;		max.z = -20.0f;
	return D3DXBoxBoundProbe ( &min, &max, &from, &dir ) ? true : false;
}

BOOL	C3DRole::Draw ( int nMapX, int nMapY )
{
	if (!m_pRole)
		return false;
#ifdef _ANALYSIS_ON
	DWORD dwAnalysisTime = ::TimeGet();
#endif
	CMyBitmap::PrepareModle();

	D3DXMATRIXA16 matrix;
	D3DXMatrixIdentity ( &matrix );
	D3DXMATRIXA16 matrixWorld;
	D3DXMatrixIdentity ( &matrixWorld );
	if (CMyBitmap::GameCameraIsProject())
	{
		CMyBitmap::Common_Scale ( &matrix, m_fScale, m_fScale, m_fScale );
		CMyBitmap::Common_Rotate ( &matrix, D3DXToRadian ( ( float )m_nRotateX ), D3DXToRadian ( ( float )m_nRotateY ), D3DXToRadian ( ( float )m_nRotateZ ) );
		CMyBitmap::Common_Translate ( &matrix, 0, 0, m_fPosH );
		CMyBitmap::Common_Translate ( &matrixWorld, m_fPosX, m_fPosY, 0 );
	}
	else
	{
		CMyBitmap::Common_Scale ( &matrix, m_fScale, m_fScale, m_fScale );
		CMyBitmap::Common_Translate ( &matrix, 0, 0, m_fPosH );
		CMyBitmap::Common_Rotate ( &matrix, 0, 0, D3DXToRadian ( ( float )( m_nRotateZ % 360 ) ) );
		CMyBitmap::Common_Rotate ( &matrixWorld, 0, D3DXToRadian ( ( float )m_nRotateY ), 0 );
		CMyBitmap::Common_Rotate ( &matrixWorld, D3DXToRadian ( ( float )m_nRotateX ), 0, 0 );
		CMyBitmap::Common_Translate ( &matrixWorld, m_fPosX, 0, m_fPosY );
	}
	D3DXMATRIXA16 matrixAll;
	D3DXMatrixMultiply ( &matrixAll, &matrix, &matrixWorld );
	m_matrix = matrixAll;

	BOOL bDrawArmet = true;
	if (m_idArmor)
	{
		if ((m_idArmor/1000)%1000 == 137 && m_idArmor%100 >= 20 && m_idArmor%100 <= 90)
			bDrawArmet = false;
		if ((m_idArmor/10000)%100 == 18)
			bDrawArmet = false;
	}

	m_pRole->Draw ( "mount", &matrix, &matrixWorld, false );
	m_pRole->Draw ( "body", &matrix, &matrixWorld, false );
	if (bDrawArmet)
		m_pRole->Draw ( "armet", &matrix, &matrixWorld, true );
	m_pRole->Draw ( "misc", &matrix, &matrixWorld, true );
	if (m_idRWeapon/10000 != 105 ||
		m_nActionType == _ACTION_ATTACK0 ||
		m_nActionType == _ACTION_ATTACK1 ||
		m_nActionType == _ACTION_ATTACK2 ||
		m_nActionType == _ACTION_ATTACK3 ||
		m_nActionType == _ACTION_FLY_ATTACK ||
		m_nActionType == _ACTION_FAST_SHOOT)
	{
		m_pRole->Draw ( "r_weapon", &matrix, &matrixWorld, true );
	}
	m_pRole->Draw ( "l_weapon", &matrix, &matrixWorld, true );
	m_pRole->ClearMatrix ( "body" );
	if (m_idMount)
		m_pRole->ClearMatrix ( "mount" );
#ifdef _ANALYSIS_ON
	DWORD dwTimePass = ::TimeGet()  - dwAnalysisTime;
	g_dw3DRoleDrawTime += dwTimePass;
	g_dw3DRoleDrawAmount ++;
	g_dw3DRoleDrawTimeFrame += dwTimePass;
	g_dw3DRoleDrawAmountFrame ++;
	if(dwTimePass > g_dw3DRoleDrawTimeMax)
		g_dw3DRoleDrawTimeMax = dwTimePass;
#endif
	return true;
}
