#pragma warning(disable:4786)
#include "BaseFunc.h"
#include "nd_bitmap.H"
#include "ani.H"
#include "3DGameMap.h"
#include "3droledata.H"
#include "Role.h"
#include "Hero.h"
#include "GameMsg.h"
#include "GameDataSet.H"
#include "GamePlayerSet.h"
#include "Item.h"
#include "Physics.h"
#include <math.h>

const int _MAX_ROLEDATA		=2;
const int _DEFAULT_JUMP_HEIGH = 120;

int CRole::s_nShadowType = SHADOW_SIMPLE;
float CRole::s_fBaseScale = 0.75f;
void CRole::Init(void)
{
	memset(&m_Info, 0L, sizeof(m_Info));

	this->InitCommand();

	m_Info.iSize = 1;
	m_Info.iAddSize = 0;
	m_Info.iZoomPercent = 100;
	m_Info.objPath.SetSearchMethod(_ASTAR_CLOSE);
	m_pIRoleView	=NULL;
	m_dwStatus[1]	=USERSTATUS_NORMAL;
	m_dwStatus[0]	=USERSTATUS_NORMAL;

	m_tmLastAtk		= 0;
	m_nLastAttackAction = -1;
	m_tmDisappear	= 0;
	m_dwRoleAttribute = 0;
	m_Info.nFlyOffset = 0;
	m_bVisible = true;
	m_nPreAction = -1;
	if(m_objEffect)
		m_objEffect->Release();
	m_objEffect = ::Game3DEffectCreate(NULL);
	if(m_objScrEffect)
		m_objScrEffect->Release();
	m_objScrEffect = ::Game3DEffectCreate(NULL);
}

BOOL CRole::Create(UINT uLook, int nViewType)
{
	m_Info.usLook = uLook;
	m_nViewType = nViewType;
	if (m_pIRoleView)
	{
		m_pIRoleView->Release();
		m_pIRoleView = NULL;
	}
	if (nViewType == _ROLE_VIEW_TASKNPC && uLook / 10000 != 0)
		nViewType = _ROLE_VIEW_NPCX;
	m_pIRoleView = ::RoleViewCreate(nViewType);
	if (!m_pIRoleView)
		return false;
	m_pIRoleView->SetVariable(_VAR_LOOK, uLook);
	return true;
}

BOOL CRole::Create(UINT uLook, int nViewType, OBJID id, CMyPos pos)
{
	m_Info.id = id;
	m_Info.usLook = uLook;
	m_nViewType = nViewType;
	if(m_pIRoleView)
	{
		m_pIRoleView->Release();
		m_pIRoleView = NULL;
	}
	if (nViewType == _ROLE_VIEW_TASKNPC && uLook / 10000 != 0)
		nViewType = _ROLE_VIEW_NPCX;
	m_pIRoleView = ::RoleViewCreate(nViewType);
	if (!m_pIRoleView)
		return false;
	m_pIRoleView->SetVariable(_VAR_LOOK, uLook);
	m_pIRoleView->SetPos((float)pos.x, (float)pos.y, 0.0f, 0, 1.0f);
	if(m_nViewType == _ROLE_VIEW_TERRAIN)
		g_objGameMap.AddTerrainNpc(uLook/10, uLook%10, id, pos);
	return true;
}

void CRole::Destroy(void)
{
	if (m_pIRoleView)
	{
		if(m_nViewType == _ROLE_VIEW_TERRAIN)
			g_objGameMap.DeleteTerrainNpc(m_Info.id);
		m_pIRoleView->Release();
		m_pIRoleView = NULL;
	}
	this->ClrAniEffect(NULL);
	this->Init();
	if(m_objEffect)
	{
		m_objEffect->Release();
		m_objEffect = NULL;
	}
	if(m_objScrEffect)
	{
		m_objScrEffect->Release();
		m_objScrEffect = NULL;
	}
}

void CRole::AllotRes()
{
	if(!m_pIRoleView)
		m_pIRoleView = ::RoleViewCreate(_ROLE_VIEW_ROLE);
	this->SetLook(m_Info.usLook);
	this->SetHair(m_Info.usHair);
	this->SetArmor(m_Info.idArmorType);
	this->SetHead(m_Info.usHead);
	this->SetRWeapon(m_Info.idRWeaponType);
	this->SetLWeapon(m_Info.idLWeaponType);
	this->SetMount(m_Info.idMountType);
	this->SetArmet(m_Info.idArmetType);
	this->SetMisc(m_Info.idMiscType);
	this->Transform(m_Info.nTransformLook);
}

void CRole::SetLook(USHORT usLook)
{
	m_Info.usLook	=usLook;

	if(m_pIRoleView && m_Info.nTransformLook == 0)
		m_pIRoleView->SetVariable(_VAR_LOOK, usLook);
	if(m_nViewType == _ROLE_VIEW_TERRAIN)
	{
		g_objGameMap.DeleteTerrainNpc(m_Info.id);
		g_objGameMap.AddTerrainNpc(usLook/10, usLook%10, m_Info.id, m_Info.posMap);
	}
}

