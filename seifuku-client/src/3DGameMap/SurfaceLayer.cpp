#include "SurfaceLayer.h"
CSurfaceLayer::CSurfaceLayer()
{
}
CSurfaceLayer::~CSurfaceLayer()
{
}
void CSurfaceLayer::Show(CMyPos posShow)
{
}
void CSurfaceLayer::Process(void* pInfo)
{
}
int  CSurfaceLayer::GetType()
{
	return LAYER_SURFACE;
}
CSurfaceLayer* CSurfaceLayer::CreateNew()
{
	CSurfaceLayer* pLayer = new CSurfaceLayer;
	return pLayer;
}

