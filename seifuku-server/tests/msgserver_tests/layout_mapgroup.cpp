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
#include "MessagePort.h"
#include "AllMsg.h"
#include "MapGroup.h"
#include "User.h"
#include "StringManager.h"
#include "AINpc.h"
#include "ActionAnalyser.h"
#include "Agent.h"
#include "BattleSystem.h"
#include "BetOutside.h"
#include "Booth.h"
#include "ClientDetector.h"
#include "DisdainDataSet.h"
#include "Enemy.h"
#include "Friend.h"
#include "FuseItem.h"
#include "GMToolCfg.h"
#include "GameAction.h"
#include "GameMap.h"
#include "Item.h"
#include "ItemAddition.h"
#include "ItemData.h"
#include "ItemType.h"
#include "Lottery.h"
#include "Adapter.h"
#include "LotteryActor.h"
#include "LotteryPrize.h"
#include "MapGroup.h"
#include "MapItem.h"
#include "MapManager.h"
#include "MapTrap.h"
#include "Network/MsgAction.h"
#include "Network/MsgAiNpcInfo.h"
#include "Network/MsgAllot.h"
#include "Network/MsgBetOutside.h"
#include "Network/MsgCallPetInfo.h"
#include "Network/MsgDataArray.h"
#include "Network/MsgDialog.h"
#include "Network/MsgFriend.h"
#include "Network/MsgFriendInfo.h"
#include "Network/MsgFuse.h"
#include "Network/MsgGemEmbed.h"
#include "Network/MsgInteract.h"
#include "Network/MsgItem.h"
#include "Network/MsgItemInfo.h"
#include "Network/MsgItemInfoEx.h"
#include "Network/MsgMapInfo.h"
#include "Network/MsgMessageBoard.h"
#include "Network/MsgName.h"
#include "Network/MsgNpc.h"
#include "Network/MsgNpcInfo.h"
#include "Network/MsgNpcInfoEx.h"
#include "Network/MsgPackage.h"
#include "Network/MsgPlayer.h"
#include "Network/MsgSynAttrInfo.h"
#include "Network/MsgSynMemberInfo.h"
#include "Network/MsgSyndicate.h"
#include "Network/MsgTalk.h"
#include "Network/MsgTeam.h"
#include "Network/MsgTeamAward.h"
#include "Network/MsgTeamMember.h"
#include "Network/MsgTick.h"
#include "Network/MsgUserAttrib.h"
#include "Network/MsgUserInfo.h"
#include "Network/MsgWalk.h"
#include "Network/MsgWeaponSkill.h"
#include "Network/MsgWeather.h"
#include "Network/NetMsg.h"
#include "Network/NetMsgTracker.h"
#include "Npc.h"
#include "NpcLink.h"
#include "NpcManager.h"
#include "NpcScene.h"
#include "NpcShop.h"
#include "NpcStorage.h"
#include "NpcTask.h"
#include "Package.h"
#include "SquareDeal.h"
#include "StatusMore.h"
#include "StatusOnce.h"
#include "SynAttr.h"
#include "Syndicate.h"
#include "Team.h"
#include "TradeBuddy.h"
#include "Transformation.h"
#include "User.h"
#include "UserData.h"
#include "UserWeaponSkill.h"
#include "WeatherRegion.h"
#include "magic.h"
#include "protocol.h"

extern "C" char sfk_layout_no_mapgroup_set[1] = { 0 };
#pragma comment(linker, "/alternatename:?m_setMapGroup@CMapGroup@@2V?$vector@VCMapGroup@@V?$allocator@VCMapGroup@@@std@@@std@@A=_sfk_layout_no_mapgroup_set")

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

