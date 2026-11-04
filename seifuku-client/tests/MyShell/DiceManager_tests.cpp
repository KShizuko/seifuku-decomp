#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "DiceManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct DiceManagerProbe : DiceManager
{
    static size_t Id() { return offsetof(DiceManagerProbe, m_idDic); }
    static size_t SelfAnte() { return offsetof(DiceManagerProbe, m_dwSelfAnte); }
    static size_t TotalAnte() { return offsetof(DiceManagerProbe, m_dwTotalAnte); }
    static size_t Status() { return offsetof(DiceManagerProbe, m_nStatus); }
    static size_t Dice() { return offsetof(DiceManagerProbe, m_dwDice); }
    static size_t Players() { return offsetof(DiceManagerProbe, m_setPlayerList); }
    static size_t TimeBegin() { return offsetof(DiceManagerProbe, m_dwTimeCountBegin); }
    static size_t CountDown() { return offsetof(DiceManagerProbe, m_bCountDown); }
    static size_t ImgString() { return offsetof(DiceManagerProbe, m_objImgString); }
};

static_assert(sizeof(DicePlayer) == 0x14 && offsetof(DicePlayer, dwBonus) == 0x10, "");
static_assert(_MAX_ANTE_SORT_AMOUNT == 23 && _DICE_AMOUNT == 3 && DICE_VALUE_ALL == 22, "");
static_assert(sizeof(DiceManager) == 0xD0 + sizeof(vector<DicePlayer*>) + 8 + sizeof(CImageString), "");
static_assert(std::has_virtual_destructor<DiceManager>::value, "");
static_assert(std::is_same<decltype(&DiceManager::Open), void (DiceManager::*)(OBJID, BOOL)>::value, "");
static_assert(std::is_same<decltype(&DiceManager::ChipIn), void (DiceManager::*)(int, DWORD)>::value, "");

TEST_CLASS(ClientDiceManagerHeader)
{
public:
    TEST_METHOD(AntesDiceVectorThenTheTimer)
    {
        Assert::IsTrue(DiceManagerProbe::Id() == 0x4);
        Assert::IsTrue(DiceManagerProbe::SelfAnte() == 0x8);
        Assert::IsTrue(DiceManagerProbe::TotalAnte() == 0x64);
        Assert::IsTrue(DiceManagerProbe::Status() == 0xC0);
        Assert::IsTrue(DiceManagerProbe::Dice() == 0xC4);
        Assert::IsTrue(DiceManagerProbe::Players() == 0xD0);
        const size_t v = 0xD0 + sizeof(vector<DicePlayer*>);
        Assert::IsTrue(DiceManagerProbe::TimeBegin() == v);
        Assert::IsTrue(DiceManagerProbe::CountDown() == v + 4);
        Assert::IsTrue(DiceManagerProbe::ImgString() == v + 8);
    }
};
