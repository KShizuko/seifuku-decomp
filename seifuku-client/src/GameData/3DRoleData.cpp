#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <map>
#include <vector>
#include <string>
#include "BaseFunc.h"
#include "MyIni.h"
#include "3DRoleData.h"

const DWORD _ACTION_UNIVERSAL	= 999;
const DWORD _LOOKTYPE_UNIVERSAL = 999;
const DWORD _WEAPONTYPE_UNIVERSAL = 999;
const DWORD _PARTTYPE_UNIVERSAL = 999;
const DWORD _VARIABLE_UNIVERSAL = 999;

typedef struct
{
	DWORD idxRWeapon;
	DWORD idxLWeapon;
}WeaponCombinInfo;
typedef std::vector<WeaponCombinInfo>	VECTOR_WEAPONCOMBIN;

typedef std::map<__int64, ActionMapEffectInfo>	MAP_ACTIONMAPEFFECT;
typedef std::map<DWORD, std::string>	MAP_GAMESOUNDINFO;
typedef std::map<__int64, std::string>	MAP_GAME3DEFFECTINFO;

typedef std::map<std::string, CMonsterInfo>	MAP_MONSTERINFO;
typedef std::map<std::string, FlyingObjInfo>	MAP_FLYINGOBJINFO;
typedef std::vector<TerrainWeatherInfo>		VECTOR_WEATHERINFO;
typedef std::vector<StatusEffectInfo>		VECTOR_STATUSEFFECT;
typedef std::vector<TerrainNpcTypeInfo>		VECTOR_TERRAINNPC;
typedef std::vector<PetInfo>				VECTOR_PETINFO;
typedef std::vector<TextActionInfo>			VECTOR_TEXTACTION;
typedef std::map<__int64, DWORD>			MAP_ACTIONINFO;
typedef std::vector<UserHelpInfo>			VECTOR_USERHELP;
typedef std::vector<TipsInfo>				VECTOR_TIPSINFO;
typedef std::map<OBJID, GameMapInfo>		MAP_GAMEMAPINFO;
typedef std::vector<PkTitleInfo>			VECTOR_PKTITLE;
typedef std::vector<EffectInfo>				VECTOR_EFFECTINFO;
typedef std::vector<MapMagicItemInfo>		VECTOR_MAPMAGICITEM;
typedef std::map<int, CTransformInfo>		MAP_TRANSFORMINFO;
typedef std::map<int, NpcTypeInfo>			MAP_NPCTYPEINFO;
typedef std::map<int, NpcXTypeInfo>			MAP_NPCXTYPEINFO;

#include "EncryptFile.h"

class C3DRoleDataX;
static IRoleData* s_pRoleData = NULL;

class C3DRoleDataX : public IRoleData
{
public:
	C3DRoleDataX();
	virtual ~C3DRoleDataX();

	virtual void		Init ( void );
	virtual DWORD		GetActionTimeInterval ( __int64 idAction );
	virtual const ActionMapEffectInfo*	GetActionMapEffect ( int nLook, int nAction, int nTerrain ) const;
	virtual const char*	GetRolePart3DEffect ( DWORD dwLook, DWORD idxAction, DWORD idxPart, DWORD idxVar, DWORD nTable );
	virtual const char*	GetActionSound ( DWORD idxLook, DWORD idxWeapon, DWORD idxAction );
	virtual const CMonsterInfo*	GetMonsterInfoByType ( OBJID idType );
	virtual const CMonsterInfo*	GetMonsterInfo ( const char* pszMonster );
	virtual CTransformInfo*	GetTransformInfo ( int nIndex );
	virtual BOOL		CanWeaponCombin ( DWORD dwWeaponR, DWORD dwWeaponL );
	virtual NpcTypeInfo*	GetNpcTypeInfo ( OBJID idType );
	virtual NpcXTypeInfo*	GetNpcXTypeInfo ( int nType );
	virtual MapMagicItemInfo*	GetMapMagicItemInfo ( int nType );
	virtual EffectInfo*	GetEffectInfo ( char* pszIndex );
	virtual FlyingObjInfo*	GetFlyingObjInfo ( OBJID idItem, OBJID idWeaponType );
	virtual DWORD		GetStatusEffectInfoAmount ( void );
	virtual StatusEffectInfo*	GetStatusEffectInfoByIndex ( int nIndex );
	virtual const char*	GetUserHelpInfoLineByIndex ( int nProfessional, int nLevel, int nSyndicate, int nMarriage, int nMete, int nIndex );
	virtual int			GetUserHelpInfoLineAmount ( int nProfessional, int nLevel, int nSyndicate, int nMarriage, int nMete );
	virtual TerrainNpcTypeInfo*	GetTerrainNpcInfo ( OBJID idType );
	virtual int			GetTipsAmount ( void );
	virtual int			GetTipLineAmount ( int nIndex );
	virtual char*		GetTipLine ( int nTipIndex, int nLineIndex );
	virtual int			GetTextActionAmount ( void );
	virtual const char*	GetTextActionTitle ( int nIndex );
	virtual const char*	GetTextAction ( int nIndex );
	virtual PetInfo*	GetPetInfo ( int nIndex );
	virtual TerrainWeatherInfo*	GetWeatherInfo ( int nTerrain );
	virtual GameMapInfo*	GetGameMapInfo ( OBJID idMap );
	virtual char*		GetPkTitle ( int nPkValue );
	virtual CONST CActionDelayInfo*	GetActionDelayInfo ( DWORD dwKey ) CONST;
	virtual CONST CWeaponInfo*		GetWeaponInfo ( DWORD dwKey ) CONST;
	virtual CONST CActionCtrlInfo*	GetActionCtrlInfo ( DWORD dwKey ) CONST;
	virtual CONST CMediaEffectInfo*	GetMediaEffectInfo ( DWORD dwKey ) CONST;
	virtual CONST RestrainInfo*		GetRestrainInfo ( DWORD dwKey ) CONST;
	virtual IAni*		GetItemMinIcon ( OBJID idType, int nExigence );
	virtual IAni*		GetPlayerFaceIcon ( int nFace, int nExigence );
	virtual IAni*		GetNpcFaceIcon ( int nFace, int nExigence );
	virtual int			GetMaleFaceAmount ( void )		{ return m_nMaleFaceAmount; }
	virtual int			GetFemaleFaceAmount ( void )	{ return m_nFemaleFaceAmount; }
	virtual int			GetLeaderScale ( void )			{ return m_nLeaderScale; }
	virtual int			GetAssistantScale ( void )		{ return m_nAssistantScale; }
	virtual int			GetSubLeaderScale ( void )		{ return m_nSubLeaderScale; }
	virtual int			GetLeaderMaxLife ( void )		{ return m_nLeaderMaxLife; }
	virtual int			GetAssistantMaxLife ( void )	{ return m_nAssistantMaxLife; }
	virtual int			GetSubLeaderMaxLife ( void )	{ return m_nSubLeaderMaxLife; }

public:
	BOOL	CreateActionInfo ( void );
	BOOL	Create3DEffectInfo ( void );
	BOOL	Load3DEffectTable ( MAP_GAME3DEFFECTINFO& setEffect, const char* pszIniFile );
	BOOL	CreateWeaponCombinInfo ( void );
	BOOL	CreateActionMapEffectInfo ( void );
	BOOL	CreateSoundInfo ( void );
	BOOL	CreateMonsterInfo ( void );
	void	LoadCommonIni ( void );
	BOOL	CreateTransFormInfo ( void );
	BOOL	CreateNpcTypeInfo ( void );
	BOOL	CreatePkTitleInfo ( void );
	BOOL	CreateEffectInfo ( void );
	BOOL	CreateMapMagicItemInfo ( void );
	BOOL	CreateFlyObjInfo ( void );
	BOOL	CreateTerrainWeatherInfo ( void );
	BOOL	CreateStatusEffectInfo ( void );
	BOOL	CreateTerrainNpcTypeInfo ( void );
	BOOL	CreateActionDelayInfo ( void );
	BOOL	CreateWeaponInfo ( void );
	BOOL	CreateActionCtrlInfo ( void );
	BOOL	CreateMediaEffectInfo ( void );
	BOOL	CreateRestrainInfo ( void );
	BOOL	CreateTipsInfo ( void );
	BOOL	CreatePetInfo ( void );
	BOOL	CreateGameMapInfo ( void );
	BOOL	CreateUserHelpInfo ( void );
	BOOL	CreateTxtActionInfo ( void );