static_assert(sizeof(CBattleSystem) == 0x10, "");
static_assert(sizeof(CBooth) == 0x13C, "");
static_assert(sizeof(CClientDetector) == 0x28, "");
static_assert(sizeof(CEnemy) == 0x18, "");
static_assert(sizeof(CFriend) == 0x24, "");
static_assert(sizeof(CGMToolCfg) == 0x18, "");
static_assert(sizeof(CGameAction) == 0x3C, "");
static_assert(sizeof(CGameMap) == 0x34, "");
static_assert(sizeof(CItem) == 0x14, "");
static_assert(sizeof(CItemAddition) == 0x8, "");
static_assert(sizeof(CItemType) == 0x8, "");
static_assert(sizeof(CMagic) == 0x4C, "");
static_assert(sizeof(CMapItem) == 0x7C, "");
static_assert(sizeof(CMapTrap) == 0x54, "");
static_assert(sizeof(CMonster) == 0xA0, "");
static_assert(sizeof(CMsgAction) == 0x420, "");
static_assert(sizeof(CMsgAllot) == 0x420, "");
static_assert(sizeof(CMsgBODice) == 0x420, "");
static_assert(sizeof(CMsgCallPetInfo) == 0x420, "");
static_assert(sizeof(CMsgDataArray) == 0x420, "");
static_assert(sizeof(CMsgDialog) == 0x42C, "");
static_assert(sizeof(CMsgFriend) == 0x420, "");
static_assert(sizeof(CMsgFriendInfo) == 0x420, "");
static_assert(sizeof(CMsgFuse) == 0x420, "");
static_assert(sizeof(CMsgGemEmbed) == 0x420, "");
static_assert(sizeof(CMsgInteract) == 0x420, "");
static_assert(sizeof(CMsgItem) == 0x420, "");
static_assert(sizeof(CMsgItemInfo) == 0x420, "");
static_assert(sizeof(CMsgItemInfoEx) == 0x420, "");
static_assert(sizeof(CMsgMapInfo) == 0x420, "");
static_assert(sizeof(CMsgMessageBoard) == 0x42C, "");
static_assert(sizeof(CMsgMonsterInfo) == 0x420, "");
static_assert(sizeof(CMsgName) == 0x42C, "");
static_assert(sizeof(CMsgNpc) == 0x420, "");
static_assert(sizeof(CMsgNpcInfo) == 0x42C, "");
static_assert(sizeof(CMsgNpcInfoEx) == 0x42C, "");
static_assert(sizeof(CMsgPlayer) == 0x42C, "");
static_assert(sizeof(CMsgSynAttrInfo) == 0x420, "");
static_assert(sizeof(CMsgSynMemberInfo) == 0x420, "");
static_assert(sizeof(CMsgSyndicate) == 0x420, "");
static_assert(sizeof(CMsgTalk) == 0x42C, "");
static_assert(sizeof(CMsgTeam) == 0x420, "");
static_assert(sizeof(CMsgTeamMember) == 0x420, "");
static_assert(sizeof(CMsgTick) == 0x420, "");
static_assert(sizeof(CMsgUserAttrib) == 0x420, "");
static_assert(sizeof(CMsgUserInfo) == 0x42C, "");
static_assert(sizeof(CMsgWalk) == 0x420, "");
static_assert(sizeof(CMsgWeaponSkill) == 0x420, "");
static_assert(sizeof(CMsgWeather) == 0x420, "");
static_assert(sizeof(CNetMsg) == 0x41C, "");
static_assert(sizeof(CNpc) == 0x68, "");
static_assert(sizeof(CNpcBet) == 0x3C, "");
static_assert(sizeof(CNpcLink) == 0x10, "");
static_assert(sizeof(CNpcManager) == 0x8, "");
static_assert(sizeof(CNpcScene) == 0x10, "");
static_assert(sizeof(CNpcTask) == 0x14, "");
static_assert(sizeof(CNpcTrunk) == 0x10, "");
static_assert(sizeof(CPackage) == 0x14, "");
static_assert(sizeof(CSquareDeal) == 0x54, "");
static_assert(sizeof(CStatusMore) == 0x20, "");
static_assert(sizeof(CStatusOnce) == 0x1C, "");
static_assert(sizeof(CSynAttr) == 0x1C, "");
static_assert(sizeof(CSyndicate) == 0x34, "");
static_assert(sizeof(CTeam) == 0x2C, "");
static_assert(sizeof(CTradeBuddy) == 0x28, "");
static_assert(sizeof(CTransformation) == 0x10, "");
static_assert(sizeof(CUserWeaponSkill) == 0x20, "");
static_assert(sizeof(CWeatherRegion) == 0x10, "");

static_assert(offsetof(CActionAnalyser, m_setTime) == 0x4, "");
static_assert(offsetof(CActionAnalyser, m_dwAmount) == 0x10, "");
static_assert(offsetof(CActionAnalyser, m_nMinAverage) == 0x14, "");
static_assert(offsetof(CActionAnalyser, m_dwMinDeviation) == 0x18, "");
static_assert(offsetof(CActionAnalyser, m_dwMaxInterval) == 0x1C, "");

static_assert(offsetof(CBooth, m_setSysBuyTime) == 0x4, "");
static_assert(offsetof(CBooth, m_setGoods) == 0x10, "");
static_assert(offsetof(CBooth, m_pUser) == 0x1C, "");
static_assert(offsetof(CBooth, m_id) == 0x20, "");
static_assert(offsetof(CBooth, m_pMap) == 0x24, "");
static_assert(offsetof(CBooth, m_nPosX) == 0x28, "");
static_assert(offsetof(CBooth, m_nPosY) == 0x2C, "");
static_assert(offsetof(CBooth, m_nDir) == 0x30, "");
static_assert(offsetof(CBooth, m_szCryOut) == 0x34, "");
static_assert(offsetof(CBooth, m_idProcess) == 0x134, "");
static_assert(offsetof(CBooth, m_bEnterMap) == 0x138, "");

