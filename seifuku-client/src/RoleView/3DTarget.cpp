#include "3DTarget.h"
#include "3DRoleData.h"
#include "GameDataSet.h"

const OBJID ID_NONE = 0;
static const int _ACTION_BRUISE0 = 320;

C3DTarget::C3DTarget()
{
	m_nLook	=	0;
	m_nDir	=	-1;
	m_idStandByMotion	=	ID_NONE;
	m_idWoundMotion		=	ID_NONE;
	m_p3DSimpleObj = NULL;
	m_bHit = false;
	m_dwTimer = ::TimeGet();
	m_nFrameAmount = 1;
}

C3DTarget::~C3DTarget()
{
	if (m_p3DSimpleObj)
	{
		m_p3DSimpleObj->Release();
		m_p3DSimpleObj = NULL;
	}
}
I3DSimpleObj* C3DTarget::GetSimpleObj(void)
{
	if (!m_p3DSimpleObj)
		m_p3DSimpleObj = ::Simple3DObjCreate(0);
	return m_p3DSimpleObj;
}
BOOL C3DTarget::SetVariable(int nVariableType, DWORD dwData)
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
		if(m_nDir == -1)
		{
			m_nDir = dwData;
		}
		return true;
	}
	return false;
}
int C3DTarget::SetAction(int nActionType, BOOL bResetMotion, BOOL bPlayerSound, int nWorldX, int nWorldY, BOOL bSetEffect)
{
	if(nActionType == _ACTION_BRUISE0)
		m_bHit = true;
	return 10;
}
void C3DTarget::SetPos ( float fX, float fY, float fHeight, int nRotate, float fScale )
{
	int nMyRotate	= -45*m_nDir-45;
	this->GetSimpleObj()->SetPos((int)fX, (int)fY, (int)fHeight, nMyRotate, fScale);
}
void C3DTarget::SetLightOffset( int type, float x, float y, float z, float a, float r, float g, float b )
{
	if (type < 1)
		type = 1;
	this->GetSimpleObj()->SetLightOffset(type, x, y, z, a, r, g, b);
}
BOOL C3DTarget::Reload ( void )
{
	return this->GetSimpleObj()->Reload();
}
void C3DTarget::SetRGBA ( float alpha, float red, float green, float blue )
{
	this->GetSimpleObj()->SetARGB(alpha, red, green, blue);
}
BOOL C3DTarget::HitTest (int nHitScreenX, int nHitScreenY, int nMapX, int nMapY )
{
	return this->GetSimpleObj()->HitTest(nHitScreenX, nHitScreenY, nMapX, nMapY);
}
BOOL C3DTarget::Draw ( int nMapX, int nMapY )
{
	const DWORD dwFrameInterval = 33;
	int nFrameIndex = (::TimeGet() - m_dwTimer)/dwFrameInterval;
	if((nFrameIndex >= m_nFrameAmount) && m_bHit)
	{
		m_dwTimer = ::TimeGet();
		nFrameIndex = 0;
		OBJID idMotion = m_idStandByMotion;
		if(m_bHit)
		{
			idMotion = m_idWoundMotion;
			m_bHit = false;
		}
		I3DMotion* pMotion = GameDataSetQuery()->Get3DMotion("body", idMotion);
		if(pMotion)
		{
			m_nFrameAmount = pMotion->GetFrameAmount();
			this->GetSimpleObj()->SetMotion(pMotion);
		}
		m_idCurrentMotion = idMotion;
	}
	this->GetSimpleObj()->SetMotion(GameDataSetQuery()->Get3DMotion("body", m_idCurrentMotion));
	this->GetSimpleObj()->SetFrame(nFrameIndex);
	return this->GetSimpleObj()->Draw2BG(nMapX, nMapY, 5, 6);
}
DWORD C3DTarget::GetVariable(int nVariableType)
{
	return 0;
}
void C3DTarget::SetLook(int nLook)
{
	m_nLook = nLook;
	NpcTypeInfo* pInfo = RoleDataQuery()->GetNpcTypeInfo(nLook/10);
	MYASSERT(pInfo);
	m_idStandByMotion = pInfo->idMotionStandBy;
	m_idCurrentMotion = pInfo->idMotionStandBy;
	m_idWoundMotion = pInfo->idMotionBlaze;
	m_idSimpleObj = pInfo->idSimpleObj;
	strcpy(m_szName, pInfo->szNpcName);
	this->GetSimpleObj()->Create(pInfo->idSimpleObj);
	m_nDir = nLook%10;
}
