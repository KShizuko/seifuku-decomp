#if !defined(AFX_ITEM_H__CA7CE885_1DCF_4B96_B308_FB7F40CCF4BA__INCLUDED_)
#define AFX_ITEM_H__CA7CE885_1DCF_4B96_B308_FB7F40CCF4BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ItemData.h"
#include "MyHeap.h"
#include "T_SingleObjSet2.h"
#include "T_SingleMap2.h"
#include <map>

const int	ITEMREPAIR_PERCENT		= 200;
const int	ITEMIDENT_PERCENT		= 10;
const int	ERROR_WEIGHT			= 123456789;
const int	QUALITY_ZERO		= 0;
const int	QUALITY_ONE			= 1;
const int	QUALITY_TWO			= 2;
const int	QUALITY_THREE		= 3;
const int	QUALITY_FOUR		= 4;

const OBJID TYPE_CHEST_SPACE3	= 1070000;
const OBJID TYPE_CHEST_SPACE6	= 1070001;
const OBJID TYPE_CHEST_SPACE9	= 1070002;

const OBJID TYPE_DRAGONBALL		= 1088000;
const OBJID TYPE_SHOOTINGSTAR	= 1088001;
const OBJID TYPE_DIVOICEITEM	= 1088002;

const OBJID GEM_WPNEXP_LOW	= 700051;
const OBJID GEM_WPNEXP_MID	= 700052;
const OBJID GEM_WPNEXP_HGT	= 700053;

const OBJID GEM_MGCEXP_LOW	= 700061;
const OBJID GEM_MGCEXP_MID	= 700062;
const OBJID GEM_MGCEXP_HGT	= 700063;

const OBJID GEM_EXP_LOW	= 700031;
const OBJID GEM_EXP_MID	= 700032;
const OBJID GEM_EXP_HGT	= 700033;

const OBJID GEM_DUR_LOW	= 700041;
const OBJID GEM_DUR_MID	= 700042;
const OBJID GEM_DUR_HGT	= 700043;

const OBJID GEM_DMG_LOW	= 700011;
const OBJID GEM_DMG_MID	= 700012;
const OBJID GEM_DMG_HGT	= 700013;

const OBJID GEM_HIT_LOW	= 700021;
const OBJID GEM_HIT_MID	= 700022;
const OBJID GEM_HIT_HGT	= 700023;

const OBJID GEM_MATK_LOW	= 700001;
const OBJID GEM_MATK_MID	= 700002;
const OBJID GEM_MATK_HGT	= 700003;

const OBJID GEM_700071_LOW	= 700071;
const OBJID GEM_700071_MID	= 700072;
const OBJID GEM_700071_HGT	= 700073;

const OBJID	NORMAL_ARROW_TYPE	= 1050000;
const OBJID	ARROW_SUBTYPE		= 50;

const OBJID	CARD_FEE_POINT		= 780000;
const OBJID	CARD_FEE_POINT2		= 780001;

const OBJID ARROW_TYPE_ID		= 170001;
const OBJID DART_TYPE_ID		= 171001;

const OBJID ARROW_PILE_WEIGHT	= 100;

enum ITEM_IDENT
{
	_ITEM_STATUS_NONE			= 0,
	_ITEM_STATUS_NOT_IDENT		= 1,
	_ITEM_STATUS_CANNOT_REPAIR	= 2,
	_ITEM_STATUS_NEVER_DAMAGE	= 4,
};

enum TARGET_MASK
{
	TARGET_NONE			= 0x0000,

	TARGET_USER			= 0x0001,
	TARGET_MONSTER		= 0x0002,
	TARGET_EUDEMON		= 0x0004,

	TARGET_SELF			= 0x0010,
	TARGET_OTHERS		= 0x0020,
	TARGET_BODY			= 0x0040,

	TARGET_CHK_PKMODE	= 0x0100,
	TARGET_FORBIDDEN	= 0x0200,
};

typedef std::map<int, int>	ITEMSCORE_SET;

#include "UserTable.h"
class CItem : public CItemData
{
protected:
	CItem();
	virtual ~CItem();
public:
	static CItem* CreateNew()		{ return (new CItem); }
	UINT	ReleaseByOwner()		{ delete this; return 0; }

