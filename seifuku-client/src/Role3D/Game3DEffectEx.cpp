#include "Game3DEffectEx.h"
#include "3DEffect.h"
#include "GameDataSet.h"
#include "ND_Bitmap.h"
#include "MyHeap.h"
#include "3DRolePart.h"
#include <mmsystem.h>

#define TimeGet timeGetTime

const OBJID ID_NONE = 0;

static CMyHeap s_heapGame3DEffectEx2("CGame3DEffectEx2");

int g_nAdjustOblique		= _ADJUST_UNSET;
int g_nAdjustSimpleObj		= _ADJUST_UNSET;
int g_nAdjustEffect			= _ADJUST_UNSET;
int g_nAdjustEffectOrder	= 0;

static int s_nNextId = 0;

Game3DEffectIdInfo::Game3DEffectIdInfo()
{
	idEffect		= ID_NONE;
	idTexture		= ID_NONE;
	pPtclInfo		= NULL;
	pPtcl3Info		= NULL;
	pShapeInfoBuf	= NULL;
	nShapes			= 0;
	bReady			= FALSE;
}

Game3DEffectIdInfo::~Game3DEffectIdInfo()
{
	::DestroyShapeInfo(pShapeInfoBuf, nShapes);
	::DestroyPtclInfo(pPtclInfo);
	::DestroyPtcl3Info(pPtcl3Info);
}

CGame3DEffectEx2::CGame3DEffectEx2(IGame3DEffectHandler* pHandler)
{
	m_setInfo.clear();
	m_pHandler = pHandler;
	m_nOffsetY = 0;
	m_Height = 0;
	m_bOblique = true;
	m_fX = 0.0;
	m_fY = 0.0;
	m_fZ = 0.0;
	m_fPosX = 0.0;
	m_fPosY = 0.0;
	m_fPosZ = 0.0;
	m_fScaleX = 1.0;
	m_fScaleY = 1.0;
	m_fScaleZ = 1.0;
}

CGame3DEffectEx2::~CGame3DEffectEx2()
{
	this->Clear();
}

DWORD CGame3DEffectEx2::Release()
{
	if (this)
		delete this;
	return 0;
}

void* CGame3DEffectEx2::operator new(size_t size) throw()
{
	return s_heapGame3DEffectEx2.Alloc(size);
}

void CGame3DEffectEx2::operator delete(void* p)
{
	s_heapGame3DEffectEx2.Free(p);
}

IGame3DEffect* Game3DEffectCreate ( IGame3DEffectHandler* pHandler )
{
	CGame3DEffectEx2* pEffect = new CGame3DEffectEx2(pHandler);
	if (!pEffect)
		return NULL;
	return pEffect;
}

BOOL CGame3DEffectEx2::Show(CMyPos posWorld, int nId)
{
	int nAmount = m_setInfo.size();
	int i;
	for(i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(!pInfo)
			return false;
		if(!IsLoaded(pInfo))
			return false;
	}
	for(i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			CMyPos myPosWorld;
			myPosWorld = posWorld;
			myPosWorld.y += m_nOffsetY;
			this->Show(pInfo, myPosWorld);
		}
	}
	return this->Process();
}

BOOL CGame3DEffectEx2::IsLoaded(Game3DEffectExInfo* pInfo)
{
	if(!pInfo)
		return false;
	for(int i = 0; i < pInfo->nInfoAmount; i++)
	{
		Game3DEffectIdInfo* pIDInfo = pInfo->pIDInfo+i;
		if(!GameDataSetQuery()->Get3DEffect(pIDInfo->idEffect, 0, 0))
			return false;
		if(!GameDataSetQuery()->Get3DTexture(pIDInfo->idTexture, 0, 0))
			return false;
	}
	return true;
}

void CGame3DEffectEx2::ShowPart(C3DRolePart* pPart)
{
	if(!pPart)
		return;
	for(int i = 0; i < m_setInfo.size(); i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo)
		{
			this->ShowPart(pInfo, pPart);
		}
	}
	this->Process();
}

