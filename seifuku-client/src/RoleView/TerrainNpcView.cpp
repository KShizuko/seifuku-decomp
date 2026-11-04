#include "TerrainNpcView.h"

const OBJID ID_NONE = 0;

CTerrainNpcView::CTerrainNpcView()
{
	m_idNpc = ID_NONE;
}

CTerrainNpcView::~CTerrainNpcView()
{
}

void	CTerrainNpcView::SetPos ( float fX, float fY, float fHeight, int nRotate, float fScale )
{
}
void	CTerrainNpcView::SetLightOffset( int type, float x, float y, float z, float a, float r, float g, float b )
{
	return;
}
void	CTerrainNpcView::SetRGBA ( float alpha, float red, float green, float blue )
{
	return;
}
BOOL	CTerrainNpcView::HitTest ( int nHitScreenX, int nHitScreenY, int nMapX, int nMapY )
{
	return false;
}
BOOL	CTerrainNpcView::Draw ( int nMapX, int nMapY )
{
	return true;
}
BOOL	CTerrainNpcView::Reload ( void )
{
	return false;
}
DWORD	CTerrainNpcView::GetVariable(int nVariableType)
{
	return 0;
}
int		CTerrainNpcView::SetAction(int nActionType, BOOL bResetMotion, BOOL bPlayerSound, int nWorldX, int nWorldY, BOOL bSetEffect)
{
	return 100;
}
BOOL	CTerrainNpcView::SetVariable(int nVariableType, DWORD dwData)
{
	if(nVariableType != _VAR_LOOK)
		return false;
	return true;
}
