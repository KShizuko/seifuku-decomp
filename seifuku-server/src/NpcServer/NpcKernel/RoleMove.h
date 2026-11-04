#if !defined(AFX_ROLEMOVE_H__2D56EB03_B4DC_483C_A793_0B49B0DAEF50__INCLUDED_)
#define AFX_ROLEMOVE_H__2D56EB03_B4DC_483C_A793_0B49B0DAEF50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "mycom.h"
#include "AutoPtr.h"
#include "TimeOut.h"
#include "T_myqueue.h"
#include "Array.h"

const int	WALK_DELAY_MS		= 600;
const int	RUN_DELAY_MS		= 300;
const int	JUMP_DELAY_MS		= 1200;
const int	CHGMAP_DELAY_MS		= 5000;
const int	JUMP_DISTANCE		= 16;
const int	CHGMAP_DISTANCE		= 100;

class CAgent;
class CRoleMove  
{
	COM_STYLE(CRoleMove)
protected:
	CRoleMove();
	virtual ~CRoleMove();

public:
	bool	Create(CAgent* pAgent);
	void	OnTimer();
public:
	CGameMap*	GetMap()									{ MYASSERT(m_pMap); return m_pMap; }
	int		GetPosX()										{ return m_pos.x; }
	int		GetPosY()										{ return m_pos.y; }
	bool	IsMoving()										{ return m_bJump || m_setPath.size() > 0; }
	bool	IsMoveEnable()									{ return !m_bLock; }
	int		GetDistance(int x, int y)						{ return __max(abs(x-m_pos.x), abs(y-m_pos.y)); }
protected:
	CTimeOutMS&	QueryTime()									{ return m_tMove; }

public:
	POINT FindRandomPos();
	void SetPos(int nPosX, int nPosY);
	void	EnterMap(OBJID idMap, int x, int y);
	bool	WalkTo(int x, int y, int nDist=0);
	bool	RunTo(int x, int y, int nDist=0);
	bool	JumpTo(int x, int y, int nDist=0);
	bool	JumpSafePos(int nSafeRange=2);
	void	FlyMap(OBJID idMap, int nPosX, int nPosY);
protected:
	bool	FindPath(int x, int y, int nDist);
	bool	FindPath(Array<int>* psetPath, int x, int y);
	void	Jump(int x, int y);
	void	ClearMove()										{ m_bJump = false; m_setPath.clear(); }

protected:
	CAgent*		m_pOwner;

protected:
	CGameMap*	m_pMap;
	POINT		m_pos;
	POINT		m_posTarget;
	CMyQueue<int>	m_setPath;
	bool		m_bJump;

protected:
	bool		m_bLock;
	CTimeOutMS	m_tMove;
	int 		m_nMoveMode;
};

#endif