	int		m_nMaleFaceAmount;
	int		m_nFemaleFaceAmount;
	int		m_nLeaderScale;
	int		m_nAssistantScale;
	int		m_nSubLeaderScale;
	int		m_nLeaderMaxLife;
	int		m_nAssistantMaxLife;
	int		m_nSubLeaderMaxLife;

	std::map< DWORD, RestrainInfo >		m_setRestrainInfo;
	std::map< DWORD, CMediaEffectInfo >	m_setMediaEffectInfo;
	std::map< DWORD, CWeaponInfo >		m_setWeaponInfo;
	std::map< DWORD, CActionDelayInfo >	m_setActionDelayInfo;
	std::map< DWORD, CActionCtrlInfo >	m_setActionCtrlInfo;

	VECTOR_WEAPONCOMBIN			m_setWeaponCombinInfo;
	MAP_ACTIONMAPEFFECT			m_setMapEffectInfo;
	MAP_GAME3DEFFECTINFO		m_setGame3DEffectInfo[3];
	MAP_GAMESOUNDINFO			m_setSoundInfo;
	VECTOR_PETINFO				m_setPetInfo;
	VECTOR_TEXTACTION			m_setTextAction;
	VECTOR_TIPSINFO				m_setTipsInfo;
	VECTOR_TERRAINNPC			m_setTerrainNpcTypeInfo;
	VECTOR_USERHELP				m_setUserHelpInfo;
	VECTOR_STATUSEFFECT			m_setStatusEffectInfo;
	VECTOR_WEATHERINFO			m_setTerrainWeatherInfo;
	MAP_FLYINGOBJINFO			m_setFlyObjInfo;
	VECTOR_EFFECTINFO			m_setEffectInfo;
	VECTOR_MAPMAGICITEM			m_setMapMagicItem;
	VECTOR_PKTITLE				m_setPkTitle;
	MAP_NPCTYPEINFO				m_setNpcType;
	MAP_NPCXTYPEINFO			m_setNpcXType;
	MAP_GAMEMAPINFO				m_setGameMapInfo;
	MAP_ACTIONINFO				m_setActionInfo;
	MAP_TRANSFORMINFO			m_setTransFormInfo;
	MAP_MONSTERINFO				m_setMonsterInfo;
};

IRoleData* RoleDataQuery ( void )
{
	if (!s_pRoleData)
		s_pRoleData = new C3DRoleDataX;
	return s_pRoleData;
}

void RoleDataDestroy ( void )
{
	if (s_pRoleData)
	{
		try { delete s_pRoleData; } catch(...) {}
		s_pRoleData = NULL;
	}
}

C3DRoleDataX::C3DRoleDataX()
{
}

C3DRoleDataX::~C3DRoleDataX()
{
}

void C3DRoleDataX::Init ( void )
{
	this->CreateActionInfo();
	this->Create3DEffectInfo();
	this->CreateWeaponCombinInfo();
	this->CreateActionMapEffectInfo();
	this->CreateSoundInfo();
	this->CreateMonsterInfo();
	this->LoadCommonIni();
	this->CreateTransFormInfo();
	this->CreateNpcTypeInfo();
	this->CreatePkTitleInfo();
	this->CreateEffectInfo();
	this->CreateMapMagicItemInfo();
	this->CreateFlyObjInfo();
	this->CreateTerrainWeatherInfo();
	this->CreateStatusEffectInfo();
	this->CreateUserHelpInfo();
	this->CreateTerrainNpcTypeInfo();
	this->CreateTipsInfo();
	this->CreatePetInfo();
	this->CreateGameMapInfo();
	this->CreateActionDelayInfo();
	this->CreateWeaponInfo();
	this->CreateActionCtrlInfo();
	this->CreateMediaEffectInfo();
	this->CreateRestrainInfo();
}

BOOL C3DRoleDataX::CreateActionInfo ( void )
{
	return true;
}