void CGame3DEffectEx2::ShowPart(Game3DEffectExInfo* pInfo, C3DRolePart* pPart)
{
	if(!pInfo)
		return;
	if(!pPart)
		return;
	if(pInfo->nState != _GAME3DEFFECT_STATE_PLAYING)
		return;
	Game3DEffectExInfo* pMyInfo = pInfo;
	int nAmount = pInfo->nInfoAmount;
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectIdInfo* pIDInfo = pInfo->pIDInfo+i;
		if(!pIDInfo->bReady)
			return;
		I3DEffect* pEffect = GameDataSetQuery()->Get3DEffect(pIDInfo->idEffect, 0, 0);
		if(!pEffect)
			return;
		I3DTexture* pTexture = GameDataSetQuery()->Get3DTexture(pIDInfo->idTexture, 0, 0);
		if(!pTexture)
			return;
		if(!pTexture->GetTexture())
			return;
		if(pMyInfo->dwCurrentFrameIndex >= pEffect->GetFrames())
			return;
		pEffect->SetTexture(pTexture);
		pEffect->SetFrames(pMyInfo->dwCurrentFrameIndex);
		pEffect->SetStepCount(pMyInfo->dwStepCount);
		pEffect->RestoreShapeInfo(pIDInfo->pShapeInfoBuf);
		pEffect->SetPtclInfo(pIDInfo->pPtclInfo);
		pEffect->SetPtcl3Info(pIDInfo->pPtcl3Info);
		pEffect->SetPtcl3Gravity(pMyInfo->fGravity[0], pMyInfo->fGravity[1], pMyInfo->fGravity[2]);
		pEffect->SetPtcl3ExternalSpeed(pMyInfo->fExternalSpeed[0], pMyInfo->fExternalSpeed[1], pMyInfo->fExternalSpeed[2]);
		RolePartInfo* pPartInfo = pPart->GetInfo();
		if(pPartInfo->nPart > 0)
		{
			I3DObj* pObj = pPart->GetLodObj(pPartInfo->idMesh[0]);
			if(!pObj)
				return;
			pEffect->Bind(pObj);
		}
		else
		{
			D3DXMATRIXA16 matrix;
			pPart->GetBoneMatrix(&matrix, 0, 0);
			pEffect->BindMatrix(&matrix);
		}
		pEffect->Draw(false, pMyInfo->nASB[i], pMyInfo->nADB[i]);
		pEffect->BackupShapeInfo(pIDInfo->pShapeInfoBuf);
	}
	pMyInfo->dwStepCount++;
}

void CGame3DEffectEx2::Reset()
{
	m_fScaleX = m_fScaleY = m_fScaleZ = 1.0;
	m_fX = m_fY = m_fZ = 0.0;
	m_fPosX = m_fPosY = m_fPosZ = 0.0;
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo)
		{
			pInfo->fPosX = pInfo->fPosY = pInfo->fPosZ = 0.0;
			pInfo->fScaleX = pInfo->fScaleY = pInfo->fScaleZ = 1.0;
			pInfo->fRotateX = pInfo->fRotateY = pInfo->fRotateZ = 0.0;
		}
	}
}

void CGame3DEffectEx2::SetPos(float x, float y, float z, int nIndex, bool bById)
{
	if(nIndex == -1)
	{
		m_fPosX = x;
		m_fPosY = y;
		m_fPosZ = z;
		return;
	}
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (bById ? pInfo->nId == nIndex : i == nIndex))
		{
			pInfo->fPosX = x;
			pInfo->fPosY = y;
			pInfo->fPosZ = z;
		}
	}
}

void CGame3DEffectEx2::Rotate(float x, float y, float z, int nIndex, bool bById)
{
	if(nIndex == -1)
	{
		m_fX = x;
		m_fY = y;
		m_fZ = z;
		return;
	}
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (bById ? pInfo->nId == nIndex : i == nIndex))
		{
			pInfo->fRotateX = x;
			pInfo->fRotateY = y;
			pInfo->fRotateZ = z;
		}
	}
}

