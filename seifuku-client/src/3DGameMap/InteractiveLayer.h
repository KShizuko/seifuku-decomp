#ifndef _INTERACTIVELAYER_H
#define _INTERACTIVELAYER_H
#include "MapLayer.h"
#include "MapObj.h"
#include "2DMapCoverObj.h"
#include "2DMapTerrainObj.h"
#include "3DMapEffect.h"
#include "MapSound.h"

class CInteractiveLayer : public CMapLayer
{
public:
	CInteractiveLayer();
	~CInteractiveLayer();

public:
	void Show(CMyPos posShow);
	void Process(void* pInfo);
	int  GetType();

public:
	static CInteractiveLayer* CreateNew();

public:
	void ClearMapObj();
	void AddMapObj(CMapObj* pObj);
	void DelObj(CMapObj* pObj);

	void PlaceTerrainObj(C2DMapTerrainObj* pTerrainObj);
	void DisplaceTerrainObj(C2DMapTerrainObj* pTerrainObj);

	int GetObjAmount();
	C2DMapTerrainObjPart* Get2DFocusMapTerrainObjPart();
	void LoadDataLayer(FILE* fp);
	CMapSound* GetFocusSound();
	C2DMapCoverObj* Get2DFocusMapCoverObj();
	C2DMapTerrainObj* Get2DFocusMapTerrainObj();
	void Del3DMapEffect(CMyPos posWorld);
	void Del3DMapEffect(C3DMapEffect* pEffect);
	C2DMapCoverObj* GetMapCoverObj(CMyPos posCell);
	C3DMapEffect* Get3DFocusMapEffect();
	void SaveDataLayer(FILE* fp);
	void SaveTextLayer(FILE* fp);
	void LoadTextLayer(FILE* fp);


	void ApplyShow(CMapObj* pObj);
	void ApplyShow(CMapObj* pNewObj, DEQUE_MAPOBJ& m_setApply);

	
	CMapObj* GetObjByIndex(int nIndex);
private:
	DEQUE_MAPOBJ m_setMapObj;
	DEQUE_MAPOBJ m_setMapObjShow;
	friend struct InteractiveLayerProbe;
};
#endif
