#include "3DTaskNpc.h"
#include "3DRoleData.h"
#include "GameDataSet.h"

const OBJID ID_NONE = 0;

C3DTaskNpc::C3DTaskNpc()
{
	m_nLook	=	0;
	m_nDir	=	-1;
	m_idStandByMotion	= ID_NONE;
	m_idRestMotion		= ID_NONE;
	m_idBlazeMotion		= ID_NONE;
	m_dwTimer = ::TimeGet();
	m_dwBlazeTime = 0;
	m_bFixDir = false;
	m_nAsb = 5;
	m_nAdb = 6;
	m_bHit = false;
	m_p3DSimpleObj = NULL;
}

C3DTaskNpc::~C3DTaskNpc()
{
	if (m_p3DSimpleObj)
	{
		m_p3DSimpleObj->Release();
		m_p3DSimpleObj = NULL;
	}
}
I3DSimpleObj* C3DTaskNpc::GetSimpleObj(void)
{
	if (!m_p3DSimpleObj)
		m_p3DSimpleObj = ::Simple3DObjCreate(0);
	return m_p3DSimpleObj;
}
BOOL C3DTaskNpc::SetVariable(int nVariableType, DWORD dwData)
{
	switch(nVariableType)
	{
	case _VAR_LOOK:
		this->SetLook((int)dwData);
		return true;
	case _VAR_ROTATEX:
		this->GetSimpleObj()->SetObliqueAngle((int)dwData);
		return true;
	case _VAR_DIR:
	case _VAR_DIRECT_DIR:
		if(m_nDir == -1 || !m_bFixDir)
		{
			m_nDir = dwData;
		}
		return true;
	}
	return false;
}
int C3DTaskNpc::SetAction(int nActionType, BOOL bResetMotion, BOOL bPlayerSound, int nWorldX, int nWorldY, BOOL bSetEffect)
{
	return 10;
}
void C3DTaskNpc::SetPos ( float fX, float fY, float fHeight, int nRotate, float fScale )
{
	int nMyRotate	= -45*m_nDir-45;
	this->GetSimpleObj()->SetPos((int)fX, (int)fY, (int)fHeight, nMyRotate, fScale);
}
void C3DTaskNpc::SetLightOffset( int type, float x, float y, float z, float a, float r, float g, float b )
{
	if (type < 1)
		type = 1;
	this->GetSimpleObj()->SetLightOffset(type, x, y, z, a, r, g, b);
}
BOOL C3DTaskNpc::Reload ( void )
{
	return this->GetSimpleObj()->Reload();
}
void C3DTaskNpc::SetRGBA ( float alpha, float red, float green, float blue )
{
	this->GetSimpleObj()->SetARGB(alpha, red, green, blue);
}
BOOL C3DTaskNpc::HitTest (int nHitScreenX, int nHitScreenY, int nMapX, int nMapY )
{
	m_bHit = this->GetSimpleObj()->HitTest(nHitScreenX, nHitScreenY, nMapX, nMapY);
	return m_bHit;
}
BOOL C3DTaskNpc::Draw ( int nMapX, int nMapY )
{
	const DWORD dwFrameInterval = 33;
	DWORD dwNow = ::TimeGet();
	if(m_bHit && dwNow >= m_dwBlazeTime + 20000 && m_idCurrentMotion != m_idBlazeMotion)
	{
		m_idCurrentMotion = m_idBlazeMotion;
		m_dwBlazeTime = dwNow;
		m_dwTimer = dwNow;
	}
	I3DMotion* pMotion = GameDataSetQuery()->Get3DMotion("body", m_idCurrentMotion);
	IF_NOT (pMotion)
		return false;
	int nFrameAmount = __max(1, pMotion->GetFrameAmount());
	int nFrameIndex = 0;
	if(dwNow >= m_dwTimer + nFrameAmount * dwFrameInterval)
	{
		m_dwTimer = dwNow;
		nFrameIndex = 0;
		if(m_idCurrentMotion == m_idBlazeMotion)
			m_idCurrentMotion = m_idStandByMotion;
		else if(m_idCurrentMotion == m_idStandByMotion)
		{
			if(::RandGet(30) == 15)
				m_idCurrentMotion = m_idRestMotion;
		}
		else
			m_idCurrentMotion = m_idStandByMotion;
	}
	else
		nFrameIndex = (dwNow - m_dwTimer)/dwFrameInterval;
	this->GetSimpleObj()->SetMotion(GameDataSetQuery()->Get3DMotion("body", m_idCurrentMotion));
	this->GetSimpleObj()->SetFrame(nFrameIndex);
	m_bHit = false;
	return this->GetSimpleObj()->Draw2BG(nMapX, nMapY, m_nAsb, m_nAdb);
}
DWORD C3DTaskNpc::GetVariable(int nVariableType)
{
	switch(nVariableType)
	{
	case _VAR_LOOK:
		return (m_nLook/10)*10 + m_nDir%8;
	case _VAR_DIR:
		return m_nDir;
	default:
		return false;
	}
}
void C3DTaskNpc::SetLook(int nLook)
{
	m_nLook = nLook;
	NpcTypeInfo* pInfo = RoleDataQuery()->GetNpcTypeInfo(nLook/10);
	if (!pInfo)
		return;
	if (!this->GetSimpleObj()->Create(pInfo->idSimpleObj))
		return;
	m_idStandByMotion	=	pInfo->idMotionStandBy;
	m_idRestMotion		=	pInfo->idMotionRest;
	m_idBlazeMotion		=	pInfo->idMotionBlaze;
	m_idCurrentMotion = m_idStandByMotion;
	m_dwTimer = 0;
	m_idSimpleObj = pInfo->idSimpleObj;
	m_nDir = nLook%10;
	strcpy(m_szName, pInfo->szNpcName);
	m_nAsb = pInfo->nAsb;
	m_nAdb = pInfo->nAdb;
	m_bFixDir = pInfo->bFixDir;
	if(strcmp(pInfo->szEffectIndex, "none") != 0)
		this->GetSimpleObj()->AddEffect(pInfo->szEffectIndex);
}
