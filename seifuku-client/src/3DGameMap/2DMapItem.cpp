#include "2DMapItem.h"
#include "GameDataSet.h"
#include "3DGameMap.h"
#include "Item.h"

C2DMapItem::C2DMapItem(OBJID idItemType)
{
	m_dwFrame		= 0;
	m_idItemType	= idItemType;
	m_pEffectEx		= ::Game3DEffectCreate(NULL);
	if(m_pEffectEx)
		m_pEffectEx->Add((char*)"ItemFlash");
	m_dwBegin	= ::TimeGet();
	m_bShowName = false;
}

C2DMapItem::~C2DMapItem()
{
	if(m_pEffectEx)
	{
		m_pEffectEx->Release();
		m_pEffectEx = NULL;
	}
}
C2DMapItem* C2DMapItem::CreateNew(CMyPos posCell, OBJID idType)
{
	if(idType == ID_NONE)
		return NULL;
	C2DMapItem* pItem = new C2DMapItem(idType);
	MYASSERT(pItem);
	pItem->SetPos(posCell);
	return pItem;
}
void C2DMapItem::Show(void* pInfo)
{
	this->GetWorldPos(m_posWorld);
	char szAni[256] = "";
	sprintf(szAni, "%u", m_idItemType);
	IAni* pAni = GameDataSetQuery()->GetDataAni("ani/MapItemIcon.Ani", szAni);
	if (!pAni)
		return;
	CMyPos posScr;
	g_objGameMap.Cell2Screen(m_posCell.x, m_posCell.y, posScr.x, posScr.y);
	CMySize sizeAni = pAni->GetSize(m_dwFrame);
	CMyPos posShow = { posScr.x-sizeAni.iWidth/2, posScr.y-sizeAni.iHeight/2};
	CMyBitmap* pBmp = pAni->GetFrame(m_dwFrame++);
	if(pBmp)
	{
		unsigned char ucA, ucR, ucG, ucB;
		ucA = m_dwARGB >> 24;
		ucR = (m_dwARGB & 0x00ff0000) >> 16;
		ucG = (m_dwARGB & 0x0000ff00) >> 8;
		ucB = m_dwARGB & 0x000000ff;
		pBmp->SetColor(ucA, ucR, ucG, ucB);
		CMyPos posBmpShow = {posShow.x, posShow.y};
		g_objGameMap.MapScaleShowPos(posBmpShow.x, posBmpShow.y);
		CMySize sizeBmp;
		pBmp->GetSize(sizeBmp.iWidth, sizeBmp.iHeight);
		CMySize sizeShow;
		sizeShow.iWidth =  sizeBmp.iWidth*g_objGameMap.GetScale()/_NORMAL_SCALE;
		sizeShow.iHeight = sizeBmp.iHeight*g_objGameMap.GetScale()/_NORMAL_SCALE;
		pBmp->ShowEx(posBmpShow.x,posBmpShow.y , NULL,sizeShow.iWidth,sizeShow.iHeight );
	}
	m_objAniEffect.Show(posShow.x, posShow.y);
	if(m_pEffectEx)
	{
		g_objGameMap.World2Bg(m_posWorld.x, m_posWorld.y, posShow.x, posShow.y);
		m_pEffectEx->Show(posShow);
	}
	if (m_bShowName || g_objGameMap.GetShowMapObjInfoFlag())
	{
		const OBJID idMoney = 300000;
		ItemTypeInfo info;
		if (!CItem::s_ItemData.GetItemTypeInfo(m_idItemType, info))
			strcpy(info.szName, GameDataSetQuery()->GetStr(idMoney));
		int nMsgLen = strlen(info.szName)*(CMyBitmap::GetFontSize()/2);
		CMyPos posShowName = {posScr.x - nMsgLen/2, posScr.y - 32};
		g_objGameMap.MapScaleShowPos(posShowName.x, posShowName.y);
		if(m_bShowName)
		{
			CMyBitmap::ShowString(posShowName.x, posShowName.y, _COLOR_YELLOW,info.szName);
		}
		else
		{
			g_objGameMap.m_InfoViewManager.AddStringView(info.szName, posShowName, _COLOR_YELLOW);
		}
	}
}
void C2DMapItem::Process(void* pInfo)
{
	if (::TimeGet() - m_dwBegin < 500)
		return;
	m_objAniEffect.Process();
	if(m_pEffectEx)
		m_pEffectEx->Process();
	if (g_objGameMap.IsPosVisible(m_posCell))
	{
		CInteractiveLayer* pLayer = g_objGameMap.GetInteractiveLayer();
		if(pLayer)
			pLayer->ApplyShow(this);
	}
}
