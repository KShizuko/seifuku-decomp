#include "Path.h"

CPath::CPath()
{
	m_setStep.clear();
	m_nSearchMethod = _ASTAR_CLOSE;
	m_nData = 15;
}

CPath::~CPath()
{
	m_setStep.clear();
}

int  CPath::GetNextStep(void)
{
	if(m_setStep.size() <= 0)
		return -1;
	char cStep = m_setStep[0];
	m_setStep.pop_front();
	return cStep;
}
void CPath::ClearStep()
{
	m_setStep.clear();
}
void CPath::AddStep(char cStep)
{
	m_setStep.push_back(cStep);
}
int  CPath::GetStepAmount()
{
	return m_setStep.size();
}
void CPath::SetPlayer(CMapObj* pMapObj)
{
	m_pMapObj = pMapObj; 
}
void CPath::SetSearchMethod(int nMethod)
{
	m_nSearchMethod = nMethod;
}
int  CPath::GetSearchMethod()
{
	return m_nSearchMethod;
}
void CPath::SetPathData(int nData)
{
	m_nData = nData;
}
int	CPath::GetData()
{
	return m_nData;
}
