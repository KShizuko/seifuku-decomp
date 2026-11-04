#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Entrance.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct EntranceProbe : CEntrance
{
    static size_t TxtMsgSet() { return offsetof(EntranceProbe, m_TxtMsgSet); }
};

static_assert(sizeof(CEntrance) == 4 + sizeof(CTxtMsgSet), "");
static_assert(std::has_virtual_destructor<CEntrance>::value, "");
static_assert(std::is_same<decltype(&CEntrance::AddMsg), BOOL (CEntrance::*)(const char*)>::value, "");
static_assert(std::is_same<decltype(&CEntrance::ClearMsg), void (CEntrance::*)(void)>::value, "");
static_assert(std::is_same<decltype(&CEntrance::GetLastMsg), const char* (CEntrance::*)(void)>::value, "");

TEST_CLASS(ClientEntranceHeader)
{
public:
    TEST_METHOD(TheMessageSetFollowsTheVptr)
    {
        Assert::IsTrue(EntranceProbe::TxtMsgSet() == 0x4);
    }
};
