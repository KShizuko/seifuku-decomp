#if !defined(INTERFACE_ROLE_HEAD_FILE)
#define INTERFACE_ROLE_HEAD_FILE

#include "I_MapData.h"
#include "AutoLink.h"

const DWORD		KEEPEFFECT_NORMAL			=0x00000000;
const DWORD		KEEPEFFECT_CRIME			=0x00000001;
const DWORD		KEEPEFFECT_FREEZE			=0x00000008;
const DWORD		KEEPEFFECT_DIE				=0x00000020;
const DWORD		KEEPEFFECT_TEAMLEADER		=0x00000040;
const DWORD		KEEPEFFECT_GHOST			=0x00000400;
const DWORD		KEEPEFFECT_RED				=0x00004000;
const DWORD		KEEPEFFECT_DEEPRED			=0x00008000;
const DWORD		KEEPEFFECT_LURKER			=0x00400000;
const DWORD		KEEPEFFECT_SYNCRIME			=0x01000000;
const DWORD		KEEPEFFECT_WING				=0x08000000;
const DWORD		KEEPEFFECT_INSTEAD			=0x20000000;

const DWORD		KEEPEFFECT_NOT_ACTIVE		=KEEPEFFECT_DIE|KEEPEFFECT_FREEZE;
const DWORD		KEEPEFFECT_NOT_BLOCKNPC		=KEEPEFFECT_GHOST|KEEPEFFECT_WING;
const DWORD		KEEPEFFECT_NOT_VIRTUOUS		=KEEPEFFECT_RED|KEEPEFFECT_DEEPRED|KEEPEFFECT_CRIME;

class CUser;
class CNpc;
class CAgent;
class IRole : public IRoleAttr
{
protected:
	IRole(){}
	virtual ~IRole(){}
public:
	template<TYPENAME T>
		T*				QueryInterface(T** ppT)					{ return (T*)QueryInterface(GUID_OF(T), (void**)ppT); }
	virtual void*		QueryInterface(int idObjType, void** ppT)		PURE_VIRTUAL_FUNCTION_0
	virtual IRoleAttr*	QueryRoleAttr()							PURE_VIRTUAL_FUNCTION_0
	virtual CAutoLink<IRole>&	QueryLink(const CAutoLink<IRole>&)				=0;

public:
	virtual OBJID		GetID()									PURE_VIRTUAL_FUNCTION_0

	virtual OBJID		GetMapID()								PURE_VIRTUAL_FUNCTION_0
	virtual CGameMap*	GetMap()								PURE_VIRTUAL_FUNCTION_0
	virtual int			GetPosX()								PURE_VIRTUAL_FUNCTION_0
	virtual int			GetPosY()								PURE_VIRTUAL_FUNCTION_0
	virtual int			GetDir()								PURE_VIRTUAL_FUNCTION_0
	virtual int			GetAttackRange(int nSizeAdd)			PURE_VIRTUAL_FUNCTION_0
	virtual int			GetSizeAdd()							PURE_VIRTUAL_FUNCTION_0

public:
	virtual bool		IsAlive()								PURE_VIRTUAL_FUNCTION_0
	virtual bool		IsFarWeapen()							PURE_VIRTUAL_FUNCTION_0
	virtual bool		IsEvil()								PURE_VIRTUAL_FUNCTION_0
	virtual bool		IsRighteous()							PURE_VIRTUAL_FUNCTION_0
	virtual bool		IsLurker()								PURE_VIRTUAL_FUNCTION_0
	virtual bool		IsWing()								PURE_VIRTUAL_FUNCTION_0

	virtual void		SetLife(int nData)							PURE_VIRTUAL_FUNCTION
	virtual void		SetPower(int nData)							PURE_VIRTUAL_FUNCTION
	virtual void		SetEffect(DWORD dwEffect)					PURE_VIRTUAL_FUNCTION
	virtual void		SetSizeAdd(int nData)						PURE_VIRTUAL_FUNCTION
	virtual DWORD		GetEffect()									PURE_VIRTUAL_FUNCTION_0

public:
	virtual int			Distance(int x, int y)						PURE_VIRTUAL_FUNCTION_0

};

class CNpc;
class CAgent;
class INpc : public IRole
{
public:
	virtual void		ReleaseByOwner()							PURE_VIRTUAL_FUNCTION
	virtual void		OnTimer()									PURE_VIRTUAL_FUNCTION
	virtual IRole*		QueryRole()									PURE_VIRTUAL_FUNCTION_0
	template<TYPENAME T>
		T*				QueryInterface(T** ppT)						{ return (T*)QueryInterface(GUID_OF(T), (void**)ppT); }
	virtual void*		QueryInterface(int id, void** ppT)			PURE_VIRTUAL_FUNCTION_0
	IRole*				QueryInterface(IRole** ppT)					{ if(ppT) *ppT=static_cast<IRole*>(this); return static_cast<IRole*>(this); }

public:
	virtual void		SetDie()									PURE_VIRTUAL_FUNCTION
	virtual void		BeAttack(IRole* pRole)						PURE_VIRTUAL_FUNCTION
	virtual void		BeKill(IRole* pRole)						PURE_VIRTUAL_FUNCTION
	virtual void		AttackOnce()								PURE_VIRTUAL_FUNCTION

public:
	virtual bool		SynchroPos(int nSourX, int nSourY, int nTargX, int nTargY)			PURE_VIRTUAL_FUNCTION_0
	virtual bool		KickBack(int nTargX, int nTargY)			PURE_VIRTUAL_FUNCTION_0
	virtual OBJID		GetMapID()									PURE_VIRTUAL_FUNCTION_0
	virtual bool		IsBlockNpc()								PURE_VIRTUAL_FUNCTION_0
	
public:
	virtual void		TalkToMe(LPCTSTR szSender, LPCTSTR szWords, LPCTSTR szEmotion)	PURE_VIRTUAL_FUNCTION
};

inline int	Distance(int x0, int y0, int x1, int y1)			{ return __max(abs(x0-x1), abs(y0-y1)); }

#endif
