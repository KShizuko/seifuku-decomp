#if !defined(AFX_ITEMADDITION_H__4C270148_7B1D_46C5_A799_B85CF277CF6E__INCLUDED_)
#define AFX_ITEMADDITION_H__4C270148_7B1D_46C5_A799_B85CF277CF6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#pragma warning(disable:4786)
#include "define.h"
#include "ConstGameData.h"
#include "GameData.h"
#include "GameObj.h"
#include "Myheap.h"

#include "SingleMap64.h"

#include <vector>
using namespace std;

enum ITEMADDITIONDATA
{
	ITEMADDITIONDATA_TYPEID = 1,
	ITEMADDITIONDATA_LEVEL,
	ITEMADDITIONDATA_LIFE,
	ITEMADDITIONDATA_ATTACK_MAX,
	ITEMADDITIONDATA_ATTACK_MIN,
	ITEMADDITIONDATA_DEFENSE,
	ITEMADDITIONDATA_MAGICATK,
	ITEMADDITIONDATA_MAGICDEF,
	ITEMADDITIONDATA_DEXTERITY,
	ITEMADDITIONDATA_DODGE,
};

extern char szItemAdditionTable[];
typedef	CGameData<ITEMADDITIONDATA,szItemAdditionTable,szID>	CItemAdditionData;

typedef ISingleMap64<CItemAdditionData, ITEMADDITIONDATA, ITEMADDITIONDATA_TYPEID, ITEMADDITIONDATA_LEVEL>	IItemAdditionSet64;
typedef CSingleMap64<CItemAdditionData, ITEMADDITIONDATA, ITEMADDITIONDATA_TYPEID, ITEMADDITIONDATA_LEVEL>	CItemAdditionSet64;

class CItemAddition
{
public:
	CItemAddition();
	virtual ~CItemAddition();
public:
	static CItemAddition*	CreateNew	()		{ return new CItemAddition; }
	ULONG	Release			()					{ delete this; return 0; }
	bool	Create			(IDatabase* pDb);
	CItemAdditionData*	QueryItemAddition(OBJID idType, int nLevel);

protected:
	IItemAdditionSet64*	m_setAddition;

	MYHEAP_DECLARATION(s_heap)
};

#endif
