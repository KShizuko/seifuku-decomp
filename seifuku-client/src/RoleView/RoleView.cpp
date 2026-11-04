#include "IRoleView.h"
#include "3DRole.h"
#include "3DStatuary.h"
#include "3DTarget.h"
#include "3DTaskNpc.h"
#include "SynStone.h"
#include "BoothNpc.h"
#include "DiceView.h"
#include "TerrainNpcView.h"
#include "NpcXView.h"

IRoleView* RoleViewCreate ( int nViewType )
{
	switch (nViewType)
	{
	case _ROLE_VIEW_ROLE:		return new C3DRole;
	case _ROLE_VIEW_STATUARY:	return new C3DStatuary;
	case _ROLE_VIEW_TARGET:		return new C3DTarget;
	case _ROLE_VIEW_TASKNPC:	return new C3DTaskNpc;
	case _ROLE_VIEW_SYNSTONE:	return new CSynStone;
	case _ROLE_VIEW_BOOTH:		return new CBoothNpc;
	case _ROLE_VIEW_MAPMAGICOBJ:	return NULL;
	case _ROLE_VIEW_DICENPC:	return new CDiceView;
	case _ROLE_VIEW_TERRAIN:	return new CTerrainNpcView;
	case _ROLE_VIEW_NPCX:		return new CNpcXView;
	}
	return NULL;
}