static_assert(offsetof(CBooth::GOODS_ST, nEmoney) == 0xC, "");

static_assert(offsetof(CClientDetector, m_bFirst) == 0x4, "");
static_assert(offsetof(CClientDetector, m_nType) == 0x8, "");
static_assert(offsetof(CClientDetector, m_nInterval) == 0xC, "");
static_assert(offsetof(CClientDetector, m_nTimeout) == 0x10, "");
static_assert(offsetof(CClientDetector, m_tDetect) == 0x14, "");
static_assert(offsetof(CClientDetector, m_tWait) == 0x1C, "");
static_assert(offsetof(CClientDetector, m_pUser) == 0x24, "");

static_assert(offsetof(CDisdainDataSet, m_map) == 0x4, "");

static_assert(offsetof(CFuseItem, m_setFuse) == 0x4, "");

static_assert(offsetof(CGMToolCfg, m_setUnknown) == 0x0, "");
static_assert(offsetof(CGMToolCfg, m_setRegion) == 0xC, "");

static_assert(offsetof(CGameAction, m_idProcess) == 0x38, "");

static_assert(offsetof(CGameMap, m_bNewbieMap) == 0x30, "");

static_assert(offsetof(CItemData, m_pData) == 0x8, "");
static_assert(offsetof(CItemData, m_pType) == 0xC, "");
static_assert(offsetof(CItemData, m_pAddition) == 0x10, "");

static_assert(offsetof(CMagic, m_setData) == 0x4, "");
static_assert(offsetof(CMagic, m_pMagic) == 0x8, "");
static_assert(offsetof(CMagic, m_idTarget) == 0xC, "");
static_assert(offsetof(CMagic, m_pos) == 0x10, "");
static_assert(offsetof(CMagic, m_nData) == 0x18, "");
static_assert(offsetof(CMagic, m_tIntone) == 0x1C, "");
static_assert(offsetof(CMagic, m_tDelay) == 0x24, "");
static_assert(offsetof(CMagic, m_bAutoAttack) == 0x2C, "");
static_assert(offsetof(CMagic, m_nRawDelay) == 0x30, "");
static_assert(offsetof(CMagic, m_nDelay) == 0x34, "");
static_assert(offsetof(CMagic, m_dwUnk38) == 0x38, "");
static_assert(offsetof(CMagic, m_dwUnk3C) == 0x3C, "");
static_assert(offsetof(CMagic, m_idProcess) == 0x40, "");
static_assert(offsetof(CMagic, m_pUser) == 0x44, "");

static_assert(offsetof(CMapGroup::EVENT_INFO, tmEvent) == 0x0, "");
static_assert(offsetof(CMapGroup::EVENT_INFO, idAction) == 0x8, "");

static_assert(offsetof(CMapItem, m_nColor) == 0x64, "");

static_assert(offsetof(CMapTrap, m_idProcess) == 0x10, "");
static_assert(offsetof(CMapTrap, m_info) == 0x14, "");
static_assert(offsetof(CMapTrap, m_tFight) == 0x28, "");
static_assert(offsetof(CMapTrap, m_bDie) == 0x30, "");
static_assert(offsetof(CMapTrap, m_nTimes) == 0x34, "");
static_assert(offsetof(CMapTrap, m_pUser) == 0x38, "");
static_assert(offsetof(CMapTrap, m_setStatus) == 0x44, "");
static_assert(offsetof(CMapTrap, m_pMap) == 0x48, "");
static_assert(offsetof(CMapTrap, m_pType) == 0x4C, "");
static_assert(offsetof(CMapTrap, m_pBattleSystem) == 0x50, "");

