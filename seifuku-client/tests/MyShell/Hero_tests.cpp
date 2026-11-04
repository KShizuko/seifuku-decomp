#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Hero.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct HeroProbe : CHero
{
#define HERO_OFF(m) static size_t m##_off() { return offsetof(HeroProbe, m); } static size_t m##_size() { return sizeof(((HeroProbe*)0)->m); }
    HERO_OFF(m_idAccount) HERO_OFF(m_dwLastTalk) HERO_OFF(m_posAlign) HERO_OFF(m_dwMoney) HERO_OFF(m_dwEmoney) HERO_OFF(m_dwAutoAllot)
    HERO_OFF(m_dwSuperSoldierTicks) HERO_OFF(m_bFirstAtk) HERO_OFF(m_objXpCounterDown) HERO_OFF(m_dwXpStartTime)
    HERO_OFF(m_tmXpAdd) HERO_OFF(m_dwSuperSoldierBegin) HERO_OFF(m_dwSuperSoldierBeginCopy) HERO_OFF(m_dwSuperSoldierTail) HERO_OFF(m_dwKoCount)
    HERO_OFF(m_dwKoScale) HERO_OFF(m_timeDie) HERO_OFF(m_timeGhost) HERO_OFF(m_dwTimeSynWarReportLastFlash)
    HERO_OFF(m_dequeSynWarReport) HERO_OFF(m_dequeItem) HERO_OFF(m_pEquipment) HERO_OFF(m_dwNobility)
    HERO_OFF(m_setFriend) HERO_OFF(m_idFriendQuery) HERO_OFF(m_dwFriendBeginQuery)
    HERO_OFF(m_bFriendQueryBusy) HERO_OFF(m_bFriendQueryReturn) HERO_OFF(m_idFriendLastApply) HERO_OFF(m_dwReserved4294) HERO_OFF(m_setEnemy) HERO_OFF(m_idEnemyQuery)
    HERO_OFF(m_dwEnemyBeginQuery) HERO_OFF(m_bEnemyQueryBusy) HERO_OFF(m_bEnemyQueryReturn) HERO_OFF(m_tmNameAlert)
    HERO_OFF(m_nPkMode) HERO_OFF(m_objProfessional) HERO_OFF(m_objTeam) HERO_OFF(m_dwLastQueryTeamLeader)
    HERO_OFF(m_dwTeamData) HERO_OFF(m_objTrade) HERO_OFF(m_objPackage) HERO_OFF(m_objBank) HERO_OFF(m_objTaskDialog)
    HERO_OFF(m_setSingleHandWeaponSkill) HERO_OFF(m_setDoubleHandWeaponSkill) HERO_OFF(m_setOtherWeaponSkill)
    HERO_OFF(m_setLevelExp) HERO_OFF(m_setMagic) HERO_OFF(m_dwMagicAttackBegin) HERO_OFF(m_dwMagicAttackBusy)
    HERO_OFF(m_bMagicAttackBusy) HERO_OFF(m_nHeightCanJump) HERO_OFF(m_dwBeginUseTransformMagicTime) HERO_OFF(m_bMagicLock)
    HERO_OFF(m_dwFlyBegin) HERO_OFF(m_dwFlyDuration) HERO_OFF(m_dwTransformBegin) HERO_OFF(m_dwTransformDuration)
    HERO_OFF(m_bShowAbortTransformDialog) HERO_OFF(m_dwMagicShieldBegin) HERO_OFF(m_dwMagicShieldDuration)
    HERO_OFF(m_dwLastBump) HERO_OFF(m_idLastSyndicateApply) HERO_OFF(m_idLastSyndicate) HERO_OFF(m_szSyndicateAnnounce)
    HERO_OFF(m_nSynListIndex) HERO_OFF(m_setSyndicateMember) HERO_OFF(m_objSynMemberInfo) HERO_OFF(m_objDummy)
    HERO_OFF(m_objPlayerDummy) HERO_OFF(m_objBooth) HERO_OFF(m_objBoothManager) HERO_OFF(m_objBulletin)
    HERO_OFF(m_bMousePlayerFixed) HERO_OFF(n_bMousePlayerTerrain) HERO_OFF(m_szWanted) HERO_OFF(m_szPoliceWanted)
    HERO_OFF(m_objDiceManager) HERO_OFF(m_bChargeUp) HERO_OFF(m_dwTimeChargeUp) HERO_OFF(m_dwChargeUpTotalTime)
    HERO_OFF(m_bShowArrow) HERO_OFF(m_objCheck) HERO_OFF(m_objCheckKeys) HERO_OFF(m_idWhoAttackMe) HERO_OFF(m_idMyTarget)
    HERO_OFF(m_idSpecialTarget) HERO_OFF(m_dwTimeSpecialTarget) HERO_OFF(m_dwTimeWhoAttackMe) HERO_OFF(m_dwTimeMyTarget)
    HERO_OFF(m_setPetInfo) HERO_OFF(m_tmTeamMemberPos) HERO_OFF(m_dwTeamMemberPosKey) HERO_OFF(m_tmPing) HERO_OFF(m_dwPing)
    HERO_OFF(m_idDialogPara) HERO_OFF(m_pKillCounter) HERO_OFF(m_pKoCounter) HERO_OFF(m_nKillCount) HERO_OFF(m_bTaskKillCount) HERO_OFF(m_nKillCountPending) HERO_OFF(m_nTaskKillCount) HERO_OFF(m_dwLuckyTime) HERO_OFF(m_dwLuckyTimeStamp) HERO_OFF(m_objLuckyEffect)
    HERO_OFF(m_dwBlessBegin) HERO_OFF(m_dwBlessSeconds) HERO_OFF(m_objBlessEffect) HERO_OFF(m_dwCurseBegin) HERO_OFF(m_dwCurseSeconds) HERO_OFF(m_objCurseEffect)
    HERO_OFF(m_dwDblExpBegin) HERO_OFF(m_dwDblExpSeconds) HERO_OFF(m_objDblExpEffect) HERO_OFF(m_dwDayBonus) HERO_OFF(m_mapTable) HERO_OFF(m_dwTableParam) HERO_OFF(m_bTableRequested) HERO_OFF(m_idExercise)
#undef HERO_OFF
};

