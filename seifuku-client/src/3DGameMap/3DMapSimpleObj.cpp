#include "3DMapSimpleObj.h"
#include "3DGameMap.h"

C3DMapSimpleObj::C3DMapSimpleObj()
{
	m_obj3DSimpleObj = NULL;
	m_nType = MAP_3DSIMPLE;
	m_dwTimeBegin = ::TimeGet();
	m_dwFrameInterval = 33;
	m_bDie = false;
	m_fHorizontal = 0.0;
	m_fVertical = 0.0;
	m_bRole = true;
}

C3DMapSimpleObj::~C3DMapSimpleObj()
{
	if(m_obj3DSimpleObj)
	{
		m_obj3DSimpleObj->Release();
		m_obj3DSimpleObj = NULL;
	}
}
void C3DMapSimpleObj::SetWorldPos(CMyPos posWorld)
{
	m_posWorld = posWorld;
	g_objGameMap.World2Cell(m_posWorld.x, m_posWorld.y, m_posCell.x, m_posCell.y);
}
void C3DMapSimpleObj::GetWorldPos(CMyPos& posWorld)
{
	posWorld = m_posWorld;
}
void C3DMapSimpleObj::SetFrameInterval(DWORD dwFrameInterval)
{
	m_dwFrameInterval = dwFrameInterval;
}
void C3DMapSimpleObj::AddEffect(char* pszIndex)
{
	if(pszIndex && m_obj3DSimpleObj)
	{
		m_obj3DSimpleObj->AddEffect(pszIndex);
	}
}
void C3DMapSimpleObj::Show(void* pInfo)
{
	CMyPos posBG;
	g_objGameMap.World2Bg(m_posWorld.x, m_posWorld.y, posBG.x, posBG.y);
	CMyPos posView;
	g_objGameMap.GetViewPos(posView);
	int nViewportBgX, nViewportBgY;
	int nRotate	= 0;
	g_objGameMap.World2Bg(posView.x, posView.y, nViewportBgX, nViewportBgY);
	if(!m_obj3DSimpleObj)
		return;
	m_obj3DSimpleObj->Rotate(m_fVertical, 0.0, m_fHorizontal);
	m_obj3DSimpleObj->SetFrame((::TimeGet()-m_dwTimeBegin)/m_dwFrameInterval);
	if(m_bRole)
		m_obj3DSimpleObj->SetPos(posBG.x, posBG.y, 0, nRotate, 65.0f/100.0f);
	else
		m_obj3DSimpleObj->SetPos(posBG.x, posBG.y, 0, nRotate, 1.0f);
	m_obj3DSimpleObj->Draw2BG(nViewportBgX, nViewportBgY);
}
void C3DMapSimpleObj::Process(void* pInfo)
{
	if(m_bDie)
		return;
	if(g_objGameMap.IsPosVisible(m_posCell))
	{
		CInteractiveLayer* pLayer = g_objGameMap.GetInteractiveLayer();
		if(pLayer)
			pLayer->ApplyShow(this);
	}
}
BOOL C3DMapSimpleObj::Create(OBJID idType, CMyPos posWorld, BOOL bRole)
{
	this->SetWorldPos(posWorld);
	m_bRole = bRole;
	m_obj3DSimpleObj = ::Simple3DObjCreate(idType);
	return m_obj3DSimpleObj != NULL;
}
C3DMapSimpleObj* C3DMapSimpleObj::CreateNew(OBJID idType, CMyPos posWorld, BOOL bRole)
{
	C3DMapSimpleObj* pObj = new C3DMapSimpleObj;
	if(pObj && pObj->Create(idType, posWorld, bRole))
		return pObj;
	SAFE_DELETE(pObj);
	return NULL;
}
