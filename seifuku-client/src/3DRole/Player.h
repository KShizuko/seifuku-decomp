#if !defined(AFX_PLAYER_H__EF9F33AA_217F_4793_A28A_4F3C56254D27__INCLUDED_)
#define AFX_PLAYER_H__EF9F33AA_217F_4793_A28A_4F3C56254D27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Role.h"
#include <deque>
#include <vector>
#include <string>
#include <list>
#include "EncryptData.h"
#include "ImageString.h"
#include "FDword.h"
using namespace std;


const int RANKAMOUNT = 7;
 
typedef struct
{
	DWORD	dwRank;
	OBJID	idTitle;
}InfoSyndicateRank;
enum 
{
		RANK_LEADER			= 100,
		RANK_ASSISTANT		= 90,
		RANK_SUBLEADER		= 80,
		RANK_SUBASSISTANT	= 70,
		RANK_TEAMLEADER		= 60,
		RANK_SAN			= 50,
		RANK_LIGHT			= 40,
		RANK_CROSS			= 30,
		RANK_NEWBIE			= 20,
		RANK_RESERVED		= 10,
		RANK_NONE			= 0,
};
const InfoSyndicateRank SYNDICATERANKINFOSET[RANKAMOUNT]=
{
	{RANK_LEADER , 100079},
	{RANK_ASSISTANT , 100080},
	{RANK_SAN , 100081},
	{RANK_NONE , 100021},
	{RANK_SUBLEADER , 100120},
	{RANK_SUBASSISTANT , 100122},
	{RANK_TEAMLEADER , 100121},
};

typedef struct{
	char	szMate[_MAX_NAMESIZE];
	DWORD	dwForce,dwDexterity, dwSpeed, dwHealth, dwSoul, dwXP, dwMaxLife;
	CEncryptData dataLife;
	CEncryptData dataMana;
	CEncryptData dataCurLife;
	CEncryptData dataCurMana;
	

	DWORD	dwProfession;
	DWORD	dwPhysicalForce;
	DWORD	dwMaxPhysicalForce;

	DWORD	dwLev;
	DWORD	dwAddPoint;
	DWORD	dwMedalSelect;
	int		nPotential;
	int		nMetempsychosis;
	int		nRepute;
	int		nDeed;
	int		nExp;
	int		nPk;
	OBJID	idSyndicate;
	DWORD	dwSyndicateRank;
	int		nSyndicateProffer;
	char	szSyndicateLeader[_MAX_NAMESIZE];
	DWORD	dwSyndicateFund;
	DWORD	dwSyndicatePopulation;
	DWORD	dwKoAmount;
	DWORD	dwVirtue;
	BOOL	bShowIcon;
	USHORT	usRWeaponRestrain;
	USHORT	usLWeaponRestrain;


	deque<CCommand> setCmd;
}CPlayerInfo;

struct TimerCmdInfo {
	CCommand	cmd;
	DWORD		tmExecute;
	std::string	strEffect;
	std::string strSound;
};

class CPlayer: public CRole
{
public:
	enum
	{
			_PLAYER_LIFE		= 1,
			_PLAYER_CURRENTLIFE = 15,
			_PLAYER_MANA		= 9,
			_PLAYER_CURRENTMANA	= 5,
	};
	enum
	{
		PROFSORT_MAGICIAN	=1, 
		PROFSORT_SOLDIER	=2, 
		PROFSORT_ARCHER		=3, 
	};
public:
	DWORD	GetData(DWORD dwDataType);
	void		SetData(DWORD dwDataType, DWORD dwData);
public:
	virtual void	Show		(void* pInfo);
	virtual void	Process		(void* pInfo);

	virtual DWORD	SetCommand	(CCommand* pCommand);

public:
	
public:
	CPlayer();
	virtual ~CPlayer();

	static	BOOL	s_bShowName;

	void	ShowSyndicate	();
	void	ShowInfo		();
	void	ShowName		();
	void	ShowLife		();

	BOOL	IsSynLeader		()  { return (strcmp(this->GetSyndicateLeaderName(),this->GetName()) == 0);}
	BOOL	IsDead			()	{ return (this->GetStatus()&USERSTATUS_DIE) != 0; }
	BOOL	IsGhost			()	{ return (this->GetStatus()&USERSTATUS_GHOST) != 0; }

public:


	const CPlayerInfo* GetPlayerInfo() const {return &m_infoPlayer;}

