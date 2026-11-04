#if !defined(AFX_NPCXVIEW_H__ROLEVIEW_4343_INCLUDED_)
#define AFX_NPCXVIEW_H__ROLEVIEW_4343_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "3DRole.h"
#include "3DRoleData.h"

class CNpcXView  : public C3DRole
{
	friend struct NpcXViewProbe;
public:
	CNpcXView();
	virtual ~CNpcXView();
protected:
	int				m_nDir;
	NpcXTypeInfo*	m_pInfo;
public:
	virtual BOOL	SetVariable(int nVariableType, DWORD dwData);
	virtual int		SetAction(int nActionType, BOOL bResetMotion= true, BOOL bPlayerSound = false, int nWorldX=0, int nWorldY=0, BOOL bSetEffect = false);
	virtual void	SetPos ( float fX, float fY, float fHeight, int nRotate, float fScale );
	virtual void	SetLightOffset( int type, float x, float y, float z, float a, float r, float g, float b );
	virtual void	SetRGBA ( float alpha, float red, float green, float blue );
	virtual BOOL	HitTest ( int nHitScreenX, int nHitScreenY, int nMapX, int nMapY );
	virtual BOOL	Draw ( int nMapX, int nMapY );
	virtual DWORD	GetVariable(int nVariableType);
};

#endif
