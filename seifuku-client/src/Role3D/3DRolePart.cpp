#include "3DRolePart.h"
#include "GameDataSet.h"

const OBJID ID_NONE = 0;

#define MeshOf(info, i)	(this->GetLodObj((info).idMesh[i]))

C3DRolePart::C3DRolePart()
	: m_objGame3DEffectEx(NULL)
{
	m_idType = ID_NONE;
	m_nAction = -1;
	m_bTextureSet = FALSE;
	memset(&m_infoPart, 0, sizeof(m_infoPart));
	D3DXMatrixIdentity(&m_matrixLocal);
	D3DXMatrixIdentity(&m_matrixWorld);
	D3DXMatrixIdentity(&m_matrixAdjust);
	m_nLodLevel = 0;
}

I3DObj* C3DRolePart::GetLodObj(OBJID idMesh)
{
	OBJID idLod = GameDataSetQuery()->GetMeshLod(idMesh, m_nLodLevel);
	return GameDataSetQuery()->Get3DObj(idLod, 0, 0);
}

C3DRolePart::~C3DRolePart()
{
	m_objGame3DEffectEx.Clear();
}

BOOL C3DRolePart::Create(const char* pszKind, OBJID id, const D3DXMATRIXA16* pMatrix)
{
	RolePartInfo* pInfo = GameDataSetQuery()->GetRolePartInfo(pszKind, id);
	if(!pInfo)
		return false;
	m_infoPart = *pInfo;
	m_idType = id;
	if(pMatrix)
		m_matrixAdjust = *pMatrix;
	return true;
}

void C3DRolePart::SetMatrix(const D3DXMATRIXA16* pMatrix)
{
	if(pMatrix)
		m_matrixAdjust = *pMatrix;
	else
		D3DXMatrixIdentity(&m_matrixAdjust);
}

BOOL C3DRolePart::Reload(const char* pszKind)
{
	RolePartInfo* pInfo = GameDataSetQuery()->GetRolePartInfo(pszKind, m_idType);
	if(pInfo)
	{
		m_infoPart = *pInfo;
		return m_objGame3DEffectEx.Reload();
	}
	if(m_idType == 0)
		return m_objGame3DEffectEx.Reload();
	return false;
}

BOOL C3DRolePart::IsLoaded()
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		if(!GameDataSetQuery()->Get3DTexture(m_infoPart.idTexture[i], 0, 0))
			return false;
		if(!MeshOf(m_infoPart, i))
			return false;
	}
	return true;
}

BOOL C3DRolePart::AddPart(const char* pszKind, OBJID id)
{
	RolePartInfo* pInfo = GameDataSetQuery()->GetRolePartInfo(pszKind, id);
	if(!pInfo)
		return false;
	int nPart = m_infoPart.nPart;
	if(nPart + pInfo->nPart > _MAXPART_ROLEPART)
		return false;
	for(int i = 0; i < pInfo->nPart; i++)
	{
		m_infoPart.idMesh[nPart+i]		= pInfo->idMesh[i];
		m_infoPart.idTexture[nPart+i]	= pInfo->idTexture[i];
		m_infoPart.idMixTex[nPart+i]	= pInfo->idMixTex[i];
		m_infoPart.nMixOpt[nPart+i]		= pInfo->nMixOpt[i];
		m_infoPart.pMaterial[nPart+i]	= pInfo->pMaterial[i];
		m_infoPart.nAsb[nPart+i]		= pInfo->nAsb[i];
		m_infoPart.nAdb[nPart+i]		= pInfo->nAdb[i];
	}
	m_infoPart.nPart += pInfo->nPart;
	return true;
}

BOOL C3DRolePart::ReplacePart(const char* pszKind, OBJID id)
{
	RolePartInfo* pInfo = GameDataSetQuery()->GetRolePartInfo(pszKind, id);
	if(!pInfo)
		return false;
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		for(int j = 0; j < pInfo->nPart; j++)
		{
			if(m_infoPart.idMesh[i] == pInfo->idMesh[j] && m_infoPart.idTexture[i] == pInfo->idTexture[j])
			{
				m_infoPart.idMesh[i] = 0;
				m_infoPart.idTexture[i] = 0;
				break;
			}
		}
	}
	RolePartInfo infoOld = m_infoPart;
	int nPart = 0;
	for(int i = 0; i < infoOld.nPart; i++)
	{
		if(infoOld.idMesh[i])
		{
			m_infoPart.idMesh[nPart]	= infoOld.idMesh[i];
			m_infoPart.idTexture[nPart]	= infoOld.idTexture[i];
			m_infoPart.idMixTex[nPart]	= infoOld.idMixTex[i];
			m_infoPart.nMixOpt[nPart]	= infoOld.nMixOpt[i];
			m_infoPart.pMaterial[nPart]	= infoOld.pMaterial[i];
			m_infoPart.nAsb[nPart]		= infoOld.nAsb[i];
			m_infoPart.nAdb[nPart]		= infoOld.nAdb[i];
			nPart++;
		}
	}
	m_infoPart.nPart = nPart;
	return true;
}

