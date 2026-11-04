#include "BaseFunc.h"
#include "c3_common.h"
#include "AlphaMask.h"
#include <malloc.h>

CMyHeap CAlphaMask::s_heap("CAlphaMask");

CAlphaMask::CAlphaMask()
{
	m_pBuffer = NULL;
}
CAlphaMask::~CAlphaMask()
{
	free(m_pBuffer);
	m_pBuffer = NULL;
}
BOOL CAlphaMask::IsTransparent(CMyPos posPoint)
{
	if	((posPoint.x < 0) ||
		(posPoint.y < 0) ||
		(posPoint.x >= m_sizeInfo.iWidth) ||
		(posPoint.y >= m_sizeInfo.iHeight))
		return true;

	int nIndex = posPoint.x + posPoint.y*m_sizeInfo.iWidth;

	int nByteOffset = 0;
	int nBitOffset = 0;
	nByteOffset = nIndex/8;
	nBitOffset = nIndex%8;
	BYTE* bData = m_pBuffer+nByteOffset;


	BYTE bMask = 1 << nBitOffset;
	if(bMask & *bData)
		return false;
	return true;
}
BOOL CAlphaMask::Load(CMySize sizeBmp, char* pszFileName)
{
	if(!pszFileName)
		return false;
	DWORD dwSize = 0;
	void* fp = ::Common_OpenRes(pszFileName, dwSize, 1);
	if(!fp)
		return false;
	m_pBuffer = (BYTE*)malloc(dwSize);
	if(::Common_ReadRes(fp, m_pBuffer, dwSize) != dwSize)
	{
		::Common_CloseRes(fp);
		free(m_pBuffer);
		return false;
	}
	int nBufferSize = (sizeBmp.iWidth * sizeBmp.iHeight) / 8;
	if((sizeBmp.iWidth * sizeBmp.iHeight) % 8 != 0)
		nBufferSize ++;
	if(nBufferSize != (int)dwSize)
	{
		::Common_CloseRes(fp);
		free(m_pBuffer);
		return false;
	}
	::Common_CloseRes(fp);
	m_sizeInfo = sizeBmp;
	return true;
}
