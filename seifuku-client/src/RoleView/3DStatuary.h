#if !defined(AFX_3DSTATUARY_H__87F25D6D_6623_4AF9_883A_DE2919519304__INCLUDED_)
#define AFX_3DSTATUARY_H__87F25D6D_6623_4AF9_883A_DE2919519304__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "3DRole.h"

class C3DStatuary  : public C3DRole
{
public:
	C3DStatuary();
	virtual ~C3DStatuary();
public:
	int		m_nActionType;
	float	m_fScale;
	int		m_nFrame;
	int		m_nDir;
public:
	virtual BOOL	SetVariable(int nVariableType, DWORD dwData);
	virtual int		SetAction(int nActionType, BOOL bResetMotion= true, BOOL bPlayerSound = false, int nWorldX=0, int nWorldY=0, BOOL bSetEffect = false);
	virtual BOOL	Draw ( int nMapX, int nMapY );
};

#endif
