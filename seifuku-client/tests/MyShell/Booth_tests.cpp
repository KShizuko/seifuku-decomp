#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Booth.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct BoothProbe : CBooth
{
    static size_t Npc() { return offsetof(BoothProbe, m_idNpc); }
    static size_t Items() { return offsetof(BoothProbe, m_dequeItem); }
    static size_t Name() { return offsetof(BoothProbe, m_szName); }
};

static_assert(sizeof(CBooth) == 4 + 4 + sizeof(DEQUE_ITEM) + 0x20, "");
static_assert(std::has_virtual_destructor<CBooth>::value, "");
static_assert(std::is_same<decltype(&CBooth::Open), BOOL (CBooth::*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CBooth::BuyItem), void (CBooth::*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CBooth::SetPrice), void (CBooth::*)(OBJID, DWORD)>::value, "");

TEST_CLASS(ClientBoothHeader)
{
public:
    TEST_METHOD(NpcIdThenDequeThenName)
    {
        Assert::IsTrue(BoothProbe::Npc() == 0x4);
        Assert::IsTrue(BoothProbe::Items() == 0x8);
        Assert::IsTrue(BoothProbe::Name() == 0x8 + sizeof(DEQUE_ITEM));
    }
};
