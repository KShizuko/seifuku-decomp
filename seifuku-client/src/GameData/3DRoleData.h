#ifndef _GAMEDATA_3DROLEDATA_H
#define _GAMEDATA_3DROLEDATA_H

#include <string>
#include <vector>
#include "GameDataSet.h"

#ifdef	GAMEDATA_EXPORTS
#define GAMEDATA_API __declspec ( dllexport )
#else
#define GAMEDATA_API __declspec ( dllimport )
#endif

typedef struct
{
	BOOL	bDirEnable;
	int		nShowTime;
	char	szEffect[32];
}ActionMapEffectInfo;

typedef struct
{
	OBJID		idType;
	DWORD		dwAntiType;
	int			nAddSize;
	int			nZoomPercent;
	int			nMaxLife;
	int			nLevel;
	int			nActResCtrl;
	int			nAsb;
	int			nAdb;
	int			nBornAction;
	int			nBodyType;
	OBJID		idArmet;
	OBJID		idRWeapon;
	OBJID		idLWeapon;
	OBJID		idMisc;
	OBJID		idMount;
	std::string	strName;
	std::string	strBornEffect;
	std::string	strBornSound;
}CMonsterInfo;

typedef struct
{
	BOOL	bCanJump;
	int		nAddSize;
	int		nScale;
	int		nLook;
	OBJID	idArmet;
	OBJID	idRWeapon;
	OBJID	idLWeapon;
	OBJID	idMisc;
	OBJID	idMount;
}CTransformInfo;

typedef struct
{
	int		nType;
	char	szNpcName[16];
	char	szEffectIndex[64];
	OBJID	idSimpleObj;
	OBJID	idMotionStandBy;
	OBJID	idMotionBlaze;
	OBJID	idMotionRest;
	int		nAsb;
	int		nAdb;
	BOOL	bFixDir;
}NpcTypeInfo;

typedef struct
{
	int		nType;
	char	szNpcName[16];
	BOOL	bFixDir;
	int		nAddSize;
	int		nScale;
	int		nLook;
	int		nHead;
	int		nHair;
	OBJID	idArmor;
	OBJID	idArmet;
	OBJID	idRWeapon;
	OBJID	idLWeapon;
	OBJID	idMisc;
	OBJID	idMount;
}NpcXTypeInfo;

typedef struct
{
	int nMaxPkValue;
	int nMinPkValue;
	char szTitle[16];
}PkTitleInfo;

typedef struct
{
	char	szTitle[64];
	char	szAniTitle[64];
	DWORD	dwLoopTime;
	DWORD	dwFrameInterval;
	DWORD	dwShowWay;
	DWORD	dwLoopInterval;
	DWORD	dwDelay;
	int		nOffsetX;
	int		nOffsetY;
	int		nExigence;
}EffectInfo;

typedef struct
{
	OBJID	idType;
	char	szStart[64];
	char	szLast[64];
	char	szEnd[64];
}MapMagicItemInfo;

typedef struct
{
	OBJID idType;
	OBJID idSimpleObj;
	char  szFlyEffect[64];
	char  szFlySound[64];
	char  szHitSound[64];
	char  szTargetEffect[64];
}FlyingObjInfo;

typedef struct
{
	int	nTerrain;
	int	nType;
	int nWindDir;
	int nLevel;
	DWORD dwColor;
}TerrainWeatherInfo;

typedef struct
{
	DWORD	dwStatus;
	char	sz3DEffect[256];
	char	sz2DEffect[256];
}StatusEffectInfo;

typedef struct
{
	int							nType;
	char						szNpcName[16];
	std::vector<std::string>	setStr;
}TerrainNpcTypeInfo;

typedef struct
{
	DWORD			dwMultiple;
	std::string		strName;
}RestrainInfo;

typedef struct
{
	std::string		strSound;
	std::string		strEffect;
}CMediaEffectInfo;

typedef struct
{
	std::string		strHitEffect;
	std::string		strHitSound;
	std::string		strBlkEffect;
	std::string		strBlkSound;
}CWeaponInfo;

typedef struct
{
	DWORD	tmWoundDelay;
	DWORD	tmBlockDelay;
	DWORD	tmDieDelay;
}CActionDelayInfo;

typedef struct
{
	enum { MAX_ACTION_SECTION = 8 };
	int		nSection;
	int		nTimePercent[MAX_ACTION_SECTION];
	int		nMovePercent[MAX_ACTION_SECTION];
	BOOL	bVisible;
}CActionCtrlInfo;

typedef struct
{
	std::vector<std::string>	m_setStr;
}TipsInfo;

typedef struct
{
	std::vector<int> setLocateTargetMethod;
	int			nEnvMethod;
	int			nScale;
	int			nAttackRange;
	int			nAddSize;
	DWORD		dwAttackInterval;
}PetInfo;

typedef struct
{
	char	szName[64];
	DWORD	dwPuzzleGrideSize;
}GameMapInfo;

