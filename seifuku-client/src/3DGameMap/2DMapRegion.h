#if !defined(AFX_2DMAPREGION_H__12CE67DD_4E10_4341_B542_2FC8082C7A28__INCLUDED_)
#define AFX_2DMAPREGION_H__12CE67DD_4E10_4341_B542_2FC8082C7A28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "2DMapObj.h"

typedef struct
{
	OBJID	m_idMap;
	CMyPos	m_posCell;
	DWORD	m_dwType;
	DWORD	m_dwCx;
	DWORD	m_dwCy;
	int		m_nNColor;
	int		m_nNShowType;
	int		m_nDColor;
	int		m_nDShowPos;
	char	m_szString[2][32];
	DWORD	m_dwAccessTime;
	BOOL	m_bAccess;
	char	m_szMedia[256];
}RegionInfo;

class C2DMapRegion
{
public:
	C2DMapRegion();
	virtual ~C2DMapRegion();

private:
	RegionInfo m_InfoRegion;
	friend struct MapRegionProbe;
public:
	void Show(void* pInfo);
	static C2DMapRegion* CreateNew(int x, int y, int cx, int cy);
	void Process(void* pInfo){;}
	int	 GetObjType(){return MAP_2DREGION;}
	BOOL IsFocus(){return false;}
	RegionInfo* GetInfo(){return &m_InfoRegion;}
	static C2DMapRegion* CreateNew(RegionInfo* pInfo);
};

#endif