static_assert(offsetof(CMonster, m_tFootPrint) == 0x10, "");
static_assert(offsetof(CMonster, m_posFootPrint) == 0x14, "");
static_assert(offsetof(CMonster, m_setStatus) == 0x1C, "");
static_assert(offsetof(CMonster, m_pOwner) == 0x20, "");
static_assert(offsetof(CMonster, m_link) == 0x2C, "");
static_assert(offsetof(CMonster, m_pMagicType) == 0x38, "");
static_assert(offsetof(CMonster, m_tMagic) == 0x3C, "");
static_assert(offsetof(CMonster, m_pData) == 0x44, "");
static_assert(offsetof(CMonster, m_idNpc) == 0x48, "");
static_assert(offsetof(CMonster, m_nPosX) == 0x4C, "");
static_assert(offsetof(CMonster, m_nPosY) == 0x50, "");
static_assert(offsetof(CMonster, m_nDir) == 0x54, "");
static_assert(offsetof(CMonster, m_dwStatus) == 0x58, "");
static_assert(offsetof(CMonster, m_dwEffect) == 0x5C, "");
static_assert(offsetof(CMonster, m_nPose) == 0x60, "");
static_assert(offsetof(CMonster, m_pMap) == 0x64, "");
static_assert(offsetof(CMonster, m_nCurrLife) == 0x68, "");
static_assert(offsetof(CMonster, m_nCurrMana) == 0x6C, "");
static_assert(offsetof(CMonster, m_pBattleSystem) == 0x70, "");
static_assert(offsetof(CMonster, m_tFight) == 0x74, "");
static_assert(offsetof(CMonster, m_nFightPause) == 0x7C, "");
static_assert(offsetof(CMonster, m_pType) == 0x80, "");
static_assert(offsetof(CMonster, m_tDie) == 0x84, "");
static_assert(offsetof(CMonster, m_bLeaveMap) == 0x8C, "");
static_assert(offsetof(CMonster, m_tAddLife) == 0x90, "");
static_assert(offsetof(CMonster, m_idGen) == 0x98, "");
static_assert(offsetof(CMonster, m_idProcess) == 0x9C, "");

static_assert(offsetof(CMsgCallPetInfo::MSG_Info, szName) == 0x10, "");

static_assert(offsetof(CMsgFriend::MSG_Info, idFriend) == 0x0, "");
static_assert(offsetof(CMsgFriend::MSG_Info, ucAction) == 0x4, "");
static_assert(offsetof(CMsgFriend::MSG_Info, ucOnlineFlag) == 0x5, "");

static_assert(offsetof(CMsgFriendInfo::MSG_Info, ucAction) == 0x20, "");

static_assert(offsetof(CMsgFuse::MSG_Info, idNpc) == 0x0, "");
static_assert(offsetof(CMsgFuse::MSG_Info, usType) == 0x4, "");
static_assert(offsetof(CMsgFuse::MSG_Info, usAmount) == 0x6, "");
static_assert(offsetof(CMsgFuse::MSG_Info, idItem) == 0x8, "");

static_assert(offsetof(CMsgInteract::MSG_Info, dwBattleEffect) == 0x18, "");

static_assert(offsetof(CMsgItemInfo::MSG_Info, id) == 0x0, "");
static_assert(offsetof(CMsgItemInfo::MSG_Info, dwType) == 0x4, "");
static_assert(offsetof(CMsgItemInfo::MSG_Info, usAmount) == 0x8, "");
static_assert(offsetof(CMsgItemInfo::MSG_Info, usAmountLimit) == 0xA, "");
static_assert(offsetof(CMsgItemInfo::MSG_Info, ucAction) == 0xC, "");
static_assert(offsetof(CMsgItemInfo::MSG_Info, ucIdent) == 0xD, "");
static_assert(offsetof(CMsgItemInfo::MSG_Info, ucPosition) == 0xE, "");
static_assert(offsetof(CMsgItemInfo::MSG_Info, ucGem1) == 0xF, "");
static_assert(offsetof(CMsgItemInfo::MSG_Info, ucMagic3) == 0x13, "");
static_assert(offsetof(CMsgItemInfo::MSG_Info, ucReduceDmg) == 0x14, "");
static_assert(offsetof(CMsgItemInfo::MSG_Info, ucAddLife) == 0x15, "");
static_assert(offsetof(CMsgItemInfo::MSG_Info, nData) == 0x18, "");

static_assert(offsetof(CMsgNpcInfo::MSG_Info, szBuf) == 0xE, "");

static_assert(offsetof(CMsgNpcInfoEx::MSG_Info, szBuf) == 0x16, "");

static_assert(offsetof(CMsgPlayer::MSG_Info, id) == 0x0, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, dwLookFace) == 0x4, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, dwStatus) == 0x8, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, dwSynID_Rank) == 0x10, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, dwMountType) == 0x14, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, dwHelmetType) == 0x18, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, dwArmorType) == 0x1C, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, dwWeaponRType) == 0x20, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, dwWeaponLType) == 0x24, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, usWeaponRAntiMonster) == 0x28, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, usWeaponLAntiMonster) == 0x2A, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, dwSpriteType) == 0x28, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, dwMantleType) == 0x2C, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, usLife) == 0x2C, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, usLevel) == 0x2E, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, usPosY) == 0x32, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, usHair) == 0x34, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, ucDir) == 0x36, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, ucPose) == 0x37, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, ucMetempsychosis) == 0x38, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, ucReserved) == 0x39, "");
static_assert(offsetof(CMsgPlayer::MSG_Info, szBuf) == 0x3A, "");

