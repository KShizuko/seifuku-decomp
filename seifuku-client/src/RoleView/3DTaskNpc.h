#if !defined(AFX_3DTASKNPC_H__383DB301_CBCA_4B9F_892E_FA78F4E7CF85__INCLUDED_)
#define AFX_3DTASKNPC_H__383DB301_CBCA_4B9F_892E_FA78F4E7CF85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "IRoleView.h"
#include "3DSimpleObj.h"

class C3DTaskNpc   : public IRoleView
{
	friend struct TaskNpcProbe;
public:
	C3DTaskNpc();
	virtual ~C3DTaskNpc();
protected:
	int m_nLook;
	int m_nDir;
	OBJID m_idStandByMotion;
	OBJID m_idRestMotion;
	OBJID m_idBlazeMotion;
	OBJID m_idSimpleObj;
	OBJID m_idCurrentMotion;
	char  m_szName[16];
	DWORD m_dwTimer;
	DWORD m_dwBlazeTime;
	int   m_nAsb;
	int   m_nAdb;
	BOOL  m_bFixDir;
	BOOL  m_bHit;
	I3DSimpleObj* m_p3DSimpleObj;
protected:
	void SetLook(int nLook);
	I3DSimpleObj* GetSimpleObj(void);
public:
	virtual BOOL	SetVariable(int nVariableType, DWORD dwData);
	virtual int		SetAction(int nActionType, BOOL bResetMotion= true, BOOL bPlayerSound = false, int nWorldX=0, int nWorldY=0, BOOL bSetEffect = false);
	virtual void	SetPos ( float fX, float fY, float fHeight, int nRotate, float fScale );
	virtual void	SetLightOffset( int type, float x, float y, float z, float a, float r, float g, float b );
	virtual void	SetRGBA ( float alpha, float red, float green, float blue );
	virtual void	AddPartEffect ( int nPart, const char* pszEffect ) {}
	virtual void	ClearPartEffect ( int nPart, const char* pszEffect ) {}
	virtual BOOL	HitTest ( int nHitScreenX, int nHitScreenY, int nMapX, int nMapY );
	virtual BOOL	Draw ( int nMapX, int nMapY );
	virtual DWORD	GetVariable(int nVariableType);
	virtual BOOL	Reload ( void );
};

#endif
