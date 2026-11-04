#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "firebug.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct FireBugProbe : CFireBug
{
    static size_t Status() { return offsetof(FireBugProbe, m_iStatus); }
    static size_t Cur() { return offsetof(FireBugProbe, m_nCurWorldPoint); }
    static size_t Show() { return offsetof(FireBugProbe, m_nShowWorldPoint); }
    static size_t Layer() { return offsetof(FireBugProbe, m_nLayer); }
    static size_t Loop() { return offsetof(FireBugProbe, m_nLoopCount); }
};
struct SysFireBugProbe : CSysFireBug
{
    static size_t Status() { return offsetof(SysFireBugProbe, m_iStatu); }
    static size_t Speed() { return offsetof(SysFireBugProbe, m_nSpeed); }
    static size_t Bugs() { return offsetof(SysFireBugProbe, m_dequeFlyBug); }
};

static_assert(sizeof(CFireBug) == 0x34, "");
static_assert(!std::is_polymorphic<CFireBug>::value, "");
static_assert(sizeof(CSysFireBug) == 0x1C + sizeof(DequeCFireBug) + 0x4, "");
static_assert(_FIREBUGSTATUS_NONE == 0 && FIREBUGSTATUS_RESET == 5, "");
static_assert(std::is_same<decltype(&CSysFireBug::Create), BOOL (CSysFireBug::*)(int, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CSysFireBug::ReSet), void (CSysFireBug::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CFireBug::Create), BOOL (CFireBug::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CFireBug::Show), void (CFireBug::*)(void*)>::value, "");
static_assert(std::is_same<decltype(&CFireBug::GetY), int (CFireBug::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CSysFireBug::Pause), void (CSysFireBug::*)()>::value && std::is_same<decltype(&CSysFireBug::Continue), void (CSysFireBug::*)()>::value, "");

TEST_CLASS(ClientFireBugHeader)
{
public:
    TEST_METHOD(BugMembersInOrder)
    {
        Assert::IsTrue(FireBugProbe::Status() == 0x4);
        Assert::IsTrue(FireBugProbe::Cur() == 0x8);
        Assert::IsTrue(FireBugProbe::Show() == 0x10);
        Assert::IsTrue(FireBugProbe::Layer() == 0x18);
        Assert::IsTrue(FireBugProbe::Loop() == 0x30);
    }
    TEST_METHOD(SystemMembersInOrder)
    {
        Assert::IsTrue(SysFireBugProbe::Status() == 0x8);
        Assert::IsTrue(SysFireBugProbe::Speed() == 0x18);
        Assert::IsTrue(SysFireBugProbe::Bugs() == 0x1C);
    }
};
