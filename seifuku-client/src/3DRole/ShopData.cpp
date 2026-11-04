#include "ShopData.h"
#include "GameMsg.h"
#include "MyIni.h"

CShopData::CShopData()
{
	m_setShopData.clear();
	this->Create();
}

CShopData::~CShopData()
{
	this->Destroy();
}

int CShopData::GetMoneyType(OBJID idShop)
{
	int nAmount = m_setShopData.size();
	for(int i = 0; i < nAmount; i ++)
	{
		ShopDataInfo* pInfo = m_setShopData[i];
		if(pInfo && pInfo->idShop == idShop)
			return pInfo->nMoneyType;
	}
	return 0;
}
BOOL CShopData::GetItemList(OBJID idShop, DEQUE_ITEM& setItem, char* pszName, int& nType)
{
	if(idShop == ID_NONE)
	{
		g_objGameMsg.AddMsg("GetItemList ERROR ID=ID_NONE！");
		return false;
	}
	int nAmount = setItem.size();
	int i = 0;
	for(i = nAmount-1; i >= 0; i --)
	{
		CItem* pItem = setItem[i];
		SAFE_DELETE(pItem);
	}
	setItem.clear();

	ShopDataInfo* pInfo = NULL;
	nAmount = m_setShopData.size();
	for(i = 0; i < nAmount; i++)
	{
		ShopDataInfo* pMyInfo = m_setShopData[i];
		if(pMyInfo && pMyInfo->idShop == idShop)
		{
			pInfo = pMyInfo;
			break;
		}
	}

	if(!pInfo)
	{
		char szMsg[128];
		sprintf(szMsg, "Cannot found Shop，ID=%u！", idShop);
		g_objGameMsg.AddMsg(szMsg);
		return false;
	}
	nType = pInfo->nType;
	strcpy(pszName, pInfo->szName);
	for(i = 0; i < pInfo->dwItemAmount; i++)
	{
		OBJID* pCurrentId = pInfo->pidItem+i;
		CItem* pItem = new CItem;
		MYASSERT(pItem);
		if(!pItem->Create(96120059, *pCurrentId))
		{
			char szMsg[128];
			sprintf(szMsg, "create item for shop error，IdType=%u！", *pCurrentId);
			g_objGameMsg.AddMsg(szMsg);
			SAFE_DELETE(pItem);
			return false;
		}
		setItem.push_back(pItem);
	}
	return true;
}

void CShopData::Create()
{
	char szShopFile[]="ini/Shop.dat";
	CMyIni* pIni = CMyIni::CreateNew(szShopFile, false);
	if(!pIni)
	{
		CMyString strMsg("open shop file %s error！", szShopFile);
		g_objGameMsg.AddMsg(strMsg.c_str());
		return;
	}

	DWORD dwAmount = pIni->GetInt("Header", "Amount");

	for(DWORD i = 0; i < dwAmount; i ++)
	{
		CMyString strSection("Shop%d", i);
		pIni->SearchSection(strSection.c_str());
		ShopDataInfo* pInfo = new ShopDataInfo;
		MYASSERT(pInfo);
		pInfo->idShop = pIni->GetContent("ID").ToLong();
		strcpy(pInfo->szName, pIni->GetContent("Name").c_str());
		pInfo->nType = pIni->GetContent("Type").ToInt();
		pInfo->nMoneyType = pIni->GetContent("MoneyType").ToInt();
		pInfo->dwItemAmount = pIni->GetContent("ItemAmount").ToLong();
		pInfo->pidItem = new OBJID[pInfo->dwItemAmount];
		for(DWORD j = 0; j < pInfo->dwItemAmount; j++)
		{
			CMyString strKey("Item%u", j);
			pInfo->pidItem[j] = pIni->GetContent(strKey.c_str()).ToLong();
		}
		m_setShopData.push_back(pInfo);
	}

	SAFE_DELETE(pIni);
}

void CShopData::Destroy()
{
	int nAmount = m_setShopData.size();
	for(int i = 0; i < nAmount; i++)
	{
		ShopDataInfo* pInfo = m_setShopData[i];
		delete[] pInfo->pidItem;
		SAFE_DELETE(pInfo);
	}
	m_setShopData.clear();
}