void C3DRolePart::Move(float x, float y, float z)
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
			p3DMesh->Move(x, y, z);
	}
}

void C3DRolePart::Rotate(float x, float y, float z)
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
			p3DMesh->Rotate(x, y, z);
	}
}

void C3DRolePart::Scale(float x, float y, float z)
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
			p3DMesh->Scale(x, y, z);
	}
}

void C3DRolePart::SetARGB(float alpha, float red, float green, float blue)
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
			p3DMesh->SetARGB (alpha, red, green, blue);
	}
}

void C3DRolePart::SetFrame(DWORD dwFrame)
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
			p3DMesh->SetFrame(dwFrame);
	}
}

void C3DRolePart::SetBlendFrame(DWORD dwFrame)
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
			p3DMesh->SetBlendFrame(dwFrame);
	}
}

void C3DRolePart::Draw(int type, float lightx, float lighty, float lightz, float sa, float sr, float sg, float sb, int nAsb, int nAdb)
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		if(!GameDataSetQuery()->Get3DTexture(m_infoPart.idTexture[i], 0, 0))
			return;
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
		{
			p3DMesh->SetDrawType(m_infoPart.nMixOpt[i]);
			int nMyAsb = nAsb == -1 ? m_infoPart.nAsb[i] : nAsb;
			int nMyAdb = nAdb == -1 ? m_infoPart.nAdb[i] : nAdb;
			p3DMesh->Draw(type, lightx, lighty, lightz, sa, sr, sg, sb, nMyAsb, nMyAdb);
		}
	}
	m_bTextureSet = FALSE;
	m_objGame3DEffectEx.Process();
	m_objGame3DEffectEx.ShowPart(this);
}

void C3DRolePart::Clear3DEffect(const char* szIndex)
{
	if(!szIndex)
		m_objGame3DEffectEx.Clear();
	else
		m_objGame3DEffectEx.DeleteByName(szIndex);
}

void C3DRolePart::AddEffect(const char* pszIndex, BOOL bForce)
{
	if(!pszIndex)
		return;
	if(_stricmp(pszIndex, "none") == 0)
	{
		m_objGame3DEffectEx.Clear();
		return;
	}
	if(!bForce && m_objGame3DEffectEx.TestEffect(pszIndex))
		return;
	m_objGame3DEffectEx.Add(pszIndex, true);
}

BOOL C3DRolePart::TestEffect(const char* pszIndex)
{
	if(!pszIndex)
		return false;
	return m_objGame3DEffectEx.TestEffect(pszIndex) == 1;
}

void C3DRolePart::SetMotion(I3DMotion* pMotion)
{
	if(!pMotion)
	{
		this->SetDefaultMotion();
		return;
	}
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
			p3DMesh->SetMotion(pMotion);
	}
}

void C3DRolePart::SetBlendMotion(I3DMotion* pMotion, float fWeight)
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
			p3DMesh->SetBlendMotion(pMotion, fWeight);
	}
}

BOOL C3DRolePart::HitTest(const D3DXVECTOR3* pvFrom, const D3DXVECTOR3* pvDir)
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(!p3DMesh)
			continue;
		for(DWORD n = 0; n < p3DMesh->GetPhyAmount(); n++)
		{
			if(p3DMesh->IsPhyHidden(n))
				continue;
			D3DXMATRIXA16 matrix;
			D3DXVECTOR3 vFrom, vDir, vMin, vMax;
			D3DXMatrixInverse(&matrix, NULL, &m_matrixWorld);
			D3DXVec3TransformCoord(&vFrom, pvFrom, &matrix);
			D3DXVec3TransformNormal(&vDir, pvDir, &matrix);
			p3DMesh->GetBoundingBox(n, &vMin, &vMax);
			if(D3DXBoxBoundProbe(&vMin, &vMax, &vFrom, &vDir))
				return true;
		}
	}
	return false;
}

void C3DRolePart::TransformCoordforSimpleObj(D3DXVECTOR3* pobjMin, D3DXVECTOR3* pobjMax, D3DXMATRIXA16* pobjMatrix)
{
	IF_NOT (pobjMin && pobjMax && pobjMatrix)
		return;
	I3DObj* p3DMesh = MeshOf(m_infoPart, 0);
	if(p3DMesh)
	{
		D3DXVECTOR3 vMin, vMax;
		p3DMesh->GetBoundingBox ( 0, &vMin, &vMax );
		D3DXVec3TransformCoord (pobjMin,
			&vMin,
								 pobjMatrix);
		D3DXVec3TransformCoord (pobjMax,
			&vMax,
								 pobjMatrix);
	}
}

C3MotionTracker C3DRolePart::GetVirtualMotion(const char* pszName)
{
	C3MotionTracker tracker;
	tracker.lpMotion = NULL;
	tracker.lpBlendMotion = NULL;
	if(!pszName)
		return tracker;
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
		{
			int nIndex = p3DMesh->GetIndexByName((char*)pszName);
			if (-1 != nIndex)
			{
				tracker.lpMotion = p3DMesh->GetMotion(nIndex);
				tracker.lpBlendMotion = p3DMesh->GetBlendMotion(nIndex);
				return tracker;
			}
		}
	}
	return tracker;
}