void CGame3DEffectEx2::Scale(float x, float y, float z, int nIndex, bool bById)
{
	if(nIndex == -1)
	{
		m_fScaleX = x;
		m_fScaleY = y;
		m_fScaleZ = z;
		return;
	}
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (bById ? pInfo->nId == nIndex : i == nIndex))
		{
			pInfo->fScaleX = x;
			pInfo->fScaleY = y;
			pInfo->fScaleZ = z;
		}
	}
}

void CGame3DEffectEx2::SetDir(int nAngle, int nId)
{
	float fAngle = D3DXToRadian((float)nAngle);
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			pInfo->fRotateZ = fAngle;
			if(nId != -1)
				return;
		}
	}
}

void CGame3DEffectEx2::BindMatrix(D3DXMATRIXA16* pMatrix, int nId)
{
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			for(int j = 0; j < pInfo->nInfoAmount; j++)
			{
				I3DEffect* pEffect = GameDataSetQuery()->Get3DEffect(pInfo->pIDInfo[j].idEffect);
				if(pEffect)
					pEffect->BindMatrix(pMatrix);
			}
			if(nId != -1)
				return;
		}
	}
}

void CGame3DEffectEx2::SetOffset3(int nX, int nY, int nZ, int nId)
{
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			pInfo->nOffsetX = nX;
			pInfo->nOffsetY = nY;
			pInfo->nOffsetZ = nZ;
		}
	}
}

void CGame3DEffectEx2::SetColor(DWORD dwColor, int nIndex, bool bById)
{
	int nAmount = m_setInfo.size();
	if(!bById)
	{
		if(nAmount != 0)
		{
			Game3DEffectExInfo* pInfo = m_setInfo[(unsigned) nIndex % (unsigned) nAmount];
			if(pInfo)
				pInfo->dwColor = dwColor;
		}
		return;
	}
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nIndex == -1 || pInfo->nId == nIndex))
		{
			pInfo->dwColor = dwColor;
			if(nIndex != -1)
				return;
		}
	}
}

void CGame3DEffectEx2::SetBlend(int nASB, int nADB, int nId, int nPart)
{
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			for(int j = 0; j < pInfo->nInfoAmount; j++)
			{
				if(nPart == -1 || j == nPart)
				{
					pInfo->nASB[j] = nASB;
					pInfo->nADB[j] = nADB;
				}
			}
			if(nId != -1)
				return;
		}
	}
}

DWORD CGame3DEffectEx2::GetFrames(int nId)
{
	DWORD dwFrames = 0;
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			for(int j = 0; j < pInfo->nInfoAmount; j++)
			{
				I3DEffect* pEffect = GameDataSetQuery()->Get3DEffect(pInfo->pIDInfo[j].idEffect);
				if(pEffect && dwFrames <= pEffect->GetFrames())
					dwFrames = pEffect->GetFrames();
			}
		}
	}
	return dwFrames;
}

void CGame3DEffectEx2::SetHeight ( float fHeight )
{
	m_Height = fHeight;
}

void CGame3DEffectEx2::Pause(int nId)
{
	DWORD dwNow = ::TimeGet();
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			pInfo->dwPauseTime = dwNow;
			if(nId != -1)
				return;
		}
	}
}

void CGame3DEffectEx2::Resume(int nId)
{
	DWORD dwNow = ::TimeGet();
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			pInfo->dwTimeBegin += dwNow - pInfo->dwPauseTime;
			pInfo->dwPauseTime = 0;
			if(nId != -1)
				return;
		}
	}
}

void CGame3DEffectEx2::SkipFrames(int nFrames, int nId)
{
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			pInfo->dwTimeBegin -= pInfo->dwFrameInterval * nFrames;
			if(nId != -1)
				return;
		}
	}
}

void CGame3DEffectEx2::SkipTime(DWORD dwTime, int nId)
{
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			pInfo->dwTimeBegin -= dwTime;
			if(nId != -1)
				return;
		}
	}
}

