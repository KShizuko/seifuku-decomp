#pragma warning(disable:4786)
#include "BaseFunc.h"
#include "ani.H"
#include "3DGameMap.h"
#include "Role.h"
#include "3droledata.H"
#include "Item.h"

int CRole::GetDir(CMyPos posTarget) const
{
	CMyPos posMap = const_cast<CRoleInfo&>(m_Info).posMap;
	int nDir	=CGameMap::GetDirection(posMap.x, posMap.y, posTarget.x, posTarget.y);
	return (nDir+1)%8;
}

int CRole::GetDir(CRole* pTarget) const
{
	if (!pTarget)
		return this->GetDir();

	CMyPos posTarget;
	pTarget->GetPos(posTarget);

	return this->GetDir(posTarget);
}

DWORD CRole::GetDistance(CRole* pTarget)
{
	CMyPos posTarget;
	pTarget->GetPos(posTarget);

	return __max(abs(m_Info.posMap.x-posTarget.x), abs(m_Info.posMap.y-posTarget.y));
}

BOOL CRole::IsMapReached(void)
{
	if(m_Info.posMap.x==m_Info.cmdProc.posTarget.x &&
			m_Info.posMap.y==m_Info.cmdProc.posTarget.y)
		return true;
	else
		return false;
}

BOOL CRole::SetPos(int iPosX, int iPosY)
{
	{
		m_Info.posMap.x	= iPosX;
		m_Info.posMap.y	= iPosY;

		g_objGameMap.Cell2World(m_Info.posMap.x, m_Info.posMap.y, m_Info.posWorld.x,m_Info.posWorld.y);
		g_objGameMap.World2Screen(m_Info.posWorld.x, m_Info.posWorld.y, m_Info.posScr.x, m_Info.posScr.y);

		m_Info.posActionBegin = m_Info.posActionEnd = m_Info.posWorld;
	}

	return true;
}

void CRole::ShowName(void)
{
	CMySize sizeFont;
	sizeFont.iHeight	=CMyBitmap::GetFontSize();
	sizeFont.iWidth		=sizeFont.iHeight/2;

	const char* pszString	=this->GetName();

	int iInfoLen	=strlen(pszString);
	if (iInfoLen <= 0)
		return;

	int nShowX	=m_Info.posScr.x-sizeFont.iWidth*iInfoLen/2;
	int nShowY	=m_Info.posScr.y-OFFSET_NAMESHOW-sizeFont.iHeight;
	{
		CMyBitmap::ShowString(nShowX+1, nShowY+1-this->GetOffsetY(), 0x0, pszString);
		CMyBitmap::ShowString(nShowX, nShowY-this->GetOffsetY(), _COLOR_WHITE, pszString);
	}
}

void CRole::Say(const char* pszWords)
{
	if (!pszWords)
		return;

	m_objSayHello.SetWords(pszWords);
}

BOOL CRole::IsMouseFocus(int evtMouse)
{
	if(!m_pIRoleView)
		return false;
	if(!this->IsVisible())
		return false;
	int nMouseX, nMouseY;
	int evtNow	=::MouseCheck(nMouseX, nMouseY);
	if(_MOUSE_NONE != evtMouse && evtMouse != evtNow)
		return false;

	int nBgViewportX, nBgViewportY;
	g_objGameMap.MapScaleMousePos(nMouseX, nMouseY);
	g_objGameMap.GetBgViewport(nBgViewportX, nBgViewportY);
	if(this->GetRoleType() == _ROLE_BOOTH_NPC)
	{
		CMyPos posCell;
		this->GetPos(posCell);
		nBgViewportX = posCell.x;
		nBgViewportY = posCell.y;
		CMyPos posMyCell;
		g_objGameMap.Screen2Cell(nMouseX, nMouseY, posMyCell.x, posMyCell.y);
		nMouseX	= posMyCell.x;
		nMouseY = posMyCell.y;
	}

	if (m_nViewType == _ROLE_VIEW_TERRAIN)
		return g_objGameMap.IsTerrainNpcFocus(m_Info.id);

	if (!m_pIRoleView->HitTest(nMouseX, nMouseY+m_Info.nFlyOffset, nBgViewportX, nBgViewportY))
		return false;

	return true;
}

BOOL CRole::IsVisible(void)
{
	CMyPos posCell;
	this->GetPos(posCell);
	if (!m_bVisible)
		return false;
	return g_objGameMap.IsPosVisible(posCell, 2);
}

BOOL CRole::IsWalking(void)
{
	BOOL bReal = false;
	switch (this->GetActionType())
	{
	case _ACTION_WALKL:
	case _ACTION_WALKL_I:

		bReal = true;
		break;

	default:
		break;
	}

	return bReal;
}