	void	SetTimerCommand	(CCommand& cmd, DWORD tmExecute, const char* pszEffect = NULL, const char* pszSound = NULL);
	void	ClrTimerCommand	(void);
	bool	ProcessTimerCommand	(void);
	DWORD	GetLastDamage	(void)	{return m_dwLastDamage;}
	void	SetLastDamage	(DWORD dwDamage)	{m_dwLastDamage = dwDamage;}
	USHORT	GetRWeaponRestrain	(void)	{return m_infoPlayer.usRWeaponRestrain;}
	USHORT	GetLWeaponRestrain	(void)	{return m_infoPlayer.usLWeaponRestrain;}
	void	SetRWeaponRestrain	(USHORT usRestrain)	{m_infoPlayer.usRWeaponRestrain = usRestrain;}
	void	SetLWeaponRestrain	(USHORT usRestrain)	{m_infoPlayer.usLWeaponRestrain = usRestrain;}
	void	SetWeaponRestrain	(int nRRestrain, int nLRestrain)
	{
		if (nRRestrain != -1)
			m_infoPlayer.usRWeaponRestrain = nRRestrain;
		if (nLRestrain != -1)
			m_infoPlayer.usLWeaponRestrain = nLRestrain;
	}
	void	SetMetempsychosis	(int nMetempsychosis);
	int		GetMetempsychosis	(void)	{return m_infoPlayer.nMetempsychosis;}

	DWORD	GetVirtue(){return m_infoPlayer.dwVirtue;}
	void	SetVirtue(DWORD dwVirtue){m_infoPlayer.dwVirtue = dwVirtue;}
	const char*	GetMate() const {return m_infoPlayer.szMate;}
	void	SetMate(const char* pszMate);
	

	virtual	DWORD	GetLife();
	virtual void	SetLife(DWORD dwLife);

	virtual	DWORD	GetCurLife();
	virtual	void	SetCurLife(int nLife);

	virtual	DWORD	GetCurMana();
	virtual	void	SetCurMana(int nMana);
	

	int		GetRepute() const {return m_infoPlayer.nRepute;}
	void	SetRepute(int nRepute) {m_infoPlayer.nRepute=nRepute;}

	int		GetDeed() const	{return m_infoPlayer.nDeed;}
	void	SetDeed(int nDeed) {m_infoPlayer.nDeed=nDeed;}

	int		GetExp() const	{return (this->GetLev() >= _MAX_LEVEL ?  0 : m_infoPlayer.nExp);}
	void	SetExp(int nExp) {m_infoPlayer.nExp=nExp;}

	int		GetPk() const {return m_infoPlayer.nPk;}
	void	SetPk(int nPk) {m_infoPlayer.nPk=nPk; PostCmd(CMD_FLASHPK);}

	DWORD	GetLev() const	{return m_infoPlayer.dwLev;}
	void	SetLev(DWORD dwLev) {m_infoPlayer.dwLev = dwLev;}

	DWORD	GetForce() const {return m_infoPlayer.dwForce;}
	void	SetForce(DWORD dwForce) {m_infoPlayer.dwForce = dwForce;}
	
	void	SetSpeed(DWORD dwSpeed) {m_infoPlayer.dwSpeed = dwSpeed;}
	DWORD   GetSpeed() {return m_infoPlayer.dwSpeed;}

	DWORD	GetHealth() const {return m_infoPlayer.dwHealth;}
	void	SetHealth(DWORD dwHealth) {m_infoPlayer.dwHealth = dwHealth;}

	DWORD	GetSoul() const {return m_infoPlayer.dwSoul;}
	void	SetSoul(DWORD dwSoul) {m_infoPlayer.dwSoul = dwSoul;}

	virtual	DWORD	GetMana();
	virtual	void	SetMana(DWORD dwMana);

	DWORD	GetProfession() const {return m_infoPlayer.dwProfession;}
	void	SetProfession(DWORD dwProfession) {m_infoPlayer.dwProfession = dwProfession;}

	DWORD	GetAddPoint() const {return m_infoPlayer.dwAddPoint;}
	void	SetAddPoint(DWORD dwAddPoint) {m_infoPlayer.dwAddPoint = dwAddPoint;}

	DWORD	GetMedalSelect() const {return m_infoPlayer.dwMedalSelect;}
	void	SetMedalSelect(DWORD dwMedalSelect) {m_infoPlayer.dwMedalSelect = dwMedalSelect;}
	