	static bool	CreateItemInfo(ItemInfoStruct& info, DWORD dwValue, CNpcType* pType, int nQuality=0);

public:
	bool	IsNormal()		{ return GetItemSort() == ITEMSORT_EXPEND; }
	bool	IsWeapon1()		{ return GetItemSort() == ITEMSORT_WEAPON1; }
	bool	IsWeapon2()		{ return GetItemSort() == ITEMSORT_WEAPON2; }
	bool	IsWeapon()		{ return GetItemSort() == ITEMSORT_WEAPON1 || GetItemSort() == ITEMSORT_WEAPON2; }
	bool	IsShield()		{ return CItem::IsShield(GetInt(ITEMDATA_TYPE)); }	bool IsHoldEnable() { return IsWeapon1() || IsWeapon2() || GetItemSort() == ITEMSORT_SHIELD || IsArrowSort(); }
	bool	IsOther()		{ return GetItemSort() == ITEMSORT_OTHER; }	bool IsArmor() { return GetItemSort() == ITEMSORT_ARMOR; }	bool IsHelmet() { return GetItemSort() == ITEMSORT_HELMET; }
	bool	IsArrowSort()	{ return CItem::IsArrowSort(GetInt(ITEMDATA_TYPE)); }			bool IsArrow() { return CItem::IsArrowSort(GetInt(ITEMDATA_TYPE)); }
	bool	IsExpend()		{ return CItem::IsExpend(GetInt(ITEMDATA_TYPE)); }
	bool	IsEquipment()	{ return CItem::IsEquipment(GetInt(ITEMDATA_TYPE)); }
	bool	IsBowSort()		{ return CItem::IsBowSort(GetInt(ITEMDATA_TYPE)); }
	bool	IsBow()			{ return CItem::IsBow(GetInt(ITEMDATA_TYPE)); }
	bool	IsGem()			{ return IsOther() && GetItemType() == ITEMTYPE_GEM; }			bool IsComposeStone() { return IsOther() && GetItemType() == 30000; }
	bool	IsNonsuch()		{ return GetInt(ITEMDATA_TYPE)%10 == 9 || GetInt(ITEMDATA_MAGIC3) >= 6; }
	bool	IsGameCard()	{ return IsOther() && GetItemType() == ITEMTYPE_GAMECARD; }
	bool	IsEatEnable()	{ return IsExpend() && (GetItemType() == 0 || GetItemType() == 10000); }		bool IsTaskItem() { return IsOther() && GetItemType() == ITEMTYPE_TASKITEM; }		bool IsActionItem() { DWORD t = GetInt(ITEMDATA_TYPE); return IsOther() && GetItemType() == ITEMTYPE_ACTIONITEM || (int)t >= 1200000 && (int)t <= 1200002; }		bool IsSpell() { return GetItemSort() == ITEMSORT_EXPEND && GetItemType() == 60000; }
	bool	IsTreasurePack()	{ DWORD t = GetInt(ITEMDATA_TYPE); return t == 1100003 || t == 1100006 || t == 1100009; }

public:
	static bool	IsBowSort(int nType)	{ return CItem::IsBow(nType); }
	static bool	IsBow(int nType)		{ return CItem::GetItemSort(nType) == ITEMSORT_WEAPON2 && CItem::GetItemType(nType) == DWEAPON_BOW; }
	static bool	IsMSword(int nType)		{ return CItem::GetItemSort(nType) == ITEMSORT_WEAPON1 && CItem::GetItemType(nType) == 21000; }
	static bool	IsWeapon1(int nType)	{ return CItem::GetItemSort(nType) == ITEMSORT_WEAPON1; }
	static bool	IsWeapon2(int nType)	{ return CItem::GetItemSort(nType) == ITEMSORT_WEAPON2; }
	static bool	IsShield(int nType)		{ return CItem::GetItemSort(nType) == ITEMSORT_SHIELD; }
	static bool	IsType44Weapon(int nType)	{ return CItem::GetItemSort(nType) == ITEMSORT_WEAPON1 && CItem::GetItemType(nType) == 40000; }
	static bool IsExpend(int nType)		{ return CItem::GetItemSort(nType) == ITEMSORT_EXPEND && CItem::GetItemType(nType) < 70000; }
	static bool	IsEquipment(int nType)	{ return CItem::GetItemSort(nType) >= ITEMSORT_HELMET && CItem::GetItemSort(nType) <= ITEMSORT_SHOES; }
	static bool IsArrowSort(int nType)	{ return CItem::GetItemSort(nType) == ITEMSORT_EXPEND && CItem::GetItemType(nType) == 50000; }
	static int  GetQuality(int nType)	{ return nType%10; }
	static bool	IsGhostGem(int nType)	{ return false; }

	static bool	IsNeedIdent(int nIdent)	{ return (nIdent & _ITEM_STATUS_NOT_IDENT) != 0; }

public:

	bool	IsEquipEnable();
	bool	IsPileEnable()		{ return IsExpend() && GetInt(ITEMDATA_AMOUNTLIMIT) > 1; }
	bool	IsRepairEnable()	{ return !IsExpend() && GetInt(ITEMDATA_AMOUNTLIMIT) >= 100; }
	bool	IsNeedRepair();
	bool	IsSellEnable();
	bool	IsExchangeEnable()	{ return (GetInt(ITEMDATA_MONOPOLY) & MONOPOLY_MASK) == 0; }
	bool	IsStorageEnable()	{ return (GetInt(ITEMDATA_MONOPOLY) & STORAGE_MASK) == 0; }
	bool	IsAlwaysDropWhenDead()	{ return (GetInt(ITEMDATA_MONOPOLY) & DROP_HINT_MASK) != 0 || IsOther() && GetItemType() == 90000; }
	bool	IsNeverDropWhenDead()	{ return (GetInt(ITEMDATA_MONOPOLY) & SELL_HINT_MASK) != 0 || IsTreasurePack() || IsGameCard() || GetItemSort() == ITEMSORT_MANTLE; }
	bool	IsNonsuchItem();
	bool	IsNormalArrow()		{ return GetInt(ITEMDATA_TYPE) == NORMAL_ARROW_TYPE; }
	bool	IsNeedIdent()		{ return IsNeedIdent(GetInt(ITEMDATA_IDENT)); }
	bool	IsCannotRepair()	{ return (GetInt(ITEMDATA_IDENT) & _ITEM_STATUS_CANNOT_REPAIR) != 0; }
	bool	IsNeverWear()		{ return (GetInt(ITEMDATA_IDENT) & _ITEM_STATUS_NEVER_DAMAGE) != 0; }

public:
	bool	TestSpecialFlag(DWORD dwMask)	;
	void	SetSpecialFlag(DWORD dwFlag)	;
	void	SetColor(int nColor)			;
	bool	IsPlunder()						;
	bool	IsChkSumOK()					;
	bool	IsHeadOrBodyLev10()				;

public:
	int		GetScore();
	bool	IsPreciousItem();
	bool	IsHighValue();
	static int	GetEmoneyRate();
	static void	SetEmoneyRate(int nRate);
	static ITEMSCORE_SET*	QueryGem1ScoreSet();
	static ITEMSCORE_SET*	QueryGem2ScoreSet();
	static ITEMSCORE_SET*	QueryAdditionScoreSet();
	static ITEMSCORE_SET*	QueryTypeScoreSet();
	static ITEMSCORE_SET*	QueryReduceDmgScoreSet();

public:

public:
	int		GetAddition(ITEMADDITIONDATA idx);
	int		GetAddition(int idx)	{ return GetAddition((ITEMADDITIONDATA)idx); }
	int		GetMaxAtk()			{ return GetInt(ITEMDATA_ATTACK_MAX_); }
	int		GetMinAtk()			{ return GetInt(ITEMDATA_ATTACK_MIN_); }
	int		GetDef()			{ return GetInt(ITEMDATA_DEFENSE_); }
	int		GetMgcDef()			{ return GetInt(ITEMDATA_MAGICDEF_); }
	int		GetDexteriy()		{ return GetInt(ITEMDATA_DEXTERITY_); }
	int		GetDodge()			{ return GetInt(ITEMDATA_DODGE); }
	int		GetLife()			{ return GetInt(ITEMDATA_LIFE); }

public:
	int		CalcRepairMoney();
	DWORD	GetSellPrice();
	int		GetItemCount()			{ if(IsExpend()) return GetInt(ITEMDATA_AMOUNT); return 1; }
	int		GetWeight()			{ return GetInt(ITEMDATA_WEIGHT_); }
	static 	int		GetWeight(OBJID idType, int nAmount=0);

public:
	void	SetIdent(bool bNeedIdent, bool bUpdate)	{
				if(bNeedIdent) SetInt(ITEMDATA_IDENT, GetInt(ITEMDATA_IDENT)|_ITEM_STATUS_NOT_IDENT, bUpdate);
				else SetInt(ITEMDATA_IDENT, GetInt(ITEMDATA_IDENT)&(~_ITEM_STATUS_NOT_IDENT), bUpdate); }
	void	SetStatus(DWORD dwData, bool bUpdate) { SetInt(ITEMDATA_IDENT, dwData, bUpdate); }

public:
	int		GetGemDmgEffect		(void);
	int		GetGemEffect700071	(void);
	int		GetGemMgcAtkEffect	(void);
	int		GetGemHitRateEffect	(void);
	int		GetGemExpEffect		(void);
	int		GetGemWpnExpEffect	(void);
	int		GetGemMgcExpEffect	(void);
	int		GetGemDurEffect		(OBJID idGemType=ID_NONE);

public:
	CItem*	Split(int nNum);

public:

	int		GetItemSubType();
	int		GetSubType()		;

	int		GetLevel	(void);
	int		GetQuality	(void)	{ return GetQuality(GetInt(ITEMDATA_TYPE)); }

	bool	ChangeType	(OBJID idNewType);
	bool	ChangeColor	(int nColor);

	bool	RecoverDur			(void);
	int		GetRecoverDurCost	(void);

public:
	int		GetItemSort()			{ return CItem::GetItemSort(GetInt(ITEMDATA_TYPE)); }
	int		GetItemType()			{ return CItem::GetItemType(GetInt(ITEMDATA_TYPE)); }
public:
	static int	GetItemSort(OBJID idType);
	static int	GetItemType(OBJID idType);
public:
	CItemTypeData*	QueryType()	{ return m_pType; }
	static int	GetSubType(OBJID idType)			{ return (idType%1000000)/1000; }

protected:
	CItem*	Clone();

	MYHEAP_DECLARATION(s_heap)
};

#endif
