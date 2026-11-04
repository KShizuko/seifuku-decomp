#include "Physics.h"
CPhysics::CPhysics()
{

}

CPhysics::~CPhysics()
{

}

DWORD CPhysics::S2DGetDistance(CMyPos posBegin, CMyPos posEnd)
{
	return (DWORD)sqrt(
					(posBegin.x - posEnd.x) *
					(posBegin.x - posEnd.x) +
					(posBegin.y - posEnd.y) *
					(posBegin.y - posEnd.y)
				);
}
BOOL CPhysics::S2DBMGetPos(int nV, float fA, float fT, CMyPos posBegin, CMyPos posEnd, CMyPos& posCurrent)
{
	int nDistance = S2DGetDistance(posBegin, posEnd);
	if(nDistance == 0)
	{
		posCurrent = posEnd;
		return true;
	}
	int nCompleteDistance = (int)(nV * fT + fA * fT * fT / 2);
	if(nCompleteDistance >= nDistance)
	{
		posCurrent = posEnd;
		return true;
	}
	posCurrent.x = (posEnd.x - posBegin.x) * nCompleteDistance / nDistance + posBegin.x;
	posCurrent.y = (posEnd.y - posBegin.y) * nCompleteDistance / nDistance + posBegin.y;
	return false;
}
void CPhysics::S3Get3DDir(CMy3DPos posBegin, CMy3DPos posEnd, CMy3DDir& dir3D)
{
	double fPei=3.14159;
	int nOffsetX = posEnd.x-posBegin.x;
	if(nOffsetX == 0)
		nOffsetX = 1;
	double fTan = (posEnd.y-posBegin.y)*1.0/nOffsetX;
	if(fTan < 0)
		fTan = -fTan;
	double fHorizontal = atan(fTan);
	int nX = posEnd.x - posBegin.x;
	int nY = posEnd.y - posBegin.y;
	if((nX > 0) && (nY < 0))
	{
		dir3D.fHorizontal = (float)(fHorizontal + fPei/2);
	}
	else if((nX < 0) && (nY >= 0))
	{
		dir3D.fHorizontal = (float)(fHorizontal - fPei/2);
	}
	else if((nX < 0) && (nY < 0))
	{
		dir3D.fHorizontal = (float)(-fPei/2 - fHorizontal);
	}
	else
	{
		dir3D.fHorizontal = (float)(fPei/2 - fHorizontal);
	}
	fTan = abs(posEnd.z-posBegin.z)/sqrt((posEnd.y-posBegin.y)*(posEnd.y-posBegin.y) + (posEnd.x-posBegin.x)*(posEnd.x-posBegin.x));
	double fVertical = atan(fTan);
	int nZ = posEnd.z-posBegin.z;
	if(nZ >=0 )
		dir3D.fVertical = (float)fVertical;
	else
		dir3D.fVertical = (float)(-fVertical);
}
void CPhysics::S2DR45VP(CMyPos posTarget, CMyPos& posPoint)
{
	CMyPos posRule={0, 32};
	float fTag = 0.0;
	if(posTarget.x == 0)
		fTag = 0.0;
	else if(posTarget.y == 0)
		fTag = 2.0;
	else
		fTag = (float)(1.0*posTarget.x/posTarget.y);
	if(fTag < 0)
		fTag = -fTag;
	if(fTag > 2.0)
		fTag = 2.0;
	float fDistance = (float)sqrt(posTarget.x*posTarget.x + posTarget.y*posTarget.y);
	float fRealDistance = (float)(16 + 16*fTag*1.0/2);
	CMyPos posRealTarget;
	if((posTarget.x == 0)&&(posTarget.y != 0))
	{
		posRealTarget.x = 0;
		posRealTarget.y = (int)((posTarget.y/abs(posTarget.y))*fRealDistance);
	}
	else if((posTarget.y == 0)&&(posTarget.x != 0))
	{
		posRealTarget.y = 0;
		posRealTarget.x = (int)((posTarget.x/abs(posTarget.x))*fRealDistance);
	}
	else if((posTarget.y == 0)&&(posTarget.x == 0))
	{
		posRealTarget.x = 0;
		posRealTarget.y = 0;
	}
	else
	{
		posRealTarget.x = (int)(1.0*posTarget.x*fRealDistance/fDistance);
		posRealTarget.y = (int)(1.0*posTarget.y*fRealDistance/fDistance);
	}
	float fScale = (float)(posPoint.x*1.0/32);
	if(fScale < 0)
		fScale = -fScale;
	posPoint.x = (int)(posRealTarget.x * fScale);
	posPoint.y = (int)(posRealTarget.y * fScale);
}
BOOL CPhysics::IsPointInRect(CMyPos posPoint, CMyRect rect)
{
	if((posPoint.x >= rect.l) &&
	   (posPoint.x <= rect.r) &&
	   (posPoint.y >= rect.b) &&
	   (posPoint.y <= rect.t))
	   return true;
	return false;
}
