#if !defined(AFX_IROLEVIEW_H__ROLEVIEW_4343_INCLUDED_)
#define AFX_IROLEVIEW_H__ROLEVIEW_4343_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
#include "BaseFunc.h"

#ifdef	ROLEVIEW_EXPORTS
#define ROLEVIEW_API __declspec ( dllexport )
#else
#define ROLEVIEW_API __declspec ( dllimport )
#endif

enum
{
	_ROLE_VIEW_NONE			= 0,
	_ROLE_VIEW_ROLE			= 1,
	_ROLE_VIEW_STATUARY		= 2,
	_ROLE_VIEW_TARGET		= 3,
	_ROLE_VIEW_TASKNPC		= 4,
	_ROLE_VIEW_SYNSTONE		= 5,
	_ROLE_VIEW_BOOTH		= 6,
	_ROLE_VIEW_MAPMAGICOBJ	= 7,
	_ROLE_VIEW_DICENPC		= 8,
	_ROLE_VIEW_TERRAIN		= 9,
	_ROLE_VIEW_NPCX			= 10,
};

enum
{
	_VAR_LOOK				= 0,
	_VAR_HAIR				= 1,
	_VAR_HEAD				= 2,
	_VAR_TRANSFORM			= 3,
	_VAR_ARMOR				= 4,
	_VAR_ARMET				= 5,
	_VAR_RWEAPON			= 6,
	_VAR_LWEAPON			= 7,
	_VAR_MOUNT				= 8,
	_VAR_MISC				= 9,
	_VAR_FRAME				= 10,
	_VAR_ROTATEX			= 11,
	_VAR_RWEAPON_ID			= 12,
	_VAR_LWEAPON_ID			= 13,
	_VAR_ARMET_ID			= 14,
	_VAR_ARMOR_ID			= 15,
	_VAR_MOUNT_ID			= 16,
	_VAR_MISC_ID			= 17,
	_VAR_WEAPON_ACTION_DATA	= 18,
	_VAR_DIR				= 19,
	_VAR_DIRECT_DIR			= 20,
	_VAR_MAX_LIFE			= 21,
	_VAR_LIFE				= 22,
	_VAR_ITEM0				= 23,
	_VAR_ITEM1				= 24,
	_VAR_DICE0				= 27,
	_VAR_DICE1				= 28,
	_VAR_DICE2				= 29,
	_VAR_ACTION_DATA		= 30,
	_VAR_BLEND				= 31,
	_VAR_DRAWTYPE			= 32,
	_VAR_ROTATEY			= 33,
	_VAR_ROTATEZ			= 34,
};

enum
{
	_ROLE_PART_ARMET		= 1,
	_ROLE_PART_BODY			= 4,
	_ROLE_PART_RWEAPON		= 6,
	_ROLE_PART_LWEAPON		= 7,
	_ROLE_PART_MOUNT		= 8,
	_ROLE_PART_MISC			= 9,
};

class IRoleView
{
public:
	virtual DWORD	Release		( void ) { delete this; return 0; }
	virtual BOOL	SetVariable(int nVariableType, DWORD dwData) = 0;
	virtual int		SetAction(int nActionType, BOOL bResetMotion= true, BOOL bPlayerSound = false, int nWorldX=0, int nWorldY=0, BOOL bSetEffect = false) = 0;
	virtual void	SetPos ( float fX, float fY, float fHeight, int nRotate, float fScale ) = 0;
	virtual void	SetLightOffset( int type, float x, float y, float z, float a, float r, float g, float b ) = 0;
	virtual void	SetRGBA ( float alpha, float red, float green, float blue ) = 0;
	virtual void	AddPartEffect ( int nPart, const char* pszEffect ) = 0;
	virtual void	ClearPartEffect ( int nPart, const char* pszEffect ) = 0;
	virtual BOOL	HitTest ( int nHitScreenX, int nHitScreenY, int nMapX, int nMapY ) = 0;
	virtual BOOL	Draw ( int nMapX, int nMapY ) = 0;
	virtual DWORD	GetVariable(int nVariableType) = 0;
	virtual BOOL	Reload ( void ) { return true; }
	virtual ~IRoleView() {}
};

extern "C" ROLEVIEW_API IRoleView* RoleViewCreate ( int nViewType );

#endif
