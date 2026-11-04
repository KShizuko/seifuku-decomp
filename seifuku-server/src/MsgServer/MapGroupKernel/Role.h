#if !defined(AFX_ROLE_H__A27D6633_2132_4F63_AA77_4EB7740D2D67__INCLUDED_)
#define AFX_ROLE_H__A27D6633_2132_4F63_AA77_4EB7740D2D67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#pragma warning(disable:4786)
#include <windows.h>
#include "common.h"
#include "define.h"
#include "I_Role.h"

class CRole : public IRole, public IMapThing
{
public:
	static bool	AttachStatus		(IRole* pRole, int nStatus, int nPower, int nSecs, int nTimes=0);
	static void	DetachWellStatus	(IRole* pRole);
	static void	DetachBadlyStatus	(IRole* pRole);
	static bool	DetachStatus		(IRole* pRole, int nStatus);
	static void	DetachAllStatus		(IRole* pRole);
	static bool	IsWellStatus		(int nStatus)
	{
		switch (nStatus)
		{
		case	STATUS_HITRATE:
		case	STATUS_DEFENCE:
		case	STATUS_ATTACK:
		case	STATUS_ADJUST_MAGICDEF:
		case	STATUS_SUPER_MATK:
		case	STATUS_ATKRANGE:
		case	0x00001000:
		case	STATUS_WEAPONDAMAGE:
		case	STATUS_MAGICDAMAGE:
		case	STATUS_ATKSPEED:
		case	STATUS_LURKER:
		case	0x00100000:
		case	STATUS_ADJUST_DODGE:
			return true;
		}
		return false;
	}
	static bool	IsBadlyStatus		(int nStatus)
	{
		switch (nStatus)
		{
		case	STATUS_POISON:
		case	STATUS_SYNCRIME:
			return true;
		}
		return false;
	}
};

#endif