static_assert(offsetof(CMsgSynAttrInfo::MSG_Info, ucRankShow) == 0x10, "");

static_assert(offsetof(CMsgTeamAward::MSG_Info, dwAward0) == 0x0, "");
static_assert(offsetof(CMsgTeamAward::MSG_Info, dwMoney) == 0x4, "");
static_assert(offsetof(CMsgTeamAward::MSG_Info, dwVirtue) == 0x8, "");

static_assert(offsetof(CMsgTick::MSG_Info, idPlayer) == 0x0, "");
static_assert(offsetof(CMsgTick::MSG_Info, dwData) == 0x4, "");
static_assert(offsetof(CMsgTick::MSG_Info, dwChkData) == 0x8, "");
static_assert(offsetof(CMsgTick::MSG_Info, dwRandom) == 0xC, "");
static_assert(offsetof(CMsgTick::MSG_Info, dwNameChk) == 0x18, "");

static_assert(offsetof(CNetMsgTracker, m_strLogFile) == 0x0, "");

static_assert(offsetof(CNpc, m_nPosY) == 0x14, "");
static_assert(offsetof(CNpc, m_pShop) == 0x18, "");
static_assert(offsetof(CNpc, m_pTask) == 0x1C, "");
static_assert(offsetof(CNpc, m_pTrunk) == 0x20, "");
static_assert(offsetof(CNpc, m_pScene) == 0x24, "");
static_assert(offsetof(CNpc, m_pLinkMap) == 0x28, "");
static_assert(offsetof(CNpc, m_pBet) == 0x2C, "");
static_assert(offsetof(CNpc, m_id) == 0x30, "");
static_assert(offsetof(CNpc, m_pData) == 0x34, "");
static_assert(offsetof(CNpc, m_bDynaNpc) == 0x38, "");
static_assert(offsetof(CNpc, m_pMap) == 0x3C, "");
static_assert(offsetof(CNpc, m_setStatus) == 0x40, "");
static_assert(offsetof(CNpc, m_dwEffect) == 0x44, "");
static_assert(offsetof(CNpc, m_tSynEvent) == 0x48, "");
static_assert(offsetof(CNpc, m_tBroadcastScore) == 0x50, "");
static_assert(offsetof(CNpc, m_tDie) == 0x58, "");
static_assert(offsetof(CNpc, m_bFeeOK) == 0x60, "");
static_assert(offsetof(CNpc, m_idProcess) == 0x64, "");

static_assert(offsetof(CPackage, m_setItem) == 0x4, "");
static_assert(offsetof(CPackage, m_nType) == 0x8, "");
static_assert(offsetof(CPackage, m_idOwner) == 0xC, "");
static_assert(offsetof(CPackage, m_idPlayer) == 0x10, "");

static_assert(offsetof(CSquareDeal, m_pUser1) == 0x14, "");
static_assert(offsetof(CSquareDeal, m_pUser2) == 0x18, "");
static_assert(offsetof(CSquareDeal, m_setItem1) == 0x1C, "");
static_assert(offsetof(CSquareDeal, m_setItem2) == 0x28, "");
static_assert(offsetof(CSquareDeal, m_nAllWeight1) == 0x34, "");
static_assert(offsetof(CSquareDeal, m_nAllWeight2) == 0x38, "");
static_assert(offsetof(CSquareDeal, m_nMoney1) == 0x3C, "");
static_assert(offsetof(CSquareDeal, m_nMoney2) == 0x40, "");
static_assert(offsetof(CSquareDeal, m_nEmoney1) == 0x44, "");
static_assert(offsetof(CSquareDeal, m_nEmoney2) == 0x48, "");
static_assert(offsetof(CSquareDeal, m_bClickOK1) == 0x4C, "");
static_assert(offsetof(CSquareDeal, m_bClickOK2) == 0x4D, "");
static_assert(offsetof(CSquareDeal, m_bUnfairOK1) == 0x4E, "");
static_assert(offsetof(CSquareDeal, m_bUnfairOK2) == 0x4F, "");
static_assert(offsetof(CSquareDeal, m_bTradeSuccess) == 0x50, "");

static_assert(offsetof(CStatusMore, m_nCount) == 0x1C, "");

static_assert(offsetof(CStatusOnce, m_nCount) == 0x18, "");

static_assert(offsetof(CTeam, m_bUnk28) == 0x28, "");

static_assert(offsetof(CTradeBuddy, m_id) == 0x8, "");
static_assert(offsetof(CTradeBuddy, m_idUser) == 0xC, "");
static_assert(offsetof(CTradeBuddy, m_idBuddy) == 0x10, "");
static_assert(offsetof(CTradeBuddy, m_szName) == 0x14, "");
static_assert(offsetof(CTradeBuddy, m_dwDate) == 0x24, "");

