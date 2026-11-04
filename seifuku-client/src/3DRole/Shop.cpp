#include "Item.h"
#include "TxtMsgSet.h"
#include "Shop.h"
#include "GameMsg.h"
#include "ShopData.h"
#include "3DGameMap.h"
#include "GameDataSet.h"
#include "Hero.h"
#include "GamePlayerSet.h"

const char szShopInfoFile[]		="ini/Shop.ini";

CShop	g_objShop;
CShopData CShop::s_ShopData;
CShop::CShop()
{
	m_dequeItem.clear();
	m_bActive = false;
	m_nMoneyType = 0;
}

CShop::~CShop()
{
	this->Close();
}

DWORD	CShop::GetItemAmount()
{
	if(!m_bActive)
	{
		OBJID idOpenShopFirst = 100083;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idOpenShopFirst));
		return 0;
	}
	return m_dequeItem.size();
}
CItem*	CShop::GetItem(DWORD dwIndex)
{
	if(!m_bActive)
	{
		OBJID idOpenShopFirst = 100083;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idOpenShopFirst));
		return NULL;
	}
	DWORD dwAmount = m_dequeItem.size();
	if(dwIndex >= dwAmount)
		return NULL;
	return m_dequeItem[dwIndex];
}
BOOL	CShop::Open(OBJID idNpc)
{
	if(m_bActive)
		this->Close();

	if(idNpc == ID_NONE)
	{
		OBJID idShopIdError = 100084;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idShopIdError));
		return false;
	}

  	if(s_ShopData.GetItemList(idNpc, m_dequeItem, m_szName, m_nShopType))
	{
		m_bActive	= true;
		m_idNpc		= idNpc;
		CPlayer* pPlayer = g_objPlayerSet.GetPlayer(idNpc);
		if(pPlayer)
		{
			CMyPos posPlayer;
			pPlayer->GetPos(posPlayer.x, posPlayer.y);
			CMyPos posHero;
			g_objHero.GetPos(posHero.x, posHero.y);
			int nDir = g_objGameMap.GetDirection(posPlayer.x, posPlayer.y, posHero.x, posHero.y);
			pPlayer->m_pIRoleView->SetVariable(_VAR_DIRECT_DIR, nDir);
		}
		m_nMoneyType = s_ShopData.GetMoneyType(idNpc);

		return true;
	}
	else
		return false;
}
void	CShop::Close()
{
	if(!m_bActive)
	{
		OBJID idOpenShopFirst = 100083;
		return;
	}
	m_bActive = false;
	m_nMoneyType = 0;
	this->ClearItem();
}
void	CShop::ClearItem()
{
	int nAmount = m_dequeItem.size();
	for(int i = 0; i < nAmount; i ++)
	{
		CItem* pItem = m_dequeItem[i];
		SAFE_DELETE(pItem);
	}
	m_dequeItem.clear();
}
char*	CShop::GetShopName()
{
	if(!m_bActive)
	{
		OBJID idOpenShopFirst = 100083;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idOpenShopFirst));
		return NULL;
	}
	return m_szName;
}
CItem*	CShop::GetItemByTypeID(OBJID idType)
{
	if(!m_bActive)
	{
		OBJID idOpenShopFirst = 100083;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idOpenShopFirst));
		return NULL;
	}
	for(DEQUE_ITEM::iterator it = m_dequeItem.begin(); it != m_dequeItem.end(); it ++)
	{
		CItem* pItem = *it;
		if(pItem && pItem->GetTypeID() == idType)
			return pItem;
	}
	return NULL;
}
int	CShop::GetMoneyType()
{
	if(!m_bActive)
	{
		OBJID idOpenShopFirst = 100083;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idOpenShopFirst));
		return 0;
	}
	return m_nMoneyType;
}
OBJID	CShop::GetShopID()
{
	if(!m_bActive)
	{
		OBJID idOpenShopFirst = 100083;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idOpenShopFirst));
		return ID_NONE;
	}
	return m_idNpc;
}
int		CShop::GetShopType()
{
	if(!m_bActive)
	{
		OBJID idOpenShopFirst = 100083;
		g_objGameMsg.AddMsg(GameDataSetQuery()->GetStr(idOpenShopFirst));
		return _SHOP_NONE;
	}
	return m_nShopType;
}
void InsertComma(char* pszNum)
{
	char szTemp[256];
	int nLen = strlen(pszNum);
	if (nLen <= 3)
		return;
	int i = 0;
	int nCount = 0;
	while (nLen > 0)
	{
		if (nCount < 3)
		{
			szTemp[i++] = pszNum[nLen - 1];
			nLen--;
			nCount++;
		}
		else
		{
			szTemp[i++] = ',';
			nCount = 0;
		}
	}
	szTemp[i] = 0;
	char* pszRev = _strdup(szTemp);
	strcpy(pszNum, _strrev(pszRev));
	free(pszRev);
}
void RemoveChar(char* psz, char chRemove)
{
	char szTemp[256];
	char* p = psz;
	int nLen = 0;
	while (*p != 0)
	{
		if (*p == chRemove)
		{
			p++;
			continue;
		}
		szTemp[nLen] = *p;
		nLen++;
		p++;
	}
	szTemp[nLen] = 0;
	strcpy(psz, szTemp);
}
