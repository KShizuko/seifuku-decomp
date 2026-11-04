#if !defined(AFX_3DSIMPLEROLE_H__A5F50D6D_99F0_418C_8C83_B00F713AC6B8__INCLUDED_)
#define AFX_3DSIMPLEROLE_H__A5F50D6D_99F0_418C_8C83_B00F713AC6B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "3DSimpleObj.h"
#include "Game3DEffectEx.h"
#include "NDsound.h"
#include "IRoleView.h"

const int _SIMPLEROLE_ROLE_FIRST = 8;
const int _SIMPLEROLE_ROLE_LAST = 11;

class C3DSimpleRole
{
	friend struct SimpleRoleProbe;
public:
	C3DSimpleRole();
	virtual ~C3DSimpleRole();

private:
	int		m_nIndex;
	int		m_nLook;
	DWORD	m_dwBegin;
	OBJID	m_idSimpleObj;
	OBJID	m_idMotionStandBy;
	OBJID	m_idMotionBlaze;
	BOOL	m_bBlaze;
	BOOL	m_bPose;
	CSound* m_pSound;
	I3DSimpleObj*	m_obj3DSimpleObj;
	IGame3DEffect*	m_objEffectF;
	IGame3DEffect*	m_objEffectB;
	IRoleView*		m_obj3DRole;
	DWORD			m_dwFrameAmount;
	BOOL			m_bResetPose;
public:
	void ShowRole(int nLook, CMyPos posScr, int nZoomPercent= 100, int nRotateX = 0, int  bFocus=0);
	void Show(int nIndex, CMyPos posScr, int nZoomPercent= 100, int nRotateX = 0, int nStatus=0);
	void Blaze();
	void Reset(){m_nIndex = -1;	}
};

#endif