static_assert(offsetof(GMTOOL_REGION, idMap) == 0x0, "");
static_assert(offsetof(GMTOOL_REGION, rc) == 0x4, "");

static_assert(offsetof(ItemInfoStruct, nReduceDmg) == 0x38, "");
static_assert(offsetof(ItemInfoStruct, nAddLife) == 0x3C, "");
static_assert(offsetof(ItemInfoStruct, nAntiMonster) == 0x40, "");
static_assert(offsetof(ItemInfoStruct, usPlunder) == 0x44, "");
static_assert(offsetof(ItemInfoStruct, usSpecialFlag) == 0x46, "");
static_assert(offsetof(ItemInfoStruct, nColor) == 0x48, "");

static_assert(offsetof(MsgPackageItemInfo, ucReduceDmg) == 0xE, "");
static_assert(offsetof(MsgPackageItemInfo, usAddLife) == 0x10, "");
static_assert(offsetof(MsgPackageItemInfo, usAntiMonster) == 0x12, "");

static_assert(offsetof(ST_LOGIN, nMinorFlag) == 0x8, "");

static_assert(offsetof(ST_LOTTERY, id) == 0x0, "");
static_assert(offsetof(ST_LOTTERY, nType) == 0x4, "");
static_assert(offsetof(ST_LOTTERY, nRank) == 0x8, "");
static_assert(offsetof(ST_LOTTERY, nChance) == 0xC, "");
static_assert(offsetof(ST_LOTTERY, strPrizeName) == 0x10, "");

static_assert(offsetof(SynAttrInfoStruct, ucRank) == 0x8, "");
static_assert(offsetof(SynAttrInfoStruct, usReserved) == 0xA, "");

static_assert(offsetof(UserWeaponSkillInfoStruct, ucUnlearn) == 0x15, "");

static_assert(sizeof(CDisdainDataSet) == 0x10 - (STLP_MAP - sizeof(CDisdainDataSet::m_map)), "");
static_assert(sizeof(CFuseItem) == 0x10 - (STLP_MAP - sizeof(CFuseItem::m_setFuse)), "");
static_assert(sizeof(CNpcShop) == 0x1C - (STLP_MAP - sizeof(CNpcShop::m_setMoneyType)), "");
static_assert(offsetof(CNpcShop, m_pOwner) == 0x14 - (STLP_MAP - sizeof(CNpcShop::m_setMoneyType)), "");
static_assert(offsetof(CNpcShop, m_idProcess) == 0x18 - (STLP_MAP - sizeof(CNpcShop::m_setMoneyType)), "");

static_assert(sizeof(CMapManager) == 0xDC - 4 * (STLP_DEQUE - sizeof(CMyQueue<OBJID>)), "");
static_assert(offsetof(CMapManager, m_idLastDynaNpc) == 0x30 - 1 * (STLP_DEQUE - sizeof(CMyQueue<OBJID>)), "");
static_assert(offsetof(CMapManager, m_idLastTrap) == 0x5C - 2 * (STLP_DEQUE - sizeof(CMyQueue<OBJID>)), "");
static_assert(offsetof(CMapManager, m_idLastCallPet) == 0x88 - 3 * (STLP_DEQUE - sizeof(CMyQueue<OBJID>)), "");
static_assert(offsetof(CMapManager, m_idProcess) == 0xB4 - 4 * (STLP_DEQUE - sizeof(CMyQueue<OBJID>)), "");
static_assert(offsetof(CMapManager, m_nMaxMapItem) == 0xC4 - 4 * (STLP_DEQUE - sizeof(CMyQueue<OBJID>)), "");
static_assert(offsetof(CMapManager, m_tMapItemLog) == 0xC8 - 4 * (STLP_DEQUE - sizeof(CMyQueue<OBJID>)), "");
static_assert(offsetof(CMapManager, m_pMapItemSet) == 0xCC - 4 * (STLP_DEQUE - sizeof(CMyQueue<OBJID>)), "");
static_assert(offsetof(CMapManager, m_tMapItem) == 0xD4 - 4 * (STLP_DEQUE - sizeof(CMyQueue<OBJID>)), "");

static_assert(offsetof(CMapGroup, m_tmHour) == 0x18 + (sizeof(CMapGroup::m_setEvent) - STLP_LIST), "");
static_assert(offsetof(CMapGroup, m_pAdapter) == 0x4C + (sizeof(CMapGroup::m_setEvent) - STLP_LIST), "");

