#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Player.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(offsetof(CPlayerInfo, setCmd) == 0x3A0 && sizeof(CPlayerInfo) == 0x3A0 + sizeof(deque<CCommand>), "");
static_assert(offsetof(CPlayerInfo, nMetempsychosis) == 0x358 && offsetof(CPlayerInfo, usRWeaponRestrain) == 0x39C && offsetof(CPlayerInfo, usLWeaponRestrain) == 0x39E, "");
static_assert(offsetof(CPlayerInfo, szMate) == 0 && offsetof(CPlayerInfo, dwForce) == 0x10 && offsetof(CPlayerInfo, dwSpeed) == 0x18, "");
static_assert(offsetof(CPlayerInfo, dwHealth) == 0x1C && offsetof(CPlayerInfo, dwSoul) == 0x20 && offsetof(CPlayerInfo, dwXP) == 0x24 && offsetof(CPlayerInfo, dwMaxLife) == 0x28, "");
static_assert(offsetof(CPlayerInfo, dataLife) == 0x2C && offsetof(CPlayerInfo, dataMana) == 0xF0 && offsetof(CPlayerInfo, dataCurLife) == 0x1B4 && offsetof(CPlayerInfo, dataCurMana) == 0x278, "");
static_assert(offsetof(CPlayerInfo, dwProfession) == 0x33C && offsetof(CPlayerInfo, dwPhysicalForce) == 0x340 && offsetof(CPlayerInfo, dwMaxPhysicalForce) == 0x344, "");
static_assert(offsetof(CPlayerInfo, dwLev) == 0x348 && offsetof(CPlayerInfo, dwAddPoint) == 0x34C && offsetof(CPlayerInfo, nExp) == 0x364 && offsetof(CPlayerInfo, nPk) == 0x368, "");
static_assert(offsetof(CPlayerInfo, idSyndicate) == 0x36C && offsetof(CPlayerInfo, dwSyndicateRank) == 0x370 && offsetof(CPlayerInfo, nSyndicateProffer) == 0x374, "");
static_assert(offsetof(CPlayerInfo, szSyndicateLeader) == 0x378 && offsetof(CPlayerInfo, dwSyndicateFund) == 0x388 && offsetof(CPlayerInfo, dwSyndicatePopulation) == 0x38C, "");
static_assert(offsetof(CPlayerInfo, dwKoAmount) == 0x390 && offsetof(CPlayerInfo, dwVirtue) == 0x394 && offsetof(CPlayerInfo, bShowIcon) == 0x398, "");

static_assert(RANKAMOUNT == 7, "");

namespace
{
    struct PlayerProbe : CPlayer
    {
        static size_t CmdList() { return offsetof(PlayerProbe, m_setCmdList); }
        static size_t Info() { return offsetof(PlayerProbe, m_infoPlayer); }
        static size_t CryOut() { return offsetof(PlayerProbe, m_szCryOut); }
        static size_t KoScale() { return offsetof(PlayerProbe, m_dwKoScale); }
        static size_t ImgNumber() { return offsetof(PlayerProbe, m_objImgNumber); }
    };
}

static_assert(std::is_same<decltype(((CPlayer*) 0)->SetData(0, 0)), void>::value && std::is_same<decltype(((CPlayer*) 0)->GetData(0)), DWORD>::value, "");
static_assert(std::is_same<decltype(((CPlayer*) 0)->ReplaceStatus(0)), void>::value && std::is_same<decltype(((CPlayer*) 0)->ProcessStatusEffect(0)), void>::value, "");
static_assert(std::is_same<decltype(((CPlayer*) 0)->ReplaceStatus2(0)), void>::value, "");
static_assert(CPlayer::_PLAYER_LIFE == 1 && CPlayer::_PLAYER_CURRENTLIFE == 15 && CPlayer::_PLAYER_MANA == 9 && CPlayer::_PLAYER_CURRENTMANA == 5, "");
static_assert(USERSTATUS2_CURSE == 1 && USERSTATUS2_BLESS == 2, "");

TEST_CLASS(ClientPlayerHeader)
{
public:
    TEST_METHOD(SyndicateRankTableIsConquers)
    {
        static const DWORD ranks[RANKAMOUNT] = { 100, 90, 50, 0, 80, 70, 60 };
        static const OBJID titles[RANKAMOUNT] = { 100079, 100080, 100081, 100021, 100120, 100122, 100121 };
        for (int i = 0; i < RANKAMOUNT; i++)
        {
            Assert::IsTrue(SYNDICATERANKINFOSET[i].dwRank == ranks[i]);
            Assert::IsTrue(SYNDICATERANKINFOSET[i].idTitle == titles[i]);
        }
    }

    TEST_METHOD(MemberOrderMatchesTheConstructor)
    {
        Assert::IsTrue(PlayerProbe::CmdList() == sizeof(CRole));
        Assert::IsTrue(PlayerProbe::Info() == PlayerProbe::CmdList() + sizeof(std::list<TimerCmdInfo>));
        Assert::IsTrue(PlayerProbe::CryOut() == PlayerProbe::Info() + sizeof(CPlayerInfo));
        Assert::IsTrue(PlayerProbe::KoScale() == PlayerProbe::CryOut() + 0x100);
        Assert::IsTrue(PlayerProbe::ImgNumber() == PlayerProbe::KoScale() + 4);
        Assert::IsTrue(sizeof(CPlayer) == PlayerProbe::ImgNumber() + sizeof(CImageString) + sizeof(CFDword*) + sizeof(DWORD));
        Assert::IsTrue(0x864 + 0xC == 0x870 && 0x870 + 0x3D0 == 0xC40 && 0xC40 + 0x100 == 0xD40 && 0xD40 + 4 == 0xD44 && 0xD44 + 0x70 == 0xDB4 && 0xDB4 + 4 == 0xDB8 && 0xDB8 + 4 == 0xDBC);
    }
};