static_assert(sizeof(SynMemberInfo) == 0x38, "");
static_assert(offsetof(SynMemberInfo, szProffer) == 0x8 && offsetof(SynMemberInfo, szName) == 0x18 && offsetof(SynMemberInfo, szRank) == 0x28, "");
static_assert(_LEVELEXP_AMOUNT == 129, "");
static_assert(sizeof(LevelExpInfo) == 8 && offsetof(LevelExpInfo, dwExpUnit) == 4, "");
static_assert(sizeof(TableRecord) == 0xC && offsetof(TableRecord, fValue) == 4 && offsetof(TableRecord, nData) == 8, "");
static_assert(CHero::XPFULL_TIME == 20000, "");
static_assert(CHero::KILL_AWARD_SPSOLDIER == 800 && CHero::KILL_AWARD_TORNADO == 1000, "");
static_assert(std::has_virtual_destructor<CHero>::value, "");
static_assert(std::is_same<decltype(&CHero::SetCommand), DWORD (CHero::*)(CCommand*)>::value, "");
static_assert(std::is_same<decltype(&CHero::ProcessCommand), BOOL (CHero::*)()>::value, "");
static_assert(std::is_same<decltype(&CHero::ReplaceStatus), void (CHero::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&CHero::GetAtkRange), int (CHero::*)()>::value, "");
static_assert(std::is_same<decltype(&CHero::DeleteRole), void (CHero::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&CHero::ReBorn), BOOL (CHero::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&CHero::AllotPoint), void (CHero::*)(int, int, int, int)>::value, "");
static_assert(std::is_same<decltype(&CHero::ResetXpFullTime), void (CHero::*)()>::value, "");
static_assert(std::is_same<decltype(&CHero::FuseItem), void (CHero::*)(OBJID, FUSE_INFO)>::value, "");
static_assert(std::is_same<decltype(&CHero::SendDataArray), void (CHero::*)(OBJID, OBJID, OBJID, OBJID, OBJID)>::value, "");
static_assert(std::is_same<decltype(&CHero::UseItem), void (CHero::*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CHero::GetItemByIndex), CItem* (CHero::*)(UINT)>::value, "");
static_assert(std::is_same<decltype(&CHero::CreateMousePlayer), void (CHero::*)(int, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CHero::SetTeamMemberPosTimer), void (CHero::*)(DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CHero::GetLevelForecast), void (CHero::*)(int&, int&)>::value, "");
static_assert(std::is_same<decltype(&CHero::FindTableRecord), TableRecord* (CHero::*)(int, int, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CHero::AddTableRecord), void (CHero::*)(int, int, int, int, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CHero::ActTable), void (CHero::*)(DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CHero::SetBlessTime), void (CHero::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&CHero::Shoot), void (CHero::*)(CPlayer*, OBJID, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CHero::Attack), void (CHero::*)(CPlayer*, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CHero::RushAttack), void (CHero::*)(CPlayer*, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CHero::SkillAttack), void (CHero::*)(CPlayer*, OBJID, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CHero::Poison), void (CHero::*)(CPlayer*, OBJID)>::value, "");
static_assert(std::is_same<decltype(&CHero::Dart), void (CHero::*)(CPlayer*, OBJID)>::value, "");
static_assert(std::is_same<decltype(&CHero::Steal), void (CHero::*)(CPlayer*)>::value, "");
static_assert(std::is_same<decltype(&CHero::GetMagicDef), DWORD (CHero::*)()>::value, "");
static_assert(std::is_same<decltype(&CHero::JoinSynMember), void (CHero::*)(CPlayer*)>::value, "");
static_assert(std::is_same<decltype(&CHero::AcceptSynMember), void (CHero::*)(CPlayer*)>::value, "");
static_assert(std::is_same<decltype(&CHero::TitleSynMember), void (CHero::*)(CPlayer*, const char*)>::value, "");
static_assert(std::is_same<decltype(static_cast<void (CHero::*)(CPlayer*)>(&CHero::KickoutSynMember)), void (CHero::*)(CPlayer*)>::value, "");
static_assert(std::is_same<decltype(&CHero::QuitSyn), void (CHero::*)()>::value, "");
static_assert(std::is_same<decltype(&CHero::GetBowShootRange), DWORD (CHero::*)()>::value, "");
static_assert(std::is_same<decltype(&CHero::SetSpecialTarget), void (CHero::*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CHero::ProcessSpecialTarget), void (CHero::*)()>::value, "");