BOOL CRole::IsRunning(void)
{
	BOOL bReal = false;
	switch (this->GetActionType())
	{
	case _ACTION_RUNL:
	case _ACTION_RUNR:
	case _ACTION_RUNL_I:
	case _ACTION_RUNR_I:
		bReal = true;
		break;

	default:
		break;
	}

	return bReal;
}

BOOL CRole::IsJumping(void)
{
	switch (this->GetActionType())
	{
	case _ACTION_JUMP:
	case _ACTION_JUMP_BACK:
	case _ACTION_JUMP_RUN:
	case _ACTION_JUMP_ATK:
	case _ACTION_FLY_MOVE:
		return true;
	default:
		return false;
	}
}

BOOL CRole::IsMoving(void)
{
	BOOL bReal = false;
	switch (this->GetActionType())
	{
	case _ACTION_WALKL:
	case _ACTION_WALKL_I:
	case _ACTION_RUNL:
	case _ACTION_RUNL_I:
	case _ACTION_RUNR:
	case _ACTION_RUNR_I:
	case _ACTION_JUMP:
	case _ACTION_JUMP_BACK:
	case _ACTION_JUMP_RUN:
	case _ACTION_JUMP_ATK:

	case _ACTION_BUMP:
	case _ACTION_POPOFF:
	case _ACTION_FLY_MOVE:
		bReal = true;
		break;

	default:
		break;
	}

	return bReal;
}

BOOL CRole::IsIdle(void)
{
	switch (this->GetCommandType())
	{
	case _COMMAND_POSE:
	case _COMMAND_STANDBY:
	case _COMMAND_EMOTION:
	case _COMMAND_ACTION:
	case _COMMAND_MINE:
	case _COMMAND_WOUND:
	case _COMMAND_DEFEND:
		return true;
	default:
		return false;
	}
}

BOOL CRole::IsStandBy(void)
{
	BOOL bStandBy = false;

	if (_COMMAND_EMOTION == m_Info.cmdProc.iType
			|| _COMMAND_STANDBY == m_Info.cmdProc.iType)
		bStandBy = true;

	switch (this->GetActionType())
	{
	case _ACTION_STANDBY:
	case _ACTION_REST1:
	case _ACTION_REST2:
	case _ACTION_REST3:
	case _ACTION_STANDBY_I:
	case _ACTION_ALERT:
	case _ACTION_ALERT_I:
		bStandBy = true;
		break;

	default:
		break;
	}

	return bStandBy;
}

BOOL CRole::IsBumpBackEnable(void)
{
	BOOL bBackEnable = false;
	switch(this->GetRoleType())
	{
	case _ROLE_PLAYER:
	case _ROLE_HERO:
	case _ROLE_MONSTER:
		bBackEnable = true;
		break;
	}

	return bBackEnable;
}

void CRole::GetPrePos(int& nPosX, int& nPosY)
{
	nPosX	=m_Info.posMap.x-_DELTA_X[m_Info.iDirection];
	nPosY	=m_Info.posMap.y-_DELTA_Y[m_Info.iDirection];
}

void CRole::ClrStatus(DWORD dwStatus, int nIndex)
{
	m_dwStatus[nIndex] &=  ~dwStatus;
}

void CRole::SetStatus(DWORD dwStatus, int nIndex)
{
	m_dwStatus[nIndex] = dwStatus;
}

BOOL CRole::TestStatus(DWORD dwStatus, int nIndex)
{
	return ((m_dwStatus[nIndex]&dwStatus) == dwStatus);
}

void CRole::SetName	(const char* pszName)
{
	if (!pszName || strlen(pszName) >= _MAX_NAMESIZE)
		return;

	strcpy(m_Info.szName, pszName);
}

int CRole::TestAniEffect(const char* pszEffect)
{
	int nAmount = m_dequeAniEffect.size();
	if(nAmount == 0)
		return 0;
	for(int i = 0; i < nAmount; i ++)
	{
		CAniEffect* pAniEffect = m_dequeAniEffect[i];
		if(pAniEffect && (0 == strcmp(pszEffect, pAniEffect->GetIndex())))
			return pAniEffect->GetLoopTime();		
	}
	return 0;
}
BOOL CRole::AddAniEffectEx  (const char* pszEffectTitle,
							 int nTimes,
							 DWORD dwInterval,
							 int nShowWay,
							 DWORD dwLoopInterval,
							 int nOffsetX,
							 int nOffsetY,
							 int nExigence,
							 DWORD dwDelay)
{
	if (!pszEffectTitle || nTimes == 0)
		return false;

	CAniEffect* pAniEffect	=new CAniEffect;
	if (!pAniEffect)
		return false;

	if (!pAniEffect->CreateEx(pszEffectTitle, nTimes, dwInterval, nShowWay, dwLoopInterval, nOffsetX, nOffsetY, nExigence, dwDelay))
	{
		SAFE_DELETE(pAniEffect);
		return false;
	}

	m_dequeAniEffect.push_back(pAniEffect);
	return true;
}

