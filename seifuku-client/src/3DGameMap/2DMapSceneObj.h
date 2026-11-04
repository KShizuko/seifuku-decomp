#ifndef _2DMAPSCENEOBJ_H
#define _2DMAPSCENEOBJ_H
#include "2DMapObj.h"
#include "BaseFunc.h"
class C2DMapSceneObj : public C2DMapObj
{
	friend struct MapSceneObjProbe;
public:
	static BOOL s_bStroke;
public:
	C2DMapSceneObj();
	~C2DMapSceneObj();
private:
	char m_szFileName[_MAX_PATH];
	char m_szIndex[_MAX_TITLESIZE];
	CMyPos m_posOffset;
	int m_nFrameIndex;
	DWORD m_dwFrameInterval;
	DWORD m_dwTimePreFrame;
	DWORD m_dwShowWay;

public:
	void Show(void* pInfo);
	void Process(void* pInfo);
	int  GetObjType   (){return MAP_SCENE;}
	BOOL IsFocus();
public:
	void   LoadDataObj(FILE* fp);
	static C2DMapSceneObj* CreateNew(char* pszFileName, char* pszIndex);
	void   Create(char* pszFileName, char* pszIndex);
	void   LoadTextObj(FILE* fp);
	void   SaveDataObj(FILE* fp);
	void   SaveTextObj(FILE* fp);
	void   SetFrameInterval(DWORD dwIntercal);
	DWORD  GetFrameInterval();
	void   SetOffset(CMyPos posOffset);
	void   GetOffset(CMyPos& posOffset);
};
#endif