TEST_CLASS(ClientHeroHeader)
{
public:
    TEST_METHOD(MembersFollowEachOtherInTheCtorOrder)
    {
#define NEXT(a, b) Assert::IsTrue(HeroProbe::b##_off() == HeroProbe::a##_off() + HeroProbe::a##_size())
        Assert::IsTrue(HeroProbe::m_pKoCounter_off() == sizeof(CPlayer));
        NEXT(m_pKoCounter, m_pKillCounter);
        NEXT(m_pKillCounter, m_nKillCount);
        NEXT(m_nKillCount, m_bTaskKillCount);
        NEXT(m_bTaskKillCount, m_nKillCountPending);
        NEXT(m_nKillCountPending, m_nTaskKillCount);
        NEXT(m_nTaskKillCount, m_idAccount);
        NEXT(m_idAccount, m_dwLastTalk);
        NEXT(m_dwLastTalk, m_dwMoney);
        NEXT(m_dwMoney, m_dwEmoney);
        NEXT(m_dwEmoney, m_dwAutoAllot);
        NEXT(m_dwAutoAllot, m_dwSuperSoldierTicks);
        NEXT(m_dwSuperSoldierTicks, m_posAlign);
        NEXT(m_posAlign, m_bFirstAtk);
        NEXT(m_bFirstAtk, m_objXpCounterDown);
        NEXT(m_objXpCounterDown, m_dwXpStartTime);
        NEXT(m_dwXpStartTime, m_tmXpAdd);
        NEXT(m_tmXpAdd, m_dwSuperSoldierBegin);
        NEXT(m_dwSuperSoldierBegin, m_dwSuperSoldierBeginCopy);
        NEXT(m_dwSuperSoldierBeginCopy, m_dwSuperSoldierTail);
        NEXT(m_dwSuperSoldierTail, m_dwKoCount);
        NEXT(m_dwKoCount, m_dwKoScale);
        NEXT(m_dwKoScale, m_timeDie);
        NEXT(m_timeDie, m_timeGhost);
        NEXT(m_timeGhost, m_dwTimeSynWarReportLastFlash);
        NEXT(m_dwTimeSynWarReportLastFlash, m_dequeSynWarReport);
        NEXT(m_dequeSynWarReport, m_dequeItem);
        NEXT(m_dequeItem, m_pEquipment);
        NEXT(m_pEquipment, m_dwNobility);
        NEXT(m_dwNobility, m_setFriend);
        NEXT(m_setFriend, m_idFriendQuery);
        NEXT(m_idFriendQuery, m_dwFriendBeginQuery);
        NEXT(m_dwFriendBeginQuery, m_bFriendQueryBusy);
        NEXT(m_bFriendQueryBusy, m_bFriendQueryReturn);
        NEXT(m_bFriendQueryReturn, m_idFriendLastApply);
        NEXT(m_idFriendLastApply, m_dwReserved4294);
        NEXT(m_dwReserved4294, m_setEnemy);
        NEXT(m_setEnemy, m_idEnemyQuery);
        NEXT(m_idEnemyQuery, m_dwEnemyBeginQuery);
        NEXT(m_dwEnemyBeginQuery, m_bEnemyQueryBusy);
        NEXT(m_bEnemyQueryBusy, m_bEnemyQueryReturn);
        NEXT(m_bEnemyQueryReturn, m_tmNameAlert);
        NEXT(m_tmNameAlert, m_nPkMode);
        NEXT(m_nPkMode, m_objProfessional);
        NEXT(m_objProfessional, m_objTeam);
        NEXT(m_objTeam, m_dwLastQueryTeamLeader);
        NEXT(m_dwLastQueryTeamLeader, m_dwTeamData);
        NEXT(m_dwTeamData, m_objTrade);
        NEXT(m_objTrade, m_objPackage);
        NEXT(m_objPackage, m_objBank);
        NEXT(m_objBank, m_objTaskDialog);
        NEXT(m_objTaskDialog, m_setSingleHandWeaponSkill);
        NEXT(m_setSingleHandWeaponSkill, m_setDoubleHandWeaponSkill);
        NEXT(m_setDoubleHandWeaponSkill, m_setOtherWeaponSkill);
        NEXT(m_setOtherWeaponSkill, m_setLevelExp);
        NEXT(m_setLevelExp, m_setMagic);
        NEXT(m_setMagic, m_dwMagicAttackBegin);
        NEXT(m_dwMagicAttackBegin, m_dwMagicAttackBusy);
        NEXT(m_dwMagicAttackBusy, m_bMagicAttackBusy);
        NEXT(m_bMagicAttackBusy, m_nHeightCanJump);
        NEXT(m_nHeightCanJump, m_dwBeginUseTransformMagicTime);
        NEXT(m_dwBeginUseTransformMagicTime, m_bMagicLock);
        Assert::IsTrue(HeroProbe::m_dwFlyBegin_off() == HeroProbe::m_bMagicLock_off() + 4);
        NEXT(m_dwFlyBegin, m_dwFlyDuration);
        NEXT(m_dwFlyDuration, m_dwTransformBegin);
        NEXT(m_dwTransformBegin, m_dwTransformDuration);
        NEXT(m_dwTransformDuration, m_bShowAbortTransformDialog);
        NEXT(m_bShowAbortTransformDialog, m_dwMagicShieldBegin);
        NEXT(m_dwMagicShieldBegin, m_dwMagicShieldDuration);
        NEXT(m_dwMagicShieldDuration, m_dwLastBump);
        NEXT(m_dwLastBump, m_idLastSyndicateApply);
        NEXT(m_idLastSyndicateApply, m_idLastSyndicate);
        NEXT(m_idLastSyndicate, m_szSyndicateAnnounce);
        NEXT(m_szSyndicateAnnounce, m_nSynListIndex);
        NEXT(m_nSynListIndex, m_setSyndicateMember);
        NEXT(m_setSyndicateMember, m_objSynMemberInfo);
        NEXT(m_objSynMemberInfo, m_objDummy);
        NEXT(m_objDummy, m_objPlayerDummy);
        NEXT(m_objPlayerDummy, m_objBooth);
        NEXT(m_objBooth, m_objBoothManager);
        NEXT(m_objBoothManager, m_objBulletin);
        NEXT(m_objBulletin, m_bMousePlayerFixed);
        NEXT(m_bMousePlayerFixed, n_bMousePlayerTerrain);
        NEXT(n_bMousePlayerTerrain, m_szWanted);
        NEXT(m_szWanted, m_szPoliceWanted);
        NEXT(m_szPoliceWanted, m_objDiceManager);
        NEXT(m_objDiceManager, m_bChargeUp);
        NEXT(m_bChargeUp, m_dwTimeChargeUp);
        NEXT(m_dwTimeChargeUp, m_dwChargeUpTotalTime);
        NEXT(m_dwChargeUpTotalTime, m_bShowArrow);
        NEXT(m_bShowArrow, m_objCheck);
        NEXT(m_objCheck, m_objCheckKeys);
        NEXT(m_objCheckKeys, m_idWhoAttackMe);
        NEXT(m_idWhoAttackMe, m_idMyTarget);
        NEXT(m_idMyTarget, m_idSpecialTarget);
        NEXT(m_idSpecialTarget, m_dwTimeSpecialTarget);
        NEXT(m_dwTimeSpecialTarget, m_dwTimeWhoAttackMe);
        NEXT(m_dwTimeWhoAttackMe, m_dwTimeMyTarget);
        NEXT(m_dwTimeMyTarget, m_setPetInfo);
        NEXT(m_setPetInfo, m_tmTeamMemberPos);
        NEXT(m_tmTeamMemberPos, m_dwTeamMemberPosKey);
        NEXT(m_dwTeamMemberPosKey, m_tmPing);
        NEXT(m_tmPing, m_dwPing);
        NEXT(m_dwPing, m_idDialogPara);
        NEXT(m_idDialogPara, m_dwLuckyTime);
        NEXT(m_dwLuckyTime, m_dwLuckyTimeStamp);
        NEXT(m_dwLuckyTimeStamp, m_objLuckyEffect);
        NEXT(m_objLuckyEffect, m_dwBlessBegin);
        NEXT(m_dwBlessBegin, m_dwBlessSeconds);
        NEXT(m_dwBlessSeconds, m_objBlessEffect);
        NEXT(m_objBlessEffect, m_dwCurseBegin);
        NEXT(m_dwCurseBegin, m_dwCurseSeconds);
        NEXT(m_dwCurseSeconds, m_objCurseEffect);
        NEXT(m_objCurseEffect, m_dwDblExpBegin);
        NEXT(m_dwDblExpBegin, m_dwDblExpSeconds);
        NEXT(m_dwDblExpSeconds, m_objDblExpEffect);
        NEXT(m_objDblExpEffect, m_dwDayBonus);
        NEXT(m_dwDayBonus, m_mapTable);
        NEXT(m_mapTable, m_dwTableParam);
        NEXT(m_dwTableParam, m_bTableRequested);
        NEXT(m_bTableRequested, m_idExercise);
        Assert::IsTrue(sizeof(CHero) == HeroProbe::m_idExercise_off() + sizeof(DWORD));
        Assert::IsTrue(HeroProbe::m_setLevelExp_size() == 0x408);
        Assert::IsTrue(HeroProbe::m_dwTableParam_size() == 12);
        Assert::IsTrue(Statuary1038[0][0] == 140 && Statuary1038[6][1] == 155);
#undef NEXT
    }
};