BOOL C3DRoleDataX::CreateWeaponCombinInfo()
{
	m_setWeaponCombinInfo.clear();
	const char szIniFile[]="ini/WeaponCombin.ini";
	FILE* fp =fopen(szIniFile, "r");
	if (!fp)
		return false;
	while (true)
	{
		DWORD idxRWeapon = 0, idxLWeapon = 0;
		int rval = fscanf(fp, "%u.%u\n", &idxRWeapon, &idxLWeapon);
		if (rval == EOF)
			break;
		WeaponCombinInfo info;
		info.idxRWeapon = idxRWeapon;
		info.idxLWeapon = idxLWeapon;
		m_setWeaponCombinInfo.push_back(info);
	}
	fclose(fp);
	return true;
}
BOOL C3DRoleDataX::CanWeaponCombin(DWORD idxRWeapon, DWORD idxLWeapon)
{
	int nAmount = m_setWeaponCombinInfo.size();
	for (int i = 0; i < nAmount; i++)
	{
		if (m_setWeaponCombinInfo[i].idxRWeapon == idxRWeapon && m_setWeaponCombinInfo[i].idxLWeapon == idxLWeapon)
			return true;
	}
	return false;
}
BOOL C3DRoleDataX::CreateActionMapEffectInfo()
{
	m_setMapEffectInfo.clear();
	const char szIniFile[] = "ini/ActionMap3DEffect.ini";
	FILE* fp = fopen(szIniFile, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", szIniFile);
		return false;
	}
	while (true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		int nIndex = atoi(szTitle);
		ActionMapEffectInfo info;
		BOOL bSucRead = true;
		bSucRead &= fscanf(fp, "Effect=%s\n", info.szEffect);
		bSucRead &= fscanf(fp, "ShowTime=%d\n", &info.nShowTime);
		bSucRead &= fscanf(fp, "DirEnable=%d\n", &info.bDirEnable);
		if (!bSucRead)
		{
			::ErrorMsg("error data: %s of %s", szTitle, szIniFile);
			return false;
		}
		m_setMapEffectInfo[(DWORD) nIndex] = info;
	}
	fclose(fp);
	return true;
}
const ActionMapEffectInfo* C3DRoleDataX::GetActionMapEffect (int nLook, int nAction, int nTerrain) const
{
	const int nWildCard = 999;
	__int64 idx = (__int64)nLook*1000000 + (__int64)nAction*1000 + nTerrain;
	MAP_ACTIONMAPEFFECT::const_iterator iter = m_setMapEffectInfo.find(idx);
	if (iter != m_setMapEffectInfo.end())
		return &iter->second;
	idx = (__int64)nWildCard*1000000 + (__int64)nAction*1000 + nTerrain;
	iter = m_setMapEffectInfo.find(idx);
	if (iter != m_setMapEffectInfo.end())
		return &iter->second;
	idx = (__int64)nLook*1000000 + (__int64)nWildCard*1000 + nTerrain;
	iter = m_setMapEffectInfo.find(idx);
	if (iter != m_setMapEffectInfo.end())
		return &iter->second;
	idx = (__int64)nLook*1000000 + (__int64)nAction*1000 + nWildCard;
	iter = m_setMapEffectInfo.find(idx);
	if (iter != m_setMapEffectInfo.end())
		return &iter->second;
	idx = (__int64)nWildCard*1000000 + (__int64)nWildCard*1000 + nTerrain;
	iter = m_setMapEffectInfo.find(idx);
	if (iter != m_setMapEffectInfo.end())
		return &iter->second;
	idx = (__int64)nWildCard*1000000 + (__int64)nAction*1000 + nWildCard;
	iter = m_setMapEffectInfo.find(idx);
	if (iter != m_setMapEffectInfo.end())
		return &iter->second;
	idx = (__int64)nLook*1000000 + (__int64)nWildCard*1000 + nWildCard;
	iter = m_setMapEffectInfo.find(idx);
	if (iter != m_setMapEffectInfo.end())
		return &iter->second;
	return NULL;
}
BOOL C3DRoleDataX::CreateSoundInfo(void)
{
	m_setSoundInfo.clear();
	const char szIniFile[]="ini/ActionSound.ini";
	FILE* fp = fopen(szIniFile, "r");
	IF_NOT (fp)
		return false;
	while (true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		DWORD idxLook, idxAction, idxWeapon;
		char szSound[256] = "";
		if (4 != sscanf(szLine, "%u.%u.%u=%s", &idxLook, &idxWeapon, &idxAction, szSound))
		{
			::ErrorMsg("error line:[%s] in %s!", szLine, szIniFile);
			break;
		}
		MYASSERT (strlen(szSound) < 128);
		DWORD id = idxLook*1000000 + idxWeapon*1000+idxAction;
		m_setSoundInfo[id]=szSound;
	}
	fclose(fp);
	return true;
}
const char* C3DRoleDataX::GetActionSound(DWORD idxLook, DWORD idxWeapon, DWORD idxAction)
{
	DWORD id = idxLook*1000000 + idxWeapon*1000+idxAction;
	std::map<DWORD, std::string>::iterator iter = m_setSoundInfo.find(id);
	if (iter != m_setSoundInfo.end())
	{
		return iter->second.c_str();
	}
	id = idxLook*1000000 + 999*1000+idxAction;
	iter = m_setSoundInfo.find(id);
	if (iter != m_setSoundInfo.end())
	{
		return iter->second.c_str();
	}
	return NULL;
}
BOOL C3DRoleDataX::CreateMonsterInfo(void)
{
	m_setMonsterInfo.clear();
	CONST CHAR INI_FILE[] = "ini/Monster.dat";
	CEncryptFile file(0x2537);
	if (!file.Open(INI_FILE))
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return false;
	}
	DWORD dwPos = 0;
	while (true)
	{
		char szLine[2048] = "";
		if (!::ReadLine(file.GetBuffer(), file.GetSize(), &dwPos, szLine, sizeof(szLine)))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		CMonsterInfo info;
		char szEffect[256] = "";
		char szSound[256] = "";
		BOOL bSucRead = true;
#define READ_MONSTER_LINE(fmt, p) \
		if (!::ReadLine(file.GetBuffer(), file.GetSize(), &dwPos, szLine, sizeof(szLine))) bSucRead = false; \
		else bSucRead &= sscanf(szLine, fmt, p); \
		if (!bSucRead) break;
		do
		{
			READ_MONSTER_LINE("SizeAdd=%d\n",     &info.nAddSize);
			READ_MONSTER_LINE("ZoomPercent=%d\n", &info.nZoomPercent);
			READ_MONSTER_LINE("MaxLife=%d\n",     &info.nMaxLife);
			READ_MONSTER_LINE("Level=%d\n",       &info.nLevel);
			READ_MONSTER_LINE("BornAction=%d\n",  &info.nBornAction);
			READ_MONSTER_LINE("BornEffect=%s\n",  szEffect);
			info.strBornEffect = szEffect;
			READ_MONSTER_LINE("BornSound=%s\n",   szSound);
			info.strBornSound = szSound;
			READ_MONSTER_LINE("ActResCtrl=%d\n",  &info.nActResCtrl);
			READ_MONSTER_LINE("ASB=%d\n",         &info.nAsb);
			READ_MONSTER_LINE("ADB=%d\n",         &info.nAdb);
			READ_MONSTER_LINE("BodyType=%d\n",    &info.nBodyType);
			READ_MONSTER_LINE("TypeID=%u\n",      &info.idType);
			READ_MONSTER_LINE("AntiType=%u\n",    &info.dwAntiType);
			READ_MONSTER_LINE("Armet=%u\n",       &info.idArmet);
			READ_MONSTER_LINE("RWeapon=%u\n",     &info.idRWeapon);
			READ_MONSTER_LINE("LWeapon=%u\n",     &info.idLWeapon);
			READ_MONSTER_LINE("Misc=%u\n",        &info.idMisc);
			READ_MONSTER_LINE("Mount=%u\n",       &info.idMount);
		} while (false);
#undef READ_MONSTER_LINE
		if (!bSucRead)
		{
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			return false;
		}
		info.strName = szTitle;
		m_setMonsterInfo[szTitle] = info;
	}
	return true;
}
void C3DRoleDataX::LoadCommonIni()
{
	const char szIniFile[]="ini/Common.ini";
	char szTitle[64];
	char szSubtitle[64];
	strcpy(szTitle, "Playerface");
	strcpy(szSubtitle, "MaleFaceAmount");
	MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, m_nMaleFaceAmount));
	strcpy(szSubtitle, "FemaleFaceAmount");
	MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, m_nFemaleFaceAmount));
	strcpy(szTitle, "Statuary");
	strcpy(szSubtitle, "LeaderScale");
	MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, m_nLeaderScale));
	strcpy(szSubtitle, "LeaderMaxLife");
	MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, m_nLeaderMaxLife));
	strcpy(szSubtitle, "AssistantScale");
	MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, m_nAssistantScale));
	strcpy(szSubtitle, "AssistantMaxLife");
	MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, m_nAssistantMaxLife));
	strcpy(szSubtitle, "SubLeaderScale");
	MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, m_nSubLeaderScale));
	strcpy(szSubtitle, "SubLeaderMaxLife");
	MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, m_nSubLeaderMaxLife));
}
BOOL C3DRoleDataX::CreateTransFormInfo()
{
	m_setTransFormInfo.clear();
	CONST char INI_FILE[] = "ini/Transform.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return false;
	}
	while (true)
	{
		char szLine[1024] = "";
		if (!fgets(szLine, 1024, fp))
			break;
		szLine[strlen(szLine)-1] = '\0';
		int nIndex = 0;
		if (1 != sscanf(szLine, "[%d]", &nIndex))
			continue;

		CTransformInfo info;
		memset(&info, 0L, sizeof(info));
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "Jump=%u\n", &info.bCanJump);
		bSuc &= fscanf(fp, "AddSize=%d\n", &info.nAddSize);
		bSuc &= fscanf(fp, "Scale=%d\n", &info.nScale);
		bSuc &= fscanf(fp, "Look=%d\n", &info.nLook);
		bSuc &= fscanf(fp, "Armet=%u\n", &info.idArmet);
		bSuc &= fscanf(fp, "RWeapon=%u\n", &info.idRWeapon);
		bSuc &= fscanf(fp, "LWeapon=%u\n", &info.idLWeapon);
		bSuc &= fscanf(fp, "Misc=%u\n", &info.idMisc);
		bSuc &= fscanf(fp, "Mount=%u\n", &info.idMount);
		if (!bSuc)
		{
			::ErrorMsg("data error in [%d] of %s", nIndex, INI_FILE);
			break;
		}
		m_setTransFormInfo[nIndex] = info;
	}
	fclose(fp);
	return true;
}
CTransformInfo* C3DRoleDataX::GetTransformInfo(int nIndex)
{
	MAP_TRANSFORMINFO::iterator iter = m_setTransFormInfo.find(nIndex);
	if (iter == m_setTransFormInfo.end())
		return NULL;
	return &iter->second;
}
BOOL C3DRoleDataX::CreateNpcTypeInfo()
{
	m_setNpcType.clear();
	m_setNpcXType.clear();
	CONST CHAR INI_FILE[] = "ini/Npc.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return false;
	}
	while(true)
	{
		char szLine[2048] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		NpcTypeInfo info;
		memset(&info, 0L, sizeof(NpcTypeInfo));
		info.nType = atoi(szTitle+strlen("NpcType"));
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "Name=%s\n", info.szNpcName);
		bSuc &= fscanf(fp, "SimpleObjID=%u\n", &info.idSimpleObj);
		bSuc &= fscanf(fp, "StandByMotion=%u\n", &info.idMotionStandBy);
		bSuc &= fscanf(fp, "BlazeMotion=%u\n", &info.idMotionBlaze);
		bSuc &= fscanf(fp, "RestMotion=%u\n", &info.idMotionRest);
		bSuc &= fscanf(fp, "Effect=%s\n", info.szEffectIndex);
		bSuc &= fscanf(fp, "ASB=%d\n", &info.nAsb);
		bSuc &= fscanf(fp, "ADB=%d\n", &info.nAdb);
		bSuc &= fscanf(fp, "FixDir=%u\n", &info.bFixDir);
		if (!bSuc)
		{
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			break;
		}
		m_setNpcType[info.nType] = info;
	}
	fclose(fp);

	CONST CHAR INI_FILE_X[] = "ini/NpcX.ini";
	fp = fopen(INI_FILE_X, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE_X);
		return false;
	}
	while(true)
	{
		char szLine[2048] = "";
		if (!fgets(szLine, 2048, fp))
			break;
		int nIndex = 0;
		if (1 != sscanf(szLine, "[%d]", &nIndex))
			continue;

		NpcXTypeInfo info;
		memset(&info, 0L, sizeof(info));
		info.nType = nIndex;
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "Name=%s\n", info.szNpcName);
		bSuc &= fscanf(fp, "AddSize=%d\n", &info.nAddSize);
		bSuc &= fscanf(fp, "Scale=%d\n", &info.nScale);
		bSuc &= fscanf(fp, "FixDir=%u\n", &info.bFixDir);
		bSuc &= fscanf(fp, "Look=%d\n", &info.nLook);
		bSuc &= fscanf(fp, "Head=%d\n", &info.nHead);
		bSuc &= fscanf(fp, "Hair=%d\n", &info.nHair);
		bSuc &= fscanf(fp, "Armet=%u\n", &info.idArmet);
		bSuc &= fscanf(fp, "Armor=%u\n", &info.idArmor);
		bSuc &= fscanf(fp, "RWeapon=%u\n", &info.idRWeapon);
		bSuc &= fscanf(fp, "LWeapon=%u\n", &info.idLWeapon);
		bSuc &= fscanf(fp, "Misc=%u\n", &info.idMisc);
		bSuc &= fscanf(fp, "Mount=%u\n", &info.idMount);
		if (!bSuc)
		{
			::ErrorMsg("error data in [%d] of %s", nIndex, INI_FILE_X);
			break;
		}
		m_setNpcXType[nIndex] = info;
	}
	fclose(fp);
	return true;
}
NpcTypeInfo* C3DRoleDataX::GetNpcTypeInfo(OBJID idType)
{
	if (m_setNpcType.empty())
		return NULL;
	MAP_NPCTYPEINFO::iterator iter = m_setNpcType.find(idType);
	if (iter == m_setNpcType.end())
		return &m_setNpcType.begin()->second;
	return &iter->second;
}
NpcXTypeInfo* C3DRoleDataX::GetNpcXTypeInfo(int nType)
{
	if (m_setNpcXType.empty())
		return NULL;
	MAP_NPCXTYPEINFO::iterator iter = m_setNpcXType.find(nType);
	if (iter == m_setNpcXType.end())
		return &m_setNpcXType.begin()->second;
	return &iter->second;
}
BOOL	C3DRoleDataX::CreatePkTitleInfo()
{
	m_setPkTitle.clear();
	const char szIniFile[]="ini/Common.ini";
	char szTitle[]="PkTitle";
	char szSubtitle[256] = "";
	int nAmount = 0;
	MYASSERT(::IniDataGet(szIniFile, "PkTitle", "Amount", nAmount));
	if (nAmount <= 0)
		return true;
	for(int i = 0; i < nAmount; i ++)
	{
		PkTitleInfo info;
		memset(&info, 0, sizeof(info));
		sprintf(szSubtitle, "PkValueMax%d", i);
		MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, info.nMaxPkValue));
		sprintf(szSubtitle, "PkValueMin%d", i);
		MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, info.nMinPkValue));
		sprintf(szSubtitle, "PkTitle%d", i);
		MYASSERT(::IniStrGet(szIniFile, szTitle, szSubtitle, info.szTitle));
		m_setPkTitle.push_back(info);
	}
	return true;
}
char* C3DRoleDataX::GetPkTitle(int nPkValue)
{
	for(int i = 0; i < m_setPkTitle.size(); i ++)
	{
		PkTitleInfo& info = m_setPkTitle[i];
		if(nPkValue <= info.nMaxPkValue && nPkValue >= info.nMinPkValue)
		{
			return info.szTitle;
		}
	}
	return NULL;
}
BOOL C3DRoleDataX::CreateEffectInfo()
{
	m_setEffectInfo.clear();
	CONST CHAR INI_FILE[] = "ini/Effect.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return false;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		EffectInfo info;
		memset(&info, 0L, sizeof(EffectInfo));
		strcpy(info.szTitle, szTitle);
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "AniTitle=%s\n", info.szAniTitle);
		bSuc &= fscanf(fp, "LoopTime=%d\n", &info.dwLoopTime);
		bSuc &= fscanf(fp, "FrameInterval=%d\n", &info.dwFrameInterval);
		bSuc &= fscanf(fp, "ShowWay=%d\n", &info.dwShowWay);
		bSuc &= fscanf(fp, "LoopInterval=%d\n", &info.dwLoopInterval);
		bSuc &= fscanf(fp, "OffsetX=%d\n", &info.nOffsetX);
		bSuc &= fscanf(fp, "OffsetY=%d\n", &info.nOffsetY);
		bSuc &= fscanf(fp, "Exigence=%d\n", &info.nExigence);
		bSuc &= fscanf(fp, "Delay=%d\n", &info.dwDelay);
		if (!bSuc)
		{
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			break;
		}
		m_setEffectInfo.push_back(info);
	}
	fclose(fp);
	return true;
}
EffectInfo* C3DRoleDataX::GetEffectInfo(char* pszIndex)
{
	if(!pszIndex)
		return NULL;
	int nAmount = m_setEffectInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		EffectInfo& info = m_setEffectInfo[i];
		if(strcmp(info.szTitle, pszIndex) == 0)
		{
			return &info;
		}
	}
	return NULL;
}
BOOL C3DRoleDataX::CreateMapMagicItemInfo()
{
	m_setMapMagicItem.clear();
	CONST CHAR INI_FILE[] = "ini/MapMagicItem.ini";
	FILE* fp = fopen(INI_FILE, "r");
	IF_NOT (fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return false;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		int nType = 0;
		if (1 != sscanf(szLine, "[Type%d]", &nType))
			continue;
		MapMagicItemInfo info;
		memset(&info, 0L, sizeof(MapMagicItemInfo));
		info.idType = nType;
		BOOL bSucRead = true;
		bSucRead &= fscanf(fp, "Start=%s\n", info.szStart);
		bSucRead &= fscanf(fp, "Last=%s\n", info.szLast);
		bSucRead &= fscanf(fp, "End=%s\n", info.szEnd);
		if (!bSucRead)
		{
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			break;
		}
		m_setMapMagicItem.push_back(info);
	}
	fclose(fp);
	return true;
}
MapMagicItemInfo*	C3DRoleDataX::GetMapMagicItemInfo(int nType)
{
	int nAmount = m_setMapMagicItem.size();
	for(int i = 0; i < nAmount; i ++)
	{
		if(m_setMapMagicItem[i].idType == nType)
			return &m_setMapMagicItem[i];
	}
	return NULL;
}
BOOL	C3DRoleDataX::CreateFlyObjInfo()
{
	m_setFlyObjInfo.clear();
	CONST CHAR INI_FILE[] = "ini/3DFlyingObj.ini";
	FILE* fp = fopen(INI_FILE, "r");
	IF_NOT (fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return false;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		DWORD dwID = 0, dwWeapon = 0;
		if (2 != sscanf(szLine, "[%u.%u]", &dwID, &dwWeapon))
			continue;
		FlyingObjInfo info;
		memset(&info, 0L, sizeof(FlyingObjInfo));
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "SimpleObjID=%u\n", &info.idSimpleObj);
		bSuc &= fscanf(fp, "EffectIndex=%s\n", info.szFlyEffect);
		bSuc &= fscanf(fp, "FlyingSound=%s\n", info.szFlySound);
		bSuc &= fscanf(fp, "HitSound=%s\n", info.szHitSound);
		bSuc &= fscanf(fp, "TargetEffect=%s\n", info.szTargetEffect);
		if (!bSuc)
		{
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			break;
		}
		char szKey[256] = "";
		sprintf(szKey, "%u.%u", dwID, dwWeapon);
		std::string strKey = szKey;
		m_setFlyObjInfo[strKey] = info;
	}
	fclose(fp);
	return true;
}
FlyingObjInfo*	C3DRoleDataX::GetFlyingObjInfo(OBJID idItem, OBJID idWeaponType)
{
	char szKey[256] = "";
	sprintf(szKey, "%u.%u", idWeaponType, idItem);
	MAP_FLYINGOBJINFO::iterator iter = m_setFlyObjInfo.find(szKey);
	if (iter != m_setFlyObjInfo.end())
		return &iter->second;
	sprintf(szKey, "%u.%u", 999999, idItem);
	iter = m_setFlyObjInfo.find(szKey);
	if (iter != m_setFlyObjInfo.end())
		return &iter->second;
	sprintf(szKey, "%u.%u", idWeaponType, 999999);
	iter = m_setFlyObjInfo.find(szKey);
	if (iter != m_setFlyObjInfo.end())
		return &iter->second;
	if (m_setFlyObjInfo.empty())
		return NULL;
	return &m_setFlyObjInfo.begin()->second;
}
BOOL	C3DRoleDataX::CreateTerrainWeatherInfo()
{
	TerrainWeatherInfo infoWeather;
	char szIni[]="ini/weather.ini";
	FILE* fp = fopen(szIni, "r");
	if(!fp)
		return false;
	while(!feof(fp))
	{
		int nReturn = fscanf(fp, "%d %d %d %d %u\n", &infoWeather.nTerrain, &infoWeather.nType, &infoWeather.nWindDir, &infoWeather.nLevel, &infoWeather.dwColor);
		if(5 == nReturn)
			m_setTerrainWeatherInfo.push_back(infoWeather);
	}
	fclose(fp);
	return true;
}
TerrainWeatherInfo*	C3DRoleDataX::GetWeatherInfo(int nTerrain)
{
	int nAmount = m_setTerrainWeatherInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		if(m_setTerrainWeatherInfo[i].nTerrain == nTerrain)
			return &m_setTerrainWeatherInfo[i];
	}
	return NULL;
}
BOOL C3DRoleDataX::CreateStatusEffectInfo()
{
	char szIni[]="ini/statuseffect.ini";
	StatusEffectInfo info;
	FILE* fp = fopen(szIni, "r");
	IF_NOT (fp)
		return false;
	while(!feof(fp))
	{
		memset(&info, 0, sizeof(StatusEffectInfo));
		int nReturn = fscanf(fp, "%x %s %s\n", &info.dwStatus, info.sz3DEffect, info.sz2DEffect);
		if(3 == nReturn)
			m_setStatusEffectInfo.push_back(info);
	}
	fclose(fp);
	return true;
}
StatusEffectInfo*	C3DRoleDataX::GetStatusEffectInfoByIndex(int nIndex)
{
	int nAmount = m_setStatusEffectInfo.size();
	if(nIndex < 0 || nIndex >= nAmount)
		return NULL;
	return &m_setStatusEffectInfo[nIndex];
}
DWORD C3DRoleDataX::GetStatusEffectInfoAmount()
{
	return m_setStatusEffectInfo.size();
}
BOOL	C3DRoleDataX::CreateTerrainNpcTypeInfo()
{
	CONST CHAR INI_FILE[] = "ini/TerrainNpc.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return false;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		TerrainNpcTypeInfo info;
		info.nType = atoi(szTitle+strlen("NpcType"));
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "Name=%s\n", info.szNpcName);
		for(int i = 0; i < 8; i ++)
		{
			char szFile[_MAX_PATH] = "";
			int nDir = 0;
			fscanf(fp, "Dir%d=%s\n", &nDir, szFile);
			std::string str = szFile;
			info.setStr.push_back(str);
		}
		if (!bSuc)
		{
			fclose(fp);
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			break;
		}
		m_setTerrainNpcTypeInfo.push_back(info);
	}
	fclose(fp);
	return true;
}
TerrainNpcTypeInfo*			C3DRoleDataX::GetTerrainNpcInfo(OBJID idType)
{
	int nAmount = m_setTerrainNpcTypeInfo.size();
	for(int i = 0; i < nAmount; i ++)
	{
		if(m_setTerrainNpcTypeInfo[i].nType == idType)
		{
			return &m_setTerrainNpcTypeInfo[i];
		}
	}
	if(nAmount > 0)
		return &m_setTerrainNpcTypeInfo[0];
	return NULL;
}
BOOL C3DRoleDataX::CreateActionDelayInfo()
{
	m_setActionDelayInfo.clear();
	CONST CHAR INI_FILE[] = "ini/ActionDelay.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return FALSE;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		DWORD dwKey = (DWORD) atoi(szTitle);
		CActionDelayInfo info;
		BOOL bSuc = TRUE;
		bSuc &= fscanf(fp, "WoundDelay=%u\n", &info.tmWoundDelay);
		bSuc &= fscanf(fp, "BlockDelay=%u\n", &info.tmBlockDelay);
		bSuc &= fscanf(fp, "DieDelay=%u\n",	&info.tmDieDelay);
		if (!bSuc)
		{
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			return FALSE;
		}
		m_setActionDelayInfo[dwKey] = info;
	}
	fclose(fp);
	return TRUE;
}
CONST CActionDelayInfo*		C3DRoleDataX::GetActionDelayInfo	(DWORD dwKey) CONST
{
	std::map< DWORD, CActionDelayInfo >::const_iterator iter = m_setActionDelayInfo.find(dwKey);
	if (iter == m_setActionDelayInfo.end())
		return NULL;

	return &((*iter).second);
}
BOOL C3DRoleDataX::CreateWeaponInfo()
{
	m_setWeaponInfo.clear();
	CONST CHAR INI_FILE[] = "ini/WeaponEffect.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return FALSE;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		DWORD dwKey = (DWORD) atoi(szTitle);
		CWeaponInfo info;
		BOOL bSuc = TRUE;
		char szEffect[256] = "";
		bSuc &= fscanf(fp, "HitEffect=%s\n", szEffect);
		info.strHitEffect = szEffect;
		if (info.strHitEffect == "none")
			info.strHitEffect = "";
		char szSound[256] = "";
		bSuc &= fscanf(fp, "HitSound=%s\n", szSound);
		info.strHitSound = szSound;
		if (info.strHitSound == "none")
			info.strHitSound = "";
		bSuc &= fscanf(fp, "BlkEffect=%s\n", szEffect);
		info.strBlkEffect = szEffect;
		if (info.strBlkEffect == "none")
			info.strBlkEffect = "";
		bSuc &= fscanf(fp, "BlkSound=%s\n", szSound);
		info.strBlkSound = szSound;
		if (info.strBlkSound == "none")
			info.strBlkSound = "";
		if (!bSuc)
		{
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			return FALSE;
		}
		m_setWeaponInfo[dwKey] = info;
	}
	fclose(fp);
	return TRUE;
}
CONST CWeaponInfo* C3DRoleDataX::GetWeaponInfo (DWORD dwKey) CONST
{
	std::map< DWORD, CWeaponInfo >::const_iterator iter = m_setWeaponInfo.find(dwKey);
	if (iter == m_setWeaponInfo.end())
		return NULL;

	return &((*iter).second);
}
BOOL C3DRoleDataX::CreateActionCtrlInfo()
{
	m_setActionCtrlInfo.clear();
	CONST CHAR INI_FILE[] = "ini/ActionCtrl.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return FALSE;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		DWORD dwKey = (DWORD) atoi(szTitle);
		CActionCtrlInfo info;
		BOOL bSuc = TRUE;
		bSuc &= fscanf(fp, "Section=%d\n", &info.nSection);
		info.nSection %= CActionCtrlInfo::MAX_ACTION_SECTION;
		for (int i = 0; i < info.nSection; i++)
		{
			char szFormat[256] = "";
			sprintf(szFormat, "TimePercent%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szFormat, &info.nTimePercent[i]);
			sprintf(szFormat, "MovePercent%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szFormat, &info.nMovePercent[i]);
		}
		bSuc &= fscanf(fp, "Visible=%u\n", &info.bVisible);
		if (!bSuc)
		{
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			return FALSE;
		}
		m_setActionCtrlInfo[dwKey] = info;
	}
	fclose(fp);
	return TRUE;
}
CONST CActionCtrlInfo* C3DRoleDataX::GetActionCtrlInfo (DWORD dwKey) CONST
{
	std::map< DWORD, CActionCtrlInfo >::const_iterator iter = m_setActionCtrlInfo.find(dwKey);
	if (iter != m_setActionCtrlInfo.end())
		return &((*iter).second);
	dwKey = dwKey%1000000 + 999*1000000;
	iter = m_setActionCtrlInfo.find(dwKey);
	if (iter != m_setActionCtrlInfo.end())
		return &((*iter).second);
	dwKey = dwKey/1000000%1000*1000000 + 999*1000 + dwKey%1000;
	iter = m_setActionCtrlInfo.find(dwKey);
	if (iter != m_setActionCtrlInfo.end())
		return &((*iter).second);
	dwKey = dwKey/1000*1000 + 999;
	iter = m_setActionCtrlInfo.find(dwKey);
	if (iter != m_setActionCtrlInfo.end())
		return &((*iter).second);
	dwKey = dwKey%1000 + 999*1000 + 999*1000000;
	iter = m_setActionCtrlInfo.find(dwKey);
	if (iter != m_setActionCtrlInfo.end())
		return &((*iter).second);
	dwKey = 999*1000000 + dwKey/1000%1000*1000 + 999;
	iter = m_setActionCtrlInfo.find(dwKey);
	if (iter != m_setActionCtrlInfo.end())
		return &((*iter).second);
	dwKey = dwKey/1000000%1000*1000000 + 999*1000 + 999;
	iter = m_setActionCtrlInfo.find(dwKey);
	if (iter != m_setActionCtrlInfo.end())
		return &((*iter).second);
	return NULL;
}
BOOL C3DRoleDataX::CreateMediaEffectInfo()
{
	m_setMediaEffectInfo.clear();
	CONST CHAR INI_FILE[] = "ini/MediaEffect.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return FALSE;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		DWORD dwKey = (DWORD) atoi(szTitle);
		CMediaEffectInfo info;
		BOOL bSuc = TRUE;
		char szEffect[256] = "";
		bSuc &= fscanf(fp, "Effect=%s\n", szEffect);
		info.strEffect = szEffect;
		if (info.strEffect == "none")
			info.strEffect = "";
		char szSound[256] = "";
		bSuc &= fscanf(fp, "Sound=%s\n", szSound);
		info.strSound = szSound;
		if (info.strSound == "none")
			info.strSound = "";
		if (!bSuc)
		{
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			return FALSE;
		}
		m_setMediaEffectInfo[dwKey] = info;
	}
	fclose(fp);
	return TRUE;
}
CONST CMediaEffectInfo* C3DRoleDataX::GetMediaEffectInfo (DWORD dwKey) CONST
{
	std::map< DWORD, CMediaEffectInfo >::const_iterator iter = m_setMediaEffectInfo.find(dwKey);
	if (iter == m_setMediaEffectInfo.end())
		return NULL;

	return &((*iter).second);
}
BOOL C3DRoleDataX::CreateRestrainInfo()
{
	m_setRestrainInfo.clear();
	CONST CHAR INI_FILE[] = "ini/restrain.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return FALSE;
	}
	while(true)
	{
		char szLine[1024] = "";
		DWORD dwKey = 0, dwMultiple = 0;
		int nRet = fscanf(fp, "%u %u %s\n", &dwKey, &dwMultiple, szLine);
		if (EOF == nRet)
			break;
		if (3 != nRet)
			continue;
		std::string strName = szLine;
		RestrainInfo& info = m_setRestrainInfo[dwKey];
		info.dwMultiple	= dwMultiple;
		info.strName	= strName;
	}
	fclose(fp);
	return m_setRestrainInfo.size() > 0;
}
CONST RestrainInfo* C3DRoleDataX::GetRestrainInfo (DWORD dwKey) CONST
{
	std::map< DWORD, RestrainInfo >::const_iterator iter = m_setRestrainInfo.find(dwKey);
	if (iter == m_setRestrainInfo.end())
		return NULL;

	return &((*iter).second);
}
BOOL	C3DRoleDataX::CreateTipsInfo()
{
	m_setTipsInfo.clear();
	const char szDataFile[] = "ini/tips.dat";
	FILE* fp = fopen(szDataFile, "rb");
	IF_NOT (fp)
		return false;
	char szHeader[32] = "";
	fread(szHeader, sizeof(char), 32, fp);
	DWORD dwVersion = 0;
	fread(&dwVersion, sizeof(DWORD), 1, fp);
	DWORD dwRecordAmount = 0;
	fread(&dwRecordAmount, sizeof(DWORD), 1, fp);
	for(DWORD dwCounter = 0; dwCounter < dwRecordAmount; dwCounter++)
	{
		TipsInfo info;
		DWORD dwStrAmount = 0;
		fread(&dwStrAmount, sizeof(DWORD), 1, fp);
		for(DWORD dwStrCounter = 0; dwStrCounter < dwStrAmount; dwStrCounter++)
		{
			DWORD dwLength = 0;
			fread(&dwLength, sizeof(DWORD), 1, fp);
			char szStr[256];
			fread(szStr, sizeof(char), dwLength, fp);
			std::string str = szStr;
			info.m_setStr.push_back(str);
		}
		m_setTipsInfo.push_back(info);
	}
	fclose(fp);
	return true;
}
int							C3DRoleDataX::GetTipsAmount()
{
	return m_setTipsInfo.size();
}
int							C3DRoleDataX::GetTipLineAmount(int nIndex)
{
	int nAmount = m_setTipsInfo.size();
	if((nIndex < 0) || (nIndex >= nAmount))
		return 0;
	return m_setTipsInfo[nIndex].m_setStr.size();
}
char*						C3DRoleDataX::GetTipLine(int nTipIndex, int nLineIndex)
{
	int nAmount = m_setTipsInfo.size();
	if((nTipIndex < 0) || (nTipIndex >= nAmount))
		return NULL;
	TipsInfo* pInfo = &m_setTipsInfo[nTipIndex];
	nAmount = pInfo->m_setStr.size();
	if((nLineIndex < 0) || (nLineIndex >= nAmount))
		return NULL;
	return (char*)pInfo->m_setStr[nLineIndex].c_str();
}
BOOL	C3DRoleDataX::CreatePetInfo()
{
	m_setPetInfo.clear();
 	const char szPetData[]	=	"ini/pet.dat";
	FILE* fp = fopen(szPetData, "rb");
	if(!fp)
		return false;
	int nRecordAmount = 0;
	fread(&nRecordAmount, sizeof(int), 1, fp);
	for(int i = 0; i < nRecordAmount; i ++)
	{
		PetInfo info;
		DWORD dwMethodAmount = 0;
		fread(&dwMethodAmount, sizeof(DWORD), 1, fp);
		DWORD dwMyCheck = 0;
		for(DWORD j = 0; j < dwMethodAmount; j++)
		{
			int nMethod = 0;
			fread(&nMethod, sizeof(int), 1, fp);
			info.setLocateTargetMethod.push_back(nMethod);
			dwMyCheck += nMethod;
		}
		dwMyCheck = dwMethodAmount*dwMyCheck;
		fread(&info.nEnvMethod, sizeof(int), 1, fp);
		fread(&info.nAddSize, sizeof(int), 1, fp);
		fread(&info.nScale, sizeof(int), 1, fp);
		fread(&info.nAttackRange, sizeof(int), 1, fp);
		fread(&info.dwAttackInterval, sizeof(DWORD), 1, fp);
		DWORD dwCheck = 0;
		fread(&dwCheck, sizeof(DWORD), 1, fp);
		dwMyCheck += info.nEnvMethod + info.setLocateTargetMethod.size() * 2
			+ info.nAddSize + info.nAttackRange * info.dwAttackInterval;
		if(dwCheck != dwMyCheck)
			exit(-1);
		m_setPetInfo.push_back(info);
	}
	fclose(fp);
	return true;
}
PetInfo*	C3DRoleDataX::GetPetInfo(int nIndex)
{
	int nAmount = m_setPetInfo.size();
	if(nIndex < 0 || nIndex >= nAmount)
		return NULL;
	return &m_setPetInfo[nIndex];
}
BOOL	C3DRoleDataX::CreateGameMapInfo()
{
	m_setGameMapInfo.clear();
	const char szDataFile[] = "ini/GameMap.dat";
	FILE* fp = fopen(szDataFile, "rb");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", szDataFile);
		return false;
	}
	DWORD dwAmount = 0;
	fread(&dwAmount, sizeof(DWORD), 1, fp);
	for(DWORD i = 0; i < dwAmount; i ++)
	{
		GameMapInfo info;
		memset(&info, 0, sizeof(GameMapInfo));
		OBJID idMap;
		fread(&idMap, sizeof(OBJID), 1, fp);
		DWORD dwLength;
		fread(&dwLength, sizeof(DWORD), 1, fp);
		fread(info.szName, sizeof(char), dwLength, fp);
		info.szName[dwLength] = '\0';
		fread(&info.dwPuzzleGrideSize, sizeof(DWORD), 1, fp);
		m_setGameMapInfo[idMap] = info;
	}
	fclose(fp);
	return true;
}
GameMapInfo*	C3DRoleDataX::GetGameMapInfo(OBJID idMap)
{
	MAP_GAMEMAPINFO::iterator iter = m_setGameMapInfo.find(idMap);
	if (iter != m_setGameMapInfo.end())
		return &iter->second;
	return NULL;
}
BOOL	C3DRoleDataX::CreateUserHelpInfo()
{
	m_setUserHelpInfo.clear();
	const char szDataFile[] = "ini/UserHelpInfo.dat";
	CEncryptFile file(0x2537);
	if (!file.Open(szDataFile))
	{
		::ErrorMsg("error on open %s.", szDataFile);
		return false;
	}
	CMyIni* pIni = CMyIni::CreateNew(file.GetBuffer(), file.GetSize(), szDataFile, true);
	if (!pIni)
		return false;

	int nAmount = pIni->GetInt("Header", "HelpInfoAmount");
	for (int i = 0; i < nAmount; i++)
	{
		char szSection[256] = "";
		sprintf(szSection, "HelpInfo%d", i);
		UserHelpInfo info;
		info.nProfession	= pIni->GetInt(szSection, "Profession");
		info.nMinLevel		= pIni->GetInt(szSection, "MinLevel");
		info.nMaxLevel		= pIni->GetInt(szSection, "MaxLevel");
		info.nSyndicate		= pIni->GetInt(szSection, "Syndicate");
		info.nMarriage		= pIni->GetInt(szSection, "Marriage");
		info.nMete			= pIni->GetInt(szSection, "Mete");
		int nLineAmount = pIni->GetInt(szSection, "HelpLineAmount");
		for (int j = 0; j < nLineAmount; j++)
		{
			char szKey[256] = "";
			sprintf(szKey, "Line%d", j);
			const char* pszLine = pIni->GetString(szSection, szKey);
			if (!pszLine)
				continue;
			char szLine[1024] = "";
			strcpy(szLine, pszLine);
			::ReplaceString(szLine, '~', ' ');
			std::string str = szLine;
			info.m_setStr.push_back(str);
		}
		m_setUserHelpInfo.push_back(info);
	}
	delete pIni;
	return true;
}
const char*	C3DRoleDataX::GetUserHelpInfoLineByIndex(int nProfessional, int nLevel, int nSyndicate, int nMarriage, int nMete, int nIndex)
{
	int nAmount = m_setUserHelpInfo.size();
	for(int i = 1; i < nAmount; i ++)
	{
		UserHelpInfo* pInfo = &m_setUserHelpInfo[i];
		if(pInfo->nProfession == nProfessional
			&& pInfo->nMete == nMete
			&& nLevel <= pInfo->nMaxLevel
			&& nLevel >= pInfo->nMinLevel
			&& (nMarriage == pInfo->nMarriage || pInfo->nMarriage == 2)
			&& (nSyndicate == pInfo->nSyndicate || pInfo->nSyndicate == 2))
		{
			int nStrAmount = pInfo->m_setStr.size();
			if(nIndex >= nStrAmount || nIndex < 0)
				return NULL;
			return pInfo->m_setStr[nIndex].c_str();
		}
	}
	if(nAmount == 0)
		return NULL;
	UserHelpInfo* pInfo = &m_setUserHelpInfo[0];
	int nStrAmount = pInfo->m_setStr.size();
	if(nIndex >= nStrAmount || nIndex < 0)
		return NULL;
	return pInfo->m_setStr[nIndex].c_str();
}
int		C3DRoleDataX::GetUserHelpInfoLineAmount(int nProfessional, int nLevel, int nSyndicate, int nMarriage, int nMete)
{
	int nAmount = m_setUserHelpInfo.size();
	for(int i = 1; i < nAmount; i ++)
	{
		UserHelpInfo* pInfo = &m_setUserHelpInfo[i];
		if(pInfo->nProfession == nProfessional
			&& pInfo->nMete == nMete
			&& nLevel <= pInfo->nMaxLevel
			&& nLevel >= pInfo->nMinLevel
			&& (nMarriage == pInfo->nMarriage || pInfo->nMarriage == 2)
			&& (nSyndicate == pInfo->nSyndicate || pInfo->nSyndicate == 2))
		{
			return pInfo->m_setStr.size();
		}
	}
	if(nAmount == 0)
		return 0;
	return m_setUserHelpInfo[0].m_setStr.size();
}
static __int64 ReplaceKeyDigits(__int64 key, int nFrom, int nTo, int nValue)
{
	if (nFrom > nTo)
	{
		int n = nFrom; nFrom = nTo; nTo = n;
	}
	__int64 low = (__int64) pow(10.0, nFrom);
	__int64 high = (__int64) pow(10.0, nTo + 1);
	__int64 rest = 0;
	if (nFrom != 0)
		rest = key % low;
	return key / high * high + (__int64) nValue * low + rest;
}
DWORD C3DRoleDataX::GetActionTimeInterval(__int64 idAction)
{
	if (!m_setActionInfo.empty())
	{
		MAP_ACTIONINFO::iterator iter = m_setActionInfo.find(idAction);
		if (iter != m_setActionInfo.end())
			return iter->second;
		iter = m_setActionInfo.find(ReplaceKeyDigits(idAction, 0, 2, 999));
		if (iter != m_setActionInfo.end())
			return iter->second;
		iter = m_setActionInfo.find(ReplaceKeyDigits(idAction, 3, 5, 999));
		if (iter != m_setActionInfo.end())
			return iter->second;
		iter = m_setActionInfo.find(ReplaceKeyDigits(idAction, 6, 8, 999));
		if (iter != m_setActionInfo.end())
			return iter->second;
		iter = m_setActionInfo.find(ReplaceKeyDigits(idAction, 9, 10, 99));
		if (iter != m_setActionInfo.end())
			return iter->second;
		iter = m_setActionInfo.find(ReplaceKeyDigits(idAction, 6, 10, 99999));
		if (iter != m_setActionInfo.end())
			return iter->second;
		iter = m_setActionInfo.find(ReplaceKeyDigits(idAction, 3, 8, 999999));
		if (iter != m_setActionInfo.end())
			return iter->second;
		iter = m_setActionInfo.find(ReplaceKeyDigits(ReplaceKeyDigits(idAction, 9, 10, 999999), 3, 5, 999999));
		if (iter != m_setActionInfo.end())
			return iter->second;
		return 33;
	}

	switch (idAction % 1000)
	{
	case 100:
	case 101:
	case 102:
	case 103:
		return 66;
	case 110:
	case 120:
	case 121:
		return 25;
	case 451:
	case 452:
	case 453:
	case 460:
	case 465:
	case 470:
		return 10;
	default:
		return 33;
	}
}
BOOL C3DRoleDataX::CreateTxtActionInfo()
{
	m_setTextAction.clear();
	CONST char INI_FILE[] = "ini/TxtAction.ini";
	CEncryptFile file(0x2537);
	if (!file.Open(INI_FILE))
		return false;
	const char* pBuf = file.GetBuffer();
	int nSize = file.GetSize();
	int nPos = 0;
	do
	{
		char szLine[1024] = "";
		int i;
		for (i = 0; i < 1024; i++)
		{
			if (nPos + i >= nSize)
			{
				nPos = nSize;
				break;
			}
			if (pBuf[nPos + i] == '\r' && pBuf[nPos + i + 1] == '\n')
			{
				nPos = nPos + i + 1;
				break;
			}
			szLine[i] = pBuf[nPos + i];
		}
		if (i >= 1024)
		{
			szLine[1023] = '\0';
			nPos += 1023;
		}
		char szAction[256] = "";
		char szText[256] = "";
		if (2 == sscanf(szLine, "%s %s", szAction, szText))
		{
			TextActionInfo info;
			info.strAction = szAction;
			info.strText = szText;
			m_setTextAction.push_back(info);
		}
	} while (nPos < nSize);
	return true;
}
int C3DRoleDataX::GetTextActionAmount()
{
	return m_setTextAction.size();
}
const char* C3DRoleDataX::GetTextActionTitle(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int) m_setTextAction.size())
		return NULL;
	return m_setTextAction[nIndex].strAction.c_str();
}
const char* C3DRoleDataX::GetTextAction(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int) m_setTextAction.size())
		return NULL;
	return m_setTextAction[nIndex].strText.c_str();
}
const CMonsterInfo* C3DRoleDataX::GetMonsterInfo(const char* pszMonster)
{
	if (!pszMonster || strlen(pszMonster)>=16)
		return NULL;
	MAP_MONSTERINFO::iterator iter = m_setMonsterInfo.find(pszMonster);
	if (iter == m_setMonsterInfo.end())
		return NULL;
	return &iter->second;
}
const CMonsterInfo* C3DRoleDataX::GetMonsterInfoByType(OBJID idType)
{
	MAP_MONSTERINFO::iterator iter = m_setMonsterInfo.begin();
	for (; iter != m_setMonsterInfo.end(); iter++)
	{
		if (iter->second.idType == idType)
			return &iter->second;
	}
	return NULL;
}
BOOL C3DRoleDataX::Create3DEffectInfo()
{
	int i;
	for (i = 0; i < 3; i++)
	{
		m_setGame3DEffectInfo[i].clear();
		char szIniFile[256] = "";
		if (i == 0)
			strcpy(szIniFile, "ini/Action3DEffect.ini");
		else
			sprintf(szIniFile, "ini/Action3DEffect%d.ini", i);
		if (!this->Load3DEffectTable(m_setGame3DEffectInfo[i], szIniFile))
			break;
	}
	return i > 0;
}
BOOL C3DRoleDataX::Load3DEffectTable(MAP_GAME3DEFFECTINFO& setEffect, const char* pszIniFile)
{
	const char* szIniFile = pszIniFile;
	FILE* fp =fopen(szIniFile, "r");
	if (!fp)
		return false;
	while (true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		if (szLine[0] == '/' || szLine[0] == ';')
			continue;
		DWORD nLook = 0, idxAction = 0, idxPart = 0, idxVar = 0;
		char szEffect[256] = "";
		if (5 != sscanf(szLine, "%u.%u.%u.%u=%s", &nLook, &idxAction, &idxPart, &idxVar, szEffect))
		{
			::ErrorMsg("invalid line [%s] found in %s.", szLine, szIniFile);
			continue;
		}
		__int64 id = (((__int64)nLook * 1000 + idxAction) * 1000 + idxPart) * 1000 + idxVar;
		setEffect[id]=szEffect;
	}
	fclose(fp);
	return true;
}
const char* C3DRoleDataX::GetRolePart3DEffect(DWORD dwLook, DWORD idxAction, DWORD idxPart, DWORD idxVar, DWORD nTable)
{
	MAP_GAME3DEFFECTINFO& setEffect = m_setGame3DEffectInfo[nTable % 3];
	if (setEffect.empty())
		return NULL;
	__int64 id = (__int64)dwLook * 1000000000 + idxAction * 1000000 + idxPart * 1000 + idxVar;
	MAP_GAME3DEFFECTINFO::iterator iter = setEffect.find(id);
	if (iter != setEffect.end())
		return iter->second.c_str();
	id = (__int64)_LOOKTYPE_UNIVERSAL * 1000000000 + idxAction * 1000000 + idxPart * 1000 + idxVar;
	iter = setEffect.find(id);
	if (iter != setEffect.end())
		return iter->second.c_str();
	id = (__int64)_LOOKTYPE_UNIVERSAL * 1000000000 + _ACTION_UNIVERSAL * 1000000 + idxPart * 1000 + idxVar;
	iter = setEffect.find(id);
	if (iter != setEffect.end())
		return iter->second.c_str();
	id = (__int64)_LOOKTYPE_UNIVERSAL * 1000000000 + idxAction * 1000000 + _PARTTYPE_UNIVERSAL * 1000 + idxVar;
	iter = setEffect.find(id);
	if (iter != setEffect.end())
		return iter->second.c_str();
	id = (__int64)_LOOKTYPE_UNIVERSAL * 1000000000 + idxAction * 1000000 + idxPart * 1000 + _VARIABLE_UNIVERSAL;
	iter = setEffect.find(id);
	if (iter != setEffect.end())
		return iter->second.c_str();
	id = (__int64)dwLook * 1000000000 + _ACTION_UNIVERSAL * 1000000 + idxPart * 1000 + idxVar;
	iter = setEffect.find(id);
	if (iter != setEffect.end())
		return iter->second.c_str();
	id = (__int64)dwLook * 1000000000 + idxAction * 1000000 + _PARTTYPE_UNIVERSAL * 1000 + idxVar;
	iter = setEffect.find(id);
	if (iter != setEffect.end())
		return iter->second.c_str();
	id = (__int64)dwLook * 1000000000 + idxAction * 1000000 + idxPart * 1000 + _VARIABLE_UNIVERSAL;
	iter = setEffect.find(id);
	if (iter != setEffect.end())
		return iter->second.c_str();
	return NULL;
}

