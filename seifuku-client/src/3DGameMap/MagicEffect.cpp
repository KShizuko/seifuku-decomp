#include "MagicEffect.h"
#include "Magic.h"
#include "GamePlayerSet.h"
#include "Player.h"
#include "Hero.h"
#include "3DMapEffect.h"
#include "3DGameMap.h"
#include "NDsound.h"
#include "Physics.h"
#include "3DMapSimpleObj.h"
#include "TerrainEffectManager.h"

CMagicEffect::CMagicEffect()
{
	m_nType = MAGICEFFECT_NONE;
	m_pData = NULL;
	m_bBegin = false;
	m_bDelayOk = false;
	m_dwBegin = ::TimeGet();
}

CMagicEffect::~CMagicEffect()
{
	this->Destroy();
}
void CMagicEffect::Destroy()
{
	int nAmount = m_setRoleInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		MagicEffectRoleInfo* pInfo = m_setRoleInfo[i];
		SAFE_DELETE(pInfo);
	}
	m_setRoleInfo.clear();
}
CMagicEffect* CMagicEffect::CreateNew(OBJID idSender, OBJID idTarget, DWORD dwMagicType, DWORD dwLevel, int nType)
{
	CMagicEffect* pEffect = new CMagicEffect;
	MYASSERT(pEffect);
	if(!pEffect->Create(idSender, idTarget, dwMagicType, dwLevel, nType))
	{
		SAFE_DELETE(pEffect);
		return NULL;
	}
	return pEffect;
}
BOOL CMagicEffect::Create(OBJID idSender, OBJID idTarget, DWORD dwMagicType, DWORD dwLevel, int nType)
{
	m_idSender = idSender;
	m_idTarget = idTarget;
	m_dwMagicType = dwMagicType;
	m_dwLevel = dwLevel;
	m_nType = nType;
	return true;
}
void CMagicEffect::AddRoleInfo(OBJID idRole, DWORD dwData)
{
	MagicEffectRoleInfo* pInfo = new MagicEffectRoleInfo;
	pInfo->dwData = dwData;
	pInfo->idRole = idRole;
	m_setRoleInfo.push_back(pInfo);
}
BOOL CMagicEffect::Process()
{
	if(m_nType == MAGICEFFECT_NONE)
	{
		MagicTypeInfo infoMagicType;
		if(!CMagic::s_objMagicData.GetMagicTypeInfo(m_dwMagicType, m_dwLevel, infoMagicType))
			return true;
		m_nType = infoMagicType.dwClientRepresent;
	}
	switch(m_nType)
	{
	case MAGICEFFECT_BODY_BODY:
		return this->ProcessBodyBody();
		break;
	case MAGICEFFECT_SOUL:
		return this->ProcessSoul();
		break;
	case MAGICEFFECT_ARROW:
		return this->ProcessArrow();
		break;
	case MAGICEFFECT_BODY_TERRAIN_BODY:
		return this->ProcessBodyTerrainBody();
		break;
	case MAGICEFFECT_BODY_TRACE_BODY:
		return this->ProcessBodyTraceBody();
		break;
	default:
		return true;
	}
}
BOOL CMagicEffect::ProcessSoul()
{
	if(!m_bBegin)
	{
		m_bBegin = true;
		m_dwMoment = 0;
		m_dwBegin = ::TimeGet();
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_idSender, true);
		if(!pPlayer)
			return true;
		CMyPos posWorld, posCell;
		pPlayer->GetPos(posCell);
		g_objGameMap.Cell2World(posCell.x, posCell.y, posWorld.x, posWorld.y);
		m_posBegin = posWorld;
		CInteractiveLayer* pLayer = g_objGameMap.GetInteractiveLayer();
		if (!pLayer)
			return true;
		m_pData = new MagicEffectSoulInfo;
		if(!m_pData)
			return true;
		MagicEffectSoulInfo* pInfo = (MagicEffectSoulInfo*)m_pData;
		C3DMapEffect*  pEffect= C3DMapEffect::CreateNew(posWorld, (char*)"soul", false, true, -1);
		pLayer->AddMapObj(pEffect);
		pEffect->SetPos(posCell);
		pInfo->pEffect[0] = pEffect;
		return false;
	}
	if(m_dwMoment == 0)
	{
		if(!m_pData)
			return true;
		MagicEffectSoulInfo* pInfo = (MagicEffectSoulInfo*)m_pData;
		if(::TimeGet() - m_dwBegin >= 1500)
		{
			m_dwMoment = 1;
			m_dwData = ::TimeGet();
		}
		else
		{
			int nOffsetY = (::TimeGet() - m_dwBegin)*100/1500;
			CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_idSender, true);
			if(!pPlayer)
			{
				pInfo->pEffect[0]->Die();
				return true;
			}
			pInfo->pEffect[0]->SetWorldPos(m_posBegin);
			pInfo->pEffect[0]->SetOffsetY(-nOffsetY);
			CMyPos posCell;
			pPlayer->GetPos(posCell);
			pInfo->pEffect[0]->SetPos(posCell);
			return false;
		}
	}
	if (m_dwMoment == 1)
	{
		if(!m_pData)
			return true;
		MagicEffectSoulInfo* pInfo = (MagicEffectSoulInfo*)m_pData;
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_idTarget, true);
		if(!pPlayer)
		{
			pInfo->pEffect[0]->Die();
			return true;
		}
		CMyPos posEnd, posCurrent, posCell;
		DWORD dwTime = ::TimeGet() - m_dwBegin - 1500;
		float fTime = (float)(dwTime*1.0/1000);
		int nCurrentV = (int)(fTime * 300.0f + 100.0f);
		float fCurrentTime = (float)((::TimeGet() - m_dwData)*1.0/1000);
		pPlayer->GetPos(posCell);
		g_objGameMap.Cell2World(posCell.x, posCell.y, posEnd.x, posEnd.y);
		posEnd.y += 50;
		BOOL bOk = CPhysics::S2DBMGetPos(nCurrentV, 0, fCurrentTime, m_posBegin, posEnd, posCurrent);
		m_posBegin = posCurrent;
		m_dwData = ::TimeGet();
		pInfo->pEffect[0]->SetWorldPos(posCurrent);
		g_objGameMap.World2Cell(posCurrent.x, posCurrent.y, posCell.x, posCell.y);
		pInfo->pEffect[0]->SetPos(posCell);
		if(bOk)
		{
			pInfo->pEffect[0]->Die();
			SAFE_DELETE(m_pData);
			if (pPlayer->GetID() == g_objHero.GetID())
			{
				if (g_objHero.m_objEffect)
					g_objHero.m_objEffect->Add((char*)"GreenGlow");
				g_objHero.AddXp(1);
				::PostCmd(CMD_XPCHANGE, 0);
			}
		}
		return bOk;
	}
	return true;
}
BOOL CMagicEffect::ProcessBodyBody()
{
	MagicTypeInfo infoMagicType;
	if(!CMagic::s_objMagicData.GetMagicTypeInfo(m_dwMagicType, m_dwLevel, infoMagicType))
		return true;
	const int nRange = 3000;
	if(!m_bBegin)
	{
		m_dwBegin = ::TimeGet();
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_idSender, true);
		if(pPlayer)
		{
			if(strcmp(infoMagicType.szSenderEffect, "NULL") != 0)
			{
				if (pPlayer->m_objEffect)
					pPlayer->m_objEffect->Add(infoMagicType.szSenderEffect);
			}
			if(!pPlayer->IsDead())
			{
				pPlayer->ResetStepDir();
				CPlayer* pTarget = g_objPlayerSet.GetPlayer(m_idTarget, true);
				if(infoMagicType.idNextMagic != ID_NONE)
					pPlayer->SetIntoneCmd(infoMagicType.dwSenderAction, pTarget? pPlayer->GetDir(pTarget) : pPlayer->GetDir(), 0, true);
				else
					pPlayer->SetIntoneCmd(infoMagicType.dwSenderAction, pTarget? pPlayer->GetDir(pTarget) : pPlayer->GetDir(), 0, false);
			}
			if(strcmp(infoMagicType.szSenderSound, "NULL") != 0)
			{
				CMyPos posWorld;
				pPlayer->GetWorldPos(posWorld);
				::DXPlaySound(infoMagicType.szSenderSound, posWorld.x, posWorld.y, nRange);
			}
		}
		this->BeginScreenEffect(infoMagicType.dwScreenRepresent);
		m_bBegin = true;
		m_dwMoment = 0;
	}
	if(::TimeGet() - m_dwBegin >= infoMagicType.dwTargetDelay && m_dwMoment == 0)
	{
		for(int i = 0; i < (int)m_setRoleInfo.size(); i++)
		{
			MagicEffectRoleInfo* pRoleInfo = m_setRoleInfo[i];
			if(!pRoleInfo)
				continue;
			CPlayer* pPlayer= g_objPlayerSet.GetPlayer(pRoleInfo->idRole, true);
			if(pPlayer)
			{
				if(strcmp(infoMagicType.szTargetEffect, "NULL") != 0)
				{
					if (pPlayer->m_objEffect)
						pPlayer->m_objEffect->Add(infoMagicType.szTargetEffect);
				}
			}
		}
		m_dwMoment = 1;
	}
	if(::TimeGet() - m_dwBegin >= infoMagicType.dwTargetDelay + infoMagicType.dwTargetWoundDelay)
	{
		for(int i = 0; i < (int)m_setRoleInfo.size(); i++)
		{
			MagicEffectRoleInfo* pRoleInfo = m_setRoleInfo[i];
			if(!pRoleInfo)
				continue;
			CPlayer* pPlayer= g_objPlayerSet.GetPlayer(pRoleInfo->idRole, true);
			if(pPlayer)
			{
				if((infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_SINGLE_HP) ||
					(infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_CROSS_HP) ||
					(infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_SECTOR_HP) ||
					(infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_ROUND_HP))
				if(!pPlayer->TestStatus(USERSTATUS_DIE))
				{
					pPlayer->ResetStepDir();
					pPlayer->SetIntoneCmd(_ACTION_BRUISE0, pPlayer->GetDir());
				}
				DWORD dwColor = _COLOR_WHITE;
				if (pPlayer->GetID() == g_objHero.GetID())
					dwColor = _COLOR_YELLOW;
				int nMultiple = g_objPlayerSet.GetRestrainMultiple(m_idSender, pRoleInfo->idRole);
				if(infoMagicType.idMagicType/10 == 1006
					||	infoMagicType.idMagicType/10 == 1005
					||	infoMagicType.idMagicType/10 == 1007)
				{
					int nAmount = infoMagicType.idMagicType%10+2;
					for(int j = 0; j < nAmount; j ++)
					{
						if(j != nAmount-1)
						{
							pPlayer->AddZoomNum(pRoleInfo->dwData/nAmount, nMultiple, dwColor, 300*j, 10);
						}
						else
						{
							pPlayer->AddZoomNum(pRoleInfo->dwData-
								(nAmount-1)*(pRoleInfo->dwData/nAmount),
								nMultiple, dwColor, 100*i, 10);
						}
					}
				}
				else
				{
					pPlayer->AddZoomNum(pRoleInfo->dwData, nMultiple, dwColor, 100, 10);
				}
				BOOL bAddLife = false;
				if(infoMagicType.dwActionSort == MAGIC_SORT_RECOVER_SINGLE_HP)
					bAddLife = true;
				if(pPlayer->GetID() != g_objHero.GetID())
				{
					if(!bAddLife)
						pPlayer->SetData(CPlayer::_PLAYER_LIFE, __max(0, (int)pPlayer->GetData(CPlayer::_PLAYER_LIFE)-(int)pRoleInfo->dwData));
					else
						pPlayer->SetData(CPlayer::_PLAYER_LIFE, __min((int)pPlayer->GetMaxLife(), (int)pPlayer->GetData(CPlayer::_PLAYER_LIFE)+(int)pRoleInfo->dwData));
				}
				if (strcmp(infoMagicType.szTargetSound, "NULL") != 0)
				{
					CMyPos posWorld;
					pPlayer->GetWorldPos(posWorld);
					::DXPlaySound(infoMagicType.szTargetSound, posWorld.x, posWorld.y, nRange);
				}
			}
		}
		return true;
	}
	return false;
}
BOOL  CMagicEffect::CreateTerrain(OBJID idSender, CMyPos posTarget, DWORD dwMagicType, DWORD dwLevel)
{
	m_idSender = idSender;
	m_posTarget = posTarget;
	m_dwMagicType = dwMagicType;
	m_dwLevel = dwLevel;
	m_nType = MAGICEFFECT_BODY_TERRAIN_BODY;
	return true;
}
CMagicEffect* CMagicEffect::CreateNewTerrain(OBJID idSender, CMyPos posTarget, DWORD dwMagicType, DWORD dwLevel)
{
	CMagicEffect* pEffect = new CMagicEffect;
	MYASSERT(pEffect);
	if(!pEffect->CreateTerrain(idSender, posTarget, dwMagicType, dwLevel))
	{
		SAFE_DELETE(pEffect);
		return NULL;
	}
	return pEffect;
}
BOOL CMagicEffect::ProcessBodyTerrainBody()
{
	MagicTypeInfo infoMagicType;
	if(!CMagic::s_objMagicData.GetMagicTypeInfo(m_dwMagicType, m_dwLevel, infoMagicType))
		return true;
	const int nRange = 3000;
	if(!m_bBegin)
	{
		m_dwBegin = ::TimeGet();
		CPlayer* pPlayer= g_objPlayerSet.GetPlayer(m_idSender, true);
		if(pPlayer)
		{
			if(strcmp(infoMagicType.szSenderEffect, "NULL") != 0)
			{
				if (pPlayer->m_objEffect)
					pPlayer->m_objEffect->Add(infoMagicType.szSenderEffect);
			}
			if(!pPlayer->IsDead() )
			{
				pPlayer->ResetStepDir();
				CPlayer* pTarget = g_objPlayerSet.GetPlayer(m_idTarget);
				if(infoMagicType.idNextMagic != ID_NONE)
					pPlayer->SetIntoneCmd(infoMagicType.dwSenderAction, pTarget? pPlayer->GetDir(pTarget) : pPlayer->GetDir(), 0, true);
				else
					pPlayer->SetIntoneCmd(infoMagicType.dwSenderAction, pTarget? pPlayer->GetDir(pTarget) : pPlayer->GetDir(), 0, false);
			}
			if(strcmp(infoMagicType.szSenderSound, "NULL") != 0)
			{
				CMyPos posWorld;
				pPlayer->GetWorldPos(posWorld);
				::DXPlaySound(infoMagicType.szSenderSound, posWorld.x, posWorld.y, nRange);
			}
			CONST int MAGICTYPE_TRASNPORT = 1080;
			if (MAGICTYPE_TRASNPORT == infoMagicType.idMagicType)
			{
				pPlayer->SetPos(m_posTarget);
				if (pPlayer->GetID() == g_objHero.GetID())
					g_objHero.SetAlignPos(m_posTarget.x, m_posTarget.y);
			}
		}
		{
			char szDir[]="ini/tme/";
			char szFile[_MAX_PATH];
			sprintf(szFile, "%s%s", szDir, infoMagicType.szGroundEffect);
			CTerainEffectUnit* pUnit = CTerainEffectUnit::CreateNew(szFile);
			if(pUnit)
			{
				CMyPos posWorld;
				g_objGameMap.Cell2World(m_posTarget.x, m_posTarget.y, posWorld.x, posWorld.y);
				pUnit->SetPos(posWorld);
				if(infoMagicType.dwActionSort == MAGICSORT_LINE ||
					MAGIC_SORT_ATTACK_CROSS_HP ==  infoMagicType.dwActionSort ||
					MAGIC_SORT_ATTACK_SECTOR_HP	== infoMagicType.dwActionSort)
				{
					CMyPos posSender;
					CPlayer* pPlayer= g_objPlayerSet.GetPlayer(m_idSender);
					if(!pPlayer && (m_idSender == g_objHero.GetID()))
						pPlayer = &g_objHero;
					if(pPlayer)
						pPlayer->GetWorldPos(posSender);
					pUnit->SetPos(posSender);
					pUnit->Rotate(posSender, posWorld);
					if(pPlayer)
					{
						pUnit->SetDir(pPlayer->GetDir());
					}
				}
				g_objGameMap.AddTerrainMagicUnit(pUnit);
			}
		}
		this->BeginScreenEffect(infoMagicType.dwScreenRepresent);
		m_bBegin = true;
	}
	if(::TimeGet() - m_dwBegin >= infoMagicType.dwTargetDelay)
	{
		for(int i = 0; i < (int)m_setRoleInfo.size(); i++)
		{
			MagicEffectRoleInfo* pRoleInfo = m_setRoleInfo[i];
			if(!pRoleInfo)
				continue;
			CPlayer* pPlayer= g_objPlayerSet.GetPlayer(pRoleInfo->idRole);
			if(!pPlayer && (pRoleInfo->idRole == g_objHero.GetID()))
				pPlayer = &g_objHero;
			if(pPlayer)
			{
				if((infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_SINGLE_HP) ||
					(infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_CROSS_HP) ||
					(infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_SECTOR_HP) ||
					(infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_ROUND_HP))
				if(!pPlayer->TestStatus(USERSTATUS_DIE))
				{
					pPlayer->ResetStepDir();
					pPlayer->SetIntoneCmd(_ACTION_BRUISE0, pPlayer->GetDir());
				}
				if(strcmp(infoMagicType.szTargetEffect, "NULL") != 0)
				{
					if (pPlayer->m_objEffect)
						pPlayer->m_objEffect->Add(infoMagicType.szTargetEffect);
				}
				int nMultiple = g_objPlayerSet.GetRestrainMultiple(m_idSender, pRoleInfo->idRole);
				DWORD dwColor = _COLOR_WHITE;
				if (pPlayer->GetID() == g_objHero.GetID())
					dwColor = _COLOR_YELLOW;
				pPlayer->AddZoomNum(pRoleInfo->dwData, nMultiple, dwColor, 100, 10);
				BOOL bAddLife = false;
				if(infoMagicType.dwActionSort == MAGIC_SORT_RECOVER_SINGLE_HP)
					bAddLife = true;
				if(pPlayer->GetID() != g_objHero.GetID())
				{
					if(!bAddLife)
						pPlayer->SetData(CPlayer::_PLAYER_LIFE, __max(0, (int)pPlayer->GetData(CPlayer::_PLAYER_LIFE)-(int)pRoleInfo->dwData));
					else
						pPlayer->SetData(CPlayer::_PLAYER_LIFE, __min((int)pPlayer->GetMaxLife(), (int)pPlayer->GetData(CPlayer::_PLAYER_LIFE)+(int)pRoleInfo->dwData));
				}
				if (strcmp(infoMagicType.szTargetSound, "NULL") != 0)
				{
					CMyPos posWorld;
					pPlayer->GetWorldPos(posWorld);
					::DXPlaySound(infoMagicType.szTargetSound, posWorld.x, posWorld.y, nRange);
				}
			}
		}
		return true;
	}
	return false;
}
BOOL CMagicEffect::ProcessBodyTraceBody()
{
	MagicTypeInfo infoMagicType;
	if(!CMagic::s_objMagicData.GetMagicTypeInfo(m_dwMagicType, m_dwLevel, infoMagicType))
		return true;
	const int nRange = 3000;
	if(!m_bBegin)
	{
		m_dwBegin = ::TimeGet();
		CPlayer* pPlayer= g_objPlayerSet.GetPlayer(m_idSender);
		if(!pPlayer && (m_idSender == g_objHero.GetID()))
			pPlayer = &g_objHero;
		if(pPlayer)
		{
			if(strcmp(infoMagicType.szSenderEffect, "NULL") != 0)
			{
				if (pPlayer->m_objEffect)
					pPlayer->m_objEffect->Add(infoMagicType.szSenderEffect);
			}
			if(!pPlayer->IsDead())
			{
				pPlayer->ResetStepDir();
				CPlayer* pTarget = g_objPlayerSet.GetPlayer(m_idTarget);
				if(infoMagicType.idNextMagic != ID_NONE)
					pPlayer->SetIntoneCmd(infoMagicType.dwSenderAction, pTarget? pPlayer->GetDir(pTarget) : pPlayer->GetDir(), 0, true);
				else
					pPlayer->SetIntoneCmd(infoMagicType.dwSenderAction, pTarget? pPlayer->GetDir(pTarget) : pPlayer->GetDir(), 0, false);
			}
			if(strcmp(infoMagicType.szSenderSound, "NULL") != 0)
			{
				CMyPos posWorld;
				pPlayer->GetWorldPos(posWorld);
				::DXPlaySound(infoMagicType.szSenderSound, posWorld.x, posWorld.y, nRange);
			}
		}
		CMyPos posWorld, posCell;
		pPlayer->GetPos(posCell);
		g_objGameMap.Cell2World(posCell.x, posCell.y, posWorld.x, posWorld.y);
		m_posBegin = posWorld;
		CInteractiveLayer* pLayer = g_objGameMap.GetInteractiveLayer();
		if (!pLayer)
			return true;
		C3DMapEffect* pObj = C3DMapEffect::CreateNew(m_posBegin, infoMagicType.szTraceEffect, false, true, -1);
		if(pObj)
		{
			pLayer->AddMapObj(pObj);
		}
		m_pData = pObj;
		m_bBegin = true;
		this->BeginScreenEffect(infoMagicType.dwScreenRepresent);
		pPlayer = g_objPlayerSet.GetPlayer(m_idTarget, true);
		if(!pPlayer)
		{
			pObj->Die();
			pObj = NULL;
			return true;
		}
		CMyPos posEnd;
		CMy3DPos posMyBegin, posMyEnd;
		CMy3DDir dir3D;
		posMyBegin.x = m_posBegin.x;
		posMyBegin.y = m_posBegin.y;
		posMyBegin.z = 0;
		pPlayer->GetPos(posCell);
		g_objGameMap.Cell2World(posCell.x, posCell.y, posEnd.x, posEnd.y);
		posEnd.y += 50;
		posMyEnd.x = posEnd.x;
		posMyEnd.y = posEnd.y;
		posMyEnd.z = 0;
		CPhysics::S3Get3DDir(posMyBegin, posMyEnd, dir3D);
		pObj->SetRotate((float)(dir3D.fHorizontal+1.57), dir3D.fVertical);
		return false;
	}
	C3DMapEffect* pObj = (C3DMapEffect*)m_pData;
	if(!pObj)
		return false;
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_idTarget, true);
	if(!pPlayer)
	{
		pObj->Die();
		pObj = NULL;
		return true;
	}
	CMyPos posEnd, posCurrent, posCell;
	DWORD dwTime = ::TimeGet() - m_dwBegin;
	float fTime = (float)(dwTime*1.0/1000);
	int nCurrentV = (int)(fTime * 300.0f + 100.0f);
	float fCurrentTime = (float)((::TimeGet() - m_dwBegin)*1.0/1000);
	pPlayer->GetPos(posCell);
	g_objGameMap.Cell2World(posCell.x, posCell.y, posEnd.x, posEnd.y);
	posEnd.y += 50;
	CMy3DPos posMyBegin, posMyEnd;
	CMy3DDir dir3D;
	posMyBegin.x = m_posBegin.x;
	posMyBegin.y = m_posBegin.y;
	posMyBegin.z = 0;
	posMyEnd.x = posEnd.x;
	posMyEnd.y = posEnd.y;
	posMyEnd.z = 0;
	CPhysics::S3Get3DDir(posMyBegin, posMyEnd, dir3D);
	pObj->SetRotate((float)(dir3D.fHorizontal+1.57), dir3D.fVertical);
	BOOL bOk = CPhysics::S2DBMGetPos(nCurrentV, 0, fCurrentTime, m_posBegin, posEnd, posCurrent);
	m_posBegin = posCurrent;
	posCurrent.y -= 50;
	pObj->SetWorldPos(posCurrent);
	if(bOk)
	{
		for(int i = 0; i < (int)m_setRoleInfo.size(); i++)
		{
			MagicEffectRoleInfo* pRoleInfo = m_setRoleInfo[i];
			if(!pRoleInfo)
				continue;
			CPlayer* pPlayer= g_objPlayerSet.GetPlayer(pRoleInfo->idRole);
			if(!pPlayer && (pRoleInfo->idRole == g_objHero.GetID()))
				pPlayer = &g_objHero;
			if(pPlayer)
			{
				if((infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_SINGLE_HP) ||
					(infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_CROSS_HP) ||
					(infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_SECTOR_HP) ||
					(infoMagicType.dwActionSort == MAGIC_SORT_ATTACK_ROUND_HP))
				if(!pPlayer->TestStatus(USERSTATUS_DIE))
				{
					pPlayer->ResetStepDir();
					pPlayer->SetIntoneCmd(_ACTION_BRUISE0, pPlayer->GetDir());
				}
				if(strcmp(infoMagicType.szTargetEffect, "NULL") != 0)
				{
					if (pPlayer->m_objEffect)
						pPlayer->m_objEffect->Add(infoMagicType.szTargetEffect);
				}
				DWORD dwColor = _COLOR_WHITE;
				if (pPlayer->GetID() == g_objHero.GetID())
					dwColor = _COLOR_YELLOW;
				int nMultiple = g_objPlayerSet.GetRestrainMultiple(m_idSender, pRoleInfo->idRole);
				pPlayer->AddZoomNum(pRoleInfo->dwData, nMultiple, dwColor, 100, 10);
				BOOL bAddLife = false;
				if(infoMagicType.dwActionSort == MAGIC_SORT_RECOVER_SINGLE_HP)
					bAddLife = true;
				if(pPlayer->GetID() != g_objHero.GetID())
				{
					if(!bAddLife)
						pPlayer->SetData(CPlayer::_PLAYER_LIFE, __max(0, (int)pPlayer->GetData(CPlayer::_PLAYER_LIFE)-(int)pRoleInfo->dwData));
					else
						pPlayer->SetData(CPlayer::_PLAYER_LIFE, __min((int)pPlayer->GetMaxLife(), (int)pPlayer->GetData(CPlayer::_PLAYER_LIFE)+(int)pRoleInfo->dwData));
				}
				if (strcmp(infoMagicType.szTargetSound, "NULL") != 0)
				{
					CMyPos posWorld;
					pPlayer->GetWorldPos(posWorld);
					::DXPlaySound(infoMagicType.szTargetSound, posWorld.x, posWorld.y, nRange);
				}
			}
		}
		pObj->Die();
		m_pData = NULL;
	}
	return bOk;
}
void CMagicEffect::BeginScreenEffect(DWORD dwScreenRepresent)
{
	if(dwScreenRepresent & MAGIC_SHAKENORMAL)
		g_objGameMap.BeginShakeEffect();
	if(dwScreenRepresent & MAGIC_COLORNORMAL)
		g_objGameMap.BeginColorEffect();
	if(dwScreenRepresent & MAGIC_SCALENORMAL)
		g_objGameMap.BeginScaleEffect();
}
