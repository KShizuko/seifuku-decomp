#ifndef _ITEM_H
#define _ITEM_H

#include "BaseFunc.h"
#include "ItemData.h"
#include "2DMapItem.h"

const int _ARROW	=	170001;
const int _DART		=	171001;

const int _NOT_MONOPOLY_NOT_DISAPPEAR	=	0;
const int _NOT_MONOPOLY_DISAPPEAR		=	1;
const int _MONOPOLY_NOT_DISAPPEAR		=	2;
const int _MONOPOLY_DISAPPEAR			=	3;
const int _MONOPOLY_DROPCONFIRM			=   4;
const int _MONOPOLY_SELLCONFIRM			=   8;
const int _MONOPOLY_NOTDROP				=  16;
const int _MONOPOLY_NOT_SELL			=  32;

const int _MAX_EQUIPMENT				=	9;

const unsigned char _MAN_USEABLE		=	0x01;
const unsigned char _WOMAN_USEABLE		=	0x02;
const unsigned char _EUNUCH_USEABLE		=	0x04;

const int	ITEMPOSITION_BACKPACK	=	0;
const int	ITEMPOSITION_HELMET		=	1;
const int	ITEMPOSITION_NECKLACE	=	2;
const int	ITEMPOSITION_ARMOR		=	3;
const int	ITEMPOSITION_WEAPONR	=	4;
const int	ITEMPOSITION_WEAPONL	=	5;
const int	ITEMPOSITION_RINGR		=	6;
const int	ITEMPOSITION_TREASURE	=	7;
const int	ITEMPOSITION_SHOES		=	8;
const int	ITEMPOSITION_MOUNT		=	9;
const int	ITEMPOSITION_SPRITE		=	10;
const int ITEMPOSITION_MANTLE		=	11;

const int ITEMPOSITION_GHOSTGEM_PACK	=	51;
const int ITEMPOSITION_EUDEMONEGG_PACK	=	52;
const int ITEMPOSITION_EUDEMON_PACK		=	53;

const int	ITEMPOSITION_USER_LIMIT	=	199;
const int	ITEMPOSITION_STORAGE	=	201;
const int	ITEMPOSITION_TRUNK		=	202;
const int	ITEMPOSITION_CHEST		=	203;
const int   ITEMPOSITION_AUCTION_STORAGE = 207;

const int	ITEMPOSITION_GROUND		=	254;
const int	ITEMPOSITION_NONE		=	255;

const int	ITEMSORT_INVALID				=	-1;
const int	ITEMSORT_EXPEND					=	0;
const int	ITEMSORT_HELMET					=	1;
const int	ITEMSORT_NECKLACE				=	2;
const int	ITEMSORT_ARMOR					=	3;
const int	ITEMSORT_WEAPON_SINGLE_HAND		=	4;
const int	ITEMSORT_WEAPON_DOUBLE_HAND		=	5;
const int	ITEMSORT_SHIELD					=	6;
const int	ITEMSORT_RING					=	7;
const int	ITEMSORT_SHOES					=	8;
const int	ITEMSORT_OTHER					=	9;
const int	ITEMSORT_MOUNT					=	10;
const int	ITEMSORT_SPRITE					=	11;
const int	ITEMSORT_TREASURE				=	12;

const int	ITEMTYPEMONEY_MIN			= 1090000;
const int	ITEMTYPEMONEY_MAX			= 1091020;

const int	SMALLHEAPMONEY_LIMIT		= 10;
const int	SMALLHEAPMONEY_TYPE			= 1090000;
const int	MIDDLEHEAPMONEY_LIMIT		= 100;
const int	MIDDLEHEAPMONEY_TYPE		= 1090010;
const int	BIGHEAPMONEY_LIMIT			= 1000;
const int	BIGHEAPMONEY_TYPE			= 1090020;

const int	SMALLHEAPGOLD_LIMIT			= 2000;
const int	SMALLHEAPGOLD_TYPE			= 1091000;
const int	MIDDLEHEAPGOLD_LIMIT		= 5000;
const int	MIDDLEHEAPGOLD_TYPE			= 1091010;
const int	BIGHEAPGOLD_LIMIT			= 10000;
const int	BIGHEAPGOLD_TYPE			= 1091020;

const int	ITEMTYPE_SPRITE		=	50000;

const int	OTHER_GEM					=	00000;
const int	OTHER_TASKITEM				=	10000;
const int	OTHER_ACTIONITEM			=	20000;