BOOL CRole::AddAniEffect(const char* pszEffect)
{
	CAniEffect* pAniEffect	=new CAniEffect;
	if (!pAniEffect)
		return false;

	if (!pAniEffect->Create(pszEffect))
	{
		SAFE_DELETE(pAniEffect);
		return false;
	}

	m_dequeAniEffect.push_back(pAniEffect);
	return true;
}

void CRole::AddScrEffect(const char* pszEffect)
{
	IF_NOT (pszEffect)
		return;
	m_objScrEffect->Add((char*)pszEffect);
}

void CRole::DelScrEffect(const char* pszEffect)
{
	IF_NOT (pszEffect)
		return;
	m_objScrEffect->DeleteByName((char*)pszEffect);
}

void CRole::ShowScrEffect(void)
{
	m_objScrEffect->Process();
	int nBgX = 0, nBgY = 0;
	g_objGameMap.World2Bg(m_Info.posWorld.x, m_Info.posWorld.y, nBgX, nBgY);
	CMyPos posScr;
	posScr.x = nBgX;
	posScr.y = nBgY - this->GetOffsetY();
	m_objScrEffect->Show(posScr);
}

BOOL CRole::IsMapAttainable(int iPosX,int iPosY)
{
	CellInfo* pCellInfo = g_objGameMap.GetCell(iPosX, iPosY);
	if(!pCellInfo)
		return false;
	if(g_objGameMap.GetGroundMask(pCellInfo) == 1)
		return false;
	return true;
}

void CRole::ProcessAniEffect()
{
	int nEffectCount	=m_dequeAniEffect.size();
	for (int i=nEffectCount-1; i>=0 ; i--)
	{
		CAniEffect* pAniEffect	=m_dequeAniEffect[i];
		if (pAniEffect)
		{
			if (!pAniEffect->Process())
			{
				m_dequeAniEffect.erase(m_dequeAniEffect.begin()+i);
				SAFE_DELETE(pAniEffect);
			}
		}
	}

	m_objEffect->Process();
}

void CRole::ShowAniEffect(int nPosX, int nPosY)
{
	int nEffectCount	=m_dequeAniEffect.size();
	for (int i=nEffectCount-1; i>=0 ; i--)
	{
		CAniEffect* pAniEffect	=m_dequeAniEffect[i];
		if (pAniEffect)
			pAniEffect->Show(nPosX, nPosY);
	}
}

void CRole::ClrAniEffect(const char* pszEffect)
{
	if (!pszEffect)
	{
		int nEffectCount	=m_dequeAniEffect.size();
		for (int i=0; i<nEffectCount; i++)
		{
			CAniEffect* pAniEffect	=m_dequeAniEffect[i];
			if (pAniEffect)
				SAFE_DELETE(pAniEffect);
		}

		m_dequeAniEffect.clear();
	}
	else
	{
		int nEffectCount	=m_dequeAniEffect.size();
		for (int i=0; i<nEffectCount; i++)
		{
			CAniEffect* pAniEffect	=m_dequeAniEffect[i];
			if (pAniEffect)
			{
				if (0 == strcmp(pszEffect, pAniEffect->GetIndex()))
					pAniEffect->Init();
			}
		}
	}
}

void CRole::SetRGBA(float r, float g, float b, float a)
{
	DWORD dwARGB = (((DWORD)((int)(a*255) & 0xff)) << 24) | (((DWORD)((int)(r*255) & 0xff)) << 16) | (((DWORD)((int)(g*255) & 0xff)) << 8) | ((int)(b*255) & 0xff);
	this->SetARGB(dwARGB);
	if (m_pIRoleView)
		m_pIRoleView->SetRGBA(r, g, b, a);
}

void CRole::SetArmor	(OBJID idType)
{
	m_Info.idArmorType = idType;
	if (m_pIRoleView && m_Info.nTransformLook == 0 && m_Info.idMantleType == 0)
		m_pIRoleView->SetVariable(_VAR_ARMOR, idType);
}

