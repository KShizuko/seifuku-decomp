#if !defined(AFX_PHYSICS_H__9293353F_A570_448C_A16E_B040AB70D000__INCLUDED_)
#define AFX_PHYSICS_H__9293353F_A570_448C_A16E_B040AB70D000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"

class CPhysics
{
public:
	CPhysics();
	virtual ~CPhysics();

	static DWORD  S2DGetDistance(CMyPos posBegin, CMyPos posEnd);
	static BOOL IsPointInRect(CMyPos posPoint, CMyRect rect);
	static BOOL S2DBMGetPos(int nV, float fA, float fT, CMyPos posBegin, CMyPos posEnd, CMyPos& posCurrent);
	static void S3Get3DDir(CMy3DPos posBegin, CMy3DPos posEnd, CMy3DDir& dir3D);
	static void S2DR45VP(CMyPos posTarget, CMyPos& posPoint);
};

#endif
