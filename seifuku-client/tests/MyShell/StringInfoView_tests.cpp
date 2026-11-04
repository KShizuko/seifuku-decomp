#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "StringInfoView.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct StringInfoViewProbe
{
    static size_t Str() { return offsetof(CStringInfoView, m_szStr); }
    static size_t Pos() { return offsetof(CStringInfoView, m_posShow); }
    static size_t Color() { return offsetof(CStringInfoView, m_dwColor); }
};

static_assert(sizeof(CStringInfoView) == 0x110, "");
static_assert(std::is_base_of<IInfoView, CStringInfoView>::value, "");
static_assert(std::is_polymorphic<CStringInfoView>::value, "");
static_assert(std::is_same<decltype(&CStringInfoView::CreateNew), CStringInfoView* (*)(char*, CMyPos, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CStringInfoView::Create), BOOL (CStringInfoView::*)(char*, CMyPos, DWORD)>::value, "");

TEST_CLASS(ClientStringInfoViewHeader)
{
public:
    TEST_METHOD(TheStringThePositionAndTheColour)
    {
        Assert::IsTrue(StringInfoViewProbe::Str() == 0x4);
        Assert::IsTrue(StringInfoViewProbe::Pos() == 0x104);
        Assert::IsTrue(StringInfoViewProbe::Color() == 0x10C);
    }
};
