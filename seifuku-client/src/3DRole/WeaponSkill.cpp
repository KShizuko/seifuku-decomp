#include "WeaponSkill.h"
#include "Hero.h"
#include "GameDataSet.h"

CWeaponSkillData CWeaponSkill::s_Data;

CWeaponSkill::CWeaponSkill()
{

}

CWeaponSkill::~CWeaponSkill()
{

}

int CWeaponSkill::GetWeaponType()
{
	return m_nWeaponSkillType/100*100000;
}
int	CWeaponSkill::GetWeaponSubType()
{
	return (m_nWeaponSkillType - m_nWeaponSkillType/100*100)/10*10000;
}
char* CWeaponSkill::GetName()
{
	return s_Data.GetWeaponSkillName(m_nWeaponSkillType);
}
DWORD CWeaponSkill::GetNextLevelExpRequired()
{
	return s_Data.GetExpRequired(m_dwLevel+1);
}
char* CWeaponSkill::GetDescribe()
{
	if((g_objHero.GetLev()/10) >= m_dwLevel)
	{
		const OBJID idImprove = 100089;
		strcpy(m_szDescribe, GameDataSetQuery()->GetStr(idImprove));
	}
	else if(m_dwLevel < 12)
	{
		const OBJID idImprove = 100090;
		sprintf(m_szDescribe, GameDataSetQuery()->GetStr(idImprove), (m_dwLevel-1)*10);
	}
	else
	{
		const OBJID idAttackImprove = 100091;
		sprintf(m_szDescribe, GameDataSetQuery()->GetStr(idAttackImprove), (m_dwLevel-12));
	}
	return	m_szDescribe;
}
CWeaponSkillData::CWeaponSkillData()
{
	this->Create();
	OBJID idErrorWeaponSkill = 100092;
	strcpy(m_szError, "ERROR");
}
CWeaponSkillData::~CWeaponSkillData()
{
	this->Destroy();
}
void CWeaponSkillData::Create()
{
	this->Destroy();
	const char IniFile[]="ini/WeaponSkillName.ini";
	FILE* fp = fopen(IniFile, "r");
	if(!fp)
		return;
	
	while(true)
	{
		char szWeaponSkillName[_MAX_NAMESIZE];
		DWORD dwIndex = 0;
		int nReturn = fscanf(fp, "%u,%s\n", &dwIndex, szWeaponSkillName);
		if(EOF == nReturn)
			break;
		if(2 != nReturn)
			continue;

		WeaponSkillInfo* pInfo = new WeaponSkillInfo;
		MYASSERT(pInfo);
		pInfo->dwWeaponSkillIndex = dwIndex;
		strcpy(pInfo->szWeaponSkillName, szWeaponSkillName);

		m_setWeaponSkillInfo.push_back(pInfo);
	}
	fclose(fp);


	const char IniExpFile[]="ini/WeaponSkillLevelExp.ini";
	fp = fopen(IniExpFile, "r");
	if(!fp)
		return;
	
	while(true)
	{
		DWORD dwLevel = 0;
		DWORD dwExp = 0;
		int nReturn = fscanf(fp, "%u,%u\n", &dwLevel, &dwExp);
		if(EOF == nReturn)
			break;
		if(2 != nReturn)
			continue;

		WeaponSkillLevelExp* pInfo = new WeaponSkillLevelExp;
		MYASSERT(pInfo);
		pInfo->dwLevel = dwLevel;
		pInfo->dwExp = dwExp;

		m_setWeaponSkillLevelExp.push_back(pInfo);
	}
	fclose(fp);
}
void CWeaponSkillData::Destroy()
{
	int nAmount, i;
	nAmount = m_setWeaponSkillInfo.size();
	for(i = 0; i < nAmount; i ++)
	{
		WeaponSkillInfo* pInfo = m_setWeaponSkillInfo[i];
		SAFE_DELETE(pInfo);
	}
	m_setWeaponSkillInfo.clear();

	nAmount = m_setWeaponSkillLevelExp.size();
	for(i = 0; i < nAmount; i ++)
	{
		WeaponSkillLevelExp* pInfo = m_setWeaponSkillLevelExp[i];
		SAFE_DELETE(pInfo);
	}
	m_setWeaponSkillLevelExp.clear();
}
char* CWeaponSkillData::GetWeaponSkillName(DWORD dwIndex)
{
	int nAmount = m_setWeaponSkillInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		WeaponSkillInfo* pInfo = m_setWeaponSkillInfo[i];
		if((pInfo)&&(dwIndex == pInfo->dwWeaponSkillIndex))
		{
			return pInfo->szWeaponSkillName;
		}
	}
	return m_szError;
}
DWORD CWeaponSkillData::GetExpRequired(DWORD dwLevel)
{
	int nAmount = m_setWeaponSkillLevelExp.size();
	for(int i = 0; i < nAmount; i ++)
	{
		WeaponSkillLevelExp* pInfo = m_setWeaponSkillLevelExp[i];
		if(pInfo && (pInfo->dwLevel == dwLevel))
		{
			return pInfo->dwExp;
		}
	}
	return 0;	
}
