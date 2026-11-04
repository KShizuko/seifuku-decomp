#include "3DRoleEx.h"
#include "GameDataSet.h"
#include <stdio.h>

static const char* const s_szNoAttachment = "";

static const char* KindOr(const char* psz) { return psz ? psz : ""; }

C3DRoleEx::C3DRoleEx()
{
	m_ucReserved[0] = m_ucReserved[1] = 0;
	m_ucReserved2 = 0;
	m_a = 0.3f;
	m_r = m_g = m_b = 0;
	m_LightOffset.x = 0;
	m_LightOffset.y = 0;
	m_LightOffset.z = -1000.0f;
	m_nShadowType = 2;
	m_nAsb = -1;
	m_nAdb = -1;
}

C3DRoleEx::~C3DRoleEx()
{
	this->DelPart(NULL);
}

DWORD C3DRoleEx::Release()
{
	if (this)
		delete this;
	return 0;
}

I3DRole* I3DRoleCreate ( void )
{
	C3DRoleEx* pRole = new C3DRoleEx;
	if (!pRole)
		return NULL;
	return pRole;
}

void LoadAdjustConfig ( void )
{
	FILE* fp = fopen("ini/role3d.ini", "r");
	if (fp)
	{
		int nRead = fscanf(fp, "%d %d %d %d\n", &g_nAdjustOblique, &g_nAdjustSimpleObj, &g_nAdjustEffect, &g_nAdjustEffectOrder);
		fclose(fp);
		if (nRead == 4)
			return;
	}
	g_nAdjustOblique = CMyBitmap::GetObliqueAngle();
	g_nAdjustSimpleObj = CMyBitmap::GetObliqueAngle();
	g_nAdjustEffect = CMyBitmap::GetObliqueAngle();
	g_nAdjustEffectOrder = 0;
}

RolePartNode* C3DRoleEx::GetNode(const char* pszKind)
{
	if (!pszKind || !*pszKind)
		return NULL;
	MAP_ROLEPARTNODE::iterator it = m_mapNode.find(pszKind);
	if (it == m_mapNode.end())
		return NULL;
	return it->second;
}

void C3DRoleEx::DeleteNode(RolePartNode* pNode)
{
	if (!pNode)
		return;
	SAFE_DELETE(pNode->pPart);
	delete pNode;
}

void C3DRoleEx::CollectTrackers(RolePartNode* pNode, std::deque<C3MotionTracker>& setTracker)
{
	if (!pNode || pNode->strParent.empty() || pNode->strBone.empty())
		return;
	RolePartNode* pParent = this->GetNode(pNode->strParent.c_str());
	if (!pParent || !pParent->pPart)
		return;
	C3MotionTracker tracker = pParent->pPart->GetVirtualMotion(pNode->strBone.c_str());
	if (!tracker.lpMotion)
		return;
	setTracker.push_front(tracker);
	this->CollectTrackers(pParent, setTracker);
}

BOOL C3DRoleEx::AddPart(const char* pszKind, const char* pszBone, const char* pszParentKind, OBJID id, const D3DXMATRIXA16* pMatrix)
{
	IF_NOT (pszKind)
		return false;
	this->DelPart(pszKind);
	C3DRolePart* pPart = new C3DRolePart;
	IF_NOT (pPart)
		return false;
	BOOL bInfo = pPart->Create(pszKind, id, pMatrix);
	if (!bInfo && id != 0)
	{
		delete pPart;
		return false;
	}
	RolePartNode* pNode = new RolePartNode;
	IF_NOT (pNode)
	{
		delete pPart;
		return false;
	}
	pNode->pPart = pPart;
	pPart->SetLodLevel(m_nLodLevel);
	if (!pszParentKind || _stricmp("none", pszParentKind) == 0 || !pszBone || _stricmp("none", pszBone) == 0)
	{
		pNode->strParent = s_szNoAttachment;
		pNode->strBone = s_szNoAttachment;
	}
	else
	{
		pNode->strParent = pszParentKind;
		pNode->strBone = pszBone;
	}
	m_mapNode[pszKind] = pNode;
	return true;
}