static_assert(offsetof(CNetMsgTracker, m_setAccount) == 0x0C + (sizeof(std::string) - STLP_STRING), "");
static_assert(offsetof(CNetMsgTracker, m_setType) == 0x18 + (sizeof(std::string) - STLP_STRING) - (STLP_MAP - sizeof(CNetMsgTracker::m_setAccount)), "");

static_assert(offsetof(ST_LOTTERY, idPrizeItem) == 0x20 + (sizeof(std::string) - VC6_STRING), "");
static_assert(offsetof(ST_LOTTERY, nColor) == 0x24 + (sizeof(std::string) - VC6_STRING), "");
static_assert(offsetof(ST_LOTTERY, nHoleNum) == 0x28 + (sizeof(std::string) - VC6_STRING), "");
static_assert(offsetof(ST_LOTTERY, nAdditionLev) == 0x2C + (sizeof(std::string) - VC6_STRING), "");
static_assert(sizeof(ST_LOTTERY) == 0x30 + (sizeof(std::string) - VC6_STRING), "");

static_assert(offsetof(CGameAction, m_infoPkBonus) + offsetof(CGameAction::PKBONUS_INFO, szTargetName) == 0x10, "");
static_assert(offsetof(CGameAction, m_infoPkBonus) + offsetof(CGameAction::PKBONUS_INFO, szHunterName) == 0x20, "");
static_assert(offsetof(CGameAction, m_infoPkBonus) + offsetof(CGameAction::PKBONUS_INFO, dwBonus) == 0x30, "");
static_assert(offsetof(CGameAction, m_infoPkBonus) + offsetof(CGameAction::PKBONUS_INFO, dwNum) == 0x34, "");

static_assert(offsetof(CUserData, m_Info) == 8, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, i64Exp) == 0x48, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, ucOldProf) == 0x52, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, usSpeed) == 0x54, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, nDeed) == 0x64, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwMedal) == 0x6C, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwVirtue) == 0x90, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwGodStatus) == 0x9C, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwExpBallUsage) == 0xA0, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwEffect) == 0xA4, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwEmoney) == 0xE4, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwChkSum) == 0xE8, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwOnlineTime) == 0xEC, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwAutoExercise) == 0xF0, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwLastLogout) == 0xF4, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, usBigPrizeFails) == 0xF8, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, usSmallPrizeFails) == 0xFA, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwMeteLev) == 0xFC, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwCurrentLayoutType) == 0x100, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwOnlineTime2) == 0x104, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwOfflineTime) == 0x108, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwLastLogout2) == 0x10C, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, i64Donation) == 0x110, "");
static_assert(offsetof(CUserData, m_Info) + offsetof(UserInfoStruct, dwBusiness) == 0x118, "");

static_assert(sizeof(CUser) == 0x9E8, "");
static_assert(sizeof(CAgent) == sizeof(CUser), "");
static_assert(offsetof(CUser, m_tMine) == 0x24, "");
static_assert(offsetof(CUser, m_setDelayAction) == 0x120, "");
static_assert(offsetof(CUser, m_vecTradeBuddy) == 0x128, "");
static_assert(offsetof(CUser, m_setItem) == 0x134, "");
static_assert(offsetof(CUser, m_tLeaveWord) == 0x16C, "");
static_assert(offsetof(CUser, m_setJumpTime) == 0x1B4, "");
static_assert(offsetof(CUser, m_tCheatKickout) == 0x1C8, "");
static_assert(offsetof(CUser, m_setVerifyAnswer) == 0x1D0, "");
static_assert(offsetof(CUser, m_nVerifyIndex) == 0x5D0, "");
static_assert(offsetof(CUser, m_tVerify) == 0x5D4, "");
static_assert(offsetof(CUser, m_dwDetectData) == 0x5DC, "");
static_assert(offsetof(CUser, m_pVerifyDataDetector) == 0x5E0, "");
static_assert(offsetof(CUser, m_dwUnk2C) == 0x5F4, "");
static_assert(offsetof(CUser, m_pStatisticSet) == 0x5F8, "");
static_assert(offsetof(CUser, m_dwUnk34) == 0x5FC, "");
static_assert(offsetof(CUser, m_pAnalyser) == 0x600, "");
static_assert(offsetof(CUser, m_dwUnk5B0) == 0x604, "");
static_assert(offsetof(CUser, m_nDoubleExp) == 0x61C, "");
static_assert(offsetof(CUser, m_tDoubleExp) == 0x620, "");
static_assert(offsetof(CUser, m_dwUnk6AC) == 0x628, "");
static_assert(offsetof(CUser, m_i64LastSaveExp) == 0x630, "");
static_assert(offsetof(CUser, m_dwUnk8E8) == 0x658, "");
static_assert(offsetof(CUser, m_nUnk858) == 0x66C, "");
static_assert(offsetof(CUser, m_setStatus) == 0x700, "");
static_assert(offsetof(CUser, m_pOvercoat) == 0x77C, "");
static_assert(offsetof(CUser, m_nPosX) == 0x928, "");
static_assert(offsetof(CUser, m_nPosY) == 0x92C, "");
static_assert(offsetof(CUser, m_nDir) == 0x930, "");
static_assert(offsetof(CUser, m_pMap) == 0x95C, "");
static_assert(offsetof(CUser, m_tOnlineTraining) == 0x984, "");
static_assert(offsetof(CUser, m_tBattleEffect) == 0x98C, "");
static_assert(offsetof(CUser, m_tContributeExp) == 0x994, "");
static_assert(offsetof(CUser, m_tUnk89C) == 0x99C, "");
static_assert(offsetof(CUser, m_tAutoHealLife) == 0x9A4, "");
static_assert(offsetof(CUser, m_tEquipmentDurability) == 0x9AC, "");
static_assert(offsetof(CUser, m_tLock) == 0x9BC, "");
static_assert(offsetof(CUser, m_idProcess) == 0x9D4, "");
static_assert(offsetof(CUser, m_dwUnkF4) == 0x9D8, "");

