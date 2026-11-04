#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Check.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct CheckProbe : CCheck
{
    static size_t TimeCheck() { return offsetof(CheckProbe, m_dwTimeCheck); }
    static size_t WS() { return offsetof(CheckProbe, m_bReportWSProcess); }
    static size_t FY() { return offsetof(CheckProbe, m_bReportFYProcess); }
    static size_t Life() { return offsetof(CheckProbe, m_bReportUseLifeMedicine); }
    static size_t Mana() { return offsetof(CheckProbe, m_bReportUseManaMedicine); }
    static size_t WS2() { return offsetof(CheckProbe, m_bReportWS2Library); }
    static size_t LifeAmount() { return offsetof(CheckProbe, m_nUseLifeMedicineAmount); }
    static size_t ManaAmount() { return offsetof(CheckProbe, m_nUseManaMedicineAmount); }
    static size_t LastLife() { return offsetof(CheckProbe, m_dwLastDecLife); }
    static size_t LastMana() { return offsetof(CheckProbe, m_dwLastDecMana); }
    static size_t Data() { return offsetof(CheckProbe, m_dwData); }
};

static_assert(sizeof(CCheck) == 0x30, "");
static_assert(std::has_virtual_destructor<CCheck>::value, "");
static_assert(CCheck::_TYPE_WS2_LIBRARY == 5, "");
static_assert(std::is_same<decltype(&CCheck::Process), BOOL (CCheck::*)()>::value, "");

TEST_CLASS(ClientCheckHeader)
{
public:
    TEST_METHOD(FiveFlagsThenTheCounters)
    {
        Assert::IsTrue(CheckProbe::TimeCheck() == 0x4);
        Assert::IsTrue(CheckProbe::WS() == 0x8);
        Assert::IsTrue(CheckProbe::FY() == 0xC);
        Assert::IsTrue(CheckProbe::Life() == 0x10);
        Assert::IsTrue(CheckProbe::Mana() == 0x14);
        Assert::IsTrue(CheckProbe::WS2() == 0x18);
        Assert::IsTrue(CheckProbe::LifeAmount() == 0x1C);
        Assert::IsTrue(CheckProbe::ManaAmount() == 0x20);
        Assert::IsTrue(CheckProbe::LastLife() == 0x24);
        Assert::IsTrue(CheckProbe::LastMana() == 0x28);
        Assert::IsTrue(CheckProbe::Data() == 0x2C);
    }
};

struct CheckKeysProbe : CCheckKeys
{
    static size_t Keys() { return offsetof(CheckKeysProbe, m_dwKeys); }
};

static_assert(sizeof(CCheckKeys) == 0x54, "");
static_assert(_MAX_CHECKKEYS == 20, "");
static_assert(std::has_virtual_destructor<CCheckKeys>::value, "");
static_assert(std::is_same<decltype(&CCheckKeys::GetKey), DWORD (CCheckKeys::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CCheckKeys::SetKey), void (CCheckKeys::*)(int, DWORD)>::value, "");

TEST_CLASS(ClientCheckKeysHeader)
{
public:
    TEST_METHOD(TwentyKeysAfterTheVptr)
    {
        Assert::IsTrue(CheckKeysProbe::Keys() == 0x4);
    }
};
