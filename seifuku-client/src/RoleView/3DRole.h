#ifndef _3DROLE_H
#define _3DROLE_H
#include "ND_Bitmap.h"
#include "3DRoleEx.h"
#include "IRoleView.h"

class C3DRole : public IRoleView
{
public:
	C3DRole();
	virtual ~C3DRole ();

	void SetFrame (DWORD dwFrame);

public:
	float		m_fPosX, m_fPosY, m_fPosH;
	int			m_nRotateX, m_nRotateY, m_nRotateZ;
	float		m_fScale;
	int			m_nActionType;
	OBJID		m_idArmor;
	OBJID		m_idArmet;
	OBJID		m_idLWeapon;
	OBJID		m_idRWeapon;
	OBJID		m_idMount;
	OBJID		m_idMisc;
	OBJID		m_idArmorType;
	OBJID		m_idArmetType;
	D3DXVECTOR3		m_LightOffset;
	int				m_nShadowType;
	int				m_nLook;
	OBJID			m_idHead;
	int				m_nHair;
	float			m_a;
	float			m_r;
	float			m_g;
	float			m_b;
	int				m_nAsb;
	int				m_nAdb;
	I3DRole*		m_pRole;
	D3DXMATRIX		m_matrix;
	DWORD			m_dwTransStart;
	DWORD			m_dwTransTime;
	DWORD			m_dwFrame;
	DWORD			m_dwBlendFrame;

public:
	I3DMotion*	GetWeaponMotion	(OBJID idWeapon, int nAction);
	I3DMotion*	m_pMotionArmor;
	I3DMotion*	m_pMotionArmet;
	I3DMotion*	m_pMotionWeaponR;
	I3DMotion*  m_pMotionWeaponL;
	I3DMotion*	m_pMotionMount;
	I3DMotion*	m_pMotionMisc;
	I3DMotion*	m_pMotionBlend;
	void		ResetMotion	(void);

public:
	DWORD	GetWeaponActionData (void);
	DWORD	GetMountActionData	(void);
	DWORD	GetMiscActionData	(void);
	I3DMotion*	GetMountMotion	(OBJID idMount, int nAction);
	I3DMotion*	GetArmetMotion	(OBJID idArmet, DWORD dwWeaponData, int nAction);
	I3DMotion*	GetMiscMotion	(OBJID idMisc, DWORD dwWeaponData, int nAction);
	BOOL SetLWeapon	(OBJID idType);
	BOOL SetRWeapon	(OBJID idType);
	void SetArmor	(OBJID idType);
	void SetBody	(OBJID idType);
	void SetMount	(OBJID idType);
	void SetMisc	(OBJID idType);
	void SetArmet	(OBJID idType);
	void SetArmetPart (OBJID idArmetType);
	void SetHead	(OBJID idHead);
	void SetLook	(int nLook);
	void SetHair	(int nHair);
	void Transform	(int nLook);
	void SetRotateX (int nRotateX){m_nRotateX = nRotateX;}
	DWORD GetActionData(void);
	DWORD GetActionData(int nActionType);
	void SetPartEffect(const char* pszKind, OBJID idPart, int nActionType);

public:
	virtual BOOL	SetVariable(int nVariableType, DWORD dwData);
	virtual int		SetAction(int nActionType, BOOL bResetMotion= true, BOOL bPlayerSound = false, int nWorldX=0, int nWorldY=0, BOOL bSetEffect = false);
	virtual void	SetPos ( float fX, float fY, float fHeight, int nRotate, float fScale );
	virtual void	SetLightOffset( int type, float x, float y, float z, float a, float r, float g, float b );
	virtual void	SetRGBA ( float alpha, float red, float green, float blue );
	virtual void	AddPartEffect ( int nPart, const char* pszEffect );
	virtual void	ClearPartEffect ( int nPart, const char* pszEffect );
	virtual BOOL	HitTest ( int nHitScreenX, int nHitScreenY, int nMapX, int nMapY );
	virtual BOOL	Draw ( int nMapX, int nMapY );
	virtual DWORD	GetVariable(int nVariableType);
	virtual BOOL	Reload ( void );
};

#ifdef _ANALYSIS_ON
extern DWORD g_dw3DRoleDrawTime;
extern DWORD g_dw3DRoleDrawAmount;
extern DWORD g_dw3DRoleDrawTimeMax;
extern DWORD g_dw3DRoleDrawTimeFrame;
extern DWORD g_dw3DRoleDrawAmountFrame;
#endif
#endif