static_assert(EVENTPACK_SIZE == 0x408, "");
static_assert(sizeof(CEnemyInfoStruct) == 0xC8, "");
static_assert(sizeof(CGameAction::PKBONUS_INFO) == 0x28, "");
static_assert(sizeof(CNetMsg::m_bufMsg) == 0x400, "");
static_assert(sizeof(FriendInfoStruct) == 0x1C, "");
static_assert(sizeof(ItemInfoStruct) == 0x4C, "");
static_assert(sizeof(NPCINFO_ST) == 0x6C, "");
static_assert(sizeof(NewMapInfo) == 0x44, "");
static_assert(sizeof(OBJID)*3 + _MAX_NAMESIZE + sizeof(DWORD) == 0x20, "");
static_assert(sizeof(ST_CREATENEWNPC) == 0x20, "");
static_assert(sizeof(ST_QUERYIP) == 0x8, "");
static_assert(sizeof(SynAttrInfoStruct) == 0x10, "");
static_assert(sizeof(TRAP_INFO) == 0x14, "");
static_assert(sizeof(TeamInfoStruct) == 0x28, "");
static_assert(sizeof(UserInfoStruct) == 0x118, "");
static_assert(sizeof(UserWeaponSkillInfoStruct) == 0x18, "");
static_assert(sizeof(WantedInfoStruct) == 0x1C, "");
static_assert(sizeof(char[_MAX_NAMESIZE]) == 0x10, "");
static_assert(sizeof(tm) == 0x24, "");

static_assert(offsetof(CStringManager, m_mapString) == 0x4, "");

static_assert(sizeof(CAdapter) == 0x1C - (STLP_MAP - sizeof(CAdapter::m_setModule)), "");
static_assert(offsetof(CAdapter, m_hCommon) == 0x8, "");
static_assert(offsetof(CAdapter, m_idProcess) == 0xC, "");
static_assert(offsetof(CAdapter, m_setModule) == 0x10, "");
static_assert(sizeof(CLotteryActor) == 0xC, "");
static_assert(offsetof(CLotteryActor, m_pUser) == 0x4, "");
static_assert(offsetof(CLotteryActor, m_idProcess) == 0x8, "");
static_assert(sizeof(CLotteryPrize) == 0xC, "");
static_assert(offsetof(CLotteryPrize, m_pItem) == 0x4, "");
static_assert(offsetof(CLotteryPrize, m_idProcess) == 0x8, "");
static_assert(offsetof(PLUGIN_MSG, unMsgSize) == 0x4, "");
static_assert(offsetof(PLUGIN_MSG, unMsgType) == 0x6, "");
static_assert(offsetof(PLUGIN_MSG, bufBody) == 0x8, "");
static_assert(sizeof(PLUGIN_ITEMINFO) == 0x48, "");
static_assert(offsetof(PLUGIN_ITEMINFO, nData) == 0x20, "");
static_assert(offsetof(PLUGIN_ITEMINFO, nGem1) == 0x24, "");
static_assert(offsetof(PLUGIN_ITEMINFO, nReduceDmg) == 0x38, "");
static_assert(offsetof(PLUGIN_ITEMINFO, nAddLife) == 0x3C, "");
static_assert(offsetof(PLUGIN_ITEMINFO, nAntiMonster) == 0x40, "");
static_assert(offsetof(PLUGIN_ITEMINFO, usPlunder) == 0x44, "");
static_assert(offsetof(PLUGIN_ITEMINFO, usSpecialFlag) == 0x46, "");

#endif