	DWORD	GetXP() const {return m_infoPlayer.dwXP;}
	void	SetXP(DWORD dwXP) {m_infoPlayer.dwXP = __min(100, dwXP);}

	void	SetKoAmount(DWORD dwAmount)	{m_infoPlayer.dwKoAmount = dwAmount; m_dwKoScale = 100;}
	DWORD	GetKoAmount()	{return m_infoPlayer.dwKoAmount;}



	DWORD	GetProfessionSort	()	{ return (m_infoPlayer.dwProfession%1000)/10; }
	DWORD	GetProfessionLevel	()	{ return m_infoPlayer.dwProfession%10; }

	virtual DWORD GetMaxLife() const {return m_infoPlayer.dwMaxLife;}
	virtual void SetMaxLife(DWORD dwMaxLife) ;

	virtual void DoLevelUp();

	virtual void ReplaceStatus	(DWORD dwStatus);
	virtual void ReplaceStatus2	(DWORD dwStatus);
	virtual	void	SetSyndicateRank(DWORD dwRank);

	OBJID	GetSydicateID(); 	
	void	SetSyndicateID(OBJID idSyndicate);
	DWORD	GetSyndicateRank();
	int		GetSyndicateProffer();
	void	SetSyndicateProffer(int nSyndicateProffer);
	char*	GetSyndicate();
	char*	GetSubSyndicate();
	char*	GetSyndicateTitle();
	char*	GetSyndicateLeaderName();
	void	SetSyndicateLeaderName(char* pszLeader);
	DWORD	GetSyndicateFund();
	void	SetSyndicateFund(DWORD dwFund);
	DWORD	GetSyndicatePopulation();
	void	SetSyndicatePopulation(DWORD dwPopulation);
	DWORD	GetPhysicalForce(){return __min(m_infoPlayer.dwPhysicalForce, this->GetMaxPhysicalForce());}
	DWORD	GetMaxPhysicalForce(){if (this->IsBlessed()) return m_infoPlayer.dwMaxPhysicalForce*150/100; return m_infoPlayer.dwMaxPhysicalForce;}
	BOOL	IsBlessed(){return this->TestStatus(USERSTATUS2_BLESS, 1);}
	BOOL	IsCursed(){return this->TestStatus(USERSTATUS2_CURSE, 1);}
	void	SetPhysicalForce(DWORD dwForce){m_infoPlayer.dwPhysicalForce = dwForce;}
	void	SetMaxPhysicalForce(DWORD dwMaxForce){m_infoPlayer.dwMaxPhysicalForce = dwMaxForce;}
	void	SetCryOut(char* pszStr){if(pszStr) strcpy(m_szCryOut, pszStr);}
	char*	GetPkTitle();
	void	AddProfessionalCoolposEffect(DWORD dwProfessional, BOOL bAllNosuch = true);
	
	BOOL	IsBaddy();

	void	ShowKoAmount	(BOOL bImmedia = true);
	void	AddMoneyEffect	(DWORD dwMoney);

	void	ProcessStatusEffect(DWORD dwStatus);

	void	SetShowIcon(BOOL bShow){m_infoPlayer.bShowIcon = bShow;}
	BOOL	GetShowIcon(){return m_infoPlayer.bShowIcon;}

	
	void    SetDexterity(DWORD dwDexterity){m_infoPlayer.dwDexterity = dwDexterity;}
	void	SetPotential(int nPotential){m_infoPlayer.nPotential = nPotential;}

	DWORD   GetDexterity(){return m_infoPlayer.dwDexterity;}
	int		GetPotential(){return m_infoPlayer.nPotential;}

	void	SetPetOwnerId(OBJID idOwner) { m_infoPlayer.dwSyndicateRank = idOwner;}
	OBJID	GetPetOwnerId() {return m_infoPlayer.dwSyndicateRank;}
	
protected:
	std::list< TimerCmdInfo > m_setCmdList;
	CPlayerInfo		m_infoPlayer;
	char			m_szCryOut[256];
	DWORD			m_dwKoScale;
	CImageString	m_objImgNumber;

public:
	static HANDLE	s_hHeap;
	static UINT		s_uNumAllocsInHeap;

	static BOOL		IsValidPt	(void* p);

	void*	operator new		(size_t size);
	void	operator delete		(void* p);
private:
	CFDword*	m_pFD;
	DWORD		m_dwLastDamage;
};

#endif
