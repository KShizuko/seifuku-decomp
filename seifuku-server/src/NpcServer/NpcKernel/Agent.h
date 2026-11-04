#if !defined(AFX_AGENT_H__71627ADA_0012_4C5F_ACD6_21928AA84DC5__INCLUDED_)
#define AFX_AGENT_H__71627ADA_0012_4C5F_ACD6_21928AA84DC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "define.h"
#include "I_MapData.h"
#include "I_Role.h"
#include "mycom.h"
#include "autoptr.h"
#include "timeout.h"
#include "GameObjSet.h"

#include "I_AiCenter.h"
#include "ItemPack.h"
#include "Team.h"
#include "Body.h"
#include "Sense.h"
#include "RoleMove.h"
#include "RoleFight.h"

const int	AGENT_VIEW_RANGE					= CELLS_PER_VIEW;
enum {_NOT_AUTOALLOT = 0, _AUTOALLOT = 1};

struct	UserInfoStruct
{
	OBJID			id;

	NAMESTR			szName;
	NAMESTR			szMate;

	DWORD			dwLookface;
	DWORD			dwHair;
	DWORD			dwMoney;
	UCHAR			ucLevel;
	int				nExp;
	int				nPotential;
	DWORD			dwReserved3C;
	USHORT			usForce;
	USHORT			usDexterity;
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
	USHORT			usPosX;
	USHORT			usPosY;
	DWORD			dwVirtue;
	DWORD			dwEffect;
	USHORT			usEnergy;
	USHORT			usMaxEnergy;
	DWORD			dwMonsterType;
	DWORD			dwMonsterSecs;
	DWORD			dwReserved84;
};

class CNetMsg;
class CMsgAction;
class CMsgInteract;
class CAgent : INpc, IAiCenterOwner
{
	COM_STYLE(CAgent)
protected:
	CAgent();
	virtual ~CAgent();

public:
	template<TYPENAME T>
		T*			QueryInterface(T** ppT)							{ if(ppT) *ppT = this; return this; }
	virtual void*	QueryInterface(int id, void** ppT)				{ void* p=NULL; if(id==GUID_OF(CAgent)) p=this; if(ppT) *ppT=p; return p; }
	DEFINE_QI(IAiCenter, m_pAiCenter)
	DEFINE_QI(CSense, m_pSense)
	DEFINE_QI(CBody, m_pBody)
	DEFINE_QI(CTeam, m_pTeam)
	DEFINE_QI(CItemPack, m_pItemPack)
	DEFINE_QI(CRoleMove, m_pMove)
	DEFINE_QI(CRoleFight, m_pFight)

public:
	bool		Create(OBJID idAgent, int nTimeOfLife);
	bool		AppendInfo(const UserInfoStruct& info);

	virtual IRoleAttr*	QueryRoleAttr()								{ return static_cast<IRoleAttr*>(this); }
	virtual CAutoLink<IRole>&	QueryLink(const CAutoLink<IRole>&)				{ return m_lnkThis; }

public:
	virtual OBJID		GetID()										{ return m_info.id; }
	virtual OBJID		GetMapID();
	virtual int			GetPosX()									{ return m_pMove->GetPosX(); }
	virtual int			GetPosY()									{ return m_pMove->GetPosY(); }
	virtual int			GetDir()									{ return m_nDir; }
	virtual int			GetAttackRange(int nSizeAdd)				{ return m_pItemPack->GetWeaponRange() + (GetSizeAdd()+nSizeAdd+1)/2; }
	virtual int			GetSizeAdd()								{ return m_nSizeAdd; }