BOOL C3DRoleEx::AddPartMesh(const char* pszKind, const char* pszPartKind, OBJID id)
{
	RolePartNode* pNode = this->GetNode(pszKind);
	if (!pNode || !pNode->pPart)
		return false;
	return pNode->pPart->AddPart(pszPartKind, id);
}

BOOL C3DRoleEx::ReplacePartMesh(const char* pszKind, const char* pszPartKind, OBJID id)
{
	RolePartNode* pNode = this->GetNode(pszKind);
	if (!pNode || !pNode->pPart)
		return false;
	return pNode->pPart->ReplacePart(pszPartKind, id);
}

void C3DRoleEx::DelPart(const char* pszKind)
{
	if (!pszKind)
	{
		for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
			this->DeleteNode(it->second);
		m_mapNode.clear();
		return;
	}
	MAP_ROLEPARTNODE::iterator it = m_mapNode.find(pszKind);
	if (it != m_mapNode.end())
	{
		this->DeleteNode(it->second);
		m_mapNode.erase(it);
	}
}

BOOL C3DRoleEx::HasPart(const char* pszKind)
{
	if (!pszKind)
		return false;
	return m_mapNode.find(pszKind) != m_mapNode.end();
}

C3DRolePart* C3DRoleEx::GetPart(const char* pszKind)
{
	RolePartNode* pNode = this->GetNode(pszKind);
	return pNode ? pNode->pPart : NULL;
}

BOOL C3DRoleEx::IsLoaded(const char* pszKind)
{
	if (!pszKind)
		return false;
	MAP_ROLEPARTNODE::iterator it = m_mapNode.find(pszKind);
	if (it == m_mapNode.end())
		return false;
	RolePartNode* pNode = it->second;
	if (!pNode || !pNode->pPart)
		return false;
	return pNode->pPart->IsLoaded() != 0;
}

BOOL C3DRoleEx::SetPartMatrix(const char* pszKind, const D3DXMATRIXA16* pMatrix)
{
	IF_NOT (pszKind)
		return false;
	RolePartNode* pNode = this->GetNode(pszKind);
	if (!pNode || !pNode->pPart)
		return false;
	pNode->pPart->SetMatrix(pMatrix);
	return true;
}

void C3DRoleEx::SetLodLevel(int nLevel)
{
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second && it->second->pPart)
			it->second->pPart->SetLodLevel(nLevel);
}

BOOL C3DRoleEx::Reload()
{
	BOOL bRet = true;
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second && it->second->pPart)
			bRet &= it->second->pPart->Reload(it->first.c_str());
	return bRet;
}

void C3DRoleEx::SetAttachment(const char* pszKind, const char* pszBone, const char* pszParentKind)
{
	RolePartNode* pNode = this->GetNode(pszKind);
	if (!pNode)
		return;
	if (!pszParentKind || _stricmp("none", pszParentKind) == 0 || !pszBone || _stricmp("none", pszBone) == 0)
	{
		pNode->strParent = s_szNoAttachment;
		pNode->strBone = s_szNoAttachment;
	}
	else
	{
		pNode->strParent = pszParentKind;
		pNode->strBone = pszBone;
	}
}

void C3DRoleEx::Move(float x, float y, float z, const char* pszKind)
{
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second->pPart && (!pszKind || it->first == pszKind))
			it->second->pPart->Move(x, y, z);
}

void C3DRoleEx::Rotate(float x, float y, float z, const char* pszKind)
{
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second->pPart && (!pszKind || it->first == pszKind))
			it->second->pPart->Rotate(x, y, z);
}

void C3DRoleEx::Scale(float x, float y, float z, const char* pszKind)
{
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second->pPart && (!pszKind || it->first == pszKind))
			it->second->pPart->Scale(x, y, z);
}

