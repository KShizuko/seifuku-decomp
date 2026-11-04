#include "3DStatuary.h"

C3DStatuary::C3DStatuary()
{
	m_nActionType	=	-1;
	m_fScale		=	-1;
	m_nFrame		=	-1;
	m_nDir			=	-1;
}

C3DStatuary::~C3DStatuary()
{

}
BOOL C3DStatuary::SetVariable(int nVariableType, DWORD dwData)
{
	switch(nVariableType)
	{
	case _VAR_FRAME:
		if(m_nFrame == -1)
		{
			m_nFrame = dwData;
			this->SetFrame(dwData);
		}
		return true;
	case _VAR_DIR:
		if(m_nDir == -1)
		{
			m_nDir = dwData;
		}
		return true;
	}
	C3DRole::SetVariable(nVariableType, dwData);
	return true;
}
int C3DStatuary::SetAction(int nActionType, BOOL bResetMotion, BOOL bPlaySound, int nWorldX, int nWorldY, BOOL bSetEffect)
{
	if(m_nActionType == -1)
		m_nActionType = nActionType;
	else
		nActionType = m_nActionType;
	bPlaySound = bSetEffect = false;
	return C3DRole::SetAction(nActionType, bResetMotion, bPlaySound, nWorldX, nWorldY, bSetEffect);
}
BOOL C3DStatuary::Draw ( int nMapX, int nMapY )
{
	this->SetRGBA(1.0, 0.5, 0.5, 0.5);
	this->SetFrame(m_nFrame);
	return C3DRole::Draw(nMapX, nMapY);
}