void CGame3DEffectEx2::Show(Game3DEffectExInfo* pInfo, CMyPos posWorld)
{
	if(!pInfo)
		return;
	if(pInfo->nState != _GAME3DEFFECT_STATE_PLAYING)
		return;
	int nAmount = pInfo->nInfoAmount;
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectIdInfo* pIDInfo = pInfo->pIDInfo+i;
		if(!pIDInfo->bReady)
			return;
		I3DEffect* pEffect = GameDataSetQuery()->Get3DEffect(pIDInfo->idEffect, 0, 0);
		if(!pEffect)
			return;
		I3DTexture* pTexture = GameDataSetQuery()->Get3DTexture(pIDInfo->idTexture, 0, 0);
		if(!pTexture)
			return;
		pEffect->SetTexture(pTexture);
		pEffect->SetFrames(pInfo->dwCurrentFrameIndex);
		pEffect->SetStepCount(pInfo->dwStepCount);
		pEffect->RestoreShapeInfo(pIDInfo->pShapeInfoBuf);
		pEffect->SetPtclInfo(pIDInfo->pPtclInfo);
		pEffect->SetPtcl3Info(pIDInfo->pPtcl3Info);
		pEffect->SetPtcl3Gravity(pInfo->fGravity[0], pInfo->fGravity[1], pInfo->fGravity[2]);
		pEffect->SetPtcl3ExternalSpeed(pInfo->fExternalSpeed[0], pInfo->fExternalSpeed[1], pInfo->fExternalSpeed[2]);
		float fPercent = (float) (unsigned int) pInfo->nScale[i] * 0.01f;
		pEffect->Scale(pInfo->fScaleX*m_fScaleX*fPercent, pInfo->fScaleY*m_fScaleY*fPercent, pInfo->fScaleZ*m_fScaleZ*fPercent);
		if(g_nAdjustEffectOrder == 0)
		{
			pEffect->Move(0, -pInfo->nOffsetZ, m_Height*1.4);
			pEffect->Rotate(pInfo->fRotateX+m_fX, pInfo->fRotateY+m_fY, pInfo->fRotateZ+m_fZ);
		}
		else
		{
			pEffect->Rotate(pInfo->fRotateX+m_fX, pInfo->fRotateY+m_fY, pInfo->fRotateZ+m_fZ);
			pEffect->Move(0, pInfo->nOffsetZ, m_Height*1.4);
		}
		if(m_bOblique)
		{
			int nAngle = g_nAdjustEffect;
			if(nAngle == _ADJUST_UNSET)
				nAngle = CMyBitmap::GetObliqueAngle();
			pEffect->Rotate ( D3DXToRadian ( (float)nAngle ), 0, 0 );
		}
		pEffect->Move(pInfo->nOffsetX+pInfo->fPosX+m_fPosX+posWorld.x, pInfo->fPosY+m_fPosY, pInfo->nOffsetY+pInfo->fPosZ+m_fPosZ+posWorld.y);
		pEffect->SetColor(pInfo->dwColor);
		pEffect->Draw(false, pInfo->nASB[i], pInfo->nADB[i]);
		pEffect->BackupShapeInfo(pIDInfo->pShapeInfoBuf);
	}
	pInfo->dwStepCount++;
}

