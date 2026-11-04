#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "BoothManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct BoothManagerProbe : CBoothManager
{
    static size_t Npc() { return offsetof(BoothManagerProbe, m_idNpc); }
    static size_t Active() { return offsetof(BoothManagerProbe, m_bActive); }
    static size_t CryOut() { return offsetof(BoothManagerProbe, m_szCryOut); }
};

static_assert(sizeof(CBoothManager) == 0x20C + sizeof(DEQUE_ITEM), "");
static_assert(offsetof(CBoothManager, m_dequeItem) == 0x20C, "");
static_assert(std::has_virtual_destructor<CBoothManager>::value, "");
static_assert(std::is_same<decltype(&CBoothManager::AddItem), void (CBoothManager::*)(OBJID, DWORD, BOOL, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CBoothManager::DelItem), void (CBoothManager::*)(OBJID, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CBoothManager::Open), BOOL (CBoothManager::*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CBoothManager::Close), void (CBoothManager::*)()>::value, "");
static_assert(std::is_same<decltype(&CBoothManager::PauseBooth), void (CBoothManager::*)()>::value, "");
static_assert(std::is_same<decltype(&CBoothManager::ContinueBooth), BOOL (CBoothManager::*)(OBJID)>::value, "");

TEST_CLASS(ClientBoothManagerHeader)
{
public:
    TEST_METHOD(PlainMembersPrecedeTheDeque)
    {
        Assert::IsTrue(BoothManagerProbe::Npc() == 0x4);
        Assert::IsTrue(BoothManagerProbe::Active() == 0x8);
        Assert::IsTrue(BoothManagerProbe::CryOut() == 0xC);
    }
};
