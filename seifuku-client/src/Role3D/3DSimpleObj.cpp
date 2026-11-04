#include "3DSimpleObj.h"
#include "GameDataSet.h"

C3DSimpleObj::C3DSimpleObj()
{
	m_nPosX = m_nPosY = 0;
	m_nRotate = 0;
	m_nObliqueAngle = g_nAdjustSimpleObj;
	if(m_nObliqueAngle == _ADJUST_UNSET)
		m_nObliqueAngle = CMyBitmap::GetObliqueAngle();
	m_vScale.x = m_vScale.y = m_vScale.z = 1.0f;
	m_vRotate.x = m_vRotate.y = m_vRotate.z = 0;
	m_LightOffset.x = 0;
	m_LightOffset.y = 0;
	m_LightOffset.z = 1000;
	m_nShadowType = 2;
	m_a = 0.3f;
	m_r = 0;
	m_g = 0;
	m_b = 0;
	m_bOblique = true;
	m_fPosH = 0;
	m_p3DRolePart = NULL;
	m_idType = 0;
}

C3DSimpleObj::~C3DSimpleObj()
{
	SAFE_DELETE(m_p3DRolePart);
}

DWORD C3DSimpleObj::Release()
{
	if (this)
		delete this;
	return 0;
}

I3DSimpleObj* Simple3DObjCreate ( OBJID idType )
{
	C3DSimpleObj* pObj = new C3DSimpleObj;
	if (!pObj)
		return NULL;
	if (idType && !pObj->Create(idType))
	{
		delete pObj;
		return NULL;
	}
	return pObj;
}

I3DSimpleObj* Simple3DObjCreate2 ( OBJID idMesh, OBJID idTexture, OBJID idMixTex, int nUnused, int nAsb, int nAdb )
{
	C3DSimpleObj* pObj = new C3DSimpleObj;
	if (!pObj)
		return NULL;
	if (!pObj->Create2(idMesh, idTexture, idMixTex, nUnused, nAsb, nAdb))
	{
		delete pObj;
		return NULL;
	}
	return pObj;
}

void C3DSimpleObj::Rotate(float x, float y, float z)
{
	m_vRotate.x = x;
	m_vRotate.y = y;
	m_vRotate.z = z;
}

void C3DSimpleObj::Scale(float x, float y, float z)
{
	m_vScale.x = x;
	m_vScale.y = y;
	m_vScale.z = z;
}

void C3DSimpleObj::SetPos(int nX, int nY, int nHeight, int nRotate, float fScale)
{
	m_nPosX = nX;
	m_nPosY = nY;
	m_nRotate = nRotate;
	m_vScale.x = m_vScale.y = m_vScale.z = fScale;
	m_nPosH = nHeight*2;
}

BOOL C3DSimpleObj::Draw2BG(int nMapX, int nMapY, int nAsb, int nAdb)
{
	IF_NOT (m_p3DRolePart)
		return false;
	if(!m_p3DRolePart->IsLoaded())
		return false;
	CMyBitmap::PrepareModle();
	D3DXMATRIXA16 matrix;
	D3DXMatrixIdentity ( &matrix );
	CMyBitmap::Common_Scale ( &matrix, m_vScale.x, m_vScale.y, m_vScale.z );
	CMyBitmap::Common_Rotate ( &matrix, m_vRotate.x, m_vRotate.y, m_vRotate.z );
	CMyBitmap::Common_Translate ( &matrix, 0, 0, (float)m_nPosH );
	CMyBitmap::Common_Rotate ( &matrix, 0, 0, D3DXToRadian ( ( float )m_nRotate ) );
	D3DXMATRIXA16 matrixLocal = matrix;
	if(m_bOblique)
		CMyBitmap::Common_Rotate ( &matrix, D3DXToRadian ( (float)m_nObliqueAngle ), 0, 0 );
	CMyBitmap::Common_Translate ( &matrix, (float)m_nPosX, m_fPosH, (float)m_nPosY );
	m_p3DRolePart->MuliplyAll(&matrix);
	D3DXMATRIXA16 matrixWorld;
	D3DXMatrixIdentity ( &matrixWorld );
	if(m_bOblique)
		CMyBitmap::Common_Rotate ( &matrixWorld, D3DXToRadian ( (float)m_nObliqueAngle ), 0, 0 );
	CMyBitmap::Common_Translate ( &matrixWorld, (float)m_nPosX, m_fPosH, (float)m_nPosY );
 	m_p3DRolePart->SetTexture(NULL, NULL);
	m_p3DRolePart->MuliplyPhy(&matrix);
	D3DXMATRIXA16 matrixAdjusted;
	D3DXMatrixMultiply(&matrixAdjusted, m_p3DRolePart->GetMatrixAdjust(), &matrix);
	*m_p3DRolePart->GetMatrixWorld() = matrixAdjusted;
	m_p3DRolePart->SetPhyMatrix(&matrixLocal, &matrixWorld);
	m_p3DRolePart->Draw(m_nShadowType, m_LightOffset.x, m_LightOffset.y, m_LightOffset.z, m_a, m_r, m_g, m_b, nAsb, nAdb);
	m_p3DRolePart->ClearMatrix();
	return true;
}