void CRole::SetHair(USHORT usHair)
{
	if ((usHair%100) == 0)
		usHair = 0;

	m_Info.usHair	=usHair;
	if(m_pIRoleView && m_Info.nTransformLook == 0)
		m_pIRoleView->SetVariable(_VAR_HAIR, usHair);
}

void CRole::SetHead(USHORT usHead)
{
	m_Info.usHead	=usHead;
	if(m_pIRoleView && m_Info.nTransformLook == 0)
		m_pIRoleView->SetVariable(_VAR_HEAD, usHead);
}

void CRole::InitCommand(void)
{
	m_Info.cmdProc.iType	=_COMMAND_STANDBY;
	m_Info.cmdProc.iStatus	=_CMDSTATUS_BEGIN;
	m_Info.cmdProc.posTarget.x	=0;
	m_Info.cmdProc.posTarget.y	=0;
}

DWORD CRole::SetCommand(CCommand* pCommand)
{
	if (!pCommand)
		return 0;
	memcpy(&(m_Info.cmdProc), pCommand, sizeof(CCommand));
	return 1;
}

int	CRole::GetCommandType(void)
{
	if (_CMDSTATUS_ACCOMPLISH == this->GetCommandStatus())
		return _COMMAND_STANDBY;

	return m_Info.cmdProc.iType;
}

DWORD CRole::GetFrameInterval(int nActType)
{
	DWORD dwMyIndex, dwInterval;
	if(m_pIRoleView)
	{
		dwMyIndex = ((m_Info.idMountType*1000 + this->GetLook())*1000 + m_pIRoleView->GetVariable(_VAR_WEAPON_ACTION_DATA))*1000 + nActType;
		dwInterval = RoleDataQuery()->GetActionTimeInterval(dwMyIndex);
	}
	else
	{
		dwInterval = 33;
	}
	if (this->IsAttacking())
	{
		if (this->TestStatus(0x00200000)
			|| this->TestStatus(USERSTATUS_SPSOLDIER)
			|| this->TestStatus(USERSTATUS_TORNADO))
			dwInterval = dwInterval*2/3;
	}
	else if (this->IsMoving())
	{
		if(this->GetActionType() == _ACTION_JUMP)
		{
			double dbDicAll = sqrt(
				(m_Info.posActionBegin.x- m_Info.posActionEnd.x) *
				(m_Info.posActionBegin.x - m_Info.posActionEnd.x) +
				(m_Info.posActionBegin.y - m_Info.posActionEnd.y) *
				(m_Info.posActionBegin.y - m_Info.posActionEnd.y)
				);
			dwInterval = __max(33, (DWORD) (dwInterval*dbDicAll/300+1));
		}
		if(this->TestStatus(USERSTATUS_TORNADO))
			dwInterval = dwInterval/2;
	}
	if(this->TestStatus(USERSTATUS_FLY) && this->GetCommandType() == _COMMAND_LOCKATK)
	{
		dwInterval = dwInterval*1/2;
	}
	return dwInterval;
}

void CRole::SetActionData(void)
{
	BOOL bAccelerate = false;
	if(this->TestStatus(USERSTATUS_FLY))
	{
		switch(m_Info.iActType)
		{
		case _ACTION_JUMP:
			m_Info.iActType = _ACTION_FLY_MOVE;
			break;
		case _ACTION_STANDBY:
			m_Info.iActType = _ACTION_FLY_STANDBY;
			break;
		case _ACTION_WALKL:
		case 115:
		case _ACTION_RUNL:
		case _ACTION_RUNR:
			bAccelerate = true;
			m_Info.iActType = _ACTION_FLY_MOVE;
			break;
		case _ACTION_ALERT:
			m_Info.iActType = _ACTION_FLY_ALERT;
			break;
		case _ACTION_ATTACK0:
		case _ACTION_ATTACK1:
		case _ACTION_ATTACK2:
		case _ACTION_ATTACK3:
			m_Info.iActType = _ACTION_FLY_ATTACK;
			break;
		case _ACTION_FLY_STANDBY:
		case _ACTION_FLY_ALERT:
		case _ACTION_FLY_MOVE:
		case _ACTION_FLY_ATTACK:
		case _ACTION_FLY_DOWN:
		case _ACTION_FLY_UP:
		case _ACTION_FLY_DIE:
		case _ACTION_FLY_WOUND:
		case _ACTION_BODY0:
			break;
		case _ACTION_BRUISE0:
			m_Info.iActType = _ACTION_FLY_WOUND;
			break;
		default:
			m_Info.iActType = _ACTION_FLY_STANDBY;
			break;
		}
	}
	CMyPos posRole;
	g_objGameMap.World2Cell(m_Info.posWorld.x, m_Info.posWorld.y, posRole.x, posRole.y);
	BOOL bPlaySound = g_objGameMap.IsPosVisible(posRole, 4);
	m_Info.iActFrameIndex	=0;
	m_Info.iActFrameAmount	=m_pIRoleView->SetAction(m_Info.iActType, true, bPlaySound, m_Info.posWorld.x, m_Info.posWorld.y, true);
	m_Info.timeFirstFrame	=::TimeGet();
	m_Info.timeFrameInterval = this->GetFrameInterval(m_Info.iActType);
	if(bAccelerate)
		m_Info.timeFrameInterval = m_Info.timeFrameInterval/4;
	if (this->IsMoving())
		m_nPreAction = m_Info.iActType;
	int nTerrain = g_objGameMap.GetTerrain(m_Info.posMap.x, m_Info.posMap.y);
	const ActionMapEffectInfo* pInfo = RoleDataQuery()->GetActionMapEffect(m_Info.usLook, m_Info.iActType, nTerrain);
	if (pInfo && pInfo->nShowTime == 0 && this->GetCommandStatus() != _CMDSTATUS_OVER)
	{
		g_objGameMap.Add3DMapEffect(m_Info.posWorld, (char*)pInfo->szEffect, true, pInfo->bDirEnable ? m_Info.iDirection : -1);
	}
	DWORD dwKey = m_pIRoleView ? m_pIRoleView->GetVariable(_VAR_ACTION_DATA) : 0;
	CONST CActionCtrlInfo* pCtrl = RoleDataQuery()->GetActionCtrlInfo(dwKey);
	m_bVisible = pCtrl ? pCtrl->bVisible : true;
}

