#ifndef _LAYER_H
#define _LAYER_H

#include "BaseFunc.h"

struct  LayerInfo
{
        unsigned short     usTerrain;
        unsigned short     usMask;
        short              sAltitude;
        LayerInfo*         pLayer;
};
typedef struct
{
        unsigned short     usTerrain;
        unsigned short     usMask;
        short              sAltitude;
}IndexLayerInfo;


class CLayer
{
public:
        unsigned short     usTerrain;
        unsigned short     usMask;
        short              sAltitude;
	
public:
	CLayer();
	virtual ~CLayer();
	
public:
	static HANDLE	        s_hHeap;
	static UINT		s_uNumAllocsInHeap;
	static BOOL		IsValidPt(void* p);

public:
	void*  operator new	(size_t size);
	void   operator delete	(void* p);
};

typedef struct
{
        LayerInfo       m_infoLayer;
        bool            m_bSearched;
}CellInfo;

#include <deque>
#include <vector>
using namespace std;
typedef deque<CLayer* >  DEQUE_LAYER;
typedef vector<IndexLayerInfo> VECTOR_INDEXLAYER;
#endif
