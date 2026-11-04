#if !defined(AFX_DICEVIEW_H__3F5D2E71_9303_4347_96BB_3A751B401882__INCLUDED_)
#define AFX_DICEVIEW_H__3F5D2E71_9303_4347_96BB_3A751B401882__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "3DTaskNpc.h"

struct CSimpleObjPtr
{
	I3DSimpleObj* p;
	CSimpleObjPtr() { p = NULL; }
	~CSimpleObjPtr() { if (p) { p->Release(); p = NULL; } }
	I3DSimpleObj* Get(void) { if (!p) p = ::Simple3DObjCreate(0); return p; }
};

class CDiceView  : public C3DTaskNpc
{
	friend struct DiceViewProbe;
public:
	CDiceView();
	virtual ~CDiceView();
protected:
	CSimpleObjPtr m_objDesk;
	CSimpleObjPtr m_objDiceRoll[3];
	CSimpleObjPtr m_objDiceStatic[3];
	BOOL		 m_bRun;
	BOOL		 m_bDiceRun[3];
	DWORD		 m_dwDice[3];
	DWORD		 m_dwDiceEnd[3];
	DWORD		 m_dwDiceFrame[3];
	DWORD		 m_dwOffset[3];
	BOOL		 m_bUp[3];
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
