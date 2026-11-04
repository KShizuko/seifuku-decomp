#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MyTimer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MyTimerProbe : CMyTimer
{
    static size_t Interval() { return offsetof(MyTimerProbe, m_dwInterval); }
    static size_t UpdateTime() { return offsetof(MyTimerProbe, m_tUpdateTime); }
};

static_assert(sizeof(CMyTimer) == 0xC, "");
static_assert(std::has_virtual_destructor<CMyTimer>::value, "");
static_assert(std::is_same<decltype(&CMyTimer::IsTimeOut), bool (CMyTimer::*)()>::value, "");
static_assert(std::is_same<decltype(&CMyTimer::ToNextTime), bool (CMyTimer::*)()>::value, "");
static_assert(std::is_same<decltype(&CMyTimer::Startup), void (CMyTimer::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMyTimer::TimeOver), bool (CMyTimer::*)()>::value, "");

TEST_CLASS(ClientMyTimerHeader)
{
public:
    TEST_METHOD(IntervalThenUpdateTime)
    {
        Assert::IsTrue(MyTimerProbe::Interval() == 0x4);
        Assert::IsTrue(MyTimerProbe::UpdateTime() == 0x8);
    }
};

TEST_CLASS(ClientMyTimerBodies)
{
public:
    TEST_METHOD(ConstructedTimerIsInactiveAndStartupActivatesIt)
    {
        CMyTimer t;
        Assert::IsFalse(t.IsActive());
        Assert::AreEqual((DWORD) 0, t.GetInterval());
        Assert::IsFalse(t.IsTimeOut());
        t.Startup(60000);
        Assert::IsTrue(t.IsActive());
        Assert::AreEqual((DWORD) 60000, t.GetInterval());
        Assert::IsFalse(t.IsTimeOut());
        t.Clear();
        Assert::IsFalse(t.IsActive());
        Assert::AreEqual((DWORD) 0, t.GetInterval());
    }
};
