#include "stdafx.h"
#include "MyShell.h"
#include "MySlider.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMySlider) == sizeof(CButton) + 0x68, "");
static_assert(std::is_base_of<CButton, CMySlider>::value, "");
static_assert(std::is_polymorphic<CMySlider>::value, "");

struct MySliderProbe
{
    static size_t Par() { return offsetof(CMySlider, m_SliPar); }
    static size_t Frame() { return offsetof(CMySlider, m_SliPar.m_uFrame); }
    static size_t Id() { return offsetof(CMySlider, m_SliPar.m_strSliderID); }
    static size_t IntPoint() { return offsetof(CMySlider, m_SliPar.m_IntPoint); }
    static size_t Down() { return offsetof(CMySlider, m_SliPar.m_bBtnDown); }
    static size_t Width() { return offsetof(CMySlider, m_SliPar.m_wdSliderWidth); }
    static size_t MaxValue() { return offsetof(CMySlider, m_SliPar.m_wdSliderMaxValue); }
    static size_t Pos() { return offsetof(CMySlider, m_SliPar.m_nSliderPos); }
    static size_t CurPos() { return offsetof(CMySlider, m_SliPar.m_nSliderCurPos); }
    static size_t Mouse() { return offsetof(CMySlider, m_SliPar.m_MousePoint); }
    static size_t V() { return offsetof(CMySlider, m_SliPar.m_bV); }
    static size_t ParSize() { return sizeof(CMySlider::MySliderParameter); }
};

TEST_CLASS(ClientMySliderHeader)
{
public:
    TEST_METHOD(TheRecordSitsWhereTheHandlersReadIt)
    {
        const size_t b = sizeof(CButton);
        Assert::IsTrue(MySliderProbe::Par() == b);
        Assert::IsTrue(MySliderProbe::ParSize() == 0x68);
        Assert::IsTrue(MySliderProbe::Frame() == b + 0x00);
        Assert::IsTrue(MySliderProbe::Id() == b + 0x04);
        Assert::IsTrue(MySliderProbe::IntPoint() == b + 0x44);
        Assert::IsTrue(MySliderProbe::Down() == b + 0x4C);
        Assert::IsTrue(MySliderProbe::Width() == b + 0x50);
        Assert::IsTrue(MySliderProbe::MaxValue() == b + 0x52);
        Assert::IsTrue(MySliderProbe::Pos() == b + 0x54);
        Assert::IsTrue(MySliderProbe::CurPos() == b + 0x58);
        Assert::IsTrue(MySliderProbe::Mouse() == b + 0x5C);
        Assert::IsTrue(MySliderProbe::V() == b + 0x64);
    }
};
