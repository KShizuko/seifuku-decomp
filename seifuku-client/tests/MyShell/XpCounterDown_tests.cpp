#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "XpCounterDown.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct XpCounterDownProbe : CXpCounterDown
{
    static size_t TimeBegin() { return offsetof(XpCounterDownProbe, m_dwTimeBegin); }
    static size_t OldNum() { return offsetof(XpCounterDownProbe, m_nOldNum); }
};

static_assert(sizeof(CXpCounterDown) == 0xC, "");
static_assert(std::has_virtual_destructor<CXpCounterDown>::value, "");
static_assert(std::is_member_function_pointer<decltype(static_cast<void (CXpCounterDown::*)(DWORD, int, DWORD, BOOL)>(&CXpCounterDown::Show))>::value, "");
static_assert(std::is_member_function_pointer<decltype(static_cast<void (CXpCounterDown::*)(int, CMyPos)>(&CXpCounterDown::Show))>::value, "");

TEST_CLASS(ClientXpCounterDownHeader)
{
public:
    TEST_METHOD(OldNumIsTheSecondDword)
    {
        Assert::IsTrue(XpCounterDownProbe::TimeBegin() == 0x4);
        Assert::IsTrue(XpCounterDownProbe::OldNum() == 0x8);
    }
};
