#ifndef _LAYER_H
#define _LAYER_H

#pragma	warning(disable:4786)
#include <memory>
#include "BaseFunc.h"
#include "MyHeap.h"
#include <vector>
using namespace std;

class CLayer
{
public:
	unsigned short		nTerrian;
	short				nAltitude;
	unsigned short		dwMask;

public:
	CLayer();
	static shared_ptr<CLayer>	CreateNew() { return shared_ptr<CLayer>(new CLayer); }
};

#include <vector>
using namespace std;
typedef vector<shared_ptr<CLayer> >  DEQUE_LAYER;
typedef vector<DEQUE_LAYER>  DEQUE2_LAYER;

#endif