BOOL CGame3DEffectEx2::Process(Game3DEffectExInfo* pInfo)
{
	if(!pInfo)
		return true;
	int nAmount = pInfo->nInfoAmount;
	if(nAmount <= 0)
		return true;
	if(pInfo->nState == _GAME3DEFFECT_STATE_ENDED)
		return true;
	if(pInfo->dwPauseTime != 0)
		return false;
	DWORD dwTimePass = ::TimeGet() - pInfo->dwTimeBegin;
	if(dwTimePass <= pInfo->dwTimeDelay)
	{
		pInfo->nState = _GAME3DEFFECT_STATE_DELAY;
		return false;
	}
	if(pInfo->nState == _GAME3DEFFECT_STATE_NEW || pInfo->nState == _GAME3DEFFECT_STATE_DELAY)
	{
		pInfo->nState = _GAME3DEFFECT_STATE_PLAYING;
		if(m_pHandler)
			m_pHandler->OnStart(pInfo->nId);
	}
	dwTimePass -= pInfo->dwTimeDelay;
	DWORD dwMaxFrameAmount = 0;
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectIdInfo* pIdInfo = pInfo->pIDInfo+i;
		if(pIdInfo)
		{
			I3DEffect* pEffect = GameDataSetQuery()->Get3DEffect(pIdInfo->idEffect, 0, 0);
			if(!pEffect)
			{
				if(dwMaxFrameAmount == 0)
					dwMaxFrameAmount = 100;
				continue;
			}
			if(!pIdInfo->bReady)
			{
				pEffect->ResetShape();
				pIdInfo->nShapes = pEffect->GetShapeAmount();
				if(!pIdInfo->pShapeInfoBuf)
					pIdInfo->pShapeInfoBuf = pEffect->CreateShapeInfo();
				pEffect->BackupShapeInfo(pIdInfo->pShapeInfoBuf);
				pIdInfo->pPtclInfo = pEffect->CreatePtclInfo();
				pEffect->SetPtclInfo(pIdInfo->pPtclInfo);
				pEffect->SetPtcl3Info(pIdInfo->pPtcl3Info);
				pEffect->SetPtcl3Gravity(pInfo->fGravity[0], pInfo->fGravity[1], pInfo->fGravity[2]);
				pEffect->SetPtcl3ExternalSpeed(pInfo->fExternalSpeed[0], pInfo->fExternalSpeed[1], pInfo->fExternalSpeed[2]);
				pIdInfo->bReady = TRUE;
			}
			if(dwMaxFrameAmount < pEffect->GetFrames())
				dwMaxFrameAmount = pEffect->GetFrames();
		}
	}
	DWORD dwAllTime = pInfo->dwLoopTime * (pInfo->dwFrameInterval*dwMaxFrameAmount+pInfo->dwLoopInterval)-pInfo->dwLoopInterval;
	if(dwTimePass > dwAllTime)
	{
		pInfo->nState = _GAME3DEFFECT_STATE_ENDED;
		if(m_pHandler)
			m_pHandler->OnEnd(pInfo->nId);
		return true;
	}
	DWORD dwData = (pInfo->dwFrameInterval*dwMaxFrameAmount+pInfo->dwLoopInterval);
	if(dwData < 1)
		dwData = 1;
	DWORD dwTimePassThisLoop = dwTimePass % dwData;
	int nFrameIndex = dwTimePassThisLoop/pInfo->dwFrameInterval;
	if(nFrameIndex < (int)dwMaxFrameAmount)
	{
		if(pInfo->nState == _GAME3DEFFECT_STATE_LOOPGAP)
		{
			pInfo->nState = _GAME3DEFFECT_STATE_PLAYING;
			if(m_pHandler)
				m_pHandler->OnLoop(pInfo->nId);
		}
		pInfo->dwCurrentFrameIndex = nFrameIndex;
	}
	else if(pInfo->nState == _GAME3DEFFECT_STATE_PLAYING)
	{
		pInfo->nState = _GAME3DEFFECT_STATE_LOOPGAP;
		if(m_pHandler)
			m_pHandler->OnLoopGap(pInfo->nId);
	}
	return false;
}

void CGame3DEffectEx2::DeleteInfo(Game3DEffectExInfo* pInfo)
{
	if(m_pHandler)
		m_pHandler->OnDelete(pInfo->nId);
	delete[] pInfo->pIDInfo;
	pInfo->pIDInfo = NULL;
	SAFE_DELETE(pInfo);
}

BOOL CGame3DEffectEx2::Process()
{
	int nAmount = m_setInfo.size();
	for(int i = nAmount-1; i >= 0; i --)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo)
		{
			if(this->Process(pInfo) && pInfo->bAutoDelete)
			{
				this->DeleteInfo(pInfo);
				m_setInfo.erase(m_setInfo.begin()+i);
			}
		}
	}
	return m_setInfo.size() == 0;
}

