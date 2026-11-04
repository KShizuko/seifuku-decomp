#if !defined(AFX_PATH_H__B9591737_CA9C_42CD_AE06_A0F3D5BD89D5__INCLUDED_)
#define AFX_PATH_H__B9591737_CA9C_42CD_AE06_A0F3D5BD89D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MapObj.h"

#include <deque>
using namespace std;
typedef struct 
{
	char cStepDir;
	unsigned char ucStepLength;
}StepInfo;
typedef deque<char>  DEQUE_STEP;

enum{_ASTAR_NONE=0, _ASTAR_CLOSE, _ASTAR_LEAVE};
class CPath  
{
public:
	CPath();
	virtual ~CPath();
	
private:
	DEQUE_STEP m_setStep;
	CMapObj* m_pMapObj;
	int		 m_nSearchMethod;
	int		 m_nData;
	BOOL     m_bFound;
public:
	void ClearStep();

	int  GetNextStep(void);
	void AddStep(char cStep);

	int TestNextStep();

	int  GetStepAmount();
	void SetPlayer(CMapObj* pMapObj);
	void SetSearchMethod(int nMethod);
	int  GetSearchMethod();
	void SetPathData(int nData);
	int	 GetData();
	int	 IsReachable(){return m_bFound;}
	void SetReachable(BOOL bReachable){m_bFound = bReachable;}
};

#endif
