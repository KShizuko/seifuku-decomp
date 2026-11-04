#ifndef HEADER_FLOOR_KATTY
#define HEADER_FLOOR_KATTY
#include "MapLayer.h"
#include "PatternRose.h"
#include "Rose.h"
#include <deque>
#include "Ani.h"

class CFloorObj:public CMapObj
{
	friend struct FloorObjProbe;
public:
	CFloorObj(){m_pAni=NULL;}
	~CFloorObj(){Destroy();}
	BOOL Create(const char* pszDataFile, const char* pszIndex,int nWorldX,int nWorldY,int nLifeTime=-1);
	void Show(void* pInfo);
	void Process(void* pInfo);
	void Destroy();
	void GetWorldPos(CMyPos& posWorld){posWorld.x = m_nWorldX;posWorld.y = m_nWorldY;}
	void SetWorldPos(CMyPos& posWorld){m_nWorldX = posWorld.y;m_nWorldY = posWorld.y;}
	IAni * GetAni(){return m_pAni;}
	int m_nLifeTime;
private:
	int m_nWorldX,m_nWorldY;
	int m_nCurrentFrame;
	IAni *m_pAni;
};
class CFloorLayer :public CMapLayer
{
public:
	CFloorLayer(){}
	~CFloorLayer(){Destory();}
	void Destory();
	void Show(CMyPos posShow);
	void Process(void* pInfo);
	int  GetType(){return LAYER_FLOOR;}
	void CreateRose(int iWorldX,int iWorldY,int nType,int nColor);
 	void CreateRose(int iWorldX,int iWorldY,int nColor,char * szChar){m_ObjPatternRose.Create(iWorldX,iWorldY,nColor,szChar);}
	static CFloorLayer * CreateNew();
 	void AddObj(CMapObj* obj);
	void ClearObj();
 
private:
	CRoseSys		m_ObjRose;
	CPatternRoseSys m_ObjPatternRose; 
	DEQUE_MAPOBJ	m_dequeMapObj;
	friend struct FloorLayerProbe;

};

#endif