void CRole::ResetActionData(void)
{
	if ((this->GetID() == g_objHero.GetID() || this->IsMyPet())
			&& this->IsMoving())
		return;

	if (_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
		this->ProcessCommand();

	m_Info.iActFrameIndex	= m_Info.iActFrameAmount;
	m_Info.posWorld			= m_Info.posActionEnd;
	CMyPos posCell = {0, 0};
	g_objGameMap.World2Cell(m_Info.posWorld.x, m_Info.posWorld.y, posCell.x, posCell.y);
	m_Info.posMap = posCell;
	g_objGameMap.World2Screen(m_Info.posWorld.x, m_Info.posWorld.y, m_Info.posScr.x, m_Info.posScr.y);
}

BOOL CRole::ProcessAction(void)
{
	if(m_Info.iActFrameIndex >= m_Info.iActFrameAmount)
	{
		m_bVisible = true;
		return true;
	}
	DWORD timeCurrent	=::TimeGet();
	m_Info.iActFrameIndex	=(timeCurrent-m_Info.timeFirstFrame+m_Info.timeFrameInterval/2)/m_Info.timeFrameInterval;
	if (m_Info.iActFrameIndex > m_Info.iActFrameAmount)
		m_Info.iActFrameIndex	=m_Info.iActFrameAmount;
	m_Info.posWorld.x	=m_Info.posActionBegin.x;
	m_Info.posWorld.y	=m_Info.posActionBegin.y;
	int nDeltaX	=m_Info.posActionEnd.x-m_Info.posActionBegin.x;
	int nDeltaY	=m_Info.posActionEnd.y-m_Info.posActionBegin.y;
	if (nDeltaX != 0 || nDeltaY != 0)
	{
		DWORD timeCost	=__max(1, m_Info.timeFrameInterval*m_Info.iActFrameAmount);
		DWORD timePass	=__min(timeCost, timeCurrent-m_Info.timeFirstFrame);
		DWORD dwKey = m_pIRoleView ? m_pIRoleView->GetVariable(_VAR_ACTION_DATA) : 0;
		CONST CActionCtrlInfo* pInfo = RoleDataQuery()->GetActionCtrlInfo(dwKey);
		if (pInfo && pInfo->nSection > 0)
		{
			int nTimePercent = __min(100, 100 * timePass / timeCost);
			int i = 0;
			for (; i < pInfo->nSection; i++)
			{
				if (nTimePercent < pInfo->nTimePercent[i])
					break;
			}
			if (i < pInfo->nSection)
			{
				int nPreSectionTimePercent = (i <= 0) ? 0 : pInfo->nTimePercent[i - 1];
				int nPreSectionTime = timeCost * nPreSectionTimePercent / 100;
				int nSectionTime = timeCost * pInfo->nTimePercent[i] / 100 - nPreSectionTime;
				int nSectionPassTimePercent = (int)(0.5f + 100.0f * (timePass - nPreSectionTime) / __max(1, nSectionTime));
				if (nSectionPassTimePercent > 100)
					nSectionPassTimePercent = 100;
				int nPreSectionMovePercent = (i <= 0) ? 0 : pInfo->nMovePercent[i - 1];
				int nMovePercent = nPreSectionMovePercent + (pInfo->nMovePercent[i] - nPreSectionMovePercent) * nSectionPassTimePercent / 100;
				DOUBLE dbChangeX = 1.0 * nDeltaX * nMovePercent / 100;
				DOUBLE dbChangeY = 1.0 * nDeltaY * nMovePercent / 100;
				m_Info.posWorld.x = m_Info.posActionBegin.x + ::Double2Int(dbChangeX);
				m_Info.posWorld.y = m_Info.posActionBegin.y + ::Double2Int(dbChangeY);
			}
			else
			{
				m_Info.posWorld = m_Info.posActionEnd;
			}
		}
		else
		{
			DOUBLE dbChangeX	=1.0*nDeltaX*timePass/timeCost;
			DOUBLE dbChangeY	=1.0*nDeltaY*timePass/timeCost;
			m_Info.posWorld.x	=m_Info.posActionBegin.x + ::Double2Int(dbChangeX);
			m_Info.posWorld.y	=m_Info.posActionBegin.y + ::Double2Int(dbChangeY);
		}
	}
	CMyPos posCell = {0, 0};
	g_objGameMap.World2Cell(m_Info.posWorld.x, m_Info.posWorld.y, posCell.x, posCell.y);
	m_Info.posMap = posCell;
	g_objGameMap.World2Screen(m_Info.posWorld.x, m_Info.posWorld.y, m_Info.posScr.x, m_Info.posScr.y);
	if(m_Info.iActFrameIndex >= m_Info.iActFrameAmount)
	{
		int nTerrain = g_objGameMap.GetTerrain(m_Info.posMap.x, m_Info.posMap.y);
		const ActionMapEffectInfo* pInfo = RoleDataQuery()->GetActionMapEffect(m_Info.usLook, m_Info.iActType, nTerrain);
		if (pInfo && pInfo->nShowTime == 1 && this->GetCommandStatus() != _CMDSTATUS_OVER)
		{
			g_objGameMap.Add3DMapEffect(m_Info.posWorld, (char*)pInfo->szEffect, true, pInfo->bDirEnable ? m_Info.iDirection : -1);
		}
		m_bVisible = true;
		return true;
	}
	else
		return false;
}

BOOL CRole::ProcessCommand(void)
{
	if (m_Info.cmdProc.iStatus == _CMDSTATUS_ACCOMPLISH)
	{
		return StandBy();
	}

	BOOL bAccomlished	=true;
	switch(m_Info.cmdProc.iType)
	{
	case _COMMAND_WALK:
		bAccomlished	=Walk();
		break;

	case _COMMAND_RUN:
		bAccomlished	=Run();
		break;

	case _COMMAND_JUMP:
		bAccomlished	=Jump();
		break;

	case _COMMAND_FOLLOW:
		bAccomlished	=Follow();
		break;

	case _COMMAND_EMOTION:
		bAccomlished	=Emotion();
		break;

	case _COMMAND_POSE:
		bAccomlished	=Pose();
		break;
	case _COMMAND_INTONE:
	case _COMMAND_ACTION:
		bAccomlished	=Action();
		break;

	case _COMMAND_ATTACK:
	case _COMMAND_SPATTACK:
		bAccomlished	=Attack();
		break;

	case _COMMAND_LOCKBOOTH:
		bAccomlished	=LockBooth();
		break;

	case _COMMAND_LOCKATK:
		bAccomlished	=LockAtk();
		break;

	case _COMMAND_BUMP:
		bAccomlished	=Bump();
		break;

	case _COMMAND_TEAMFOLLOW:
		bAccomlished	=TeamFollow();
		break;

	case _COMMAND_RUSHATK:
		bAccomlished	=RushAtk();
		break;

	case _COMMAND_DIE:
		bAccomlished	=Die();
		break;

	case _COMMAND_WALKFORWARD:
		bAccomlished	=WalkForward();
		break;

	case _COMMAND_RUNFORWARD:
		bAccomlished	=RunForward();
		break;

	case _COMMAND_PICKUP:
		bAccomlished	=PickUp();
		break;

	case _COMMAND_MINE:
		bAccomlished	=Mine();
		break;
	case _COMMAND_WOUND:
		bAccomlished	=Wound();
		break;
	case _COMMAND_TAKEOFF:
		bAccomlished	=TakeOff();
		break;
	case _COMMAND_LANDING:
		bAccomlished	=Landing();
		break;
	default:
		bAccomlished	=StandBy();
		break;

	}

	return bAccomlished;
}

void CRole::Process(void* pInfo)
{
	this->ProcessAniEffect();

	if (this->ProcessAction())
	{
		this->ProcessCommand();
		this->SetActionData();
	}

	if (this->GetActionType() == _ACTION_WALKL ||
				this->GetActionType() == _ACTION_RUNL ||
					this->GetActionType() == _ACTION_RUNR)
	{
		CMyPos posCurrent = m_Info.posMap;
		CMyPos posPreStep;
		int nDir = this->GetDir();
		if(nDir > 3)
			g_objGameMap.World2Cell(m_Info.posActionEnd.x, m_Info.posActionEnd.y, posPreStep.x, posPreStep.y);
		else
			g_objGameMap.World2Cell(m_Info.posActionBegin.x, m_Info.posActionBegin.y, posPreStep.x, posPreStep.y);

		m_Info.posMap = posPreStep;

		if (g_objGameMap.IsPosVisible(m_Info.posMap, 2))
			g_objGameMap.ApplyObjShow(this);

		m_Info.posMap = posCurrent;
	}
	else
	{
		if (g_objGameMap.IsPosVisible(m_Info.posMap, 2))
			g_objGameMap.ApplyObjShow(this);
	}

	m_objFStr.Process();
}

void CRole::Show()
{
	if ((m_dwStatus[0] & 4) == 4)
		return;
	if (!this->IsVisible())
		return;
	if (!m_pIRoleView)
		return;
	{
		int nBgX, nBgY;
		g_objGameMap.World2Bg(m_Info.posWorld.x, m_Info.posWorld.y, nBgX, nBgY);
		int nOffsetY = 0;
		int nShadowY = 0;
		switch(this->GetActionType())
		{
		case _ACTION_JUMP:
			{
 				if(!this->TestStatus(USERSTATUS_FLY))
				{
					nOffsetY = this->GetJumpHeight();
					nShadowY = this->GetJumpShadowHeight();
				}
			}
			break;
		case _ACTION_WALKL:
		case _ACTION_WALKR:
		case _ACTION_RUNL:
		case _ACTION_RUNR:
			{
			}
			break;
		default:
			break;
		}
		nOffsetY += m_Info.nFlyOffset;
		nShadowY -= m_Info.nFlyOffset;
		int nRotate	= -45*m_Info.iDirection-45;
		if (this->IsJumping())
		{
			int nDeltaX = m_Info.posActionEnd.x - m_Info.posActionBegin.x;
			int nDeltaY = m_Info.posActionEnd.y - m_Info.posActionBegin.y;
			nRotate = (int)((atan2((double)nDeltaY, (double)nDeltaX) + 3.1415926) * -180.0 / 3.1415926 - 90);
		}
		if (this->IsAttacking())
		{
			CPlayer* pTarget = g_objPlayerSet.GetPlayer(m_Info.cmdProc.idTarget, true);
			if (pTarget)
			{
				CMyPos posTarget;
				pTarget->GetWorldPos(posTarget);
				int nDeltaX = posTarget.x - m_Info.posWorld.x;
				int nDeltaY = posTarget.y - m_Info.posWorld.y;
				nRotate = (int)((atan2((double)nDeltaY, (double)nDeltaX) + 3.1415926) * -180.0 / 3.1415926 - 90);
			}
		}
		float fScale = this->GetZoomPercent()*s_fBaseScale/100.0f;
		m_pIRoleView->SetPos(nBgX, nBgY-nShadowY-nOffsetY, nShadowY, nRotate, fScale);
		if (s_nShadowType == SHADOW_SIMPLE &&
				!this->TestStatus(USERSTATUS_GHOST) &&
				this->GetViewType() != _ROLE_VIEW_TERRAIN)
		{
			IAni* pAni = GameDataSetQuery()->GetDataAni("ani/common.ani", "SimpleShadow");
			if (pAni)
			{
				int nShowX = 0, nShowY =0;
				g_objGameMap.World2Screen(m_Info.posWorld.x, m_Info.posWorld.y, nShowX, nShowY);
				CMyBitmap* pBmp = pAni->GetFrame(0);
				if (pBmp)
				{
					int nAlpha = 255;
					if (m_tmDisappear != 0)
					{
						nAlpha = 255 - (int)(::TimeGet() - m_tmDisappear) * 50 / 1000;
						if (nAlpha < 0)
							nAlpha = 0;
					}
					float fShadow = this->GetAddSize() / 2.0f + this->GetZoomPercent() * 1.5f / 100.0f;
					CMySize sizeShadow;
					pBmp->GetSize(sizeShadow.iWidth, sizeShadow.iHeight);
					CMyPos posShadow = {(int)(nShowX - sizeShadow.iWidth * fShadow / 2.0f), (int)(nShowY - sizeShadow.iHeight * fShadow / 4.0f - nShadowY - nOffsetY)};
					g_objGameMap.MapScaleShowPos(posShadow.x, posShadow.y);
					sizeShadow.iWidth = (int)(g_objGameMap.GetScale() * sizeShadow.iWidth * fShadow / _NORMAL_SCALE);
					sizeShadow.iHeight = (int)(g_objGameMap.GetScale() * sizeShadow.iHeight * fShadow / _NORMAL_SCALE);
					pBmp->ShowEx(posShadow.x, posShadow.y, NULL, sizeShadow.iWidth, sizeShadow.iHeight, (int)(nAlpha * 0.7f));
				}
			}
		}
		m_pIRoleView->SetAction(this->GetActionType(), false);
		m_pIRoleView->SetVariable(_VAR_FRAME, m_Info.iActFrameIndex);
		float alpha			= 1.0f;
		float alphaShadow	= 0.35f;
		if (m_tmDisappear != 0)
		{
			DWORD dwTimeGoesBy = ::TimeGet()-m_tmDisappear;
			alpha = 1.0-dwTimeGoesBy*0.2/750;
			if (alpha < 0.0)
				alpha = 0.0;
			alphaShadow = alpha*0.35f;
		}
		if (this->IsMonster())
		{
			const CMonsterInfo* pInfo = RoleDataQuery()->GetMonsterInfo(m_Info.szName);
			if (pInfo)
				m_pIRoleView->SetVariable(_VAR_BLEND, (pInfo->nAdb << 16) + pInfo->nAsb);
		}
		float fColor = 0.1f;
		if (this->TestStatus(USERSTATUS_GHOST))
			m_pIRoleView->SetLightOffset(SHADOW_NONE, 0.0f, 0.0f, 0.0f, 0.8f, fColor, fColor, fColor);
		else
			m_pIRoleView->SetLightOffset(CRole::s_nShadowType, 300, -300, -1000, alphaShadow, fColor, fColor, fColor);
		unsigned char ucA, ucR, ucG, ucB;
		ucA = m_dwARGB >> 24;
		ucR = (m_dwARGB & 0x00ff0000) >> 16;
		ucG = (m_dwARGB & 0x0000ff00) >> 8;
		ucB = m_dwARGB & 0x000000ff;
		m_pIRoleView->SetRGBA(1.0*ucA/255*alpha, 1.0*ucR/255, 1.0*ucG/255, 1.0*ucB/255);
		CMyPos posView;
		g_objGameMap.GetViewPos(posView);
		int nViewportBgX, nViewportBgY;
		g_objGameMap.World2Bg(posView.x, posView.y, nViewportBgX, nViewportBgY);
		m_pIRoleView->Draw(nViewportBgX, nViewportBgY);
		this->ShowAniEffect(m_Info.posScr.x, m_Info.posScr.y-nOffsetY);
		int nDialogDir = this->GetDir();
		if(m_Info.posScr.x < 100)
			nDialogDir = 1;
		if(m_Info.posScr.x > CMyBitmap::GetScreenWidth() - 100)
			nDialogDir = 5;
		m_objSayHello.ShowWords(m_Info.posScr.x, m_Info.posScr.y-nOffsetY, nDialogDir);
		CMyPos posFStr;
		posFStr.x = m_Info.posScr.x;
		posFStr.y = m_Info.posScr.y-nOffsetY-100;
		m_objFStr.Show(posFStr);
		CMyPos posWorld;
		posWorld.x = nBgX;
		posWorld.y = nBgY-nOffsetY;
		m_objEffect->Show(posWorld);
	}
}

int CRole::GetWalkHeight()
{
	CMyPos posBegin, posEnd;
	g_objGameMap.World2Cell(m_Info.posActionBegin.x, m_Info.posActionBegin.y, 
								posBegin.x, posBegin.y);
	g_objGameMap.World2Cell(m_Info.posActionEnd.x, m_Info.posActionEnd.y, 
								posEnd.x, posEnd.y);
	CellInfo* pCellBegin = g_objGameMap.GetCell(posBegin.x, posBegin.y);
	CellInfo* pCellEnd = g_objGameMap.GetCell(posEnd.x, posEnd.y);
	
	int nOffsetY = 0;
	if(pCellBegin && pCellEnd)
	{

		int nHeightDef = g_objGameMap.GetGroundAltitude(pCellBegin) -
						 g_objGameMap.GetBaseAltitude(posBegin) -
						 g_objGameMap.GetGroundAltitude(pCellEnd) +
						 g_objGameMap.GetBaseAltitude(posEnd);
		
		double dbDicAll = sqrt(
			(m_Info.posActionBegin.x- m_Info.posActionEnd.x) * 
			(m_Info.posActionBegin.x - m_Info.posActionEnd.x) +
			(m_Info.posActionBegin.y - m_Info.posActionEnd.y) * 
			(m_Info.posActionBegin.y - m_Info.posActionEnd.y)
			);
		CMyPos posWorld; 
		this->GetWorldPos(posWorld);
		double dbDicNow = sqrt(
			(m_Info.posActionBegin.x- posWorld.x) * 
			(m_Info.posActionBegin.x - posWorld.x) +
			(m_Info.posActionBegin.y - posWorld.y) * 
			(m_Info.posActionBegin.y - posWorld.y)
			);
		double dbOffsetY;
		dbOffsetY = nHeightDef * dbDicNow / dbDicAll; 
		nOffsetY = (int)dbOffsetY;
	}
	int nAltitude = g_objGameMap.GetGroundAltitude(pCellBegin)-g_objGameMap.GetBaseAltitude(posBegin) - nOffsetY;
	return nAltitude;
}

int	CRole::GetJumpHeight()
{
	DEQUE_CELL setCell;
	CMyPos posHighest;

	CMyPos posBegin, posEnd;
	g_objGameMap.World2Cell(m_Info.posActionBegin.x, m_Info.posActionBegin.y, 
								posBegin.x, posBegin.y);
	g_objGameMap.World2Cell(m_Info.posActionEnd.x, m_Info.posActionEnd.y, 
								posEnd.x, posEnd.y);


	g_objGameMap.Sampling(setCell, posHighest, posBegin, posEnd, 32);

	CellInfo* pBeginCell = g_objGameMap.GetCell(posBegin.x, posBegin.y);
	if(!pBeginCell)
		return 0;

	CellInfo* pEndCell = g_objGameMap.GetCell(posEnd.x, posEnd.y);
	if(!pEndCell)
		return 0;

	CellInfo* pHighestCell = g_objGameMap.GetCell(posHighest.x, posHighest.y);
	if(!pHighestCell)
		return 0;

	int nHightestAltitude = g_objGameMap.GetGroundAltitude(pHighestCell) - g_objGameMap.GetBaseAltitude(posHighest);

	int nBeginAltitude = g_objGameMap.GetGroundAltitude(pBeginCell) - g_objGameMap.GetBaseAltitude(posBegin);
	
	int nEndAltitude = g_objGameMap.GetGroundAltitude(pEndCell) - g_objGameMap.GetBaseAltitude(posEnd);
	
	BOOL bHaveHighestPoint = false;
	if(nHightestAltitude > nBeginAltitude)
		bHaveHighestPoint = true;

	CMyPos posHighestWorldPos, posBeginWorldPos, posEndWorldPos;
	posBeginWorldPos = m_Info.posActionBegin;
	posEndWorldPos = m_Info.posActionEnd;
	if(bHaveHighestPoint)
	{
		g_objGameMap.Cell2World(posHighest.x, posHighest.y, posHighestWorldPos.x, posHighestWorldPos.y);
	}

	nHightestAltitude = nHightestAltitude + sqrt((posBeginWorldPos.x - posEndWorldPos.x) * (posBeginWorldPos.x - posEndWorldPos.x) +
			(posBeginWorldPos.y - posEndWorldPos.y) * (posBeginWorldPos.y - posEndWorldPos.y))/3;

	posHighestWorldPos.x = (posBeginWorldPos.x + posEndWorldPos.x)/2;
	posHighestWorldPos.y = (posBeginWorldPos.y + posEndWorldPos.y)/2;

	nHightestAltitude += 20;
	BOOL beUp = false;
	CMyPos posCurrentWorld;
	this->GetWorldPos(posCurrentWorld);

	DWORD dwB2N = (posBeginWorldPos.x - posCurrentWorld.x) * 
					(posBeginWorldPos.x - posCurrentWorld.x) + 
					(posBeginWorldPos.y - posCurrentWorld.y) * 
					(posBeginWorldPos.y - posCurrentWorld.y);

	DWORD dwB2H = (posBeginWorldPos.x - posHighestWorldPos.x) * 
					(posBeginWorldPos.x - posHighestWorldPos.x) + 
					(posBeginWorldPos.y - posHighestWorldPos.y) * 
					(posBeginWorldPos.y - posHighestWorldPos.y);
	if(dwB2H > dwB2N)
		beUp = true;

	int nHeight = 0;
	if(beUp)
	{
		int nHeightDef =  nHightestAltitude - nBeginAltitude;
		double dbDisNow = sqrt((posBeginWorldPos.x - posCurrentWorld.x) * (posBeginWorldPos.x - posCurrentWorld.x) +
 					   (posBeginWorldPos.y - posCurrentWorld.y) * (posBeginWorldPos.y - posCurrentWorld.y));
		double dbDisEnd = sqrt((posBeginWorldPos.x - posHighestWorldPos.x) * (posBeginWorldPos.x - posHighestWorldPos.x) +
 					   (posBeginWorldPos.y - posHighestWorldPos.y) * (posBeginWorldPos.y - posHighestWorldPos.y));
		if(dbDisEnd == 0)
			dbDisEnd = 1;
		double pi = 3.1415926535;
		double dbAngle = pi*dbDisNow/dbDisEnd/2;
		nHeight = nBeginAltitude + nHeightDef*sin(dbAngle);

	}
	else
	{
		int nHeightDef =  nHightestAltitude - nEndAltitude;
		double dbDisNow = sqrt((posEndWorldPos.x - posCurrentWorld.x) * (posEndWorldPos.x - posCurrentWorld.x) +
 					   (posEndWorldPos.y - posCurrentWorld.y) * (posEndWorldPos.y - posCurrentWorld.y));
		double dbDisEnd = sqrt((posEndWorldPos.x - posHighestWorldPos.x) * (posEndWorldPos.x - posHighestWorldPos.x) +
 					   (posEndWorldPos.y - posHighestWorldPos.y) * (posEndWorldPos.y - posHighestWorldPos.y));
		if(dbDisEnd == 0)
			dbDisEnd = 1;

		double pi = 3.1415926535;
		double dbAngle = pi*dbDisNow/dbDisEnd/2;
		nHeight = nEndAltitude + nHeightDef*sin(dbAngle);
	}
			
	return nHeight;	
}

int CRole::GetJumpShadowHeight()
{
	int nHeight = this->GetJumpHeight();
	CMyPos posCell;
	g_objGameMap.World2Cell(m_Info.posWorld.x, m_Info.posWorld.y,
							posCell.x, posCell.y);
	CellInfo* pCellInfo = g_objGameMap.GetCell(posCell.x, posCell.y);
	if(!pCellInfo)
		return 0;	
	int nJumpShadowHeight = g_objGameMap.GetGroundAltitude(pCellInfo) - g_objGameMap.GetBaseAltitude(posCell) - nHeight;
	if(nJumpShadowHeight > 0)
		nJumpShadowHeight = 0;
	return nJumpShadowHeight;
}
DWORD CRole::GetJumpFrameInterval()
{
	double dbDicAll = sqrt(
		(m_Info.posActionBegin.x- m_Info.posActionEnd.x) * 
		(m_Info.posActionBegin.x - m_Info.posActionEnd.x) +
		(m_Info.posActionBegin.y - m_Info.posActionEnd.y) * 
		(m_Info.posActionBegin.y - m_Info.posActionEnd.y)
		);
	DWORD dwInteraval = this->GetFrameInterval(_ACTION_JUMP);
	DWORD dwMyInteraval = (DWORD) (dwInteraval*dbDicAll/300+1);
	if(dwMyInteraval < 36)
			dwMyInteraval = 36;
	return dwMyInteraval;
}
int	CRole::GetOffsetY()
{
	int nOffsetY = 0;
	switch(this->GetActionType())
	{
	case _ACTION_JUMP:
		{
			if (!this->TestStatus(USERSTATUS_FLY))
				nOffsetY = this->GetJumpHeight();
		}
		break;
	case _ACTION_WALKL:
	case _ACTION_RUNL:
	case _ACTION_RUNR:
		{
			nOffsetY = this->GetWalkHeight();
		}
		break;
	default:
		nOffsetY = g_objGameMap.GetAltitude(m_Info.posMap)- g_objGameMap.GetBaseAltitude(m_Info.posMap);
		break;
	}
	if (this->TestStatus(USERSTATUS_FLY))
		nOffsetY += m_Info.nFlyOffset;
	return nOffsetY;
}
void CRole::AddFStr(char* pszStr, int nMaxHeight, DWORD dwColor, int nStepHeight)
{
	m_objFStr.AddStr(pszStr, nMaxHeight, dwColor, nStepHeight);
}
void CRole::Transform(int nIndex)
{
	if(nIndex == 0)
	{
		if(m_Info.nTransformLook == 0)
			return;
		if(m_pIRoleView)
		{
			m_pIRoleView->SetVariable(_VAR_LOOK, m_Info.usLook);
			m_pIRoleView->SetVariable(_VAR_HAIR, m_Info.usHair);
			m_pIRoleView->SetVariable(_VAR_ARMOR, m_Info.idArmorType);
			m_pIRoleView->SetVariable(_VAR_HEAD, m_Info.usHead);
			m_pIRoleView->SetVariable(_VAR_ARMET, m_Info.idArmetType);
			m_pIRoleView->SetVariable(_VAR_RWEAPON, m_Info.idRWeaponType);
			m_pIRoleView->SetVariable(_VAR_LWEAPON, m_Info.idLWeaponType);
			m_pIRoleView->SetVariable(_VAR_MISC, m_Info.idMiscType);
			m_pIRoleView->SetVariable(_VAR_MOUNT, m_Info.idMountType);
			if (m_Info.idMantleType != 0)
				m_pIRoleView->SetVariable(_VAR_ARMOR, m_Info.idMantleType);
		}
		m_Info.nTransformLook = 0;
		this->SetAddSize(0);
		this->SetZoomPercent(100);
		return;
	}
	CTransformInfo* pInfo = RoleDataQuery()->GetTransformInfo(nIndex);
	if(!pInfo)
		return;
	this->SetAddSize(pInfo->nAddSize);
	this->SetZoomPercent(pInfo->nScale);
	m_Info.nTransformLook = nIndex;
	if(m_pIRoleView)
	{
		m_pIRoleView->SetVariable(_VAR_TRANSFORM, pInfo->nLook);
		m_pIRoleView->SetVariable(_VAR_ARMET, pInfo->idArmet);
		m_pIRoleView->SetVariable(_VAR_RWEAPON, pInfo->idRWeapon);
		m_pIRoleView->SetVariable(_VAR_LWEAPON, pInfo->idLWeapon);
		m_pIRoleView->SetVariable(_VAR_MISC, pInfo->idMisc);
		m_pIRoleView->SetVariable(_VAR_MOUNT, pInfo->idMount);
	}
}
void CRole::ShootArrow(OBJID idTarget, int nHpLose)
{
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(idTarget, true);
	if(!pPlayer)
		return;
	CMyPos posTarget;
	pPlayer->GetPos(posTarget);

	CCommand cmd;
	memset(&cmd, 0, sizeof(CCommand));
	cmd.iType		=_COMMAND_ATTACK;
	cmd.iStatus		=_CMDSTATUS_BEGIN;
	cmd.idTarget	=idTarget;
	cmd.dwData		=nHpLose;
	cmd.nDir	=	this->GetDir(posTarget);
	this->SetCommand(&cmd);
	CMagicEffect* pMagicEffect = CMagicEffect::CreateNewArrow(this->GetID(), idTarget, 500000);
	g_objGameMap.AddMagicEffect(pMagicEffect);
}
void CRole::SetDir(int nDir)
{
	m_Info.iDirection=nDir%8;
	if(m_Info.cmdProc.iType == _COMMAND_EMOTION)
		m_Info.cmdProc.nDir = m_Info.iDirection;
}
void CRole::SetFlyOffset(int nOffset)
{
	m_Info.nFlyOffset = nOffset;
}
int	CRole::GetFlyOffset()
{
	return m_Info.nFlyOffset;
}
void CRole::SetFlyOffsetGoal(int nOffset)
{
	m_Info.nFlyOffsetGoal = nOffset;
}
int	CRole::GetFlyOffsetGoal()
{
	return m_Info.nFlyOffsetGoal;
}
BOOL CRole::HaveBow()
{
	if(m_Info.nTransformLook != 0)
		return false;
	if(this->GetRWeapon()/1000%1000 == 500)
		return true;
	else
		return false;
}
BOOL	CRole::IsGuard(void)
{
	if (this->IsNpc() && strstr(this->GetName(), GameDataSetQuery()->GetStr(100028)))
		return true;
	return false;
}
BOOL	CRole::IsMyPet(void)
{
	if(!this->IsPet())
		return false;
	if(g_objHero.CheckPet(this->GetID()))
		return true;
	return false;
}
