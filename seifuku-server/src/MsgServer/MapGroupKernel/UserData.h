#if !defined(AFX_USERDATA_H__6C3EB549_7325_4A50_82DF_FD07A49E1300__INCLUDED_)
#define AFX_USERDATA_H__6C3EB549_7325_4A50_82DF_FD07A49E1300__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#pragma warning(disable:4786)
#include "define.h"
#include "GameObj.h"
#include "windows.h"
#include "GameData.h"

const int		MEDAL_BUFSIZE		= 64+1;
const int		TITLE_BUFSIZE		= 64+1;

enum USERDATA{
	USERDATA_NAME=1,
};

extern char	szUserTable[];

enum {_NOT_AUTOALLOT = 0, _AUTOALLOT = 1};

struct WantedInfoStruct{
		char	szName[_MAX_NAMESIZE];
		int		nBonuty;
		OBJID	idModifyWanted;
		OBJID	idWanted;
};

struct	UserInfoStruct
{
	OBJID			id;

	NAMESTR			szName;
	NAMESTR			szMate;

	DWORD			dwLookface;
	DWORD			dwHair;
	DWORD			dwMoney;
	DWORD			dwMoney_saved;
	DWORD			dwCoin_money;
	UCHAR			ucLevel;
	__int64			i64Exp;
	USHORT			usForce;
	UCHAR			ucOldProf;
	USHORT			usSpeed;
	USHORT			usHealth;
	USHORT			usSoul;
	USHORT			usAdditional_point;
	UCHAR			ucAuto_allot;
	USHORT			usLife;
	USHORT			usMana;
	UCHAR			ucProfession;
	int				nDeed;
	SHORT			sPk;
	UCHAR			ucNobility;
	DWORD			dwMedal;
	DWORD			dwMedal_select;
	UCHAR			ucMetempsychosis;
	OBJID			idSyndicate;
	OBJID			idRecordmap;
	USHORT			usRecordX;
	USHORT			usRecordY;
	OBJID			idAccount;
	DWORD			dwLast_login;
	DWORD			dwTaskMask;
	DWORD			dwVirtue;
	OBJID			idHome;
	DWORD			dwLockKey;
	DWORD			dwGodStatus;
	DWORD			dwExpBallUsage;

	DWORD			dwEffect[2];
	DWORD			dwStatus;
	UCHAR			ucPkMode;
	USHORT			usEnergy;
	USHORT			usMaxEnergy;
	DWORD			dwCurrLockKey;

	DWORD			dwMonsterType;
	DWORD			dwMonsterSecs;
	
	struct WantedInfoStruct infoWanted;
	OBJID	idPoliceWanted;

	DWORD			dwEmoney;
	DWORD			dwChkSum;
	DWORD			dwOnlineTime;
	DWORD			dwAutoExercise;
	DWORD			dwLastLogout;
	USHORT			usBigPrizeFails;
	USHORT			usSmallPrizeFails;
	DWORD			dwMeteLev;
	DWORD			dwCurrentLayoutType;
	DWORD			dwOnlineTime2;
	DWORD			dwOfflineTime;
	DWORD			dwLastLogout2;
	__int64			i64Donation;
	DWORD			dwBusiness;
};

class IRecord;
class IDatabase;
class CUserData
{
public:
	CUserData();
	virtual ~CUserData();

#ifdef _DEBUG
	void			SetName			(const char* pszName);
#endif

protected:
	bool	Create					(OBJID idUser, IDatabase* pDb);
	bool	LoadInfo				(void);
	bool	SaveInfo				(void);

public:
	DWORD GetLastLogin();
	void			SetMate			(LPCTSTR szMate, BOOL bUpdate = TRUE);
	char*			GetMate			();

	void			SetLife			(int nLife, BOOL bUpdate = false);
	USHORT			GetLife			()	{return m_Info.usLife;}

	int				GetMana			()	{return m_Info.usMana;}
	void			SetMana			(int nPower, BOOL bUpdate = false);

	void			SetExp			(__int64 i64Exp, BOOL bUpdate = false);
	__int64			GetExp			()	{return m_Info.i64Exp;}

	void			SetPk			(int nPk, BOOL bUpdate = TRUE);
	int				GetPk			()	{return m_Info.sPk;}

	void			SetLev			(int nLev, BOOL bUpdate = TRUE);
	int				GetLev			();

