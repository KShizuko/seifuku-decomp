#include "NpcXView.h"

CNpcXView::CNpcXView()
{
	m_nDir = -1;
	m_pInfo = NULL;
}

CNpcXView::~CNpcXView()
{
}
BOOL	CNpcXView::SetVariable(int nVariableType, DWORD dwData)
{
	if (!m_pInfo && nVariableType != _VAR_LOOK)
		return false;
	switch(nVariableType)
	{
	case _VAR_LOOK:
		m_pInfo = RoleDataQuery()->GetNpcXTypeInfo(dwData/10);
		if (!m_pInfo)
			return true;
		this->SetLook(m_pInfo->nLook);
		this->SetHair(m_pInfo->nHair);
		this->SetArmet(m_pInfo->idArmet);
		this->SetArmor(m_pInfo->idArmor);
		this->SetHead(m_pInfo->nHead);
		this->SetRWeapon(m_pInfo->idRWeapon);
		this->SetLWeapon(m_pInfo->idLWeapon);
		this->SetMisc(m_pInfo->idMisc);
		this->SetMount(m_pInfo->idMount);
		return true;
	case _VAR_DIR:
	case _VAR_DIRECT_DIR:
		if (m_nDir != -1 && m_pInfo->bFixDir)
			return true;
		m_nDir = dwData;
		m_nRotateZ = -45*m_nDir-45;
		return true;
	}
	return false;
}
int		CNpcXView::SetAction(int nActionType, BOOL bResetMotion, BOOL bPlayerSound, int nWorldX, int nWorldY, BOOL bSetEffect)
{
	return C3DRole::SetAction(100, bResetMotion, bPlayerSound, nWorldX, nWorldY, bSetEffect);
}
void	CNpcXView::SetPos ( float fX, float fY, float fHeight, int nRotate, float fScale )
{
	if (m_pInfo && m_pInfo->bFixDir)
		nRotate = -45*m_nDir-45;
	C3DRole::SetPos(fX, fY, fHeight, nRotate, fScale);
}
void	CNpcXView::SetLightOffset( int type, float x, float y, float z, float a, float r, float g, float b )
{
	C3DRole::SetLightOffset(type, x, y, z, a, r, g, b);
}
void	CNpcXView::SetRGBA ( float alpha, float red, float green, float blue )
{
	C3DRole::SetRGBA(alpha, red, green, blue);
}
BOOL	CNpcXView::HitTest ( int nHitScreenX, int nHitScreenY, int nMapX, int nMapY )
{
	return C3DRole::HitTest(nHitScreenX, nHitScreenY, nMapX, nMapY);
}
BOOL	CNpcXView::Draw ( int nMapX, int nMapY )
{
	return C3DRole::Draw(nMapX, nMapY);
}
DWORD	CNpcXView::GetVariable(int nVariableType)
{
	return C3DRole::GetVariable(nVariableType);
}