const int	EXPEND_INVALID				=	-1;
const int	EXPEND_MEDICINE				=	00000;
const int	EXPEND_POISON				=	10000;
const int	EXPEND_DART					=	20000;
const int	EXPEND_BOOK					=	30000;
const int	EXPEND_NOTUSE				=	40000;
const int	EXPEND_ARROW				=	50000;
const int	EXPEND_SPELL				=	60000;
const int	EXPEND_NOT_DIRECTUSE		=	70000;

const int	NECKLACE_NECKLACE			=	00000;
const int	NECKLACE_SACHET				=	10000;
const int	NECKLACE_AMULET				=	20000;

const int	RING_RING				=	00000;
const int	RING_THUMB_RING			=	10000;
const int	RING_BANGLE				=	20000;

const int	SWEAPON_NONE				=	00000;
const int	SWEAPON_BLADE				=	10000;
const int	SWEAPON_AXE					=	20000;
const int	SWEAPON_HAMMER				=	30000;
const int	SWEAPON_HOOK				=	40000;
const int	SWEAPON_CLUB				=	50000;
const int	SWEAPON_SWORD				=	60000;
const int	SWEAPON_CRUTCH				=	70000;
const int	SWEAPON_SCOURGE				=	80000;
const int	SWEAPON_SHORT				=	90000;

const int	DWEAPON_BOW					=	00000;
const int	DWEAPON_BLADE				=	10000;
const int	DWEAPON_AXE					=	20000;
const int	DWEAPON_HAMMER				=	30000;
const int	DWEAPON_HOOK				=	40000;
const int	DWEAPON_STAFF				=	50000;
const int	DWEAPON_SHOVEL				=	60000;
const int	DWEAPON_HALBERD				=	70000;
const int	DWEAPON_FORK				=	80000;
const int	DWEAPON_SPEAR				=	90000;

const int ITEMTYPE_GHOSTGEM = 30000;
const int ITEMTYPE_GHOSTGEM_ACTIVE_ATK = 31000;
const int ITEMTYPE_GHOSTGEM_PASSIVE_ATK = 32000;
const int ITEMTYPE_GHOSTGEM_EUDEMON = 33000;
const int ITEMTYPE_GHOSTGEM_RELEASE = 34000;
const int ITEMTYPE_GHOSTGEM_TRACE = 35000;
const int ITEMTYPE_GHOSTGEM_PROTECTIVE = 36000;
const int ITEMTYPE_GHOSTGEM_SPECIAL = 37000;
const int ITEMTYPE_GHOSTGEM_INVITE = 38000;

const int ITEMTYPE_EUDEMON = 70000;
const int ITEMTYPE_EUDEMONEGG = 80000;

enum
{
	_ITEM_STATUS_NONE			=	0,
	_ITEM_STATUS_NOT_IDENT		=	1,
	_ITEM_STATUS_CANNOT_REPAIR	=	2,
	_ITEM_STATUS_NEVER_DAMAGE	=	4,
	_ITEM_STATUS_MAGIC_ADD		=	8,

};

const unsigned short TARGET_NONE = 0x0000;
const unsigned short TARGET_USER = 0x0001;
const unsigned short TARGET_MONSTER = 0x0002;
const unsigned short TARGET_EUDEMON = 0x0004;
const unsigned short TARGET_SELF = 0x0010;
const unsigned short TARGET_OTHERS = 0x0020;
const unsigned short TARGET_BODY = 0x0040;
const unsigned short TARGET_CHK_PKMODE = 0x0100;
const unsigned short TARGET_FORBIDDEN = 0x0200;

const OBJID MONEYITEM_TYPE = 720000;

class CItem
{
	friend struct ItemProbe;
public:
	CItem();
	virtual ~CItem();

	static	CItemData s_ItemData;
	static  int ItemGetSort		(OBJID idType);
	static  int ItemGetSubType	(OBJID idTyep);
	static  int ItemGetBaseClass(OBJID idType);

protected:
	OBJID			m_idItem;
	OBJID			m_idOwner;
	ItemTypeInfo	m_infoType;
	unsigned char	m_ucPosition;
	DWORD			m_dwSellPrice;
	DWORD			m_dwSellEmoneyPrice;
	int				m_nMaxAttackModify;
	int				m_nMinAttackModify;
	int				m_nDefenceModify;
	int				m_nDexterityModify;
	int				m_nDodgeModify;
	int				m_nMagicAttackModify;
	int				m_nMagicDefenceModify;
	DWORD			m_dwBless;
	DWORD			m_dwEnchant;
	DWORD			m_dwRestrain;
	ItemAdditionInfo m_infoAddition;
public:

	BOOL			Create(OBJID idItem, OBJID idItemType);
	void			SetAmount(unsigned short usAmount){m_infoType.usAmount = usAmount;}
	void			SetAmountLimit(unsigned short usAmountLimit){m_infoType.usAmountLimit = usAmountLimit;}
	void			SetGem1(unsigned char ucGem1){m_infoType.ucGem1 = ucGem1;}
	void			SetGem2(unsigned char ucGem2){m_infoType.ucGem2 = ucGem2;}
	void			SetMagic1(unsigned char ucMagic1){m_infoType.ucMagic1 = ucMagic1;}
	void			SetMagic2(unsigned char ucMagic2){m_infoType.ucMagic2 = ucMagic2;}
	void			SetMagic3(unsigned char ucMagic3){m_infoType.ucMagic3 = ucMagic3; UpdateAdditionInfo();}
	void			SetPosition(unsigned char ucPosition){m_ucPosition = ucPosition;}
	void			SetBless(DWORD dwBless){m_dwBless = dwBless;}
	DWORD			GetBless(){return m_dwBless;}
	void			SetEnchant(DWORD dwEnchant){m_dwEnchant = dwEnchant;}
	DWORD			GetEnchant(){return m_dwEnchant;}
	void			SetRestrain(DWORD dwRestrain){m_dwRestrain = dwRestrain;}
	DWORD			GetRestrain(){return m_dwRestrain;}
	void			UpdateAdditionInfo();

	void			SetStatus(unsigned char ucStatus){m_infoType.ucStatus = ucStatus;}
	unsigned char	GetStatus(){return m_infoType.ucStatus;}
	BOOL			TestStatus(unsigned char ucStatus)
	{
		if ( m_infoType.ucStatus == 0  && ucStatus == 0)
			return true;
		return m_infoType.ucStatus & ucStatus;
	}

	OBJID			GetID(){return m_idItem;}
	int				GetSort();
	int				GetSubType();
	OBJID			GetTypeID(){return m_infoType.uID;}
	char*			GetName(){return m_infoType.szName;}
	char*			GetShowName(){return m_infoType.szShowName;}
	unsigned char	GetProfessionRequired(){return m_infoType.ucRequiredProfession;}
	unsigned char	GetWeaponSkillRequired(){return m_infoType.ucRequiredWeaponSkill;}
	unsigned char	GetLevelRequired(){return m_infoType.ucRequiredLevel;}
	unsigned char	GetSexRequried(){return m_infoType.ucRequiredSex;}
	unsigned short	GetForceRequired(){return m_infoType.usRequiredForce;}
	unsigned short	GetSpeedRequired(){return m_infoType.usRequiredSpeed;}
	unsigned short	GetHealthRequired(){return m_infoType.usRequiredHealth;}
	unsigned short	GetSoulRequired(){return m_infoType.usRequiredSoul;}
	unsigned char	GetMonopoly(){return m_infoType.ucMonopoly;}
	unsigned short	GetWeight(){return m_infoType.usWeight;}
	unsigned int	GetPrice(){return m_infoType.uPrice;}
	unsigned int	GetEmoneyPrice(){return m_infoType.uEmoneyPrice;}
	unsigned short	GetMaxAttack(){return m_infoType.usMaxAttack;}
	unsigned short	GetMinAttack(){return m_infoType.usMinAttack;}
	short			GetDefense(){return m_infoType.usDefense;}
	short			GetDexterity(){return m_infoType.usDexterity;}
	short			GetDodge(){return m_infoType.usDodge;}
	short			GetLife(){return m_infoType.sLife;}
	short			GetMana(){return m_infoType.sMana;}
	unsigned short	GetAmount(){return m_infoType.usAmount;}

	unsigned short	GetAmountLimit(){return m_infoType.usAmountLimit;}
	unsigned char	GetGem1(){return m_infoType.ucGem1;}
	unsigned char	GetGem2(){return m_infoType.ucGem2;}
	unsigned char	GetMagic1(){return m_infoType.ucMagic1;}
	unsigned char	GetLucky(){return m_infoType.ucMagic2;}
	unsigned char	GetAddition(){return m_infoType.ucMagic3;}
	unsigned char	GetPosition(){return m_ucPosition;}
	DWORD			GetSellPrice();
	DWORD			GetRepairCost();

	DWORD			GetWeaponSkillType();

	int				GetAtkRange () { return m_infoType.usRange; }