BOOL C3DRoleEx::HitTest(int nScrX, int nScrY, int nMapX, int nMapY, const char* pszKind)
{
	D3DXVECTOR3 vFrom, vDir;
	CMyBitmap::ScrRayBuild(nScrX, nScrY, vFrom, vDir);
	if (pszKind)
	{
		RolePartNode* pNode = this->GetNode(pszKind);
		if (!pNode || !pNode->pPart)
			return false;
		return pNode->pPart->HitTest(&vFrom, &vDir);
	}
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second->pPart && it->second->pPart->HitTest(&vFrom, &vDir))
			return true;
	return false;
}

void C3DRoleEx::Draw(const char* pszKind, D3DXMATRIXA16* pMatrixBase, D3DXMATRIXA16* pMatrixWorld, bool bClearMatrix)
{
	RolePartNode* pNode = this->GetNode(pszKind);
	if (!pNode)
		return;
	std::deque<C3MotionTracker> setTracker;
	this->CollectTrackers(pNode, setTracker);
	CMyBitmap::PrepareModle();
	C3DRolePart* pPart = pNode->pPart;
	IF_NOT (pPart)
		return;
	pPart->SetTexture(NULL, NULL);
	D3DXMATRIXA16 mBase = *pMatrixBase;
	CMyBitmap::Common_Scale(&mBase, 1.0f, 1.0f, 1.0f);
	D3DXMATRIXA16 matrix, matrixLocal, matrixWorld, matrixBlend0, matrixBlend1;
	switch (setTracker.size())
	{
	case 0:
		D3DXMatrixMultiply(&matrix, pMatrixBase, pMatrixWorld);
		D3DXMatrixMultiply(&matrixWorld, &mBase, pMatrixWorld);
		pPart->MuliplyAll(&matrix);
		pPart->MuliplyPhy(&matrix);
		D3DXMatrixMultiply(pPart->GetMatrixWorld(), pPart->GetMatrixAdjust(), &matrixWorld);
		pPart->SetPhyMatrix(pMatrixBase, pMatrixWorld);
		break;
	case 1:
		{
			C3MotionTracker t0 = setTracker[0];
			pPart->MuliplyMotion(t0.lpMotion, t0.lpBlendMotion);
			CMyBitmap::Motion_GetMatrix_Blend(t0, 0, &matrixBlend0);
			D3DXMatrixMultiply(&matrixLocal, &matrixBlend0, pMatrixBase);
			D3DXMatrixMultiply(&matrix, &matrixLocal, pMatrixWorld);
			pPart->MuliplyPhy(&matrix);
			D3DXMatrixMultiply(&matrixWorld, &matrixBlend0, &mBase);
			D3DXMatrixMultiply(&matrix, &matrixWorld, pMatrixWorld);
			D3DXMatrixMultiply(pPart->GetMatrixWorld(), pPart->GetMatrixAdjust(), &matrix);
			pPart->SetPhyMatrix(&matrixLocal, pMatrixWorld);
		}
		break;
	case 2:
		{
			C3MotionTracker t0 = setTracker[0];
			C3MotionTracker t1 = setTracker[1];
			pPart->MuliplyMotion(t1.lpMotion, t1.lpBlendMotion);
			CMyBitmap::Motion_GetMatrix_Blend(t0, 0, &matrixBlend0);
			CMyBitmap::Motion_GetMatrix_Blend(t1, 0, &matrixBlend1);
			D3DXMatrixMultiply(&matrix, &matrixBlend0, pMatrixBase);
			D3DXMatrixMultiply(&matrixLocal, &matrixBlend1, &matrix);
			D3DXMatrixMultiply(&matrixWorld, &matrixBlend1, ::MotionMatrixGet(t1.lpMotion, 0));
			pPart->MuliplyPhy(&matrixWorld);
			pPart->SetPhyMatrix(&matrixLocal, pMatrixWorld);
		}
		break;
	default:
		if (bClearMatrix)
			pPart->ClearMatrix();
		return;
	}
	pPart->Draw(m_nShadowType, m_LightOffset.x, m_LightOffset.y, m_LightOffset.z, m_a, m_r, m_g, m_b, m_nAsb, m_nAdb);
	if (bClearMatrix)
		pPart->ClearMatrix();
}