void CGame3DEffectEx2::Clear()
{
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(m_pHandler)
			m_pHandler->OnDelete(pInfo->nId);
		if(pInfo)
		{
			delete[] pInfo->pIDInfo;
			pInfo->pIDInfo = NULL;
			SAFE_DELETE(pInfo);
			m_setInfo[i] = NULL;
		}
	}
	m_setInfo.clear();
	m_fScaleX = m_fScaleY = m_fScaleZ = 1.0;
	m_nOffsetY = 0;
	m_Height = 0;
	m_bOblique = true;
	m_fX = m_fY = m_fZ = 0.0;
	m_fPosX = m_fPosY = m_fPosZ = 0.0;
}

void CGame3DEffectEx2::DeleteByName(const char* pszIndex)
{
	if(!pszIndex)
		return;
	int nAmount = m_setInfo.size();
	for(int i= 0; i< nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && 0 == strcmp(pszIndex, pInfo->szIndex))
		{
			if(m_pHandler)
				m_pHandler->OnDelete(pInfo->nId);
			m_setInfo.erase(m_setInfo.begin()+i);
			delete[] pInfo->pIDInfo;
			pInfo->pIDInfo = NULL;
			SAFE_DELETE(pInfo);
			return;
		}
	}
}

void CGame3DEffectEx2::Delete(int nId)
{
	int nAmount = m_setInfo.size();
	for(int i= 0; i< nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if((pInfo && pInfo->nId == nId) || nId == -1)
		{
			if(m_pHandler)
				m_pHandler->OnDelete(pInfo->nId);
			m_setInfo.erase(m_setInfo.begin()+i);
			if(pInfo)
			{
				delete[] pInfo->pIDInfo;
				pInfo->pIDInfo = NULL;
				SAFE_DELETE(pInfo);
			}
			return;
		}
	}
}

int CGame3DEffectEx2::Add(const char* pszIndex, bool bAutoDelete)
{
	if(!pszIndex)
		return -1;
	if(_stricmp(pszIndex, "none") == 0)
		return -1;
	const CMy3DEffectInfo* pEffectInfo = GameDataSetQuery()->GetMy3DEffectInfo(pszIndex);
	if (!pEffectInfo)
		return -1;
	Game3DEffectExInfo* pInfo = new Game3DEffectExInfo;
	MYASSERT (pInfo);
	memset(pInfo, 0L, sizeof(Game3DEffectExInfo));
	this->BuildInfo(pInfo, pEffectInfo);
	strcpy(pInfo->szIndex, pszIndex);
	pInfo->bAutoDelete	= bAutoDelete;
	pInfo->nOffsetX		= pEffectInfo->nOffsetX;
	pInfo->nOffsetY		= pEffectInfo->nOffsetY;
	pInfo->nOffsetZ		= pEffectInfo->nOffsetZ;
	pInfo->nId = s_nNextId++;
	m_setInfo.push_back(pInfo);
	return pInfo->nId;
}

