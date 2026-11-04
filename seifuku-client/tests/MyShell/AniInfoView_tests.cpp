#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "AniInfoView.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct AniInfoViewProbe
{
    static size_t File() { return offsetof(CAniInfoView, m_szFile); }
    static size_t Index() { return offsetof(CAniInfoView, m_szIndex); }
    static size_t Pos() { return offsetof(CAniInfoView, m_posShow); }
    static size_t Frame() { return offsetof(CAniInfoView, m_nFrame); }
};

static_assert(sizeof(CAniInfoView) == 0x210, "");
static_assert(std::is_base_of<IInfoView, CAniInfoView>::value, "");
static_assert(std::is_polymorphic<CAniInfoView>::value, "");
static_assert(std::is_same<decltype(&CAniInfoView::CreateNew), CAniInfoView* (*)(char*, char*, CMyPos, int)>::value, "");
static_assert(std::is_same<decltype(&CAniInfoView::Create), BOOL (CAniInfoView::*)(char*, char*, CMyPos, int)>::value, "");

TEST_CLASS(ClientAniInfoViewHeader)
{
public:
    TEST_METHOD(TheTwoNamesThePositionAndTheFrame)
    {
        Assert::IsTrue(AniInfoViewProbe::File() == 0x4);
        Assert::IsTrue(AniInfoViewProbe::Index() == 0x104);
        Assert::IsTrue(AniInfoViewProbe::Pos() == 0x204);
        Assert::IsTrue(AniInfoViewProbe::Frame() == 0x20C);
    }
};
