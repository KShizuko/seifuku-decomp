#if !defined(AFX_3DROLEPART_H__709EC8A9_1A3B_4C9A_898F_380DC5124782__INCLUDED_)
#define AFX_3DROLEPART_H__709EC8A9_1A3B_4C9A_898F_380DC5124782__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
#include "ND_Bitmap.h"
#include "GameDataSet.h"
#include "Game3DEffectEx.h"

class C3DRolePart
{
public:
	C3DRolePart();
	virtual ~C3DRolePart();

public:
	BOOL		Create			(const char* pszKind, OBJID id, const D3DXMATRIXA16* pMatrix = NULL);
	void		SetMatrix		(const D3DXMATRIXA16* pMatrix);
	BOOL		Reload			(const char* pszKind);
	BOOL		IsLoaded		(void);
	BOOL		AddPart			(const char* pszKind, OBJID id);
	BOOL		ReplacePart		(const char* pszKind, OBJID id);
	void		Move			(float x, float y, float z);
	void		Rotate			(float x, float y, float z);
	void		Scale			(float x, float y, float z);
	void		SetARGB			(float alpha, float red, float green, float blue);
	void		SetFrame		(DWORD dwFrame);
	void		SetBlendFrame	(DWORD dwFrame);
	void		Draw			(int type, float lightx, float lighty, float lightz, float sa, float sr, float sg, float sb, int nAsb, int nAdb);
	void		Clear3DEffect	(const char* szIndex = NULL);
	void		AddEffect		(const char* pszIndex, BOOL bForce);
	BOOL		TestEffect		(const char* pszIndex);
	void		SetMotion		(I3DMotion* pMotion);
	void		SetBlendMotion	(I3DMotion* pMotion, float fWeight);
	void		SetTexture		(I3DTexture* pTexture = NULL, I3DTexture* pMixTexture = NULL);
	void		SetDrawType		(int nType);

	void		SetDefaultMotion	(void);
	CGame3DEffectEx2* QueryGame3DEffectEx(){return &this->m_objGame3DEffectEx;}
	RolePartInfo*	GetInfo		(void){return &this->m_infoPart;}

	void		MuliplyPhy		(D3DXMATRIXA16* pobjMatrix);
	void		SetPhyMatrix	(D3DXMATRIXA16* pobjLocal, D3DXMATRIXA16* pobjWorld);
	void		ClearMatrix		(void);
	void		MuliplyAll		(D3DXMATRIXA16* pobjMatrix);
	void		MuliplyMotion	(C3Motion* pMotion, C3Motion* pBlendMotion);
	void		GetBoneMatrix	(D3DXMATRIXA16* pobjOut, int nMesh, int nPhy);
	BOOL		HitTest			(const D3DXVECTOR3* pvFrom, const D3DXVECTOR3* pvDir);
	void		TransformCoordforSimpleObj(D3DXVECTOR3* pobjMin, D3DXVECTOR3* pobjMax, D3DXMATRIXA16* pobjMatrix);
	C3MotionTracker	GetVirtualMotion	(const char* pszName);

	OBJID		GetTypeID(void)	{return m_idType;}
	D3DXMATRIXA16*	GetMatrixLocal(void){return &m_matrixLocal;}
	D3DXMATRIXA16*	GetMatrixWorld(void){return &m_matrixWorld;}
protected:
	OBJID	m_idType;
	int		m_nAction;
	D3DXMATRIXA16	m_matrixLocal;
	D3DXMATRIXA16	m_matrixWorld;
	D3DXMATRIXA16	m_matrixAdjust;
	CGame3DEffectEx2	m_objGame3DEffectEx;
	BOOL	m_bTextureSet;
	int		m_nLodLevel;

public:
	void	SetLodLevel(int nLevel){m_nLodLevel = nLevel;}
	int		GetLodLevel(void){return m_nLodLevel;}
	RolePartInfo m_infoPart;
	I3DObj*		GetLodObj		(OBJID idMesh);
	D3DXMATRIXA16*	GetMatrixAdjust(void){return &m_matrixAdjust;}
};

#endif
