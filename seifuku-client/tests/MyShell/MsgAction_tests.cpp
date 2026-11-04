#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgAction.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgActionProbe : CMsgAction
{
    typedef CMsgAction::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgActionProbe, m_pInfo); }
};

static_assert(sizeof(CMsgAction) == 0x408, "");
static_assert(sizeof(MsgActionProbe::Info) == 0x18, "");
static_assert(offsetof(MsgActionProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgActionProbe::Info, dwTimeStamp) == 0x4, "");
static_assert(offsetof(MsgActionProbe::Info, idUser) == 0x8, "");
static_assert(offsetof(MsgActionProbe::Info, dwData) == 0xC, "");
static_assert(offsetof(MsgActionProbe::Info, unPosX) == 0x10, "");
static_assert(offsetof(MsgActionProbe::Info, unPosY) == 0x12, "");
static_assert(offsetof(MsgActionProbe::Info, unDir) == 0x14, "");
static_assert(offsetof(MsgActionProbe::Info, usAction) == 0x16, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgAction::*)(OBJID, int, int, USHORT, USHORT, DWORD)>(&CMsgAction::Create)), BOOL (CMsgAction::*)(OBJID, int, int, USHORT, USHORT, DWORD)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgAction::*)(OBJID, int, int, USHORT, USHORT, USHORT, USHORT, DWORD)>(&CMsgAction::Create)), BOOL (CMsgAction::*)(OBJID, int, int, USHORT, USHORT, USHORT, USHORT, DWORD)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgAction::*)(DWORD, OBJID, DWORD, USHORT)>(&CMsgAction::Create)), BOOL (CMsgAction::*)(DWORD, OBJID, DWORD, USHORT)>::value, "");
static_assert(actionEnterMap == 74 && actionGetItemSet == 75 && actionGetGoodFriend == 76 && actionGetWeaponSkillSet == 77 && actionGetMagicSet == 78 && actionGetSynAttr == 97 && actionGetStatistic == 130, "");
static_assert(actionChgDir == 79 && actionEmotion == 81 && actionChgMap == 85 && actionXpCLear == 93 && actionReborn == 94 && actionDelRole == 95 && actionSetPkMode == 96, "");
static_assert(actionMine == 99 && actionTeamMemeberPos == 100 && actionLanding == 120 && actionRequest == 127 && actionJump == 133 && actionGhost == 137 && actionChangeFace == 142, "");
static_assert(actionFlyMap == 86 && actionUplev == 92 && actionTeamLeaderPos == 101 && actionAbortMagic == 103 && actionMapARGB == 104 && actionQueryTeamMember == 106, "");
static_assert(actionKickBack == 108 && actionDropMagic == 109 && actionDropSkill == 110 && actionCreateBooth == 111 && actionResumeBooth == 113 && actionDestroyBooth == 114, "");
static_assert(actionPostCmd == 116 && actionQueryEquipment == 117 && actionAbortTransform == 118 && actionSetCheckKey == 119 && actionGetMoney == 121 && actionDischarge == 122 && actionQueryEnemyInfo == 123 && actionSoundEffect == 124, "");
static_assert(actionOpenDialog == 126 && actionCheckMotion == 128 && actionAlignPos == 129 && actionDisappear == 131 && actionLeaveMap == 132 && actionEquip == 135 && actionUnequip == 136, "");
static_assert(actionQueryPlayer == 102 && actionSynchro == 138 && actionReportSuperState == 139 && actionQueryFriendInfo == 140 && actionQueryLeaveWord == 141, "");
static_assert(actionTableRequest == 230 && actionTableAct == 231 && actionTableClear == 232 && actionTaskKillCount == 1000 && actionTaskKillCountAdd == 1001, "");

TEST_CLASS(ClientMsgActionHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgActionProbe::InfoPtr() == 0x404);
    }
};