	void			SetForce		(int nForce, BOOL bUpdate = TRUE);
	DWORD			GetForce		()  {return m_Info.usForce;}

	void			SetAddPoint		(int nAddPoint, BOOL bUpdate = TRUE);
	int				GetAddPoint		();

	void			SetMedalSelect	(int nData, BOOL fUpdate = TRUE);

	void			SetAutoAllot	(int nAutoAllot, BOOL fUpdate = TRUE);
	int				GetAutoAllot	();

	void			SetMetempsychosis	(int nMetempsychosis, BOOL fUpdate = TRUE);
	int				GetMetempsychosis	();

	void			SetMoney		(int nMoney, BOOL bUpdate = false);
	void			SetMoneySaved	(int nMoney, BOOL bUpdate = false);
	void			SetEmoney		(DWORD dwEmoney);

	void			SetRecordPos	(OBJID idMap, DWORD dwX, DWORD dwY, BOOL bUpdate = false);
	OBJID			GetRecordMap	()	{return m_Info.idRecordmap;}
	DWORD			GetRecordX		()	{return m_Info.usRecordX;}
	DWORD			GetRecordY		()	{return m_Info.usRecordY;}

	void			SetOldProf		(DWORD dwOldProf, BOOL bUpdate);
	DWORD			GetOldProf		()	{return m_Info.ucOldProf;}
	void			SetProfession	(DWORD dwProfession, BOOL bUpdate = TRUE);
	DWORD			GetProfession	()	{return m_Info.ucProfession;}

	void			SetSpeed		(DWORD dwSpeed, BOOL bUpdate = TRUE);
	DWORD			GetSpeed		()	{return m_Info.usSpeed;}

	void			SetHealth		(DWORD dwHealth, BOOL fUpdate = TRUE);
	DWORD			GetHealth		()	{return m_Info.usHealth;}

	void			SetSoul			(DWORD dwSoul, BOOL fUpdate = TRUE);
	DWORD			GetSoul			()	{return m_Info.usSoul;}

	void			SetLookFace		(DWORD dwLookFace, BOOL fUpdate = TRUE);
	DWORD			GetLookFace		()	{return m_Info.dwLookface;}

	void			SetHair			(DWORD dwHair, BOOL fUpdate = TRUE);
	DWORD			GetHair			()	{return m_Info.dwHair;}

	void			SetLastLogin	();

	void			SetTaskMask		(DWORD dwMask);
	DWORD			GetTaskMask		()	{ return m_Info.dwTaskMask; }

	void			SetVirtue		(DWORD dwVirtue, BOOL bUpdate = TRUE);
	DWORD			GetVirtue		()	{ return m_Info.dwVirtue; }

	void			SetHomeID		(OBJID idHome, BOOL bUpdate = TRUE);
	DWORD			GetHomeID		()	{ return m_Info.idHome; }

	void			SetLockKey		(DWORD dwLockKey, BOOL bUpdate = TRUE);
	void			SetCurrLockKey	(DWORD dwLockKey, BOOL bUpdate = TRUE)		{ m_Info.dwCurrLockKey = dwLockKey; }
	DWORD			GetRealLockKey	()	{ return m_Info.dwLockKey; }
	DWORD			GetCurrLockKey	()	{ return m_Info.dwCurrLockKey; }

	void			SetGodStatus		(DWORD dwData, BOOL bUpdate);
	void			SetExpBallUsage		(DWORD dwData, BOOL bUpdate);
	void			SetOnlineTime		(DWORD dwData, BOOL bUpdate);
	void			SetAutoExercise		(DWORD dwData, BOOL bUpdate);
	void			SetLastLogout		(DWORD dwData, BOOL bUpdate);
	void			SetBigPrizeFails	(int nData, BOOL bUpdate);
	void			SetSmallPrizeFails	(int nData, BOOL bUpdate);
	void			SetMeteLev			(int nData, BOOL bUpdate);
	void			SetCurrentLayoutType(BOOL bUpdate);
	DWORD			GetBusiness			();
	void			SetBusiness			(DWORD dwData);
	__int64			GetDonation			();
	void			SetDonation			(__int64 i64Data);

protected:
	DWORD			CalcChkSum			();
	bool			IsChkSumOk			();

public:
	UserInfoStruct&		GetInfo()	{ return m_Info; }
protected:
	UserInfoStruct		m_Info;
	IRecord*			m_pRes;

	friend class CUser;
};

#endif