void CGame3DEffectEx2::BuildInfo(Game3DEffectExInfo* pInfo, const CMy3DEffectInfo* pEffectInfo)
{
	pInfo->nInfoAmount = pEffectInfo->nAmount;
	pInfo->pIDInfo = new Game3DEffectIdInfo[pInfo->nInfoAmount];
	for(int i = 0; i < pInfo->nInfoAmount; i++)
	{
		Game3DEffectIdInfo* pIDInfo = pInfo->pIDInfo+i;
		memset(pIDInfo, 0L, sizeof(Game3DEffectIdInfo));
		pIDInfo->idEffect		= pEffectInfo->idEffect[i];
		pIDInfo->idTexture		= pEffectInfo->idTexture[i];
		pInfo->nASB[i]			= pEffectInfo->nASB[i];
		pInfo->nADB[i]			= pEffectInfo->nADB[i];
		pInfo->nScale[i]		= pEffectInfo->nScale[i];
		I3DEffect* pEffect = GameDataSetQuery()->Get3DEffect(pIDInfo->idEffect, 0, 0);
		if(pEffect)
		{
			pEffect->ResetShape();
			pIDInfo->nShapes = pEffect->GetShapeAmount();
			if(!pIDInfo->pShapeInfoBuf)
				pIDInfo->pShapeInfoBuf = pEffect->CreateShapeInfo();
			pEffect->BackupShapeInfo(pIDInfo->pShapeInfoBuf);
			pIDInfo->pPtclInfo = pEffect->CreatePtclInfo();
			pEffect->SetPtclInfo(pIDInfo->pPtclInfo);
			pIDInfo->pPtcl3Info = pEffect->CreatePtcl3Info();
			pEffect->SetPtcl3Info(pIDInfo->pPtcl3Info);
		}
	}
	pInfo->dwTimeDelay	= pEffectInfo->nDelay;
	pInfo->dwLoopTime	= pEffectInfo->nLoopTime;
	pInfo->dwFrameInterval	= pEffectInfo->nFrameInterval;
	pInfo->dwLoopInterval	= pEffectInfo->nLoopInterval;
	pInfo->nState		= _GAME3DEFFECT_STATE_NEW;
	pInfo->dwColor		= 0xFFFFFFFF;
	pInfo->fPosX = pInfo->fPosY = pInfo->fPosZ = 0.0;
	pInfo->fScaleX = pInfo->fScaleY = pInfo->fScaleZ = 1.0;
	pInfo->fRotateX = pInfo->fRotateY = pInfo->fRotateZ = 0.0;
	pInfo->dwTimeBegin = ::TimeGet();
}

BOOL CGame3DEffectEx2::Reload()
{
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(!pInfo)
			continue;
		const CMy3DEffectInfo* pEffectInfo = GameDataSetQuery()->GetMy3DEffectInfo(pInfo->szIndex);
		if(!pEffectInfo)
			continue;
		delete[] pInfo->pIDInfo;
		pInfo->pIDInfo = NULL;
		this->BuildInfo(pInfo, pEffectInfo);
	}
	return true;
}

void CGame3DEffectEx2::SetPtcl3Gravity(float x, float y, float z, int nId)
{
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			pInfo->fGravity[0] = x;
			pInfo->fGravity[1] = y;
			pInfo->fGravity[2] = z;
			if(nId != -1)
				return;
		}
	}
}

void CGame3DEffectEx2::SetPtcl3ExternalSpeed(float x, float y, float z, int nId)
{
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			pInfo->fExternalSpeed[0] = x;
			pInfo->fExternalSpeed[1] = y;
			pInfo->fExternalSpeed[2] = z;
			if(nId != -1)
				return;
		}
	}
}

void CGame3DEffectEx2::SetTexture(OBJID idTexture, int nId, int nPart)
{
	if((int) idTexture < 0)
		return;
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			if(nPart < 0 || nPart >= pInfo->nInfoAmount)
			{
				for(int j = 0; j < pInfo->nInfoAmount; j++)
					pInfo->pIDInfo[j].idTexture = idTexture;
			}
			else
				pInfo->pIDInfo[nPart].idTexture = idTexture;
		}
	}
}

void CGame3DEffectEx2::EverPlay(int nId)
{
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo && (nId == -1 || pInfo->nId == nId))
		{
			pInfo->dwLoopTime = 99999999;
		}
	}
}

BOOL CGame3DEffectEx2::TestEffect(const char* pszIndex)
{
	if(!pszIndex)
		return false;
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo)
		{
			if(strcmp(pInfo->szIndex, pszIndex) == 0)
				return true;
		}
	}
	return false;
}

int CGame3DEffectEx2::GetId(const char* pszIndex)
{
	if(!pszIndex)
		return -1;
	int nAmount = m_setInfo.size();
	for(int i = 0; i < nAmount; i++)
	{
		Game3DEffectExInfo* pInfo = m_setInfo[i];
		if(pInfo)
		{
			if(strcmp(pInfo->szIndex, pszIndex) == 0)
				return pInfo->nId;
		}
	}
	return -1;
}
