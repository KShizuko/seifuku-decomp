#include "BoothNpc.h"

const OBJID ID_NONE = 0;

CBoothNpc::CBoothNpc()
{
	m_dwItem1 = m_dwItem0 = ID_NONE;
}

CBoothNpc::~CBoothNpc()
{

}
BOOL CBoothNpc::HitTest ( int nHitScreenX, int nHitScreenY, int nMapX, int nMapY )
{
	for(int i = 0; i < 5; i ++)
	{
		int nX = nMapX + BoothMaskOffset[m_nDir][i][0];
		int nY = nMapY + BoothMaskOffset[m_nDir][i][1];
		if(nX == nHitScreenX && nY == nHitScreenY)
			return true;
	}
	return false;
}
BOOL CBoothNpc::Draw (int nMapX, int nMapY)
{
	return C3DTaskNpc::Draw(nMapX, nMapY);
}
BOOL CBoothNpc::SetVariable(int nVariableType, DWORD dwData)
{
	switch(nVariableType)
	{
	case _VAR_ITEM0:
		m_dwItem0 = nVariableType;
		break;
	case _VAR_ITEM1:
		m_dwItem1 = nVariableType;
		break;
	default:
		return C3DTaskNpc::SetVariable(nVariableType, dwData);
		break;
	}
	return true;
}
