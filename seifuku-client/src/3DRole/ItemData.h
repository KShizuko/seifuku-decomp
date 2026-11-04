#if !defined(AFX_ITEMDATA_H__B6A8F74E_31AA_46B4_BF3C_03AF450B1E90__INCLUDED_)
#define AFX_ITEMDATA_H__B6A8F74E_31AA_46B4_BF3C_03AF450B1E90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
#include <map>
#include <string>
#include <vector>

using namespace std;
enum
{
	_SPRITE_ADDITION_PATK	= 0,
	_SPRITE_ADDITION_PDEF	= 1,
	_SPRITE_ADDITION_MATK	= 2,
	_SPRITE_ADDITION_MDEF	= 3,
	_SPRITE_ADDITION_SOUL	= 4,
};
typedef struct
{
	unsigned int	uID;
	char			szName[_MAX_NAMESIZE];
	unsigned char	ucRequiredProfession;
	unsigned char	ucRequiredWeaponSkill;
	unsigned char	ucRequiredLevel;
	unsigned char	ucRequiredSex;
	unsigned short	usRequiredForce;
	unsigned short	usRequiredSpeed;
	unsigned short	usRequiredHealth;
	unsigned short	usRequiredSoul;
	unsigned char	ucMonopoly;
	unsigned short	usWeight;
	unsigned int	uPrice;
	int				uLuck;
	unsigned short	usMaxAttack;
	unsigned short	usMinAttack;
	short			usDefense;
	short			usDexterity;
	short			usDodge;
	short			sLife;
	short			sMana;
	unsigned short	usAmount;
	unsigned short	usAmountLimit;
	unsigned char	ucStatus;
	unsigned char	ucGem1;
	unsigned char	ucGem2;
	unsigned char	ucMagic1;
	unsigned char	ucMagic2;
	unsigned char	ucMagic3;
	unsigned short	usMagicAttack;
	unsigned short	usMagicDefence;
	unsigned short  usRange;
	unsigned short  usAttackSpeed;
	unsigned char	ucFrayMode;
	unsigned char	ucRepairMode;
	unsigned short	usTypeMask;
	unsigned int	uEmoneyPrice;
	char			szShowName[32];
	char			szDesc[128];
}ItemTypeInfo;

typedef struct
{
	unsigned short nLife;
	unsigned short nAttackMax;
	unsigned short nAttackMin;
	unsigned short nDefense;
	unsigned short nMAttack;
	unsigned short nMDef;
	unsigned short nDexterity;
	unsigned short nDodge;
}ItemAdditionInfo;

typedef map<OBJID, ItemTypeInfo> MAP_ITEMINFO;
typedef map<__int64, ItemAdditionInfo> MAP_ITEMADDITIONINFO;
typedef map<DWORD, DWORD> MAP_FUSE;

class CItemData
{
	friend struct ItemDataProbe;
public:
	CItemData();
	virtual ~CItemData();

private:
	MAP_ITEMINFO			m_mapItemType;
	MAP_ITEMADDITIONINFO	m_mapItemAddition;
	MAP_FUSE				m_mapFuse;


private:
	BOOL	LoadItemTypeInfo();
	BOOL	CreateIndexTable();
	BOOL	CreateAdditionalInfo();
public:
	BOOL	GetItemTypeInfo(OBJID idType, ItemTypeInfo& infoItemType);
	BOOL	GetItemAdditionInfo(OBJID idType, DWORD dwLevel, ItemAdditionInfo& infoAdditional);
	DWORD	GetFuseInfo(DWORD dwKey);
};

#endif
