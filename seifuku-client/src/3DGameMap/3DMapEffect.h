#if !defined(AFX_3DMAPEFFECT_H__4DCD9A12_136D_4BEF_B80D_2204A58B7FBE__INCLUDED_)
#define AFX_3DMAPEFFECT_H__4DCD9A12_136D_4BEF_B80D_2204A58B7FBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "3DMapObj.h"
#include "Game3DEffectEx.h"

class C3DMapEffect 	:	public C3DMapObj 
{
public:
	static BOOL s_bStroke;
protected:
	CMyPos			m_posWorld;
	BOOL			m_bDelSelf;
	BOOL			m_bDie;
	int				m_nOffsetY;
	BOOL			m_bSave;
	char			m_szIndex[64];
	float			m_fHorizontal;
	float			m_fVertical;
	BOOL			m_bOver;
	int				m_nDir;
	IGame3DEffect*	m_objEffect;

public:
	C3DMapEffect();
	virtual ~C3DMapEffect();

public:
	static  C3DMapEffect* CreateNew(CMyPos posWorld, char* pszIndex, BOOL bDelSelf = true, BOOL bSave = false, int nDir = -1);
	BOOL Create(CMyPos posWorld, char* pszIndex, BOOL bDelSelf = true, BOOL bSave = false);

public:
	void SetRotate(float fHorizontal, float fVertical);
	virtual void Show(void* pInfo);
	virtual void Process(void* pInfo);
	virtual void SetWorldPos(CMyPos posWorld);
	virtual void GetWorldPos(CMyPos& posWorld);
	virtual int GetObjType(){return MAP_3DEFFECT;}
	virtual BOOL IsFocus();
	virtual void Die(){m_bDie = true;}
	virtual void SetOffsetY(int nOffset){m_nOffsetY = nOffset;}

public:
	BOOL	NeedSave(){return m_bSave;}
	void	LoadDataObj(FILE* fp);
	void	LoadTextObj(FILE* fp);
	void	SaveDataObj(FILE* fp);
	void	SaveTextObj(FILE* fp);
	void	SetDir(int nDir);
public:
	BOOL	IsOver(){return m_bOver;}
	IGame3DEffect*	QueryGame3DEffectEx(){return m_objEffect;}
};

#endif