BOOL C3DSimpleObj::HitTest(int nHitScreenX, int nHitScreenY, int nMapX, int nMapY)
{
	IF_NOT (m_p3DRolePart)
		return false;
	if(!m_p3DRolePart->IsLoaded())
		return false;
	float br = D3DXToRadian ( ( float )( m_nRotate % 45 ) );
	D3DXMATRIXA16 matrix;
	D3DXMatrixIdentity ( &matrix );
	CMyBitmap::Common_Rotate ( &matrix, 0, 0, br );
	CMyBitmap::Common_Scale ( &matrix, m_vScale.x - m_vScale.x * 0.2f, m_vScale.y - m_vScale.y * 0.2f, m_vScale.z - m_vScale.z * 0.3f );
	CMyBitmap::Common_Translate ( &matrix, (float)m_nPosX, m_fPosH, (float)m_nPosY );
	D3DXVECTOR3 min, max;
	m_p3DRolePart->TransformCoordforSimpleObj(&min, &max, &matrix);
	float temp;
	if ( min.x > max.x )
	{
		temp = min.x;
		min.x = max.x;
		max.x = temp;
	}
	if ( min.y > max.y )
	{
		temp = min.y;
		min.y = max.y;
		max.y = temp;
	}
	if ( min.z > max.z )
	{
		temp = min.z;
		min.z = max.z;
		max.z = temp;
	}
	D3DXVECTOR3 from, dir;
	CMyBitmap::ScrRayBuild ( nHitScreenX, nHitScreenY, from, dir );
	return D3DXBoxBoundProbe ( &min, &max, &from, &dir );
}

void C3DSimpleObj::SetLightOffset(int type, float x, float y, float z, float a, float r, float g, float b)
{
	m_nShadowType = type;
	m_LightOffset.x = x;
	m_LightOffset.y = -y;
	m_LightOffset.z = -z;
	m_a = a;
	m_r = r;
	m_g = g;
	m_b = b;
}

BOOL C3DSimpleObj::Create(OBJID idType)
{
	SAFE_DELETE(m_p3DRolePart);
	C3DSimpleObjExInfo* pInfoEx = GameDataSetQuery()->Get3DSimpleObjExInfo(idType);
	if(pInfoEx)
	{
		m_p3DRolePart = new C3DRolePart;
		IF_NOT (m_p3DRolePart)
			return false;
		RolePartInfo* pPartInfo = m_p3DRolePart->GetInfo();
		memset(pPartInfo, 0, sizeof(RolePartInfo));
		pPartInfo->nPart = pInfoEx->part.nPart;
		for(int i = 0; i < pInfoEx->part.nPart; i++)
		{
			pPartInfo->idMesh[i] = pInfoEx->part.idMesh[i];
			pPartInfo->idTexture[i] = pInfoEx->part.idTexture[i];
			pPartInfo->idMixTex[i] = pInfoEx->part.idMixTex[i];
			pPartInfo->nAsb[i] = pInfoEx->part.nAsb[i];
			pPartInfo->nAdb[i] = pInfoEx->part.nAdb[i];
			pPartInfo->nMixOpt[i] = pInfoEx->part.nMixOpt[i];
			pPartInfo->pMaterial[i] = pInfoEx->part.pMaterial[i];
		}
		return true;
	}
	C3DSimpleObjInfo* pInfo = GameDataSetQuery()->Get3DSimpleObjInfo(idType);
	if(!pInfo || pInfo->nParts <= 0)
		return false;
	m_p3DRolePart = new C3DRolePart;
	IF_NOT (m_p3DRolePart)
		return false;
	RolePartInfo* pPartInfo = m_p3DRolePart->GetInfo();
	memset(pPartInfo, 0, sizeof(RolePartInfo));
	pPartInfo->nPart = pInfo->nParts;
	for(int i = 0; i < pInfo->nParts; i++)
	{
		pPartInfo->idMesh[i] = pInfo->idMesh[i];
		pPartInfo->idTexture[i] = pInfo->idTexture[i];
		pPartInfo->nAsb[i] = 5;
		pPartInfo->nAdb[i] = 6;
	}
	m_idType = idType;
	return true;
}

