#include "ItemData.h"
#include "EncryptFile.h"

CItemData::CItemData()
{
	this->LoadItemTypeInfo();
	this->CreateAdditionalInfo();
	this->CreateIndexTable();
}
CItemData::~CItemData()
{
}
BOOL CItemData::LoadItemTypeInfo()
{
	CEncryptFile file(0x2537);
	if (!file.Open("ini/ItemType.dat"))
		return false;
	int nCount = 0;
	ItemTypeInfo info;
	memset(&info, 0, sizeof(ItemTypeInfo));
	while (file.GetPos() < file.GetSize())
	{
		char szLine[1024] = "";
		file.ReadLine(szLine, sizeof(szLine));
		int nRead = sscanf(szLine, "%u %s %u %u %u %u %u %u %u %u %d %u %u %u %u %u %d %d %d %d %d %u %u %d %d %d %d %d %d %u %u %u %u %d %d %d %u %s %s\n",
						&info.uID, info.szName, &info.ucRequiredProfession, &info.ucRequiredWeaponSkill, &info.ucRequiredLevel, &info.ucRequiredSex,
						&info.usRequiredForce, &info.usRequiredSpeed, &info.usRequiredHealth, &info.usRequiredSoul,
						&info.ucMonopoly, &info.usWeight, &info.uPrice, &info.uLuck, &info.usMaxAttack, &info.usMinAttack,
						&info.usDefense, &info.usDexterity, &info.usDodge, &info.sLife, &info.sMana, &info.usAmount, &info.usAmountLimit,
						&info.ucStatus, &info.ucGem1, &info.ucGem2, &info.ucMagic1, &info.ucMagic2, &info.ucMagic3,
						&info.usMagicAttack, &info.usMagicDefence, &info.usRange, &info.usAttackSpeed,
						&info.ucFrayMode, &info.ucRepairMode, &info.usTypeMask, &info.uEmoneyPrice, info.szShowName, info.szDesc);
		if (nRead == 39)
		{
			OBJID idType = info.uID;
			m_mapItemType[idType] = info;
		}
	}
	return true;
}
BOOL CItemData::CreateIndexTable()
{
	char szFuseFile[]="ini/fuse.ini";
	FILE* fp = fopen(szFuseFile, "r");
	if(!fp)
	{
		::ErrorOut("can't open file:ini/Fuse.ini");
		return false;
	}
	m_mapFuse.clear();
	while(true)
	{
		char szLine[1024] = "";
		if(-1 == fscanf(fp, "%s\n", szLine))
			break;
		DWORD dwKey = 0, dwValue = 0;
		if(2 != sscanf(szLine, "%u=%u", &dwKey, &dwValue))
			continue;
		m_mapFuse[dwKey] = dwValue;
	}
	fclose(fp);
	return true;
}
BOOL CItemData::GetItemTypeInfo(OBJID idType, ItemTypeInfo& infoItemType)
{
	if (m_mapItemType.empty())
		return false;
	MAP_ITEMINFO::iterator itr = m_mapItemType.find(idType);
	if (itr == m_mapItemType.end())
		return false;
	memcpy(&infoItemType, &itr->second, sizeof(ItemTypeInfo));
	return true;
}
BOOL	CItemData::GetItemAdditionInfo(OBJID idType, DWORD dwLevel, ItemAdditionInfo& infoAdditional)
{
	memset(&infoAdditional, 0, sizeof(ItemAdditionInfo));
	__int64 i64Data = idType;
	i64Data = (i64Data << 32) + dwLevel;
	MAP_ITEMADDITIONINFO::iterator iter = m_mapItemAddition.find(i64Data);
	if(iter != m_mapItemAddition.end())
	{
		infoAdditional = iter->second;
		return true;
	}
	return false;
}
BOOL	CItemData::CreateAdditionalInfo()
{
	const char szIniFile[256] = "ini/ItemAdd.ini";
	FILE* fp = fopen(szIniFile, "r");
	if(!fp)
		return false;
	DWORD dwID = 0, dwLevel = 0, dwLife = 0, dwAttackMax = 0, dwAttackMin = 0, dwDefense = 0, dwMAttack = 0, dwMDef = 0, dwDexterity = 0, dwDodge = 0;
	int nLine = 0;
	while(true)
	{
		char szLine[1024] = "";
		int nRead = fscanf(fp, "%u %u %u %u %u %u %u %u %u %u\n", &dwID, &dwLevel, &dwLife, &dwAttackMax, &dwAttackMin,
						&dwDefense, &dwMAttack, &dwMDef, &dwDexterity, &dwDodge);
		if(-1 == nRead)
		{
			fclose(fp);
			return true;
		}
		nLine++;
		if(10 != nRead)
			continue;
		ItemAdditionInfo info;
		info.nLife		= (unsigned short)dwLife;
		info.nAttackMax	= (unsigned short)dwAttackMax;
		info.nAttackMin	= (unsigned short)dwAttackMin;
		info.nDefense	= (unsigned short)dwDefense;
		info.nMAttack	= (unsigned short)dwMAttack;
		info.nMDef		= (unsigned short)dwMDef;
		info.nDexterity	= (unsigned short)dwDexterity;
		info.nDodge		= (unsigned short)dwDodge;
		__int64 i64Data = dwID;
		i64Data = (i64Data << 32) + dwLevel;
		m_mapItemAddition[i64Data] = info;
	}
	fclose(fp);
	return true;
}
DWORD	CItemData::GetFuseInfo(DWORD dwKey)
{
	MAP_FUSE::iterator iter = m_mapFuse.find(dwKey);
	if(iter != m_mapFuse.end())
		return iter->second;
	return 0;
}
