#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Trade.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct TradeProbe : CTrade
{
    static size_t HeroItems() { return offsetof(TradeProbe, m_setHeroItem); }
    static size_t PlayerItems() { return offsetof(TradeProbe, m_setPlayerItem); }
    static size_t HeroMoney() { return offsetof(TradeProbe, m_dwHeroMoney); }
    static size_t HeroEmoney() { return offsetof(TradeProbe, m_dwHeroEmoney); }
    static size_t PlayerMoney() { return offsetof(TradeProbe, m_dwPlayerMoney); }
    static size_t PlayerEmoney() { return offsetof(TradeProbe, m_dwPlayerEmoney); }
    static size_t HeroAgree() { return offsetof(TradeProbe, m_bHeroAgree); }
    static size_t PlayerAgree() { return offsetof(TradeProbe, m_bPlayerAgree); }
    static size_t Active() { return offsetof(TradeProbe, m_bActive); }
    static size_t Player() { return offsetof(TradeProbe, m_idPlayer); }
    static size_t PlayerName() { return offsetof(TradeProbe, m_szPlayerName); }
    static size_t PlayerLevel() { return offsetof(TradeProbe, m_dwPlayerLevel); }
    static size_t LastApply() { return offsetof(TradeProbe, m_idLastApplyPlayer); }
    static size_t LastApplyName() { return offsetof(TradeProbe, m_szLastApplyPlayerName); }
};

static_assert(sizeof(CTrade) == 4 + 2 * sizeof(DEQUE_ITEM) + 0x48, "");
static_assert(std::has_virtual_destructor<CTrade>::value, "");
static_assert(std::is_same<decltype(&CTrade::Open), void (CTrade::*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CTrade::GetHeroItemByIndex), CItem* (CTrade::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CTrade::SetPlayerEmoney), void (CTrade::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&CTrade::SetHeroEmoney), void (CTrade::*)(DWORD)>::value, "");

TEST_CLASS(ClientTradeHeader)
{
public:
    TEST_METHOD(MembersFollowTheTwoDequesInTheCtorOrder)
    {
        const size_t d = sizeof(DEQUE_ITEM);
        Assert::IsTrue(TradeProbe::HeroItems() == 0x4);
        Assert::IsTrue(TradeProbe::PlayerItems() == 0x4 + d);
        Assert::IsTrue(TradeProbe::HeroMoney() == 0x4 + 2 * d);
        Assert::IsTrue(TradeProbe::HeroEmoney() == 0x4 + 2 * d + 0x4);
        Assert::IsTrue(TradeProbe::PlayerMoney() == 0x4 + 2 * d + 0x8);
        Assert::IsTrue(TradeProbe::PlayerEmoney() == 0x4 + 2 * d + 0xC);
        Assert::IsTrue(TradeProbe::HeroAgree() == 0x4 + 2 * d + 0x10);
        Assert::IsTrue(TradeProbe::PlayerAgree() == 0x4 + 2 * d + 0x14);
        Assert::IsTrue(TradeProbe::Active() == 0x4 + 2 * d + 0x18);
        Assert::IsTrue(TradeProbe::Player() == 0x4 + 2 * d + 0x1C);
        Assert::IsTrue(TradeProbe::PlayerName() == 0x4 + 2 * d + 0x20);
        Assert::IsTrue(TradeProbe::PlayerLevel() == 0x4 + 2 * d + 0x30);
        Assert::IsTrue(TradeProbe::LastApply() == 0x4 + 2 * d + 0x34);
        Assert::IsTrue(TradeProbe::LastApplyName() == 0x4 + 2 * d + 0x38);
    }
};
