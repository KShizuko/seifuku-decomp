#include <winsock2.h>
#include <windows.h>
#include <stddef.h>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#define private public
#define protected public
#include "define.h"
#include "UserManager.h"
#include "NpcManager.h"
#include "MapManager.h"
#include "NpcWorld.h"
#include "AllMsg.h"
#include "Agent.h"
#include "Body.h"
#include "DynaNpcGen.h"
#include "GameMap.h"
#include "Item.h"
#include "ItemPack.h"
#include "ItemType.h"
#include "MapManager.h"
#include "NetWork/MsgAction.h"
#include "NetWork/MsgAiNpcInfo.h"
#include "NetWork/MsgInteract.h"
#include "NetWork/MsgItem.h"
#include "NetWork/MsgItemInfo.h"
#include "NetWork/MsgLogin.h"
#include "NetWork/MsgMagicEffect.h"
#include "NetWork/MsgNpc.h"
#include "NetWork/MsgPlayer.h"
#include "NetWork/MsgSyndicate.h"
#include "NetWork/MsgTalk.h"
#include "NetWork/MsgTeam.h"
#include "NetWork/MsgTeamMember.h"
#include "NetWork/MsgUserAttrib.h"
#include "NetWork/MsgUserInfo.h"
#include "NetWork/MsgWalk.h"
#include "Npc.h"
#include "NpcGenerator.h"
#include "NpcManager.h"
#include "NpcType.h"
#include "NpcWorld.h"
#include "RoleFight.h"
#include "RoleMove.h"
#include "Sense.h"
#include "SynManager.h"
#include "Team.h"
#include "User.h"
#include "UserManager.h"

#ifdef NDEBUG

namespace
{
	const size_t STLP_MAP		= 0x0C;
	const size_t STLP_LIST		= 0x04;
	const size_t STLP_DEQUE		= 0x28;
	const size_t STLP_STRING	= 0x0C;
	const size_t STLP_VECBOOL	= 0x14;
	const size_t VC6_STRING		= 0x10;
}

static_assert(sizeof(CAgent) == 0xD0, "");
static_assert(sizeof(CBody) == 0x8, "");
static_assert(sizeof(CDynaNpcGen) == 0x14, "");
static_assert(sizeof(CGameMap) == 0x20, "");
static_assert(sizeof(CItem) == 0x5C, "");
static_assert(sizeof(CItemPack) == 0x14, "");
static_assert(sizeof(CItemType) == 0x8, "");
static_assert(sizeof(CMapManager) == 0x8, "");
static_assert(sizeof(CMsgAction) == 0x414, "");
static_assert(sizeof(CMsgAiNpcInfo) == 0x414, "");
static_assert(sizeof(CMsgInteract) == 0x414, "");
static_assert(sizeof(CMsgItem) == 0x414, "");
static_assert(sizeof(CMsgItemInfo) == 0x414, "");
static_assert(sizeof(CMsgLogin) == 0x414, "");
static_assert(sizeof(CMsgMagicEffect) == 0x414, "");
static_assert(sizeof(CMsgNpc) == 0x414, "");
static_assert(sizeof(CMsgPlayer) == 0x420, "");
static_assert(sizeof(CMsgSyndicate) == 0x414, "");
static_assert(sizeof(CMsgTalk) == 0x420, "");
static_assert(sizeof(CMsgTeam) == 0x414, "");
static_assert(sizeof(CMsgTeamMember) == 0x414, "");
static_assert(sizeof(CMsgUserAttrib) == 0x414, "");
static_assert(sizeof(CMsgUserInfo) == 0x420, "");
static_assert(sizeof(CMsgWalk) == 0x414, "");
static_assert(sizeof(CNpc) == 0xC8, "");
static_assert(sizeof(CNpcGenerator) == 0x34, "");
static_assert(sizeof(CNpcType) == 0xC, "");
static_assert(sizeof(CNpcWorld) == 0x34, "");
static_assert(sizeof(CRoleFight) == 0x28, "");
static_assert(sizeof(CSense) == 0x10, "");
static_assert(sizeof(CSynManager) == 0x8, "");
static_assert(sizeof(CSyndicate) == 0x20, "");
static_assert(sizeof(CTeam) == 0x18, "");
static_assert(sizeof(CUser) == 0x6C, "");
static_assert(sizeof(CUserManager) == 0x8, "");

static_assert(offsetof(CMsgUserInfo::MSG_Info, dwReserved16) == 0x16, "");
static_assert(offsetof(CMsgUserInfo::MSG_Info, nDeed) == 0x1A, "");
static_assert(offsetof(CMsgUserInfo::MSG_Info, dwMedal) == 0x1E, "");
static_assert(offsetof(CMsgUserInfo::MSG_Info, dwMedalSelect) == 0x22, "");
static_assert(offsetof(CMsgUserInfo::MSG_Info, dwVirtue) == 0x26, "");

static_assert(offsetof(CNpc, m_i64Effect) == 0x40, "");

static_assert(offsetof(CNpcGenerator, m_nTotalGen) == 0xC, "");
static_assert(offsetof(CNpcGenerator, m_tBadGen) == 0x28, "");

static_assert(offsetof(ST_USERINFO, i64Effect) == 0x1C, "");
static_assert(offsetof(ST_USERINFO, dwMedalSelect) == 0x34, "");

static_assert(offsetof(UserInfoStruct, dwReserved3C) == 0x3C, "");
static_assert(offsetof(UserInfoStruct, dwEffect) == 0x74, "");

static_assert(sizeof(CNpcManager) == 0x6C - 2 * (STLP_DEQUE - sizeof(CMyQueue<OBJID>)), "");
static_assert(sizeof(CRoleMove) == 0x54 - (STLP_DEQUE - sizeof(CRoleMove::m_setPath)), "");

static_assert(sizeof(CNetMsg::m_bufMsg) == 0x400, "");
static_assert(sizeof(ItemInfoStruct) == 0x50, "");
static_assert(sizeof(ST_USERINFO) == 0x48, "");
static_assert(sizeof(UserInfoStruct) == 0x88, "");

#endif