void CRole::SetMantle	(OBJID idType)
{
	m_Info.idMantleType = idType;
	if (m_pIRoleView && m_Info.nTransformLook == 0)
	{
		if (idType == 0)
			m_pIRoleView->SetVariable(_VAR_ARMOR, m_Info.idArmorType);
		else
			m_pIRoleView->SetVariable(_VAR_ARMOR, idType);
	}
}
void CRole::SetArmet	(OBJID idType)
{
	m_Info.idArmetType = idType;
	if (m_pIRoleView && m_Info.nTransformLook == 0)
		m_pIRoleView->SetVariable(_VAR_ARMET, idType);
}

void CRole::SetRWeapon	(OBJID idType)
{
	m_Info.idRWeaponType = idType;
	if (m_pIRoleView && m_Info.nTransformLook == 0)
	{
		m_pIRoleView->SetVariable(_VAR_RWEAPON, idType);
	}
}

void CRole::SetLWeapon	(OBJID idType)
{
	m_Info.idLWeaponType = idType;
	if (m_pIRoleView && m_Info.nTransformLook == 0)
	{
		m_pIRoleView->SetVariable(_VAR_LWEAPON, idType);
	}
}
void CRole::SetMount	(OBJID idType)
{
	m_Info.idMountType = idType;
	if (m_pIRoleView && m_Info.nTransformLook == 0)
		m_pIRoleView->SetVariable(_VAR_MOUNT, idType);
}

void CRole::SetMisc		(OBJID idType)
{
	m_Info.idMiscType = idType;
	if (m_pIRoleView && m_Info.nTransformLook == 0)
		m_pIRoleView->SetVariable(_VAR_MISC, idType);
}

BOOL CRole::IsAttacking(void)
{
	return CRole::IsAttackAction(this->GetActionType());
}

int CRole::GetIDType(OBJID id)
{
	if((id >= MONSTERID_FIRST) && (id <= MONSTERID_LAST))
		return _PLAYER_ID_MONSTER;
	if((id >= PETID_FIRST) && (id <= PETID_LAST))
		return _PLAYER_ID_PET;
	if((id >= SYSNPCID_FIRST) && (id <= SYSNPCID_LAST))
		return _PLAYER_ID_SYSNPC;
	if((id >= DYNANPCID_FIRST) && (id <= DYNANPCID_LAST))
		return _PLAYER_ID_DYNNPC;
	if((id >= CALLPETID_FIRST) && (id <= CALLPETID_LAST))
		return _PLAYER_ID_CALLPET;
	if(id >= PLAYER_ID_FIRST)
		return _PLAYER_ID_PLAYER;
	return 0;
}
DWORD CRole::GetActionDelayInfoKey	(CRole* pTarget)
{
	if (!pTarget)
		return 0;

	DWORD dwWeaponData = 0;
	if (m_pIRoleView)
		dwWeaponData = m_pIRoleView->GetVariable(_VAR_WEAPON_ACTION_DATA);

	DWORD dwActionData = this->GetActionType() % 1000;

	DWORD dwLookData = this->GetLook() % 10;

	DWORD dwDistance = this->GetDistance(pTarget) % 10;

	DWORD dwRelativeDir = (this->GetDir(pTarget) + 8 - this->GetDir()) % 8;

	DWORD dwKey = dwRelativeDir	* 100000000	+
		dwDistance		* 10000000	+
		dwLookData		* 1000000	+
		dwWeaponData	* 1000		+
		dwActionData;

	return dwKey;
}
DWORD CRole::GetWeaponInfoKey (CRole* pTarget)
{
	DWORD dwKey = 0;
	if (m_pIRoleView)
		dwKey = m_pIRoleView->GetVariable(_VAR_WEAPON_ACTION_DATA);

	if (pTarget && pTarget->IsMonster())
	{
		CONST CMonsterInfo* pInfo = RoleDataQuery()->GetMonsterInfo(pTarget->GetName());
		if (pInfo)
		{
			dwKey += pInfo->nBodyType * 1000;
		}
	}

	return dwKey;
}
int CRole::GetHitDir (CRole* pTarget)
{
	if (!pTarget)
		return 0;

	int nDir = (this->GetDir(pTarget) - pTarget->GetDir() + 8) % 8;
	BOOL bNear = nDir < 5;
	if (!bNear)
		nDir = 8 - nDir;

	int nResult = 0;
	if (nDir >= 0)
	{
		if (nDir < 2)
			nResult = 2;
		else if (nDir == 2)
			nResult = bNear ? 3 : 4;
		else if (nDir > 2 && nDir < 5)
			nResult = 1;
	}
	return nResult;
}