void C3DRolePart::GetBoneMatrix(D3DXMATRIXA16* pobjOut, int nMesh, int nPhy)
{
	if(m_infoPart.nPart <= 0)
	{
		*pobjOut = m_matrixLocal;
		return;
	}
	D3DXMATRIXA16 matrix;
	D3DXMatrixIdentity(&matrix);
	I3DObj* p3DMesh = MeshOf(m_infoPart, nMesh % m_infoPart.nPart);
	if(p3DMesh && p3DMesh->GetPhyAmount() > 0)
	{
		D3DXMATRIXA16* pMatrix = p3DMesh->GetBoneMatrix((DWORD)nPhy % p3DMesh->GetPhyAmount(), 0);
		D3DXMatrixMultiply(&matrix, &m_matrixAdjust, pMatrix);
	}
	*pobjOut = matrix;
}

void C3DRolePart::MuliplyPhy(D3DXMATRIXA16* pobjMatrix)
{
	D3DXMatrixMultiply(&m_matrixLocal, &m_matrixAdjust, pobjMatrix);
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(!p3DMesh)
			continue;
		for(DWORD n = 0; n < p3DMesh->GetPhyAmount(); n++)
			*p3DMesh->GetPhyMatrix(n) = m_matrixLocal;
	}
}

void C3DRolePart::MuliplyAll(D3DXMATRIXA16* pobjMatrix)
{
	if(!pobjMatrix || m_infoPart.nPart <= 0)
		return;
	I3DObj* p3DMesh = MeshOf(m_infoPart, 0);
	if(p3DMesh)
	{
		D3DXMATRIXA16 matrix;
		D3DXMatrixMultiply(&matrix, &m_matrixAdjust, pobjMatrix);
		p3DMesh->Muliply ( &matrix );
	}
}

void C3DRolePart::MuliplyMotion(C3Motion* pMotion, C3Motion* pBlendMotion)
{
	if(!pMotion || m_infoPart.nPart <= 0)
		return;
	C3MotionTracker tracker;
	tracker.lpMotion = pMotion;
	tracker.lpBlendMotion = pBlendMotion;
	D3DXMATRIXA16 matrixBlend, matrixMotion;
	CMyBitmap::Motion_GetMatrix_Blend(tracker, 0, &matrixBlend);
	matrixMotion = *::MotionMatrixGet(pMotion, 0);
	I3DObj* p3DMesh = MeshOf(m_infoPart, 0);
	if(p3DMesh)
	{
		D3DXMATRIXA16 matrix, matrixAll;
		D3DXMatrixMultiply(&matrix, &m_matrixAdjust, &matrixBlend);
		D3DXMatrixMultiply(&matrixAll, &matrix, &matrixMotion);
		p3DMesh->Muliply(&matrixAll);
	}
}

void C3DRolePart::SetPhyMatrix(D3DXMATRIXA16* pobjLocal, D3DXMATRIXA16* pobjWorld)
{
	D3DXMATRIXA16 matrixLocal;
	D3DXMatrixMultiply(&matrixLocal, &m_matrixAdjust, pobjLocal);
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
			p3DMesh->SetPhyMatrix(&matrixLocal, pobjWorld);
	}
}

void C3DRolePart::ClearMatrix()
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
			p3DMesh->ClearMatrix();
	}
}

void C3DRolePart::SetTexture(I3DTexture* pTexture, I3DTexture* pMixTexture)
{
	if(!pTexture && !pMixTexture)
	{
		if(m_bTextureSet)
			return;
	}
	else
		m_bTextureSet = TRUE;
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(!p3DMesh)
			continue;
		I3DTexture* pMyTexture = pTexture ? pTexture : GameDataSetQuery()->Get3DTexture(m_infoPart.idTexture[i]);
		p3DMesh->ChangeTexture(pMyTexture, NULL);
		I3DTexture* pMyMixTexture = pMixTexture ? pMixTexture : GameDataSetQuery()->Get3DTexture(m_infoPart.idMixTex[i]);
		p3DMesh->ChangeMixTexture(pMyMixTexture, NULL);
		if(m_infoPart.pMaterial[i])
			p3DMesh->SetMaterial(m_infoPart.pMaterial[i]);
	}
}

void C3DRolePart::SetDrawType(int nType)
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if(p3DMesh)
			p3DMesh->SetDrawType(nType);
	}
}

void C3DRolePart::SetDefaultMotion(void)
{
	for(int i = 0; i < m_infoPart.nPart; i++)
	{
		I3DObj* p3DMesh = MeshOf(m_infoPart, i);
		if (p3DMesh)
		{
			C3DObjInfo* p3DObjInfo = GameDataSetQuery()->Get3DObjInfo(m_infoPart.idMesh[i]);
			if(p3DObjInfo)
				p3DObjInfo->p3DObj->SetMotion(p3DObjInfo->pDefaultMotion);
		}
	}
}