typedef struct
{
	int		nProfession;
	int		nMinLevel;
	int		nMaxLevel;
	int		nSyndicate;
	int		nMarriage;
	int		nMete;
	int		nReserved;
	std::vector<std::string>	m_setStr;
}UserHelpInfo;

typedef struct
{
	std::string strAction;
	std::string strText;
}TextActionInfo;

class IRoleData
{
public:
	virtual void		Init ( void ) = 0;
	virtual DWORD		GetActionTimeInterval ( __int64 idAction ) = 0;
	virtual const ActionMapEffectInfo*	GetActionMapEffect ( int nLook, int nAction, int nTerrain ) const = 0;
	virtual const char*	GetRolePart3DEffect ( DWORD dwLook, DWORD idxAction, DWORD idxPart, DWORD idxVar, DWORD nTable ) = 0;
	virtual const char*	GetActionSound ( DWORD idxLook, DWORD idxWeapon, DWORD idxAction ) = 0;
	virtual const CMonsterInfo*	GetMonsterInfoByType ( OBJID idType ) = 0;
	virtual const CMonsterInfo*	GetMonsterInfo ( const char* pszMonster ) = 0;
	virtual CTransformInfo*	GetTransformInfo ( int nIndex ) = 0;
	virtual BOOL		CanWeaponCombin ( DWORD dwWeaponR, DWORD dwWeaponL ) = 0;
	virtual NpcTypeInfo*	GetNpcTypeInfo ( OBJID idType ) = 0;
	virtual NpcXTypeInfo*	GetNpcXTypeInfo ( int nType ) = 0;
	virtual MapMagicItemInfo*	GetMapMagicItemInfo ( int nType ) = 0;
	virtual EffectInfo*	GetEffectInfo ( char* pszIndex ) = 0;
	virtual FlyingObjInfo*	GetFlyingObjInfo ( OBJID idItem, OBJID idWeaponType ) = 0;
	virtual DWORD		GetStatusEffectInfoAmount ( void ) = 0;
	virtual StatusEffectInfo*	GetStatusEffectInfoByIndex ( int nIndex ) = 0;
	virtual const char*	GetUserHelpInfoLineByIndex ( int nProfessional, int nLevel, int nSyndicate, int nMarriage, int nMete, int nIndex ) = 0;
	virtual int			GetUserHelpInfoLineAmount ( int nProfessional, int nLevel, int nSyndicate, int nMarriage, int nMete ) = 0;
	virtual TerrainNpcTypeInfo*	GetTerrainNpcInfo ( OBJID idType ) = 0;
	virtual int			GetTipsAmount ( void ) = 0;
	virtual int			GetTipLineAmount ( int nIndex ) = 0;
	virtual char*		GetTipLine ( int nTipIndex, int nLineIndex ) = 0;
	virtual int			GetTextActionAmount ( void ) = 0;
	virtual const char*	GetTextActionTitle ( int nIndex ) = 0;
	virtual const char*	GetTextAction ( int nIndex ) = 0;
	virtual PetInfo*	GetPetInfo ( int nIndex ) = 0;
	virtual TerrainWeatherInfo*	GetWeatherInfo ( int nTerrain ) = 0;
	virtual GameMapInfo*	GetGameMapInfo ( OBJID idMap ) = 0;
	virtual char*		GetPkTitle ( int nPkValue ) = 0;
	virtual CONST CActionDelayInfo*	GetActionDelayInfo ( DWORD dwKey ) CONST = 0;
	virtual CONST CWeaponInfo*		GetWeaponInfo ( DWORD dwKey ) CONST = 0;
	virtual CONST CActionCtrlInfo*	GetActionCtrlInfo ( DWORD dwKey ) CONST = 0;
	virtual CONST CMediaEffectInfo*	GetMediaEffectInfo ( DWORD dwKey ) CONST = 0;
	virtual CONST RestrainInfo*		GetRestrainInfo ( DWORD dwKey ) CONST = 0;
	virtual IAni*		GetItemMinIcon ( OBJID idType, int nExigence = EXIGENCE_NORMAL ) = 0;
	virtual IAni*		GetPlayerFaceIcon ( int nFace, int nExigence = EXIGENCE_NORMAL ) = 0;
	virtual IAni*		GetNpcFaceIcon ( int nFace, int nExigence = EXIGENCE_NORMAL ) = 0;
	virtual int			GetMaleFaceAmount ( void ) = 0;
	virtual int			GetFemaleFaceAmount ( void ) = 0;
	virtual int			GetLeaderScale ( void ) = 0;
	virtual int			GetAssistantScale ( void ) = 0;
	virtual int			GetSubLeaderScale ( void ) = 0;
	virtual int			GetLeaderMaxLife ( void ) = 0;
	virtual int			GetAssistantMaxLife ( void ) = 0;
	virtual int			GetSubLeaderMaxLife ( void ) = 0;
	virtual				~IRoleData ( void ) {}
};

extern "C" GAMEDATA_API IRoleData* RoleDataQuery ( void );
extern "C" GAMEDATA_API void RoleDataDestroy ( void );

#endif