void C3DRoleEx::SetMotion(const char* pszKind, I3DMotion* pMotion)
{
	RolePartNode* pNode = this->GetNode(pszKind);
	if (pNode && pNode->pPart)
		pNode->pPart->SetMotion(pMotion);
}

void C3DRoleEx::SetDefaultMotion(const char* pszKind)
{
	RolePartNode* pNode = this->GetNode(pszKind);
	if (pNode && pNode->pPart)
		pNode->pPart->SetDefaultMotion();
}

void C3DRoleEx::SetBlendMotion(const char* pszKind, I3DMotion* pMotion, float fWeight)
{
	RolePartNode* pNode = this->GetNode(pszKind);
	if (pNode && pNode->pPart)
		pNode->pPart->SetBlendMotion(pMotion, fWeight);
}

void C3DRoleEx::ClearMatrix(const char* pszKind)
{
	RolePartNode* pNode = this->GetNode(pszKind);
	if (pNode && pNode->pPart)
		pNode->pPart->ClearMatrix();
}

void C3DRoleEx::SetLightOffset(int type, float x, float y, float z, float a, float r, float g, float b)
{
	if (type < 1)
		type = 1;
	m_nShadowType = type;
	m_LightOffset.x = x;
	m_LightOffset.y = y;
	m_LightOffset.z = z;
	m_a = a;
	m_r = r;
	m_g = g;
	m_b = b;
}

void C3DRoleEx::SetTexture(I3DTexture* pTexture, I3DTexture* pMixTexture, const char* pszKind)
{
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second->pPart && (!pszKind || it->first == pszKind))
			it->second->pPart->SetTexture(pTexture, pMixTexture);
}

void C3DRoleEx::SetDrawType(int nType, const char* pszKind)
{
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second->pPart && (!pszKind || it->first == pszKind))
			it->second->pPart->SetDrawType(nType);
}

void C3DRoleEx::SetARGB(float alpha, float red, float green, float blue)
{
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second->pPart)
			it->second->pPart->SetARGB(alpha, red, green, blue);
}

void C3DRoleEx::SetFrame(DWORD dwFrame)
{
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second->pPart)
			it->second->pPart->SetFrame(dwFrame);
}

void C3DRoleEx::SetBlendFrame(DWORD dwFrame)
{
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second->pPart)
			it->second->pPart->SetBlendFrame(dwFrame);
}

void C3DRoleEx::AddEffect(const char* pszIndex, const char* pszKind, BOOL bForce)
{
	if (!pszIndex)
		return;
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second->pPart && (!pszKind || it->first == pszKind))
			it->second->pPart->AddEffect(pszIndex, bForce);
}

void C3DRoleEx::Clear3DEffect(const char* pszIndex, const char* pszKind)
{
	for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
		if (it->second->pPart && (!pszKind || it->first == pszKind))
			it->second->pPart->Clear3DEffect(pszIndex);
}

BOOL C3DRoleEx::TestEffect(const char* pszIndex, const char* pszKind)
{
	if (!pszKind)
	{
		for (MAP_ROLEPARTNODE::iterator it = m_mapNode.begin(); it != m_mapNode.end(); ++it)
			if (it->second && it->second->pPart && it->second->pPart->TestEffect(pszIndex))
				return true;
		return false;
	}
	RolePartNode* pNode = this->GetNode(pszKind);
	if (!pNode || !pNode->pPart)
		return false;
	return pNode->pPart->TestEffect(pszIndex);
}
