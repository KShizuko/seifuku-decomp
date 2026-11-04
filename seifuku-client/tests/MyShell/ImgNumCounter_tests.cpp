#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "ImgNumCounter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct ImgNumCounterProbe : CImgNumCounter
{
#define INC_OFF(m) static size_t m##_off() { return offsetof(ImgNumCounterProbe, m); }
    INC_OFF(m_nNumber) INC_OFF(m_nCurrent) INC_OFF(m_nStep) INC_OFF(m_dwBeginTime) INC_OFF(m_dwInterval)
    INC_OFF(m_dwColor) INC_OFF(m_posShow) INC_OFF(m_dwZoomPercent) INC_OFF(m_setNumImg)
#undef INC_OFF
};

static_assert(sizeof(NumImgInfo) == 8, "");
static_assert(sizeof(CImgNumCounter) == 0x28 + sizeof(VEC_NUMIMG), "");
static_assert(std::is_same<decltype(((CImgNumCounter*) 0)->Start(0)), void>::value, "");
static_assert(std::is_same<decltype(((CImgNumCounter*) 0)->Show()), BOOL>::value, "");
static_assert(std::is_same<decltype(((CImgNumCounter*) 0)->End()), void>::value, "");

TEST_CLASS(ClientImgNumCounterHeader)
{
public:
    TEST_METHOD(CounterMembersSitWhereTheConstructorWritesThem)
    {
        Assert::IsTrue(ImgNumCounterProbe::m_nNumber_off() == 0x4);
        Assert::IsTrue(ImgNumCounterProbe::m_nCurrent_off() == 0x8);
        Assert::IsTrue(ImgNumCounterProbe::m_nStep_off() == 0xC);
        Assert::IsTrue(ImgNumCounterProbe::m_dwBeginTime_off() == 0x10);
        Assert::IsTrue(ImgNumCounterProbe::m_dwInterval_off() == 0x14);
        Assert::IsTrue(ImgNumCounterProbe::m_dwColor_off() == 0x18);
        Assert::IsTrue(ImgNumCounterProbe::m_posShow_off() == 0x1C);
        Assert::IsTrue(ImgNumCounterProbe::m_dwZoomPercent_off() == 0x24);
        Assert::IsTrue(ImgNumCounterProbe::m_setNumImg_off() == 0x28);
    }
};
