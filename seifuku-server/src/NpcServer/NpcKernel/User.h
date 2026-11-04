#if !defined(AFX_USER_H__0B50AF5C_138D_4DE4_A117_D461318A495B__INCLUDED_)
#define AFX_USER_H__0B50AF5C_138D_4DE4_A117_D461318A495B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "GameObj.h"
#include "define.h"
#include "Npc.h"
#include "I_Role.h"
#include "TimeOut.h"

const int	PLAYERACTIVE_SECS		= 30;
const int	MASK_CHANGELOOK			= 10000000;

struct ST_USERINFO
{
		OBJID			id;
		NAMESTR			szName;
		DWORD			dwLookFace;
		USHORT			usHair;
		DWORD			i64Effect;

		DWORD			dwHelmetType;
		DWORD			dwArmorType;
		DWORD			dwWeaponRType;
		DWORD			dwWeaponLType;
		DWORD			dwMountType;

		DWORD			dwMedalSelect;
		OBJID			idSyn;
		UCHAR			nRank;

		USHORT			usPosX, usPosY;
		UCHAR			ucDir;
		UCHAR			ucPose;

		UCHAR			ucSizeAdd;
};

enum { POSE_STAND=0, };
class CUser : public CGameObj, IRole
{
public:
	CUser();
	virtual ~CUser();
	static CUser*	CreateNew()		{ return new CUser; }
	bool	Create(ST_USERINFO* pInfo, OBJID idMap);
	ULONG	ReleaseByOwner()		{ delete this; return 0; }
	IRole*	QueryRole()				{ return (IRole*)this; }
	IRoleAttr*	QueryRoleAttr()		{ return static_cast<IRoleAttr*>(this); }
	virtual CAutoLink<IRole>&	QueryLink(const CAutoLink<IRole>&)				{ return m_lnkThis; }
	template<TYPENAME T>
		T*		QueryInterface(T** ppT)								{ if(ppT) *ppT = this; return this; }
	virtual void*	QueryInterface(int id, void** ppT)				{ void* p=NULL; if(id==GUID_OF(CUser)) p=this; if(ppT) *ppT=p; return p; }
	void		OnTimer();

public:
	OBJID		GetID()			{ return m_Info.id; }
	LPCTSTR		GetName()		{ return m_Info.szName; }
	DWORD		GetLookFace()	{ return m_Info.dwLookFace; }
	DWORD		GetEffect()		{ return m_Info.i64Effect; }

	DWORD		GetHelmetType()	{ return m_Info.dwHelmetType; }
	DWORD		GetArmorType()	{ return m_Info.dwArmorType; }
	DWORD		GetWeaponRType()	{ return m_Info.dwWeaponRType; }
	DWORD		GetWeaponLType()	{ return m_Info.dwWeaponLType; }
	DWORD		GetMountType()	{ return m_Info.dwMountType; }

	OBJID		GetSynID()		{ return m_Info.idSyn; }

	OBJID		GetMapID();
	int			GetPosX()		{ return m_Info.usPosX; }
	int			GetPosY()		{ return m_Info.usPosY; }
	int			GetHair()		{ return m_Info.usHair; }
	int			GetDir()		{ return m_Info.ucDir; }
	int			GetPose()		{ return m_Info.ucPose; }
	int			GetAttackRange(int nSizeAdd);
	int			GetSizeAdd()	{ return m_Info.ucSizeAdd; }

public:
	bool		IsAlive()		{ return (GetEffect() & KEEPEFFECT_DIE) == 0; }
	bool		IsFarWeapen()	{ return GetWeaponLType() != ID_NONE; }
	bool		IsVirtuous()	{ return (GetEffect() & KEEPEFFECT_NOT_VIRTUOUS) == 0; }
	bool		IsPker()		{ return (GetEffect() & KEEPEFFECT_DEEPRED) != 0; }
	bool		IsEvil()		{ return (GetEffect() & (KEEPEFFECT_DEEPRED|KEEPEFFECT_CRIME)) != 0; }
	bool		IsRighteous()	{ return (GetEffect() & KEEPEFFECT_DEEPRED) == 0; }
	bool		IsCrime()		{ return (GetEffect() & KEEPEFFECT_CRIME) != 0; }
	bool		IsLurker()		{ return (GetEffect() & (KEEPEFFECT_LURKER)) != 0; }
	bool		IsBlockNpc()	{ return (GetEffect() & KEEPEFFECT_NOT_BLOCKNPC) == 0; }
	bool		IsWing()		{ return (GetEffect() & KEEPEFFECT_WING) != 0; }
	bool		IsTeamLeader()	{ return (GetEffect() & KEEPEFFECT_TEAMLEADER) != 0; }
	CGameMap*	GetMap()		{ return m_pMap; }

public:
	void		SetEffect(DWORD dwEffect);
	void		SetDir(int nDir)			{ m_Info.ucDir	= nDir; }
	void		SetPose(int nPose)			{ m_Info.ucPose	= nPose; }
	void		SetPos(int uPosX, int nPosY);
	void		FlyMap(OBJID idMap, int nPosX, int nPosY);
	bool		MoveForward(int nDir);
	void		JumpPos(int nPosX, int nPosY, int nDir);
	void		SetSizeAdd(int nSizeAdd);

public:
	void		SetLife(int nLife)				{}
	void		SetPower(int nPower)			{}

public:
	int			Distance(int x, int y)		{ return __max(abs(x-GetPosX()), abs(y-GetPosY())); }

protected:
	CGameMap*	m_pMap;
	ST_USERINFO	m_Info;
	CTimeOut	m_tActive;
	CAutoLink<IRole>	m_lnkThis;
};

#endif