	virtual bool		IsAlive()									{ return (m_info.dwEffect & KEEPEFFECT_DIE) == 0; }
	virtual bool		IsFarWeapen()								{ return 0; }
	virtual bool		IsEvil()									{ return (m_info.dwEffect & (KEEPEFFECT_DEEPRED|KEEPEFFECT_CRIME)) != 0; }
	virtual bool		IsRighteous()								{ return (m_info.dwEffect & KEEPEFFECT_NOT_VIRTUOUS) == 0; }
	virtual bool		IsLurker()									{ return (m_info.dwEffect & KEEPEFFECT_LURKER) != 0; }
	virtual bool		IsWing()									{ return (m_info.dwEffect & KEEPEFFECT_WING) != 0; }
	virtual int			Distance(int x, int y)						{ return __max(abs(x-GetPosX()), abs(y-GetPosY())); }

public:
	virtual void		SetLife(int nData);
	virtual void		SetPower(int nData);
	virtual void		SetEffect(DWORD dwEffect);
	virtual void		SetSizeAdd(int nData)						{ m_nSizeAdd = nData; }

public:
	virtual void		OnTimer();
	virtual IRole*		QueryRole()									{ return (IRole*)this; }

public:
	virtual void		SetDie();
	virtual void		BeAttack(IRole* pRole);
	virtual void		BeKill(IRole* pRole);
	virtual void		AttackOnce();

public:
	virtual bool		SynchroPos(int nSourX, int nSourY, int nTargX, int nTargY);
	virtual bool		KickBack(int nTargX, int nTargY);
	virtual CGameMap*	GetMap()									{ return m_pMove->GetMap(); }
	virtual bool		IsBlockNpc()								{ return (GetEffect() & KEEPEFFECT_NOT_BLOCKNPC) == 0; }

	virtual bool	CheckCondition(bool bLogicNot, int idxFactFunction, VarTypeSetRef setParam, ARGUMENT_SET* psetArgument)
						{ return (bLogicNot==0) == (m_pSense->CheckCondition(idxFactFunction, setParam, psetArgument)!=0); }
	virtual int		Priority2Durable(int nPriority);

public:
	int				GetDistance(int x, int y)						{ return m_pMove->GetDistance(x, y); }
	int				GetLife()										{ return m_info.usLife; }
	LPCTSTR			GetName()										{ return m_info.szName; }
	int				GetLookRange()									{ return AGENT_VIEW_RANGE; }
	bool			IsCloseTarget(IRole* pRole)						{ return GetDistance(pRole->GetPosX(), pRole->GetPosY()) <= pRole->GetAttackRange(GetSizeAdd()); }
	bool			IsLookTarget(IRole* pRole)						{ return GetDistance(pRole->GetPosX(), pRole->GetPosY()) <= GetLookRange(); }
protected:
	int				GetMaxLife();
	int				GetProfessionSort	()							{ return (m_info.ucProfession % 1000) / 10; }
	int				GetProfessionLevel	()							{ return m_info.ucProfession % 10; }
	DWORD			GetEffect()										{ return m_info.dwEffect; }
	CTimeOutMS&		QueryTime()										{ return m_tAction; }

public:
	bool Login();
	void TalkToMe(LPCTSTR szSender, LPCTSTR szWords, LPCTSTR szEmotion);
	void PrivateTalkToMe(LPCTSTR szSender, LPCTSTR szWords, LPCTSTR szEmotion);
	bool			SendMsg(CNetMsg* pMsg);
	void			LoginOK()										{ 	MYASSERT(m_nStep == STEP_LOGIN); m_nStep = STEP_LOGIN_OK; }
protected:

protected:
	UserInfoStruct	m_info;
	int				m_nDir;
	int				m_nTimeOfLife;
	CAutoLink<IRole>		m_lnkThis;
	int				m_nSizeAdd;

protected:
	CJavaObj<IAiCenter>		m_pAiCenter;
	CJavaObj<CBody>			m_pBody;
	CJavaObj<CSense>		m_pSense;
	CJavaObj<CTeam>			m_pTeam;
	CJavaObj<CItemPack>		m_pItemPack;
	CJavaObj<CRoleMove>		m_pMove;
	CJavaObj<CRoleFight>	m_pFight;

protected:
	enum { STEP_NONE, STEP_OFFLINE, STEP_LOGIN, STEP_LOGIN_OK };
	int				m_nStep;
	CTimeOutMS		m_tAction;
};

#endif