	unsigned short	GetMinMagicAttack(){return m_infoType.usDexterity;}
	unsigned short	GetMaxMagicAttack(){return m_infoType.usMagicAttack;}
	unsigned short	GetSoul(){return m_infoType.usDodge;}
	unsigned short	GetAdditionLife(){return m_infoAddition.nLife;}
	unsigned short	GetAdditionAttackMax(){return m_infoAddition.nAttackMax;}
	unsigned short	GetAdditionDefense(){return m_infoAddition.nDefense;}
	unsigned short	GetAdditionMAttack(){return m_infoAddition.nMAttack;}
	unsigned short	GetAdditionMDef(){return m_infoAddition.nMDef;}
	unsigned short	GetAdditionDexterity(){return m_infoAddition.nDexterity;}
	unsigned short	GetAdditionDodge(){return m_infoAddition.nDodge;}
	unsigned short	GetMagicDefence();
	unsigned short  GetAttackSpeed(){return m_infoType.usAttackSpeed;}

	enum {TREASURE_DRAGONBALL=1088000, TREASURE_SHOOTINGSTAR=1088001,TREASURE_SOULSTONE=1037150};

	BOOL	IsDragonBall(void)		{ return (TREASURE_DRAGONBALL == GetTypeID()); }
	BOOL	IsShootingStar(void)	{ return (TREASURE_SHOOTINGSTAR == GetTypeID()); }
	BOOL	IsSoulStone(void)		{return (TREASURE_SOULSTONE == GetTypeID()); }

	enum {SOULSTONE_ALIVE=1033020,SOULSTONE_IMPROVE=1033030};
	BOOL	IsAliveSoulStone(void)   {return (SOULSTONE_ALIVE == GetTypeID()); }
	BOOL	IsImproveSoulStone(void) {return (SOULSTONE_IMPROVE == GetTypeID()); }

	char*	GetDesc();

	BOOL	IsActionItem(void)	{ return ITEMSORT_OTHER == GetSort() && OTHER_ACTIONITEM == GetSubType(); }
	BOOL	IsTaskItem	(void)	{ return ITEMSORT_OTHER == GetSort() && OTHER_TASKITEM == GetSubType(); }
	BOOL	IsMonopolyItem(void) {return this->GetMonopoly() == _MONOPOLY_NOT_DISAPPEAR || this->GetMonopoly() == _MONOPOLY_DISAPPEAR; }
	BOOL	IsGem		(void)	{ return ITEMSORT_OTHER == GetSort() && OTHER_GEM == GetSubType(); }
	BOOL	IsArrow		(void)	{ return ITEMSORT_EXPEND == GetSort() && EXPEND_ARROW == GetSubType(); }
	BOOL	IsBow		(void)	{ return ITEMSORT_WEAPON_DOUBLE_HAND == GetSort() && DWEAPON_BOW == GetSubType(); }
	BOOL	IsSingleHandWeapon(void)	{ return ITEMSORT_WEAPON_SINGLE_HAND == GetSort(); }
	BOOL	IsDoubleHandWeapon(void)	{ return ITEMSORT_WEAPON_DOUBLE_HAND == GetSort(); }
	BOOL	IsHelmet	(void)	{ return ITEMSORT_HELMET == GetSort(); }
	BOOL	IsArmor		(void)	{ return ITEMSORT_ARMOR == GetSort(); }
	BOOL	IsShield	(void)	{ return ITEMSORT_SHIELD == GetSort(); }
	BOOL	IsBacksword	(void)	{ return ITEMSORT_WEAPON_SINGLE_HAND == GetSort() && 21000 == GetSubType(); }
	BOOL	IsAdditionStone(void);

	BOOL    IsMoneyItem(){ return GetTypeID () == MONEYITEM_TYPE;}

	void	SetBoothSellPrice(DWORD dwPrice);
	DWORD	GetBoothSellPrice();
	void	SetBoothSellEmoneyPrice(DWORD dwPrice);
	DWORD	GetBoothSellEmoneyPrice(){return m_dwSellEmoneyPrice;}
	void	SetOwnerID(OBJID idOwner);
	OBJID	GetOwnerID(){return m_idOwner;}
	DWORD	GetFuseInfo();

	int		GetMaxAttackModify(){return m_nMaxAttackModify;}
	int		GetMinAttackModify(){return m_nMinAttackModify;}
	int		GetDefenceModify(){return m_nDefenceModify;}
	int		GetDexterityModify(){return m_nDexterityModify;}
	int		GetDodgeModify(){return m_nDodgeModify;}
	int		GetMagicAttackModify(){return m_nMagicAttackModify;}
	int		GetMagicDefenceModify(){return m_nMagicDefenceModify;}
	static	BOOL	IsNonsuchItem(OBJID idItemType);
	BOOL	IsNonsuchType(){return (m_infoType.usTypeMask & 1) != 0;}
};

#include <deque>
using namespace std;
typedef deque<CItem*>DEQUE_ITEM;

#endif