IAni* C3DRoleDataX::GetItemMinIcon ( OBJID idItemType, int nExigence )
{
	if(idItemType == 0)
	{
		return NULL;
	}
	else
	{
		char szAniFile[] = "ani/ItemMinIcon.Ani";
		char szTitle[64];
		sprintf(szTitle, "Item%d", idItemType);
		IAni* pAni = ::GameDataSetQuery()->GetDataAni(szAniFile, szTitle, nExigence);
		if (!pAni)
		{
			pAni = ::GameDataSetQuery()->GetDataAni(szAniFile, "ItemDefault", nExigence);
		}

		return pAni;
	}
}

IAni* C3DRoleDataX::GetPlayerFaceIcon ( int nFace, int nExigence )
{
	char szAniFile[] = "ani/PlayerFace.Ani";
	char szTitle[64];
	sprintf(szTitle, "Face%d", nFace);
	return ::GameDataSetQuery()->GetDataAni(szAniFile, szTitle, nExigence);
}

IAni* C3DRoleDataX::GetNpcFaceIcon ( int nFace, int nExigence )
{
	char szAniFile[] = "ani/NpcFace.Ani";
	char szTitle[64];
	sprintf(szTitle, "Face%d", nFace);
	return ::GameDataSetQuery()->GetDataAni(szAniFile, szTitle, nExigence);
}