BOOL C3DSimpleObj::Reload()
{
	IF_NOT (m_p3DRolePart)
		return false;
	if(m_idType == 0)
		return true;
	C3DSimpleObjExInfo* pInfoEx = GameDataSetQuery()->Get3DSimpleObjExInfo(m_idType);
	if(pInfoEx)
	{
		RolePartInfo* pPartInfo = m_p3DRolePart->GetInfo();
		memset(pPartInfo, 0, sizeof(RolePartInfo));
		pPartInfo->nPart = pInfoEx->part.nPart;
		for(int i = 0; i < pInfoEx->part.nPart; i++)
		{
			pPartInfo->idMesh[i] = pInfoEx->part.idMesh[i];
			pPartInfo->idTexture[i] = pInfoEx->part.idTexture[i];
			pPartInfo->idMixTex[i] = pInfoEx->part.idMixTex[i];
			pPartInfo->nAsb[i] = pInfoEx->part.nAsb[i];
			pPartInfo->nAdb[i] = pInfoEx->part.nAdb[i];
			pPartInfo->nMixOpt[i] = pInfoEx->part.nMixOpt[i];
			pPartInfo->pMaterial[i] = pInfoEx->part.pMaterial[i];
		}
		return true;
	}
	C3DSimpleObjInfo* pInfo = GameDataSetQuery()->Get3DSimpleObjInfo(m_idType);
	if(!pInfo || pInfo->nParts <= 0)
		return false;
	RolePartInfo* pPartInfo = m_p3DRolePart->GetInfo();
	memset(pPartInfo, 0, sizeof(RolePartInfo));
	pPartInfo->nPart = pInfo->nParts;
	for(int i = 0; i < pInfo->nParts; i++)
	{
		pPartInfo->idMesh[i] = pInfo->idMesh[i];
		pPartInfo->idTexture[i] = pInfo->idTexture[i];
		pPartInfo->nAsb[i] = 5;
		pPartInfo->nAdb[i] = 6;
	}
	return true;
}

BOOL C3DSimpleObj::Create2(OBJID idMesh, OBJID idTexture, OBJID idMixTex, int nUnused, int nAsb, int nAdb)
{
	SAFE_DELETE(m_p3DRolePart);
	m_p3DRolePart = new C3DRolePart;
	IF_NOT (m_p3DRolePart)
		return false;
	RolePartInfo* pPartInfo = m_p3DRolePart->GetInfo();
	memset(pPartInfo, 0, sizeof(RolePartInfo));
	pPartInfo->nPart = 1;
	pPartInfo->idMesh[0] = idMesh;
	pPartInfo->idTexture[0] = idTexture;
	pPartInfo->idMixTex[0] = idMixTex;
	pPartInfo->nMixOpt[0] = 2;
	pPartInfo->nAsb[0] = nAsb;
	pPartInfo->nAdb[0] = nAdb;
	m_idType = 0;
	return true;
}

void C3DSimpleObj::SetMotionById(OBJID idMotion)
{
	IF_SUC (m_p3DRolePart)
	{
		I3DMotion* pMotion = GameDataSetQuery()->Get3DMotion("body", idMotion);
		m_p3DRolePart->SetMotion(pMotion);
	}
}

void C3DSimpleObj::SetARGB(float alpha, float red, float green, float blue)
{
	IF_SUC (m_p3DRolePart)
		m_p3DRolePart->SetARGB(alpha, red, green, blue);
}

void C3DSimpleObj::SetTexture(I3DTexture* pTexture, I3DTexture* pMixTexture, int nMixOpt)
{
	IF_NOT (m_p3DRolePart)
		return;
	if(pTexture)
	{
		m_p3DRolePart->SetTexture(pTexture, pMixTexture);
		m_p3DRolePart->SetDrawType(nMixOpt);
	}
}

void C3DSimpleObj::SetFrame(DWORD dwFrame)
{
	IF_SUC (m_p3DRolePart)
		m_p3DRolePart->SetFrame(dwFrame);
}

void C3DSimpleObj::SetMotion(I3DMotion* pMotion)
{
	IF_SUC (m_p3DRolePart)
		m_p3DRolePart->SetMotion(pMotion);
}

void C3DSimpleObj::AddEffect(const char* pszIndex)
{
	IF_SUC (m_p3DRolePart)
		m_p3DRolePart->AddEffect(pszIndex, FALSE);
}
