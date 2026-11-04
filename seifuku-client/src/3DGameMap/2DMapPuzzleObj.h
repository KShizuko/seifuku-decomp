#if !defined(AFX_2DMAPPUZZLEOBJ_H__022895C6_F922_49BE_B930_CA3E4AE2C1CD__INCLUDED_)
#define AFX_2DMAPPUZZLEOBJ_H__022895C6_F922_49BE_B930_CA3E4AE2C1CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "2DMapObj.h"
#include "PuzzleBmp.h"

class C2DMapPuzzleObj  : public C2DMapObj
{
	friend struct MapPuzzleObjProbe;
public:
	C2DMapPuzzleObj();
	virtual ~C2DMapPuzzleObj();

public:
	void Show(void* pInfo);
	void Process(void* pInfo){;}
	int	 GetObjType   (){return MAP_PUZZLE;}
	BOOL IsFocus(){return false;}
	void SetBase(CMySize infoSize){;}
	void GetBase(CMySize& infoSize){;}
	void LoadData(FILE* fp);
	void SaveData(FILE* fp);
	void LoadFile(char* pszFile);
	void SaveText(FILE* fp);
	void LoadText(FILE* fp);
private:
	char m_szFileName[_MAX_PATH];
public:
	CPuzzleBmp	m_bmpPuzzle;
};

#endif
