#ifndef _TERRAINOBJPART_H
#define _TERRAINOBJPART_H

#include "MapObj.h"
#include "Layer.h"
#include "BaseFunc.h"
class CTerrainObjPart : public CMapObj
{
public:
	CTerrainObjPart();
	virtual ~CTerrainObjPart();
	static CTerrainObjPart*		CreateNew(FILE* fp);

private:
	char              m_szAniTitle[64];
	char              m_szAniFile[255];
	POINT             m_posOffset;
	DWORD             m_dwFrameInterval;

	POINT             m_posSceneOffset;
	SIZE              m_sizeBase;
	DEQUE_LAYER       m_setLayer;
	int               m_nThickness;
	int               m_nHeight;

public:

protected:
	void    Destroy();

public:
	char*   GetAniTitle()					{return m_szAniTitle;}
	POINT	GetOffset()						{return m_posOffset;}
	DWORD   GetAniInterval()				{return m_dwFrameInterval;}

	POINT	GetSceneOffset()				{return m_posSceneOffset;}
	SIZE 	GetBase()						{ return m_sizeBase; }
	CLayer* GetLayer(int nX, int nY);
	int     GetThick()						{return m_nThickness;}
	int     GetHeight()						{return m_nHeight;}

};

#include <vector>
using namespace std;
typedef vector<CTerrainObjPart* >  DEQUE_SCENEPART;

#endif
